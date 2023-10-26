#ifndef W0RMNET_LOGGER_H
#define W0RMNET_LOGGER_H

#define COMP_LOGGER_MAX_ENTRIES 20

/**
 * In-game logger for individual machines, routers, etc.
 */
typedef struct {
    char* logEntries[COMP_LOGGER_MAX_ENTRIES];
    int numEntries;
} Logger;

void comp_logger_add_entry(Logger* logger, const char* entry);

#endif //W0RMNET_LOGGER_H
