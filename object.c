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
    obj_field_add(obj, OBJ_INT,          INTERN("material"),  OFFSETOF(ta_entity, material));
    obj_field_add(obj, OBJ_INT,          INTERN("mesh"),      OFFSETOF(ta_entity, mesh));
    obj_field_add(obj, OBJ_INT,          INTERN("shader"),    OFFSETOF(ta_entity, shader));
    obj_field_add(obj, OBJ_INT,          INTERN("texture"),   OFFSETOF(ta_entity, texture));
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

#if 0
void material_init(ta_material *material)
{
    static ta_object obj = { 0 };
    if (!obj.type) {
        obj.type = OBJ_MATERIAL;
        obj.name = INTERN("ta_material");
        obj_field_add(&obj, FIELD_STRING, INTERN("name"), OFFSETOF(ta_material, name));
    }
    material->object = &obj;
}

void mesh_init(ta_mesh *mesh)
{
    static ta_object obj = { 0 };
    if (!obj.type) {
        obj.type = OBJ_MESH;
        obj.name = INTERN("ta_mesh");
        obj_field_add(&obj, FIELD_STRING, INTERN("name"), OFFSETOF(ta_mesh, name));
        obj_field_add(&obj, FIELD_STRING, INTERN("path"), OFFSETOF(ta_mesh, path));
    }
    mesh->object = &obj;
}

void shader_init(ta_shader *shader)
{
    static ta_object obj = { 0 };
    if (!obj.type) {
        obj.type = OBJ_SHADER;
        obj.name = INTERN("ta_shader");
        obj_field_add(&obj, FIELD_STRING, INTERN("name"), OFFSETOF(ta_shader, name));
        obj_field_add(&obj, FIELD_STRING, INTERN("path"), OFFSETOF(ta_shader, path));
    }
    shader->object = &obj;
}

void texture_init(ta_texture *texture)
{
    static ta_object obj = { 0 };
    if (!obj.type) {
        obj.type = OBJ_TEXTURE;
        obj.name = INTERN("ta_texture");
        obj_field_add(&obj, FIELD_STRING, INTERN("name"), OFFSETOF(ta_texture, name));
        obj_field_add(&obj, FIELD_STRING, INTERN("path"), OFFSETOF(ta_texture, path));
    }
    texture->object = &obj;
}

void transform_init(ta_transform *transform)
{
    static ta_object obj = { 0 };
    if (!obj.type) {
        obj.type = OBJ_TRANSFORM;
        obj.name = INTERN("ta_transform");
        obj_field_add(&obj, FIELD_OBJECT, INTERN("position"), OFFSETOF(ta_transform, position));
        obj_field_add(&obj, FIELD_OBJECT, INTERN("rotation"), OFFSETOF(ta_transform, rotation));
        obj_field_add(&obj, FIELD_OBJECT, INTERN("scale"),    OFFSETOF(ta_transform, scale));
    }
    transform->object = &obj;
}

void entity_init(ta_entity *entity)
{
    static ta_object obj = { 0 };
    if (!obj.type) {
        obj.type = OBJ_ENTITY;
        obj.name = INTERN("ta_entity");
        obj_field_add(&obj, FIELD_STRING, INTERN("name"),      OFFSETOF(ta_entity, name));
        obj_field_add(&obj, FIELD_OBJECT, INTERN("material"),  OFFSETOF(ta_entity, material));
        obj_field_add(&obj, FIELD_OBJECT, INTERN("mesh"),      OFFSETOF(ta_entity, mesh));
        obj_field_add(&obj, FIELD_OBJECT, INTERN("shader"),    OFFSETOF(ta_entity, shader));
        obj_field_add(&obj, FIELD_OBJECT, INTERN("texture"),   OFFSETOF(ta_entity, texture));
        obj_field_add(&obj, FIELD_OBJECT, INTERN("transform"), OFFSETOF(ta_entity, transform));
    }
    entity->object = &obj;
}
#endif

void entity_free(ta_entity *entity)
{
    // TODO: Free mesh, texture, etc. via reference counting
}

void entity_print(FILE *f, ta_entity *e)
{
    fprintf(f, "Entity: %d\n", e->uid);
}

void entity_save(file *f, ta_entity *e)
{
    entity_print(f->hnd, e);
}

#if 0
void entity_load(file *f, scene *scn, ta_entity_type type, unsigned int uid)
{
    ta_entity *e = scene_entity_init(scn, type, uid);

    /*token_type token = 0;
    do {
    token = token_infer(f);
    printf("%s\n", token_type_str(token));
    } while(token != TOKEN_EOF);*/
}
#endif