#include "sprites.h"

SpriteRect sprite_sheet[] = {
        (SpriteRect){(Rectangle){0, 0, 64, 64},(Vector2){0, 32}},
        (SpriteRect){(Rectangle){64, 0, 64, 64},(Vector2){0, 32}},
    (SpriteRect){(Rectangle){128, 0, 64, 64},(Vector2){0, 32}},
};

Texture2D load_sprite_sheet() {
    Image image = LoadImage("../resources/tileset.png");  // Load image data into CPU memory (RAM)
    Texture2D texture = LoadTextureFromImage(image);       // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(image);                                    // Unload image data from CPU memory (RAM)

    image = LoadImageFromTexture(texture);                 // Load image from GPU texture (VRAM -> RAM)
    UnloadTexture(texture);                                // Unload texture from GPU memory (VRAM)

    texture = LoadTextureFromImage(image);                 // Recreate texture from retrieved image data (RAM -> VRAM)
    UnloadImage(image);                                    // Unload retrieved image data from CPU memory (RAM)

    return texture;
}
