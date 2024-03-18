#include "app.h"
#include <netinet/in.h>

char buffer[BUFFER_SIZE];
char resp[] = "HTTP/1.0 200 OK\r\n"
              "Server: webserver-c\r\n"
              "Content-type: text/html\r\n\r\n"
              "<html>hello, world</html>\r\n";

int create_socket(struct Connection *conn) {
  // Create a socket
  conn->sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (conn->sockfd == -1) {
    perror("webserver (socket)");
  }

  printf("socket created successfully\n");
  // Create the address to bind the socket to

  struct sockaddr_in soc_host = {};
  struct SockConn host = {0, soc_host};
  host.len = sizeof(host.addr);
  host.addr.sin_family = AF_INET;
  host.addr.sin_port = htons(PORT);
  host.addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Create client address

  struct sockaddr_in soc_client = {};
  struct SockConn client = {0, soc_client};
  client.len = sizeof(client.addr);

  // Bind the socket to the address
  if (bind(conn->sockfd, (struct sockaddr *)&host.addr, host.len) != 0) {
    perror("webserver (bind)");
    return -1;
  }
  printf("socket successfully bound to address\n");

  conn->host_sock = host;
  conn->client_sock = client;
  return 0;
}

int server_listen(int sockfd) {
  // Listen for incoming connections
  if (listen(sockfd, SOMAXCONN) != 0) {
    perror("webserver (listen)");
    return 1;
  }
  printf("server listening for connections\n");
  return 0;
}

void serve(struct Connection *conn) {
  for (;;) {
    // Accept incoming connections
    struct SockConn host = conn->host_sock;
    int newsockfd = accept(conn->sockfd, (struct sockaddr *)&host.addr,
                           (socklen_t *)&host.len);

    if (newsockfd < 0) {
      perror("webserver (accept)");
      continue;
    }
    printf("connection accepted\n");

    // Get client address

    struct SockConn client = conn->client_sock;
    int sockn = getsockname(newsockfd, (struct sockaddr *)&client.addr,
                            (socklen_t *)&client.len);

    if (sockn < 0) {
      perror("webserver (getsockname)");
      continue;
    }

    // Read from the socket
    int valread = read(newsockfd, buffer, BUFFER_SIZE);
    if (valread < 0) {
      perror("webserver (read)");
      continue;
    }

    // Read the request
    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);

    printf("[%s:%u] %s %s %s\n", inet_ntoa(client.addr.sin_addr),
           ntohs(client.addr.sin_port), method, version, uri);

    // Write to the socket
    int valwrite = write(newsockfd, resp, strlen(resp));
    if (valwrite < 0) {
      perror("webserver (write)");
      continue;
    }

    close(newsockfd);
  }
}

void listen_and_serve(struct Connection *conn) {
  server_listen(conn->sockfd);
  serve(conn);
}

int main() {
  struct Connection conn;
  create_socket(&conn);
  listen_and_serve(&conn);
}
