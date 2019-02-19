#include "scene.h"
#include "file.h"
#include "entity.h"
#include "parse.h"
#include "symbol.h"
#include "dlb_types.h"
#include "dlb_memory.h"
#include "dlb_vector.h"
#include "dlb_hash.h"

struct dlb_hash type_loaders;
void init_type_loaders() {
    dlb_hash_init(&type_loaders, "Type Loaders", 16);
    //dlb_hash_insert(&type_loaders, dlb_symbol_hash(sym_entity), 0);
};

scene *scene_init(const char *name) {
    scene *scn = dlb_calloc(1, sizeof(*scn));
    scn->name = name;
    scn->next_uid = 1;
    dlb_vec_reserve(scn->entities, 2);
    return scn;
}

void scene_free(scene *scn) {
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_free(e);
    }
    dlb_vec_free(scn->entities);
    free(scn);
}

void scene_save(scene *scn, file *entitydb) {
    fprintf(entitydb->hnd, "%s\n", scn->name);
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_save(e, entitydb);
    }
}

scene *scene_load(file *f) {
    const char *name = read_string(f, CHAR_EOL, CHAR_STRING_LITERAL);
    file_expect_char(f, CHAR_EOL, 1);
    file_allow_char(f, CHAR_EOL, 0);
    scene *scn = scene_init(name);
    for (;;) {
        char c = file_char(f);
        switch(c) {
        case EOF:
            goto end;
        case '!': {
            unsigned int uid = read_uint(f, ":");
            file_expect_char(f, ":", 1);
            const char *type = read_string(f, CHAR_EOL, CHAR_TYPE);
            file_expect_char(f, CHAR_EOL, 1);
            file_allow_char(f, CHAR_EOL, 0);

            // TODO: Register type names (e.g. entity) in lookup table that maps
            //       them to their respective loader
            if (type == sym_entity) {
                entity_load(scn, uid, f);
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

void scene_print(scene *scn) {
    // Print loaded entities
    printf("name: %s\n", scn->name);
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_print(stdout, e);
    }
    fflush(stdout);
}