#ifndef W0RMNET_SPRITES_H
#define W0RMNET_SPRITES_H

#include "raylib.h"

#define SPRITE_X_SCALE 64
#define SPRITE_Y_SCALE 32

#define SPRITE_SELECTED 0
#define SPRITE_ROUTER 1
#define SPRITE_ROUTER_BLINK 2
#define SPRITE_LAPTOP 3
#define SPRITE_UNKNOWN 4
#define SPRITE_SWITCH_FIREWALL 5
#define SPRITE_DESKTOP 6

typedef struct {
    Rectangle rect; // Section of the spritesheet
    Vector2 offset; // offset by which to render, in order to align to isometric grid
} SpriteRect;

extern SpriteRect spriteSheet[];

extern Texture2D textureSpriteSheet;

Texture2D load_sprite_sheet();

#endif //W0RMNET_SPRITES_H
