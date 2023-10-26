#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "blogoperation.h"
#include "game.h"
#include "common.h"

#define NEW_CONNECTION 1
#define NEW_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE_TOPIC 4
#define DISCONNECT 5

int main(int argc, char **argv)
{
    struct BlogOperation blog_operation;

    struct sockaddr_storage storage;
    if (0 != server_sock_addr_init(argv[1], argv[2], &storage))
    {
        exit(EXIT_FAILURE);
    }

    // socket to receive the client connection
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        exit(EXIT_FAILURE);
    }

    struct sockaddr *addr = (struct sockaddr *)&storage;
    if (0 != bind(s, addr, sizeof(storage)))
    {
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
    {
        exit(EXIT_FAILURE);
    }

    if (0 != listen(s, 10))
    {
        exit(EXIT_FAILURE);
    }

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int clientSocket;

    #define MAX_CLIENTS 10
    bool client_ids[MAX_CLIENTS] = {false};

    // find first available client id
    int client_id = blog_operation.client_id;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!client_ids[i])
        {
            client_id = i+1;
            client_ids[i] = true;
            break;
        }
    }

    if (client_id == 0)
    {
        // no available client ids
        exit(EXIT_FAILURE);
    }

    // socket to communicate with client
    clientSocket = accept(s, caddr, &caddrlen);
    if (clientSocket == -1) {
        exit(EXIT_FAILURE);
    } else {
        printf("client %d connected\n", client_id);
    }

    while (true) {
        // receives msg from clientSocket, stores it in blog struct
        recv(clientSocket, &blog_operation, sizeof(struct BlogOperation), 0);

        // ...

        if (blog_operation.operation_type == DISCONNECT)
        {
            // client disconnects
            close(clientSocket);
            client_ids[client_id] = false; // mark client id as unused
        }

        // ...
    }

    return 0;

    // sending response msg to client
    send(clientSocket, &blog_operation, sizeof(struct BlogOperation), 0);

    return 0;
}