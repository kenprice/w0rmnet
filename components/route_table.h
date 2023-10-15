#ifndef W0RMNET_ROUTE_TABLE_H
#define W0RMNET_ROUTE_TABLE_H

#include "utils/device_id.h"

// RouteTable
typedef struct {
    char dest[DEVICE_ID_LEN]; // Destination
    char via[DEVICE_ID_LEN];  // Which next hop to send to
} RouteTableRecord;

typedef struct {
    RouteTableRecord records[100];
    int num_records;
    int max_records;
} RouteTable;

void route_table_add_record(RouteTable* route_table, char* dest, char* via);

#endif //W0RMNET_ROUTE_TABLE_H
