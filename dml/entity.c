#include "entity.h"
#include "scene.h"
#include "prop.h"
#include "file.h"
#include "symbol.h"
#include "parse.h"
#include "dlb_types.h"
#include "dlb_vector.h"

void entity_print(FILE *hnd, entity *e) {
    fprintf(hnd, "!%d:entity\n", e->uid);
    for (prop *prop = e->properties; prop != dlb_vec_end(e->properties); prop++) {
        fprintf(hnd, "  %s:%s", prop->name, prop_type_str(prop->type));
        if (prop->array) {
            fprintf(hnd, "[]");
        }
        switch(prop->type) {
        case PROP_INT:
            fprintf(hnd, " = %d", prop->value.as_int);
            break;
        case PROP_FLOAT:
            fprintf(hnd, " = %f", prop->value.as_float);
            break;
        case PROP_STRING:
            fprintf(hnd, " = \"%s\"", prop->value.as_string);
            break;
        default:
            break;
        }
        fprintf(hnd, "\n");
    }
}

void entity_save(entity *e, file *f) {
    entity_print(f->hnd, e);
}

static entity *entity_init(scene *scn, uint32_t uid) {
    if (uid) {
        DLB_ASSERT(uid >= scn->uid);
        scn->uid = uid;
    }
    entity *e = dlb_vec_alloc(scn->entities);
    e->uid = scn->uid++;
    return e;
}

void entity_load(scene *scn, uint32_t uid, file *f) {
    entity *e = entity_init(scn, uid);

    for (;;) {
        char c = file_getc(f);
        switch(c) {
        case EOF: case '!':
            return;
        case '\n':
            continue;
        case ' ':
            file_expect(f, ' ');
            break;
        default:
            DLB_ASSERT(0); // wtf?
        }

        const char *name = parse_string(f, ':');
        const char *type = parse_string(f, ' ');
        file_expect(f, '=');
        file_expect(f, ' ');

        prop *p = prop_find_or_create(e, name);
        if (type == sym_int) {
            p->type = PROP_INT;
            p->value.as_int = parse_int(f, '\n');
        } else if (type == sym_float) {
            p->type = PROP_FLOAT;
            p->value.as_float = parse_float(f, '\n');
        } else if (type == sym_string) {
            p->type = PROP_STRING;
            file_expect(f, '"');
            p->value.as_string = parse_string(f, '"');
            file_expect(f, '\n');
        } else {
            DLB_ASSERT(0); // wtf?
        }
    }
}

void entity_free(entity *e) {
    dlb_vec_free(e->properties);
}

entity *entity_create(scene *scn, const char *name, int age, float weight,
                      const char *height, const char *city) {
    entity *e = entity_init(scn, 0);
    prop_set_string(e, intern(CSTR("name")), name);
    prop_set_int(e, intern(CSTR("age")), age);
    prop_set_float(e, intern(CSTR("weight")), weight);
    prop_set_string(e, intern(CSTR("height")), height);
    prop_set_string(e, intern(CSTR("city")), city);
    return e;
}