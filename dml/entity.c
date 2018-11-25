#include "entity.h"
#include "scene.h"
#include "prop.h"
#include "file.h"
#include "symbol.h"
#include "parse.h"
#include "dlb_types.h"
#include "dlb_vector.h"

void entity_print(FILE *f, entity *e) {
    fprintf(f, "!%d:entity\n", e->uid);
    for (prop *prop = e->properties; prop != dlb_vec_end(e->properties); prop++) {
        fprintf(f, "  %s:%s", prop->name, prop_type_str(prop->type));
        if (prop->array) {
            fprintf(f, "[]");
        }
        switch(prop->type) {
        case PROP_INT:
            fprintf(f, " = %d", prop->value.as_int);
            break;
        case PROP_FLOAT:
            fprintf(f, " = 0x%x  # %f", *(unsigned *)&prop->value.as_float, prop->value.as_float);
            break;
        case PROP_STRING:
            fprintf(f, " = \"%s\"", prop->value.as_string);
            break;
        default:
            DLB_ASSERT(0);
        }
        fprintf(f, "\n");
    }
}

void entity_save(entity *e, file *f) {
    entity_print(f->hnd, e);
}

entity *entity_init(scene *scn, unsigned int uid) {
    if (uid) {
        DLB_ASSERT(uid >= scn->next_uid);
        scn->next_uid = uid;
    }
    entity *e = dlb_vec_alloc(scn->entities);
    e->uid = scn->next_uid++;
    return e;
}

void entity_load(scene *scn, unsigned int uid, file *f) {
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

        const char *name = read_string(f, ':', CHAR_PROP_IDENTIFIER);

        size_t f_lineno = f->line_number;
        size_t f_column = f->line_column;
        const char *type = read_string(f, ' ', CHAR_TYPE_IDENTIFIER);
        file_expect(f, '=');
        file_expect(f, ' ');

        prop *p = prop_find_or_create(e, name);
        if (type == sym_int) {
            p->type = PROP_INT;
            p->value.as_int = read_int(f, '\n');
        } else if (type == sym_float) {
            p->type = PROP_FLOAT;
            p->value.as_float = read_float(f, '\n');
        } else if (type == sym_string) {
            p->type = PROP_STRING;
            file_expect(f, '"');
            p->value.as_string = read_string(f, '"', 0);
            file_expect(f, '\n');
        } else {
            fprintf(stderr, "%s:%d:%d [PARSE_ERROR] Expected type identifier at line %d, column %d\n",
                    f->filename, f->line_number, f->line_column, f_lineno, f_column);
            getchar();
            exit(1);
        }
    }
}

void entity_free(entity *e) {
    dlb_vec_free(e->properties);
}