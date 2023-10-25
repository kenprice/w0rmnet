#ifndef W0RMNET_POLYGON_H
#define W0RMNET_POLYGON_H

typedef struct {
    int x;
    int y;
} PolyPoint;

typedef struct {
    PolyPoint points[10];
    int numPoints;
} Polygon;

char* register_polygon(Polygon polygon, char* entityId);

#endif //W0RMNET_POLYGON_H
