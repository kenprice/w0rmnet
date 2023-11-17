#include <glib.h>
#include <stdio.h>
#include "recent_events.h"
#include "../../components/component_registry.h"
#include "../../events/device_events.h"
#include "../../lib/raygui.h"
#include "../../utils/math_macros.h"
#include "../../lib/text_rectangle_bounds.h"

#define MAX_NUM_EVENTS 30

typedef struct {
    Rectangle rect;
    DeviceEvent events[MAX_NUM_EVENTS];
    int numEvents;

    AreaViewerWindowState* areaViewer;

    // Scroll
    Rectangle scrollPanelView;
    Vector2 scrollPanelScroll;
} RecentEventsState;

RecentEventsState recentEventsState;

void recent_events_on_device_pwned(DeviceEvent event);
void focus_area_viewer_on_device(Device* device);

void init_recent_events_view(AreaViewerWindowState* areaViewer) {
    events_register_device_event_listener(recent_events_on_device_pwned);
    recentEventsState.numEvents = 0;
    recentEventsState.areaViewer = areaViewer;
}

void update_recent_events_view(Rectangle rect) {
    recentEventsState.rect = rect;
}

void render_recent_events_view() {
    GuiPanel(recentEventsState.rect, NULL);

    Rectangle textRect = recentEventsState.rect;
    textRect = (Rectangle){ textRect.x + 8, textRect.y + 4, textRect.width - 16, textRect.height - 8 };

    int numEvents = min(EventLogMessagesSize, 40); // limit to last 40

    char log[10000] = "";
    for (int i = 0; i < numEvents; i++) {
        char buffer[100];
        event_log_message_copy_to(buffer, EventLogMessages[EventLogMessagesSize-1-i]);
        buffer[strlen(buffer)] = '\0';
        buffer[strlen(buffer)-1] = '\n';
        strcat(log, buffer);
    }

    DrawTextBoxed(GuiGetFont(), log, textRect, 14, 0, true, WHITE);
}

void focus_area_viewer_on_device(Device* device) {
    Area* area = find_area_by_device(device);
    if (!area) return;

    recentEventsState.areaViewer->area = area;

    Position* position = g_hash_table_lookup(componentRegistry.positions, device->entityId);
    if (!position) return;

    area_viewer_center_at_position(recentEventsState.areaViewer, position);
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
        focus_area_viewer_on_device(event.device);
    }
}
