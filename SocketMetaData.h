//
// Created by Mudassir Noor on 2020-02-17.
//

#ifndef SOCKETMETADATA_H
#define SOCKETMETADATA_H

#include "List.h"
#include <pthread.h>
#include <unistd.h>
#include "Helpers.h"

typedef struct SocketMetaData {
    int sockfd;
    pthread_cond_t condition;
    pthread_mutex_t mutex;
    List* messages;
} SocketMetaData;

/**
 * Creates a new socket with initialized attributes
 * @param _sockfd socket id
 * @return pointer to new socket
 */
SocketMetaData* newSocket(int _sockfd);

/**
 * Adds a message to the end of the message list in SocketMetaData
 * @param socketMetaData socket containing the message list
 * @param message message to add to list
 */
void AddMessage(SocketMetaData* socketMetaData, char* message);

/**
 * Removes a message from the beginning of the list
 * @param socketMetaData socket to remove message from
 * @return removed message
 */
char* RemoveMessage(SocketMetaData* socketMetaData);

/**
 * Destroys a socket
 * @param socketMetaData
 */
void DestroySocketMetaData(SocketMetaData* socketMetaData);

#endif //SOCKETMETADATA_H