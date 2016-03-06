//
//  main.c
//  Library
//
//  Created by William Edward Gillespie on 2/25/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ClientMessage.h"
#include "ServerMessage.h"
#include "BookInfo.h"

int doesContainUserIdAndPassword(int inputUserId, int inputPassword);
int getBookInformationFromFile(BookInfo bookInfo[], int *size);
int writeBookInformationToFile(BookInfo bookInfo[], int size);

int main(int argc, const char * argv[]) {
    BookInfo bookInfo[10];
    int nElems;// = &q;
    memset(bookInfo, 0, sizeof(bookInfo));
    int d = getBookInformationFromFile(bookInfo, &nElems);
    writeBookInformationToFile(bookInfo, nElems);
    //printf("S: %lu\n", sizeof(bookInfo)/ sizeof(BookInfo));
    /*
    int i = 0;
    while (i < 4) {
        BookInfo bi = bookInfo[i];
        printf("%s|%s|%s|%d|%d|%s|%d|%d\n", bi.isbn, bi.authors, bi.title, bi.edition, bi.year, bi.publisher, bi.inventory, bi.available);
        i++;
    }
     */
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
int getBookInformationFromFile(BookInfo bookInfo[], int *size){//maybe include isbn?
    FILE *fp = fopen("books.txt", "r");
    int bufferSize = 512;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    
    int iteration = 0;
    int isFirstIteration = 1;//1 means yes, 0 means no
    while (fgets(buffer, bufferSize, (FILE*)fp)) {
        if (isFirstIteration == 1) {
            isFirstIteration = 0;
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

        indicesOfDelimiters[delimiterCounter] = index - 1;//index of newline

        /*now I know the delimiters; I can fill the book info array with the info*/
        //isbn
        int sizeOfSubstring = indicesOfDelimiters[0] - 1 - 0;
        memset(bookInfo[iteration].isbn, 0, sizeof(bookInfo[iteration].isbn));
        strncpy(bookInfo[iteration].isbn, buffer, sizeOfSubstring);
        
        //authors
        memset(bookInfo[iteration].authors, 0, sizeof(bookInfo[iteration].authors));
        sizeOfSubstring =indicesOfDelimiters[1]  - 1 - indicesOfDelimiters[0];
        strncpy(bookInfo[iteration].authors, buffer + indicesOfDelimiters[1] + 1, sizeOfSubstring);
        
        //title
        memset(bookInfo[iteration].title, 0, sizeof(bookInfo[iteration].title));
        sizeOfSubstring = indicesOfDelimiters[2] - 1 - indicesOfDelimiters[1];
        strncpy(bookInfo[iteration].title, buffer + indicesOfDelimiters[1] + 1, sizeOfSubstring);
        
        //edition
        sizeOfSubstring = indicesOfDelimiters[3] - 1 - indicesOfDelimiters[2];
        char editionChar[sizeOfSubstring + 1];
        memset(editionChar, 0, sizeof(editionChar));
        strncpy(editionChar, buffer + indicesOfDelimiters[2] + 1, sizeOfSubstring);
        int edition = atoi(editionChar);
        bookInfo[iteration].edition = edition;
        
        //year
        sizeOfSubstring = indicesOfDelimiters[4] - 1 - indicesOfDelimiters[3];
        char yearChar[sizeOfSubstring + 1];
        memset(yearChar, 0, sizeof(yearChar));
        strncpy(yearChar, buffer + indicesOfDelimiters[3] + 1, sizeOfSubstring);
        int year = atoi(yearChar);
        bookInfo[iteration].year = year;
        
        //publisher
        sizeOfSubstring = indicesOfDelimiters[5] - 1 - indicesOfDelimiters[4];
        memset(bookInfo[iteration].publisher, 0, sizeof(bookInfo[iteration].publisher));
        strncpy(bookInfo[iteration].publisher, buffer + indicesOfDelimiters[4] + 1, sizeOfSubstring);
        
        //inventory
        sizeOfSubstring = indicesOfDelimiters[6] - 1 - indicesOfDelimiters[5];
        char inventoryChar[sizeOfSubstring + 1];
        memset(inventoryChar, 0, sizeof(inventoryChar));
        strncpy(inventoryChar, buffer + indicesOfDelimiters[5] + 1, sizeOfSubstring);
        int inventory = atoi(inventoryChar);
        bookInfo[iteration].inventory = inventory;
        
        //available
        sizeOfSubstring = indicesOfDelimiters[7] - 1 - indicesOfDelimiters[6];
        char availableChar[sizeOfSubstring + 1];
        memset(availableChar, 0, sizeof(availableChar));
        strncpy(availableChar, buffer + indicesOfDelimiters[6] + 1, sizeOfSubstring);
        int available = atoi(availableChar);
        bookInfo[iteration].available = available;
        iteration++;
    }
    *size = iteration;
    fclose(fp);
    return -1;
}

int writeBookInformationToFile(BookInfo bi[], int size){
    FILE *fp = fopen("newBooks.txt", "w");
    for (int i = 0; i < size; i++) {
        char buffer[1000];
        sprintf(buffer, "%s|%s|%s|%d|%d|%s|%d|%d\n", bi[i].isbn, bi[i].authors, bi[i].title, bi[i].edition, bi[i].year, bi[i].publisher, bi[i].inventory, bi[i].available);
        fputs(buffer, fp);
    }
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



















