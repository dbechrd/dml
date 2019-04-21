#pragma once
#include "dlb_types.h"
#include "file.h"

typedef enum token_type {
    TOKEN_EOF,
    TOKEN_WHITESPACE,
    TOKEN_COMMENT,
    TOKEN_IDENT,
    TOKEN_NUMBER,
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
        case TOKEN_EOF:          return "EOF";
        case TOKEN_IDENT:        return "IDENTIFIER";
        case TOKEN_WHITESPACE:   return "WHITESPACE";
        case TOKEN_COMMENT:      return "COMMENT";
        case TOKEN_NUMBER:       return "NUMBER";
        case TOKEN_INT:          return "INT";
        case TOKEN_FLOAT:        return "FLOAT";
        case TOKEN_STRING:       return "STRING";
        case TOKEN_ARRAY_START:  return "ARRAY_START";
        case TOKEN_ARRAY_END:    return "ARRAY_END";
        case TOKEN_OBJECT_START: return "OBJECT_START";
        case TOKEN_OBJECT_END:   return "OBJECT_END";
        default: DLB_ASSERT(0); return 0;
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
        //void *buffer;
    } value;
} token;

typedef enum prop_type {
    PROP_NULL,
    PROP_INT,
    PROP_FLOAT,
    PROP_CHAR,
    PROP_STRING,
    PROP_ARRAY,
    PROP_OBJECT,
} prop_type;

static inline const char *prop_type_str(prop_type type) {
    switch(type) {
        case PROP_NULL:   return "<null>";
        case PROP_INT:    return "int32";
        case PROP_FLOAT:  return "float32";
        case PROP_CHAR:   return "char";
        case PROP_STRING: return "string";
        case PROP_ARRAY:  return "[]";
        case PROP_OBJECT: return "{}";
        default: DLB_ASSERT(0); return 0;
    }
};

typedef struct prop {
    const char *name;
    prop_type type;
    const char *type_alias;
    size_t length;
    union {
        int32 as_int;
        float as_float;
        char as_char;
        int32 *int_array;
        float *float_array;
        char *char_array;
        const char *string;
        void *buffer;
    } value;
} prop;

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

typedef struct scene {
    const char *name;
    uint32_t next_uid;
    entity *entities;
} scene;

void init_type_loaders();

scene *scene_init(const char *name);
void scene_free(scene *scn);
void scene_print(scene *scn);
void scene_save(file *f, scene *scn);
scene *scene_load(file *f);

entity *entity_init(scene *scn, entity_type type, unsigned int uid);
void entity_free(entity *e);
void entity_print(FILE *hnd, entity *e);
void entity_save(file *f, entity *e);
void entity_load(file *f, scene *scn, entity_type type, unsigned int uid);

prop *prop_find(entity *e, const char *name);
prop *prop_create(entity *e, const char *name);
void prop_set_int(entity *e, const char *name, int value);
void prop_set_float(entity *e, const char *name, float value);
void prop_set_char(entity *e, const char *name, char value);
void prop_set_string(entity *e, const char *name, const char *value);
void prop_clear(entity *e, const char *name);