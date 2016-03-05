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
#include "BookInfo.h"

int doesContainUserIdAndPassword(int inputUserId, int inputPassword);
int getBookInformationFromFile(BookInfo bookInfo[], int size);


int main(int argc, const char * argv[]) {
    BookInfo bookInfo[10];
    int d = getBookInformationFromFile(bookInfo, sizeof(bookInfo));
    /*
    int d = doesContainUserIdAndPassword(123, 555);
    if (d == 1) {
        fprintf(stderr, "matched\n");
    }else{
        fprintf(stderr, "no match\n");
    }
     */
    return 0;
}
/*
 Fills the struct with the book info if the info was correctly found
 returns 0 if the book information was not correctly found
 */
int getBookInformationFromFile(BookInfo bookInfo[], int size){//maybe include isbn?
    FILE *fp = fopen("books.txt", "r");
    int bufferSize = 512;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    
    int iteration = 0;
    
    int firstIteration = 1;
    while (fgets(buffer, bufferSize, (FILE*)fp)) {
        if (firstIteration == 1) {
            firstIteration = 0;
            continue;
        }
        //fprintf(stderr, "buffer: %s\n", buffer);
        int COUNT_OF_DELIMITERS = 8;
        int indicesOfDelimiters[COUNT_OF_DELIMITERS];
        memset(indicesOfDelimiters, 0, sizeof(indicesOfDelimiters));
        int delimiterCounter = 0;
        int index = 0;
        while (buffer[index] != 0) {
            if (buffer[index] == '|') {
                indicesOfDelimiters[delimiterCounter] = index;
                delimiterCounter++;
            }
            index++;
        }
        //fprintf(stderr, "$$buffer: %s", buffer);
        //printf("delim counter: %d\n", delimiterCounter);
        indicesOfDelimiters[delimiterCounter] = index - 1;//index of newline
        //fprintf(stderr, "$$buffer: %s", buffer);
        /*
        for (int k = 0; k < COUNT_OF_DELIMITERS; k++) {
            printf("index: %d, value: %d\n", k, indicesOfDelimiters[k]);
        }
        */
        /*now I know the delimiters; I can fill the book info array with the info*/
        memset(bookInfo[iteration].isbn, 0, sizeof(bookInfo[iteration]));
        strncpy(bookInfo[iteration].isbn, buffer, 13);
        //fprintf(stderr, "buffer: %s\n", buffer);
        
        fprintf(stderr,"isbn: %s\n", bookInfo[iteration].isbn);
        iteration++;
        
    }
    
    fclose(fp);
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
        fprintf(stderr, "buf: %s", buffer);
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
        indexOfNewLine = i - 1;
        int numLenOfUserId = indexOfPipeDelimiter;
        for (int index = indexOfPipeDelimiter - 1; index >= 0; index--) {
            int numericalChar = (int)(buffer[index] - '0');
            int multiplier = (int)pow(10, (numLenOfUserId - 1) - index);
            currentUserId += numericalChar * multiplier;
        }
        //checking the password
        for (int index = indexOfNewLine - 1; index > indexOfPipeDelimiter; index--) {
            int numericalChar = (int)(buffer[index] - '0');
            int exponent = (indexOfNewLine - 1) - index;
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



















