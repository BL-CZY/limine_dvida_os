#include "./crc32.h"
#include "mod/mmu/mem_utils.h"

static uint32_t i_table[256]; // CRC lookup table array.

void initialize_crc32(void) {

	int iPos,iCodes;
	//0x04C11DB7 is the official polynomial used by PKZip, WinZip and Ethernet.
	unsigned int iPolynomial = 0x04C11DB7;

	memset(&i_table, 0, sizeof(i_table));

	// 256 values representing ASCII character codes.
	for(iCodes = 0; iCodes <= 0xFF; iCodes++)
	{
		i_table[iCodes] = reflect(iCodes, 8) << 24;

		for(iPos = 0; iPos < 8; iPos++)
		{
			i_table[iCodes] = (i_table[iCodes] << 1)
				^ ((i_table[iCodes] & (1 << 31)) ? iPolynomial : 0);
		}

		i_table[iCodes] = reflect(i_table[iCodes], 32);
	}
}

uint32_t reflect(uint32_t iReflect, char cChar) {

	int iPos;
	uint32_t iValue = 0;

	// Swap bit 0 for bit 7, bit 1 For bit 6, etc....
	for(iPos = 1; iPos < (cChar + 1); iPos++)
	{
		if(iReflect & 1)
		{
			iValue |= (1 << (cChar - iPos));
		}
		iReflect >>= 1;
	}

	return iValue;
}

void partial_crc(uint32_t *iCRC, uint8_t *sData, size_t iDataLength) {

	while(iDataLength--)
	{
		//If your compiler complains about the following line, try changing
		//	each occurrence of *iCRC with ((unsigned int)*iCRC).

		*iCRC = (*iCRC >> 8) ^ i_table[(*iCRC & 0xFF) ^ *sData++];
	}
}

unsigned long full_crc(uint8_t *sData, uint64_t ulDataLength) {
    uint32_t ulCRC = 0xffffffff;
    partial_crc((unsigned int *)&ulCRC, sData, ulDataLength);
    return(ulCRC ^ 0xffffffff);
}