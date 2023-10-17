#include "packet_routing_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"
#include "../utils/timer.h"

Timer timer;

#define TIME_PACKET_SEND 0.1

/**
 * returns 0 if error, 1 if success
 */
int send_packet(char* entity_id, Packet* packet, Connection* connection) {
    char buffer[200];
    strcpy(buffer, packet->toAddress);
    char* token = strtok(buffer, ".");
    char path[10][11];
    int parts = 0;
    while (token != NULL) {
        strcpy(path[parts], token);
        token = strtok(NULL, ".");
        parts++;
    }

    // Searching for top
    if (!packet->topLevelFound) {
        char* cur_part = path[0];
        Device* cur_device = (Device*)g_hash_table_lookup(componentRegistry.devices, entity_id);

        // Shitty prepend, update from address
        char from_address[110];
        strcpy(from_address, cur_device->name);
        if (strlen(packet->fromAddress) > 0) {
            strcat(from_address, ".");
            strcat(from_address, packet->fromAddress);
        }
        strcpy(packet->fromAddress, from_address);

        if (strcmp(cur_part, cur_device->name) == 0) {
            // End search! Found!
            packet->topLevelFound = true;
        } else {
            char* to_entity = connection->parentEntityId;

            PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, to_entity);
            if (!to_packet_buffer) return 0;

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
                char* to_entity = find_device_entity_id_by_device_id(cur_part);

                if (strcmp(to_entity, connection->toEntityIds[i]) == 0) {
                    PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, to_entity);
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
            packet_queue_write(&packet_buffer->sendQ, packet);
            packet = packet_queue_read(&packet_buffer->recvQ);
        }
    }
}

void update_packet_buffer(char* entity_id, PacketBuffer* packet_buffer) {
    // Pop one and send
    Packet* packet = packet_queue_read(&packet_buffer->sendQ);
    if (!packet) return;
    Connection* connection = (Connection*) g_hash_table_lookup(componentRegistry.connections, entity_id);

    send_packet(entity_id, packet, connection);
}

void initialize_packet_routing_system() {
    StartTimer(&timer, 1);
}

void update_packet_routing_system() {
    if (!TimerDone(timer)) return;

    iterate_packet_buffers(update_packet_buffer);
    update_routers();

    StartTimer(&timer, TIME_PACKET_SEND);
}

void render_packet_routing_system() {
}
