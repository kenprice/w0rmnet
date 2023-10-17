#include "tiles.h"

Texture2D textureTileSheet;

TileRect tileSheet[] = {
        (TileRect){(Rectangle){0, 0, 64, 64},(Vector2){0, 0}},
};

Texture2D load_tile_sheet() {
    Image image = LoadImage("resources/map_tiles.png");  // Load image data into CPU memory (RAM)
    textureTileSheet = LoadTextureFromImage(image);       // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(image);                                    // Unload image data from CPU memory (RAM)

    image = LoadImageFromTexture(textureTileSheet);    // Load image from GPU texture (VRAM -> RAM)
    UnloadTexture(textureTileSheet);                   // Unload texture from GPU memory (VRAM)

    textureTileSheet = LoadTextureFromImage(image);    // Recreate texture from retrieved image data (RAM -> VRAM)
    UnloadImage(image);                                    // Unload retrieved image data from CPU memory (RAM)

    return textureTileSheet;
}
