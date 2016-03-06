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
#include "ServerMessage.h"

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
    int respStringLen;               /* Length of received response */
    int requestID = 0;
    
    char optionsMenu[] = "You can query, borrow, or return books\n\t-type q to query\n\t-type b to borrow\n\t-type r to retrun\n\t-type e to quit\nYour Respones: ";
    
    
    if ((argc < 4) || (argc > 5))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <1: Server IP> <2: user ID> <3: password> [4: <Echo Port>]\n", argv[0]);
        exit(1);
    }
    
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    userId = argv[2];       /* Second arg: string to echo */
    userPassword = argv[3];
    
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
    
    /* struct for the server message that the client will recieve from server */
    ServerMessage serverMessage;
    memset(&serverMessage, 0, sizeof(serverMessage));
    int serverMessageLen = sizeof(serverMessage);
    
    /* get username and password from the user; put it into clientMessage struct */
    ClientMessage clientMessage;
    int clientMessageLen = sizeof(clientMessage);
    memset(&clientMessage, 0, clientMessageLen);
    
    printf("welcome to the library\n  You can type your userid and password, or 0 to quit\n");
    
    do { // authenticating the user; after this do while, the user can
        printf("Enter Your userID (or 0 to quit):");
        scanf("%d", &clientMessage.userID);
        if (clientMessage.userID == 0) {
            printf("Exiting program");
            exit(0);
        }
        printf("\nEnter your password: ");
        scanf("%d", &clientMessage.password);
        clientMessage.requestType = Login;
        clientMessage.requestID = requestID;
        requestID++;
        if (sendto(sock, &clientMessage, clientMessageLen, 0, (struct sockaddr *)
                   &echoServAddr, sizeof(echoServAddr)) != clientMessageLen)
            DieWithError("sendto() sent a different number of bytes than expected");
        
        /* Recv a response */
        fromSize = sizeof(fromAddr);
        if ((respStringLen = recvfrom(sock, &serverMessage, serverMessageLen, 0,
                                      (struct sockaddr *) &fromAddr, &fromSize)) != serverMessageLen)
            DieWithError("recvfrom() failed");
        
        if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
        {
            fprintf(stderr,"Error: received a packet from unknown source.\n");
            exit(1);
        }
        
        if (serverMessage.responseType == Okay) {
            printf("\nUser authenticated; welcome to the library.");
            break;
        }else {
            printf("invalid login credentials; you will have to re-enter your credentials.");
        }
    }while (1);
    
    
    
    /* Send the string to the server */
    if (sendto(sock, &clientMessage, clientMessageLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != clientMessageLen)
        DieWithError("sendto() sent a different number of bytes than expected");
    
    printf("passed sendto");
    
    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, &serverMessage, serverMessageLen, 0,
                                 (struct sockaddr *) &fromAddr, &fromSize)) != serverMessageLen)
        DieWithError("recvfrom() failed");
    
    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }
    
    /* null-terminate the received data */
    //echoBuffer[respStringLen] = '\0';
    //printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */
    printf("end");
    close(sock);
    exit(0);
}
