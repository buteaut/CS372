/*
* Author: Thomas Buteau
* Program: chatclient
* Class: CS372-400
* Date: 10-29-17
* Description: This is the client program header file for project 1. It takes two arguments; hostname, and host port# for the server
*           it is supposed to connect to. Once a connection is established with the server this "client" and the server
*           can send messages back and forth until either types "/quit" where upon the connection will be broken and this
*           program will terminate. Note that the code used here is largely recycled from a previous assignment for CS344-400.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <string>

void error(const char *msg);

void sendData(int socketFD, char* message);

void receiveData(int socketFD, char* buffer);