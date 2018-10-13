#include <stdio.h>
#include <string.h>
#include "dlb_memory.h"
#include "dlb_vector.h"
#include "dlb_hash.h"
#include "dlb_arena.h"

#define DLB_MEMORY_IMPLEMENTATION
#include "dlb_memory.h"
#define DLB_VECTOR_IMPLEMENTATION
#include "dlb_vector.h"
#define DLB_HASH_IMPLEMENTATION
#include "dlb_hash.h"
#define DLB_ARENA_IMPLEMENTATION
#include "dlb_arena.h"

#define CSTR(s) (s), sizeof(s) - 1
#define SYM(s) sym(CSTR(s))

uint32_t strhash(const char *s) {
    uint32_t hash;
    MurmurHash3_x86_32(s, sizeof(s) - 1, (void *)&hash);
    return hash;
}

bool streq(const char *a, const char *b, size_t len) {
    return (a == b || strncmp(a, b, len) == 0);
}

//------------------------------------------------------------------------------

typedef enum file_mode {
    FILE_READ,
    FILE_WRITE,
} file_mode;

const char *file_mode_str(file_mode mode) {
    const char *str;
    switch(mode) {
    case FILE_READ: str = "rb"; break;
    case FILE_WRITE: str = "wb"; break;
    default: DLB_ASSERT(0);
    }
    return str;
}

typedef struct file {
    const char *filename;
    file_mode mode;
    FILE *hnd;
    char last;
} file;

typedef struct symbol {
    uint32_t hash;
    size_t len;
    const char *str;
} symbol;

typedef enum prop_type {
    PROP_NULL,
    PROP_INT,
    PROP_FLOAT,
    PROP_STRING,
} prop_type;

const char *prop_type_str(prop_type type) {
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
    int array;
    size_t length;
    union {
        int int32;
        float float32;
        const char *string;
        int *int32_a;
        float *float32_a;
        const char **string_a;
    } value;
} prop;

typedef struct entity {
    uint32_t uid;
    prop *properties;
} entity;

typedef struct scene {
    uint32_t uid;
    entity *entities;
} scene;

//------------------------------------------------------------------------------

file *file_open(const char *filename, file_mode mode) {
    file *file = dlb_calloc(1, sizeof(*file));
    file->filename = filename;
    file->mode = mode;
    file->hnd = fopen(filename, file_mode_str(mode));
    return file;
}

//char *file_read_all(file *f) {
//    DLB_ASSERT(!fseek(f->hnd, 0, SEEK_END));
//    long pos = ftell(f->hnd);
//    DLB_ASSERT(pos <= SIZE_MAX);
//    DLB_ASSERT(!fseek(f->hnd, 0, SEEK_SET));
//
//    size_t end = (size_t)pos;
//    char *buf = dlb_malloc(end + 1);
//    fread(buf, end, 1, f->hnd);
//    buf[end] = 0;
//    return buf;
//}

char file_getc(file *f) {
    f->last = fgetc(f->hnd);
    return f->last;
}

void file_expect(file *f, char chr) {
    char c = file_getc(f);
    DLB_ASSERT(c == chr);
}

void file_close(file *f) {
    fclose(f->hnd);
    free(f);
}

//------------------------------------------------------------------------------

#define SYM_MAX_LEN 256
#define INT_MAX_LEN 32
#define FLOAT_MAX_LEN 64
#define STRING_MAX_LEN 256

dlb_arena string_arena;
symbol *symbols;

static const char *sym_entity;
static const char *sym_int;
static const char *sym_float;
static const char *sym_string;

const char *sym(const char *s, size_t len) {
    DLB_ASSERT(len < SYM_MAX_LEN);
    uint32_t hash = strhash(s);
    for (symbol *i = symbols; i != dlb_vec_end(symbols); i++) {
        if (i->hash == hash && streq(i->str, s, len)) {
            return i->str;
        }
    }

    char *str = dlb_arena_alloc(&string_arena, len + 1);
    memcpy(str, s, len);
    str[len] = 0;

    symbol *sym = dlb_vec_alloc(symbols);
    sym->hash = hash;
    sym->len = len;
    sym->str = str;
    return str;
}

void sym_init() {
    sym_entity = SYM("entity");
    sym_int = SYM("int32");
    sym_float  = SYM("float32");
    sym_string = SYM("string");
}

//------------------------------------------------------------------------------

void eat_chars(char *buf, size_t buf_len, size_t *len, file *f, char delim) {
    for (size_t i = 0; i < buf_len; i++) {
        char c = file_getc(f);
        if (c == delim) {
            if (len) *len = i;
            return;
        }
        buf[i] = c;
    }
    DLB_ASSERT(0); // TODO: Eat extra input until delim found?
}

uint32_t parse_int(file *f, char delim) {
    char buf[INT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, INT_MAX_LEN, 0, f, delim);
    uint32_t value = (uint32_t)atoi(buf);
    return value;
}

float parse_float(file *f, char delim) {
    char buf[FLOAT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, FLOAT_MAX_LEN, 0, f, delim);
    float value = (float)atof(buf);
    return value;
}

