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

#if DEBUG
#define DLOG(fmt, ...) printf("Debug: " fmt "\n", ##__VA_ARGS__);
#else
#define DLOG(fmt, ...) do{} while(0)
#endif

unsigned long FILE_LEN = 9821696;
unsigned long LOGO_LEN = 6220800;
unsigned long FAST_LEN = 315000;

uint8_t headerLogo[] = {
    0x53, 0x50, 0x4C, 0x41, 0x53, 0x48, 0x21, 0x21,
    0x38, 0x04, 0x00, 0x00, 0x7F, 0x07, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x6E, 0x00,
    0x00, 0xE2, 0x5E, 0x00, 0x00, 0x20, 0x93, 0x00,
    0x00, 0x52, 0x94, 0x00, 0x00, 0x5E, 0x60, 0x00,
    0x00, 0x68, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t headerFast[] = {
    0x53, 0x50, 0x4C, 0x41, 0x53, 0x48, 0x21, 0x21,
    0x5E, 0x01, 0x00, 0x00, 0x39, 0x01, 0x00, 0x00 };

int main(int argc, const char * argv[]) {
    
    long int logosize = 0;
    
    long int fastsize = 0;
    
    FILE *logoF = NULL;
    FILE *fastF = NULL;
    FILE *fileF = NULL;
    
    uint8_t *logoBin = malloc(sizeof(uint8_t) * LOGO_LEN);
    
    uint8_t *fastBin = malloc(sizeof(uint8_t) * FAST_LEN);
    
    uint8_t *fileBin = malloc(sizeof(uint8_t) * FILE_LEN);
    
    logoF = fopen("bootlogo.raw", "r");
    if (!logoF) {
        perror("Error while read bootlogo.raw");
        return errno;
    }
    
    fastF = fopen("fastboot.raw", "r");
    if (!fastF) {
        perror("Error while read fastboot.raw");
        return errno;
    }
    
    
    fseek(logoF, 0L, SEEK_END);
    logosize = ftell(logoF);
    
    fseek(logoF, 0L, SEEK_SET);
    
    fseek(fastF, 0L, SEEK_END);
    fastsize = ftell(fastF);
    
    fseek(fastF, 0L, SEEK_SET);
    
    
    for (long int i = 0; i < LOGO_LEN; i++) {
        logoBin[i] = fgetc(logoF);
    }
    
    for (long int i = 0; i < FAST_LEN; i++) {
        fastBin[i] = fgetc(fastF);
    }
    
    for (long long int i = 0; i < FILE_LEN; i++)
    {
        if (i >= 0 && i < 48) {
            fileBin[i] = headerLogo[i];
        }
        
        if (i >= 48 && i < 512) {
            fileBin[i] = 0x00;
        }
        
        if (i >= 512 && i < 6221312)
        {
            fileBin[i] = logoBin[i - 512];
        }
        
        if (i >= 7234048 && i < 7234064) {
            fileBin[i] = headerFast[i - 7234048];
        }
        
        if (i >= 7234560 && i < 7549560)
        {
            fileBin[i] = fastBin[i - 7234560];
        }
    }
    
    fileF = fopen("logo-modified.bin", "w");
    if (!fileF) {
        perror("Error writing file logo-modified.bin");
        return errno;
    }
    
    for (long int i = 0; i < FILE_LEN; i++) {
        fwrite(&fileBin[i], sizeof(uint8_t), 1, fileF);
    }
    
    printf("All ok. logo-modified.bin created correctly! \n");
    
    free(fileBin);
    free(logoBin);
    free(fastBin);
    
    return 0;
}
