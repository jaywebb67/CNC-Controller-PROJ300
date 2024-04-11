#include "flash.hpp"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include <cstdint>
#include "mbed.h"




/* DEFINE the SECTORS according to your reference manual
 * STM32F446RE have:-
 *  Sector 0 to Sector 3 each 16KB
 *  Sector 4 as 64KB
 *  Sector 5 to Sector 7 each 128KB
 */

static uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;

    if((Address < 0x08003FFF) && (Address >= 0x08000000))
    {
        sector = FLASH_SECTOR_0;
    }
    else if((Address < 0x08007FFF) && (Address >= 0x08004000))
    {
        sector = FLASH_SECTOR_1;
    }
    else if((Address < 0x0800BFFF) && (Address >= 0x08008000))
    {
        sector = FLASH_SECTOR_2;
    }
    else if((Address < 0x0800FFFF) && (Address >= 0x0800C000))
    {
        sector = FLASH_SECTOR_3;
    }
    else if((Address < 0x0801FFFF) && (Address >= 0x08010000))
    {
        sector = FLASH_SECTOR_4;
    }
    else if((Address < 0x0803FFFF) && (Address >= 0x08020000))
    {
        sector = FLASH_SECTOR_5;
    }
    else if((Address < 0x0805FFFF) && (Address >= 0x08040000))
    {
        sector = FLASH_SECTOR_6;
    }
    else if((Address < 0x0807FFFF) && (Address >= 0x08060000))
    {
        sector = FLASH_SECTOR_7;
    }
    else if((Address < 0x0809FFFF) && (Address >= 0x08080000))
    {
        sector = FLASH_SECTOR_8;
    }
    else if((Address < 0x080BFFFF) && (Address >= 0x080A0000))
    {
        sector = FLASH_SECTOR_9;
    }
    else if((Address < 0x080DFFFF) && (Address >= 0x080C0000))
    {
        sector = FLASH_SECTOR_10;
    }
    else if((Address < 0x080FFFFF) && (Address >= 0x080E0000))
    {
        sector = FLASH_SECTOR_11;
    }
    else if((Address < 0x08103FFF) && (Address >= 0x08100000))
    {
        sector = FLASH_SECTOR_12;
    }
    else if((Address < 0x08107FFF) && (Address >= 0x08104000))
    {
        sector = FLASH_SECTOR_13;
    }
    else if((Address < 0x0810BFFF) && (Address >= 0x08108000))
    {
        sector = FLASH_SECTOR_14;
    }
    else if((Address < 0x0810FFFF) && (Address >= 0x0810C000))
    {
        sector = FLASH_SECTOR_15;
    }
    else if((Address < 0x0811FFFF) && (Address >= 0x08110000))
    {
        sector = FLASH_SECTOR_16;
    }
    else if((Address < 0x0813FFFF) && (Address >= 0x08120000))
    {
        sector = FLASH_SECTOR_17;
    }
    else if((Address < 0x0815FFFF) && (Address >= 0x08140000))
    {
        sector = FLASH_SECTOR_18;
    }
    else if((Address < 0x0817FFFF) && (Address >= 0x08160000))
    {
        sector = FLASH_SECTOR_19;
    }
    else if((Address < 0x0819FFFF) && (Address >= 0x08180000))
    {
        sector = FLASH_SECTOR_20;
    }
    else if((Address < 0x081BFFFF) && (Address >= 0x081A0000))
    {
        sector = FLASH_SECTOR_21;
    }
    else if((Address < 0x081DFFFF) && (Address >= 0x081C0000))
    {
        sector = FLASH_SECTOR_22;
    }
    else if ((Address < 0x081FFFFF) && (Address >= 0x081E0000))
    {
        sector = FLASH_SECTOR_23;
    }
    return sector;
}










void float2Bytes(uint8_t * ftoa_bytes_temp,float float_variable)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    thing.a = float_variable;

    for (uint8_t i = 0; i < 4; i++) {
      ftoa_bytes_temp[i] = thing.bytes[i];
    }

}

float Bytes2float(uint8_t * ftoa_bytes_temp)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    for (uint8_t i = 0; i < 4; i++) {
    	thing.bytes[i] = ftoa_bytes_temp[i];
    }

   float float_variable =  thing.a;
   return float_variable;
}


