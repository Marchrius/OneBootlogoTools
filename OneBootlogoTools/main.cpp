//
//  main.c
//  OnePlusOneBootConverter
//
//  Created by Matteo Gaggiano on 23/09/14.
//  Copyright (c) 2014 Matteo Gaggiano. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define AUTHOR "Matteo Gaggiano"
#define VERSION "1.0b2"

#if DEBUG
#define DLOG(fmt, ...) printf("Debug: " fmt "\n", ##__VA_ARGS__);
#else
#define DLOG(fmt, ...) do{} while(0)
#endif

#ifdef __LINUX__
#define uint8_t unsigned char
#endif

void usage();

typedef unsigned short bool;
#define FALSE 0
#define TRUE  1

unsigned long FILE_LEN = 9821696;
unsigned long LOGO_LEN = 6220800;
unsigned long FAST_LEN = 315000;

int main(int argc, const char * argv[]) {
    
    long int logosize = 0;
    
    long int fastsize = 0;
    
	long unsigned int i = 0;
    
    FILE *logoF = NULL;
    FILE *fastF = NULL;
    FILE *fileF = NULL;
    
    bool flashBoot = FALSE;
    bool flashFast = FALSE;
    
	char option = 0;

	while ((option = getopt(argc, argv,"bf")) != -1) {
        switch (option) {
       	     case 'b' : flashBoot = TRUE;
       	         break;
       	     case 'f' : flashFast = TRUE;
       	         break;
			default:
				usage(); 
       	         exit(EXIT_FAILURE);
       	}
	}

	if ( !flashBoot && !flashFast ) {
		usage();
		exit(EXIT_FAILURE);
	}

    uint8_t *logoBin = malloc(sizeof(uint8_t) * LOGO_LEN);
    
    uint8_t *fastBin = malloc(sizeof(uint8_t) * FAST_LEN);
    
    uint8_t *fileBin = malloc(sizeof(uint8_t) * FILE_LEN);
    
    logoF = fopen("bootlogo.raw", "r");
    if (!logoF && flashBoot) {
        perror("Error while read bootlogo.raw");
        return errno;
    }
    
	flashBoot = TRUE;
    
    fastF = fopen("fastboot.raw", "r");
    if (!fastF && flashFast) {
        perror("Error while read fastboot.raw");
        return errno;
    }
    
    fileF = fopen("logo.bin", "r");
    if (!fileF) {
        perror("Error while read logo.bin");
        return errno;
    }
    
    if (flashBoot) {
        fseek(logoF, 0L, SEEK_END);
        logosize = ftell(logoF);
        
        fseek(logoF, 0L, SEEK_SET);
    }
    
    if (flashFast) {
        fseek(fastF, 0L, SEEK_END);
        fastsize = ftell(fastF);
        
        fseek(fastF, 0L, SEEK_SET);
    }
    
    for ( i = 0; i < FILE_LEN; i++) {
        fileBin[i] = fgetc(fileF);
    }
    
    fclose(fileF);

    for ( i = 0; i < LOGO_LEN && flashBoot; i++) {
        logoBin[i] = fgetc(logoF);
    }
	
	fclose(logoF);
    
    for ( i = 0; i < FAST_LEN && flashFast; i++) {
        fastBin[i] = fgetc(fastF);
    }

	fclose(fastF);
    
    for ( i = 0; i < FILE_LEN; i++)
    {
        if (i >= 512 && i < 6221312 && flashBoot)
        {
            fileBin[i] = logoBin[i - 512]; // bootlogo image
        } else
        
        if (i >= 7234560 && i < 7549560 && flashFast)
        {
            fileBin[i] = fastBin[i - 7234560]; // fastboot logo image
        }
    }
    
    fileF = fopen("logo-modified.bin", "w");
    if (!fileF) {
        perror("Error writing file logo-modified.bin");
        return errno;
    }
    
    for ( i = 0; i < FILE_LEN; i++) {
        fwrite(&fileBin[i], sizeof(uint8_t), 1, fileF);
    }
    
    printf("All ok. logo-modified.bin created correctly! \n");
    
    free(fileBin);
    free(logoBin);
    free(fastBin);
    
    return 0;
}

void usage() {
    printf("OneBootlogoTools:\n\t-b bootlogo.raw \t Overwrite the bootlogo\n\t-f fastboot.raw \t Overwrite the fastboot image\n\nVersion: " VERSION " by " AUTHOR "\n");
}
