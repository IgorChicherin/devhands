#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif // !BUFFER_SIZE

#ifndef VIEWS_H
#define VIEWS_H

typedef struct Request {
  char path[BUFFER_SIZE];
  char method[25];
  int sockfd;
} Request;

typedef void (*HandlerFunc)(Request *request);

typedef struct Response {
  char path[BUFFER_SIZE];
  char method[25];
  int status_code;
  int sockfd;
} Response;

typedef struct Handler {
  char *method;
  HandlerFunc func;
} Handler;

typedef struct View {
  char path[BUFFER_SIZE];
  int handlers_count;
  Handler handlers[255];
} View;

typedef struct ViewsList {
  int views_count;
  View views[255];
} ViewsList;

Response make_response(const char *path, int status_code, const char *method,
                       int sockfd);

void register_view(ViewsList *views, View view);

void add_handler(View *view, char *method, HandlerFunc func);

void render_string(Response resp, const char *html_string, char *data);

void send_response(int sockfd, const char *data);

char *parse_url_path(char *path);

#endif // !VIEWS_H
