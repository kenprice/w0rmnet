#include <glib.h>
#include <stdio.h>
#include <math.h>
#include "minimap.h"
#include "../../lib/raygui.h"
#include "../../lib/raymath.h"
#include "../../utils/uuid.h"
#include "../../world/world_map.h"
#include "../../components/component_registry.h"

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
    Rectangle rect;

    MinimapNode* treeRoot;
    GList* radialNodes;

    int iterations;
} MinimapState;

MinimapState minimapState;

#define MINIMAP_VIEW_HEIGHT 280

int count_leaves_in_network(char* routerEntityId);

typedef struct RadialNode {
    char* entityId;
    double x;
    double y;
    struct RadialNode* parentNode;
} RadialNode;

static const int CircleRadius = 10;
static const int CircleRadiusDelta = 30;

/**
 * Interactive, tree-based graph visualization
 * Andrew Pavlo
 * pg 18
 * https://scholarworks.rit.edu/cgi/viewcontent.cgi?referer=&httpsredir=1&article=1355&context=theses
 * @param rootTreeId Entity ID of root of tree
 * @param vertexId Entity ID of current node, start with root of tree
 * @param parentNode Parent node of children created in a recursive call, start with NULL
 * @param alpha Lower bound of annulus wedge in radians, start with 0
 * @param beta Upper bound of annulus wedge in radians, start with 2pi
 * @param depth Depth of subtree, Start with 1
 * @param visited_ List of visited nodes, start with NULL
 * @param outputGraph Output
 */
void radial_positions(char* rootTreeId, char* vertexId, RadialNode* parentNode_, double alpha, double beta, int depth, GList* visited_, GList* outputGraph) {
    RadialNode* parentNode = parentNode_;
    if (strcmp(vertexId, rootTreeId) == 0) {
        RadialNode* node = calloc(1, sizeof(RadialNode));
        node->entityId = vertexId;
        node->x = 0;
        node->y = 0;
        node->parentNode = NULL;
        g_list_append(outputGraph, node);
        parentNode = node;
    }

    double theta = alpha;
    double radius = CircleRadius + (CircleRadiusDelta * depth);
    int leaves = count_leaves_in_network(vertexId);

    GList* visited = visited_ ? visited_ : g_list_alloc();
    g_list_append(visited, vertexId);

    RouteTable* routeTable = g_hash_table_lookup(componentRegistry.routeTable, vertexId);

    for (int i = 0; i < routeTable->numRecords; i++) {
        if (strcmp(routeTable->records[i].prefix, "*") == 0) continue;
        Wire* wire = g_hash_table_lookup(componentRegistry.wires, routeTable->records[i].wireEntityId);
        char* childDeviceId = strcmp(vertexId, wire->entityA) == 0 ? wire->entityB : wire->entityA;
        Device* childDevice = g_hash_table_lookup(componentRegistry.devices, childDeviceId);
        if (!childDevice) continue;
        if (!g_list_find(visited, childDevice->entityId)) {
            g_list_append(visited, childDevice->entityId);
        } else {
            continue;
        }

        int lambda = count_leaves_in_network(childDevice->entityId);
        double mi = theta + ((double)lambda / leaves * (beta - alpha));

        RadialNode* node = calloc(1, sizeof(RadialNode));
        node->entityId = childDeviceId;
        node->x = radius * cos((theta + mi) / 2.0);
        node->y = radius * sin((theta + mi) / 2.0);
        node->parentNode = parentNode;
        g_list_append(outputGraph, node);

        if (childDevice->type == DEVICE_TYPE_ROUTER && lambda > 0) {
            radial_positions(rootTreeId, childDevice->entityId, node, theta, mi, depth + 1, visited, outputGraph);
        }

        theta = mi;
    }
};

/**
 * Count leaves in the network tree.
 * Assumptions:
 * - non-router devices are leaves
 */
