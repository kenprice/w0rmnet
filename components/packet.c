#include <string.h>
#include "component_registry.h"
#include "packet.h"

// simple fifo queue
// https://gist.github.com/ryankurte/61f95dc71133561ed055ff62b33585f8

Packet* packet_alloc(char* from_address, char* to_address, char* message) {
    Packet* packet = calloc(1, sizeof(Packet));
    packet->from_address = calloc(1, sizeof(char) * strlen(from_address) + 1);
    strcpy(packet->from_address, from_address);
    packet->to_address = calloc(1, sizeof(char) * strlen(to_address) + 1);
    strcpy(packet->to_address, to_address);
    packet->message = calloc(1, sizeof(char) * strlen(message) + 1);
    strcpy(packet->message, message);
    packet->hops = 0;
    packet->top_level_found = false;
    return packet;
}

// TODO: Packet free

Packet* packet_queue_read(PacketQueue* queue) {
    if (queue->tail == queue->head) {
        return NULL;
    }
    void* handle = queue->packets[queue->tail];
    queue->packets[queue->tail] = NULL;
    queue->tail = (queue->tail + 1) % queue->size;
    return handle;
}

int packet_queue_write(PacketQueue *queue, Packet* packet) {
    if (((queue->head + 1) % queue->size) == queue->tail) {
        return -1;
    }
    queue->packets[queue->head] = packet;
    queue->head = (queue->head + 1) % queue->size;
    return 0;
}

PacketQueue packet_queue_alloc(size_t size) {
    PacketQueue queue;
    queue.size = size;
    queue.tail = 0;
    queue.head = 0;
    queue.packets = calloc(size, sizeof(Packet*));
    return queue;
}

void iterate_packet_buffers(void (*cb)(char*,PacketBuffer *)) {
    GHashTableIter iter;
    char* entity_id;
    PacketBuffer* packet_buffer;

    g_hash_table_iter_init(&iter, component_registry.packet_buffers);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &packet_buffer)) {
        (*cb)(entity_id, packet_buffer);
    }
}
