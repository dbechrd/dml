#include "object.h"
#include "symbol.h"
#include "file.h"
#include "dlb_types.h"
#include "dlb_vector.h"

ta_object tg_objects[OBJ_COUNT];
dlb_hash tg_objects_by_name;

const char *ta_object_type_str(ta_object_type type) {
    switch(type) {
        case OBJ_TA_ENTITY:   return "OBJ_TA_ENTITY";
        case OBJ_TA_TEXTURE:  return "OBJ_TA_TEXTURE";
        case OBJ_TA_MATERIAL: return "OBJ_TA_MATERIAL";
        case OBJ_TA_MESH:     return "OBJ_TA_MESH";
        default: DLB_ASSERT(!"Unknown object type"); return 0;
    }
};

void obj_field_add(ta_object *obj, ta_object_type type, const char *name,
    u32 offset)
{
    ta_object_field *fo = dlb_vec_alloc(obj->fields);
    fo->type = type;
    fo->name = name;
    fo->offset = offset;
}

ta_object_field *obj_field_find(ta_object_type type, const char *name)
{
    ta_object_field *field = 0;
    ta_object *obj = &tg_objects[type];
    for (ta_object_field *f = obj->fields; f != dlb_vec_end(obj->fields); f++) {
        if (f->name == name) {
            field = f;
            break;
        }
    }
    return field;
}

void obj_register()
{
    DLB_ASSERT(!tg_objects_by_name.size);
    dlb_hash_init(&tg_objects_by_name, DLB_HASH_STRING, "[obj_register]", 16);
    ta_object *obj;

    // Atomic types
    obj = &tg_objects[OBJ_INT];
    obj->type = OBJ_INT;
    obj->name = INTERN(STRING(int));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(int)), obj);

    obj = &tg_objects[OBJ_FLOAT];
    obj->type = OBJ_FLOAT;
    obj->name = INTERN(STRING(float));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(float)), obj);

    obj = &tg_objects[OBJ_STRING];
    obj->type = OBJ_STRING;
    obj->name = INTERN(STRING(const char *));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(const char *)), obj);

    // Compound types
    obj = &tg_objects[OBJ_TA_VEC3];
    obj->type = OBJ_TA_VEC3;
    obj->name = INTERN(STRING(ta_vec3));
    obj_field_add(obj, OBJ_FLOAT, INTERN("x"), OFFSETOF(ta_vec3, x));
    obj_field_add(obj, OBJ_FLOAT, INTERN("y"), OFFSETOF(ta_vec3, y));
    obj_field_add(obj, OBJ_FLOAT, INTERN("z"), OFFSETOF(ta_vec3, z));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_vec3)), obj);

    obj = &tg_objects[OBJ_TA_VEC4];
    obj->type = OBJ_TA_VEC4;
    obj->name = INTERN(STRING(ta_vec4));
    obj_field_add(obj, OBJ_FLOAT, INTERN("x"), OFFSETOF(ta_vec4, x));
    obj_field_add(obj, OBJ_FLOAT, INTERN("y"), OFFSETOF(ta_vec4, y));
    obj_field_add(obj, OBJ_FLOAT, INTERN("z"), OFFSETOF(ta_vec4, z));
    obj_field_add(obj, OBJ_FLOAT, INTERN("w"), OFFSETOF(ta_vec4, w));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_vec4)), obj);

    obj = &tg_objects[OBJ_TA_TRANSFORM];
    obj->type = OBJ_TA_TRANSFORM;
    obj->name = INTERN(STRING(ta_transform));
    obj_field_add(obj, OBJ_TA_VEC3, INTERN("position"), OFFSETOF(ta_transform, position));
    obj_field_add(obj, OBJ_TA_VEC4, INTERN("rotation"), OFFSETOF(ta_transform, rotation));
    obj_field_add(obj, OBJ_TA_VEC3, INTERN("scale"),    OFFSETOF(ta_transform, scale));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_transform)), obj);

    // Scene-level object types
    obj = &tg_objects[OBJ_TA_ENTITY];
    obj->type = OBJ_TA_ENTITY;
    obj->name = INTERN(STRING(ta_entity));
    obj_field_add(obj, OBJ_STRING,       INTERN("name"),      OFFSETOF(ta_entity, name));
    obj_field_add(obj, OBJ_STRING,       INTERN("material"),  OFFSETOF(ta_entity, material));
    obj_field_add(obj, OBJ_STRING,       INTERN("mesh"),      OFFSETOF(ta_entity, mesh));
    obj_field_add(obj, OBJ_STRING,       INTERN("shader"),    OFFSETOF(ta_entity, shader));
    obj_field_add(obj, OBJ_STRING,       INTERN("texture"),   OFFSETOF(ta_entity, texture));
    obj_field_add(obj, OBJ_TA_TRANSFORM, INTERN("transform"), OFFSETOF(ta_entity, transform));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_entity)), obj);

    obj = &tg_objects[OBJ_TA_MATERIAL];
    obj->type = OBJ_TA_MATERIAL;
    obj->name = INTERN(STRING(ta_material));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_material, name));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_material)), obj);

    obj = &tg_objects[OBJ_TA_MESH];
    obj->type = OBJ_TA_MESH;
    obj->name = INTERN(STRING(ta_mesh));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_mesh, name));
    obj_field_add(obj, OBJ_STRING, INTERN("path"), OFFSETOF(ta_mesh, path));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_mesh)), obj);

    obj = &tg_objects[OBJ_TA_SHADER];
    obj->type = OBJ_TA_SHADER;
    obj->name = INTERN(STRING(ta_shader));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_shader, name));
    obj_field_add(obj, OBJ_STRING, INTERN("path"), OFFSETOF(ta_shader, path));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_shader)), obj);

    obj = &tg_objects[OBJ_TA_TEXTURE];
    obj->type = OBJ_TA_TEXTURE;
    obj->name = INTERN(STRING(ta_texture));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_texture, name));
    obj_field_add(obj, OBJ_STRING, INTERN("path"), OFFSETOF(ta_texture, path));
    dlb_hash_insert(&tg_objects_by_name, CSTR(STRING(ta_texture)), obj);
}

