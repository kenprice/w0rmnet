#include <string.h>
#include "route_table.h"

void route_table_add_record(RouteTable* route_table, char* dest, char* via) {
    if (route_table->num_records >= route_table->max_records) {
        return;
    }
    int i = route_table->num_records;
    strcpy(route_table->records[i].dest, dest);
    strcpy(route_table->records[i].via, via);
}
