#include "packet_routing_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"
#include "../utils/timer.h"

/**
 * returns 0 if error, 1 if success
 */
int send_packet(Packet* packet, Connection* connection) {
    // Find best recipient and send.
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

    send_packet(packet, connection);
}

void update_packet_buffers() {
    iterate_packet_buffers(update_packet_buffer);
}

Timer timer;
void initialize_packet_routing_system() {
    StartTimer(&timer, 1);
}

void update_packet_routing_system() {
    if (!TimerDone(timer)) return;

    update_packet_buffers();
    update_routers();

    StartTimer(&timer, 1);
}

void render_packet_routing_system() {
}