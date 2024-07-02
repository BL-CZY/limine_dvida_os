#include <stdint.h>

#include "./ata_pio.h"

#include "mod/builtin_shell/stdio.h"

storage_device_t ata_primary_drive = {
    .identified = false,
    .base_port = 0x1F0,
};
storage_device_t ata_secondary_drive = {
    .identified = false,
    .base_port = 0x170,
};

/**
 * function error codes:
 *  0: the ata drive is present, and the information is in the drive_info_buffer
 *  1: no ata drive
 *  2: the drive is not ata
 *  3: error
*/
int identify_ata_drive(storage_device_t *drive) {
    // send 0xA0 to the drive port to start the process
    outb(drive->base_port + ATA_DRIVE_PORT_OFFSET, ATA_CMD_START_IDENTIFY);

    // clear all these ports
    outb(drive->base_port + ATA_SECTOR_COUNT_PORT_OFFSET, 0);
    outb(drive->base_port + ATA_LBA_LOW_PORT_OFFSET, 0);
    outb(drive->base_port + ATA_LBA_MID_PORT_OFFSET, 0);
    outb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET, 0);

    // identify command
    outb(drive->base_port + ATA_COMMAND_PORT_OFFSET, ATA_CMD_IDENTITY);
    
    // 400ns delay
    for(int i = 0; i < 14; ++i) {
        inb(drive->base_port + ATA_STATUS_PORT_OFFSET);
    }

    // read the status port
    while(true) {
        if(inb(drive->base_port + ATA_STATUS_PORT_OFFSET) == 0) {
            // drive doesn't exist
            return 1;
        }

        if(inb(drive->base_port + ATA_LBA_MID_PORT_OFFSET) != 0 || inb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET) != 0) {
            // drive is not ata
            return 2;
        }

        if((inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b00000001) == 0b00000001) {
            // error
            return 3;
        }

        // if the 3th bit is set, break out of the loop
        if((inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b00001000) == 0b00001000) {
            break;
        }
    }

    drive->identified = true;

    // identified as PATA PIO for now
    drive->drive_device_type = PATA;
    drive->dma_available = false;

    uint16_t drive_info_buffer[256];

    // read in the data
    for(int i = 0; i < 256; ++i) {
        drive_info_buffer[i] = inw(drive->base_port + ATA_DATA_PORT_OFFSET);
    }

    if((drive_info_buffer[83] & 0b0000010000000000) == 0b0000010000000000) {
        drive->is_lba48_supported = true;
    }

    drive->lba28_sector_count = ((uint32_t)(drive_info_buffer[60]) | (uint32_t)(drive_info_buffer[61]) << 16);
    drive->lba48_sector_count = ((uint64_t)(drive_info_buffer[100]) | ((uint64_t)(drive_info_buffer[101])) << 16 | ((uint64_t)(drive_info_buffer[102])) << 32 | ((uint64_t)(drive_info_buffer[103])) << 48);

    // set serial number

    for(int i = 0; i < 10; ++i) {
        drive->serial[i * 2] = (uint8_t)(drive_info_buffer[34 + i] >> 8);
        drive->serial[(i * 2) + 1] = (uint8_t)(drive_info_buffer[34 + i]);
    }

    // set sector count per track
    drive->num_sectors_per_track = drive_info_buffer[21];

    // identify gpt header

    uint8_t buffer[512];
    pio_read_sectors(drive, 1, 1, buffer);

    if(!is_gpt_present(buffer)) {
        create_gpt(drive);
    } else {
        kprintf("GPT Partition Table identified\n");
        read_gpt(drive, &drive->device_gpt_header, &drive->device_gpt_table);
    }

    // the log part

    kprintf("ATA drive identified on port %x\n", drive->base_port);
    kprintf("drive serial is: ");
    for(int i = 0; i < 20; ++i) {
        kprintf("%x ", drive->serial[i]);
    }
    kprintf("\n");

    if(drive->is_lba48_supported) {
        kprintf("lba48 is supported by the drive\n");
    } else {
        kprintf("lba48 is not supported by the drive\n");
    }

    kprintf("addressable sectors for lba28 count: %x\n", drive->lba28_sector_count);

    if(drive->is_lba48_supported) {
        kprintf("addressable sectors for lba48 count: %x\n", drive->lba48_sector_count);
    }

    kprintf("device sector num per track: %u", drive->num_sectors_per_track);

    return 0;
}

/**
 * these functions 
 * return 1 if drive is not identified
 * return 2 if it's not ready
 * return 3 if timeout
 * return 4 if failed to flush the cache
 * return 5 if the sector doesn't exist
*/

