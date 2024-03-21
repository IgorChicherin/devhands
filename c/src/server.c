#include "server.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>

char buffer[BUFFER_SIZE];

Server server_constructor(int domain, int service, int protocol,
                          u_long interface, int port, int backlog,
                          void (*launch)(Server *server), View *views) {
  Server server;

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
  memcpy(&server.views, views, sizeof(View) * 1);
  return server;
}

void serve(Server *server) {
  for (;;) {
    // Accept incoming connections
    int host_len = sizeof(server->address);
    int newsockfd = accept(server->socket, (struct sockaddr *)&server->address,
                           (socklen_t *)&host_len);

    if (newsockfd < 0) {
      perror("webserver (accept)");
      continue;
    }

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
      continue;
    }

    // Read the request
    Request req;
    req.sockfd = newsockfd;

    char version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", req.method, req.path, version);

    printf("%s:%u [%s] %s %s\n", inet_ntoa(client.sin_addr),
           ntohs(client.sin_port), req.method, version, req.path);

    for (int i = 0; i < 1; i++) {
      View view = server->views[i];

      if (strcmp(req.path, view.path) == 0) {
        view.handlers[0].func(&req);
        printf("served\n");
      } else {
        char resp_404[] = "HTTP/1.1 404 Not Found\r\n"
                          "Server: webserver-c\r\n"
                          "Content-type: text/html\r\n\r\n"
                          "<html>Page not found</html>\r\n";
        int valwrite = write(req.sockfd, resp_404, strlen(resp_404));
        printf("404\n");
        if (valwrite < 0) {
          perror("webserver (write)");
        }
      }
    }

    close(newsockfd);
  }
}