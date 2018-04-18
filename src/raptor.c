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

#define PORT 7899    // port to bind to
#define BACKLOG 10   // max connections
#define BUFSIZE 1024
#define LOCALHOST "127.0.0.1"

void handle_sigchild(int sig) {
    sig = 0; // ignore it
    while(waitpid(-1, NULL, WNOHANG) > 0) {
    }
}

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

void client_handler(char str[], int comm_fd)
{
    bzero( str, 100);
    read( comm_fd, str, 100);
    printf("Echoing back - %s",str);
    write(comm_fd, str, strlen(str)+1);
}

int run_server()
{
    struct sigaction sa = {
        .sa_handler = handle_sigchild,
        .sa_flags = SA_RESTART | SA_NOCLDSTOP
    };

    int rc = 0;
    int sockfd;
  
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);

    const char *ho = "127.0.0.1";
    const char *po = "7899";

    char str[100];
    int comm_fd;

    printf("Hello -> Welcome to Velociraptor\n");

    // create a sigaction that handles SIGCHLD
    sigemptyset(&sa.sa_mask);
    rc = sigaction(SIGCHLD, &sa, 0);
    check(rc != -1, "Failed to setup signal handler for child processes.");

    sockfd = server_listen(ho, po); 

    while(1)
    {
        comm_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        check(comm_fd >= 0, "Failed to accept connection.");

        debug("-> Client Connection Made <-");

        rc = fork();
        if(rc == 0) {
            // child process
            close(sockfd);
            // handle the client
            client_handler(str, comm_fd);
            exit(0);
        } else {
            //server process
            close(comm_fd);
        }
    } 
    
    return 1;

error:
    return -1;
}
