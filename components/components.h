#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H

#include "raylib.h"
#include "connection.h"
#include "device.h"
#include "packet.h"
#include "sprite.h"

// RouteTable
typedef struct {
    char dest[DEVICE_ID_LEN];
    char via[DEVICE_ID_LEN];
} RouteTableRecord;

typedef struct {
    RouteTableRecord records[100];
    int num_records;
    int max_records;
} RouteTable;

// Components

typedef struct {
    Vector2 coord;
} Position;

// Operating System Components

typedef struct {
    char fingerprint[32]; // Identifies the program being run
    char name[32]; // Name of program
} Process; // Models a running program

typedef struct {
    char id[32];
} OperatingSystem;

#endif //EXAMPLE_COMPONENTS_H
