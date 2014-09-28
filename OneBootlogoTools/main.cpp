//
//  main.cpp
//  OnePlusOneBootConverter
//
//  Created by Matteo Gaggiano on 23/09/14.
//  Copyright (c) 2014 Matteo Gaggiano. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../lodepng/lodepng.h"

#define AUTHOR "Matteo Gaggiano"
#define VERSION "1.1b2"

#if DEBUG
#define DLOG(fmt, ...) printf("Debug: " fmt "\n", ##__VA_ARGS__);
#else
#define DLOG(fmt, ...) do{} while(0)
#endif

#ifdef __LINUX__
#define uint8_t unsigned char
#endif

#define usage(x) std::cout << getexecname(x) << "\n\t-b \t Overwrite the bootlogo\n\t-f \t Overwrite the fastboot image\n\t-s \t Save raw images\n\nVersion: " VERSION " by " AUTHOR << std::endl;

unsigned long FILE_LEN = 9821696;
unsigned long LOGO_LEN = 6220800;
unsigned long FAST_LEN = 315000;

char* getexecname(const char * x)
{
    char*a,*b=NULL;
    strtok_r((char*)x, "/",&a);
    while (a!=NULL) {strtok_r(NULL, "/",&a);b=(a==NULL)?b:strdup(a);}
    return b;
}

void RGBToBGR(std::vector<unsigned char>& image)
{
    unsigned char temp;
    for (int i = 0; i < image.size(); i+=3) {
        temp = image[i];
        image[i] = image[i+2];
        image[i+2] = temp;
    }
}

int main(int argc, const char * argv[]) {
    
	long unsigned int i = 0;
    bool flashBoot = false, flashFast = false, saverawimages = false;
    
	char opt = 0;

    while (opt < argc) {
        if (strcmp(argv[0], "-b") == 0) flashBoot = true;
        if (strcmp(argv[0], "-f") == 0) flashFast = true;
        if (strcmp(argv[0], "-s") == 0) saverawimages = true;
        opt++;
    }
    
	if ( !flashBoot && !flashFast ) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
    
    std::vector<unsigned char> logoF, fastF;
    unsigned int w, h, error;
    FILE *fileF = NULL;

    if (flashBoot) {
        std::cout<<"Loading bootlogo.png"<<std::endl;
        if ((error = lodepng::decode(logoF, w, h, "bootlogo.png", LCT_RGB, 8))) {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (logoF.size() != LOGO_LEN) {
            std::cerr << "Error: the w*h of the bootlogo.png isn't 1080x1920" << std::endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(logoF);
        if (saverawimages) {
            lodepng::save_file(logoF, "bootlogo.raw");
            std::cout<<"Saved bootlogo.raw"<<std::endl;
        }
    }
    
    if (flashFast) {
        std::cout<<"Loading fastboot.png"<<std::endl;
        if ((error = lodepng::decode(fastF, w, h, "fastboot.png", LCT_RGB, 8))) {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (fastF.size() != LOGO_LEN) {
            std::cerr << "Error: the w*h of the fastboot.png isn't 350x300" << std::endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(logoF);
        if (saverawimages) {
            lodepng::save_file(fastF, "fastboot.raw");
            std::cout<<"Loading fastboot.raw"<<std::endl;
        }
    }
    
    uint8_t *fileBin = (uint8_t *) malloc(sizeof(uint8_t) * FILE_LEN);
    
    fileF = fopen("logo.bin", "r");
    if (!fileF) {
        perror("Error while read logo.bin");
        return errno;
    }
    
    for ( i = 0; i < FILE_LEN; i++) {
        fileBin[i] = fgetc(fileF);
    }
    
    fclose(fileF);
    bool bm = false, fm = false;
    for ( i = 0; i < FILE_LEN; i++)
    {
        if (i >= 512 && i < 6221312 && flashBoot)
        {
            fileBin[i] = logoF[i - 512]; // bootlogo image
            if (!bm) {
                std::cout<<"Writing bootlogo image"<<std::endl;
                bm=true;
            }
        } else
        
        if (i >= 7234560 && i < 7549560 && flashFast)
        {
            fileBin[i] = fastF[i - 7234560]; // fastboot logo image
            if (!fm) {
                std::cout<<"Writing fastboot image"<<std::endl;
                fm=true;
            }
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
    
    std::cout<<"All jobs done! The file logo-modified.bin was created correctly"<<std::endl;
    
    free(fileBin);
    
    return 0;
}
