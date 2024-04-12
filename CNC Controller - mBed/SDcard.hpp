#ifndef _SDCARD_HPP_
#define _SDCARD_HPP_


#include "mbed.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "Gcode.hpp"

#define SDclk PC_10
#define SD_miso PC_11
#define SD_mosi PC_12
#define SD_CD PF_4
#define SD_CS PF_3


extern FILE file;
extern FATFileSystem fs;
// Instantiate the SDBlockDevice by specifying the SPI pins connected to the SDCard
//               MOSI, MISO, SCK,   CS,
extern SDBlockDevice sdCard;


void sdCard_Init();
void sdCardMode();
void readFile(const char *filename);
void listFiles();




#endif