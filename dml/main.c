#include "scene.h"
#include "file.h"
//#include "prop.h"
#include "symbol.h"
//#include "entity.h"
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
#include "dlb_hash.h"
#define DLB_ARENA_IMPLEMENTATION
#include "dlb_arena.h"

int main(int argc, char *argv[]) {
    sym_init();

    const char *filename = "data/scene.dat";
    scene *scene = scene_init();
    file *scene_dat;

    // Create entities
    entity_create(scene, "Timmy", 42, 123.0f, "5ft 6in", "San Francisco");
    entity_create(scene, "Bobby", 24, 321.0f, "6ft 5in", "Fran Sansisco");

    // Save scene data
    scene_dat = file_open(filename, FILE_WRITE);
    scene_save(scene, scene_dat);
    file_close(scene_dat);

    // Free scene
    scene_free(scene);
    scene = NULL;

    // Load scene data
    scene_dat = file_open(filename, FILE_READ);
    scene = scene_load(scene_dat);
    file_close(scene_dat);

    // Print loaded entities
    printf("-- Entities -------------------------\n");
    for (entity *e = scene->entities; e != dlb_vec_end(scene->entities); e++) {
        entity_print(stdout, e);
    }
    printf("-------------------------------------\n");

    scene_free(scene);
    printf("fin.");
    getchar();
    return 0;
}
