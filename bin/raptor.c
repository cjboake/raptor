#include <stdio.h>
#include "../src/dbg.h"
#include "raptor.h"

int main(int argc, char *argv[])
{
    int rc = 0;
    rc = run_server();
    
    check(rc >= 0, "Server failed to run.");

    return 1;
error:
    return -1;
}
