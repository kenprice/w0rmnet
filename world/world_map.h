#ifndef W0RMNET_WORLD_MAP_H
#define W0RMNET_WORLD_MAP_H

#include "../components/components.h"

#define REGION_ID_MAX_LEN 20
#define ZONE_ID_MAX_LEN 20

#define AREA_ID_SPECIAL_PLAYER 0

/*
 * World Map is separated into three layers:
 * Region
 * Zone
 * Area (decoupled from particular entities, zones, region, but respects hierarchical relationship usually)
 */

/*
 * Area is the thing that contains devices
 */
typedef struct {
    char areaId[UUID_STR_LEN];
    char areaName[100];
    char parentRegionId[REGION_ID_MAX_LEN];
    char parentZoneId[ZONE_ID_MAX_LEN];

    // Entities in area
    char entities[100][UUID_STR_LEN];
    int numEntities;

    // Map stuff
    int width;
    int height;
} Area;

typedef struct {
    char zoneId[ZONE_ID_MAX_LEN];
    char zoneLabel[100];
    char parentRegion[REGION_ID_MAX_LEN];

    Area areas[100];
    int numAreas;

    // Special entities
    char gateway[UUID_STR_LEN]; // Gateway for zone
} Zone;

typedef struct {
    char regionId[REGION_ID_MAX_LEN];
    char regionLabel[100];

    Zone zones[100];
    int numZones;
} Region;

typedef struct {
    Region regions[20];
    int numRegions;

    Area* playerArea;
} WorldMap;

extern WorldMap worldMap;

void initialize_world();

bool is_entity_in_area(Area area, char* entityId);

#endif //W0RMNET_WORLD_MAP_H
