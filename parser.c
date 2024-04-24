#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct header {
  char *field;
  char *value;
};

struct request {
  char *type;
  char *url;
  char *version;
  struct header **headers;
  char *content;
};

void parse_http(char *request_data) {
  struct request request;
  char *lineptr = NULL;
  char *line = NULL;

  request.type = strtok_r(request_data, " ", &lineptr);
  request.url = strtok_r(NULL, " ", &lineptr);
  request.version = strtok_r(NULL, "\r\n", &lineptr);

  printf("Type: %s\nURL: %s\nVersion: %s\n", request.type, request.url,
         request.version);
  do {
    line = strtok_r(NULL, "\r\n", &lineptr);
    printf("%s\n", line);
  } while (line != NULL);
}

int main(void) {
  char *ip = "127.0.0.1";
  int port = 8080;
  int opt = 1;

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror(NULL);
    exit(1);
  }

  int r = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                     sizeof(opt));
  if (r) {
    perror(NULL);
    exit(1);
  }

  struct sockaddr_in addr;
  int addr_len = sizeof(addr);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);

  r = bind(fd, (struct sockaddr *)&addr, addr_len);
  if (r < 0) {
    perror(NULL);
    exit(1);
  }

  r = listen(fd, 5);
  if (r < 0) {
    perror(NULL);
    exit(1);
  }

  printf("Listening on %s:%d\n", ip, port);

  struct sockaddr_in client_addr;
  unsigned int client_addr_len = sizeof(client_addr);

  int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd < 0) {
    perror(NULL);
    exit(1);
  }

  char *buffer = malloc(0x10000 * sizeof(char));
  if (buffer == NULL) {
    perror(NULL);
    exit(1);
  }

  ssize_t nread = recv(client_fd, buffer, 0xFFFF, 0);
  printf("read %zd bytes\n", nread);
  printf("%s\n", buffer);

  write(client_fd, "HTTP/1.1 204 No Content\r\nConnection: Close\r\n", 45);

  close(client_fd);
  close(fd);

  parse_http(buffer);

  free(buffer);
  return 0;
}
