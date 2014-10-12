//
//  main.cpp
//  OneBootlogoTools
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

#define usage(x) std::cout << getexecname(x) << "\n\t-b \t Overwrite the bootlogo\n\t-f \t Overwrite the fastboot image\n\t-l \t Overwrite the lowbattery image\n\t-s \t Save raw images\n\nVersion: " VERSION " by " AUTHOR << std::endl;

#define FILE_LEN 9821696L

#define LOGO_START 512L
#define LOGO_LEN (1080L*1919L*3L)
#define LOGO_END (LOGO_START+LOGO_LEN)


#define FAST_START 7234560L
#define FAST_LEN (350L*313L*3L)
#define FAST_END (FAST_START+FAST_LEN)

#define LOWBAT_START 7563776L
#define LOWBAT_LEN (645L*1074L*3L)
#define LOWBAT_END (LOWBAT_START+LOWBAT_LEN)

char* getexecname(const char * x)
{
    char* a,* b = NULL;
    a = strtok((char*)x, "/");
    while (a != NULL) {
		a = strtok(NULL, "/");
		b = (a==NULL) ? b : strdup(a);
	}
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
    bool flashBoot = false, flashFast = false, flashLowBattery = false, saverawimages = false;
    
    for (int opt = 0; opt < argc; opt++) {
        if (strcmp(argv[opt], "-b") == 0) flashBoot = true;
        if (strcmp(argv[opt], "-f") == 0) flashFast = true;
        if (strcmp(argv[opt], "-l") == 0) flashLowBattery = true;
        if (strcmp(argv[opt], "-s") == 0) saverawimages = true;
    }
    
	if ( !flashBoot && !flashFast ) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
    
    std::vector<unsigned char> bootlogoF, fastbootF, lowbatteryF;
    unsigned int w, h, error;
    FILE *fileF = NULL;

    if (flashBoot) {
        std::cout<<"Loading bootlogo.png"<<std::endl;
        if ((error = lodepng::decode(bootlogoF, w, h, "bootlogo.png", LCT_RGB, 8))) {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (bootlogoF.size() != LOGO_LEN) {
            std::cerr << "Error: the w*h of the bootlogo.png isn't 1080x1919" << std::endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(bootlogoF);
        if (saverawimages) {
            lodepng::save_file(bootlogoF, "bootlogo.raw");
            std::cout<<"Saved bootlogo.raw"<<std::endl;
        }
    }
    
    if (flashFast) {
        std::cout<<"Loading fastboot.png"<<std::endl;
        if ((error = lodepng::decode(fastbootF, w, h, "fastboot.png", LCT_RGB, 8))) {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (fastbootF.size() != FAST_LEN) {
            std::cerr << "Error: the w*h of the fastboot.png isn't 350x313" << std::endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(fastbootF);
        if (saverawimages) {
            lodepng::save_file(fastbootF, "fastboot.raw");
            std::cout<<"Saved fastboot.raw"<<std::endl;
        }
    }
    
    if (flashLowBattery) {
        std::cout<<"Loading lowbattery.png"<<std::endl;
        if ((error = lodepng::decode(lowbatteryF, w, h, "lowbattery.png", LCT_RGB, 8))) {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (lowbatteryF.size() != LOWBAT_LEN) {
            std::cerr << "Error: the w*h of the lowbattery.png isn't 645x1074" << std::endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(lowbatteryF);
        if (saverawimages) {
            lodepng::save_file(lowbatteryF, "lowbattery.raw");
            std::cout<<"Saved lowbattery.raw"<<std::endl;
        }
    }
    
    uint8_t *fileBin = (uint8_t *) malloc(sizeof(uint8_t) * FILE_LEN);
    
    fileF = fopen("logo.bin", "r");
    if (!fileF) {
        perror("Error while read logo.bin");
        return errno;
    }
    
    for ( i = 0; i < FILE_LEN; i++) {
        fileBin[i] = (fgetc(fileF));
    }
    
    fclose(fileF);
    bool bm = false, fm = false, lm = false;
    for ( i = 0; i < FILE_LEN; i++)
    {
        if (i >= LOGO_START && i < LOGO_END && flashBoot)
        {
            fileBin[i] = (bootlogoF[i - 512]); // bootlogo image
            if (!bm) {
                std::cout<<"Writing bootlogo image"<<std::endl;
                bm=true;
            }
        } else
        
        if (i >= FAST_START && i < FAST_END && flashFast)
        {
            fileBin[i] = (fastbootF[i - FAST_START]); // fastboot logo image
            if (!fm) {
                std::cout<<"Writing fastboot image"<<std::endl;
                fm=true;
            }
        } else
        if (i >= LOWBAT_START && i < LOWBAT_END && flashLowBattery)
        {
            fileBin[i] = (lowbatteryF[i - LOWBAT_START]); // lowbattery logo image
            if (!lm) {
                std::cout<<"Writing lowbattery image"<<std::endl;
                lm=true;
            }
        }
    }
    
    std::cout << "Saving modified binary..." << std::endl;
    
    fileF = fopen("logo-modified.bin", "w");
    if (!fileF) {
        perror("Error writing file logo-modified.bin");
        return errno;
    }
    
    for ( i = 0; i < FILE_LEN; i++) {
        fwrite(&fileBin[i], sizeof(unsigned char), 1, fileF);
    }

    
    std::cout<<"All jobs done! The file logo-modified.bin was created correctly"<<std::endl;
        
    return 0;
}
