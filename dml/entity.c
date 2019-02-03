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
        fprintf(f, "  %s:", prop->name);
        if (prop->type_alias == NULL) {
            fprintf(f, "%s", prop_type_str(prop->type));
            if (prop->length > 0 && prop->type != PROP_STRING) {
                fprintf(f, CHAR_ARRAY_LEN_START "%d" CHAR_ARRAY_LEN_END, prop->length);
            }
        } else {
            fprintf(f, "%s", prop->type_alias);
        }
        fprintf(f, " = ");
        switch(prop->type) {
        case PROP_INT:
            if (prop->length == 0) {
                fprintf(f, "%d", prop->value.as_int);
            } else {
                fprintf(f, CHAR_ARRAY_START);
                for (size_t i = 0; i < prop->length; i++) {
                    fprintf(f, "%d", prop->value.as_int);
                    if (i < prop->length - 1) {
                        fprintf(f, ", ");
                    }
                }
                fprintf(f, CHAR_ARRAY_END);
            }
            break;
        case PROP_FLOAT:
            if (prop->length == 0) {
                fprintf(f, "%f:0x%x", prop->value.as_float,
                        *(unsigned *)&prop->value.as_float);
            } else {
                fprintf(f, CHAR_ARRAY_START);
                for (size_t i = 0; i < prop->length; i++) {
                    fprintf(f, "%f:0x%x", prop->value.as_float,
                            *(unsigned *)&prop->value.as_float);
                    if (i < prop->length - 1) {
                        fprintf(f, ", ");
                    }
                }
                fprintf(f, CHAR_ARRAY_END);
            }
            break;
        case PROP_CHAR:
            if (prop->length == 0) {
                fprintf(f, "'%c'", prop->value.as_char);
            } else {
                fprintf(f, CHAR_ARRAY_START);
                for (size_t i = 0; i < prop->length; i++) {
                    fprintf(f, "'%c'", prop->value.as_char);
                    if (i < prop->length - 1) {
                        fprintf(f, ", ");
                    }
                }
                fprintf(f, CHAR_ARRAY_END);
            }
            break;
        case PROP_STRING:
            fprintf(f, "\"%.*s\"", prop->length, prop->value.string);
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
        char c = file_char(f);
        switch(c) {
        case EOF: case '!':
            f->replay = true;
            return;
        case '\n':
            continue;
        case ' ': case '\t':
            continue;
        case '#':
            file_allow_char(f, CHAR_STRING_LITERAL, 0);
            continue;
        default:
            f->replay = true;
            break;
        }

        file_pos pos_identifier = f->pos;
        file_expect_char(f, CHAR_IDENTIFIER_START, 1);
        f->replay = true;
        const char *name = read_string(f, ":", CHAR_IDENTIFIER);
        file_expect_char(f, ":", 1);

        file_pos pos_type = f->pos;
        const char *type = read_string(f, CHAR_WHITESPACE CHAR_ARRAY_LEN_START, CHAR_TYPE);
        size_t arr_len = 0;
        if (file_allow_char(f, CHAR_ARRAY_LEN_START, 1)) {
            arr_len = read_uint(f, CHAR_ARRAY_LEN_END);
            file_expect_char(f, CHAR_ARRAY_LEN_END, 1);
        }
        file_allow_char(f, CHAR_WHITESPACE, 0);
        file_expect_char(f, "=", 1);
        file_allow_char(f, CHAR_WHITESPACE, 0);

        prop *p = prop_create(e, name);
        if (p) {
            if (type == sym_int) {
                p->type = PROP_INT;
                if (arr_len == 0) {
                    p->value.as_int = read_int(f, CHAR_SEPARATOR);
                } else {
                    file_expect_char(f, CHAR_ARRAY_START, 1);
                    file_allow_char(f, CHAR_WHITESPACE, 0);
                    p->length = arr_len;
                    p->value.int_array = calloc(p->length, sizeof(*p->value.int_array));
                    for (size_t i = 0; i < p->length; i++) {
                        p->value.int_array[i] = read_int(f, CHAR_SEPARATOR ":" ",");
                        file_allow_char(f, CHAR_WHITESPACE, 0);
                        if (i < p->length - 1) {
                            file_expect_char(f, ",", 1);
                        } else {
                            file_allow_char(f, ",", 1);
                        }
                        file_allow_char(f, CHAR_WHITESPACE, 0);
                    }
                    file_expect_char(f, CHAR_ARRAY_END, 1);
                }
            } else if (type == sym_float) {
                p->type = PROP_FLOAT;
                if (arr_len == 0) {
                    p->value.as_float = read_float(f, CHAR_SEPARATOR ":");
                } else {
                    file_expect_char(f, CHAR_ARRAY_START, 1);
                    file_allow_char(f, CHAR_WHITESPACE, 0);
                    p->length = arr_len;
                    p->value.float_array = calloc(p->length, sizeof(*p->value.float_array));
                    for (size_t i = 0; i < p->length; i++) {
                        p->value.float_array[i] = read_float(f, CHAR_SEPARATOR ":" ",");
                        file_allow_char(f, CHAR_WHITESPACE, 0);
                        if (i < p->length - 1) {
                            file_expect_char(f, ",", 1);
                        } else {
                            file_allow_char(f, ",", 1);
                        }
                        file_allow_char(f, CHAR_WHITESPACE, 0);
                    }
                    file_expect_char(f, CHAR_ARRAY_END, 1);
                }
            } else if (type == sym_char) {
                p->type = PROP_CHAR;
                if (arr_len == 0) {
                    file_expect_char(f, CHAR_CHAR_DELIM, 1);
                    p->value.as_char = read_char(f, CHAR_CHAR_DELIM, CHAR_CHAR_LITERAL);
                    file_expect_char(f, CHAR_CHAR_DELIM, 1);
                } else {
                    file_expect_char(f, CHAR_ARRAY_START, 1);
                    file_allow_char(f, CHAR_WHITESPACE, 0);
                    p->length = arr_len;
                    p->value.char_array = calloc(p->length, sizeof(*p->value.char_array));
                    for (size_t i = 0; i < p->length; i++) {
                        file_expect_char(f, CHAR_CHAR_DELIM, 1);
                        p->value.char_array[i] = read_char(f, CHAR_CHAR_DELIM, CHAR_CHAR_LITERAL);
                        file_expect_char(f, CHAR_CHAR_DELIM, 1);

                        file_allow_char(f, CHAR_WHITESPACE, 0);
                        if (i < p->length - 1) {
                            file_expect_char(f, ",", 1);
                        } else {
                            file_allow_char(f, ",", 1);
                        }
                        file_allow_char(f, CHAR_WHITESPACE, 0);
                    }
                    file_expect_char(f, CHAR_ARRAY_END, 1);
                }

            } else if (type == sym_string) {
                file_expect_char(f, CHAR_STRING_DELIM, 1);
                p->type = PROP_STRING;
                p->value.string = read_string(f, CHAR_STRING_DELIM, CHAR_STRING_LITERAL);
                p->length = dlb_symbol_len(p->value.string);
                file_expect_char(f, CHAR_STRING_DELIM, 1);
            } else if (type == sym_vec3) {
                file_expect_char(f, CHAR_ARRAY_START, 1);
                file_allow_char(f, CHAR_WHITESPACE, 0);
                p->type = PROP_FLOAT;
                p->type_alias = sym_vec3;
                p->length = 3;
                p->value.float_array = calloc(p->length, sizeof(*p->value.float_array));
                for (size_t i = 0; i < p->length; i++) {
                    p->value.float_array[i] = read_float(f, CHAR_SEPARATOR ":" ",");
                    file_allow_char(f, CHAR_WHITESPACE, 0);
                    if (i < p->length - 1) {
                        file_expect_char(f, ",", 1);
                    } else {
                        file_allow_char(f, ",", 1);
                    }
                    file_allow_char(f, CHAR_WHITESPACE, 0);
                }
                file_expect_char(f, CHAR_ARRAY_END, 1);
            } else {
                PANIC_FILE(f, "[PARSE_ERROR] Expected type identifier at %d:%d\n",
                           pos_type.line, pos_type.column);
            }
        } else {
            PANIC_FILE(f, "[PARSE_ERROR] Redefinition of identifier '%s' at %d:%d\n",
                       name, pos_identifier.line, pos_identifier.column);
        }

        DLB_ASSERT(1);
    }
}

void entity_free(entity *e) {
    for (prop *prop = e->properties; prop != dlb_vec_end(e->properties); prop++) {
        if (prop->length > 0 && prop->type != PROP_STRING) {
            free(prop->value.buffer);
        }
    }
    dlb_vec_free(e->properties);
}