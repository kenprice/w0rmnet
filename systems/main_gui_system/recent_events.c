#include <glib.h>
#include <stdio.h>
#include "recent_events.h"
#include "../../components/component_registry.h"
#include "../../events/device_events.h"
#include "../../lib/raygui.h"
#include "../../utils/math_macros.h"
#include "../../lib/text_rectangle_bounds.h"

#define MAX_NUM_EVENTS 40

typedef struct {
    Rectangle rect;
    // Scroll
    Rectangle scrollPanelView;
    Vector2 scrollPanelScroll;

    AreaViewerWindowState* areaViewer;
} RecentEventsState;

RecentEventsState recentEventsState;

void recent_events_on_device_pwned(DeviceEvent event);
void focus_area_viewer_on_device(Device* device);

void init_recent_events_view(AreaViewerWindowState* areaViewer) {
    events_register_device_event_listener(recent_events_on_device_pwned);
    recentEventsState.areaViewer = areaViewer;
}

void update_recent_events_view(Rectangle rect) {
    recentEventsState.rect = rect;
}

void render_recent_events_view() {
    GuiPanel(recentEventsState.rect, NULL);

    Rectangle textRect = recentEventsState.rect;
    textRect = (Rectangle){ textRect.x + 8, textRect.y + 4, textRect.width - 16, textRect.height - 8 };

    int numEvents = min(EventLogMessagesSize, MAX_NUM_EVENTS); // limit to last N events

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

void recent_events_on_device_pwned(DeviceEvent event) {
    if (event.type == DevicePwnedEvent) {
        focus_area_viewer_on_device(event.device);
    }
}
