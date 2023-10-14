#ifndef W0RMNET_WORLD_MAP_H
#define W0RMNET_WORLD_MAP_H

#include "../components/components.h"

#define AREA_ID_SPECIAL_PLAYER 0

/*
 * World Map is separated into three layers:
 * Region
 * Zone
 * Area
 */

/*
 * Area is the thing that contains devices
 */
typedef struct {
    int area_id;
    char area_name[100];

    char entities[100][UUID_STR_LEN];
    int num_entities;

    int width;
    int height;
} Area;

extern Area player_area;
extern Area area_registry[100];
extern int area_current;

void init_player_area();

#endif //W0RMNET_WORLD_MAP_H
