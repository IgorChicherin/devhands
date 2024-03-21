#include "app.h"
#include "server.c"

void handler_func(Request *request) {}

Handler hello_handler = {
    "GET",
    handler_func,
};

int main() {
  View hello_view = {"hello", "hello", hello_handler};
  View views[1] = {hello_view};

  struct Server server = server_constructor(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                                            INADDR_ANY, 8080, 1, serve, views);
  server.launch(&server);
}
