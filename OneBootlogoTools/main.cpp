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

using namespace std;

#include "../lodepng/lodepng.h"
#include "item.h"

#define AUTHOR "Matteo Gaggiano (from a work by chillstep1998)"
#define VERSION "1.2b3"

#if DEBUG
#define DLOG(fmt, ...) printf("Debug: " fmt "\n", ##__VA_ARGS__);
#else
#define DLOG(fmt, ...) do{} while(0)
#endif

#ifdef __LINUX__
#define uint8_t unsigned char
#endif

#define usage(x) cout << getexecname(x) << "\n\t-b \t Overwrite the bootlogo\n\
\t-f \t Overwrite the fastboot image\n\
\t-l \t Overwrite the lowbattery image\n\
\t-s \t Save raw images\n\n\
Version: " VERSION " by " AUTHOR << endl;

#define FILE_LEN 9821696L
#define PNG_CHANNEL 3L

#define LOGO_START 512L
#define LOGO_W      1080L
#define LOGO_H      1919L
#define LOGO_LEN (LOGO_W*LOGO_H*PNG_CHANNEL)
#define LOGO_END (LOGO_START+LOGO_LEN)


#define FAST_START 7234560L
#define FAST_W 350L
#define FAST_H 313L
#define FAST_LEN (FAST_W*FAST_H*PNG_CHANNEL)
#define FAST_END (FAST_START+FAST_LEN)

#define LOWBAT_START 7563776L
#define LOWBAT_W 645L
#define LOWBAT_H 1074L
#define LOWBAT_LEN (LOWBAT_W*LOWBAT_H*PNG_CHANNEL)
#define LOWBAT_END (LOWBAT_START+LOWBAT_LEN)


#define bigendian(n) ((n & 0xFF) << 24)\
| ((n & 0xFF00) << 8)\
| ((n >> 8) & 0xFF00)\
| (n >> 24)\

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

void RGBToBGR(vector<unsigned char>& image)
{
    unsigned char temp;
    for (int i = 0; i < image.size(); i+=3) {
        temp = image[i];
        image[i] = image[i+2];
        image[i+2] = temp;
    }
}

#if 0
int newLogo(int, const char*[]);
#endif

int main(int argc, const char * argv[]) {
#if 0
    newLogo(argc, argv);
    return 0;
#endif
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
    
    vector<unsigned char> bootlogoF, fastbootF, lowbatteryF;
    unsigned int w, h, error;
    FILE *fileF = NULL;

    if (flashBoot) {
        cout<<"Loading bootlogo.png"<<endl;
        if ((error = lodepng::decode(bootlogoF, w, h, "bootlogo.png", LCT_RGB, 8))) {
            cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
            exit(EXIT_FAILURE);
        }
        if (bootlogoF.size() != LOGO_LEN) {
            cerr << "Error: the w*h of the bootlogo.png isn't " << LOGO_W << "x" << LOGO_H << endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(bootlogoF);
        if (saverawimages) {
            lodepng::save_file(bootlogoF, "bootlogo.raw");
            cout<<"Saved bootlogo.raw"<<endl;
        }
    }
    
    if (flashFast) {
        cout<<"Loading fastboot.png"<<endl;
        if ((error = lodepng::decode(fastbootF, w, h, "fastboot.png", LCT_RGB, 8))) {
            cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
            exit(EXIT_FAILURE);
        }
        if (fastbootF.size() != FAST_LEN) {
            cerr << "Error: the w*h of the fastboot.png isn't " << FAST_W << "x" << FAST_W << endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(fastbootF);
        if (saverawimages) {
            lodepng::save_file(fastbootF, "fastboot.raw");
            cout<<"Saved fastboot.raw"<<endl;
        }
    }
    
    if (flashLowBattery) {
        cout<<"Loading lowbattery.png"<<endl;
        if ((error = lodepng::decode(lowbatteryF, w, h, "lowbattery.png", LCT_RGB, 8))) {
            cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
            exit(EXIT_FAILURE);
        }
        if (lowbatteryF.size() != LOWBAT_LEN) {
            cerr << "Error: the w*h of the lowbattery.png isn't " << LOWBAT_W << "x" << LOWBAT_H << endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(lowbatteryF);
        if (saverawimages) {
            lodepng::save_file(lowbatteryF, "lowbattery.raw");
            cout<<"Saved lowbattery.raw"<<endl;
        }
    }
    
    uint8_t *fileBin = (uint8_t *) malloc(sizeof(uint8_t) * FILE_LEN);
    
//    uint8_t fileBin[FILE_LEN];
    
    bzero(fileBin, FILE_LEN);
    
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
        if ( flashBoot && i >= LOGO_START && i < LOGO_END )
        {
            fileBin[i] = (bootlogoF[i - LOGO_START]); // bootlogo image
            if (!bm) {
                cout << "Writing bootlogo image" << endl;
                bm=true;
            }
        } else
        
        if ( flashFast && i >= FAST_START && i < FAST_END)
        {
            fileBin[i] = (fastbootF[i - FAST_START]); // fastboot logo image
            if (!fm) {
                cout << "Writing fastboot image" << endl;
                fm=true;
            }
        } else
        if ( flashLowBattery && i >= LOWBAT_START && i < LOWBAT_END )
        {
            fileBin[i] = (lowbatteryF[i - LOWBAT_START]); // lowbattery logo image
            if (!lm) {
                cout << "Writing lowbattery image" << endl;
                lm=true;
            }
        }
    }
    
    cout << "Saving modified binary..." << endl;
    
    fileF = fopen("logo-modified.bin", "w");
    if (!fileF) {
        perror("Error writing file logo-modified.bin");
        return errno;
    }
    
//    for ( i = 0; i < FILE_LEN; i++) {
        fwrite(&fileBin, sizeof(unsigned char)*FILE_LEN, FILE_LEN, fileF);
//    }

    free(fileBin);
    
    cout << "All jobs done! The file logo-modified.bin was created correctly" << endl;
        
    return 0;
}

#if 0
int newLogo(int argc, const char * argv[]) {
    
    FILE *f = fopen("/Users/matteo/Desktop/logo.bin", "r");
    if (!f) {
        perror("Reading file");
        exit(1);
    }
    
    fseek(f, 0L, SEEK_END);
    long size = ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    uint8_t *binary = (uint8_t*)malloc(sizeof(uint8_t)*size);
    
    printf("File size:          %ld\n", size);
    for (int i = 0; i < size; i++) {
        binary[i] = fgetc(f);
    }

    item items[7];
    
    int j = 0;
    for ( int i = 0; i < size; i+=soa(kMagicNumber)) {
        if (strncmp((const char*)binary+i, kMagicNumber, soa(kMagicNumber)) == 0)
            items[j++] = getItem(i, binary, true);
    }
    
    vector<uint8_t> enc;
    int w, h;
    lodepng::decode(&enc, &w, &h, "bootlogo.png", LCT_RGB, 8);
    printItem(items[2]);
    
    printf("\n");
    
    return 0;
}
#endif
