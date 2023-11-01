#include <glib.h>
#include <math.h>
#include "minimap.h"
#include "../../lib/raygui.h"
#include "../../utils/uuid.h"
#include "../../world/world_map.h"
#include "../../components/component_registry.h"

#define MINIMAP_VIEW_HEIGHT 280
static const int CircleRadius = 15;
static const int CircleRadiusDelta = 35;

typedef struct {
    Rectangle rect;

    GList* radialNodes;
    char* networkRoot;

    int iterations;
} MinimapState;

typedef struct RadialNode {
    char* entityId;
    double x;
    double y;
    struct RadialNode* parentNode;
} RadialNode;

int count_leaves_in_network(char* routerEntityId);

MinimapState minimapState;

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
        if (!childDevice || !childDevice->visible) continue;
        if (!g_list_find(visited, childDevice->entityId)) {
            g_list_append(visited, childDevice->entityId);
        } else {
            continue;
        }

        int lambda = count_leaves_in_network(childDevice->entityId);
        double mi = theta + ((double)lambda / leaves * (beta - alpha));

        RadialNode* node = calloc(1, sizeof(RadialNode));
        node->entityId = childDeviceId;

        double jitter = depth == 2 ? (i%2)*4 : 0;
        node->x = (jitter + radius) * cos((theta + mi) / 2.0);
        node->y = (jitter + radius) * sin((theta + mi) / 2.0);
        node->parentNode = parentNode;
        g_list_append(outputGraph, node);

        if (childDevice->type == DEVICE_TYPE_ROUTER && lambda > 0) {
            radial_positions(rootTreeId, childDevice->entityId, node, theta, mi, depth + 1, visited, outputGraph);
        }

        theta = mi;
    }

    if (depth == 0) {
        g_list_free(visited);
    }
}

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
            if (!childDevice || !childDevice->visible) continue;
            if (childDevice->type == DEVICE_TYPE_ROUTER) {
                if (!g_list_find(visited, childDevice)) {
                    g_list_append(visited, childDevice);
                    g_queue_push_tail(q, childDevice);
                }

                if (count_leaves_in_network(childDevice->entityId) == 0) {
                    leaves++;
                }
            } else {
                leaves++;
            }
        }
    }

    g_list_free(visited);
    g_queue_free(q);

    return leaves;
}

void init_minimap_view(Rectangle rect) {
    minimapState.rect = rect;
    minimapState.rect.height = MINIMAP_VIEW_HEIGHT;

    GList* outputGraph = g_list_alloc();
    char* networkRoot = worldMap.regions[0].zones[0].gateway;
    minimapState.networkRoot = networkRoot;
    radial_positions(networkRoot, networkRoot, NULL, 0, 2 * PI, 0, NULL, outputGraph);

    minimapState.radialNodes = outputGraph->next;
}

void update_minimap_view(Rectangle rect) {
    minimapState.rect = rect;
    minimapState.rect.height = MINIMAP_VIEW_HEIGHT;

    g_list_free_full(minimapState.radialNodes, free);

    GList* outputGraph = g_list_alloc();
    char* networkRoot = worldMap.regions[0].zones[0].gateway;
    radial_positions(networkRoot, networkRoot, NULL, 0, 2 * PI, 0, NULL, outputGraph);

    minimapState.radialNodes = outputGraph->next;
}

void render_minimap_view() {
    GuiPanel(minimapState.rect, NULL);

    Rectangle rect = minimapState.rect;
    rect.x += 140;
    rect.y += 140;

    GList* curNodeEntry = minimapState.radialNodes;
    while (curNodeEntry && curNodeEntry->data) {
        RadialNode* node = (RadialNode*)curNodeEntry->data;

        if (node->parentNode) {
            Wire* wire = search_wire_by_entity_ids(node->entityId, node->parentNode->entityId);
            bool active = wire && (wire->sendQtoA.head != wire->sendQtoA.tail || wire->sendQtoB.head != wire->sendQtoB.tail);
            DrawLine(rect.x + node->x, rect.y + node->y,
                     rect.x + node->parentNode->x, rect.y + node->parentNode->y, active ? GREEN : GRAY);
        }
        curNodeEntry = curNodeEntry->next;
    }

    curNodeEntry = minimapState.radialNodes;
    while (curNodeEntry && curNodeEntry->data) {
        RadialNode* node = (RadialNode*)curNodeEntry->data;

        if (node->parentNode) {
            Device* device = g_hash_table_lookup(componentRegistry.devices, node->entityId);
            bool isOwned = device && device->owner == DEVICE_OWNER_PLAYER;
            DrawCircle(rect.x + node->x, rect.y + node->y, 2, isOwned ? RED : WHITE);
        } else {

            DrawCircle(rect.x + node->x, rect.y + node->y, 3, BLUE);
        }

        curNodeEntry = curNodeEntry->next;
    }
}