uint32_t Flash_Write_Data (uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords)
{

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError;
	int sofar=0;

    /* Allocate memory for a copy of the sector */
    uint32_t *sectorCopy = (uint32_t *) malloc(numberofwords * sizeof(uint32_t));
    if (sectorCopy == NULL) {
        /* Memory allocation failed */
        return HAL_FLASH_GetError();
    }

    /* Read the contents of the sector and store them in the copy */
    memcpy(sectorCopy, (void *)StartSectorAddress, numberofwords * sizeof(uint32_t));


    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Erase the user Flash area */

    /* Get the number of sector to erase from 1st sector */

    uint32_t StartSector = GetSector(StartSectorAddress);
    uint32_t EndSectorAddress = StartSectorAddress + numberofwords*4;
    uint32_t EndSector = GetSector(EndSectorAddress);

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = StartSector;
    EraseInitStruct.NbSectors     = (EndSector - StartSector) + 1;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
        you have to make sure that these data are rewritten before they are accessed during code
        execution. If this cannot be done safely, it is recommended to flush the caches by setting the
        DCRST and ICRST bits in the FLASH_CR register. */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
    {
        free(sectorCopy);
        return HAL_FLASH_GetError ();
    }

    /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
    uint32_t calculatedChecksum = 0;
    while (sofar<numberofwords)
    {
        if (sectorCopy[sofar] != Data[sofar]) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, Data[sofar]) == HAL_OK)
            {
                calculatedChecksum += Data[sofar]; // Update checksum
                StartSectorAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
                sofar++;
            }
            else
            {
            /* Error occurred while writing data in Flash memory*/
                free(sectorCopy);
                return HAL_FLASH_GetError ();
            }
        }
    }

    /* Program the calculated checksum at the end of the data */
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, calculatedChecksum) != HAL_OK) {
        free(sectorCopy);
        return HAL_FLASH_GetError();
    }

    /* Lock the Flash to disable the flash control register access (recommended
        to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    free(sectorCopy);
    
    return 0;
}


uint32_t Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
    uint32_t calculatedChecksum = 0;
    uint32_t storedChecksum;


    while (numberofwords > 0) {
        *RxBuf = *(__IO uint32_t *)StartSectorAddress;
        if (*RxBuf == 0xFFFFFFFF) {
            // If the read value is the flash's erased state, it indicates an unsuccessful read
            return 0;
        }
        StartSectorAddress += 4;
        calculatedChecksum += *RxBuf; // Update checksum
        RxBuf++;
        numberofwords--;
    }
    // Read the stored checksum from Flash
    storedChecksum = *(__IO uint32_t *)StartSectorAddress;

    // Verify checksum
    if (calculatedChecksum == storedChecksum) {
        return 1; // Checksum verification succeeded
    } else {
        return 0; // Checksum verification failed
    }

}

void Convert_To_Str (uint32_t *Data, char *Buf)
{
	int numberofbytes = ((strlen((char *)Data)/4) + ((strlen((char *)Data) % 4) != 0)) *4;

	for (int i=0; i<numberofbytes; i++)
	{
		Buf[i] = Data[i/4]>>(8*(i%4));
	}
}


void Flash_Write_NUM (uint32_t StartSectorAddress, float* Num, uint16_t noOfFloats)
{
    // Allocate memory for a buffer to hold serialized float values
    uint8_t *buffer = (uint8_t *)malloc(noOfFloats * sizeof(float));
    if (buffer == NULL) {
        // Memory allocation failed
        return;
    }

    // Serialize float values into the buffer
    for (int i = 0; i < noOfFloats; i++) {
        memcpy(&buffer[i * sizeof(float)], &Num[i], sizeof(float));
    }

    // Write the serialized data to flash memory
    Flash_Write_Data(StartSectorAddress, (uint32_t *)buffer, (noOfFloats * sizeof(float) + 3) / 4);

    // Free the allocated buffer
    free(buffer);
}


uint8_t Flash_Read_NUM (uint32_t StartSectorAddress, uint16_t noOfFloats,float* destination)
{
	uint8_t buffer[4*noOfFloats];

	if(!Flash_Read_Data(StartSectorAddress, (uint32_t *)buffer, noOfFloats)){
        return 0;
    }
    // Convert byte array to float values and store them in the destination array
    for (uint16_t i = 0; i < noOfFloats; i++) {
        // Extract 4 bytes for each float value and convert to float
        uint32_t temp = *((uint32_t*)&buffer[i * 4]);
        destination[i] = *((float*)&temp);
    }
	return 1;

}