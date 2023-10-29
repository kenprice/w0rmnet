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

Polygon comp_polygon_points2(int x1, int y1, int x2, int y2) {
    Polygon wirePolygon;
    wirePolygon.numPoints = 2;
    wirePolygon.points[0] = (PolyPoint){ x1, y1 };
    wirePolygon.points[1] = (PolyPoint){ x2, y2 };
    return wirePolygon;
}

Polygon comp_polygon_points4(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    Polygon wirePolygon;
    wirePolygon.numPoints = 4;
    wirePolygon.points[0] = (PolyPoint){ x1, y1 };
    wirePolygon.points[1] = (PolyPoint){ x2, y2 };
    wirePolygon.points[2] = (PolyPoint){ x3, y3 };
    wirePolygon.points[3] = (PolyPoint){ x4, y4 };
    return wirePolygon;
}
