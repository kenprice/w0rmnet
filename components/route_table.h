#ifndef W0RMNET_ROUTE_TABLE_H
#define W0RMNET_ROUTE_TABLE_H

#include "utils/device_id.h"

// RouteTable
typedef struct {
    char dest[DEVICE_NAME_LEN]; // Destination
    char via[DEVICE_NAME_LEN];  // Which next hop to send to
} RouteTableRecord;

typedef struct {
    RouteTableRecord records[100];
    int numRecords;
    int maxRecords;
} RouteTable;

void route_table_add_record(RouteTable* routeTable, char* dest, char* via);

#endif //W0RMNET_ROUTE_TABLE_H
