#include "app.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>

char buffer[BUFFER_SIZE];
char resp[] = "HTTP/1.0 200 OK\r\n"
              "Server: webserver-c\r\n"
              "Content-type: text/html\r\n\r\n"
              "<html>hello, world</html>\r\n";

struct Server server_constructor(int domain, int service, int protocol,
                                 u_long interface, int port, int backlog,
                                 void (*launch)(struct Server *server)) {
  struct Server server;

  server.domain = domain;
  server.service = service;
  server.protocol = protocol;
  server.interface = interface;
  server.port = port;
  server.backlog = backlog;

  // binding address
  server.address.sin_family = domain;
  server.address.sin_addr.s_addr = htonl(interface);
  server.address.sin_port = htons(port);

  // Create a socket
  server.socket = socket(domain, service, protocol);

  if (server.socket == 0) {
    perror("Failed to create socket");
    exit(1);
  }

  // Binding socket with address
  int res = bind(server.socket, (struct sockaddr *)&server.address,
                 sizeof(server.address));

  if (res < 0) {
    perror("Failed to bind socket");
    exit(1);
  }

  // Listen connections
  res = listen(server.socket, server.backlog);

  if (res < 0) {
    perror("Failed to listen to socket");
    exit(1);
  }

  server.launch = launch;

  return server;
}

void serve(struct Server *server) {
  for (;;) {
    // Accept incoming connections
    int host_len = sizeof(server->address);
    int newsockfd = accept(server->socket, (struct sockaddr *)&server->address,
                           (socklen_t *)&host_len);

    if (newsockfd < 0) {
      perror("webserver (accept)");
      continue;
    }
    printf("connection accepted\n");

    // Get client address
    struct sockaddr_in client;
    int client_len = sizeof(client);
    int sockn = getsockname(newsockfd, (struct sockaddr *)&client,
                            (socklen_t *)&client_len);

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

    printf("[%s:%u] %s %s %s\n", inet_ntoa(client.sin_addr),
           ntohs(client.sin_port), method, version, uri);

    // Write to the socket
    int valwrite = write(newsockfd, resp, strlen(resp));
    if (valwrite < 0) {
      perror("webserver (write)");
      continue;
    }

    close(newsockfd);
  }
}

int main() {
  struct Server server = server_constructor(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                                            INADDR_ANY, 8080, 1, serve);
  server.launch(&server);
}
