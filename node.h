#pragma once

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

typedef struct {
    const char *name;
    const char *path;
} ta_texture;

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

/*
typedef enum {
    TA_TRANSFORM,
    TA_TEXTURE,
    TA_MATERIAL,
    TA_MESH,
    TA_SHADER,
} ta_component_type;

typedef struct {
    ta_component_type type;
    union {
        ta_transform transform;
        ta_texture texture;
        ta_material material;
        ta_mesh mesh;
        ta_shader shader;
    } data;
} ta_component;
*/

typedef struct ta_entity_s ta_entity;
typedef struct ta_entity_s {
    const char *name;

    //ta_component *components;
    ta_material material;
    ta_mesh mesh;
    ta_shader shader;
    ta_texture texture;
    ta_transform transform;

    ta_entity *parent;
    //ta_entity *next;  // TODO: Is a sibling linked list useful?
    ta_entity **children;
} ta_entity;

////////////////////////////////////////////////////////////////////////////////

extern ta_entity *tg_root;