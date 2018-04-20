#ifndef _statserve_h
#define _statserve_h

#include "./lib/ringbuffer.h"

int read_some(RingBuffer * buffer, int fd, int is_socket);
int write_some(RingBuffer * buffer, int fd, int is_socket);
int run_server();
int server_listen(const char *host, const char *port);
int attempt_listen(struct addrinfo *info);

#endif
