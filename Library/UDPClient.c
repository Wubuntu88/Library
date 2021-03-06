//
//  UDPClient.c
//  Library
//
//  Created by William Edward Gillespie on 2/25/16.
//  Copyright © 2016 William Edward Gillespie. All rights reserved.
//  Class: COSC 439
//  Instructor: Dr. Poh
/*  This program is a client that communicates with the UDPServer.c program
 To compile, issue the command: gcc -o client.exe UDPClient.c BookInfo.c DieWithError.c
    To run the program: ./client.exe 127.0.0.1 2001 
    (that will work on a single machine)
 
    In this program a user can communicate with the server's library database.
    The user can Query, Borrow, and return books.
*/

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include "ClientMessage.h"
#include "ServerMessage.h"
#include "BookInfo.h"

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
    int respStringLen;               /* Length of received response */
    int requestID = 0;
    
    char optionsMenu[] = "You can query, borrow, or return books\n\t-type q to query\n\t-type b to borrow\n\t-type r to retrun\n\t-type e to quit\nYour Respones: ";
    
    
    if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <1: Server IP> [2: <Echo Port>]\n", argv[0]);
        exit(1);
    }
    
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    
    if (argc == 3)
        echoServPort = atoi(argv[2]);  /* Use given port, if any */
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
    
    UserAuthentication:do { // authenticating the user; after this do while, the user can
        printf("Enter Your userID (or 0 to quit):");
        scanf("%d", &clientMessage.userID);
        if (clientMessage.userID == 0) {
            printf("Exiting program\n");
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
            exit(0);
        }
        
        if (serverMessage.responseType == Okay) {
            printf("\nUser authenticated; welcome to the library.\n");
            break;
        }else if (serverMessage.responseType == InvalidLogin){
            printf("invalid login credentials; you will have to re-enter your credentials.\n");
        }else {
            printf("undefined message from server; you will have to re-enter your credentials.\n");
        }
    }while (1);//loop infinitely until a break statement or a call to exit()
    //now the user is authenticated, the user can query, borrow, return books, or logout.
    MainLoop:do{
        printf("you can query (1), borrow(2), return(3) books, logout(4) or quit(0).\n");
        printf("query (1): \n");
        printf("borrow (2): \n");
        printf("return (3): \n");
        printf("logout (4): \n");
        printf("quit (0): \n");
        printf("your selection: ");
        
        int choice;
        scanf("%d", &choice);
        if (choice == 1){//query
            char isbn[sizeof(clientMessage.isbn)];
            memset(isbn, 0, sizeof(isbn));
            do{//loop to get valid ISBN
                printf("Enter the ISBN (or 0 to get out): ");
                scanf("%s", isbn);
                int isValid = isValidISBN(isbn, sizeof(isbn));
                if (isValid != 0) {//if it is a valid isbn (its valid if its is not 0)
                    strcpy(clientMessage.isbn, isbn);
                    break;
                }else if (strncmp(isbn, "0\0", 2) == 0){
                    goto MainLoop;//get out and go up to top of do while
                }else{//invalid isbn
                    printf("invalid ISBN\n");
                }
            }while (1);
            
            clientMessage.requestID = requestID;
            clientMessage.requestType = Query;
        }else if(choice == 2){//borrow
            char isbn[sizeof(clientMessage.isbn)];
            memset(isbn, 0, sizeof(isbn));
            do{//loop to get valid ISBN
                printf("Enter the ISBN (or 0 to get out): ");
                scanf("%s", isbn);
                int isValid = isValidISBN(isbn, sizeof(isbn));
                if (isValid != 0) {//if it is a valid isbn (its valid if its is not 0)
                    strcpy(clientMessage.isbn, isbn);
                    break;
                }else if (strncmp(isbn, "0\0", 2) == 0){
                    goto MainLoop;//get out and go up to top of do while
                }else{//invalid isbn
                    printf("invalid ISBN\n");
                }
            }while (1);
            
            clientMessage.requestID = requestID;
            clientMessage.requestType = Borrow;
        }else if (choice == 3){//return book
            char isbn[sizeof(clientMessage.isbn)];
            memset(isbn, 0, sizeof(isbn));
            do{//loop to get valid ISBN
                printf("Enter the ISBN (or 0 to get out): ");
                scanf("%s", isbn);
                int isValid = isValidISBN(isbn, sizeof(isbn));
                if (isValid != 0) {//if it is a valid isbn (its valid if its is not 0)
                    strcpy(clientMessage.isbn, isbn);
                    break;
                }else if (strncmp(isbn, "0\0", 2) == 0){
                    goto MainLoop;//get out and go up to top of do while
                }else{//invalid isbn
                    printf("invalid ISBN\n");
                }
            }while (1);
            
            clientMessage.requestID = requestID;
            clientMessage.requestType = Return;
        }else if (choice == 4){//logout
            clientMessage.requestID = requestID;
            clientMessage.requestType = Logout;
        }else if(choice == 0){//quit
            clientMessage.requestID = requestID;
            clientMessage.requestType = Logout;//tell server I am logging out
            if (sendto(sock, &clientMessage, clientMessageLen, 0, (struct sockaddr *)
                       &echoServAddr, sizeof(echoServAddr)) != clientMessageLen)
                DieWithError("sendto() sent a different number of bytes than expected");
            printf("exiting program.\n");
            exit(0);
        }else{//undetermined
            printf("no valid choice selected; select a valid choice.\n");
            continue;
        }

        /* now that the user has chosen an appropriate choice, 
         I send the message to the server and await a response. */
        
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
            exit(0);
        }
        
        /* Now I have the response from the server in the serverMessage struct */
        if (serverMessage.responseType == Okay) {
            if (clientMessage.requestType == Query) {
                printf("Book Information:\n");
                printf("ISBN: %s\n", serverMessage.isbn);
                printf("Title: %s\n", serverMessage.title);
                printf("Author(s): %s\n", serverMessage.authors);
                printf("Pusblisher: %s\n", serverMessage.publisher);
                printf("Edition: %d, Year: %d\n", serverMessage.edition, serverMessage.year);
                printf("Inventory: %d, Available: %d\n", serverMessage.inventory, serverMessage.available);
            }else if(clientMessage.requestType == Borrow){
                printf("successfully borrowed: %s\n", serverMessage.title);
            }else if (clientMessage.requestType == Return){
                printf("successfully returned: %s\n", serverMessage.title);
            }else if(clientMessage.requestType == Logout){
                printf("you have logged out; you may log in again if you wish, or quit.\n");
                goto UserAuthentication;//go back up so the user can start over
            }
        }else if (serverMessage.responseType == ISBNError){
            printf("the ISBN you entered is invalid\n");
        }else if (serverMessage.responseType == AllGone){
            printf("sorry, there are no more copies of the book: \n");
            printf("%s\n", serverMessage.title);
        }else if (serverMessage.responseType == NoInventory){
            printf("Im sorry, this book doesn't exist in our inventory: \n");
            printf("%s\n", serverMessage.title);
        }else {
            printf("Message recieved that isn't a proper response from server.\n");
        }
        
        
        requestID++;
    }while (1);//loop infinitely until a break statement or a call to exit()
    
    /* Send the string to the server */
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
    
    /* null-terminate the received data */
    //echoBuffer[respStringLen] = '\0';
    //printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */
    printf("end");
    close(sock);
    exit(0);
}
