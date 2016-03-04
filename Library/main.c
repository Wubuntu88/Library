//
//  main.c
//  Library
//
//  Created by William Edward Gillespie on 2/25/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "ClientMessage.h"

int main(int argc, const char * argv[]) {
    ClientMessage clientMessage;
    clientMessage.requestID = 5;
    clientMessage.requestType = Query;
    strcpy(clientMessage.isbn, "9780132126953");
    printf("reqID: %d\n", clientMessage.requestID);
    
    if (clientMessage.requestType == Query) {
        printf("req type: %d\n", clientMessage.requestType);
    }
    printf("isbn: %s\n", clientMessage.isbn);
    return 0;
}
