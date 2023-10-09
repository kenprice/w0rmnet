#include "uuid.h"

char* generate_uuid() {
    uuid_t binuuid;
    uuid_generate_random(binuuid);
    char *uuid = malloc(UUID_STR_LEN);
    uuid_unparse_lower(binuuid, uuid);

    return uuid;
}
