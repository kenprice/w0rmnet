#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "route_table.h"

char* comp_route_table_serialize(RouteTable* routeTable) {
    char recordsBuffer[10000] = "";
    for (int i = 0; i < routeTable->numRecords; i++) {
        char recordBuffer[1000] = "";
        RouteTableRecord record = routeTable->records[i];
        sprintf(recordBuffer, "%s,%s", record.prefix, record.wireEntityId);
        strcat(recordsBuffer, recordBuffer);
        strcat(recordsBuffer, "|");
    }
    recordsBuffer[strlen(recordsBuffer) - 1] = '\0';

    char routeTableBuffer[10000];
    sprintf(routeTableBuffer, "%s;%d", recordsBuffer, routeTable->numRecords);

    char* data = calloc(strlen(routeTableBuffer)+1, sizeof(char));
    strcpy(data, routeTableBuffer);
    return data;
}

RouteTable* comp_route_table_deserialize(char* data) {
    RouteTable* routeTable = calloc(1, sizeof(RouteTable));

    char** splitData = g_strsplit(data, ";", 2);
    routeTable->numRecords = atoi(splitData[1]);

    char** routes = g_strsplit(splitData[0], "|", routeTable->numRecords);
    for (int i = 0; i < routeTable->numRecords; i++) {
        char** route = g_strsplit(routes[i], ",", 2);
        strcpy(routeTable->records[i].prefix, route[0]);
        strcpy(routeTable->records[i].wireEntityId, route[1]);
        g_strfreev(route);
    }

    g_strfreev(splitData);
    g_strfreev(routes);

    return routeTable;
}
