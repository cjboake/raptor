#include "net.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "dbg.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "raptor.h"
#include "net.h"
#include "./lib/ringbuffer.h"

// look up: send(), recv(), write()
int read_some(RingBuffer * buffer, int fd, int is_socket)
{
    int rc = 0;

    // Check the validity/contents of the data structure
    if(RingBuffer_available_data(buffer) == 0) {
        buffer->start = buffer->end = 0;
    }

    // Check the validity/contents of the data structure
    if(is_socket) {
        rc = recv(fd, RingBuffer_starts_at(buffer), 
                RingBuffer_available_space(buffer), 0);        
    }
    
    bstring data = NULL;
    
    check_debug(RingBuffer_gets != NULL, "The ringbuffer read_some is null.\n");

    data = RingBuffer_gets(buffer, 0);
   
    printf("Raptor echo -> ");
    
    check(rc >= 0, "Ringbuffer read failed.");

    // save to the data data structure
    RingBuffer_commit_write(buffer, rc);
    check(RingBuffer_empty(buffer) != 0, "The ringbuffer is empty.\n");   

    return rc;
error:
    return -1;
}

int write_some(RingBuffer * buffer, int fd, int is_socket)
{
    int rc = 0;
    // take in the data structure
    bstring data = RingBuffer_get_all(buffer);
    
    // write that shit - Note extra param in send
    if (is_socket) {
        rc = send(fd, bdata(data), blength(data), 0);
    } else {
        rc = write(fd, bdata(data), blength(data));
    }
    bdestroy(data);
    return rc;
error:
    return -1;
}

int parse_line(RingBuffer *recv_rb, RingBuffer *send_rb)
{
    int rc = 0;
    bstring data = NULL;
    data = RingBuffer_gets(recv_rb, RingBuffer_available_data(recv_rb));
   
    //now fucking write it to the send_rb
    rc = RingBuffer_write(send_rb, bdata(data), blength(data));

    return rc;
error:
    return -1;

}
