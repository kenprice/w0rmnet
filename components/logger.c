#include <stdlib.h>
#include <string.h>
#include "logger.h"

void comp_logger_add_entry(Logger* logger, const char* entry) {
    if (!logger) return;

    if (logger->numEntries == COMP_LOGGER_MAX_ENTRIES) {
        // rolling logger; drop one off and add
        free(logger->logEntries[0]);
        for (int i = 0; i < COMP_LOGGER_MAX_ENTRIES-1; i++) {
            logger->logEntries[i] = logger->logEntries[i+1];
        }
        logger->numEntries--;
    }

    char buffer[200];
    strncpy(buffer, entry, 199);
    buffer[199] = '\0'; // Prevent overflow, just in case.

    logger->logEntries[logger->numEntries] = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(logger->logEntries[logger->numEntries], buffer);
    logger->numEntries++;
}