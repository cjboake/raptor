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
#include "./lib/hashmap.h"
#include "./lib/hashmap_algos.h"
#include "./lib/stats.h"
#include "./lib/list.h"
#include "./lib/tstree.h"

#define PORT 7899    // port to bind to
#define BACKLOG 10   // max connections
#define BUFSIZE 1024
#define LOCALHOST "127.0.0.1"

#define RB_SIZE 1024 * 10

//bstring node = Hashmap_get(map, blist->entry[1]);
 //   printf("This one is a longshot: %s\n", bdata(node));


void create_stat(Hashmap * map, struct bstrList *blist)
{
    printf("Here are the blist entries:\n");
    printf("Here are the blist entry 1: %s\n", bdata(blist->entry[0]));
    printf("Here are the blist entry 2: %s\n", bdata(blist->entry[1]));
    printf("Here are the blist entry 3: %s\n", bdata(blist->entry[2]));

    // need to save this to a hashmap
    Stats *myStat = Stats_create();
    
    int data = atoi(bdata(blist->entry[2]));
    printf("This is our data: %d\n", data); // such wow

    Hashmap_set(map, blist->entry[1], myStat);

    Stats *node = Hashmap_get(map, blist->entry[1]);
    printf("This one is a longshot: %lf\n", node->max);
}

int get_mean(struct bstrList *blist)
{
    printf("PRINTING: this is the get_mean\n");

    return 1;
}

int update_sample(struct bstrList *blist)
{
    printf("PRINTING: this is the update_sample\n");

    return 1;
}

int delete_sample(struct bstrList *bstr)
{
    printf("PRINTING: this is the delete_sample\n");

    return 1;
}

int dump(struct bstrList *bstr)
{
    printf("PRINTING: dump\n");

    return 1;
}

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

void add_routes(TSTree * tree, char **arr)
{
    int i = 0;

    for (i = 0; i < 6; i++) {
        TSTree_insert(tree,
                arr[i], 
                strlen(arr[i]), 
                arr[i]);
    }
}

struct bstrList *input_parser(RingBuffer *recv_rb)
{
    printf("In the input_parser.\n");
    bstring data = NULL;
    data = RingBuffer_gets(recv_rb, RingBuffer_available_data(recv_rb));
    
    bstring a = bfromcstr("\0"); 

    struct bstrList *bist = bsplit(data, ' ');
    
    printf("These bstrings... %s\n", bdata(bist->entry[0]));    
    
    return bist;
}

void url_router(Hashmap * map, struct bstrList *blist)
{
    bstring a, b, c, d, e;
    bstring url = blist->entry[0];

    printf("Made it into url_router: %s\n", bdata(blist->entry[0]));

    a = bfromcstr("/create"); 
    b = bfromcstr("/mean\n");
    c = bfromcstr("/upsample\n");
    d = bfromcstr("/delete\n");
    e = bfromcstr("/dump\n");

    if(biseq(url, a) == 1){
        create_stat(map, blist);        
    } else if(biseq(url, b) == 1){
        get_mean(blist); 
    } else if(biseq(url, c) == 1){
        update_sample(blist); 
    } else if(biseq(url, d) == 1){
        delete_sample(blist);
    } else if(biseq(url, e) == 1){
        dump(blist); 
    }
}

void client_handler(int comm_fd)
{
    RingBuffer *send_rb = RingBuffer_create(RB_SIZE);
    RingBuffer *recv_rb = RingBuffer_create(RB_SIZE);
    Hashmap *map = Hashmap_create(NULL, NULL);
    //add_routes(routes, routes_array);

    read_some(recv_rb, comm_fd, 1);
   // parse_line(recv_rb, send_rb);    

    struct bstrList *blist = input_parser(recv_rb);
   
    url_router(map, blist);

    // The Parser will then route you to one of our four CRUD operations
        // it may/may not make sense to externalize the data structure 
        // and these operations
    
    // Seperately, exists our Stats engine, which will serve numeric functions
        // It will simply receive and return data to the CRUD functions,
        // which will access the data structures
    
    // ultimately, the return values will be written to the RingBuffer and 
    // sent back to the client ayyy

    //now fucking write it to the send_rb
    write_some(send_rb, comm_fd, 1);
}

int run_server(const char *host, const char *port)
{
    struct sigaction sa = {
        .sa_handler = handle_sigchild,
        .sa_flags = SA_RESTART | SA_NOCLDSTOP
    };

    int rc = 0;
    int sockfd;
  
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);

    char str[100];
    int comm_fd;

    printf("Hello -> Welcome to Velociraptor\n");

    // create a sigaction that handles SIGCHLD
    sigemptyset(&sa.sa_mask);
    rc = sigaction(SIGCHLD, &sa, 0);
    check(rc != -1, "Failed to setup signal handler for child processes.");

    sockfd = server_listen(host, port); 

    while(1)
    {
        comm_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        check(comm_fd >= 0, "Failed to accept connection.");

        debug("-> Client Connection Made <-\n");

        rc = fork();
        if(rc == 0) {
            // child process
            close(sockfd);
            // handle the client
            client_handler(comm_fd);
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
