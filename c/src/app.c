#include "app.h"
#include "server.c"
#include "views.c"

int main() {
  View hello_view = {"/c/hello-world", 0};
  add_handler(&hello_view, "GET", hello_handler_func);

  View cpu_bound_view = {"/cpu/%s/%s", 0};
  add_handler(&cpu_bound_view, "GET", cpu_bound_handler_func);

  ViewsList views = {0};
  register_view(&views, hello_view);
  register_view(&views, cpu_bound_view);

  Server server = server_constructor(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                                     INADDR_ANY, 3004, 1, serve, &views);

  // server.launch(&server);

  for (int i = 0; i < CPU_CORES; ++i) {
    int id = fork();
    if (id != 0) {
      server.launch(&server);
    }
  }
  
  close(server.epfd);
  close(server.socket);
}
