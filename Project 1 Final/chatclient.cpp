/*
* Author: Thomas Buteau
* Program: chatclient
* Class: CS372-400
* Date: 10-29-17
* Description: This is the client program implementation file for project 1. It takes two arguments; hostname, and host port# for the server
*           it is supposed to connect to. Once a connection is established with the server this "client" and the server
*           can send messages back and forth until either types "/quit" where upon the connection will be broken and this
*           program will terminate. Note that the code used here is largely recycled from a previous assignment for CS344-400.
*/

#include "chatclient.hpp"

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

/*sendData takes the message argument and sends it to the process pointed to by the socket argument. It will display errors
* if they occur.
*/
void sendData(int socketFD, char* message) {
	int charsWritten;
	charsWritten = send(socketFD, message, strlen(message), 0);
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(message)) printf("CLIENT: WARNING: Not all data written to socket!\n");
}

/* receiveData receives and processes messages from the network. If an error occurs an error message will be displayed
*  otherwise the received message will be displayed.
*/
void receiveData(int socketFD, char* buffer) {
	memset(buffer, '\0', sizeof(buffer));
	int recvInt;
	recvInt = recv(socketFD, buffer, 500, 0);
	if (recvInt == 0) {
		std::cout << "Server has closed connection. Exiting." << std::endl;
		exit(0);
	}
	else if (recvInt == -1) {
		std::cout << "Error receiving message from server." << std::endl;
	}

	std::cout << buffer << std::endl;
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[513]; //10 character name + "> " + 500 character message + \0
	char message[501];
	char clientname[11];

	if (argc != 3) { fprintf(stderr, "USAGE: %s host_name host_port\n", argv[0]); exit(0); } // Check usage & args

	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	//printf("The clinet is using port number %d\n", portNumber);
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number

	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
		error("CLIENT: ERROR connecting");
		exit(1);
	}
	memset((char*)&clientname, '\0', sizeof(clientname));
	std::cout << "Connected to server, enter user handle: ";
	std::fgets(clientname, 10, stdin);
	clientname[strcspn(clientname, "\n")] = '\0'; //removing the newline character taken from https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input#28462221
	
	while(1) {
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
											  
		memset(message, '\0', sizeof(message));
		std::cout << clientname << "> ";
		
		std::fgets(message, 500, stdin);
		message[strcspn(message, "\n")] = '\0'; //removing the newline character taken from https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input#28462221
		
		if (strcmp(message, "\\quit") == 0) {
			std::cout << "Exiting program" << std::endl;
			close(socketFD); // Close the socket
			exit(0);
		}
		message[501] = '\0';
		strcpy(buffer, clientname);
		strcat(buffer, "> ");
		strcat(buffer, message);
		sendData(socketFD, buffer);
		
		receiveData(socketFD, buffer);
	}
	
	return 0;
}
