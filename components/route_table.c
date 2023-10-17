#include <string.h>
#include "route_table.h"

void route_table_add_record(RouteTable* routeTable, char* dest, char* via) {
    if (routeTable->numRecords >= routeTable->maxRecords) {
        return;
    }
    int i = routeTable->numRecords;
    strcpy(routeTable->records[i].dest, dest);
    strcpy(routeTable->records[i].via, via);
}
