#ifndef BLOGOPERATION_H
#define BLOGOPERATION_H

#define NEW_CONNECTION 1
#define NEW_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE_TOPIC 4
#define DISCONNECT 5
#define UNSUBSCRIBE_TOPIC 6

struct BlogOperation
{
  int client_id;
  int operation_type;
  int server_response;
  char topic[50];
  char content[2048];
};

#endif

// NEW_CONNECTION 1
// NEW_POST 2
// LIST_TOPICS 3
// SUBSCRIBE_TOPIC 4
// DISCONNECT 5
// UNSUBSCRIBE_TOPIC 6
