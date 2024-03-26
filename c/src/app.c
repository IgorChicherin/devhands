#include "app.h"
#include "server.c"
#include "views.c"

int main() {
  View hello_view = {"/hello", 0};
  add_handler(&hello_view, "GET", hello_handler_func);

  View cpu_bound_view = {"/cpu/%s/%s", 0};
  add_handler(&cpu_bound_view, "GET", cpu_bound_handler_func);

  ViewsList views = {0};
  register_view(&views, hello_view);
  register_view(&views, cpu_bound_view);

  Server server = server_constructor(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                                     INADDR_ANY, 8080, 1, serve, &views);
  server.launch(&server);
}
