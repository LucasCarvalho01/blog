#ifndef BLOG_H
#define BLOG_H  

#define MAX_TOPICS 20
#define MAX_TOPIC_NAME_LENGTH 50

void printTopics(char topics[][MAX_TOPIC_NAME_LENGTH], int num_topics);
void setResponse(struct BlogOperation *response, int client_id, int operation, int isServerResponse, char *topic, char* content);
void subscribeToTopic(char topic[], int client_id, bool subscriptions[][MAX_TOPICS], char topics[][MAX_TOPIC_NAME_LENGTH], int *num_topics);

#endif