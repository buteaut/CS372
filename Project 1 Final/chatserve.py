# Author: Thomas Buteau
# Program: Project 1 Chat Server
# Course: CS372-400
# Date: 10-29-17
# Description: This is the server chat program for the project 1 assignment. It is designed to create a socket
# using the port number provided as argument 0 and wait for a connection from the client program. When the connection
# is made the two programs can send messages back and forth until one of them quits. At which point this program will
# return to waiting for a connection. In order to kill the program press ctrl+c. Note that the bulk of the code here
# is based on lecture material. Other sources used will be cited as needed.

from socket import *
import sys
import signal

## code for ctrl c interupt based on code seen at https://pythonadventures.wordpress.com/2012/11/21/handle-ctrlc-in-your-script/
def sigint_handler(signum, frame):
    print("Terminating program")
    sys.exit() 
signal.signal(signal.SIGINT, sigint_handler)

## The receiveMessage function waits for a message from the other program and when it arrives displays it to the screen.
## Should it receive notification the other program has disconnected it will return 1 instead of 0 to main.
def receiveMessage():
    disconnect = 0
    sentence = connectionSocket.recv(501)
    if not sentence: print("Client disconnected."); disconnect = 1; return disconnect
    print(sentence.decode('UTF-8'))
    return disconnect

## The sendMessage function reads in a message from the user and sends it to the socket connected program. If the user
## types "/quit" a message will not be sent and the function will return 1 instead of 0 to main.
def sendMessage():
    response = input(serverName + "> ")
    disconnect = 0
    if (response == "\\quit"): disconnect = 1; return disconnect
    response = serverName + "> " + response
    connectionSocket.send(response.encode('ascii'))
    return disconnect


if not (len(sys.argv) == 3): print("charserve.py <port #> <server name>"); sys.exit()
serverPort = sys.argv[1]
serverName = sys.argv[2]
serverSocket = socket(AF_INET,SOCK_STREAM)
try: serverSocket.bind(('',int(serverPort)))
except: print("Port not available"); sys.exit()
serverSocket.listen(1)
print ("The server " + serverName + " is ready on port " + serverPort)

while 1:
    print("Waiting for connection.")
    connectionSocket, addr = serverSocket.accept()
    print("Connection established.")
    alive = 1
    while (alive == 1):
        if(receiveMessage() == 1): break #client has disconnected, break call/response loop

        if (sendMessage() == 1): 
            print("Closing connection.")
            connectionSocket.close()
            break

    connectionSocket.close()
    print("Connection closed.")
