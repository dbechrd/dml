#include "object.h"
#include "symbol.h"
#include "file.h"
#include "scene.h"
#include "dlb_types.h"
#include "dlb_vector.h"

ta_schema tg_schemas[F_OBJ_COUNT];
dlb_hash tg_schemas_by_name;

const char *ta_object_type_str(ta_field_type type) {
    switch(type) {
        case F_OBJ_ENTITY:   return "F_OBJ_ENTITY";
        case F_OBJ_TEXTURE:  return "F_OBJ_TEXTURE";
        case F_OBJ_MATERIAL: return "F_OBJ_MATERIAL";
        case F_OBJ_MESH:     return "F_OBJ_MESH";
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
    obj = &tg_schemas[F_ATOM_INT];
    obj->type = F_ATOM_INT;
    obj->name = INTERN(STRING(int));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(int)), obj);

    obj = &tg_schemas[F_ATOM_FLOAT];
    obj->type = F_ATOM_FLOAT;
    obj->name = INTERN(STRING(float));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(float)), obj);

    obj = &tg_schemas[F_ATOM_STRING];
    obj->type = F_ATOM_STRING;
    obj->name = INTERN(STRING(const char *));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(const char *)), obj);

    // Compound types
    obj = &tg_schemas[F_OBJ_VEC3];
    obj->type = F_OBJ_VEC3;
    obj->name = INTERN(STRING(ta_vec3));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("x"), OFFSETOF(ta_vec3, x));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("y"), OFFSETOF(ta_vec3, y));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("z"), OFFSETOF(ta_vec3, z));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_vec3)), obj);

    obj = &tg_schemas[F_OBJ_VEC4];
    obj->type = F_OBJ_VEC4;
    obj->name = INTERN(STRING(ta_vec4));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("x"), OFFSETOF(ta_vec4, x));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("y"), OFFSETOF(ta_vec4, y));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("z"), OFFSETOF(ta_vec4, z));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("w"), OFFSETOF(ta_vec4, w));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_vec4)), obj);

    obj = &tg_schemas[F_OBJ_TRANSFORM];
    obj->type = F_OBJ_TRANSFORM;
    obj->name = INTERN(STRING(ta_transform));
    obj_field_add(obj, F_OBJ_VEC3, INTERN("position"), OFFSETOF(ta_transform, position));
    obj_field_add(obj, F_OBJ_VEC4, INTERN("rotation"), OFFSETOF(ta_transform, rotation));
    obj_field_add(obj, F_OBJ_VEC3, INTERN("scale"),    OFFSETOF(ta_transform, scale));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_transform)), obj);

    // Scene-level object types
    obj = &tg_schemas[F_OBJ_ENTITY];
    obj->type = F_OBJ_ENTITY;
    obj->name = INTERN(STRING(ta_entity));
    obj_field_add(obj, F_ATOM_STRING,       INTERN("name"),      OFFSETOF(ta_entity, name));
    obj_field_add(obj, F_ATOM_STRING,       INTERN("material"),  OFFSETOF(ta_entity, material));
    obj_field_add(obj, F_ATOM_STRING,       INTERN("mesh"),      OFFSETOF(ta_entity, mesh));
    obj_field_add(obj, F_ATOM_STRING,       INTERN("shader"),    OFFSETOF(ta_entity, shader));
    obj_field_add(obj, F_ATOM_STRING,       INTERN("texture"),   OFFSETOF(ta_entity, texture));
    obj_field_add(obj, F_OBJ_TRANSFORM,     INTERN("transform"), OFFSETOF(ta_entity, transform));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_entity)), obj);

    obj = &tg_schemas[F_OBJ_MATERIAL];
    obj->type = F_OBJ_MATERIAL;
    obj->name = INTERN(STRING(ta_material));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_material, name));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_material)), obj);

    obj = &tg_schemas[F_OBJ_MESH];
    obj->type = F_OBJ_MESH;
    obj->name = INTERN(STRING(ta_mesh));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_mesh, name));
    obj_field_add(obj, F_ATOM_STRING, INTERN("path"), OFFSETOF(ta_mesh, path));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_mesh)), obj);

    obj = &tg_schemas[F_OBJ_SHADER];
    obj->type = F_OBJ_SHADER;
    obj->name = INTERN(STRING(ta_shader));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_shader, name));
    obj_field_add(obj, F_ATOM_STRING, INTERN("path"), OFFSETOF(ta_shader, path));
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_shader)), obj);

    obj = &tg_schemas[F_OBJ_TEXTURE];
    obj->type = F_OBJ_TEXTURE;
    obj->name = INTERN(STRING(ta_texture));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_texture, name));
    obj_field_add(obj, F_ATOM_STRING, INTERN("path"), OFFSETOF(ta_texture, path));
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
        INDENT(f, level + 1);
        if (field->type >= F_OBJ) {
            fprintf(f, "%s:\n", field->name);
            obj_print(f, field->type, ptr + field->offset, level + 1);
        } else {
            fprintf(f, "%s: ", field->name);
            switch (field->type ) {
                case F_ATOM_INT: {
                    int *val = (int *)(ptr + field->offset);
                    fprintf(f, "%d\n", *val);
                    break;
                } case F_ATOM_UINT: {
                    u32 *val = (u32 *)(ptr + field->offset);
                    fprintf(f, "%u\n", *val);
                    break;
                } case F_ATOM_FLOAT: {
                    float *val = (float *)(ptr + field->offset);
                    fprintf(f, "0x%08X (%f)\n", *(u32 *)val, *val);
                    break;
                } case F_ATOM_STRING: {
                    const char **val = (const char **)(ptr + field->offset);
                    fprintf(f, "\"%s\"  # %08X\n", IFNULL(*val, ""), (u32)*val);
                    break;
                } default: {
                    PANIC("Unexpected field type, don't know how to print");
                }
            }
        }
    }
}

ta_material *entity_material(ta_entity *e)
{
    scene_ref *ref = dlb_hash_search(&e->scene->refs_by_name, CSTR("material_name"));
    DLB_ASSERT(ref);
    DLB_ASSERT(ref->type = F_OBJ_MATERIAL);
    obj_print(stdout, ref->type, ref->ptr, 0);
    return ref->ptr;
}