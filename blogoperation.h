#ifndef BLOGOPERATION_H
#define BLOGOPERATION_H

struct BlogOperation {
  int client_id;
  int operation_type;
  int server_response;
  char topic[50];
  char content[2048];
};

#endif