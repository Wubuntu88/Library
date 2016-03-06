//
//  UDPServer.c
//  Library
//
//  Created by William Edward Gillespie on 2/25/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//  Class: COSC 439
//  Instructor: Dr. Poh
/*  To compile this program: gcc -o server.exe UDPServer.c BookInfo.c DieWithError.c
    To run this program: ./server.exe 2001
 
    This program is a server that manages a library database.  Users send messages to the server and the server responds to requests (query, borrow and return)
    The server also keeps track of users logging in and logging out.
*/
//my imports
#include "ClientMessage.h"
#include "UDPServer.h"
#include "ServerMessage.h"
#include "BookInfo.h"

//other imports
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <math.h>

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage);  /* External error handling function */
int doesContainUserIdAndPassword(int inputUserId, int inputPassword);
void getBookInformationFromFile(BookInfo bookInfo[], int *size);
void writeBookInformationToFile(BookInfo bookInfo[], int size);
int indexOfUserID(int userID, int userIdList[], int sizeOfList);
int indexOfBookInfoForISBN(char isbn[], BookInfo bookInfo[], int numberOfBooks);
int isValidISBN(char isbn[], int size);//returns 1 if valid, 0 if invalid

int main(int argc, const char * argv[]) {
    
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
    
    ClientMessage clientMessage;
    ServerMessage serverMessage;
    
    //data structure to hold the userIDs
    int userIDs[200];
    memset(&userIDs, -1, sizeof(userIDs));
    int indexToPutNextUserID = 0;
    
    //data structure to hold information about books
    BookInfo bookInfo[10];
    int numberOfBooks;
    memset(bookInfo, 0, sizeof(bookInfo));
    getBookInformationFromFile(bookInfo, &numberOfBooks);
    unsigned int cyclicalDataUpdateCounter = 0;
    unsigned short TIMES_BETWEEN_UPDATE = 5;
    
    
    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }
    
    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");
    
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, &clientMessage, sizeof(clientMessage), 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
        /*
         The server responds to a client based on the client request:
         request types: Login, Logout, Query, Borrow, Return
         */
        memset(&serverMessage, 0, sizeof(serverMessage));
        int isAValidISN = 0;
        switch (clientMessage.requestType) {
            case Login:
                if (doesContainUserIdAndPassword(clientMessage.userID, clientMessage.password)) {
                    userIDs[indexToPutNextUserID] = clientMessage.userID;
                    indexToPutNextUserID = (indexToPutNextUserID + 1) % sizeof(userIDs);
                    serverMessage.responseType = Okay;
                    serverMessage.requestID = clientMessage.requestID;
                    serverMessage.userID = clientMessage.userID;
                    printf("successful login attempt; userID: %d\n", clientMessage.userID);
                }else{//if there is no Id and password for the user
                    serverMessage.responseType = InvalidLogin;
                    serverMessage.requestID = clientMessage.requestID;
                    serverMessage.userID = clientMessage.userID;
                    printf("unsuccessful login attempt; claimed userID: %d\n", clientMessage.userID);
                }
                break;
            case Logout:
                printf("**********\nUser Logout-- userID: %d\n**********\n", clientMessage.userID);
                serverMessage.requestID = clientMessage.requestID;
                serverMessage.userID = clientMessage.userID;
                int indexOfLoggingOutUserID = indexOfUserID(clientMessage.userID, userIDs, sizeof(userIDs));
                if (indexOfLoggingOutUserID >= 0) {
                    userIDs[indexOfLoggingOutUserID] = -1;
                    serverMessage.responseType = Okay;
                }else{//if the user tries to log out when they never logged in (should never happen)
                    serverMessage.responseType = InvalidLogin;//not sure what to send back to client
                }
                break;
            case Query:
                printf("**********\nBook Query-- userID: %d; ISBN: %s\n**********\n", clientMessage.userID,clientMessage.isbn);
                serverMessage.requestID = clientMessage.requestID;
                serverMessage.userID = clientMessage.userID;
                //get isbn from clientMessage; must check if it is valid
                isAValidISN = isValidISBN(clientMessage.isbn, sizeof(clientMessage.isbn));
                if (isAValidISN == 0) {//if it is invalid
                    serverMessage.responseType = ISBNError;
                    break;
                }
                
                //must check if the user is in the system
                if (indexOfUserID(clientMessage.userID, userIDs, sizeof(userIDs)) < 0) {//not found
                    serverMessage.responseType = InvalidLogin;
                }else{//user is in the system
                    int index = indexOfBookInfoForISBN(clientMessage.isbn, bookInfo, numberOfBooks);
                    if (index >= 0) {//means the book corresponding to the isbn was found
                        memset(&serverMessage, 0, sizeof(serverMessage));
                        strcpy(serverMessage.isbn, bookInfo[index].isbn);
                        strcpy(serverMessage.authors, bookInfo[index].authors);
                        strcpy(serverMessage.title, bookInfo[index].title);
                        serverMessage.edition = bookInfo[index].edition;
                        serverMessage.year = bookInfo[index].year;
                        strcpy(serverMessage.publisher, bookInfo[index].publisher);
                        serverMessage.inventory = bookInfo[index].inventory;
                        serverMessage.available  = bookInfo[index].available;
                        
                        serverMessage.responseType = Okay;
                    }else{//book with given isbn was not found
                        serverMessage.responseType = NoInventory;
                    }
                }
                break;
            case Borrow:
                printf("**********\nBook Borrowing-- userID: %d, ISBN: %s\n**********\n", clientMessage.userID, clientMessage.isbn);
                serverMessage.requestID = clientMessage.requestID;
                serverMessage.userID = clientMessage.userID;
                
                isAValidISN = isValidISBN(clientMessage.isbn, sizeof(clientMessage.isbn));
                if (isAValidISN == 0) {//if it is invalid
                    serverMessage.responseType = ISBNError;
                    break;
                }
                
                //must check if the user has logged in
                if (indexOfUserID(clientMessage.userID, userIDs, sizeof(userIDs)) < 0) {//not found
                    serverMessage.responseType = InvalidLogin;
                }else{//means the user has logged in
                    int index = indexOfBookInfoForISBN(clientMessage.isbn, bookInfo, numberOfBooks);
                    if (index >= 0) {//book found in inventory
                        if (bookInfo[index].available == 0) {//no books left
                            strcpy(serverMessage.title, bookInfo[index].title);
                            serverMessage.responseType = AllGone;
                        }else{//means the user can borrow a book; decrement available variables
                            serverMessage.responseType = Okay;
                            strcpy(serverMessage.title, bookInfo[index].title);
                            bookInfo[index].available--;
                            cyclicalDataUpdateCounter = (cyclicalDataUpdateCounter + 1) % TIMES_BETWEEN_UPDATE;
                        }
                    }else {//book not found in inventory
                        serverMessage.responseType = NoInventory;
                        break;
                    }
                }
                break;
            case Return:
                printf("**********\nBook Return-- userID: %d, ISBN: %s\n**********\n", clientMessage.userID, clientMessage.isbn);
                serverMessage.requestID = clientMessage.requestID;
                serverMessage.userID = clientMessage.userID;
                
                isAValidISN = isValidISBN(clientMessage.isbn, sizeof(clientMessage.isbn));
                if (isAValidISN == 0) {//if it is invalid
                    serverMessage.responseType = ISBNError;
                    break;
                }
                
                if (indexOfUserID(clientMessage.userID, userIDs, sizeof(userIDs)) < 0) {//not found
                    serverMessage.responseType = InvalidLogin;
                }else{//means the user has logged in
                    int index = indexOfBookInfoForISBN(clientMessage.isbn, bookInfo, numberOfBooks);
                    if (index >= 0) {
                        serverMessage.responseType = Okay;
                        strcpy(serverMessage.title, bookInfo[index].title);
                        bookInfo[index].available++;
                        cyclicalDataUpdateCounter = (cyclicalDataUpdateCounter + 1) % TIMES_BETWEEN_UPDATE;
                    }else{//means book with given isbn was not found
                        serverMessage.responseType = NoInventory;
                    }
                }
            default:
                printf("no request type detected\n");
        }//end of switch for requestType
        
        if (cyclicalDataUpdateCounter == 0) {//if it is a multiple of 5, write to file
            writeBookInformationToFile(bookInfo, numberOfBooks);
            printf("Book Info File updated,\n");
        }
        
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        
        /* Send received datagram back to the client */
        
        if (sendto(sock, &serverMessage, sizeof(serverMessage), 0,
                   (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) < 0)
            DieWithError("sendto() sent a different number of bytes than expected");
        
    }//end of infinite for loop
}//end of main()

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
}//end of doesContainUserIdAndPassword(int inputUserId, int inputPassword)

