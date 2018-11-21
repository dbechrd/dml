#pragma once
#include "dlb_types.h"
typedef struct entity entity;

typedef enum prop_type {
    PROP_NULL,
    PROP_INT,
    PROP_FLOAT,
    PROP_STRING,
} prop_type;

static inline const char *prop_type_str(prop_type type) {
    const char *str;
    switch(type) {
    case PROP_NULL: str = "<null>"; break;
    case PROP_INT: str = "int32"; break;
    case PROP_FLOAT: str = "float32"; break;
    case PROP_STRING: str = "string"; break;
    default: DLB_ASSERT(0);
    }
    return str;
};

typedef struct prop {
    const char *name;
    prop_type type;
    int32_t array;
    size_t length;
    union {
        int32_t as_int;
        float as_float;
        const char *as_string;
        int32_t *int_array;
        float *float_array;
        const char **string_array;
    } value;
} prop;

prop *prop_find(entity *e, const char *name);
prop *prop_find_or_create(entity *e, const char *name);
void prop_set_int(entity *e, const char *name, int value);
void prop_set_float(entity *e, const char *name, float value);
void prop_set_string(entity *e, const char *name, const char *value);