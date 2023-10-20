#include <stdio.h>
#include "router.h"
#include "../components/component_registry.h"

Router* entity_router_create_blank() {
    Router* router = calloc(1, sizeof(Router));
    char *entity_id = generate_uuid();
    strcpy(router->entityId, entity_id);

    char *device_id = generate_device_id();
    router->device.type = DEVICE_TYPE_ROUTER;
    router->device.owner = DEVICE_OWNER_NOBODY;
    router->device.pwned = DEVICE_NOT_PWNED;
    strncpy(router->device.entityId, entity_id, UUID_STR_LEN);
    strncpy(router->device.name, device_id, DEVICE_NAME_LEN);

    router->position.coord.x = 0;
    router->position.coord.y = 0;

    router->sprite.spriteId = SPRITE_ROUTER;

    router->connection.numConns = 0;
    router->connection.maxConns = 100;

    strncpy(router->packetBuffer.entityId, entity_id, UUID_STR_LEN);
    router->packetBuffer.sendQ = packet_queue_alloc(50);
    router->packetBuffer.recvQ = packet_queue_alloc(50);

    return router;
}

char* entity_router_register_components(Router router) {
    char* entity_id = strdup(router.entityId);

    register_device(router.device, entity_id);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &router.position, sizeof(Position));
    g_hash_table_insert(componentRegistry.positions, entity_id, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &router.sprite, sizeof(Sprite));
    g_hash_table_insert(componentRegistry.sprites, entity_id, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &router.connection, sizeof(Connection));
    g_hash_table_insert(componentRegistry.connections, entity_id, connection);

    PacketBuffer* packet_buffer = calloc(1, sizeof(PacketBuffer));
    packet_buffer->sendQ = packet_queue_alloc(10);
    packet_buffer->recvQ = packet_queue_alloc(10);
    strcpy(packet_buffer->entityId, entity_id);
    g_hash_table_insert(componentRegistry.packetBuffers, entity_id, packet_buffer);

    return entity_id;
}

/**
 * Tab-separated values of serialized components.
 * Serialized components themselves are semicolon-separated values
 *
 * @param router
 * @return
 */
char* entity_router_serialize(Router router) {
    char buffer[10000] = "";

    char* strDevice = comp_device_serialize(&router.device);
    char* strPosition = comp_position_serialize(&router.position);
    char* strSprite = comp_sprite_serialize(&router.sprite);
    char* strConnection = comp_connection_serialize(&router.connection);
    char* strPacketBuffer = comp_packet_buffer_serialize(&router.packetBuffer);

    sprintf(buffer, "%s\t%s\t%s\t%s\t%s\t%s", router.entityId, strDevice, strPosition, strSprite, strConnection, strPacketBuffer);

    char* data = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(data, buffer);

    free(strDevice);
    free(strPosition);
    free(strSprite);
    free(strConnection);
    free(strPacketBuffer);

    return data;
}

Router entity_router_deserialize(char* data) {
    char** routerData = g_strsplit(data, "\t", 7);

    Router router;
    strcpy(router.entityId, routerData[0]);
    router.device = *(comp_device_deserialize(routerData[1]));
    router.position = *(comp_position_deserialize(routerData[2]));
    router.sprite = *(comp_sprite_deserialize(routerData[3]));
    router.connection = *(comp_connection_deserialize(routerData[4]));
    router.packetBuffer = *(comp_packet_buffer_deserialize(routerData[5]));

    return router;
}
