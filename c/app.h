#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct SockConn {
  int len;
  struct sockaddr_in addr;
};

struct Connection {
  int sockfd;
  // struct sockaddr_in host_addr;
  // struct sockhaddr_in client_addr;
  SockConn host_sock;
  SockConn client_sock;
};

void main(void);
int create_socket(Connection *conn);
int listen(int sockfd);
void serve(Connection *conn);
void listen_and_serve(Connection *conn);

struct Server {};
