#include "app.h"

char buffer[BUFFER_SIZE];
char resp[] = "HTTP/1.0 200 OK\r\n"
              "Server: webserver-c\r\n"
              "Content-type: text/html\r\n\r\n"
              "<html>hello, world</html>\r\n";

void create_socket(struct Connection *conn) {
  // Create a socket
  &conn->sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (conn.sockfd == -1) {
    perror("webserver (socket)");
    return -1;
  }

  printf("socket created successfully\n");
  // Create the address to bind the socket to

  SockConn host = {0, srtuct sockaddr_in};
  host.len = sizeof(host.addr);
  host.addr.sin_family = AF_INET;
  host.addr.sin_port = htons(PORT);
  host.addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Create client address
  SockConn client = {0, srtuct sockaddr_in};
  client.len = sizeof(client.addr);

  // Bind the socket to the address
  if (bind(conn.sockfd, (struct sockaddr *)&host.addr, host.len) != 0) {
    perror("webserver (bind)");
    return -1;
  }
  printf("socket successfully bound to address\n");

  conn.host = host;
  conn.client = client;
}

void listen(int sockfd) {
  // Listen for incoming connections
  if (listen(sockfd, SOMAXCONN) != 0) {
    perror("webserver (listen)");
    return 1;
  }
  printf("server listening for connections\n");
  return 0;
}

void serve(Connection *conn) {
  for (;;) {
    // Accept incoming connections
    SockCoonn host = conn.host;
    int newsockfd = accept(conn.sockfd, (struct sockaddr *)&host.addr,
                           (socklen_t *)&host.len);

    if (newsockfd < 0) {
      perror("webserver (accept)");
      continue;
    }
    printf("connection accepted\n");

    // Get client address

    SockClient client = conn.client;
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
    delete client;
    delete host;
  }
}

void listen_and_serve(Connection *conn) {
  listen(conn.sockfd);
  serve(conn);
}

void main() {
  Connection conn;
  create_socket(&conn);
  listen_and_serve(&conn);
}
