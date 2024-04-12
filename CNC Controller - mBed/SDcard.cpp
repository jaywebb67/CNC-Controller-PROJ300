#include "SDcard.hpp"
#include <cstdio>


SDBlockDevice sdCard(PB_5, PB_4, PB_3, PF_3);

void sdCard_Init(){
    //checks if card detect pin is triggered
    if(!(GPIOF->IDR & (1U<<4))){
        if ( 0 != sdCard.init()){
            printf("Init failed \n");
            system_reset();
        }
    }
    else{
        printf("SD card not inserted! Returning to menu\n\r");
        return;
    }
}

void sdCardMode(){
    char file[MAX_CHARACTER_PER_LINE];
    printf("Displaying SD card file directory:\r\n");
    listFiles();
    printf("Enter file name: ");
    scanf(" %[^\n]",file);
    readFile(file);
}


void listFiles() {
    DIR *dir = opendir("/sd/"); // Open root directory
    struct dirent *de;

    if (dir != NULL) {
        while ((de = readdir(dir)) != NULL) {
            printf("%s\n\r", de->d_name); // Print file name
        }
        closedir(dir); // Close directory
    } else {
        printf("Error opening directory\n");
    }
}



void readFile(const char *filename) {
    FILE *file = fopen(filename, "r"); // Open file in read mode

    if (file != NULL) {
        char buffer[MAX_CHARACTER_PER_LINE];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s\n\r",buffer);
            parse_gcode(buffer); // Process each line
        }
        fclose(file); // Close file
        printf("File Completed! Returning to menu...\n\r");
    } else {
        printf("Error opening file\n");
    }
}