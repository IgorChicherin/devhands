#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

struct SockConn {
  int len;
  struct sockaddr_in addr;
};

struct Connection {
  int sockfd;
  // struct sockaddr_in host_addr;
  // struct sockhaddr_in client_addr;
  struct SockConn host_sock;
  struct SockConn client_sock;
};

int main(void);

struct Server {
  int domain;
  int service;
  int protocol;
  u_long interface;
  int port;
  int backlog;
  struct sockaddr_in address;
  int socket;
  void (*launch)(struct Server *server);
};

struct Server server_constructor(int domain, int service, int protocol,
                                 u_long interface, int port, int backlog,
                                 void (*launch)(struct Server *server));
