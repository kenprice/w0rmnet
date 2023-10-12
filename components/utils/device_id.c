#include <stdlib.h>
#include <string.h>
#include "device_id.h"
#include "device_id_wordlist.h"
#include "../../utils/random.h"

char* generate_device_id() {
    char* device_id = malloc(10 + 1);
    const char* word1 = WORDLIST[randint(WORDLIST_MAX)];
    const char* word2 = WORDLIST[randint(WORDLIST_MAX)];

    strcpy(device_id, word1);
    strcat(device_id, word2);

    return device_id;
}
