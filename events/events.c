#include <glib.h>
#include <stdio.h>
#include "events.h"

EventLogMessage EventLogMessages[1000];
int EventLogMessagesSize = 0;

extern const char* EventLogMessageTable[] = {
    "", //0
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "%s is discovered and is revealed in the map.", // 10
    "%s is pwned and has been added to your botnet.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "You receive %d Bitcredits.", // 20
    "You find a new exploit. %s",
    "You find a new credentials list. %s",
};

void events_add_event_log_message_char(enum EventType eventType, char* entityId, char* arg1) {
    EventLogMessages[EventLogMessagesSize].eventType = eventType;
    EventLogMessages[EventLogMessagesSize].logMessageFormat = EventLogMessageTable[eventType];
    strcpy(EventLogMessages[EventLogMessagesSize].arg1.string, arg1);
    if (entityId) {
        strcpy(EventLogMessages[EventLogMessagesSize].entityId, entityId);
    }
    EventLogMessagesSize++;
}

void events_add_event_log_message_int(enum EventType eventType, char* entityId, int arg1) {
    EventLogMessages[EventLogMessagesSize].eventType = eventType;
    EventLogMessages[EventLogMessagesSize].logMessageFormat = EventLogMessageTable[eventType];
    EventLogMessages[EventLogMessagesSize].arg1.number = arg1;
    if (entityId) {
        strcpy(EventLogMessages[EventLogMessagesSize].entityId, entityId);
    }
    EventLogMessagesSize++;
}

void event_log_message_copy_to(char* dest, EventLogMessage eventLogMessage) {
    switch (eventLogMessage.eventType) {
        case DevicePwnedEvent:
        case DeviceDiscoveredEvent:
            sprintf(dest, eventLogMessage.logMessageFormat, eventLogMessage.arg1.string);
            break;
        case PlayerReceivesBitCreditsEvent:
            sprintf(dest, eventLogMessage.logMessageFormat, eventLogMessage.arg1.number);
            break;
        default:
            break;
    }
}
