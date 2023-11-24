#ifndef W0RMNET_EVENTS_H
#define W0RMNET_EVENTS_H

#include "../utils/uuid.h"

typedef enum {
    DeviceDiscoveredEvent = 10,
    DevicePwnedEvent,
    DevicePwnedViaLogin,
    DevicePwnedViaExploit,

    PlayerReceivesBitCreditsEvent = 20,
    PlayerReceivesExploitEvent,
    PlayerReceivesCredDumpEvent,

    WormCreatedEvent = 30,
    WormInfectsDeviceEvent,
} EventType ;

typedef union {
    int number;
    char string[50];
} EventLogMessageArg;

typedef struct {
    EventType eventType;
    const char* logMessageFormat;
    char entityId[UUID_STR_LEN];
    EventLogMessageArg arg1;
    EventLogMessageArg arg2;
    EventLogMessageArg arg3;
} EventLogMessage;

extern const char* EventLogMessageTable[];

extern EventLogMessage EventLogMessages[1000];
extern int EventLogMessagesSize;

void events_add_event_log_message_char(EventType eventType, char* entityId, char* arg1);

void events_add_event_log_message_char_char(EventType eventType, char* entityId, char* arg1, char* arg2);

void events_add_event_log_message_int(EventType eventType, char* entityId, int arg1);

void event_log_message_copy_to(char* dest, EventLogMessage eventLogMessage);

#endif //W0RMNET_EVENTS_H
