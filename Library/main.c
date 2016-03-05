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
#include "ServerMessage.h"

int doesContainUserIdAndPassword(int inputUserId, int inputPassword);
int getBookInformationFromFile(ServerMessage serverMessage);


int main(int argc, const char * argv[]) {
    int d = doesContainUserIdAndPassword(239, 777);
    if (d == 1) {
        fprintf(stderr, "matched\n");
    }else{
        fprintf(stderr, "no match\n");
    }
    return 0;
}
/*
 Fills the struct with the book info if the info was correctly found
 returns 0 if the book information was not correctly found
 */
int getBookInformationFromFile(ServerMessage serverMessage){//maybe include isbn?
    
    return -1;
}

/*
 returns 1 if there is a match; returns 0 if there is no match
 */
int doesContainUserIdAndPassword(int inputUserId, int inputPassword){
    FILE *fp = fopen("userIDs", "r");
    int bufferSize = 255;
    char buffer[bufferSize];
    memset(buffer, 0, sizeof(buffer));
    
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
        //buffer[i] is null char: 0
        //buffer[i-1] is the newline char: \n
        
        //checking the userID
        indexOfNewLine = i;
        int numLenOfUserId = indexOfPipeDelimiter;
        for (int index = indexOfPipeDelimiter - 1; index >= 0; index--) {
            int numericalChar = (int)(buffer[index] - '0');
            int multiplier = (int)pow(10, (numLenOfUserId - 1) - index);
            currentUserId += numericalChar * multiplier;
        }
        //checking the password
        for (int index = indexOfNewLine - 2; index > indexOfPipeDelimiter; index--) {
            int numericalChar = (int)(buffer[index] - '0');
            int exponent = (indexOfNewLine - 2) - index;
            int multiplier = (int)pow(10, exponent);
            currentPassword += numericalChar * multiplier;
        }
        if (currentUserId == inputUserId && currentPassword == inputPassword) {
            fclose(fp);
            return 1;//match
        }
    }
    fclose(fp);
    return 0;//did not match
}



















