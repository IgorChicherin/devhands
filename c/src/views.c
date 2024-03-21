#include "views.h"
#include <string.h>

Response make_response(const char *path, const char *method) {
  Response resp;
  strcpy(resp.path, path);
  strcpy(resp.method, method);
  return resp;
}

// void (*create_handler(HandlerFunc))(Request *) {
//    handler_func(request);
// }
