#include <netinet/in.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include "server.h"

int setnonblocking(int sockfd) {
  int old_option = fcntl(sockfd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  return fcntl(sockfd, F_SETFL, new_option);
}

void epoll_ctl_add(int epfd, int fd, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_ctl()\n");
		exit(1);
	}
}

Server server_constructor(int domain, int service, int protocol,
                          __u_long interface, int port, int backlog,
                          void (*launch)(Server *server), ViewsList *views) {
  Server server;

  server.domain = domain;
  server.service = service;
  server.protocol = protocol;
  server.interface = interface;
  server.port = port;
  server.backlog = backlog;

  // binding address
  server.address.sin_family = domain;
  server.address.sin_addr.s_addr = htonl(interface);
  server.address.sin_port = htons(port);

  // Create a socket
  server.socket = socket(domain, service, protocol);

  if (server.socket == 0) {
    perror("Failed to create socket");
    exit(1);
  }

  
  // Allow reuse sockets
  int yes = 1;
  if (setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Setsockopt error");
        exit(1);
  } 

  // Binding socket with address
  int res = bind(server.socket, (struct sockaddr *)&server.address,
                 sizeof(server.address));

  if (res < 0) {
    perror("Failed to bind socket");
    exit(1);
  }

  // Set sock non blocking
  if (setnonblocking(server.socket) == -1) {
    perror("Failed set socket non-blocking mode");
    exit(-1);
  }

  // Listen connections
  res = listen(server.socket, MAX_CONN);

  if (res < 0) {
    perror("Failed to listen to socket");
    exit(1);
  }

 // create epoll instance
  server.epfd = epoll_create1(0);
  if (server.epfd == -1) {
      perror("epoll_create");
  }

  epoll_ctl_add(server.epfd, server.socket, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLHUP);

  server.events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_EVENTS);
  server.launch = launch;

  memcpy(&server.views, views, sizeof(ViewsList));
  return server;
}

void serve(Server *server) {
  
  for (;;) {

    int count_fds = epoll_wait(server->epfd, server->events, MAX_EVENTS, 0);

    for (int i = 0 ; i < count_fds; ++i) {
      struct epoll_event event = server->events[i];

      // Handle connection
      if (event.data.fd == server->socket) 
        if (handle_conn(server) < 0) continue;
      
      if (event.events & EPOLLIN) {
        char buffer[BUFFER_SIZE];

        // Read from the socket
        int valread = read(event.data.fd, buffer, BUFFER_SIZE);

        if (valread < 0) continue;

        // Get client address3004w
        struct sockaddr_in client;
        int client_len = sizeof(client);
        int sockn = getsockname(event.data.fd, (struct sockaddr *)&client,
                                (socklen_t *)&client_len);

        if (sockn < 0) {
          perror("webserver (getsockname)");
          // free(buffer);
          continue;
        }

        // Read the request
        Request req;
        req.sockfd = event.data.fd;

         char version[BUFFER_SIZE];
        
        sscanf(buffer, "%s %s %s", req.method, req.path, version);

        printf("%s:%u [%s] %s %s\n", inet_ntoa(client.sin_addr),
              ntohs(client.sin_port), req.method, version, req.path);
        
        // free(version);
        route_view(server, &req);
      
        epoll_ctl(server->epfd, EPOLL_CTL_DEL, event.data.fd, NULL);
				close(event.data.fd);
      }

      /* check if the connection is closing */
			if (event.events & (EPOLLRDHUP | EPOLLHUP)) {
				printf("[+] connection closed\n");
				epoll_ctl(server->epfd, EPOLL_CTL_DEL, event.data.fd, NULL);
				close(event.data.fd);
				continue;
			}
    }
  }
}

void route_view(Server *server, Request *req) {
      bool view_found = false;
        for (int i = 0; i < server->views.views_count && !view_found; i++) {
          View view = server->views.views[i];

          if (strcmp(req->path, view.path) == 0) {

            for (int j = 0; j < view.handlers_count; j++) {
              Handler handler = view.handlers[j];

              if (strcmp(req->method, handler.method) == 0) {
                view_found = true;
                handler.func(req);
                close(req->sockfd);
                break;
              }
            }
          }
        }     

      if (!view_found) {
      char resp_404[] = "HTTP/1.1 404 Not Found\r\n"
                        "Server: webserver-c\r\n"
                        "Content-type: text/html\r\n\r\n"
                        "<html>Page not found</html>\r\n";

      int valwrite = write(req->sockfd, resp_404, strlen(resp_404));
      if (valwrite < 0) {
        perror("webserver (write)");
      }
    }
}

int handle_conn(Server *server) {
  // Accept incoming connections
  int host_len = sizeof(server->address);
  int newsockfd = accept(server->socket, (struct sockaddr *)&server->address,
                        (socklen_t *)&host_len);

  if (newsockfd < 0) {
    perror("webserver (accept)");
    return -1;
  }
  
  if (setnonblocking(newsockfd) == -1) {
    perror("Failed set socket non-blocking mode");
    exit(-1);
  }

  epoll_ctl_add(server->epfd, newsockfd, EPOLLIN | EPOLLET | EPOLLONESHOT);

  return newsockfd;
}
