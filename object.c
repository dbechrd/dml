#include "object.h"
#include "symbol.h"
#include "file.h"
#include "dlb_types.h"
#include "dlb_vector.h"

const char *ta_object_type_str(ta_object_type type) {
    switch(type) {
        case OBJ_ENTITY:   return "OBJ_ENTITY";
        case OBJ_TEXTURE:  return "OBJ_TEXTURE";
        case OBJ_MATERIAL: return "OBJ_MATERIAL";
        case OBJ_MESH:     return "OBJ_MESH";
        default: DLB_ASSERT(!"Unknown object type"); return 0;
    }
};

void obj_field_add(ta_object *obj, ta_object_field_type type, const char *name,
    u32 offset)
{
    ta_object_field *fo = dlb_vec_alloc(obj->fields);
    fo->type = type;
    fo->name = name;
    fo->offset = offset;
}

ta_object_field *obj_field_find(const ta_object *obj, const char *name)
{
    ta_object_field *field = 0;
    for (ta_object_field *f = obj->fields; f != dlb_vec_end(obj->fields); f++) {
        if (f->name == name) {
            field = f;
            break;
        }
    }
    return field;
}

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