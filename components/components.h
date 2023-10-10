#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H

#include "raylib.h"
#include "connection.h"
#include "device.h"
#include "packet.h"

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

// Operating System Components

typedef struct {
    char fingerprint[32]; // Identifies the program being run
    char name[32]; // Name of program
} Process; // Models a running program

typedef struct {
    char id[32];
} OperatingSystem;

#endif //EXAMPLE_COMPONENTS_H
