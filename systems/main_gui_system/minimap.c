#include <stdio.h>
#include <math.h>
#include "minimap.h"
#include "../../lib/raygui.h"
#include "../../lib/raymath.h"
#include "../../utils/uuid.h"
#include "../../world/world_map.h"

#define MINIMAP_SPRING_MAX_ITER 100

const float C1 = 2;
const float C2 = 5;
const float C3 = 1.5;
const float C4 = 0.1;

typedef struct MinimapNode {
    Vector2 vector;
    char entityId[UUID_STR_LEN];
    struct MinimapNode** children;
} MinimapNode;

typedef struct {
    MinimapNode* nodeA;
    MinimapNode* nodeB;
} MinimapEdge;

typedef struct {
    Rectangle rect;
    MinimapNode nodes[100];
    int numNodes;
    MinimapEdge edges[100];
    int numEdges;

    int iterations;
} MinimapState;

MinimapState minimapState;

#define MINIMAP_VIEW_HEIGHT 280

int bfs(MinimapNode* root) {

}

MinimapNode* generate_tree() {
    float x = 10, y = 10;

    MinimapNode* root = calloc(1, sizeof(MinimapNode));
    root->vector = (Vector2){MINIMAP_VIEW_HEIGHT / 2, 5};
    root->children = calloc(worldMap.numRegions, sizeof(MinimapNode));

    for (int r = 0; r < worldMap.numRegions; r++) {
        Region region = worldMap.regions[r];
        MinimapNode* regionNode = calloc(1, sizeof(MinimapNode));
        regionNode->vector = (Vector2){ x, y };
        root->children[r] = regionNode;
        y += 10;

        regionNode->children = calloc(region.numZones, sizeof(MinimapNode*));
        for (int z = 0; z < region.numZones; z++) {
            Zone zone = region.zones[z];
            MinimapNode* zoneNode = calloc(1, sizeof(MinimapNode));
            zoneNode->vector = (Vector2){ x, y };
            regionNode->children[z] = zoneNode;
            y += 10;

            zoneNode->children = calloc(zone.numAreas, sizeof(MinimapNode*));
            for (int a = 0; a < zone.numAreas; a++) {
                Area area = zone.areas[a];
                MinimapNode *areaNode = calloc(1, sizeof(MinimapNode));
                areaNode->vector = (Vector2) {x, y};
                zoneNode->children[a] = areaNode;
                y += 10;

                areaNode->children = calloc(area.numEntities, sizeof(MinimapNode*));
                for (int i = 0; i < area.numEntities; i++) {
                    MinimapNode* node = calloc(1, sizeof(MinimapNode));
                    node->vector = (Vector2) {x, y};
                    areaNode->children[i] = node;
                }
                y -= 10;
                x += 5;
            }

            y -= 10;
            x += 5;
        }
        y -= 10;
        x += 10;
    }
}

void init_minimap_view(Rectangle rect) {
    minimapState.rect = rect;
    minimapState.rect.height = MINIMAP_VIEW_HEIGHT;

    minimapState.numNodes = 9;
    minimapState.numEdges = 8;



    minimapState.nodes[0].vector = (Vector2){ 140, 140 };
    minimapState.nodes[1].vector = (Vector2){ 200, 140 };
    minimapState.nodes[2].vector = (Vector2){ 130, 200 };
    minimapState.nodes[3].vector = (Vector2){ 30, 150 };
    minimapState.nodes[4].vector = (Vector2){ 200, 200 };
    minimapState.nodes[5].vector = (Vector2){ 20, 30 };
    minimapState.nodes[6].vector = (Vector2){ 40, 50 };
    minimapState.nodes[7].vector = (Vector2){ 60, 70 };
    minimapState.nodes[8].vector = (Vector2){ 80, 90 };

    minimapState.edges[0].nodeA = &minimapState.nodes[0];
    minimapState.edges[0].nodeB = &minimapState.nodes[1];
    minimapState.edges[1].nodeA = &minimapState.nodes[0];
    minimapState.edges[1].nodeB = &minimapState.nodes[2];
    minimapState.edges[2].nodeA = &minimapState.nodes[0];
    minimapState.edges[2].nodeB = &minimapState.nodes[3];
    minimapState.edges[3].nodeA = &minimapState.nodes[1];
    minimapState.edges[3].nodeB = &minimapState.nodes[4];
    minimapState.edges[4].nodeA = &minimapState.nodes[0];
    minimapState.edges[4].nodeB = &minimapState.nodes[5];
    minimapState.edges[5].nodeA = &minimapState.nodes[0];
    minimapState.edges[5].nodeB = &minimapState.nodes[6];
    minimapState.edges[6].nodeA = &minimapState.nodes[0];
    minimapState.edges[6].nodeB = &minimapState.nodes[7];
    minimapState.edges[7].nodeA = &minimapState.nodes[0];
    minimapState.edges[7].nodeB = &minimapState.nodes[8];

    minimapState.iterations = 0;
}

