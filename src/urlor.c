#include "./lib/tstree.h"
#include "./lib/bstrlib.h"

TSTree *add_route_data(TSTree * routes, bstring line)
{
    struct bstrList *data = bsplit(line, ' ');
    check(data->qty == 2, "Line '%s' does not have 2 columns",
            bdata(line));

    routes = TSTree_insert(routes,
            bdata(data->entry[0]),
            blength(data->entry[0]),
            bstrcpy(data->entry[1]));

    bstrListDestroy(data);

    return routes;

error:
    return NULL;
}

TSTree *load_routes(const char *file)
{
    TSTree *routes = NULL;
    bstring line = NULL;
    FILE *routes_map = NULL;

    routes_map = fopen(file, "r");
    check(routes_map != NULL, "Failed to open routes: %s", file);

    while ((line = bgets((bNgetc) fgetc, routes_map, '\n')) != NULL) {
        check(btrimws(line) == BSTR_OK, "Failed to trim line.");
        routes = add_route_data(routes, line);
        check(routes != NULL, "Failed to add route.");
        bdestroy(line);
    }

    fclose(routes_map);
    return routes;

error:
    if (routes_map) fclose(routes_map);
    if (line) bdestroy(line);

    return NULL;
}

bstring match_url(TSTree * routes, bstring url)
{
    bstring route = TSTree_search(routes, bdata(url), blength(url));

    if (route == NULL) {
        printf("No exact match found, trying prefix.\n");
        route = TSTree_search_prefix(routes, bdata(url), blength(url));
    }

    return route;
}

bstring read_line(const char *prompt)
{
    printf("%s", prompt);

    bstring result = bgets((bNgetc) fgetc, stdin, '\n');
    check_debug(result != NULL, "stdin closed.");

    check(btrimws(result) == BSTR_OK, "Failed to trim.");

    return result;

error:
    return NULL;
}

void bdestroy_cb(void *value, void *ignored)
{
    (void)ignored;
    bdestroy((bstring) value);
}

void destroy_routes(TSTree * routes)
{
    TSTree_traverse(routes, bdestroy_cb, NULL);
    TSTree_destroy(routes);
}
