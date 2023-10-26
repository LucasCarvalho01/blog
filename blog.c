#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "blog.h"
#include "blogoperation.h"

    void prinTopics(char topics[MAX_TOPICS][MAX_TOPIC_NAME_LENGTH], int num_topics) {
        if (num_topics == 0) {
            printf("no topics available\n");
        } else {
            for (int i = 0; i < num_topics; i++) {
                printf("%s; ", topics[i]);
            }
            printf("\n");
        }
    }

void setResponse(struct BlogOperation *response, int client_id, int operation, int isServerResponse, char *topic, char* content) {
    response->client_id = client_id;
    response->operation_type = operation;
    response->server_response = isServerResponse;
    memcpy(response->topic, topic, strlen(topic));
    memcpy(response->content, content, strlen(content));   
}

void subscribeToTopic(char topic[], int client_id, bool subscriptions[][MAX_TOPICS], char topics[][MAX_TOPIC_NAME_LENGTH], int *num_topics) {
    int topic_index = -1;
    for (int i = 0; i < *num_topics; i++) {
        if (strcmp(topics[i], topic) == 0) {
            topic_index = i;
            break;
        }
    }

    if (topic_index == -1) {
        // topic does not exist, create new topic
        topic_index = *num_topics;
        strcpy(topics[topic_index], topic);
        (*num_topics)++;
    }

    subscriptions[client_id-1][topic_index] = true;
}