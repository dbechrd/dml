#include "scene.h"
#include "file.h"
#include "parse.h"
#include "object.h"
#include "symbol.h"
#include "dlb_memory.h"
#include "dlb_vector.h"
#include "dlb_hash.h"
#include "dlb_arena.h"
#include <stdio.h>
#include <string.h>

#define DLB_MEMORY_IMPLEMENTATION
#include "dlb_memory.h"
#define DLB_VECTOR_IMPLEMENTATION
#include "dlb_vector.h"
#define DLB_HASH_IMPLEMENTATION
#define DLB_HASH_TEST
#include "dlb_hash.h"
#define DLB_ARENA_IMPLEMENTATION
#include "dlb_arena.h"

ta_entity *entity_create(ta_scene *scn, const char *name) {
    ta_entity *e = scene_obj_init(scn, F_OBJ_ENTITY);
    e->type = ENTITY_DEFAULT;
    e->name = name;
    e->transform.position.x = 1.1f;
    e->transform.position.y = 1.2f;
    e->transform.position.z = 1.3f;
    e->transform.rotation.x = 2.1f;
    e->transform.rotation.y = 2.2f;
    e->transform.rotation.z = 2.3f;
    e->transform.rotation.w = 2.4f;
    e->transform.scale.x = 3.1f;
    e->transform.scale.y = 3.2f;
    e->transform.scale.z = 3.3f;
    return e;
}

void write_scene(const char *filename) {
    ta_scene *scene = scene_init("test scene");
    entity_create(scene, "Timmy");
    entity_create(scene, "Bobby");

    printf("[WRITE: %s]\n", filename);
    scene_print(scene, stdout);
    printf("\n");

    file *data_file = file_open(filename, FILE_WRITE);
    scene_print(scene, data_file->hnd);
    file_close(data_file);
    scene_free(scene);
}

void read_scene(const char *filename) {
    file *data_file = file_open(filename, FILE_READ);
    ta_scene *scene = scene_load(data_file);
    file_close(data_file);

    printf("[READ %s]\n", filename);
    fprintf(stdout, "---------------------------------\n");
    scene_print(scene, stdout);
    fprintf(stdout, "---------------------------------\n");
    scene_free(scene);
}

void tests() {
    parse_tests();
    dlb_hash_test();
}

DLB_ASSERT_HANDLER(assert_handler) {
    fprintf(stderr,
        "\n---[DLB_ASSERT_HANDLER]---------------------------------------------------------\n"
        "Source file: %s:%d\n\n"
        "%s\n"
        "--------------------------------------------------------------------------------\n",
        filename, line, expr
    );
    UNUSED(getchar());
    exit(1);
}

int main(int argc, char *argv[]) {
    dlb_assert_handler = assert_handler;
    tests();
    init_symbols();
    obj_register();

    //const char *filename = "data/scene.dml";
    //write_scene(filename);
    //read_scene(filename);

    const char *filename = "data/custom.dml";
    read_scene(filename);

    printf("\nfin.\n");
    UNUSED(getchar());
    return 0;
}