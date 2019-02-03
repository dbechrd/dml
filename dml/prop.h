#pragma once
#include "dlb_types.h"
typedef struct entity entity;

typedef enum prop_type {
    PROP_NULL,
    PROP_INT,
    PROP_FLOAT,
    PROP_CHAR,
    PROP_STRING,
} prop_type;

static inline const char *prop_type_str(prop_type type) {
    const char *str;
    switch(type) {
    case PROP_NULL:   str = "<null>";  break;
    case PROP_INT:    str = "int32";   break;
    case PROP_FLOAT:  str = "float32"; break;
    case PROP_CHAR:   str = "char";    break;
    case PROP_STRING: str = "string";  break;
    default: DLB_ASSERT(0);
    }
    return str;
};

typedef struct prop {
    const char *name;
    prop_type type;
    const char *type_alias;
    size_t length;
    union {
        int32_t as_int;
        float as_float;
        char as_char;
        int32_t *int_array;
        float *float_array;
        char *char_array;
        const char *string;
        void *buffer;
    } value;
} prop;

prop *prop_find(entity *e, const char *name);
prop *prop_create(entity *e, const char *name);
void prop_set_int(entity *e, const char *name, int value);
void prop_set_float(entity *e, const char *name, float value);
void prop_set_char(entity *e, const char *name, char value);
void prop_set_string(entity *e, const char *name, const char *value);
void prop_clear(entity *e, const char *name);