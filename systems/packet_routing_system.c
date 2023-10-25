#include <stdio.h>
#include "packet_routing_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"
#include "../utils/timer.h"
#include "../components/wire.h"

Timer timer;

//#define TIME_PACKET_SEND 0.025
#define TIME_PACKET_SEND 1.0



int send_packet(char* fromEntity, Packet* packet) {
    Wire* wire = NULL;

    // Search all wires to see if the destination is immediately adjacent
//    Device* device = find_device_by_address(packet->toAddress);
//    if (device) {
//        wire = search_wire_by_entityIds(fromEntity, device->entityId);
//
//        if (strcmp(wire->entityA, fromEntity) == 0) {
//            packet_queue_write(&wire->sendQtoB, packet);
//            return 1;
//        } else if (strcmp(wire->entityB, fromEntity) == 0) {
//            packet_queue_write(&wire->sendQtoA, packet);
//            return 1;
//        }
//        return 0;
//    }

    // Look up in route table
    RouteTable* routeTable = g_hash_table_lookup(componentRegistry.routeTable, fromEntity);
    if (!routeTable) return 0;

    int starIdx = -1;
    int foundIdx = -1;
    for (int i = 0; i < routeTable->numRecords; i++) {
        char* prefix = routeTable->records[i].prefix;
        if (strcmp(prefix, "*") == 0) {
            starIdx = i;
        }
        if (strncmp(prefix, packet->toAddress, strlen(prefix)) == 0) {
            foundIdx = i;
        }
    }

    if (starIdx == -1 && foundIdx == -1) return 0;

    if (foundIdx > -1) {
        wire = g_hash_table_lookup(componentRegistry.wires, routeTable->records[foundIdx].wireEntityId);
    } else if (starIdx > -1) {
        wire = g_hash_table_lookup(componentRegistry.wires, routeTable->records[starIdx].wireEntityId);
    }
    if (!wire) return 0;

    if (strcmp(wire->entityA, fromEntity) == 0) {
        packet_queue_write(&wire->sendQtoB, packet);
        return 1;
    } else if (strcmp(wire->entityB, fromEntity) == 0) {
        packet_queue_write(&wire->sendQtoA, packet);
        return 1;
    }

    return 0;
}

/**
 * returns 0 if error, 1 if success
 */
int send_packetX(char* fromEntity, Packet* packet, Connection* connection) {
    char** path = g_strsplit(packet->toAddress, ".", 10);
    int parts = 0;
    while (path[parts]) {
        parts++;
    }

    // Searching for top
    if (!packet->topLevelFound) {
        char* curPart = path[0];
        Device* curDevice = (Device*)g_hash_table_lookup(componentRegistry.devices, fromEntity);

        // Shitty prepend, update from address
        char fromAddress[110];
        strcpy(fromAddress, curDevice->name);
        if (strlen(packet->fromAddress) > 0) {
            strcat(fromAddress, ".");
            strcat(fromAddress, packet->fromAddress);
        }
        strcpy(packet->fromAddress, fromAddress);

        if (strcmp(curPart, curDevice->name) == 0) {
            // End search! Found!
            packet->topLevelFound = true;
        } else {
            char* toEntity = connection->parentEntityId;

            PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, toEntity);
            if (!to_packet_buffer) return 0;

            strcpy(connection->activeEntityId, toEntity);
            packet_queue_write(&to_packet_buffer->recvQ, packet);
            return 1;
        }
    }
    if (packet->topLevelFound) {
        // Top found, traverse back down
        packet->hops++;

        if (packet->hops < parts) { // More hops to go
            char* cur_part = path[packet->hops];

            for (int i = 0; i < connection->numConns; i++) {
                char* toEntity = find_device_entity_id_by_device_id(cur_part);

                if (strcmp(toEntity, connection->toEntityIds[i]) == 0) {
                    PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, toEntity);
                    strcpy(connection->activeEntityId, toEntity);
                    packet_queue_write(&to_packet_buffer->recvQ, packet);
                    return 1;
                }
            }
            // Not found!
            return 0;
        }
    }
    return 0;
}

void update_routers() {
    GHashTableIter iter;
    guint* key_;

    Device* device;
    g_hash_table_iter_init(&iter, componentRegistry.devices);
    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &device)) {
        if (device->type != DEVICE_TYPE_ROUTER) continue;

        PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, key_);
        Packet* packet = packet_queue_read(&packet_buffer->recvQ);
        while (packet) {
            if (strcmp(packet->toAddress, device->address) != 0) {
                packet_queue_write(&packet_buffer->sendQ, packet);
            } else {
                // Packet is sent to this router! Do something with it
            }
            packet = packet_queue_read(&packet_buffer->recvQ);
        }
    }
}

void update_packet_buffer(char* entityId, PacketBuffer* packetBuffer) {
    // Pop one and send
    Packet* packet = packet_queue_read(&packetBuffer->sendQ);
//    Connection* connection = (Connection*) g_hash_table_lookup(componentRegistry.connections, entityId);

    if (packet) {
//        send_packet(entityId, packet, connection);
        send_packet(entityId, packet);
    } else {
        // clear active connection
//        connection->activeEntityId[0] = '\0';
    }
}

void update_wires() {
    GHashTableIter iter;
    char* entityId;
    Wire* wire;

    g_hash_table_iter_init(&iter, componentRegistry.wires);
    while (g_hash_table_iter_next (&iter, (gpointer) &entityId, (gpointer) &wire)) {
        Packet* packet = packet_queue_read(&wire->sendQtoA);
        if (packet) {
            PacketBuffer* packetBuffer = g_hash_table_lookup(componentRegistry.packetBuffers, wire->entityA);
            if (packetBuffer) packet_queue_write(&packetBuffer->recvQ, packet);
        }
        packet = packet_queue_read(&wire->sendQtoB);
        if (packet) {
            PacketBuffer* packetBuffer = g_hash_table_lookup(componentRegistry.packetBuffers, wire->entityB);
            if (packetBuffer) packet_queue_write(&packetBuffer->recvQ, packet);
        }
    }
}

void initialize_packet_routing_system() {
    StartTimer(&timer, 1);
}

void update_packet_routing_system() {
    if (!TimerDone(timer)) return;

    iterate_packet_buffers(update_packet_buffer);
    update_routers();
    update_wires();

    StartTimer(&timer, TIME_PACKET_SEND);
}

void render_packet_routing_system() {
}
