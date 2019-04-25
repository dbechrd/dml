#pragma once
#include "dlb_types.h"
#include "dlb_hash.h"
#include "file.h"

typedef enum {
    OBJ_NULL,

    // Atomic types
    OBJ_INT,
    OBJ_UINT,
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
    const char *name;
    //ta_texture *texture;  // TODO: Use file id?
} ta_material;

typedef struct {
    const char *name;
    const char *path;
} ta_mesh;

typedef struct {
    const char *name;
    const char *path;
} ta_shader;

typedef struct {
    const char *name;
    const char *path;
} ta_texture;

typedef enum {
    ENTITY_DEFAULT
} ta_entity_type;

typedef struct ta_entity_s ta_entity;
struct ta_entity_s {
    ta_entity_type type;
    const char *name;

    const char *material;
    const char *mesh;
    const char *shader;
    const char *texture;
    ta_transform transform;

    ta_entity *parent;
    //ta_entity *next;  // TODO: Is a sibling linked list useful?
    ta_entity **children;
};

void ta_material_print(FILE *f, ta_material *o);
void ta_mesh_print(FILE *f, ta_mesh *o);
void ta_shader_print(FILE *f, ta_shader *o);
void ta_texture_print(FILE *f, ta_texture *o);
void ta_entity_print(FILE *f, ta_entity *o);