#include "prop.h"
#include "entity.h"
#include "parse.h"
#include "symbol.h"
#include "dlb_vector.h"
#include <string.h>

prop *prop_find(entity *e, const char *name) {
    bool found = false;
    prop *p = e->properties;
    while (p != dlb_vec_end(e->properties)) {
        if (p->name == name) {
            found = true;
            break;
        }
        p++;
    }
    return found ? p : NULL;
}

prop *prop_create(entity *e, const char *name) {
    prop *p = prop_find(e, name);
    if (p != NULL) { return NULL; }  // Prevent duplicates

    p = dlb_vec_alloc(e->properties);
    p->name = name;
    return p;
}

void prop_set_int(entity *e, const char *name, int value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_INT;
    p->value.as_int = value;
}

void prop_set_float(entity *e, const char *name, float value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_FLOAT;
    p->value.as_float = value;
}

void prop_set_char(entity *e, const char *name, char value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_CHAR;
    p->value.as_char = value;
}

void prop_set_string(entity *e, const char *name, const char *value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_STRING;
    p->length = strlen(value);
    DLB_ASSERT(p->length <= STRING_MAX_LEN);
    p->value.string = value;
}

void prop_clear(entity *e, const char *name) {
    prop *p = prop_find(e, name);
    if (p) {
        // NOTE: Don't free interned strings because they might be shared
        if (p->length > 0 && p->type_alias != sym_string) {
            free(p->value.buffer);
        }
        memset(p, 0, sizeof(*p));
        p->name = "[PROPERTY CLEARED]";
    }
}