//
// Created by Ken Price on 10/7/23.
//

#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "sprites.h"

// Components
typedef struct {
    char id[32];
} Device;

typedef struct {
    Vector2 coord;
} Position;

typedef struct {
    int sprite_id;
} Sprite;

typedef struct {
    char id[32];
} OperatingSystem;

typedef struct {
    GHashTable* devices;
    GHashTable* positions;
    GHashTable* sprites;
} ComponentRegistry;


// Entities
typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    OperatingSystem os;
} Machine;

typedef struct {
    Device device;
    Position position;
    Sprite sprite;
} Router;

void init_component_registry(ComponentRegistry* registry);

void create_entities(ComponentRegistry* registry);

// System
void device_rendering_system(Texture2D texture, ComponentRegistry* registry);

#endif //EXAMPLE_COMPONENTS_H
