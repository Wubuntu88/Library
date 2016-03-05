//
//  main.c
//  Library
//
//  Created by William Edward Gillespie on 2/25/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ClientMessage.h"

int doesContainUserIdAndPassword(int inputUserId, int inputPassword);

int main(int argc, const char * argv[]) {
    int d = doesContainUserIdAndPassword(123, 555);
    if (d == 1) {
        fprintf(stderr, "matched\n");
    }else{
        fprintf(stderr, "no match\n");
    }
    return 0;
}

int doesContainUserIdAndPassword(int inputUserId, int inputPassword){
    FILE *fp = fopen("userIDs", "r");
    int bufferSize = 255;
    char buffer[bufferSize];
    
    while (fgets(buffer, bufferSize, (FILE*)fp) != NULL) {
        int i = 0;
        int indexOfPipeDelimiter = 0;
        int indexOfNewLine = 0;
        int currentUserId = 0;
        int currentPassword = 0;
        while (buffer[i] != 0) {
            if (buffer[i] == '|') {
                indexOfPipeDelimiter = i;
            }
            i++;
        }
        indexOfNewLine = i;
        int numLenOfUserId = indexOfPipeDelimiter;
        for (int index = indexOfPipeDelimiter - 1; index >= 0; index--) {
            int numericalChar = (int)(buffer[index] - '0');
            int multiplier = (int)pow(10, (numLenOfUserId - 1) - index);
            currentUserId += numericalChar * multiplier;
        }
        fprintf(stderr, "buffer: %s\n", buffer);
        fprintf(stderr, "myuserid: %d\n", currentUserId);
        
        //do the same to find the password
        //I had a problem with the index of the End of file
        //it was 12 when it should be 7 (i think)
        //int numLenOfPassword = (indexOfNewLine - 1) - indexOfPipeDelimiter;
        printf("end of line index: %d", indexOfNewLine);
        for (int index = indexOfNewLine - 2; index > indexOfPipeDelimiter; index--) {
            int numericalChar = (int)(buffer[index] - '0');
            int exponent = (indexOfNewLine - 2) - index;
            int multiplier = (int)pow(10, exponent);
            printf("index: %d\n", index);
            //printf("exponent: %d\n", exponent);
            //printf("current pass: %d\n", currentPassword);
            currentPassword += numericalChar * multiplier;
        }
        fprintf(stderr, "buffer: %s\n", buffer);
        fprintf(stderr, "mypassword: %d\n", currentPassword);
        
        if (currentUserId == inputUserId && currentPassword == inputPassword) {
            return 1;
        }
        
    }
    
    
    return 0;
}



















