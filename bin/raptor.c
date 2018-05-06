#include <stdio.h>
#include "dbg.h"
#include "raptor.h"
#include "utils.h"


int main(int argc, char *argv[])
{
    int rc = 0;
    const char *host = "127.0.0.1";
    const char *port = "7899";

    // refactor this later, I'm sick of dealing with it
    if(argc == 1) {
        rc = run_server(host, port);
    } else if(argc == 2) {
        if(is_host(argv[2]))
            rc = run_server(argv[2], port);
        else
            rc = run_server(host, argv[2]);
    } else if(argc == 3) {
        if(is_host(argv[2]))
            rc = run_server(argv[2], argv[3]);
        else
            rc = run_server(argv[3], argv[2]);
    } else {
        printf("NONE of those inputs were fucking acceptable.\n");
    }


    check(rc >= 0, "Server failed to run.");

    return 1;
error:
    return -1;
}
