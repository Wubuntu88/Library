Student: William Gillespie
My client server programs:  These programs simulate making transactions with a library database.
Client:
To compile the client program, type: gcc -o client.exe UDPClient.c BookInfo.c DieWithError.c
To execute the client program, type: ./client.exe 127.0.0.1 2001 
(that will work for a single machine)

Server:
To compile the server program, type: gcc -o server.exe UDPServer.c BookInfo.c DieWithError.c 
To run the server program, type: ./server.exe 2001

The books.txt file stores the book database
The userIDs file stores the userIDs and passwords (but the extra credit authentication protocol was not developed)
The BookInfo.h file has a BookInfo struct; the BookInfo.c file has a method that determines a valid isbn.
There are ClientMessage.h and ServerMessage.h that respectively contain the structs used for the clients message and the servers message.
The main.c file is only for my tests and playing around.
