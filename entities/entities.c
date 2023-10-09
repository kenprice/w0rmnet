#include "entities.h"
#include "../components/utils/device_id.h"
#include "../entities/machine.h"
#include "../entities/router.h"

void create_entities(ComponentRegistry* registry) {
    char* machine_id1 = generate_device_id();
    char* machine_id2 = generate_device_id();
    char* router_id1 = generate_device_id();
    Machine machine;
    machine.sprite.sprite_id = SPRITE_SERVER;
    strncpy(machine.device.id, machine_id1, DEVICE_ID_LEN);
    machine.position.coord = (Vector2){0, 0};
    strncpy(machine.connection.from_device_id, machine_id1, DEVICE_ID_LEN);
    strncpy(machine.connection.to_device_id, router_id1, DEVICE_ID_LEN);
    create_machine_full(registry, machine);

    machine.sprite.sprite_id = SPRITE_SERVER;
    strncpy(machine.device.id, machine_id2, DEVICE_ID_LEN);
    machine.position.coord = (Vector2){1, 5};
    strncpy(machine.connection.from_device_id, machine_id2, DEVICE_ID_LEN);
    strncpy(machine.connection.to_device_id, router_id1, DEVICE_ID_LEN);
    create_machine_full(registry, machine);

    Router router;
    router.sprite.sprite_id = SPRITE_ROUTER;
    strncpy(router.device.id, router_id1, DEVICE_ID_LEN);
    router.position.coord = (Vector2){3, 3};
    strncpy(router.connection.from_device_id, router_id1, DEVICE_ID_LEN);
    strncpy(router.connection.to_device_id, machine_id1, DEVICE_ID_LEN);
    create_router_full(registry, router);
}
