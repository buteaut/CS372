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

#include "ftserver.h"

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

/*
* control is the function that directs the program after a connection has been made with the client.
* Control will receive the command from the client and either call the appropriate function or report
* a bad command both to the console and to the client. Then control closes the connection to the client.
*/
void control(int connectionFD) {
	char command[10]; //command code from client
	memset(command, 0, sizeof(command));
	const char* errorMSG = "_error_"; //error message to send to client
	printf("waiting to receive command\n");
	recv(connectionFD, command, sizeof(command) - 1, 0);
	printf("received command %s\n", command);

	if (strcmp(command, "-l") == 0) {
		//list directory
		list(connectionFD);
	}

	else if (strcmp(command, "-g") == 0) {
		//get file
		getf(connectionFD);
	}

	else { //bad command code from client
		//kill it
		printf("Invalid command received from client.\n");
		send(connectionFD, errorMSG, strlen(errorMSG), 0);
	}
	close(connectionFD); // Close the existing socket which is connected to the client
}

/*
* list is the function that directs the program after the list command has been received from
* the client. List will then call newCon to create the dataport connection with the client as
* well as dirget to create and send a list of the directory contents. Finally, list will send
* a final message to the client to notify the client that the list is complete and close the 
* socket.
*/
void list(int connectionFD) {
	int newsocket = newCon(connectionFD);
	if (newsocket == -1) return;
	printf("Sending directory contents\n");
	dirget(newsocket);
	const char* finished = "_finished_";
	send(newsocket, finished, strlen(finished), 0);
	close(newsocket);
}

/*
* getf is the function that directs the program after the get command has been received from
* the client. Getf will then call newCon to create the dataport connection with the client as
* well as retrieveFile to send a copy of the specified file. Finally, getf will send a final
* message to the client to notify the client that the transfer is complete and close the
* socket.
*/
void getf(int connectionFD) {
	int newsocket = newCon(connectionFD);
	if (newsocket == -1) return;	
	retrieveFile(newsocket);
	sleep(1);
	const char* end = "_EOF_";
	send(newsocket, end, strlen(end), 0);
	close(newsocket);	
}

/*
* newCon is a function which creates a new dataport connection between the server and the 
* client and returns said connection. This function is largely based on code from previous
* assignments in CS344-400.
*/
int newCon(int connectionFD) {
	const char* valid = "_valid_"; //success message to send to client
	const char* errorMSG = "_error_"; //error message to send to client
	char portNumber[10];
	char clientaddr[100];
	int socketFD;
	memset(portNumber, 0, sizeof(portNumber));
	memset(clientaddr, 0, sizeof(clientaddr));
	//printf("sending acknowledgment %s\n", valid);
	send(connectionFD, valid, strlen(valid), 0);
	recv(connectionFD, portNumber, sizeof(portNumber) - 1, 0);
	//printf("Received portNumber %s\n", portNumber);
	if (portNumber[0] == NULL) {
		printf("Error with portNumber\n");
		send(connectionFD, errorMSG, sizeof(errorMSG), 0);
		return -1;
	}
	send(connectionFD, valid, strlen(valid), 0);
	printf("waiting for client address\n");
	recv(connectionFD, clientaddr, sizeof(clientaddr), 0);
	printf("Connection from %s\n", clientaddr);
	if (clientaddr[0] == NULL) {
		send(connectionFD, errorMSG, sizeof(errorMSG), 0);
		return -1;
	}
	send(connectionFD, valid, strlen(valid), 0);
	printf("Received request to transmit on port %s\n", portNumber);
	sleep(2); //give client time to set up new socket

	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(atoi(portNumber)); // Store the port number

	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	serverHostInfo = gethostbyname(clientaddr); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

																											// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
		error("CLIENT: ERROR connecting");
		exit(1);
	}
	printf("Finished creating data connection %i\n", socketFD);
	return socketFD;
}

/*
* dirget is a function to create and return an array of char strings which list all of the items in the current
* directory. The code for this function is largely based on http://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
*/
void dirget(int newsocket) { //char dirlist[][100]) {
	char dirlist[256][100];
	memset(dirlist, 0, sizeof(dirlist));

	struct dirent *de;  // Pointer for directory entry
						// opendir() returns a pointer of DIR type. 
	DIR *dr = opendir(".");

	if (dr == NULL)  // opendir returns NULL if couldn't open directory
	{
		printf("Could not open current directory");
		return;
	}

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
	// for readdir()
	int x = 0;
	while ((de = readdir(dr)) != NULL) {
		strcpy(dirlist[x], de->d_name);
		printf("%s\n", dirlist[x]);
		//send(newsocket, dirlist[x], strlen(dirlist[x]), 0);
		x++;
	}
	for (int y = 0; y < x; y++) {
		send(newsocket, dirlist[y], sizeof(dirlist[y]), 0);
	}
	closedir(dr);

	//return dirlist;
}

/*
* retrieveFile is a function to verify a file exists and then send it to the client. The code for this function is based on
* http://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/ as well as 
* https://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
*/
void retrieveFile(int newsocket) {
	char filename[100];
	memset(filename, 0, sizeof(filename));
	recv(newsocket, filename, sizeof(filename), 0);
	printf("File %s requested from client\n", filename);

	struct dirent *de;  // Pointer for directory entry
						// opendir() returns a pointer of DIR type. 
	DIR *dr = opendir(".");

	if (dr == NULL)  // opendir returns NULL if couldn't open directory
	{
		printf("Could not open current directory");
		return;
	}

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
	// for readdir()
	int found = 0;
	int x = 0;
	while ((de = readdir(dr)) != NULL) {
		if (strcmp(filename, de->d_name) == 0) {
			found = 1;
		}
	}

	if (found == 0) {
		printf("File not found. Sending error message\n");
		const char* errorNF = "_error_";
		send(newsocket, errorNF, strlen(errorNF), 0);
		return;
	}
	printf("Sending %s to client\n", filename);
	const char* fileFound = "_found_";
	send(newsocket, fileFound, strlen(fileFound), 0);
	int filedata = open(filename, O_RDONLY);
	char buffer[100];
	while (1) {
		memset(buffer, 0, sizeof(buffer));
		// Read data into buffer.  We may not have enough to fill up buffer, so we
		// store how many bytes were actually read in bytes_read.
		int bytes_read = read(filedata, buffer, sizeof(buffer));
		if (bytes_read == 0) // We're done reading from the file
			break;

		if (bytes_read < 0) {
			// handle errors
		}
		// You need a loop for the write, because not all of the data may be written
		// in one call; write will return how many bytes were written. p keeps
		// track of where in the buffer we are, while we decrement bytes_read
		// to keep track of how many bytes are left to write.
		void *p = buffer;
		while (bytes_read > 0) {
			int bytes_written = send(newsocket, p, bytes_read, 0);
			if (bytes_written <= 0) {
				// handle errors
			}
			bytes_read -= bytes_written;
			p += bytes_written;
		}
	}

}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

																			 // Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

												// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	while (1) {
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		printf("Server open on %i\n", portNumber);
		control(establishedConnectionFD);	
	}
	
	close(listenSocketFD); // Close the listening socket
	return 0;
}