int count_leaves_in_network(char* routerEntityId) {
    Device* device = g_hash_table_lookup(componentRegistry.devices, routerEntityId);

    if (device->type != DEVICE_TYPE_ROUTER) return 1;

    GList* visited = g_list_alloc();
    GQueue* q = g_queue_new();

    g_list_append(visited, device);
    g_queue_push_tail(q, device);

    int leaves = 0;

    while (q->length) {
        Device* current = g_queue_pop_head(q);
        RouteTable* routeTable = g_hash_table_lookup(componentRegistry.routeTable, current->entityId);

        for (int i = 0; i < routeTable->numRecords; i++) {
            if (strcmp(routeTable->records[i].prefix, "*") == 0) continue;
            Wire* wire = g_hash_table_lookup(componentRegistry.wires, routeTable->records[i].wireEntityId);
            char* childDeviceId = strcmp(current->entityId, wire->entityA) == 0 ? wire->entityB : wire->entityA;

            Device* childDevice = g_hash_table_lookup(componentRegistry.devices, childDeviceId);
            if (!childDevice) continue;
            if (childDevice->type == DEVICE_TYPE_ROUTER) {
                if (!g_list_find(visited, childDevice)) {
                    g_list_append(visited, childDevice);
                    g_queue_push_tail(q, childDevice);
                }
            } else {
                leaves++;
            }
        }
    }

    return leaves;
}



MinimapNode* generate_tree() {
    float x = 10, y = 10;

    MinimapNode* root = calloc(1, sizeof(MinimapNode));
    root->vector = (Vector2){MINIMAP_VIEW_HEIGHT / 2, 5};
    root->children = calloc(worldMap.numRegions, sizeof(MinimapNode*));

    for (int r = 0; r < worldMap.numRegions; r++) {
        Region* region = &worldMap.regions[r];
        MinimapNode* regionNode = calloc(1, sizeof(MinimapNode));
        regionNode->vector = (Vector2){ x, y };
        root->children[r] = regionNode;
        y += 10;

        regionNode->children = calloc(region->numZones, sizeof(MinimapNode*));
        for (int z = 0; z < region->numZones; z++) {
            Zone* zone = &region->zones[z];
            MinimapNode* zoneNode = calloc(1, sizeof(MinimapNode));
            zoneNode->vector = (Vector2){ x, y };
            regionNode->children[z] = zoneNode;
            y += 10;

            zoneNode->children = calloc(zone->numAreas, sizeof(MinimapNode*));
            for (int a = 0; a < zone->numAreas; a++) {
                Area* area = &zone->areas[a];
                MinimapNode *areaNode = calloc(1, sizeof(MinimapNode));
                areaNode->vector = (Vector2) {x, y};
                zoneNode->children[a] = areaNode;
                y += 10;

                int numAreaEntities = 0;
                for (int i = 0; i < area->numEntities; i++) {
                    Device* device = g_hash_table_lookup(componentRegistry.devices, area->entities[i]);
                    if (device) numAreaEntities++;
                }
                areaNode->children = calloc(numAreaEntities, sizeof(MinimapNode*));
                for (int i = 0; i < numAreaEntities; i++) {
                    MinimapNode* node = calloc(1, sizeof(MinimapNode));
                    node->vector = (Vector2) {x, y};
                    areaNode->children[i] = node;
                    x += 5;
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

    return root;
}

void init_minimap_view(Rectangle rect) {
    minimapState.rect = rect;
    minimapState.rect.height = MINIMAP_VIEW_HEIGHT;

    minimapState.treeRoot = generate_tree();

    GList* outputGraph = g_list_alloc();
    char* networkRoot = worldMap.regions[0].zones[0].gateway;
    radial_positions(networkRoot, networkRoot, NULL, 0, 2 * PI, 0, NULL, outputGraph);

    minimapState.radialNodes = outputGraph->next;
}

void render_minimap_view(Rectangle rect__) {
    GuiPanel(minimapState.rect, NULL);

    Rectangle rect = minimapState.rect;
    rect.x += 140;
    rect.y += 140;

    GList* curNodeEntry = minimapState.radialNodes;
    while (curNodeEntry && curNodeEntry->data) {
        RadialNode* node = (RadialNode*)curNodeEntry->data;
        if (node->parentNode) {
            DrawLine(rect.x + node->x, rect.y + node->y,
                     rect.x + node->parentNode->x, rect.y + node->parentNode->y, GRAY);
        }
        curNodeEntry = curNodeEntry->next;
    }

    curNodeEntry = minimapState.radialNodes;
    while (curNodeEntry && curNodeEntry->data) {
        RadialNode* node = (RadialNode*)curNodeEntry->data;
        DrawCircle(rect.x + node->x, rect.y + node->y, 2, WHITE);
        curNodeEntry = curNodeEntry->next;
    }
}
