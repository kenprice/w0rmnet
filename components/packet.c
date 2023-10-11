#include <string.h>
#include "packet.h"

// simple fifo queue
// https://gist.github.com/ryankurte/61f95dc71133561ed055ff62b33585f8

Packet* packet_alloc(char* from_address, char* to_address, char* message) {
    Packet* packet = calloc(1, sizeof(Packet));
    packet->from_address = calloc(1, sizeof(char) * strlen(from_address));
    strcpy(packet->from_address, from_address);
    packet->to_address = calloc(1, sizeof(char) * strlen(to_address));
    strcpy(packet->to_address, to_address);
    packet->message = calloc(1, sizeof(char) * strlen(message));
    strcpy(packet->message, message);
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

PacketQueue* packet_queue_alloc(size_t size) {
    PacketQueue* queue = calloc(1, sizeof(PacketQueue));
    queue->size = size;
    queue->packets = calloc(size, sizeof(Packet*));
    return queue;
}
