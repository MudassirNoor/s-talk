//
// Created by Mudassir Noor on 2020-02-17.
//
#include "SocketMetaData.h"
#include "stdlib.h"
#include <unistd.h>

SocketMetaData* newSocket(int _sockfd) {
    SocketMetaData* new = malloc(sizeof(SocketMetaData));
    new->sockfd = _sockfd;
    pthread_cond_init(&new->condition, NULL);
    pthread_mutex_init(&new->mutex, NULL);
    new->messages = ListCreate();
    return new;
}

void AddMessage(SocketMetaData* socketMetaData, char* message) {
    ListPrepend(socketMetaData->messages, message);
}

char* RemoveMessage(SocketMetaData* socketMetaData) {
    return ListTrim(socketMetaData->messages);
}

void DestroySocketMetaData(SocketMetaData* socketMetaData) {
    close(socketMetaData->sockfd);

    pthread_mutex_destroy(&socketMetaData->mutex);
    pthread_cond_destroy(&socketMetaData->condition);

    ListFree(socketMetaData->messages, (void (*)(void *)) freeItem);
    free(socketMetaData);
}