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
#define MAX_TOPICS 10
#define MAX_TOPIC_NAME_LENGTH 50

void *client_thread(void *data);
void sendPostToSubscriptors(struct BlogOperation blog_operation, int topic_id);
int find_topic_position(char *topic_name);

struct client_data
{
    int csock;
    struct sockaddr_storage storage;
};

struct BlogOperation newPostStruct;
struct client_data *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
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

        struct client_data *cdata = malloc(sizeof(struct client_data));
        if (!cdata)
        {
            printf("malloc failed\n");
            exit(EXIT_FAILURE);
        }

        cdata->csock = clientSocket;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }
    close(s);
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
    // printf("[log] connection from %s\n", caddrstr);

    while (true)
    {
        memset(response.topic, 0, sizeof(response.topic));
        memset(response.content, 0, sizeof(response.content));

        // receives msg from clientSocket, stores it in blog struct
        recv(cdata->csock, &clientBlogOperation, sizeof(struct BlogOperation), 0);
        printOperationDebug(clientBlogOperation, false);

        if (clientBlogOperation.operation_type == NEW_CONNECTION)
        {
            // find first available client id
            int client_id = clientBlogOperation.client_id;

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_ids[i] == false)
                {
                    pthread_mutex_lock(&clients_mutex);

                    clients[i] = cdata;
                    client_id = i + 1;
                    client_ids[i] = true;

                    printf("client %02d connected\n", client_id);
                    pthread_mutex_unlock(&clients_mutex);

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
            char *topicsNames = malloc(MAX_TOPICS * MAX_TOPIC_NAME_LENGTH * sizeof(char));
            getTopics(topics, numTopics, topicsNames);
            setResponse(&response, clientBlogOperation.client_id, LIST_TOPICS, 1, "", topicsNames);
            free(topicsNames);
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

            int topic_id = find_topic_position(clientBlogOperation.topic);
            sendPostToSubscriptors(clientBlogOperation, topic_id);

            printf("new post added in %s by %02d\n", clientBlogOperation.topic, clientBlogOperation.client_id);
            setResponse(&response, clientBlogOperation.client_id, NEW_POST, 1, "", "");
        }

        else if (clientBlogOperation.operation_type == DISCONNECT)
        {
            unsubscribeAllTopics(clientBlogOperation.client_id, subscriptions, numTopics);

            pthread_mutex_lock(&clients_mutex);
            client_ids[clientBlogOperation.client_id - 1] = false; // mark client id as unused
            clients[clientBlogOperation.client_id - 1] = NULL;

            printf("client %02d was disconnected\n", clientBlogOperation.client_id);
            setResponse(&response, clientBlogOperation.client_id, DISCONNECT, 1, "", "");
            send(cdata->csock, &response, sizeof(struct BlogOperation), 0);
            pthread_mutex_unlock(&clients_mutex);

            break;
        }

        printOperationDebug(response, true);
        send(cdata->csock, &response, sizeof(struct BlogOperation), 0);
    }

    close(cdata->csock);
    free(data);
    pthread_exit(EXIT_SUCCESS);
}

// void sendPostToSubscriptors(struct BlogOperation blog_operation, int topic_id)
// {
//     pthread_mutex_lock(&clients_mutex);

//     for (int i = 0; i < MAX_CLIENTS; i++)
//     {
//         if (subscriptions[i][topic_id] == true)
//         {
//             if (write(clients[i]->csock, blog_operation.content, strlen(blog_operation.content)) < 0)
//             {
//                 perror("fail to send post to subscriptors\n");
//                 break;
//             }
//         }
//     }

//     pthread_mutex_unlock(&clients_mutex);
// }

void sendPostToSubscriptors(struct BlogOperation blog_operation, int topic_id)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_ids[i] && subscriptions[i][topic_id])
        {
            if (send(clients[i]->csock, &blog_operation, sizeof(blog_operation), 0) == -1)
            {
                printf("Error sending message to client %d\n", i);
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int find_topic_position(char *topic_name)
{
    for (int i = 0; i < numTopics; i++)
    {
        if (strcmp(topics[i], topic_name) == 0)
        {
            return i;
        }
    }
    return -1;
}
