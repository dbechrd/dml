#pragma once
#include "file.h"
#include "object.h"
#include "dlb_types.h"
#include "dlb_hash.h"

typedef enum token_type {
    TOKEN_UNKNOWN,
    TOKEN_EOF,
    TOKEN_WHITESPACE,
    TOKEN_NEWLINE,
    TOKEN_INDENT,
    TOKEN_COMMENT,
    TOKEN_IDENTIFIER,
    TOKEN_KW_NULL,
    TOKEN_KW_TRUE,
    TOKEN_KW_FALSE,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_ARRAY_START,
    TOKEN_ARRAY_END,
    TOKEN_OBJECT_START,
    TOKEN_OBJECT_END,
    TOKEN_LIST_SEPARATOR,
} token_type;

typedef struct token {
    token_type type;
    size_t length;
    union {
        int32 as_int;
        float as_float;
        int32 *int_array;
        float *float_array;
        const char *string;
    } value;
} token;

typedef struct ta_scene_s ta_scene;

typedef struct scene_ref_s {
    ta_field_type type;
    void *ptr;
} scene_ref;

typedef struct ta_scene_s {
    const char *name;

    scene_ref *refs;
    dlb_hash refs_by_name;

    ta_sun_light *sun_lights;
    ta_point_light  *point_lights;
    ta_material *materials;
    ta_mesh *meshes;
    ta_shader *shaders;
    ta_texture *textures;
    ta_entity *entities;
} ta_scene;

const char *token_type_str(token_type type);
ta_scene *scene_init(const char *name);
ta_scene *scene_load(file *f);
void scene_free(ta_scene *scn);
void scene_print(ta_scene *scn, FILE *hnd);
void *scene_obj_init(ta_scene *scn, ta_field_type type);