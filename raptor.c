#include <stdio.h>
#include <unistd.h>
#include "dbg.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "raptor.h"

#define PORT 7899    // port to bind to
#define BACKLOG 10   // max connections
#define BUFSIZE 1024
#define LOCALHOST "127.0.0.1"

int attempt_listen(struct addrinfo *info)
{
    int sockfd = -1; // default fail
    int rc = -1;
    int yes = 1;

    check(info != NULL, "Invalid addrinfo");

    sockfd = socket(info->ai_family, info->ai_socktype, 
            info->ai_protocol);
    check_debug(sockfd != -1, "Failed to bind to address. Trying more.");

    //set the SO_REUSEADDR
    rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    check_debug(rc == 0, "Failed to set SO_REUSADDR");

    //try and bind to it
    rc = bind(sockfd, info->ai_addr, info->ai_addrlen);
    check_debug(rc == 0, "Failed to bind to socket.");

    //now listen with a backlog
    rc = listen(sockfd, BACKLOG);
    check_debug(rc == 0, "Failed to listen to socket.");

    return sockfd;

error:
    return -1;
}

int server_listen(const char *host, const char *port)
{
    int rc = 0;
    int sockfd = -1;
    struct addrinfo *info = NULL;
    struct addrinfo *next_p = NULL;
    struct addrinfo addr = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE
    };

    check(host != NULL, "Invalid host.");
    check(port != NULL, "Invalid port.");

    //get the address info for the host and the port
    rc = getaddrinfo(NULL, port, &addr, &info);

    //cycle through the available list to find one
    for(next_p = info; next_p != NULL; next_p = next_p->ai_next)
    {
        // attempt to listen to each one
        sockfd = attempt_listen(next_p);
        if(sockfd != -1) break;
    }

    // either we found one and were able to listen or nothing.
    check(sockfd != -1, "All possible addresses failed.");

error:
    if (info) freeaddrinfo(info);

    return sockfd;
}

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
    servaddr.sin_port = htons(PORT);

    // don't wanna wait 20 secods
    optval = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
            (const void *)&optval, sizeof(int));

    // bind that shit
    bind(listenfd,(struct sockaddr *) &servaddr, sizeof(servaddr));
    
    listen(listenfd, 10);

    comm_fd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    check(comm_fd >= 0, "Failed to accept connection.");

    debug("-> Client connected <-");

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
