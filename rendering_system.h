#ifndef EXAMPLE_RENDERING_SYSTEM_H
#define EXAMPLE_RENDERING_SYSTEM_H

#include "component_registry.h"
#include "raylib.h"
#include <glib.h>
#include <stdio.h>


/**
 * Isometric coord ("local") to "global" coord.
 * https://yal.cc/understanding-isometric-grids/
 */
Vector2 convert_local_to_global(float iso_x_origin, float iso_y_origin, float x, float y) {
    float iso_w = (float)SPRITE_X_SCALE/2;
    float iso_h = (float)SPRITE_Y_SCALE/2;
    float global_x = iso_x_origin + (x - y) * iso_w;
    float global_y = iso_y_origin + (x + y) * iso_h;
    return (Vector2){global_x, global_y};
}

Vector2 convert_global_to_local(float iso_x_origin, float iso_y_origin, float x, float y) {
    float iso_w = (float)SPRITE_X_SCALE/2;
    float iso_h = (float)SPRITE_Y_SCALE/2;
    float local_x = ((y - iso_y_origin) / iso_h + (x - iso_x_origin) / iso_w) / 2;
    float local_y = ((y - iso_y_origin) / iso_h - (x - iso_x_origin) / iso_w) / 2;
    return (Vector2){local_x, local_y};
}

void draw_mouse_coords() {
    Vector2 mouse_pos = GetMousePosition();
    char buffer[1000];
    sprintf(buffer, "Mouse Global: (%d,%d)", (int)mouse_pos.x, (int)mouse_pos.y);
    DrawText(buffer, 3, 3, 10, WHITE);

    Vector2 local_pos = convert_global_to_local(400, 0, mouse_pos.x, mouse_pos.y);
    sprintf(buffer, "Mouse Local: (%d,%d)", (int)local_pos.x, (int)local_pos.y);
    DrawText(buffer, 3, 15, 10, WHITE);
}

void draw_isometric_grid_tile(float x, float y) {
    int iso_w = SPRITE_X_SCALE/2;
    int iso_h = SPRITE_Y_SCALE/2;
    DrawLine(x, y, x+iso_w, y+iso_h, WHITE);
    DrawLine(x, y, x-iso_w, y+iso_h, WHITE);
    DrawLine(x, y+(iso_h*2), x+iso_w, y+iso_h, WHITE);
    DrawLine(x, y+(iso_h*2), x-iso_w, y+iso_h, WHITE);
}

void draw_isometric_grid(int screen_width, int screen_height) {
    int iso_x = screen_width / 2;
    int iso_y = 0;
    int iso_w = SPRITE_X_SCALE/2;
    int iso_h = SPRITE_Y_SCALE/2;

    int num_x_tiles = 12;
    int num_y_tiles = 12;


    for (int y = 0; y < num_y_tiles; y++) {
        for (int x = 0; x < num_x_tiles; x++) {
            int global_x = iso_x + (x - y) * iso_w;
            int global_y = iso_y + (x + y) * iso_h;
            draw_isometric_grid_tile(global_x, global_y);
//            DrawLine(global_x, global_y, global_x+iso_w, global_y+iso_h, WHITE);
//            DrawLine(global_x, global_y, global_x-iso_w, global_y+iso_h, WHITE);
//            DrawLine(global_x, global_y+(iso_h*2), global_x+iso_w, global_y+iso_h, WHITE);
//            DrawLine(global_x, global_y+(iso_h*2), global_x-iso_w, global_y+iso_h, WHITE);

            char buffer[100];
            sprintf(buffer, "%d,%d", x, y);
            DrawText(buffer, global_x, global_y+10, 10, BLACK);
//            DrawLine(global_x, global_y+iso_h, global_x - (iso_w), global_y + (iso_h), WHITE);
        }
    }
}

void device_rendering_system(Texture2D texture, ComponentRegistry* registry) {
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, registry->sprites);

    guint* key_;
    Sprite* val;

    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &val))
    {
        Position* position = (Position*)g_hash_table_lookup(registry->positions, key_);
        Device* dev = (Device*)g_hash_table_lookup(registry->devices, key_);

        float iso_x = 400; // TODO: Adjust
        float iso_y = 0;
        float iso_w = SPRITE_X_SCALE/2;
        float iso_h = SPRITE_Y_SCALE/2;
        float x = position->coord.x;
        float y = position->coord.y;
        float global_x = iso_x + (x - y) * iso_w;
        float global_y = iso_y + (x + y) * iso_h;

        Rectangle sprite_rect = sprite_sheet[val->sprite_id].rect;
        Vector2 offset = sprite_sheet[val->sprite_id].offset;
        offset = (Vector2){-sprite_rect.width/2-offset.x, -offset.y};

//        Vector2 coord = (Vector2){global_x, global_y}; // TODO: Add alignment stuff in tileset
        Vector2 coord = (Vector2){global_x+offset.x, global_y+offset.y}; // TODO: Add alignment stuff in tileset

        DrawTextureRec(texture, sprite_rect, coord, WHITE);
    }

    draw_mouse_coords();
}

#endif //EXAMPLE_RENDERING_SYSTEM_H
