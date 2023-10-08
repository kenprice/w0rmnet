#ifndef EXAMPLE_UTILS_H
#define EXAMPLE_UTILS_H

#include <stdlib.h>
#include <uuid/uuid.h>

#define UUID_STR_LEN 37

char* generate_uuid() {
    uuid_t binuuid;
    uuid_generate_random(binuuid);
    char *uuid = malloc(UUID_STR_LEN);
    uuid_unparse_lower(binuuid, uuid);

    return uuid;
}


#endif //EXAMPLE_UTILS_H
