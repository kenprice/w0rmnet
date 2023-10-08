#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H

#include "raylib.h"

#define DEVICE_ID_LEN 32

// Types
typedef struct {
    char device_id[DEVICE_ID_LEN];
    char** path; // Sequence of device IDs
} Address; // IP-like address

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
    char fingerprint[32];
} Process; // Models a running program

typedef struct {
    char id[32];
} OperatingSystem;

// Composite components
typedef struct {
    char from_device_id[DEVICE_ID_LEN];
    char to_device_id[DEVICE_ID_LEN];
} Connection; // Wire-like structure

typedef struct {
    Connection conns[100];
    int max_conns;
} NetworkInterface;

typedef struct {
    Connection conn; // Connection this packet is currently in
    char* message;
} Packet;

#endif //EXAMPLE_COMPONENTS_H
