#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include "blogoperation.h"
#include "blog.h"
#include "common.h"

#define MAX_CLIENTS 10
#define MAX_TOPICS 20
#define MAX_TOPIC_NAME_LENGTH 50
#define MAX_POSTS 20

void *client_thread(void *data);
void send_message_to_clients(int sender_id, char *message);

typedef struct
{
    int csock;
    struct sockaddr_storage storage;
} client_data;

client_data *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
struct BlogOperation blog_operation;
char topics[MAX_TOPICS][MAX_TOPIC_NAME_LENGTH];
bool client_ids[MAX_CLIENTS] = {false};
bool subscriptions[MAX_CLIENTS][MAX_TOPICS] = {false};
int numTopics = 0;

int main(int argc, char **argv)
{
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

    char addrstr[256];
    converterEnderecoEmString(addr, addrstr, 256);
    printf("[log] bound to %s, waiting connections\n", addrstr);

    while (true)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        // socket to communicate with client
        int clientSocket;
        clientSocket = accept(s, caddr, &caddrlen);
        if (clientSocket == -1)
        {
            exit(EXIT_FAILURE);
        }

        printf("accepted new connection. creating thread to communicate\n");

        struct client_data *cdata = malloc(sizeof(client_data));
        if (!cdata)
        {
            printf("malloc failed\n");
            exit(EXIT_FAILURE);
        }

        cdata->csock = clientSocket;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);

        printf("thread created\nWaiting new connections\n");
    }

    return 0;
}

void *client_thread(void *data)
{
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);
    struct BlogOperation clientBlogOperation;
    struct BlogOperation response;

    char caddrstr[256];
    converterEnderecoEmString(caddr, caddrstr, 256);
    printf("[log] connection from %s\n", caddrstr);

    while (true)
    {
        // receives msg from clientSocket, stores it in blog struct
        recv(cdata->csock, &clientBlogOperation, sizeof(struct BlogOperation), 0);
        printOperationDebug(clientBlogOperation);

        if (clientBlogOperation.operation_type == NEW_CONNECTION)
        {
            // find first available client id
            int client_id = clientBlogOperation.client_id;

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_ids[i] == false)
                {
                    client_id = i + 1;
                    client_ids[i] = true;
                    printf("client %02d connected\n", client_id);
                    break;
                }
            }

            if (client_id == 0)
            {
                // no available client ids
                printf("no available client ids\n");
                break;
            }

            setResponse(&response, client_id, NEW_CONNECTION, 1, "", "");
        }

        if (clientBlogOperation.operation_type == LIST_TOPICS)
        {
            char *topicsNames = getTopics(topics, numTopics);
            setResponse(&response, clientBlogOperation.client_id, LIST_TOPICS, 1, "", topicsNames);
        }

        else if (clientBlogOperation.operation_type == SUBSCRIBE_TOPIC)
        {
            subscribeToTopic(clientBlogOperation.topic, clientBlogOperation.client_id, subscriptions, topics, &numTopics);
            setResponse(&response, clientBlogOperation.client_id, SUBSCRIBE_TOPIC, 1, clientBlogOperation.topic, "");
        }

        else if (clientBlogOperation.operation_type == UNSUBSCRIBE_TOPIC)
        {
            unsubscribeToTopic(clientBlogOperation.client_id, clientBlogOperation.topic, subscriptions, topics, numTopics);
            setResponse(&response, clientBlogOperation.client_id, UNSUBSCRIBE_TOPIC, 1, clientBlogOperation.topic, "");
        }

        else if (clientBlogOperation.operation_type == NEW_POST)
        {
            createNewPost(clientBlogOperation, subscriptions, topics, &numTopics);

            setResponse(&response, clientBlogOperation.client_id, NEW_POST, 1, clientBlogOperation.topic, clientBlogOperation.content);
        }

        else if (clientBlogOperation.operation_type == DISCONNECT)
        {
            unsubscribeAllTopics(clientBlogOperation.client_id, subscriptions, numTopics);
            client_ids[clientBlogOperation.client_id - 1] = false; // mark client id as unused

            printf("client %02d was disconnected\n", clientBlogOperation.client_id);
            setResponse(&response, clientBlogOperation.client_id, DISCONNECT, 1, "", "");
            send(cdata->csock, &response, sizeof(struct BlogOperation), 0);

            break;
        }

        // sending response msg to client
        send(cdata->csock, &response, sizeof(struct BlogOperation), 0);
    }

    close(cdata->csock);
    pthread_exit(EXIT_SUCCESS);
}
