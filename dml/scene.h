#pragma once
#include <stdint.h>
#include "file.h"
#include "entity.h"

typedef struct scene {
    uint32_t uid;
    entity *entities;
} scene;

scene *scene_init();
void scene_save(scene *scn, file *entitydb);
scene *scene_load(file *f);
void scene_free(scene *scn);