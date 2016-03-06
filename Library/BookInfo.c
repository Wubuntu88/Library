//
//  BookInfo.c
//  Library
//
//  Created by William Edward Gillespie on 3/5/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#include "BookInfo.h"

/*
 Returns 1 if valid; 0 if invalid
 size should be 14
 */
int isValidISBN(char isbn[], int size){
    if (size != 14) {//incorrect size
        return 0;
    }
    int checkBitIndex = size - 2;
    int checkDigit;
    if ('0' <= isbn[checkBitIndex] &&  isbn[checkBitIndex] <= '9') {
        checkDigit = isbn[checkBitIndex] - '0';
    }else{
        return 0;//invalid isbn if checkbit isn't numeric
    }
    int finalIndex = size - 3;
    int sumOfWeightedProducts = 0;
    for (int i = 0; i <= finalIndex; i++) {
        if ('0' <= isbn[i] &&  isbn[i] <= '9') {
            if (i % 2 == 0) {
                sumOfWeightedProducts += (isbn[i] - '0');
            }else{
                sumOfWeightedProducts +=(isbn[i] - '0') * 3;
            }
        } else {
            return 0;//invalid isbn if isbn[i] isn't numeric
        }
    }
    
    int computedCheckDigit = 0;
    int remainder = sumOfWeightedProducts % 10;
    if (remainder != 0) {
        computedCheckDigit = 10 - remainder;
    }
    
    if (computedCheckDigit == checkDigit) {
        return 1;
    }else{
        return 0;
    }
}