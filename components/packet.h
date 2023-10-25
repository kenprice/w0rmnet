#ifndef W0RMNET_PACKET_H
#define W0RMNET_PACKET_H

#include <stdbool.h>
#include "../utils/uuid.h"

typedef struct {
    char* fromEntityId;
    char* fromAddress; // ex. w0odyv3rs0.un1tyd1zzy.d34dlyd3uce
    char* toAddress; // ex. w0odyv3rs0.un1tyd1zzy.d34dlyd3uce
    char* message;

    bool topLevelFound; // has this packet reached the top-level of address?
    int hops; // How many hops from the top-level of address traversed thus far, e.g. from w0odyv3rs0 for w0odyv3rs0.un1tyd1zzy.d34dlyd3uce
} Packet;

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    Packet** packets;
} PacketQueue;

typedef struct {
    char entityId[UUID_STR_LEN];
    PacketQueue sendQ;
    PacketQueue recvQ;
} PacketBuffer;

Packet* packet_alloc(char* fromEntityId, char* fromAddress, char* toAddress, char* message);

PacketQueue packet_queue_alloc(size_t size);

Packet* packet_queue_read(PacketQueue* queue);

Packet* packet_queue_peek(PacketQueue* queue);

int packet_queue_write(PacketQueue *queue, Packet* packet);

void iterate_packet_buffers(void (*cb)(char*,PacketBuffer *));

char* comp_packet_buffer_serialize(PacketBuffer* packetBuffer);

PacketBuffer* comp_packet_buffer_deserialize(char* data);

#endif //W0RMNET_PACKET_H
