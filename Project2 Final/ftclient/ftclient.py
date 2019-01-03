# Author: Thomas Buteau
# Program: Project 2 Client
# Course: CS372-400
# Date: 11-26-17
# Description: This is the client program for Project 2. This program connects with the server program and either receives
# a list of the items in the server's working directory to display on the terminal or receives a copy of a specified file and 
# writes said file to the client's working directory. Note that the bulk of the code here is based on lecture material.
# Other sources used will be cited as needed.

from socket import *
import sys
#import socket

#getLocalHost finds and returns the local host address
#code for this function taken from https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
def getLocalHost():
    s = socket(AF_INET, SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    return s.getsockname()[0]

#getList receives and prints the directory entries provided by the server
def getList():
    print("Receiving directory structure from " + str(addr))
    result = (connectionSocket.recv(100)).decode('UTF-8')
    while(result != "_finished_"):
        print(result)
        result = (connectionSocket.recv(100)).decode('UTF-8')

#getFile writes to file the data provided by the server
#code for this function based on  https://stackoverflow.com/questions/38580012/transfer-files-with-sockets-in-python-3
def getFile():
    found = (connectionSocket.recv(100)).decode('UTF-8')
    if(found == "_found_"):
        print("Receiving " + filename)
        filecpy = open(filename, 'w')
        buffer = (connectionSocket.recv(100)).decode('UTF-8')
        while(buffer != "_EOF_"):
            filecpy.write(buffer)
            buffer = None
            buffer = (connectionSocket.recv(100)).decode('UTF-8')
        print("File trasnfer complete.")
    else:
        print("Error, server file not found")

#main starts here
serverName = sys.argv[1]
serverPort = int(sys.argv[2])
command = str(sys.argv[3])
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverName, serverPort))

#send the command "-l" or "-g"
clientSocket.send(command.encode('ascii'))
validCommand = (clientSocket.recv(1024)).decode('UTF-8')

if not(validCommand == "_valid_"):
    print("Error with sent command")
    exit(1)
if sys.argv[3] == "-l":
    #send the port number
    newPortNum = int(sys.argv[4])
    clientSocket.send(str(newPortNum).encode('ascii'))

else: 
    #send the port number
    newPortNum = int(sys.argv[5])
    clientSocket.send(str(newPortNum).encode('ascii'))

validPort = (clientSocket.recv(1024)).decode('UTF-8')
if not(validPort == "_valid_"):
    print("Error with sent port number")
    exit(1)

#send the client address
clientSocket.send(getLocalHost().encode('ascii'))
validHost = (clientSocket.recv(1024)).decode('UTF-8')
if not(validHost == "_valid_"):
    print("Error with sent address")
    exit(1)

serverSocket = socket(AF_INET,SOCK_STREAM)
serverSocket.bind(('',newPortNum))
serverSocket.listen(1)
connectionSocket, addr = serverSocket.accept()

if(command == "-l"):
    getList()
    
if(command == "-g"):
    filename = str(sys.argv[4])
    print("The file is " + filename)
    connectionSocket.send(filename.encode('ascii'))
    getFile()
        
        