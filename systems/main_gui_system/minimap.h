#ifndef W0RMNET_MINIMAP_H
#define W0RMNET_MINIMAP_H

#include "raylib.h"

#define UI_MINIMAP_VIEW_HEIGHT 280

void init_minimap_view(Rectangle rect);

void update_minimap_view(Rectangle rect);

void render_minimap_view();

#endif //W0RMNET_MINIMAP_H
