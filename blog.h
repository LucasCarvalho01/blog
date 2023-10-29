#ifndef BLOG_H
#define BLOG_H

#include "blogoperation.h"

#define MAX_TOPICS 20
#define MAX_TOPIC_NAME_LENGTH 50
#define MAX_CLIENTS 10

void printTopics(char topics[][MAX_TOPIC_NAME_LENGTH], int num_topics);
void setResponse(struct BlogOperation *response, int client_id, int operation, int isServerResponse, char *topic, char *content);
void createNewPost(struct BlogOperation blog_operation, bool subscriptions[][MAX_TOPICS], char topics[][MAX_TOPIC_NAME_LENGTH], int num_topics);
void subscribeToTopic(char topic[], int client_id, bool subscriptions[][MAX_TOPICS], char topics[][MAX_TOPIC_NAME_LENGTH], int *num_topics);
void unsubscribeToTopic(int client_id, char *topic_name, bool subscriptions[MAX_CLIENTS][MAX_TOPICS], char topics[MAX_TOPICS][MAX_TOPIC_NAME_LENGTH], int num_topics);
void unsubscribeAllTopics(int client_id, bool subscriptions[MAX_CLIENTS][MAX_TOPICS], int num_topics);

#endif