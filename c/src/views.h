// char resp[] = "HTTP/1.0 200 OK\r\n"
//               "Server: webserver-c\r\n"
//               "Content-type: text/html\r\n\r\n"
//               "<html>hello, world</html>\r\n";

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif // !BUFFER_SIZE

typedef struct Request {
  char path[BUFFER_SIZE];
  char method[25];
  int sockfd;
} Request;

typedef struct Response {
  char path[BUFFER_SIZE];
  char method[25];
} Response;

typedef void (*HandlerFunc)(Request *request);

typedef struct Handler {
  char method[25];
  HandlerFunc func;
} Handler;

typedef struct View {
  char path[BUFFER_SIZE];
  char template_name[255];
  Handler handlers[255];
} View;

Response make_response(const char *path, const char *method);

char *render_template(Response resp, const char *template_name);

char *render_string(Response resp, const char *html_string);

// void (*create_handler(HandlerFunc))(Request *);

// void create_handler(HandlerFunc func);
