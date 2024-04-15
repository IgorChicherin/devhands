#include <netinet/in.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include "server.h"

char buffer[BUFFER_SIZE];

int setnonblocking(int sockfd) {
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) ==-1) {
		return -1;
	}
	return 0;
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
                          u_long interface, int port, int backlog,
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
  res = listen(server.socket, server.backlog);

  if (res < 0) {
    perror("Failed to listen to socket");
    exit(1);
  }

 // create epoll instance
  server.epfd = epoll_create(5);
  if (server.epfd == -1) {
      perror("epoll_create");
  }

  struct epoll_event ev;
  memcpy(&ev.events, server.events, sizeof(server.events));
	ev.data.fd = server.epfd;

  epoll_ctl_add(server.epfd, server.socket, EPOLLIN | EPOLLOUT | EPOLLET);
  server.launch = launch;

  memcpy(&server.views, views, sizeof(ViewsList));
  return server;
}

void serve(Server *server) {
  
  for (;;) {

    int count_fds = epoll_wait(server->epfd, server->events, MAX_EVENTS, -1);

    for (int i = 0 ; i < count_fds; ++i) {

      // Handle connection
      if (server->events[i].data.fd == server->socket) {
          // Accept incoming connections
          int host_len = sizeof(server->address);
          int newsockfd = accept(server->socket, (struct sockaddr *)&server->address,
                                (socklen_t *)&host_len);

          if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
          }

          setnonblocking(newsockfd);
          epoll_ctl_add(server->epfd, newsockfd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);
      }
      
      if (server->events[i].events & EPOLLIN) {
        // Read from the socket
        int valread = read(server->events[i].data.fd, buffer, BUFFER_SIZE);
        if (valread < 0) {
          continue;
        }


        // Get client address
        struct sockaddr_in client;
        int client_len = sizeof(client);
        int sockn = getsockname(server->events[i].data.fd, (struct sockaddr *)&client,
                                (socklen_t *)&client_len);

        if (sockn < 0) {
          perror("webserver (getsockname)");
          continue;
        }

        // Read the request
        Request req;
        req.sockfd = server->events[i].data.fd;

        char version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", req.method, req.path, version);

        printf("%s:%u [%s] %s %s\n", inet_ntoa(client.sin_addr),
              ntohs(client.sin_port), req.method, version, req.path);

        route_view(server, &req);
    
      }

      /* check if the connection is closing */
			if (server->events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
				printf("[+] connection closed\n");
				epoll_ctl(server->epfd, EPOLL_CTL_DEL, server->events[i].data.fd, NULL);
				close(server->events[i].data.fd);
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