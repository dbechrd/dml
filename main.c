#include "scene.h"
#include "file.h"
#include "parse.h"
//#include "prop.h"
#include "symbol.h"
#include "entity.h"
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

entity *entity_create(scene *scn, const char *name, int age, float weight,
                      const char *height, const char *city) {
    entity *e = entity_init(scn, ENTITY_GENERAL, 0);
    prop_set_string(e, intern(CSTR("name")), name);
    prop_set_int(e, intern(CSTR("age")), age);
    prop_set_float(e, intern(CSTR("weight")), weight);
    prop_set_string(e, intern(CSTR("height")), height);
    prop_set_string(e, intern(CSTR("city")), city);
    return e;
}

void write_scene(const char *filename) {
    scene *scene = scene_init("test scene");
    entity_create(scene, "Timmy", 42, 123.0f, "5ft 6in", "San Francisco");
    entity_create(scene, "Bobby", 24, 321.0f, "6ft 5in", "Fran Sansisco");

    printf("[WRITE: %s]\n", filename);
    scene_print(scene);
    printf("\n");

    file *data_file = file_open(filename, FILE_WRITE);
    scene_save(scene, data_file);
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

void print(char *blah) {
    float bleh = parse_float(blah);
    printf("%s = %f [0x%x]\n", blah, bleh, *(unsigned *)&bleh);
}

void test_float_parse()
{
    float a = 123.0f;
    float b = parse_float("123.0f");
    float c = parse_float("0x42f60000");
    float d = parse_float("0x42f60000(123)");
    DLB_ASSERT(b == a);
    DLB_ASSERT(c == a);
    DLB_ASSERT(d == a);
}

void tests() {
    test_float_parse();
    dlb_hash_test();
}

DLB_ASSERT_HANDLER(assert_handler) {
    PANIC("%s:%d [ASSERT FAILED] %s", filename, line, expr);
}

int main(int argc, char *argv[]) {
    dlb_assert_handler = assert_handler;
    tests();
    init_symbols();
    init_type_loaders();

    //const char *filename = "data/scene.dml";
    //write_scene(filename);
    //read_scene(filename);

    const char *filname_cus = "data/chet_alpha.dml";
    read_scene(filname_cus);

    printf("fin.\n");
	getchar();
    return 0;
}