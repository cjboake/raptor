#ifndef _statserve_h
#define _statserve_h

int run_server();
int server_listen(const char *host, const char *port);
int attempt_listen(struct addrinfo *info);

#endif
