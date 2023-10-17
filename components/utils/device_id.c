#include <stdlib.h>
#include <string.h>
#include "device_id.h"
#include "device_id_wordlist.h"
#include "../../utils/random.h"

char* generate_device_id() {
    char* deviceId = malloc(10 + 1);
    const char* word = WORDLIST[randint(WORDLIST_MAX)];

    strcpy(deviceId, word);

    return deviceId;
}
