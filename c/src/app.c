#include "app.h"
#include "server.c"

void handler_func(Request *request) {
  // Write to the socket

  char resp[] = "HTTP/1.1 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hello, world</html>\r\n";

  int valwrite = write(request->sockfd, resp, strlen(resp));
  if (valwrite < 0) {
    perror("webserver (write)");
  }
}

Handler hello_handler = {
    "GET",
    handler_func,
};

int main() {
  View hello_view = {"/hello", "hello", hello_handler};
  View views[1] = {hello_view};

  Server server = server_constructor(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                                     INADDR_ANY, 8080, 1, serve, views);
  server.launch(&server);
}
