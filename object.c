#include "object.h"
#include "symbol.h"
#include "file.h"
#include "scene.h"
#include "dlb_types.h"
#include "dlb_vector.h"

ta_schema tg_schemas[F_COUNT];
dlb_hash tg_schemas_by_name;

const char *ta_field_type_str(ta_field_type type) {
    switch(type) {
        case F_ATOM_INT:       return "ATOM_INT";
        case F_ATOM_UINT:      return "ATOM_UINT";
        case F_ATOM_FLOAT:     return "ATOM_FLOAT";
        case F_ATOM_STRING:    return "ATOM_STRING";
        case F_TA_VEC3:        return "TA_VEC3";
        case F_TA_COLOR3:      return "TA_COLOR3";
        case F_TA_VEC4:        return "TA_VEC4";
        case F_TA_COLOR4:      return "TA_COLOR4";
        case F_TA_TRANSFORM:   return "TA_TRANSFORM";
        case F_TA_SUN_LIGHT:   return "TA_SUN_LIGHT";
        case F_TA_POINT_LIGHT: return "TA_POINT_LIGHT";
        case F_TA_MATERIAL:    return "TA_MATERIAL";
        case F_TA_MESH:        return "TA_MESH";
        case F_TA_SHADER:      return "TA_SHADER";
        case F_TA_TEXTURE:     return "TA_TEXTURE";
        case F_TA_ENTITY:      return "TA_ENTITY";
        default: return "<UNKNOWN_TA_FIELD_TYPE>";
    }
};

