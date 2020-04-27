/**
 * Created by Mudassir Noor on 2020-02-21.
 *
 * References:
 * 1. Beej's Guide to Network Programming, Brian “Beej Jorgensen” Hall, https://beej.us/guide/bgnet/
 * 2. POSIX Threads Programming, Blaise Barney, Lawrence Livermore National Laboratory, https://computing.llnl.gov/tutorials/pthreads/
 */

#include "List.h"
#include "SocketMetaData.h"
#include "pthread.h"
#include <unistd.h>
#include <netdb.h>

int main(int argc, char **argv) {
    validateInputParameters(argc);

    char *localPortNumber = argv[1];
    char *remoteHostName = argv[2];
    char *remotePortNumber = argv[3];
    struct addrinfo *localAddressResults, *localAddress, *remoteAddressResults, *remoteAddress;

    SocketMetaData* sendingSocketMetaData = newSocket(0);
    SocketMetaData* receivingSocketMetaData = newSocket(0);


    localAddressResults = getAddrInfoResults(NULL, localPortNumber, AI_PASSIVE);
    remoteAddressResults = getAddrInfoResults(remoteHostName, remotePortNumber, 0);
    localAddress = getBoundedAddress(&receivingSocketMetaData->sockfd, localAddressResults);

    for (remoteAddress = remoteAddressResults; remoteAddress != NULL; remoteAddress = remoteAddress->ai_next) {
        sendingSocketMetaData->sockfd = createSocket(remoteAddress);
        if (sendingSocketMetaData->sockfd < 0) {
            continue;
        }
        else
            break;
    }
    sendingSocketMetaData->sockfd = receivingSocketMetaData->sockfd;

    void* argThread2[2];
    void* argThread3[2];

    argThread2[0] = (void*) receivingSocketMetaData;
    argThread3[0] = (void*) sendingSocketMetaData;

    argThread2[1] = (void*) remoteAddress;
    argThread3[1] = (void*) remoteAddress;

    pthread_create(&readThread, NULL, (void *(*)(void *)) readMessage, sendingSocketMetaData);
    pthread_create(&receiveThread, NULL, (void *(*)(void *)) receiveMessage, argThread2);
    pthread_create(&sendThread, NULL, (void *(*)(void *)) sendMessage, argThread3);
    pthread_create(&printThread, NULL, (void *(*)(void *)) printMessage, receivingSocketMetaData);

    pthread_join(readThread, NULL);
    pthread_join(receiveThread, NULL);
    pthread_join(sendThread, NULL);
    pthread_join(printThread, NULL);

    DestroySocketMetaData(sendingSocketMetaData);
    DestroySocketMetaData(receivingSocketMetaData);

    freeaddrinfo(localAddress);
    freeaddrinfo(remoteAddress);

    return 0;
}