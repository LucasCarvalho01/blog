#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "blogoperation.h"
#include "common.h"

#define NEW_CONNECTION 1
#define NEW_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE_TOPIC 4
#define DISCONNECT 5
#define UNSUBSCRIBE_TOPIC 6

#define INVALID_COMMAND -1

int takeInput(struct BlogOperation *operation);
int getClientAction(char *input);
void printError(int errorCode);
void printOperation(struct BlogOperation operation);

int main(int argc, char **argv)
{
  char *ip = argv[1];
  char *port = argv[2];
  int actionCode;
  int count;
  int id = 0;
  bool connected = false;

  struct BlogOperation operation;

  struct sockaddr_storage storage;
  addrparse(ip, port, &storage);

  struct sockaddr *addr = (struct sockaddr *)&storage;

  int s;
  s = socket(storage.ss_family, SOCK_STREAM, 0);
  if (s == -1)
  {
    exit(EXIT_FAILURE);
  }

  if (0 != connect(s, addr, sizeof(storage)))
  {
    exit(EXIT_FAILURE);
  }

  // client connected to server
  char addrstr[256];
  converterEnderecoEmString(addr, addrstr, 256);
  printf("connected to %s\n", addrstr);

  // listening to console commands
  while (true)
  {
    // if not connected, send new connection message
    if (connected == false)
    {
      operation.client_id = id;
      operation.operation_type = NEW_CONNECTION;
      operation.server_response = 0;
      strcpy(operation.topic, "");
      strcpy(operation.content, "");

      printf("enviando msg para primeira conexao\n");
      printOperation(operation);

      count = send(s, &operation, sizeof(operation), 0);
      if (count != sizeof(operation))
      {
        printf("error sending message\n");
        exit(EXIT_FAILURE);
      }

      // waiting for server response
      count = recv(s, &operation, sizeof(operation), 0);
      if (count != sizeof(operation))
      {
        printf("error receiving message\n");
        exit(EXIT_FAILURE);
      }

      printf("\nrecebida msg de primeira conexao\n");
      printOperation(operation);

      // if id has changed, connection was successful
      if (operation.client_id != 0)
      {
        printf("client %02d connected\n", operation.client_id);
        connected = true;
        id = operation.client_id;
      }
      else
      {
        printf("no available client ids\n");
        exit(EXIT_FAILURE);
      }
      continue;
    }

    actionCode = takeInput(&operation);

    // in case of invalid command
    if (actionCode < 0)
    {
      printError(actionCode);
      continue;
    }

    // in case of new post, get content from command line
    if (actionCode == NEW_POST)
    {
      char content[2048];
      fgets(content, 2048, stdin);
      strcpy(operation.content, content);
    }

    operation.client_id = id;
    operation.operation_type = actionCode;
    operation.server_response = 0;

    printf("\nenviando nova msg para server\n");
    printOperation(operation);

    // sending message
    count = send(s, &operation, sizeof(operation), 0);
    if (count != sizeof(operation))
    {
      printf("error sending message\n");
      exit(EXIT_FAILURE);
    }

    if (actionCode == DISCONNECT)
    {
      printf("client %02d was disconnected\n", operation.client_id);
      break;
    }

    // waiting for server response
    count = recv(s, &operation, sizeof(operation), 0);
    if (count != sizeof(operation))
    {
      printf("error receiving message\n");
      exit(EXIT_FAILURE);
    }

    printf("\nrecebida nova msg from server\n");
    printOperation(operation);
  }

  close(s);
  return 0;
}

int takeInput(struct BlogOperation *operation)
{
  char input[128], command[64];
  char *token;
  char delim[] = " ";
  int intAction;

  // Read command
  fgets(input, 128, stdin);
  token = strtok(input, delim);
  strcpy(command, token);

  intAction = getClientAction(command);

  // If it passes topic name, stores it
  token = strtok(NULL, delim);

  // handles subscribe and unsubscribe commands
  if (token != NULL && (intAction == SUBSCRIBE_TOPIC || intAction == UNSUBSCRIBE_TOPIC))
  {
    strcpy(operation->topic, token);
  }
  // handles publish command, get in which topic will be posted
  else if (intAction == NEW_POST)
  {
    token = strtok(NULL, delim);
    if (token != NULL)
    {
      strcpy(operation->topic, token);
    }
  }
  // others actions dont need topic nor content
  else
  {
    strcpy(operation->topic, "");
    strcpy(operation->content, "");
  }

  return intAction;
}

// Get string action and convert to specified int
int getClientAction(char *input)
{
  if (!strncmp(input, "publish", 7))
    return NEW_POST;
  if (!strncmp(input, "subscribe", 9))
    return SUBSCRIBE_TOPIC;
  if (!strncmp(input, "unsubscribe", 11))
    return UNSUBSCRIBE_TOPIC;
  if (!strncmp(input, "list", 4))
    return LIST_TOPICS;
  if (!strncmp(input, "exit", 4))
    return DISCONNECT;
  else
    return INVALID_COMMAND;
}

void printError(int errorCode)
{
  char error[64] = "";

  switch (errorCode)
  {
  case INVALID_COMMAND:
    strcpy(error, "error: command not found");
    break;

  default:
    break;
  }

  printf("%s\n", error);
}

void printOperation(struct BlogOperation operation)
{
  printf("client_id: %d\n", operation.client_id);
  printf("operation_type: %d\n", operation.operation_type);
  printf("server_response: %d\n", operation.server_response);
  printf("topic: %s\n", operation.topic);
  printf("content: %s\n", operation.content);
}