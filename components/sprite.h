#ifndef W0RMNET_SPRITE_H
#define W0RMNET_SPRITE_H

typedef struct {
    int spriteId;
} Sprite;

void iterate_sprites(void (*cb)(char*,Sprite*));

char* comp_sprite_serialize(Sprite* sprite);

Sprite* comp_sprite_deserialize(char* data);

#endif //W0RMNET_SPRITE_H
