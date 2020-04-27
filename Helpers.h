//
// Created by Mudassir Noor on 2020-02-17.
//

#ifndef HELPERS_H
#define HELPERS_H
#include <netdb.h>
#include "SocketMetaData.h"

pthread_t readThread, receiveThread, sendThread, printThread;

/**
 * Binds socket to specified address
 * @param socket
 * @param address of machine
 * @return -1 on failure
 */
int bindSocket(int socket, struct addrinfo* address);

/**
 * Creates socket given an address
 * @param address of a machine
 * @return -1 on failure
 */
int createSocket(struct addrinfo* address);

/**
 * Creates a socket and binds it to a machine address
 * @param sockfd
 * @param addressResults list of possible addresses that the socket can be bound to
 * @return address that has been successfully bounded or gives an error and exits
 */
struct addrinfo* getBoundedAddress(int* sockfd, struct addrinfo* addressResults);

/**
 * Successfully creates gets address information using the hostname and port number
 * @param hostname
 * @param portNumber
 * @param ai_flag Pass AI_PASSIVE here to indicate its the local address to bind the socket
 * @return list of possible addresses or prints an error and exits
 */
struct addrinfo* getAddrInfoResults(char* hostname, char* portNumber, int ai_flag);

/**
 * Ensures there is sufficient arguments passed in from the command line
 * @param argc count of number of arguments
 */
void validateInputParameters(int argc);

/**
 * Read Thread -> responsible for reading keyboard input from the user and placing it on the sending list
 * @param senderSocket
 */
void readMessage(void* argument);

/**
 * Send Thread -> responsible for sending message from the user to the remote address from the sending list
 */
void sendMessage(void* argument[]);

/**
 * Receive Thread -> receives messages from the remote address and appends it to the received messages list
 * @param receivingSocket
 * @param remoteAddress
 */
void receiveMessage(void* argument[]);

/**
 * Print Thread -> prints messages to the screen from the received messages list
 * @param receivingSocket
 */
void printMessage(void* argument);

/**
 * Frees dynamically allocated memory pointed to by the item in the list
 * @param item
 */
void freeItem(void* item);

/**
 * Print to the screen from the list
 * @param list
 */
void printMessageToScreen(void* message);

/**
 * Sends given message to the specified address
 * @param sockfd socket file descriptor
 * @param message message to send
 * @param address address to send to
 */
void sendToAddress(int sockfd, void* message, struct addrinfo* address);

#endif //HELPERS_H