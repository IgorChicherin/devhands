#include "views.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

Response make_response(const char *path, int status_code, const char *method,
                       int sockfd) {
  Response resp;
  strcpy(resp.path, path);
  strcpy(resp.method, method);
  resp.status_code = status_code;
  resp.sockfd = sockfd;
  return resp;
}

void send_response(int sockfd, const char *data) {
  // Write to the socket
  int valwrite = write(sockfd, data, strlen(data));
  if (valwrite < 0) {
    perror("webserver (write)");
  }
}
void render_string(Response resp, const char *html_string, char *data) {

  sprintf(data,
          "HTTP/1.1 %d OK\r\n"
          "Server: webserver-c\r\n"
          "Content-type: text/html\r\n\r\n"
          "%s\r\n",
          resp.status_code, html_string);
}

char *parse_url_path(char *path) {
  char *splited = NULL;

  splited = strtok(path, "/");

  if (splited != NULL) {
    return splited;
  }

  splited = strtok(NULL, "/");
  return splited;
}

void add_handler(View *view, char *method, HandlerFunc func) {
  Handler handler = {method, func};
  view->handlers[view->handlers_count++] = handler;
}

void register_view(ViewsList *views, View view) {
  views->views[views->views_count++] = view;
}

void hello_handler_func(Request *request) {
  Response resp =
      make_response(request->path, 200, request->method, request->sockfd);
  char data[BUFFER_SIZE];
  render_string(resp, "<html>hello, world</html>", data);
  send_response(resp.sockfd, data);
}

Handler hello_handler = {
    "GET",
    hello_handler_func,
};

void cpu_bound_handler_func(Request *request) {
  Response resp =
      make_response(request->path, 200, request->method, request->sockfd);

  char *response_string_tmpl = "<html>cpu-bound completed %s</html>";

  char *d = parse_url_path(request->path);
  printf("%s\n", d);

  char data[BUFFER_SIZE];
  render_string(resp, response_string_tmpl, data);
  send_response(resp.sockfd, data);
}

Handler cpu_bound_handler = {
    "GET",
    cpu_bound_handler_func,
};
