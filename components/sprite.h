#ifndef W0RMNET_SPRITE_H
#define W0RMNET_SPRITE_H

typedef struct {
    int sprite_id;
} Sprite;

void iterate_sprites(void (*cb)(char*,Sprite*));

#endif //W0RMNET_SPRITE_H
