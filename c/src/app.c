#include "app.h"
#include "server.c"
#include "views.c"

int main() {
  View hello_view = {"/hello", 0};
  add_handler(&hello_view, "GET", hello_handler_func);

  ViewsList views = {0};
  register_view(&views, hello_view);

  Server server = server_constructor(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                                     INADDR_ANY, 8080, 1, serve, &views);
  server.launch(&server);
}