const char *parse_string(file *f, char delim) {
    char buf[STRING_MAX_LEN + 1] = { 0 };
    size_t len;
    eat_chars(buf, STRING_MAX_LEN, &len, f, delim);
    return sym(buf, len);
}

//------------------------------------------------------------------------------

prop *prop_find(entity *e, const char *name) {
    bool found = false;
    prop *p = e->properties;
    for (;;) {
        if (p == dlb_vec_end(e->properties)) {
            p = NULL;
            break;
        }
        if (p->name == name) {
            break;
        }
        p++;
    }
    return p;
}

prop *prop_find_or_create(entity *e, const char *name) {
    bool found = false;
    prop *p = e->properties;
    for (;;) {
        if (p == dlb_vec_end(e->properties)) {
            p = dlb_vec_alloc(e->properties);
            p->name = name;
            break;
        }
        if (p->name == name) {
            break;
        }
        p++;
    }
    return p;
}

void prop_set_int(entity *e, const char *name, int value) {
    prop *p = prop_find_or_create(e, name);
    p->type = PROP_INT;
    p->value.int32 = value;
}

void prop_set_float(entity *e, const char *name, float value) {
    prop *p = prop_find_or_create(e, name);
    p->type = PROP_FLOAT;
    p->value.float32 = value;
}

void prop_set_string(entity *e, const char *name, const char *value) {
    prop *p = prop_find_or_create(e, name);
    p->type = PROP_STRING;
    p->length = strlen(value);
    DLB_ASSERT(p->length <= STRING_MAX_LEN);
    p->value.string = value;
}

entity *entity_init(scene *scn, uint32_t uid) {
    if (uid) {
        DLB_ASSERT(uid >= scn->uid);
        scn->uid = uid;
    }
    entity *e = dlb_vec_alloc(scn->entities);
    e->uid = scn->uid++;
    return e;
}

void entity_print(FILE *hnd, entity *e) {
    fprintf(hnd, "!%d:entity\n", e->uid);
    for (prop *prop = e->properties; prop != dlb_vec_end(e->properties); prop++) {
        fprintf(hnd, "  %s:%s", prop->name, prop_type_str(prop->type));
        if (prop->array) {
            fprintf(hnd, "[]");
        }
        switch(prop->type) {
        case PROP_INT:
            fprintf(hnd, " = %d", prop->value.int32);
            break;
        case PROP_FLOAT:
            fprintf(hnd, " = %f", prop->value.float32);
            break;
        case PROP_STRING:
            fprintf(hnd, " = \"%s\"", prop->value.string);
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

void entity_load(entity *e, file *f) {
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
            p->value.int32 = parse_int(f, '\n');
        } else if (type == sym_float) {
            p->type = PROP_FLOAT;
            p->value.float32 = parse_float(f, '\n');
        } else if (type == sym_string) {
            p->type = PROP_STRING;
            file_expect(f, '"');
            p->value.string = parse_string(f, '"');
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
    prop_set_string(e, SYM(STRING(name)), name);
    prop_set_int(e, SYM(STRING(age)), age);
    prop_set_float(e, SYM(STRING(weight)), weight);
    prop_set_string(e, SYM(STRING(height)), height);
    prop_set_string(e, SYM(STRING(city)), city);
    return e;
}

//------------------------------------------------------------------------------

scene *scene_init() {
    scene *scn = dlb_calloc(1, sizeof(*scn));
    scn->uid = 1;
    dlb_vec_reserve(scn->entities, 2);
    return scn;
}

void scene_save(scene *scn, file *entitydb) {
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_save(e, entitydb);
    }
}

scene *scene_load(file *f) {
    scene *scn = scene_init();
    file_getc(f);
    for (;;) {
        switch(f->last) {
        case EOF:
            goto end;
        case '!': {
            uint32_t uid = parse_int(f, ':');
            const char *type = parse_string(f, '\n');

            if (type == sym_entity) {
                entity *e = entity_init(scn, uid);
                entity_load(e, f);
            } else {
                DLB_ASSERT(0); // wtf?
            }
            break;
        }
        default:
            DLB_ASSERT(0); // wtf?
        }
    }
end:
    return scn;
}

void scene_free(scene *scn) {
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_free(e);
    }
    dlb_vec_free(scn->entities);
    free(scn);
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    sym_init();

    const char *filename = "scene.dat";
    scene *scn = scene_init();
    file *scene_dat;

    // Create entities
    entity_create(scn, "Timmy", 42, 123.0f, "5ft 6in", "San Francisco");
    entity_create(scn, "Bobby", 24, 321.0f, "6ft 5in", "Fran Sansisco");

    // Save scene data
    scene_dat = file_open(filename, FILE_WRITE);
    scene_save(scn, scene_dat);
    file_close(scene_dat);

    // Free scene
    scene_free(scn);
    scn = NULL;

    // Load scene data
    scene_dat = file_open(filename, FILE_READ);
    scn = scene_load(scene_dat);
    file_close(scene_dat);

    // Print loaded entities
    printf("-- Entities -------------------------\n");
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_print(stdout, e);
    }
    printf("-------------------------------------\n");

    scene_free(scn);
    printf("fin.");
    getchar();
    return 0;
}
