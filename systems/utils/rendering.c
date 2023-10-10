#include "rendering.h"

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