void ta_material_print(FILE *f, ta_material *o)
{
    fprintf(f, "ta_material:\n");
    fprintf(f, "  name: \"%s\"\n", IFNULL(o->name, ""));
}

void ta_mesh_print(FILE *f, ta_mesh *o)
{
    fprintf(f, "ta_mesh:\n");
    fprintf(f, "  name: \"%s\"\n", IFNULL(o->name, ""));
    fprintf(f, "  path: \"%s\"\n", IFNULL(o->path, ""));
}

void ta_shader_print(FILE *f, ta_shader *o)
{
    fprintf(f, "ta_shader:\n");
    fprintf(f, "  name: \"%s\"\n", IFNULL(o->name, ""));
    fprintf(f, "  path: \"%s\"\n", IFNULL(o->path, ""));
}

void ta_texture_print(FILE *f, ta_texture *o)
{
    fprintf(f, "ta_texture:\n");
    fprintf(f, "  name: \"%s\"\n", IFNULL(o->name, ""));
    fprintf(f, "  path: \"%s\"\n", IFNULL(o->path, ""));
}

void ta_entity_print(FILE *f, ta_entity *o)
{
    fprintf(f, "ta_entity:\n");
    fprintf(f, "  name:     \"%s\"\n", IFNULL(o->name,     ""));
    fprintf(f, "  material: \"%s\"\n", IFNULL(o->material, ""));
    fprintf(f, "  mesh:     \"%s\"\n", IFNULL(o->mesh,     ""));
    fprintf(f, "  shader:   \"%s\"\n", IFNULL(o->shader,   ""));
    fprintf(f, "  texture:  \"%s\"\n", IFNULL(o->texture,  ""));
    fprintf(f, "  transform:\n");
    fprintf(f, "    position:\n");
    fprintf(f, "      x: %f\n", o->transform.position.x);
    fprintf(f, "      y: %f\n", o->transform.position.y);
    fprintf(f, "      z: %f\n", o->transform.position.z);
    fprintf(f, "    rotation:\n");
    fprintf(f, "      x: %f\n", o->transform.rotation.x);
    fprintf(f, "      y: %f\n", o->transform.rotation.y);
    fprintf(f, "      z: %f\n", o->transform.rotation.z);
    fprintf(f, "      w: %f\n", o->transform.rotation.w);
    fprintf(f, "    scale:\n");
    fprintf(f, "      x: %f\n", o->transform.scale.x);
    fprintf(f, "      y: %f\n", o->transform.scale.y);
    fprintf(f, "      z: %f\n", o->transform.scale.z);
}