#ifndef W0RMNET_RECENT_EVENTS_H
#define W0RMNET_RECENT_EVENTS_H

#include "raylib.h"
#include "area_viewer_window.h"

void init_recent_events_view(AreaViewerWindowState* areaViewer);

void update_recent_events_view(Rectangle rect);

void render_recent_events_view();

#endif //W0RMNET_RECENT_EVENTS_H
