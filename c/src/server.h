#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "views.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif // !BUFFER_SIZE

#ifndef CPU_CORES
#define CPU_CORES 8
#endif // !CPU_CORES

#ifndef MAX_CONN
#define MAX_CONN 512
#endif // !MAX_CONN

#ifndef MAX_EVENTS
#define MAX_EVENTS 1024 * MAX_CONN 
#endif // !MAX_EVENTS

int setnonblocking(int sockfd);
void epoll_ctl_add(int epfd, int fd, uint32_t events);

typedef struct Server {
  int domain;
  int service;
  int protocol;
  __u_long interface;
  int port;
  int backlog;
  struct sockaddr_in address;
  int socket;
  void (*launch)(struct Server *server);
  int views_count;
  int epfd;
  struct epoll_event *events;
  ViewsList views;
} Server;

Server server_constructor(int domain, int service, int protocol,
                          __u_long interface, int port, int backlog,
                          void (*launch)(Server *server), ViewsList *views);

void route_view(Server *server, Request *req);
int handle_conn(Server *server);