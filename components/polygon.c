#include <glib.h>
#include <string.h>
#include "polygon.h"
#include "component_registry.h"

char* register_polygon(Polygon polygon, char* entityId) {
    Polygon* newPolygon = calloc(1, sizeof(Polygon));
    memcpy(newPolygon, &polygon, sizeof(Polygon));

    g_hash_table_insert(componentRegistry.polygons, entityId, newPolygon);

    return entityId;
}
