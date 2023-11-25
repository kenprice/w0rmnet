#include "packet_routing_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"
#include "../components/known_hosts.h"
#include "../entities/helpers.h"
#include "../lib/log/log.h"
#include "../utils/timer.h"

Timer timer;

#define TIME_PACKET_SEND 0.1

void update_routers();
void update_wires();
void update_packet_buffer(char* entityId, PacketBuffer* packetBuffer);

void initialize_packet_routing_system() {
    StartTimer(&timer, 1);
}

void update_packet_routing_system() {
    if (!TimerDone(timer)) return;

    update_wires();
    iterate_packet_buffers(update_packet_buffer);
    update_routers();

    StartTimer(&timer, TIME_PACKET_SEND);
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
    Packet* packet = packet_queue_read(&packetBuffer->sendQ);

    if (packet) {
        if (!entity_send_packet_from_entity(entityId, packet)) {
            // Dead end!
            free(packet);
        }
    }

    // Update knownHosts for entity based on received packet
    packet = packet_queue_peek(&packetBuffer->recvQ);
    Device* device = g_hash_table_lookup(componentRegistry.devices, entityId);

    if (packet && device && strcmp(device->address, packet->toAddress) == 0) {
        KnownHosts* knownHosts = g_hash_table_lookup(componentRegistry.knownHosts, entityId);
        if (knownHosts) {
            if (!comp_known_hosts_entity_in_list(knownHosts, packet->fromAddress)) {
                log_debug("%s has %d known hosts, adding %s", device->address, knownHosts->numEntities, packet->fromAddress);
            }
            comp_known_hosts_add_entity(knownHosts, entityId, packet->fromEntityId);
        }
    }
}

static void wire_send_packet_to_q_a(Wire* wire, Packet* packet) {
    Device* device = g_hash_table_lookup(componentRegistry.devices, wire->entityA);
    PacketBuffer* packetBuffer = g_hash_table_lookup(componentRegistry.packetBuffers, wire->entityA);
    if (packetBuffer) {
//        log_debug("Wire forwards packet to %s", device ? device->address : wire->entityA);
        packet_queue_write(&packetBuffer->recvQ, packet);
    } else {
        log_debug("No packet buffer for %s", device ? device->address : wire->entityA);
    }
}

static void wire_send_packet_to_q_b(Wire* wire, Packet* packet) {
    Device* device = g_hash_table_lookup(componentRegistry.devices, wire->entityB);
    PacketBuffer* packetBuffer = g_hash_table_lookup(componentRegistry.packetBuffers, wire->entityB);
    if (packetBuffer) {
//        log_debug("Wire forwards packet to %s", device ? device->address : wire->entityB);
        packet_queue_write(&packetBuffer->recvQ, packet);
    } else {
        log_debug("No packet buffer for %s", device ? device->address : wire->entityB);
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
            wire_send_packet_to_q_a(wire, packet);
        }
        packet = packet_queue_read(&wire->sendQtoB);
        if (packet) {
            wire_send_packet_to_q_b(wire, packet);
        }
    }
}
