//
// Created by Ken Price on 10/7/23.
//
#ifndef EXAMPLE_SPRITES_H
#define EXAMPLE_SPRITES_H

#include "raylib.h"

#define SPRITE_X_SCALE 64
#define SPRITE_Y_SCALE 32

#define SPRITE_ROUTER 0
#define SPRITE_SERVER 1

typedef struct {
    Rectangle rect; // Section of the spritesheet
    Vector2 offset; // offset by which to render, in order to align to isometric grid
} SpriteRect;

extern SpriteRect sprite_sheet[];

Texture2D load_sprite_sheet();

#endif //EXAMPLE_SPRITES_H
