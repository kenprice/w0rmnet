#ifndef W0RMNET_PACKET_H
#define W0RMNET_PACKET_H

#include "../utils/uuid.h"

typedef struct {
    char* from_entity_id;
    char* from_address; // ex. w0odyv3rs0.un1tyd1zzy.d34dlyd3uce
    char* to_address; // ex. w0odyv3rs0.un1tyd1zzy.d34dlyd3uce
    char* message;
    bool top_level_found; // has this packet reached the top-level of address?
    int hops; // How many hops from the top-level of address traversed thus far, e.g. from w0odyv3rs0 for w0odyv3rs0.un1tyd1zzy.d34dlyd3uce
} Packet;

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    Packet** packets;
} PacketQueue;

typedef struct {
    char entity_id[UUID_STR_LEN];
    PacketQueue send_q;
    PacketQueue recv_q;
} PacketBuffer;

Packet* packet_alloc(char* from_entity_id, char* to_address, char* message);

PacketQueue packet_queue_alloc(size_t size);

Packet* packet_queue_read(PacketQueue* queue);

Packet* packet_queue_peek(PacketQueue* queue);

int packet_queue_write(PacketQueue *queue, Packet* packet);

void iterate_packet_buffers(void (*cb)(char*,PacketBuffer *));

#endif //W0RMNET_PACKET_H
