#include "tiles.h"

Texture2D texture_tile_sheet;

TileRect tile_sheet[] = {
        (TileRect){(Rectangle){0, 0, 64, 64},(Vector2){0, 0}},
};

Texture2D load_tile_sheet() {
    Image image = LoadImage("../resources/map_tiles.png");  // Load image data into CPU memory (RAM)
    texture_tile_sheet = LoadTextureFromImage(image);       // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(image);                                    // Unload image data from CPU memory (RAM)

    image = LoadImageFromTexture(texture_tile_sheet);    // Load image from GPU texture (VRAM -> RAM)
    UnloadTexture(texture_tile_sheet);                   // Unload texture from GPU memory (VRAM)

    texture_tile_sheet = LoadTextureFromImage(image);    // Recreate texture from retrieved image data (RAM -> VRAM)
    UnloadImage(image);                                    // Unload retrieved image data from CPU memory (RAM)

    return texture_tile_sheet;
}
