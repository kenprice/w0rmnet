#include "raylib.h"
#include "../../lib/raygui.h"
#include "../../graphics/sprites.h"
#include "../../graphics/tiles.h"

/**
 * @param rect
 * @param numXTiles
 * @param numYTiles
 * @param zoom
 */
void isometric_map_draw_grid(Rectangle rect,  int numXTiles, int numYTiles, float zoom) {
    Color color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
    color = (Color){color.r, color.g, color.b, color.a*0.25f};

    float isoX = rect.x / zoom;
    float isoY = rect.y / zoom;
    int isoW = SPRITE_X_SCALE / 2;
    int isoH = SPRITE_Y_SCALE / 2;

    for (int y = -numYTiles; y < numYTiles*2; y++) {
        for (int x = -numXTiles; x < numXTiles*2; x++) {
            int globalX = isoX + (x - y) * isoW;
            int globalY = isoY + (x + y) * isoH;

            if (y == -numYTiles && x == -numXTiles) continue;

            if (y == -numYTiles) {
                DrawLine(globalX, globalY+(isoH * 2), globalX - isoW, globalY + isoH, color);
            } else if (x == -numXTiles) {
                DrawLine(globalX, globalY+(isoH * 2), globalX + isoW, globalY + isoH, color);
            } else {
                DrawLine(globalX, globalY+(isoH * 2), globalX + isoW, globalY + isoH, color);
                DrawLine(globalX, globalY+(isoH * 2), globalX - isoW, globalY + isoH, color);
            }
        }
    }
}

void isometric_map_render_tiles(Rectangle rect, int numXTiles, int numYTiles, float zoom) {
    float isoX = rect.x / zoom;
    float isoY = rect.y / zoom;
    int isoW = SPRITE_X_SCALE / 2;
    int isoH = SPRITE_Y_SCALE / 2;

    for (int y = 0; y < numYTiles; y++) {
        for (int x = 0; x < numXTiles; x++) {
            int globalX = isoX + (x - y) * isoW;
            int globalY = isoY + (x + y) * isoH;

            Vector2 offset = tileSheet[0].offset;
            offset = (Vector2){-tileSheet[0].rect.width / 2 - offset.x, -offset.y};

            DrawTextureRec(textureTileSheet, tileSheet[0].rect, (Vector2){globalX + offset.x, globalY + offset.y}, WHITE);
        }
    }
}

Vector2 isometric_map_local_to_global(Rectangle rect, float x, float y, float zoom) {
    float isoX = rect.x / zoom;
    float isoY = rect.y / zoom;
    float isoW = (float)SPRITE_X_SCALE / 2;
    float isoH = (float)SPRITE_Y_SCALE / 2;
    float globalX = isoX + (x - y) * isoW;
    float globalY = isoY + (x + y) * isoH;
    return (Vector2){globalX, globalY};
}

Vector2 isometric_map_global_to_local(Rectangle rect, Vector2 offset, float x, float y, float zoom) {
    float scaledX = (x - rect.x) / zoom;
    float scaledY = (y - rect.y) / zoom;
    float isoX = offset.x / zoom;
    float isoY = offset.y / zoom;
    float isoW = (float)SPRITE_X_SCALE / 2;
    float isoH = (float)SPRITE_Y_SCALE / 2;
    float localX = ((scaledY - isoY) / isoH + (scaledX - isoX) / isoW) / 2;
    float localY = ((scaledY - isoY) / isoH - (scaledX - isoX) / isoW) / 2;
    return (Vector2){localX, localY};
}
