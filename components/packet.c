#include <stdio.h>
#include <string.h>
#include "component_registry.h"
#include "packet.h"

// simple fifo queue
// https://gist.github.com/ryankurte/61f95dc71133561ed055ff62b33585f8

Packet* packet_alloc(char* fromEntityId, char* fromAddress, char* toAddress, char* message) {
    Packet* packet = calloc(1, sizeof(Packet));
    packet->fromEntityId = calloc(1, sizeof(char) * strlen(fromEntityId));
    strcpy(packet->fromEntityId, fromEntityId);

    packet->fromAddress = calloc(strlen(fromAddress), sizeof(char) * 110);
    strcpy(packet->fromAddress, fromAddress);

    packet->toAddress = calloc(1, sizeof(char) * strlen(toAddress) + 1);
    strcpy(packet->toAddress, toAddress);
    packet->message = calloc(1, sizeof(char) * strlen(message) + 1);
    strcpy(packet->message, message);
    packet->hops = 0;
    packet->topLevelFound = false;
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

Packet* packet_queue_peek(PacketQueue* queue) {
    if (queue->tail == queue->head) {
        return NULL;
    }
    void* handle = queue->packets[queue->tail];
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

    g_hash_table_iter_init(&iter, componentRegistry.packetBuffers);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &packet_buffer)) {
        (*cb)(entity_id, packet_buffer);
    }
}

char* comp_packet_buffer_serialize(PacketBuffer* packetBuffer) {
    // TODO Serialize Packets in PacketQueues
    char buffer[100];

    sprintf(buffer, "%s;%d;%d", packetBuffer->entityId, (int)packetBuffer->sendQ.size, (int)packetBuffer->recvQ.size);

    char* data = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(data, buffer);
    return data;
}

PacketBuffer* comp_packet_buffer_deserialize(char* data) {
    PacketBuffer* packetBuffer = calloc(1, sizeof(PacketBuffer));

    char** splitData = g_strsplit(data, ";", 3);

    strcpy(packetBuffer->entityId, splitData[0]);
    packetBuffer->sendQ.size = atoi(splitData[1]);
    packetBuffer->sendQ.packets = calloc(packetBuffer->sendQ.size, sizeof(Packet*));
    packetBuffer->recvQ.size = atoi(splitData[2]);
    packetBuffer->recvQ.packets = calloc(packetBuffer->recvQ.size, sizeof(Packet*));

    g_strfreev(splitData);

    return packetBuffer;
}

