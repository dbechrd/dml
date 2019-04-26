#pragma once
#include "file.h"
#include "object.h"
#include "dlb_types.h"

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
} token_type;

static inline const char *token_type_str(token_type type) {
    switch(type) {
        case TOKEN_UNKNOWN:      return "????????";
        case TOKEN_EOF:          return "EOF";
        case TOKEN_WHITESPACE:   return "WHITESPACE";
        case TOKEN_NEWLINE:      return "NEWLINE";
        case TOKEN_INDENT:       return "INDENT";
        case TOKEN_COMMENT:      return "COMMENT";
        case TOKEN_IDENTIFIER:   return "IDENTIFIER";
        case TOKEN_KW_NULL:      return "KEYWORD";
        case TOKEN_KW_TRUE:      return "KEYWORD";
        case TOKEN_KW_FALSE:     return "KEYWORD";
        case TOKEN_INT:          return "INT";
        case TOKEN_FLOAT:        return "FLOAT";
        case TOKEN_STRING:       return "STRING";
        case TOKEN_ARRAY_START:  return "ARRAY_START";
        case TOKEN_ARRAY_END:    return "ARRAY_END";
        case TOKEN_OBJECT_START: return "OBJECT_START";
        case TOKEN_OBJECT_END:   return "OBJECT_END";
        default: DLB_ASSERT(!"Unknown token type");  return 0;
    }
};

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

typedef struct scene {
    const char *name;
    ta_entity *entities;
    ta_material *materials;
    ta_mesh *meshes;
    ta_shader *shaders;
    ta_texture *textures;
} scene;

scene *scene_init(const char *name);
scene *scene_load(file *f);
void scene_free(scene *scn);
void scene_print(scene *scn, FILE *hnd);
void *scene_obj_init(scene *scn, ta_field_type type);