int pio_read_sectors(storage_device_t *drive, int64_t sector_index, uint16_t sector_count, uint8_t *result) {
    if(!drive->identified) {
        return 1;
    }

    uint64_t lba = 0;

    //* if lba48 available use lba48
    if(sector_index < 0) {
        // if this is true, index from the end of the drive

        //* use the lba48 sector count if possible
        if(drive->is_lba48_supported) {
            lba = drive->lba48_sector_count - sector_index - 1;
        } else {
            lba = drive->lba28_sector_count - sector_index - 1;
        }
    } else {
        lba = sector_index;
    }

    // check if the sectors are out of bound
    if(drive->is_lba48_supported) {
        if((lba + sector_count) > drive->lba48_sector_count) {
            return 5;
        }
    } else {
        if((lba + sector_count) > drive->lba28_sector_count) {
            return 5;
        }
    }

    int time = 0;
    while((inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b10000000) == 0b10000000) {
        ++time;
        if(time > 100000)
        {
            // error timeout when waiting for reset
            return 2;
        }
    }

    if(drive->is_lba48_supported) {
        // LBA48
        outb(drive->base_port + ATA_DRIVE_PORT_OFFSET, ATA_CMD_LBA48);

        // output the higher 8 bytes of sector count
        outb(drive->base_port + ATA_SECTOR_COUNT_PORT_OFFSET, (uint8_t)(sector_count >> 8));

        // output the 4, 5, and 6 byte of lba
        outb(drive->base_port + ATA_LBA_LOW_PORT_OFFSET, (uint8_t)((lba >> 24) & 0xFF));
        outb(drive->base_port + ATA_LBA_MID_PORT_OFFSET, (uint8_t)((lba >> 32) & 0xFF));
        outb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET, (uint8_t)((lba >> 40) & 0xFF));

        //output the lower 8 bytes of sector count
        outb(drive->base_port + ATA_SECTOR_COUNT_PORT_OFFSET, (uint8_t)(sector_count & 0xFF));

        // output the 1, 2, and 3 byte of lba
        outb(drive->base_port + ATA_LBA_LOW_PORT_OFFSET, (uint8_t)((lba) & 0xFF));
        outb(drive->base_port + ATA_LBA_MID_PORT_OFFSET, (uint8_t)((lba >> 8) & 0xFF));
        outb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET, (uint8_t)((lba >> 16) & 0xFF));
        
        // output the read ext command
        outb(drive->base_port + ATA_COMMAND_PORT_OFFSET, ATA_CMD_READ_SECTORS_EXT);
    } else {
        // LBA28
        outb(drive->base_port + ATA_DRIVE_PORT_OFFSET, (uint8_t)(ATA_CMD_LBA28 | ((lba >> 24) & 0x0F)));
        
        // Set sector count
        outb(drive->base_port + ATA_SECTOR_COUNT_PORT_OFFSET, sector_count);

        // Set LBA address
        outb(drive->base_port + ATA_LBA_LOW_PORT_OFFSET, (uint8_t)(lba & 0xFF));
        outb(drive->base_port + ATA_LBA_MID_PORT_OFFSET, (uint8_t)((lba >> 8) & 0xFF));
        outb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET,(uint8_t)((lba >> 16) & 0xFF));

        // output the read command
        outb(drive->base_port + ATA_COMMAND_PORT_OFFSET, ATA_CMD_READ_SECTORS);
    }

    for(int i = 0; i < sector_count; ++i) {
        // 400ns delay
        for(int j = 0; j < 14; ++j) {
            inb(drive->base_port + ATA_STATUS_PORT_OFFSET);
        }
        
        time = 0;
        while((inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b00001000) != 0b00001000 || (inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b10000000) == 0b10000000)
        {
            ++time;
            if(time > 100000)
            {
                // error timeout
                return 3;
            }
        }

        // Read data from the data port
        for(int j = 0; j < 512; j += 2)
        {
            uint16_t temp = inw(drive->base_port + ATA_DATA_PORT_OFFSET);
            result[(i * 512) + j] = (uint8_t)temp;
            result[(i * 512) + j + 1] = (uint8_t)(temp >> 8);
        }
    }

    return 0;
}

