#define _POSIX_C_SOURCE 1

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LOCALHOST_IP 0x7F000001
#define ROBOT_IP 0xC0A80014

int main(int argc, char **argv) {
  int sd, rv;
  struct sockaddr_in addr;
  const char *s;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(333);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  printf("socket()\n");
  sd = socket(PF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  printf("connect()\n");
  rv = connect(sd, (struct sockaddr *)&addr, sizeof(addr));
  if (rv < 0) {
    perror("connect()");
    exit(EXIT_FAILURE);
  }

  printf("write()\n");

  s = "Test123\n";
  rv = write(sd, s, sizeof(s));
  if (rv < 0) {
    perror("write()");
    exit(EXIT_FAILURE);
  }

  close(sd);
}
