#include <stdio.h>
#include "rendering.h"
#include "../../graphics/sprites.h"

Camera2D camera = { 0 };

void initialize_camera(int screen_width, int screen_height) {
    camera.target = (Vector2){0, 0};
    camera.offset = (Vector2){ screen_width/2.0f, screen_height/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void update_camera() {
    int offset = 10;

    if (IsKeyDown(KEY_RIGHT)) camera.offset.x -= offset;
    else if (IsKeyDown(KEY_LEFT)) camera.offset.x += offset;

    if (IsKeyDown(KEY_UP)) camera.offset.y += offset;
    else if (IsKeyDown(KEY_DOWN)) camera.offset.y -= offset;
}

/**
 * Isometric coord ("local") to "global" coord.
 * https://yal.cc/understanding-isometric-grids/
 */
Vector2 convert_local_to_global(float x, float y) {
    float iso_x = camera.offset.x;
    float iso_y = camera.offset.y;
    float iso_w = (float)SPRITE_X_SCALE/2;
    float iso_h = (float)SPRITE_Y_SCALE/2;
    float global_x = iso_x + (x - y) * iso_w;
    float global_y = iso_y + (x + y) * iso_h;
    return (Vector2){global_x, global_y};
}

Vector2 convert_global_to_local(float x, float y) {
    float iso_x = camera.offset.x;
    float iso_y = camera.offset.y;
    float iso_w = (float)SPRITE_X_SCALE/2;
    float iso_h = (float)SPRITE_Y_SCALE/2;
    float local_x = ((y - iso_y) / iso_h + (x - iso_x) / iso_w) / 2;
    float local_y = ((y - iso_y) / iso_h - (x - iso_x) / iso_w) / 2;
    return (Vector2){local_x, local_y};
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