int pio_write_sectors(storage_device_t *drive, int64_t sector_index, uint16_t sector_count, uint8_t *input) {
    if(!drive->identified) {
        // error unidentified drive
        return 1;
    }

    uint64_t lba = 0;

    //* if lba48 available use lba48
    if(sector_index < 0) {
        // if this is true, index from the end of the drive

        //* use the lba48 sector count if possible
        if(drive->is_lba48_supported) {
            lba = drive->lba48_sector_count + sector_index;
        } else {
            lba = drive->lba28_sector_count + sector_index;
        }
    } else {
        lba = sector_index;
    }
    
    // check if the sectors are out of bound
    if(drive->is_lba48_supported) {
        if((lba + sector_count) > drive->lba48_sector_count) {
            return 5;
        }
    } else {
        if((lba + sector_count) > drive->lba28_sector_count) {
            return 5;
        }
    }

    int time = 0;
    while((inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b10000000) == 0b10000000) {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 2;
        }
    }

    if(drive->lba48_sector_count) {
        // LBA48
        outb(drive->base_port + ATA_DRIVE_PORT_OFFSET, ATA_CMD_LBA48);

        // output the higher 8 bytes of sector count
        outb(drive->base_port + ATA_SECTOR_COUNT_PORT_OFFSET, (uint8_t)(sector_count >> 8));

        // output the 4, 5, and 6 byte of lba
        outb(drive->base_port + ATA_LBA_LOW_PORT_OFFSET, (uint8_t)((lba >> 24) & 0xFF));
        outb(drive->base_port + ATA_LBA_MID_PORT_OFFSET, (uint8_t)((lba >> 32) & 0xFF));
        outb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET, (uint8_t)((lba >> 40) & 0xFF));

        //output the lower 8 bytes of sector count
        outb(drive->base_port + ATA_SECTOR_COUNT_PORT_OFFSET, (uint8_t)(sector_count & 0xFF));

        // output the 1, 2, and 3 byte of lba
        outb(drive->base_port + ATA_LBA_LOW_PORT_OFFSET, (uint8_t)((lba) & 0xFF));
        outb(drive->base_port + ATA_LBA_MID_PORT_OFFSET, (uint8_t)((lba >> 8) & 0xFF));
        outb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET, (uint8_t)((lba >> 16) & 0xFF));
        
        // output the write ext command
        outb(drive->base_port + ATA_COMMAND_PORT_OFFSET, ATA_CMD_WRITE_SECTORS_EXT);
    } else {
        // Select drive (Assuming drive 0, replace with appropriate value if needed)
        // set 111 to enter LBA mode
        outb(drive->base_port + ATA_DRIVE_PORT_OFFSET, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
        
        // Set sector count to 1
        outb(drive->base_port + ATA_SECTOR_COUNT_PORT_OFFSET, sector_count);

        // Set LBA address
        outb(drive->base_port + ATA_LBA_LOW_PORT_OFFSET, (uint8_t)(lba & 0xFF));
        outb(drive->base_port + ATA_LBA_MID_PORT_OFFSET, (uint8_t)((lba >> 8) & 0xFF));
        outb(drive->base_port + ATA_LBA_HIGH_PORT_OFFSET,(uint8_t)((lba >> 16) & 0xFF));

        // Issue the read command
        outb(drive->base_port + ATA_COMMAND_PORT_OFFSET, ATA_CMD_WRITE_SECTORS);
    }

    for(int i = 0; i < sector_count; ++i) {
        // 400ns delay
        for(int j = 0; j < 14; ++j) {
            inb(drive->base_port + ATA_STATUS_PORT_OFFSET);
        }

        time = 0;
        // if the DRQ bit is not set or the BSY bit is set, keep polling
        while((inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b00001000) != 0b00001000 || (inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b10000000) == 0b10000000)
        {
            ++time;
            if(time > 100000)
            {
                //error timeout
                return 3;
            }
        }

        // write data to the data port
        for(int j = 0; j < 512; j += 2)
        {
            outw(drive->base_port + ATA_DATA_PORT_OFFSET, ((uint16_t)(input[(i * 512) + j]) | (uint16_t)input[(i * 512) + j + 1] << 8));
        }

        // flush the cache
        outb(drive->base_port + ATA_COMMAND_PORT_OFFSET, 0xE7);
    }

    time = 0;
    while((inb(drive->base_port + ATA_STATUS_PORT_OFFSET) & 0b10000000) == 0b10000000) {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 4;
        }
    }

    return 0;
}

void ata_drive_init() {
    int exception = identify_ata_drive(&ata_primary_drive);
    if(exception != 0) {
        kprintf("no ata drive identified at port %x, exception code %u\n", ata_primary_drive.base_port, exception);
    }

    kprintf("\n");

    exception = identify_ata_drive(&ata_secondary_drive);
    if(exception != 0) {
        kprintf("no ata drive identified at port %x, exception code %u\n", ata_secondary_drive.base_port, exception);
    }
}
