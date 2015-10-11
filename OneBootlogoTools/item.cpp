//
//  item.cpp
//  OneBootlogoTools
//
//  Created by Matteo Gaggiano on 16/10/14.
//  Copyright (c) 2014 Matteo Gaggiano. All rights reserved.
//

#include "item.h"
#include "../lodepng/lodepng.h"

using namespace::std;

item getItem (int start, uint8_t *binary, bool is_new)
{
    item item;
    char buff[64];
    sprintf(buff, "0x%.2x%.2x%.2x%.2x", binary[start + 35], binary[start + 34], binary[start + 33], binary[start + 32]);
    int w = (int)strtol(buff, NULL, 16);
    sprintf(buff, "0x%.2x%.2x%.2x%.2x", binary[start + 39], binary[start + 38], binary[start + 37], binary[start + 36]);
    int h = (int)strtol(buff, NULL, 16);
    sprintf(buff, "0x%.2x%.2x%.2x%.2x", binary[start + 43], binary[start + 42], binary[start + 41], binary[start + 40]);
    int s =  (int)strtol(buff, NULL, 16);
    item.width = w;
    item.height = h;
    item.size = s;
    item.pos = start+512;
    bzero(item.name, soa(item.name));
    for (int i = start + 160; i < start+160+64; i++) {
        item.name[i-start-160] = binary[i];
    }
    return item;
}

void printData(item item, uint8_t *binary, int n) {
    int s = item.pos;
    int e = s+item.size;
    int size = e-s;
    char buff[64];
    char path[32];
    sprintf(buff, "/usr/bin/xxd -c %d -s %d -l %d %s", n, s, size, "logo.bin");
    
    FILE *fp;
    fp = popen(buff, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        exit(1);
    }
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        printf("%s", path);
    }
    pclose(fp);
}

void printItem(item item)
{
    printf("Name:\t%s\nWidth:\t%d\nHeight:\t%d\nStart:\t%d\nEnd:\t%d\nSize:\t%d\n\n", item.name, item.width, item.height, item.pos, item.pos+item.size, item.size);
}

item encodeItem(item item, uint8_t *binary) {
    vector <uint8_t> encoded;
    unsigned error;
    if ((error = lodepng::decode(encoded, item.width, item.height, "bootlogo.png", LCT_RGB, 8))) {
        
    }
    printf("Esito per %s: %s\n", item.name, lodepng_error_text(error));
    
    
}

int decodeAllItem(item *items, uint8_t *binary) {
    for (int i = 0; i < 7; i++) {
        printf("Preparo la decodifica di %s\n", items[i].name);
        uint8_t *decoded = decodeItem(items[i], binary);
        printf("Scrivo il file png\n");
        unsigned c = lodepng_encode24_file(items[i].name,
                                           decoded,
                                           items[i].width,
                                           items[i].height
                                           );
        printf("Esito per %s: %s\n", items[i].name, lodepng_error_text(c));
    }
    return 0;
}

uint8_t *decodeItem(item item, uint8_t *binary) {
    unsigned long decodedBytes = 0, i = 0, pixelBytes = item.width*item.height*PixelsNumber;
    uint8_t data[item.size] ;
    uint8_t *image = (uint8_t*)malloc(pixelBytes);
    unsigned long length = item.size;
    
    printf("Copio i raw byte di %s\n", item.name);
    for (int j = item.pos; j < item.pos+item.size; j++) {
        data[j-item.pos] = binary[j];
    }
    
    printf("Faccio qualcosa che non ho ben capito\n");
    
    while((i < length) && (decodedBytes < pixelBytes)){
        
        memset(&image[decodedBytes], data[i], (data[i + 1]));
        decodedBytes += (unsigned char)data[i+1];
        i = i + 2 	;
        
        if ((pixelBytes - decodedBytes < (unsigned char)data[i + 1]) && (i  < length)){
            memset(&image[decodedBytes], data[i], pixelBytes - decodedBytes);
            decodedBytes = pixelBytes;
            fprintf(stdout, "More information was in encoding than resolution called for.\n");
            break;
        }
    }
    fprintf(stdout, "%lu decoded bytes\n", decodedBytes);
    
    if (false){
        fprintf(stdout, "Swapping red and blue color..\n");
        unsigned char old;
        i = 0;
        while( i < pixelBytes){
            old = image[i];
            memset(&image[i], image[i + 2], 1);
            memset(&image[i + 2], old, 1);
            i += PixelsNumber;
            
        }
    }
    return image;
}