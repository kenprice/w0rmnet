#include <glib.h>
#include <stdio.h>
#include "events.h"

EventLogMessage EventLogMessages[1000];
int EventLogMessagesSize = 0;

const char* EventLogMessageTable[] = {
    [DeviceDiscoveredEvent] = "%s is discovered and is revealed in the map.",
    [DevicePwnedEvent] = "%s is pwned and has been added to your botnet.",
    [DevicePwnedViaLogin] = "%s is pwned by %s via login.",
    [DevicePwnedViaExploit] = "%s is pwned by %s via exploit.",
    [PlayerReceivesBitCreditsEvent] = "You receive %d Bitcredits.",
    [PlayerReceivesExploitEvent] = "New exploit: %s",
    [PlayerReceivesCredDumpEvent] = "New cred dump: %s",
    [WormInfectsDeviceEvent] = "%s is infected by %s",
};

void events_add_event_log_message_char(EventType eventType, char* entityId, char* arg1) {
    EventLogMessages[EventLogMessagesSize].eventType = eventType;
    EventLogMessages[EventLogMessagesSize].logMessageFormat = EventLogMessageTable[eventType];
    strncpy(EventLogMessages[EventLogMessagesSize].arg1.string, arg1, 50);
    if (entityId) {
        strncpy(EventLogMessages[EventLogMessagesSize].entityId, entityId, UUID_STR_LEN);
    }
    EventLogMessagesSize++;
}

void events_add_event_log_message_char_char(EventType eventType, char* entityId, char* arg1, char* arg2) {
    EventLogMessages[EventLogMessagesSize].eventType = eventType;
    EventLogMessages[EventLogMessagesSize].logMessageFormat = EventLogMessageTable[eventType];
    strncpy(EventLogMessages[EventLogMessagesSize].arg1.string, arg1, 50);
    strncpy(EventLogMessages[EventLogMessagesSize].arg2.string, arg2, 50);
    if (entityId) {
        strncpy(EventLogMessages[EventLogMessagesSize].entityId, entityId, UUID_STR_LEN);
    }
    EventLogMessagesSize++;
}

void events_add_event_log_message_int(EventType eventType, char* entityId, int arg1) {
    EventLogMessages[EventLogMessagesSize].eventType = eventType;
    EventLogMessages[EventLogMessagesSize].logMessageFormat = EventLogMessageTable[eventType];
    EventLogMessages[EventLogMessagesSize].arg1.number = arg1;
    if (entityId) {
        strncpy(EventLogMessages[EventLogMessagesSize].entityId, entityId, UUID_STR_LEN);
    }
    EventLogMessagesSize++;
}

void event_log_message_copy_to(char* dest, EventLogMessage eventLogMessage) {
    switch (eventLogMessage.eventType) {
        case DevicePwnedEvent:
        case DeviceDiscoveredEvent:
        case PlayerReceivesExploitEvent:
        case PlayerReceivesCredDumpEvent:
            sprintf(dest, eventLogMessage.logMessageFormat, eventLogMessage.arg1.string);
            break;
        case DevicePwnedViaExploit:
        case DevicePwnedViaLogin:
            sprintf(dest, eventLogMessage.logMessageFormat, eventLogMessage.arg1.string, eventLogMessage.arg2.string);
            break;
        case PlayerReceivesBitCreditsEvent:
            sprintf(dest, eventLogMessage.logMessageFormat, eventLogMessage.arg1.number);
            break;
        default:
            break;
    }
}
