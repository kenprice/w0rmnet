#ifndef W0RMNET_DEVICE_INFO_PANEL_H
#define W0RMNET_DEVICE_INFO_PANEL_H

#include "../../components/components.h"
#include "../../lib/raygui.h"

void init_device_info_panel();

int render_device_info_panel(Rectangle rect, Device* device);

#endif //W0RMNET_DEVICE_INFO_PANEL_H
