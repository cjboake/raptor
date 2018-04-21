#include <stdio.h>
#include "utils.h"
#include <string.h>



//  do a general validator
int serv_validator(char *argv[])
{

    return 1;
}


//  it'll also need to see what each element it
int is_host(const char *arg)
{
    const char *period = ".";
    
    char* pPosition = strchr(period, '|'); 
    
    if(pPosition == NULL)
        return 0;
    else
        return 1;
}



//todo: we're gonna need some sort of input validator
    //if(argv[0] == NULL && argv[1] == NULL){
    //    rc = run_server(host, port);
    //} else if(argv[0] == NULL && argv[1] != NULL) {     // if host is null
    //    rc = run_server(host, argv[1]);
    //} else if(argv[0] != NULL && argv[1] == NULL) {   // if port is null
    //    rc = run_server(host,);
    //} else if(argv[0] =!= NULL && argv[1] != NULL) {   // if neither are null
    //    rc = run_server(argv[0], argv[1]);
    //} else {
    //    printf("Use a valid input, dude.\n");
    //}

