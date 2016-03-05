//
//  ClientMessage.h
//  Library
//
//  Created by William Edward Gillespie on 3/2/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#ifndef ClientMessage_h
#define ClientMessage_h

#include <stdio.h>

typedef struct {
    unsigned int requestID;
    unsigned int userID;
    unsigned int password;
    enum {Login, Logout, Query, Borrow, Return} requestType;
    char isbn[13];
} ClientMessage;

#endif /* ClientMessage_h */