void obj_field_add(ta_schema *obj, ta_field_type type, const char *name,
    u32 offset, bool alias)
{
    ta_schema_field *field = dlb_vec_alloc(obj->fields);
    field->type = type;
    field->name = name;
    field->offset = offset;
    field->alias = alias;
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
    obj = &tg_schemas[F_TA_VEC3];
    obj->type = F_TA_VEC3;
    obj->name = INTERN(STRING(ta_vec3));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("x"), OFFSETOF(ta_vec3, x), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("y"), OFFSETOF(ta_vec3, y), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("z"), OFFSETOF(ta_vec3, z), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_vec3)), obj);

    obj = &tg_schemas[F_TA_COLOR3];
    obj->type = F_TA_COLOR3;
    obj->name = INTERN("ta_color3");
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("r"), OFFSETOF(ta_vec3, x), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("g"), OFFSETOF(ta_vec3, y), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("b"), OFFSETOF(ta_vec3, z), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR("ta_color3"), obj);

    obj = &tg_schemas[F_TA_VEC4];
    obj->type = F_TA_VEC4;
    obj->name = INTERN(STRING(ta_vec4));
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("x"), OFFSETOF(ta_vec4, x), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("y"), OFFSETOF(ta_vec4, y), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("z"), OFFSETOF(ta_vec4, z), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("w"), OFFSETOF(ta_vec4, w), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_vec4)), obj);

    obj = &tg_schemas[F_TA_COLOR4];
    obj->type = F_TA_COLOR4;
    obj->name = INTERN("ta_color4");
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("r"), OFFSETOF(ta_vec4, x), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("g"), OFFSETOF(ta_vec4, y), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("b"), OFFSETOF(ta_vec4, z), 0);
    obj_field_add(obj, F_ATOM_FLOAT, INTERN("a"), OFFSETOF(ta_vec4, w), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_vec4)), obj);

    obj = &tg_schemas[F_TA_TRANSFORM];
    obj->type = F_TA_TRANSFORM;
    obj->name = INTERN(STRING(ta_transform));
    obj_field_add(obj, F_TA_VEC3, INTERN("position"), OFFSETOF(ta_transform, position), 0);
    obj_field_add(obj, F_TA_VEC4, INTERN("rotation"), OFFSETOF(ta_transform, rotation), 0);
    obj_field_add(obj, F_TA_VEC3, INTERN("scale"),    OFFSETOF(ta_transform, scale), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_transform)), obj);

    // Scene-level object types
    obj = &tg_schemas[F_TA_SUN_LIGHT];
    obj->type = F_TA_SUN_LIGHT;
    obj->name = INTERN(STRING(ta_sun_light));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"),      OFFSETOF(ta_sun_light, name), 0);
    obj_field_add(obj, F_TA_VEC3,    INTERN("direction"), OFFSETOF(ta_sun_light, direction), 0);
    obj_field_add(obj, F_TA_COLOR3,  INTERN("color"),     OFFSETOF(ta_sun_light, color), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_sun_light)), obj);

    obj = &tg_schemas[F_TA_POINT_LIGHT];
    obj->type = F_TA_POINT_LIGHT;
    obj->name = INTERN(STRING(ta_point_light));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"),     OFFSETOF(ta_point_light, name), 0);
    obj_field_add(obj, F_TA_VEC3,    INTERN("position"), OFFSETOF(ta_point_light, position), 0);
    obj_field_add(obj, F_TA_COLOR3,  INTERN("color"),    OFFSETOF(ta_point_light, color), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_point_light)), obj);

    obj = &tg_schemas[F_TA_MATERIAL];
    obj->type = F_TA_MATERIAL;
    obj->name = INTERN(STRING(ta_material));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_material, name), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_material)), obj);

    obj = &tg_schemas[F_TA_MESH];
    obj->type = F_TA_MESH;
    obj->name = INTERN(STRING(ta_mesh));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_mesh, name), 0);
    obj_field_add(obj, F_ATOM_STRING, INTERN("path"), OFFSETOF(ta_mesh, path), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_mesh)), obj);

    obj = &tg_schemas[F_TA_SHADER];
    obj->type = F_TA_SHADER;
    obj->name = INTERN(STRING(ta_shader));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_shader, name), 0);
    obj_field_add(obj, F_ATOM_STRING, INTERN("path"), OFFSETOF(ta_shader, path), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_shader)), obj);

    obj = &tg_schemas[F_TA_TEXTURE];
    obj->type = F_TA_TEXTURE;
    obj->name = INTERN(STRING(ta_texture));
    obj_field_add(obj, F_ATOM_STRING, INTERN("name"), OFFSETOF(ta_texture, name), 0);
    obj_field_add(obj, F_ATOM_STRING, INTERN("path"), OFFSETOF(ta_texture, path), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_texture)), obj);

    obj = &tg_schemas[F_TA_ENTITY];
    obj->type = F_TA_ENTITY;
    obj->name = INTERN(STRING(ta_entity));
    obj_field_add(obj, F_ATOM_STRING,   INTERN("name"),      OFFSETOF(ta_entity, name), 0);
    obj_field_add(obj, F_ATOM_STRING,   INTERN("material"),  OFFSETOF(ta_entity, material), 0);
    obj_field_add(obj, F_ATOM_STRING,   INTERN("mesh"),      OFFSETOF(ta_entity, mesh), 0);
    obj_field_add(obj, F_ATOM_STRING,   INTERN("shader"),    OFFSETOF(ta_entity, shader), 0);
    obj_field_add(obj, F_ATOM_STRING,   INTERN("texture"),   OFFSETOF(ta_entity, texture), 0);
    obj_field_add(obj, F_TA_TRANSFORM, INTERN("transform"), OFFSETOF(ta_entity, transform), 0);
    dlb_hash_insert(&tg_schemas_by_name, CSTR(STRING(ta_entity)), obj);
}

void obj_print(FILE *f, ta_field_type type, u8 *ptr, int level)
{
    ta_schema *schema = &tg_schemas[type];
    if (level == 0) {
        fprintf(f, "%s:\n", schema->name);
    }

    for (ta_schema_field *field = schema->fields; field != dlb_vec_end(schema->fields); field++)
    {
        if (field->alias) {
            continue;
        }

        for (int i = 0; i < level + 1; i++) {
            fprintf(f, "  ");
        }

        if (field->type > F_ATOM_END) {
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
    DLB_ASSERT(ref->type = F_TA_MATERIAL);
    obj_print(stdout, ref->type, ref->ptr, 0);
    return ref->ptr;
}