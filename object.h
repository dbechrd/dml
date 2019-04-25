#pragma once
#include "dlb_types.h"
#include "dlb_hash.h"
#include "file.h"

typedef enum {
    OBJ_NULL,

    // Atomic types
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STRING,
    OBJ_ATOMIC_LAST = OBJ_STRING,

    // Compound types
    OBJ_TA_VEC3,
    OBJ_TA_VEC4,
    OBJ_TA_TRANSFORM,

    // Scene-level objects
    OBJ_TA_ENTITY,
    OBJ_TA_MATERIAL,
    OBJ_TA_MESH,
    OBJ_TA_SHADER,
    OBJ_TA_TEXTURE,

    OBJ_COUNT
} ta_object_type;

typedef struct {
    ta_object_type type;
    const char *name;
    int offset;
} ta_object_field;

typedef struct {
    ta_object_type type;
    const char *name;
    ta_object_field *fields;
} ta_object;

extern ta_object tg_objects[OBJ_COUNT];
extern dlb_hash tg_objects_by_name;

const char *ta_object_type_str(ta_object_type type);
void obj_field_add(ta_object *obj, ta_object_type type, const char *name,
    u32 offset);
ta_object_field *obj_field_find(ta_object_type type, const char *name);
void obj_register();

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    float x, y, z;
} ta_vec3;

typedef struct {
    float x, y, z, w;
} ta_vec4;

typedef struct {
    ta_vec3 position;
    ta_vec4 rotation;
    ta_vec3 scale;
} ta_transform;

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    u32 uid;
    const char *name;
    //ta_texture *texture;  // TODO: Use file id?
} ta_material;

typedef struct {
    u32 uid;
    const char *name;
    const char *path;
} ta_mesh;

typedef struct {
    u32 uid;
    const char *name;
    const char *path;
} ta_shader;

typedef struct {
    u32 uid;
    const char *name;
    const char *path;
} ta_texture;

typedef enum {
    ENTITY_DEFAULT
} ta_entity_type;

typedef struct ta_entity_s ta_entity;
struct ta_entity_s {
    u32 uid;
    ta_entity_type type;
    const char *name;

    u32 material;
    u32 mesh;
    u32 shader;
    u32 texture;
    ta_transform transform;

    ta_entity *parent;
    //ta_entity *next;  // TODO: Is a sibling linked list useful?
    ta_entity **children;
};

//void material_init(ta_material *material);
//void entity_init(ta_entity *entity);
void entity_free(ta_entity *entity);
void entity_print(FILE *f, ta_entity *e);
void entity_save(file *f, ta_entity *e);