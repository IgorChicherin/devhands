#include <arpa/inet.h>
#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
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
  struct SockConn host_sock;
  struct SockConn client_sock;
};

int main(void);
int create_socket(struct Connection *conn);
int server_listen(int sockfd);
void serve(struct Connection *conn);
void listen_and_serve(struct Connection *conn);

struct Server {};
