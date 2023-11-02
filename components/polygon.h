#ifndef W0RMNET_POLYGON_H
#define W0RMNET_POLYGON_H

typedef struct {
    int x;
    int y;
} PolyPoint;

typedef struct {
    PolyPoint points[10];
    int numPoints;
    int bisectAt;
} Polygon;

char* register_polygon(Polygon polygon, char* entityId);

Polygon comp_polygon_points2(int x1, int y1, int x2, int y2);

Polygon comp_polygon_points4(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

#endif //W0RMNET_POLYGON_H
