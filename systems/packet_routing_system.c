#include "packet_routing_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"
#include "../utils/timer.h"

/**
 * returns 0 if error, 1 if success
 */
int send_packet(char* entity_id, Packet* packet, Connection* connection) {
    char buffer[200];
    strcpy(buffer, packet->to_address);
    char* token = strtok(buffer, ".");
    char path[10][11];
    int parts = 0;
    while (token != NULL) {
        strcpy(path[parts], token);
        token = strtok(NULL, ".");
        parts++;
    }

    // Searching for top
    if (!packet->top_level_found) {
        char* cur_part = path[0];
        Device* cur_device = (Device*)g_hash_table_lookup(component_registry.devices, entity_id);

        // Shitty prepend, update from address
        char from_address[110];
        strcpy(from_address, cur_device->id);
        strcat(from_address, ".");
        strcat(from_address, packet->from_address);
        strcpy(packet->from_address, from_address);

        if (strcmp(cur_part, cur_device->id) == 0) {
            // End search! Found!
            packet->top_level_found = true;
        } else {
            // If some machine, send to router (assuming max one router connected)
            if (cur_device->type != DEVICE_TYPE_ROUTER) {
                for (int i = 0; i < connection->num_conns; i++) {
                    char* to_entity = find_device_entity_id_by_device_id(connection->to_device_id[i]);

                    Device* device = (Device*)g_hash_table_lookup(component_registry.devices, to_entity);
                    if (device->type != DEVICE_TYPE_ROUTER) continue;

                    PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, to_entity);
                    if (!to_packet_buffer) return 0;

                    packet_queue_write(&to_packet_buffer->recv_q, packet);
                    return 1;
                }
            }
            // If router, send to parent
            if (cur_device->type == DEVICE_TYPE_ROUTER) {
                RouteTable* route_table = (RouteTable*)g_hash_table_lookup(component_registry.route_tables, entity_id);
                if (route_table != NULL && strlen(route_table->gateway) > 0) {
                    char* to_entity = find_device_entity_id_by_device_id(route_table->gateway);
                    PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, to_entity);
                    if (!to_packet_buffer) return 0;

                    packet_queue_write(&to_packet_buffer->recv_q, packet);
                }
            }
        }
    }
    if (packet->top_level_found) {
        // Top found, traverse back down
        packet->hops++;

        if (packet->hops < parts) { // More hops to go
            char* cur_part = path[packet->hops];
            for (int i = 0; i < connection->num_conns; i++) {
                char* to_entity = find_device_entity_id_by_device_id(cur_part);

                if (strcmp(cur_part, connection->to_device_id[i]) == 0) {
                    PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, to_entity);
                    packet_queue_write(&to_packet_buffer->recv_q, packet);
                    return 1;
                }
            }
            // Not found!
            return 0;
        }
    }
    return 0;




    // First, see if destination address is in next hop
    for (int i = 0; i < connection->num_conns; i++) {
        char* to_entity = find_device_entity_id_by_device_id(connection->to_device_id[i]);

        if (strcmp(packet->to_address, connection->to_device_id[i]) == 0) {
            PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, to_entity);
            packet_queue_write(&to_packet_buffer->recv_q, packet);
            return 1;
        }
    }

    // If not, send to router
    for (int i = 0; i < connection->num_conns; i++) {
        char* to_entity = find_device_entity_id_by_device_id(connection->to_device_id[i]);

        Device* device = (Device*)g_hash_table_lookup(component_registry.devices, to_entity);
        if (device->type != DEVICE_TYPE_ROUTER) continue;

        PacketBuffer* to_packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, to_entity);
        if (!to_packet_buffer) return 0;

        packet_queue_write(&to_packet_buffer->recv_q, packet);
        return 1;
    }

    return 0;
}

void update_routers() {
    GHashTableIter iter;
    guint* key_;

    Device* device;
    g_hash_table_iter_init(&iter, component_registry.devices);
    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &device)) {
        if (device->type != DEVICE_TYPE_ROUTER) continue;

        PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, key_);
        Packet* packet = packet_queue_read(&packet_buffer->recv_q);
        if (!packet) continue;

        packet_queue_write(&packet_buffer->send_q, packet);
    }
}

void update_packet_buffer(char* entity_id, PacketBuffer* packet_buffer) {
    // Pop one and send
    Packet* packet = packet_queue_read(&packet_buffer->send_q);
    if (!packet) return;
    Connection* connection = (Connection*) g_hash_table_lookup(component_registry.connections, entity_id);

    send_packet(entity_id, packet, connection);
}

Timer timer;
void initialize_packet_routing_system() {
    StartTimer(&timer, 1);
}

void update_packet_routing_system() {
    if (!TimerDone(timer)) return;

    iterate_packet_buffers(update_packet_buffer);
    update_routers();

    StartTimer(&timer, 1);
}

void render_packet_routing_system() {
}
