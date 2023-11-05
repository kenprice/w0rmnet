#include "sprites.h"

Texture2D textureSpriteSheet;

SpriteRect spriteSheet[] = {
    (SpriteRect){(Rectangle){0, 0, 64, 64},(Vector2){0, 32}},
    (SpriteRect){(Rectangle){64, 0, 64, 64},(Vector2){0, 32}},
    (SpriteRect){(Rectangle){64, 64, 64, 64},(Vector2){0, 32}},
    (SpriteRect){(Rectangle){128, 0, 64, 64},(Vector2){0, 32}},
    (SpriteRect){(Rectangle){192, 0, 64, 64},(Vector2){0, 32}},
    (SpriteRect){(Rectangle){0, 64, 64, 64},(Vector2){0, 32}},
};

Texture2D load_sprite_sheet() {
    Image image = LoadImage("resources/tileset.png");  // Load image data into CPU memory (RAM)
    textureSpriteSheet = LoadTextureFromImage(image);       // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(image);                                    // Unload image data from CPU memory (RAM)

    image = LoadImageFromTexture(textureSpriteSheet);    // Load image from GPU texture (VRAM -> RAM)
    UnloadTexture(textureSpriteSheet);                   // Unload texture from GPU memory (VRAM)

    textureSpriteSheet = LoadTextureFromImage(image);    // Recreate texture from retrieved image data (RAM -> VRAM)
    UnloadImage(image);                                    // Unload retrieved image data from CPU memory (RAM)

    return textureSpriteSheet;
}
