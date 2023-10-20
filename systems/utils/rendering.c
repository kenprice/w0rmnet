#include <stdio.h>
#include "rendering.h"
#include "../../graphics/sprites.h"

Camera2D camera = { 0 };

void initialize_camera(float x_offset, float y_offset) {
    camera.target = (Vector2){0, 0};
    camera.offset = (Vector2){x_offset, y_offset};
    camera.rotation = 0.0f;
//    camera.zoom = 1.0f;
    camera.zoom = 0.5f;
}

/**
 * Isometric coord ("local") to "global" coord.
 * https://yal.cc/understanding-isometric-grids/
 */
Vector2 convert_local_to_global(float x, float y) {
    float iso_x = 0;
    float iso_y = 0;
    float iso_w = (float)SPRITE_X_SCALE/2;
    float iso_h = (float)SPRITE_Y_SCALE/2;
    float global_x = iso_x + (x - y) * iso_w;
    float global_y = iso_y + (x + y) * iso_h;
    return (Vector2){global_x, global_y};
}

Vector2 convert_global_to_local(float x, float y) {
    float scaledX = x / camera.zoom;
    float scaledY = y / camera.zoom;
    float isoX = camera.offset.x / camera.zoom;
    float isoY = camera.offset.y / camera.zoom;
    float isoW = (float)SPRITE_X_SCALE / 2;
    float isoH = (float)SPRITE_Y_SCALE / 2;
    float localX = ((scaledY - isoY) / isoH + (scaledX - isoX) / isoW) / 2;
    float localY = ((scaledY - isoY) / isoH - (scaledX - isoX) / isoW) / 2;
    return (Vector2){localX, localY};
}

void draw_mouse_coords() {
    Vector2 mouse_pos = GetMousePosition();
    char buffer[1000];
    sprintf(buffer, "Mouse Global: (%d,%d)", (int)mouse_pos.x, (int)mouse_pos.y);
    DrawText(buffer, 3, 3, 10, WHITE);

    Vector2 local_pos = convert_global_to_local(mouse_pos.x, mouse_pos.y);
    sprintf(buffer, "Mouse Local: (%d,%d)", (int)local_pos.x, (int)local_pos.y);
    DrawText(buffer, 3, 15, 10, WHITE);

    sprintf(buffer, "Camera Offset: (%d,%d)", (int)camera.offset.x, (int)camera.offset.y);
    DrawText(buffer, 3, 27, 10, WHITE);
}
