#pragma once
#include "file.h"
#include "prop.h"
typedef struct scene scene;

typedef struct entity {
    uint32_t uid;
    prop *properties;
} entity;

void entity_print(FILE *hnd, entity *e);
void entity_save(entity *e, file *f);
void entity_load(scene *scn, uint32_t uid, file *f);
void entity_free(entity *e);
entity *entity_create(scene *scn, const char *name, int age, float weight,
                      const char *height, const char *city);