//
//  UDPServer.c
//  Library
//
//  Created by William Edward Gillespie on 2/25/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//
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

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage);  /* External error handling function */
int doesContainUserIdAndPassword(int inputUserId, int inputPassword);

int main(int argc, const char * argv[]) {
    
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    ClientMessage clientMessage;
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
    int userIDs[200];
    memset(&userIDs, -1, sizeof(userIDs));
    int indexToPutNextUserID = 0;
    
    
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
        fprintf(stderr, "before recv");
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, &clientMessage, sizeof(clientMessage), 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
        fprintf(stderr, "after recv");
        fprintf(stderr, "userID: %d", clientMessage.userID);
        fprintf(stderr, "user password: %d", clientMessage.password);
        /*
         The server responds to a client based on the client request:
         request types: Login, Logout, Query, Borrow, Return
         */
        switch (clientMessage.requestType) {
            case Login:
                if (doesContainUserIdAndPassword(clientMessage.userID, clientMessage.password)) {
                    userIDs[indexToPutNextUserID] = clientMessage.userID;
                    indexToPutNextUserID = (indexToPutNextUserID + 1) % sizeof(userIDs);
                }
                printf("login");
                break;
            case Logout:
                printf("logout");
                int indexOfLoggingOutUserID = -1;
                for (int i = 0; i < sizeof(userIDs); i++) {
                    if (userIDs[i] == clientMessage.userID) {
                        indexOfLoggingOutUserID = i;
                        break;
                    }
                }
                if (indexOfLoggingOutUserID >= 0) {
                    userIDs[indexOfLoggingOutUserID] = -1;
                }else{
                    //tell the user that the ID was invalid
                }
                break;
            case Query:
                printf("query");
                //get isbn from user; must check if it is valid
                int isbn = 0;
                scanf("%d", &isbn);
                //MUST TURN INTEGER INTO CHAR ISBN, THEN VALIDATE ISBN
                //THEN CHECK THE FILE FOR THE INFO AND RETURN IT TO THE USER.
            case Borrow:
                printf("borrow");
                break;
            case Return:
                printf("return");
                break;
            default:
                printf("no request type detected");
        }
        //printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        
        /* Send received datagram back to the client */
        /*
        if (sendto(sock, echoBuffer, recvMsgSize, 0,
                   (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");
        */
    }
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

















