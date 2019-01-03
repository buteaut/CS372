/*
* Author: Thomas Buteau
* Program: ftserver
* Class: CS372-400
* Date: 11-26-17
* Decription: This is the server program for the project 2 assignment. It is designed to create a socket
* using the port number provided as an argument and wait for a connection from the client program. When the connection
* is made the server will receive a command from the client and they will switch to a port designated by the client to
* either send a directory to the client or a specific file. An error message will be displayed if a bad command, a bad 
* filename is given, or if a connection error occur. In order to kill the program press ctrl+c. Note that the bulk of 
* the networking code here is based on a previous assignment for CS344-400. Other sources used will be cited as needed.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>


void error(const char *msg);

void sendData(int socketFD, char* message);

void receiveData(int socketFD, char* buffer);

void control(int connectionFD);

void list(int connectionFD);

void getf(int connectionFD);

int newCon(int connectionFD);

void dirget(int con);

void retrieveFile(int con);