#include "pata.h"
#include "../../lib/file_system/file_system.h"
#include "../../lib/std/stdio.h"

uint8_t sector_buffer[512];

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
    
    time = 0;
    while((inb(ATA_STATUS_PORT) & 0b00001000) != 0b00001000 || (inb(ATA_STATUS_PORT) & 0b10000000) == 0b10000000)
    {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 1;
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

    time = 0;
    while((inb(ATA_STATUS_PORT) & 0b00001000) != 0b00001000 || (inb(ATA_STATUS_PORT) & 0b10000000) == 0b10000000)
    {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 1;
        }
    }

    // write data to the data port
    for(int i = 0; i < 512; i += 2)
    {
        outw(ATA_DATA_PORT, ((uint16_t)(sector_buffer[i] << 8) | (uint16_t)sector_buffer[i + 1]));
    }

    return 0;
}
