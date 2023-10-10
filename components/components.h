#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H

#include "raylib.h"
#include "../utils/uuid.h"
#include "connection.h"
#include "device.h"

// Types
typedef struct {
    char device_id[DEVICE_ID_LEN];
    char** path; // Sequence of device IDs
} Address; // IP-like address

// Components

typedef struct {
    Vector2 coord;
} Position;

typedef struct {
    int sprite_id;
} Sprite;

typedef struct {
    char fingerprint[32];
} Process; // Models a running program

typedef struct {
    char id[32];
} OperatingSystem;

typedef struct {
    char* address;
    char* message;
} Packet;

#endif //EXAMPLE_COMPONENTS_H
