//
// Created by Mudassir Noor on 2020-02-17.
//

#include "Helpers.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define BUFFERSIZE 200

int termination = 0;

int bindSocket(int socket, struct addrinfo* address){
    return bind(socket, address->ai_addr, address->ai_addrlen);
}

int createSocket(struct addrinfo* address) {
    return socket(address->ai_family, address->ai_socktype, address->ai_protocol);
}

struct addrinfo* getBoundedAddress(int* sockfd, struct addrinfo* addressResults) {
    struct addrinfo* actualResult;
    for (actualResult = addressResults; actualResult != NULL; actualResult = actualResult->ai_next) {
        *sockfd = createSocket(actualResult);
        if (*sockfd < 0) {
            continue;
        }

        if (bindSocket(*sockfd, actualResult) < 0) {
            close(*sockfd);
            continue;
        }
        break;
    }

    if (actualResult == NULL) {
        fprintf(stderr, "Failed to bind local socket\n");
        exit(-1);
    }

    return actualResult;
}

struct addrinfo* getAddrInfoResults(char* hostname, char* portNumber, int ai_flag) {
    int state;
    struct addrinfo hint, *results;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_protocol = IPPROTO_UDP;

    if (ai_flag == AI_PASSIVE) {
        hint.ai_flags = AI_PASSIVE;
    }

    if ((state = getaddrinfo(hostname, portNumber, &hint, &results)) < 0) {
        fprintf(stderr, "Could not get address information: %s\n", gai_strerror(state));
        exit(-1);
    }

    return results;
}

void validateInputParameters(int argc) {
    if (argc <= 0) {
        perror("Please pass in the parameters: [port number] [remote machine name] [remote port number");
        exit(-1);
    } else if (argc < 4) {
        perror("Insufficient parameters");
        exit(-1);
    }
}

void readMessage(void* argument) {
    SocketMetaData* senderSocket = (SocketMetaData*) argument;
    do {
        void *message = malloc(BUFFERSIZE);
        read(0, message, BUFFERSIZE);

        if (strcmp(message, "!\n") == 0)
            termination = 1;

        pthread_mutex_lock(&senderSocket->mutex);
        ListAppend(senderSocket->messages, message);
        pthread_cond_signal(&senderSocket->condition);
        pthread_mutex_unlock(&senderSocket->mutex);
    } while (termination == 0);
    pthread_exit(NULL);
}

void sendMessage(void* argument[]) {
    SocketMetaData* senderSocket = (SocketMetaData*) argument[0];
    struct addrinfo* remoteAddress = (struct addrinfo *) argument[1];
    do {
        pthread_mutex_lock(&senderSocket->mutex);
        pthread_cond_wait(&senderSocket->condition, &senderSocket->mutex);
        while (ListFirst(senderSocket->messages) != NULL) {
                void* text = RemoveMessage(senderSocket);
                sendToAddress(senderSocket->sockfd, text, remoteAddress);
                freeItem(text);
        }
        pthread_mutex_unlock(&senderSocket->mutex);
    } while(termination == 0);

    pthread_cancel(receiveThread);
    pthread_cancel(printThread);
    pthread_exit(NULL);
}

void receiveMessage(void* argument[]) {
    int received;
    SocketMetaData* receivingSocket = (SocketMetaData*) argument[0];
    struct addrinfo* remoteAddress = (struct addrinfo*) argument[1];
    do {
        void* message = malloc(BUFFERSIZE);
        received = recvfrom(receivingSocket->sockfd, message, BUFFERSIZE, 0, remoteAddress->ai_addr, &remoteAddress->ai_addrlen);
        if (received < 0){
            fprintf(stderr, "Receive failed: %s\n", strerror(errno));
        }
        else {
            if (strcmp(message, "!\n") == 0)
                termination = 1;
            pthread_mutex_lock(&receivingSocket->mutex);
            AddMessage(receivingSocket, message);
            pthread_cond_signal(&receivingSocket->condition);
            pthread_mutex_unlock(&receivingSocket->mutex);
        }
    } while (termination == 0);
    pthread_exit(NULL);
}

void printMessage(void* argument) {
    SocketMetaData* receivingSocket = (SocketMetaData*) argument;
    do {
        pthread_mutex_lock(&receivingSocket->mutex);
        pthread_cond_wait(&receivingSocket->condition, &receivingSocket->mutex);
        while (ListFirst(receivingSocket->messages) != NULL) {
                void* text = RemoveMessage(receivingSocket);
                printMessageToScreen(text);
                freeItem(text);
            }
        pthread_mutex_unlock(&receivingSocket->mutex);
    } while (termination == 0);

    pthread_cancel(readThread);
    pthread_cancel(sendThread);
    pthread_exit(NULL);
}

void printMessageToScreen(void* message) {
    char fullMessage[BUFFERSIZE];
    strcpy(fullMessage, "Other: ");
    strcat(fullMessage, message);
    unsigned long int size = strlen(fullMessage);
    write(0, fullMessage, size);
}

void sendToAddress(int sockfd, void* message, struct addrinfo* address) {
    unsigned long int size = strlen(message);
    int sent = sendto(sockfd, message, size, 0, address->ai_addr, address->ai_addrlen);
    if (sent < 0){
        fprintf(stderr, "Send Failed: %s", strerror(errno));
    }
}

void freeItem(void* item) {
    free(item);
}