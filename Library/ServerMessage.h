//
//  ServerMessage.h
//  Library
//
//  Created by William Edward Gillespie on 3/4/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#ifndef ServerMessage_h
#define ServerMessage_h

#include <stdio.h>

typedef struct {
    unsigned int requestID;
    unsigned int userID;
    enum {Okay, ISBNError, AllGone, NoInventory, InvalidLogin} responseType;
    char isbn[13];
    char authors[100];
    char title[100];
    unsigned int edition;
    unsigned int year;
    char publisher[100];
    unsigned int inventory;
    unsigned int available;
} ServerMessage;

#endif /* ServerMessage_h */
