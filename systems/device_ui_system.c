#include "glib.h"
#include "device_ui_system.h"
#include "utils/rendering.h"
#include "../components/component_registry.h"

Texture2D texture;
Shader shader_outline;

Device* selected_device;

void draw_popover(int x, int y, char* message) {
    DrawRectangle(x-1, y-12, MeasureText(message, 10)+2, 10, BLACK);
    DrawText(message, x, y-12, 10, GREEN);
}

void draw_device_id(Device device, Vector2 coord) {
    int width = MeasureText(device.id, 10);
    DrawRectangle(coord.x-1, coord.y-11, width+2, 10, BLACK);
    DrawText(device.id, coord.x, coord.y-11, 10, GREEN);
}

void detect_mouse_collision() {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 current_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

    Device* device = find_device_by_coord(current_tile.x, current_tile.y);
    if (device != NULL) {
        device = find_device_by_coord(current_tile.x, current_tile.y);

        draw_device_id(*device, mouse_pos);
    }
}

void render_selected_device() {
    if (selected_device == NULL) return;

    char* entity_id = selected_device->entity_id;
    Sprite* sprite = (Sprite*)g_hash_table_lookup(component_registry.sprites, entity_id);

    // Render Sprite
    Position* position = (Position*)g_hash_table_lookup(component_registry.positions, entity_id);
    Vector2 coord = position->coord;
    SpriteRect sprite_rect = sprite_sheet[sprite->sprite_id];

    Vector2 global_coord = convert_local_to_global(coord.x, coord.y);
    float global_x = global_coord.x;
    float global_y = global_coord.y;

    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-sprite_rect.rect.width/2-offset.x, -offset.y};

    BeginMode2D(camera);

    DrawTextureRec(texture, sprite_sheet[SPRITE_SELECTED].rect, (Vector2){global_x+offset.x, global_y+offset.y}, WHITE);
    DrawTextureRec(texture, sprite_rect.rect, (Vector2){global_x+offset.x, global_y+offset.y}, WHITE);

    EndMode2D();
}

void initialize_device_ui_system(Texture2D loaded_texture, Shader loaded_shader_outline) {
    texture = loaded_texture;
    selected_device = NULL;
    shader_outline = loaded_shader_outline;
}

void update_device_ui_system() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_pos = GetMousePosition();
        Vector2 clicked_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

        Device* device = find_device_by_coord(clicked_tile.x, clicked_tile.y);
        if (device != NULL) {
            selected_device = device;
        }
    }
}

void render_device_ui_system() {
    render_selected_device();

    detect_mouse_collision();
    draw_mouse_coords();
}

