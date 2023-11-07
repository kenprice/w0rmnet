#include <glib.h>
#include <stdio.h>
#include "recent_events.h"
#include "../../events/events.h"
#include "../../lib/raygui.h"

#define MAX_NUM_EVENTS 30

typedef struct {
    Rectangle rect;
    DeviceEvent events[MAX_NUM_EVENTS];
    int numEvents;
} RecentEventsState;

RecentEventsState recentEventsState;

void recent_events_on_device_pwned(DeviceEvent event);

void init_recent_events_view() {
    events_register_device_event_listener(recent_events_on_device_pwned);
    recentEventsState.numEvents = 0;
}

void update_recent_events_view(Rectangle rect) {
    recentEventsState.rect = rect;
}

void render_recent_events_view() {
    GuiPanel(recentEventsState.rect, NULL);

    Vector2 textVec = (Vector2){recentEventsState.rect.x + 8, recentEventsState.rect.y + 4};
    int numEvents = recentEventsState.numEvents;
    int break1 = numEvents / 3;
    int break2 = 2 * numEvents / 3;

    for (int i = 0; i < numEvents; i++) {
        float fade = 0;
        if (i == 0) fade = 0;
        if (i > 0) fade = 0.5f;
        if (i > break1) fade = 0.7f;
        if (i > break2) fade = 0.9f;

        char buffer[100];
        switch (recentEventsState.events[i].type) {
            case DevicePwnedEvent:
                sprintf(buffer, "%s pwned.", recentEventsState.events[i].device->name);
                DrawTextEx(GuiGetFont(), buffer, textVec, 14, 0, Fade(WHITE, 1.0f - fade));
                textVec.y += 14;
                break;
            default:
                break;
        }

        if (textVec.y > GetScreenHeight()) break;
    }
}

void add_event(DeviceEvent event) {
    for (int i = recentEventsState.numEvents; i > 0; i--) {
        recentEventsState.events[i] = recentEventsState.events[i - 1];
    }

    recentEventsState.events[0] = event;

    if (recentEventsState.numEvents++ >= MAX_NUM_EVENTS) {
        recentEventsState.numEvents = MAX_NUM_EVENTS-1;
    }
}

void recent_events_on_device_pwned(DeviceEvent event) {
    if (event.type == DevicePwnedEvent) {
        add_event(event);
    }
}
