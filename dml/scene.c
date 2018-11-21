#include "scene.h"
#include "file.h"
#include "entity.h"
#include "parse.h"
#include "symbol.h"
#include "dlb_types.h"
#include "dlb_memory.h"
#include "dlb_vector.h"

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