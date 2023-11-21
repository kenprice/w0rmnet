#ifndef W0RMNET_MAIN_GUI_SYSTEM_H
#define W0RMNET_MAIN_GUI_SYSTEM_H

#include "raylib.h"
#include "../components/components.h"
#include "../world/world_map.h"
#include "../store/worms.h"

void initialize_main_gui_system();

void update_main_gui_system();

void render_main_gui_system();

void gui_load_area_view_left_panel(Area* area);

void gui_load_device_info_panel(Device* device);

void gui_set_active_worm(Worm* worm);

Device* gui_get_selected_device();

#endif //W0RMNET_MAIN_GUI_SYSTEM_H
