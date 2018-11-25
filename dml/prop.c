#include "prop.h"
#include "entity.h"
#include "parse.h"
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

prop *prop_find_or_create(entity *e, const char *name) {
    bool found = false;
    prop *p = e->properties;
    while (p != dlb_vec_end(e->properties)) {
        if (p->name == name) {
            found = true;
            break;
        }
        p++;
    }
    if (!found) {
        p = dlb_vec_alloc(e->properties);
        p->name = name;
    }
    return p;
}

void prop_set_int(entity *e, const char *name, int value) {
    prop *p = prop_find_or_create(e, name);
    p->type = PROP_INT;
    p->value.as_int = value;
}

void prop_set_float(entity *e, const char *name, float value) {
    prop *p = prop_find_or_create(e, name);
    p->type = PROP_FLOAT;
    p->value.as_float = value;
}

void prop_set_string(entity *e, const char *name, const char *value) {
    prop *p = prop_find_or_create(e, name);
    p->type = PROP_STRING;
    p->length = strlen(value);
    DLB_ASSERT(p->length <= STRING_MAX_LEN);
    p->value.as_string = value;
}
