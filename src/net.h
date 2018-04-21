#ifndef _net_h
#define _net_h

#include "./lib/ringbuffer.h"

int read_some(RingBuffer * buffer, int fd, int is_socket);
int write_some(RingBuffer * buffer, int fd, int is_socket);
int parse_line(RingBuffer *recv_rb, RingBuffer *send_rb);

#endif
