#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "views.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif // !BUFFER_SIZE

typedef struct Server {
  int domain;
  int service;
  int protocol;
  u_long interface;
  int port;
  int backlog;
  struct sockaddr_in address;
  int socket;
  void (*launch)(struct Server *server);
  int views_count;
  ViewsList views;
} Server;

Server server_constructor(int domain, int service, int protocol,
                          u_long interface, int port, int backlog,
                          void (*launch)(Server *server), ViewsList *views);
