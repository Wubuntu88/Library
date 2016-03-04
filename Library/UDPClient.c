//
//  UDPClient.c
//  Library
//
//  Created by William Edward Gillespie on 2/25/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include "ClientMessage.h"

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *userId;                /* String to send to echo server */
    char *userPassword;
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    //int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    
    if ((argc < 4) || (argc > 5))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <1: Server IP> <2: user ID> <3: password> [4: <Echo Port>]\n", argv[0]);
        exit(1);
    }
    
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    userId = argv[2];       /* Second arg: string to echo */
    userPassword = argv[3];
    
    /*********
     start of my code
     *********/
    
    char welcomeMessage[] = "welcome to the library\n";
    char optionsMenu[] = "You can query, borrow, or return books\n\t-type q to query\n\t-type b to borrow\n\t-type r to retrun\n\t-type e to quit\nYour Respones: ";
    
    ClientMessage clientMessage;
    memset(&clientMessage, 0, sizeof(clientMessage));
    clientMessage.requestID = 5;
    clientMessage.requestType = Query;
    strcpy(clientMessage.isbn, "9780132126953");
    printf("reqID: %d\n", clientMessage.requestID);
    printf("req type: %d\n", clientMessage.requestType);
    
    printf("ISBN: %s\n", clientMessage.isbn);
    printf("%s", welcomeMessage);
    printf("%s", optionsMenu);
    
    
    /*********
     end of my code
     *********/
    
    //if ((echoStringLen = strlen(userId)) > ECHOMAX)  /* Check input length */
    //    DieWithError("Echo word too long");
    
    if (argc == 5)
        echoServPort = atoi(argv[4]);  /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */
    
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */
    
    int clientMessageLen = sizeof(clientMessage);
    /* Send the string to the server */
    if (sendto(sock, &clientMessage, clientMessageLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != clientMessageLen)
        DieWithError("sendto() sent a different number of bytes than expected");
    
    printf("passed sendto");
    
    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                                 (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
        DieWithError("recvfrom() failed");
    
    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }
    
    /* null-terminate the received data */
    echoBuffer[respStringLen] = '\0';
    printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */
    
    close(sock);
    exit(0);
}