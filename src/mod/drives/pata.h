#ifndef PATA_DRIVER_H
#define PATA_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "../../lib/utils/asm_utils.h"
#include "../../lib/utils/ata_utils.h"
#include "../../lib/time/time.h"
#include "../../lib/utils/handle_utils.h"

// ATA Ports
#define ATA_DATA_PORT 0x1F0
#define ATA_ERROR_PORT 0x1F1
#define ATA_SECTOR_COUNT_PORT 0x1F2
#define ATA_LBA_LOW_PORT 0x1F3
#define ATA_LBA_MID_PORT 0x1F4
#define ATA_LBA_HIGH_PORT 0x1F5
#define ATA_DRIVE_PORT 0x1F6
#define ATA_COMMAND_PORT 0x1F7
#define ATA_STATUS_PORT 0x1F7

// ATA Commands
#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_WRITE_SECTORS 0x30
#define ATA_CMD_IDENTITY 0xEC

extern uint16_t drive_info_buffer[256];

extern bool is_lba48_supported;
extern uint32_t lba28_sector_count;
extern uint64_t lba48_sector_count;

int identify_ata_drive(uint16_t drive_port);

#endif
