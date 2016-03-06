//
//  BookInfo.h
//  Library
//
//  Created by William Edward Gillespie on 3/5/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#ifndef BookInfo_h
#define BookInfo_h

#include <stdio.h>
typedef struct {
    char isbn[14];
    char authors[100];
    char title[100];
    unsigned int edition;
    unsigned int year;
    char publisher[100];
    unsigned int inventory;
    unsigned int available;
} BookInfo;

#endif /* BookInfo_h */
