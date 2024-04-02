#include "fs_driver.h"
#include "../../lib/file_system/file_system.h"
#include "../../lib/std/stdio.h"

uint8_t sector_buffer[512];

int wait_for_disk() {
    uint32_t time = 0;
    // Poll for completion 
    while ((inb(ATA_STATUS_PORT) & 0x80) != 0x80)
    {
        ++time;
        if(time > 100000)
        {
            //error timeout
            return 1;
        }
    }
    //no error
    return 0;
}

int read_sector(uint64_t lba) {
    sleep(1);
    //no error
    outb(ATA_ERROR_PORT, 0);
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
    if(wait_for_disk())
    {
        return 1; //failed
    }

    sleep(1);

    return 0;

    // Read data from the data port
    for(int i = 0; i < 512; i += 2)
    {
        uint16_t temp = inw(ATA_DATA_PORT);
        sector_buffer[i] = (uint8_t)temp;
        sector_buffer[i + 1] = (uint8_t)(temp >> 8);
    }

    return 0;
}