#include "pata.h"
#include "../../lib/file_system/file_system.h"
#include "../../lib/std/stdio.h"

uint8_t sector_buffer[512];
uint16_t drive_info_buffer[256];

bool is_lba48_supported;
uint32_t lba28_sector_count;
uint64_t lba48_sector_count;

/**
 * function error codes:
 *  0: the ata drive is present, and the information is in the drive_info_buffer
 *  1: no ata drive
 *  2: the drive is not ata
 *  3: error
*/
int identify_ata_drive(uint16_t drive_port) {
    // send 0xA0 to the drive port to start the process
    outb(drive_port, 0xA0);

    // clear all these ports
    outb(ATA_SECTOR_COUNT_PORT, 0);
    outb(ATA_LBA_LOW_PORT, 0);
    outb(ATA_LBA_MID_PORT, 0);
    outb(ATA_LBA_HIGH_PORT, 0);

    // identify command
    outb(ATA_COMMAND_PORT, 0xEC);
    
    // 400ns delay
    for(int i = 0; i < 14; ++i) {
        inb(ATA_STATUS_PORT);
    }

    // read the status port
    while(true) {
        if(inb(ATA_STATUS_PORT) == 0) {
            // drive doesn't exist
            return 1;
        }

        if(inb(ATA_LBA_MID_PORT) != 0 || inb(ATA_LBA_HIGH_PORT) != 0) {
            // drive is not ata
            return 2;
        }

        if((inb(ATA_STATUS_PORT) & 0b00000001) == 0b00000001) {
            // error
            return 3;
        }

        // if the 3th bit is set, break out of the loop
        if((inb(ATA_STATUS_PORT) & 0b00001000) == 0b00001000) {
            break;
        }
    }

    // read in the data
    for(int i = 0; i < 256; ++i) {
        drive_info_buffer[i] = inw(ATA_DATA_PORT);
    }

    if((drive_info_buffer[83] & 0b0000010000000000) == 0b0000010000000000) {
        is_lba48_supported = true;
    }

    lba28_sector_count = ((uint32_t)(drive_info_buffer[60]) | (uint32_t)(drive_info_buffer[61]) << 16) - 1;
    lba48_sector_count = ((uint64_t)(drive_info_buffer[100]) | ((uint64_t)(drive_info_buffer[101])) << 16 | ((uint64_t)(drive_info_buffer[102])) << 32 | ((uint64_t)(drive_info_buffer[103])) << 48) - 1;

    // the log part

    kprintf("ATA drive identified on port %x\n", drive_port);

    if(is_lba48_supported) {
        kprintf("lba48 is supported by the drive\n");
    } else {
        kprintf("lba48 is not supported by the drive\n");
    }

    kprintf("addressable sectors for lba28 count: %x\n", lba28_sector_count);

    if(is_lba48_supported) {
        kprintf("addressable sectors for lba48 count: %x\n", lba48_sector_count);
    }

    return 0;
}

/**
 * these functions return 1 if it's not ready
 * return 2 if timeout
 * return 3 if failed to flush the cache
*/

int pio_read_sector(uint64_t lba) {
    int time = 0;
    while((inb(ATA_STATUS_PORT) & 0b10000000) == 0b10000000) {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 1;
        }
    }
    // Select drive (Assuming drive 0, replace with appropriate value if needed)
    // set 111 to enter LBA mode
    outb(ATA_DRIVE_PORT, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    
    // Set sector count to 1
    outb(ATA_SECTOR_COUNT_PORT, 1);

    // Set LBA address
    outb(ATA_LBA_LOW_PORT, (uint8_t)(lba & 0xFF));
    outb(ATA_LBA_MID_PORT, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_LBA_HIGH_PORT,(uint8_t)((lba >> 16) & 0xFF));

    // Issue the read command
    outb(ATA_COMMAND_PORT, ATA_CMD_READ_SECTORS);

    // 400ns delay
    for(int i = 0; i < 14; ++i) {
        inb(ATA_STATUS_PORT);
    }
    
    time = 0;
    while((inb(ATA_STATUS_PORT) & 0b00001000) != 0b00001000 || (inb(ATA_STATUS_PORT) & 0b10000000) == 0b10000000)
    {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 2;
        }
    }

    // Read data from the data port
    for(int i = 0; i < 512; i += 2)
    {
        uint16_t temp = inw(ATA_DATA_PORT);
        sector_buffer[i] = (uint8_t)temp;
        sector_buffer[i + 1] = (uint8_t)(temp >> 8);
    }

    return 0;
}

int pio_write_sector(uint64_t lba) {
    int time = 0;
    while((inb(ATA_STATUS_PORT) & 0b10000000) == 0b10000000) {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 1;
        }
    }
    // Select drive (Assuming drive 0, replace with appropriate value if needed)
    // set 111 to enter LBA mode
    outb(ATA_DRIVE_PORT, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    
    // Set sector count to 1
    outb(ATA_SECTOR_COUNT_PORT, 1);

    // Set LBA address
    outb(ATA_LBA_LOW_PORT, (uint8_t)(lba & 0xFF));
    outb(ATA_LBA_MID_PORT, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_LBA_HIGH_PORT,(uint8_t)((lba >> 16) & 0xFF));

    // Issue the read command
    outb(ATA_COMMAND_PORT, ATA_CMD_WRITE_SECTORS);

    // 400ns delay
    for(int i = 0; i < 14; ++i) {
        inb(ATA_STATUS_PORT);
    }

    time = 0;
    while((inb(ATA_STATUS_PORT) & 0b00001000) != 0b00001000 || (inb(ATA_STATUS_PORT) & 0b10000000) == 0b10000000)
    {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 2;
        }
    }

    // write data to the data port
    for(int i = 0; i < 512; i += 2)
    {
        outw(ATA_DATA_PORT, ((uint16_t)(sector_buffer[i] << 8) | (uint16_t)sector_buffer[i + 1]));
    }

    // flush the cache
    outb(ATA_COMMAND_PORT, 0xE7);

    time = 0;
    while((inb(ATA_STATUS_PORT) & 0b10000000) == 0b10000000) {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 3;
        }
    }

    return 0;
}

void ata_drive_init() {
    int temp = identify_ata_drive(ATA_DRIVE_PORT);
    if(temp != 0) {
        switch(temp)
        {
            case 1:
                kpanic("ATA drive init failed as there is no drive present");
                break;

            case 2:
                kpanic("ATA drive init failed as the drive is not ATA");
                break;

            case 3:
                kpanic("ATA drive init failed as there was an error with the drive");
                break;
        }
    }
}