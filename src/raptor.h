#ifndef _statserve_h
#define _statserve_h

#include "./lib/ringbuffer.h"

int run_server();
int server_listen(const char *host, const char *port);
int attempt_listen(struct addrinfo *info);
void handle_sigchild(int sig);
void client_handler(int comm_fd);

#endif
