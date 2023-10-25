#ifndef W0RMNET_ROUTE_TABLE_H
#define W0RMNET_ROUTE_TABLE_H

#include "utils/device_id.h"
#include "../utils/uuid.h"

typedef struct {
    char prefix[100]; // Address prefix or *, * = anything that doesn't match
    char wireEntityId[UUID_STR_LEN];
} RouteTableRecord;

/**
 * "Routing Table"
 *    Prefix             Wire
 *    ------             ----
 *    nc.metro.t4ffy     Wire 1    Routes packet to e.g. nc.metro.t4ffy, nc.metro.t4ffy.b0ron to Wire 1
 *    *                  Wire 2    Routes everything else to Wire 2
 *
 */
typedef struct {
    RouteTableRecord records[50];
    int numRecords;
} RouteTable;

char* comp_route_table_serialize(RouteTable* routeTable);

RouteTable* comp_route_table_deserialize(char* data);

#endif //W0RMNET_ROUTE_TABLE_H
