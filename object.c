#include "object.h"
#include "symbol.h"
#include "file.h"
#include "dlb_types.h"
#include "dlb_vector.h"

ta_schema tg_schemas[OBJ_COUNT];
dlb_hash tg_schemas_by_name;

const char *ta_object_type_str(ta_field_type type) {
    switch(type) {
        case OBJ_TA_ENTITY:   return "OBJ_TA_ENTITY";
        case OBJ_TA_TEXTURE:  return "OBJ_TA_TEXTURE";
        case OBJ_TA_MATERIAL: return "OBJ_TA_MATERIAL";
        case OBJ_TA_MESH:     return "OBJ_TA_MESH";
        default: DLB_ASSERT(!"Unknown object type"); return 0;
    }
};

void obj_field_add(ta_schema *obj, ta_field_type type, const char *name,
    u32 offset)
{
    ta_schema_field *fo = dlb_vec_alloc(obj->fields);
    fo->type = type;
    fo->name = name;
    fo->offset = offset;
}

ta_schema_field *obj_field_find(ta_field_type type, const char *name)
{
    ta_schema_field *field = 0;
    ta_schema *obj = &tg_schemas[type];
    for (ta_schema_field *f = obj->fields; f != dlb_vec_end(obj->fields); f++) {
        if (f->name == name) {
            field = f;
            break;
        }
    }
    return field;
}

void obj_register()
{
    DLB_ASSERT(!tg_schemas_by_name.size);
    dlb_hash_init(&tg_schemas_by_name, DLB_HASH_STRING, "[obj_register]", 16);
    ta_schema *obj;

    // Atomic types
    obj = &tg_schemas[OBJ_INT];
    obj->type = OBJ_INT;
    obj->name = INTERN(STRING(int));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(int)), obj);

    obj = &tg_schemas[OBJ_FLOAT];
    obj->type = OBJ_FLOAT;
    obj->name = INTERN(STRING(float));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(float)), obj);

    obj = &tg_schemas[OBJ_STRING];
    obj->type = OBJ_STRING;
    obj->name = INTERN(STRING(const char *));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(const char *)), obj);

    // Compound types
    obj = &tg_schemas[OBJ_TA_VEC3];
    obj->type = OBJ_TA_VEC3;
    obj->name = INTERN(STRING(ta_vec3));
    obj_field_add(obj, OBJ_FLOAT, INTERN("x"), OFFSETOF(ta_vec3, x));
    obj_field_add(obj, OBJ_FLOAT, INTERN("y"), OFFSETOF(ta_vec3, y));
    obj_field_add(obj, OBJ_FLOAT, INTERN("z"), OFFSETOF(ta_vec3, z));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_vec3)), obj);

    obj = &tg_schemas[OBJ_TA_VEC4];
    obj->type = OBJ_TA_VEC4;
    obj->name = INTERN(STRING(ta_vec4));
    obj_field_add(obj, OBJ_FLOAT, INTERN("x"), OFFSETOF(ta_vec4, x));
    obj_field_add(obj, OBJ_FLOAT, INTERN("y"), OFFSETOF(ta_vec4, y));
    obj_field_add(obj, OBJ_FLOAT, INTERN("z"), OFFSETOF(ta_vec4, z));
    obj_field_add(obj, OBJ_FLOAT, INTERN("w"), OFFSETOF(ta_vec4, w));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_vec4)), obj);

    obj = &tg_schemas[OBJ_TA_TRANSFORM];
    obj->type = OBJ_TA_TRANSFORM;
    obj->name = INTERN(STRING(ta_transform));
    obj_field_add(obj, OBJ_TA_VEC3, INTERN("position"), OFFSETOF(ta_transform, position));
    obj_field_add(obj, OBJ_TA_VEC4, INTERN("rotation"), OFFSETOF(ta_transform, rotation));
    obj_field_add(obj, OBJ_TA_VEC3, INTERN("scale"),    OFFSETOF(ta_transform, scale));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_transform)), obj);

    // Scene-level object types
    obj = &tg_schemas[OBJ_TA_ENTITY];
    obj->type = OBJ_TA_ENTITY;
    obj->name = INTERN(STRING(ta_entity));
    obj_field_add(obj, OBJ_STRING,       INTERN("name"),      OFFSETOF(ta_entity, name));
    obj_field_add(obj, OBJ_STRING,       INTERN("material"),  OFFSETOF(ta_entity, material));
    obj_field_add(obj, OBJ_STRING,       INTERN("mesh"),      OFFSETOF(ta_entity, mesh));
    obj_field_add(obj, OBJ_STRING,       INTERN("shader"),    OFFSETOF(ta_entity, shader));
    obj_field_add(obj, OBJ_STRING,       INTERN("texture"),   OFFSETOF(ta_entity, texture));
    obj_field_add(obj, OBJ_TA_TRANSFORM, INTERN("transform"), OFFSETOF(ta_entity, transform));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_entity)), obj);

    obj = &tg_schemas[OBJ_TA_MATERIAL];
    obj->type = OBJ_TA_MATERIAL;
    obj->name = INTERN(STRING(ta_material));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_material, name));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_material)), obj);

    obj = &tg_schemas[OBJ_TA_MESH];
    obj->type = OBJ_TA_MESH;
    obj->name = INTERN(STRING(ta_mesh));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_mesh, name));
    obj_field_add(obj, OBJ_STRING, INTERN("path"), OFFSETOF(ta_mesh, path));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_mesh)), obj);

    obj = &tg_schemas[OBJ_TA_SHADER];
    obj->type = OBJ_TA_SHADER;
    obj->name = INTERN(STRING(ta_shader));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_shader, name));
    obj_field_add(obj, OBJ_STRING, INTERN("path"), OFFSETOF(ta_shader, path));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_shader)), obj);

    obj = &tg_schemas[OBJ_TA_TEXTURE];
    obj->type = OBJ_TA_TEXTURE;
    obj->name = INTERN(STRING(ta_texture));
    obj_field_add(obj, OBJ_STRING, INTERN("name"), OFFSETOF(ta_texture, name));
    obj_field_add(obj, OBJ_STRING, INTERN("path"), OFFSETOF(ta_texture, path));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_texture)), obj);
}

#define INDENT(f, l) for (int i = 0; i < l; i++) { fprintf(f, "  "); }

void obj_print(FILE *f, ta_field_type type, u8 *ptr, int level)
{
    ta_schema *schema = &tg_schemas[type];
    if (level == 0) {
        fprintf(f, "%s:\n", schema->name);
    }

    for (ta_schema_field *field = schema->fields; field != dlb_vec_end(schema->fields); field++)
    {
        if (field->type > OBJ_ATOMIC_LAST) {
            INDENT(f, level + 1);
            fprintf(f, "%s:\n", field->name);
            obj_print(f, field->type, ptr + field->offset, level + 1);
        } else {
            INDENT(f, level + 1);
            fprintf(f, "%s: ", field->name);
            switch (field->type ) {
                case OBJ_INT: {
                    fprintf(f, "%d\n", *(int *)(ptr + field->offset));
                    break;
                } case OBJ_UINT: {
                    fprintf(f, "%u\n", *(u32 *)(ptr + field->offset));
                    break;
                } case OBJ_FLOAT: {
                    fprintf(f, "%f\n", *(float *)(ptr + field->offset));
                    break;
                } case OBJ_STRING: {
                    fprintf(f, "\"%s\"\n", IFNULL(*(const char **)(ptr + field->offset), ""));
                    break;
                } default: {
                    PANIC("Unexpected field type, don't know how to print");
                }
            }
        }
    }
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