void update_minimap_view(Rectangle rect) {
    if (minimapState.iterations > MINIMAP_SPRING_MAX_ITER) return;

    minimapState.rect = rect;
    minimapState.rect.height = MINIMAP_VIEW_HEIGHT;

    Vector2 force[minimapState.numNodes];

    for (int i = 0; i < minimapState.numNodes; i++) {
        MinimapNode* nodeA = &minimapState.nodes[i];

        force[i] = (Vector2){ 0, 0 };

        for (int j = 0; j < minimapState.numEdges; j++) {
            MinimapEdge edge = minimapState.edges[j];
            MinimapNode* nodeB = edge.nodeA == nodeA ? edge.nodeB : edge.nodeA;
            if (nodeA == nodeB) continue;

            // Vector in the direction of point 1 to point 2
            Vector2 forceVector = Vector2Subtract(nodeB->vector, nodeA->vector);
            // Normalize the vector so that we can scale it
            if (forceVector.x == 0 && forceVector.y == 0) {
                // Dunno what to do with zero vectors; shouldn't happen.
                continue;
            } else if (forceVector.x == 0) {
                // RayMath turns this into Inf unless we account for x or y being 0
                forceVector.y = 1;
            } else if (forceVector.y == 0) {
                forceVector.x = 1;
            } else {
                forceVector = Vector2Normalize(forceVector);
            }

            float distance = Vector2Distance(nodeB->vector, nodeA->vector);

            // From Spring algorithm
            if (edge.nodeA == nodeA || edge.nodeB == nodeA) {
                if (distance == C2) continue;
                // Attractive force
                forceVector = Vector2Scale(forceVector, C1 * log(distance / C2));
                force[i] = Vector2Add(force[i], forceVector);
            } else {
                // Repulsive force
                forceVector = Vector2Scale(forceVector, -(C3 / sqrt(distance)));
                force[i] = Vector2Add(force[i], forceVector);
            }
        }

    }

    for (int i = 0; i < minimapState.numNodes; i++) {
        MinimapNode *node = &minimapState.nodes[i];
        node->vector = Vector2Add(node->vector, force[i]);
    }

    minimapState.iterations++;
}

void render_minimap_view(Rectangle rect) {
    GuiPanel(minimapState.rect, NULL);

    for (int j = 0; j < minimapState.numEdges; j++) {
        MinimapEdge edge = minimapState.edges[j];

        DrawLine(minimapState.rect.x + edge.nodeA->vector.x,
                 minimapState.rect.y + edge.nodeA->vector.y,
                 minimapState.rect.x + edge.nodeB->vector.x,
                 minimapState.rect.y + edge.nodeB->vector.y,
                 GRAY);
    }

    for (int i = 0; i < minimapState.numNodes; i++) {
        MinimapNode* node = &minimapState.nodes[i];
        DrawCircle(minimapState.rect.x + node->vector.x, minimapState.rect.y + node->vector.y, 2, WHITE);
    }
}
