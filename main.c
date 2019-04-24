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

ta_entity *entity_create(scene *scn, const char *name) {
    ta_entity *e = scene_obj_init(scn, OBJ_TA_ENTITY, 0);
    e->type = ENTITY_DEFAULT;
    e->name = name;
    return e;
}

void write_scene(const char *filename) {
    scene *scene = scene_init("test scene");
    entity_create(scene, "Timmy");
    entity_create(scene, "Bobby");

    printf("[WRITE: %s]\n", filename);
    scene_print(scene);
    printf("\n");

    file *data_file = file_open(filename, FILE_WRITE);
    scene_save(data_file, scene);
    file_close(data_file);
    scene_free(scene);
}

void read_scene(const char *filename) {
    file *data_file = file_open(filename, FILE_READ);
    scene *scene = scene_load(data_file);
    file_close(data_file);

    printf("[READ %s]\n", filename);
    scene_print(scene);
    printf("\n");
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
    getchar();
    exit(1);
}

int main(int argc, char *argv[]) {
    dlb_assert_handler = assert_handler;
    tests();
    init_symbols();
    obj_register();
    init_type_loaders();

    //const char *filename = "data/scene.dml";
    //write_scene(filename);
    //read_scene(filename);

    const char *filname_cus = "data/custom.dml";
    read_scene(filname_cus);

    printf("fin.\n");
	getchar();
    return 0;
}