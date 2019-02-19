#pragma once
#include <stdint.h>
#include "file.h"
#include "entity.h"

typedef struct scene {
    const char *name;
    uint32_t next_uid;
    entity *entities;
} scene;

void init_type_loaders();

scene *scene_init(const char *name);
void scene_free(scene *scn);
void scene_save(scene *scn, file *entitydb);
scene *scene_load(file *f);
void scene_print(scene *scn);