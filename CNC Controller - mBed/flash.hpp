#ifndef _FLASH_H_
#define _FLASH_H_

#include "stdint.h"
#include <cstdint>


uint32_t Flash_Write_Data (uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords);

uint32_t Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords);

void Convert_To_Str (uint32_t *Data, char *Buf);

void Flash_Write_NUM (uint32_t StartSectorAddress, float* Num, uint16_t noOfFloats);

uint8_t Flash_Read_NUM (uint32_t StartSectorAddress,uint16_t noOfFloats, float* destination);



#endif /* INC_FLASH_SECTOR_F4_H_ */