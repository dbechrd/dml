#pragma once
#include "file.h"
#include "dlb_types.h"
#include "dlb_hash.h"

typedef enum {
    F_NULL,

    // Atomic types
    F_ATOM_INT,
    F_ATOM_UINT,
    F_ATOM_FLOAT,
    F_ATOM_STRING,
    F_ATOM_END = F_ATOM_STRING,

    // Object types
    F_TA_VEC3,
    F_TA_COLOR3,
    F_TA_VEC4,
    F_TA_COLOR4,
    F_TA_TRANSFORM,
    F_TA_SUN_LIGHT,
    F_TA_POINT_LIGHT,
    F_TA_MATERIAL,
    F_TA_MESH,
    F_TA_SHADER,
    F_TA_TEXTURE,
    F_TA_ENTITY,
    F_COUNT,
} ta_field_type;

typedef struct {
    ta_field_type type;
    const char *name;
    int offset;
    bool alias;
} ta_schema_field;

typedef struct {
    ta_field_type type;
    const char *name;
    ta_schema_field *fields;
} ta_schema;

extern ta_schema tg_schemas[];
extern dlb_hash tg_schemas_by_name;

const char *ta_field_type_str(ta_field_type type);
void obj_field_add(ta_schema *obj, ta_field_type type, const char *name,
    u32 offset, bool alias);
ta_schema_field *obj_field_find(ta_field_type type, const char *name);
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
typedef struct ta_scene_s ta_scene;

typedef struct ta_sun_light_s {
    ta_scene *scene;
    const char *name;
    ta_vec3 direction;
    ta_vec3 color;
} ta_sun_light;

typedef struct ta_point_light_s {
    ta_scene *scene;
    const char *name;
    ta_vec3 position;
    ta_vec3 color;
} ta_point_light;

typedef struct ta_material_s {
    ta_scene *scene;
    const char *name;
    //ta_texture *texture;  // TODO: Use file id?
} ta_material;

typedef struct ta_mesh_s {
    ta_scene *scene;
    const char *name;
    const char *path;
} ta_mesh;

typedef struct ta_shader_s {
    ta_scene *scene;
    const char *name;
    const char *path;
} ta_shader;

typedef struct ta_texture_s {
    ta_scene *scene;
    const char *name;
    const char *path;
} ta_texture;

typedef enum {
    ENTITY_DEFAULT
} ta_entity_type;

typedef struct ta_entity_s ta_entity;
typedef struct ta_entity_s {
    ta_scene *scene;
    const char *name;
    ta_entity_type type;
    const char *material;
    const char *mesh;
    const char *shader;
    const char *texture;
    ta_transform transform;
    ta_entity *parent;
    //ta_entity *next;  // TODO: Is a sibling linked list useful?
    ta_entity **children;
} ta_entity;

void obj_print(FILE *f, ta_field_type type, u8 *ptr, int level);
ta_material *entity_material(ta_entity *e);