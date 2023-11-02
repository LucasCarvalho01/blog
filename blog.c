#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "blog.h"

void printTopics(char topics[MAX_TOPICS][MAX_TOPIC_NAME_LENGTH], int num_topics)
{
    if (num_topics == 0)
    {
        printf("no topics available\n");
    }
    else
    {
        for (int i = 0; i < num_topics; i++)
        {
            printf("%s; ", topics[i]);
        }
        printf("\n");
    }
}

char *getTopics(char topics[MAX_TOPICS][MAX_TOPIC_NAME_LENGTH], int numTopics)
{
    char *contentToSend = malloc(MAX_TOPICS * MAX_TOPIC_NAME_LENGTH * sizeof(char));

    if (numTopics == 0)
    {
        strcpy(contentToSend, "no topics available");
        return contentToSend;
    }

    printf("%s ", topics[0]);
    strcpy(contentToSend, topics[0]);
    for (int i = 1; i < numTopics; i++)
    {
        printf("%s ", topics[i]);
        strcat(contentToSend, "; ");
        strcat(contentToSend, topics[i]);
    }

    return contentToSend;
}

void setResponse(struct BlogOperation *response, int client_id, int operation, int isServerResponse, char *topic, char *content)
{
    response->client_id = client_id;
    response->operation_type = operation;
    response->server_response = isServerResponse;
    memcpy(response->topic, topic, strlen(topic));
    memcpy(response->content, content, strlen(content));
}

void subscribeToTopic(char topic[], int client_id, bool subscriptions[][MAX_TOPICS], char topics[][MAX_TOPIC_NAME_LENGTH], int *numTopics)
{
    int aux = -1;

    for (int i = 0; i < (*numTopics); i++)
    {
        if (strcmp(topics[i], topic) == 0)
        {
            aux = i;
            break;
        }
    }

    if (aux == -1)
    {
        // topic does not exist, create new topic
        aux = *numTopics;
        strcpy(topics[aux], topic);
        (*numTopics)++;
    }

    printf("client %02d subscribed to %s\n", client_id, topic);
    subscriptions[client_id - 1][aux] = true;
}

void unsubscribeToTopic(int client_id, char *topic_name, bool subscriptions[MAX_CLIENTS][MAX_TOPICS], char topics[MAX_TOPICS][MAX_TOPIC_NAME_LENGTH], int num_topics)
{
    int topic = -1;

    for (int i = 0; i < num_topics; i++)
    {
        if (strcmp(topic_name, topics[i]) == 0)
        {
            topic = i;
            break;
        }
    }

    if (topic == -1)
    {
        // topic does not exist, ignore
        return;
    }

    printf("client %02d unsubscribed to %s\n", client_id, topic_name);

    subscriptions[client_id - 1][topic] = false;
}

void unsubscribeAllTopics(int client_id, bool subscriptions[MAX_CLIENTS][MAX_TOPICS], int num_topics)
{
    for (int i = 0; i < num_topics; i++)
    {
        subscriptions[client_id - 1][i] = false;
    }
}

void createNewPost(struct BlogOperation blog_operation, bool subscriptions[][MAX_TOPICS], char topics[][MAX_TOPIC_NAME_LENGTH], int *numTopics)
{
    bool topicExists = false;
    int topicIndex = -1;

    // check if topic exists
    for (int i = 0; i < (*numTopics); i++)
    {
        if (strcmp(blog_operation.topic, topics[i]) == 0)
        {
            topicExists = true;
            topicIndex = i;
            break;
        }
    }

    if (topicExists == false)
    {
        // topic does not exist, create new topic
        int aux = (*numTopics);
        strcpy(topics[aux], blog_operation.topic);
        (*numTopics)++;
    }

    printf("new post added in %s by %02d\n", blog_operation.topic, blog_operation.client_id);
}

void printOperationDebug(struct BlogOperation operation)
{
    printf("\nReceived msg from client:\n");
    printf("\nclient_id: %d\n", operation.client_id);
    printf("operation_type: %d\n", operation.operation_type);
    printf("server_response: %d\n", operation.server_response);
    printf("topic: %s\n", operation.topic);
    printf("content: %s\n", operation.content);
}
