#ifndef W0RMNET_CONNECTION_H
#define W0RMNET_CONNECTION_H

#include "utils/device_id.h"

typedef struct {
    char from_device_id[DEVICE_ID_LEN];
    char to_device_id[100][DEVICE_ID_LEN];
    int num_conns;
    int max_conns;
} Connection; // Wire-like structure

int add_device_to_connection(char* entity_id, char* device_id);

void iterate_connections(void (*cb)(char*,Connection*));

#endif //W0RMNET_CONNECTION_H