void getBookInformationFromFile(BookInfo bookInfo[], int *size){
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
        int sizeOfSubstring = indicesOfDelimiters[0];
        memset(bookInfo[iteration].isbn, 0, sizeof(bookInfo[iteration].isbn));
        strncpy(bookInfo[iteration].isbn, buffer, sizeOfSubstring);
        
        //authors
        memset(bookInfo[iteration].authors, 0, sizeof(bookInfo[iteration].authors));
        sizeOfSubstring =indicesOfDelimiters[1] - 1 - indicesOfDelimiters[0];
        strncpy(bookInfo[iteration].authors, buffer + indicesOfDelimiters[0] + 1, sizeOfSubstring);
        
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
}

void writeBookInformationToFile(BookInfo bi[], int size){
    FILE *fp = fopen("books.txt", "w");
    for (int i = 0; i < size; i++) {
        char buffer[1000];
        sprintf(buffer, "%s|%s|%s|%d|%d|%s|%d|%d\n", bi[i].isbn, bi[i].authors, bi[i].title, bi[i].edition, bi[i].year, bi[i].publisher, bi[i].inventory, bi[i].available);
        fputs(buffer, fp);
    }
    fclose(fp);
}

int indexOfUserID(int userID, int userIdList[], int sizeOfList){
    for(int i = 0; i < sizeOfList; i++){
        if (userID == userIdList[i]) {
            return i;
        }
    }
    return -1;
}

int indexOfBookInfoForISBN(char isbn[], BookInfo bookInfo[], int numberOfBooks){
    for (int i = 0; i < numberOfBooks; i++) {
        if (strcmp(bookInfo[i].isbn, isbn) == 0) {
            return i;
        }
    }
    return -1;
}












