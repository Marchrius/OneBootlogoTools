//
//  item.h
//  OneBootlogoTools
//
//  Created by Matteo Gaggiano on 16/10/14.
//  Copyright (c) 2014 Matteo Gaggiano. All rights reserved.
//

#ifndef __OneBootlogoTools__item__
#define __OneBootlogoTools__item__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../lodepng/lodepng.h"

#define PixelsNumber 3

#define soa(x) (sizeof(x)/sizeof(*x))

const char kMagicNumber[8] = { 'S', 'P', 'L', 'A', 'S', 'H', '!', '!' };
const char *units[5] = {"B", "KB", "MB", "GB", "TB"};

typedef struct {
    int width;
    int height;
    int size;
    int pos;
    char name[64];
} item;

item getItem(int, uint8_t*, bool);
void printItem(item);
void printData(item, uint8_t*, int);
bool isPrintable(uint8_t);
bool isALogoFile(uint8_t*);
int decodeAllItem(item *, uint8_t*);
uint8_t *decodeItem(item, uint8_t*);
#endif /* defined(__OneBootlogoTools__item__) */
