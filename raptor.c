#include <stdio.h>
#include <unistd.h>
#include "dbg.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define PORT 7899    // port to bind to
#define BACKLOG 10   // max connections
#define BUFSIZE 1024
#define LOCALHOST "127.0.0.1"


int run_server()
{
    int sockfd, newfd;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct sockaddr_in servaddr; 
    
    char str[100];
    int listenfd; 
    int comm_fd;
    int optval;
    int status;

    printf("Hello -> Welcome to Velociraptor\n");

    // create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    check(listenfd > 1, "Failed to open socket.");
  
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(7899);

    // don't wanna wait 20 secods
    optval = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
            (const void *)&optval, sizeof(int));

    // bind that shit
    bind(listenfd,(struct sockaddr *) &servaddr, sizeof(servaddr));
    
    listen(listenfd, 10);

    comm_fd = accept(listenfd, (struct sockaddr*) NULL, NULL);

    while(1)
    {
        bzero( str, 100);
        read(comm_fd,str,100);
        printf("Echoing back - %s",str);
        write(comm_fd, str, strlen(str)+1);
    } 
    
    
    return 1;

error:
    return -1;
}

int main(int argc, char *argv[])
{
    run_server();

    return 1;
error:
    return -1;
}
