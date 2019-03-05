#pragma once
#include "file.h"
#include "prop.h"
typedef struct scene scene;

typedef enum entity_type {
    ENTITY_GENERAL,
    ENTITY_TEXTURE,
    ENTITY_MATERIAL,
    ENTITY_MESH,
} entity_type;

typedef struct entity {
    entity_type type;
    unsigned int uid;
    prop *properties;
} entity;

entity *entity_init(scene *scn, entity_type type, unsigned int uid);
void entity_print(FILE *hnd, entity *e);
void entity_save(entity *e, file *f);
void entity_load(scene *scn, entity_type type, unsigned int uid, file *f);
void entity_free(entity *e);