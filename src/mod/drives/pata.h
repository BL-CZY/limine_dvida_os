#ifndef PATA_DRIVER_H
#define PATA_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "../../lib/utils/asm_utils.h"
#include "../../lib/utils/ata_utils.h"
#include "../../lib/time/time.h"
#include "../../lib/utils/handle_utils.h"
#include "../../lib/utils/ata_utils.h"

// ATA Ports
#define ATA_DATA_PORT_OFFSET 0
#define ATA_ERROR_PORT_OFFSET 1
#define ATA_SECTOR_COUNT_PORT_OFFSET 2
#define ATA_LBA_LOW_PORT_OFFSET 3
#define ATA_LBA_MID_PORT_OFFSET 4
#define ATA_LBA_HIGH_PORT_OFFSET 5
#define ATA_DRIVE_PORT_OFFSET 6
#define ATA_COMMAND_PORT_OFFSET 7
#define ATA_STATUS_PORT_OFFSET 7

// ATA Commands
#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_READ_SECTORS_EXT 0x24
#define ATA_CMD_WRITE_SECTORS 0x30
#define ATA_CMD_WRITE_SECTORS_EXT 0x34
#define ATA_CMD_START_IDENTIFY 0xA0
#define ATA_CMD_IDENTITY 0xEC
#define ATA_CMD_LBA28 0xE0
#define ATA_CMD_LBA48 0x40

int identify_ata_drive(ata_drive_t *drive);

#endif
