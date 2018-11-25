#pragma once
#include "file.h"
#include "prop.h"
typedef struct scene scene;

typedef struct entity {
    unsigned int uid;
    prop *properties;
} entity;

entity *entity_init(scene *scn, unsigned int uid);
void entity_print(FILE *hnd, entity *e);
void entity_save(entity *e, file *f);
void entity_load(scene *scn, unsigned int uid, file *f);
void entity_free(entity *e);