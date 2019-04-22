#pragma once
#include "dlb_types.h"
#include "file.h"

typedef enum {
    OBJ_NULL,
    OBJ_MATERIAL,
    OBJ_MESH,
    OBJ_SHADER,
    OBJ_TEXTURE,
    OBJ_TRANSFORM,
    OBJ_ENTITY,
    OBJ_COUNT
} ta_object_type;

typedef struct {
    const char *name;
    int offset;
} ta_object_field;

typedef struct {
    ta_object_type type;
    const char *name;
    ta_object_field *fields;
} ta_object;

const char *ta_object_type_str(ta_object_type type);
void obj_field_add(ta_object *obj, const char *name, u32 offset);
void *obj_field_find(const ta_object *obj, const char *name);

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    float x, y, z;
} ta_vec3;

typedef struct {
    float x, y, z, w;
} ta_vec4;

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    ta_object *object;
    const char *name;
    //ta_texture *texture;  // TODO: Use file id?
} ta_material;

typedef struct {
    ta_object *object;
    const char *name;
    const char *path;
} ta_mesh;

typedef struct {
    ta_object *object;
    const char *name;
    const char *path;
} ta_shader;

typedef struct {
    ta_object *object;
    const char *name;
    const char *path;
} ta_texture;

typedef struct {
    ta_object *object;
    ta_vec3 position;
    ta_vec4 rotation;
    ta_vec3 scale;
} ta_transform;

typedef enum {
    ENTITY_DEFAULT
} ta_entity_type;

typedef struct ta_entity_s ta_entity;
typedef struct ta_entity_s {
    const ta_object *object;
    ta_entity_type type;
    u32 uid;
    const char *name;

    ta_material material;
    ta_mesh mesh;
    ta_shader shader;
    ta_texture texture;
    ta_transform transform;

    ta_entity *parent;
    //ta_entity *next;  // TODO: Is a sibling linked list useful?
    ta_entity **children;
} ta_entity;

void entity_init(ta_entity *entity);
void entity_free(ta_entity *entity);
void entity_print(FILE *f, ta_entity *e);
void entity_save(file *f, ta_entity *e);