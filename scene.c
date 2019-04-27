#include "scene.h"
#include "parse.h"
#include "symbol.h"
#include "object.h"
#include "dlb_types.h"
#include "dlb_memory.h"
#include "dlb_vector.h"
#include "dlb_hash.h"

const char *token_type_str(token_type type)
{
    switch(type) {
        case TOKEN_UNKNOWN:        return "????????";
        case TOKEN_EOF:            return "EOF";
        case TOKEN_WHITESPACE:     return "WHITESPACE";
        case TOKEN_NEWLINE:        return "NEWLINE";
        case TOKEN_INDENT:         return "INDENT";
        case TOKEN_COMMENT:        return "COMMENT";
        case TOKEN_IDENTIFIER:     return "IDENTIFIER";
        case TOKEN_KW_NULL:        return "KEYWORD";
        case TOKEN_KW_TRUE:        return "KEYWORD";
        case TOKEN_KW_FALSE:       return "KEYWORD";
        case TOKEN_INT:            return "INT";
        case TOKEN_FLOAT:          return "FLOAT";
        case TOKEN_STRING:         return "STRING";
        case TOKEN_ARRAY_START:    return "ARRAY_START";
        case TOKEN_ARRAY_END:      return "ARRAY_END";
        case TOKEN_OBJECT_START:   return "OBJECT_START";
        case TOKEN_OBJECT_END:     return "OBJECT_END";
        case TOKEN_LIST_SEPARATOR: return "LIST_SEPARATOR";
        default: DLB_ASSERT(!"Unknown token type");  return 0;
    }
};

ta_scene *scene_init(const char *name)
{
    ta_scene *scene = dlb_calloc(1, sizeof(*scene));
    scene->name = name;
    dlb_hash_init(&scene->refs_by_name, DLB_HASH_STRING, name, 32);
    //dlb_vec_reserve(scn->entities, 2);
    return scene;
}

void scene_free(ta_scene *scene)
{
    // TODO: Call free on each object individually
    //for (ta_entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
    //    entity_free(e);
    //}
    dlb_hash_free(&scene->refs_by_name);
    dlb_vec_free(scene->entities);
    dlb_vec_free(scene->materials);
    dlb_vec_free(scene->meshes);
    dlb_vec_free(scene->shaders);
    dlb_vec_free(scene->textures);
    free(scene);
}

static token *token_read(file *f, token **tokens)
{
    token *token = dlb_vec_alloc(*tokens);
    char c = file_peek(f);
    switch(c) {
        case EOF:
        {
            token->type = TOKEN_EOF;
            break;
        }
        case ' ':
        {
            file_expect_char(f, C_WHITESPACE, 1);
            token_type prev_token_type = TOKEN_UNKNOWN;
            int tokens_len = dlb_vec_len(*tokens);
            if (tokens_len > 1) {
                prev_token_type = (*tokens)[tokens_len - 2].type;
            }
            if (prev_token_type == TOKEN_NEWLINE ||
                prev_token_type == TOKEN_INDENT)
            {
                file_expect_char(f, C_WHITESPACE, 1);
                token->type = TOKEN_INDENT;
            } else {
                token->type = TOKEN_WHITESPACE;
            }
            break;
        }
        case '\n':
        {
            token->type = TOKEN_NEWLINE;
            file_expect_char(f, C_NEWLINE, 1);
            break;
        }
        case '#':
        {
            token->type = TOKEN_COMMENT;
            file_expect_char(f, C_COMMENT_START, 1);
            char buf[MAX_COMMENT_LEN + 1] = { 0 };
            int len = 0;
            file_read(f, buf, MAX_COMMENT_LEN, C_COMMENT, C_COMMENT_END, &len);
            token->length = len;
            token->value.string = intern(buf, len);
            break;
        }
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
        {
            DLB_ASSERT(str_contains_chr(C_IDENT_START, file_peek(f)));
            char buf[MAX_IDENT_LEN + 1] = { 0 };
            int len = 0;
            file_read(f, buf, MAX_IDENT_LEN, C_IDENT, 0, &len);
            token->length = len;
            token->value.string = intern(buf, len);
            if (file_allow_char(f, C_IDENT_END, 1)) {
                token->type = TOKEN_IDENTIFIER;
            } else if (token->value.string == sym_kw_null) {
                token->type = TOKEN_KW_NULL;
            } else if (token->value.string == sym_kw_true) {
                token->type = TOKEN_KW_TRUE;
            } else if (token->value.string == sym_kw_false) {
                token->type = TOKEN_KW_FALSE;
            } else {
                PANIC_FILE(f, "Expected : after identifier '%s'\n", token->value.string);
            }
            break;
        }
        case '+': case '-': case '0': case '1': case '2':case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            DLB_ASSERT(str_contains_chr(C_NUMBER_START, file_peek(f)));
            char buf[MAX_NUMBER_LEN + 1] = { 0 };
            int len = 0;
            int read = 0;
            char next = file_peek(f);
            if (next == '0') {
                buf[len++] = file_char(f);
                next = file_peek(f);
                if (next == 'x') {
                    token->type = TOKEN_FLOAT;
                    buf[len++] = file_char(f);
                    next = file_read(f, buf + len, 8, C_NUMBER_HEX, 0, &read);
                    len += read;
                    file_allow_char(f, C_WHITESPACE, 0);
                    if (file_allow_char(f, "(", 1)) {
                        file_read(f, 0, 0, 0, ")", 0);
                        file_expect_char(f, ")", 1);
                    }
                } else if (next == 'b') {
                    token->type = TOKEN_INT;
                    buf[len++] = file_char(f);
                    file_read(f, buf + len, 32, C_NUMBER_BINARY, 0, &read);
                    len += read;
                }
            }
            if (token->type == TOKEN_UNKNOWN) {
                file_read(f, buf, 1, C_NUMBER_SIGN, 0, &read);
                len += read;
                file_read(f, buf + len, MAX_NUMBER_LEN - len, C_NUMBER_INT, 0, &read);
                len += read;
                next = file_peek(f);
                if (next == '.') {
                    token->type = TOKEN_FLOAT;
                    int read = 0;
                    file_read(f, buf + len, MAX_NUMBER_LEN - len,
                        C_NUMBER_FLOAT, 0, &read);
                    len += read;
                } else {
                    token->type = TOKEN_INT;
                }
            }
            switch (token->type) {
                case TOKEN_INT: {
                    token->value.as_int = parse_int(buf);
                    break;
                } case TOKEN_FLOAT: {
                    token->value.as_float = parse_float(buf);
                    break;
                } default: {
                    DLB_ASSERT(!"Token type could not be resolved");
                }
            }
            break;
        }
        case '"':
        {
            token->type = TOKEN_STRING;
            file_expect_char(f, "\"", 1);
            char buf[MAX_STRING_LEN + 1] = { 0 };
            int len = 0;
            char delim = 0;
            int read = 0;
            do {
                delim = file_read(f, buf + len, MAX_STRING_LEN - len, C_STRING,
                    "\\\"", &read);
                len += read;
                if (delim == '\\') {
                    buf[len++] = file_char_escaped(f);
                }
            } while (delim == '\\');
            file_expect_char(f, "\"", 1);
            token->length = len;
            token->value.string = len ? intern(buf, len) : 0;
            break;
        }
        case '[':
        {
            token->type = TOKEN_ARRAY_START;
            file_expect_char(f, C_ARRAY_START, 1);
            break;
        }
        case ']':
        {
            token->type = TOKEN_ARRAY_END;
            file_expect_char(f, C_ARRAY_END, 1);
            break;
        }
        case '{':
        {
            token->type = TOKEN_OBJECT_START;
            file_expect_char(f, C_OBJECT_START, 1);
            break;
        }
        case '}':
        {
            token->type = TOKEN_OBJECT_END;
            file_expect_char(f, C_OBJECT_END, 1);
            break;
        }
        case ',':
        {
            token->type = TOKEN_LIST_SEPARATOR;
            file_expect_char(f, C_LIST_SEPARATOR, 1);
            break;
        }
        default:
        {
            PANIC_FILE(f, "I don't know what's going on.. weird tokens bro.\n");
        }
    }
    return token;
}

static token *scene_tokenize(file *f)
{
    token *tokens = 0;
    while (token_read(f, &tokens)->type != TOKEN_EOF) {}
    return tokens;
}

static void tokens_print(token *tokens)
{
    for (token *tok = tokens; tok != dlb_vec_end(tokens); tok++) {
        switch (tok->type) {
            case TOKEN_EOF: {
                break;
            } case TOKEN_WHITESPACE: {
                printf(" ");
                break;
            } case TOKEN_NEWLINE: {
                printf("\n");
                break;
            } case TOKEN_INDENT: {
                printf("  ");
                break;
            }
            case TOKEN_COMMENT: case TOKEN_IDENTIFIER: case TOKEN_STRING:
            case TOKEN_KW_NULL: case TOKEN_KW_TRUE: case TOKEN_KW_FALSE:
            {
                if (tok->type == TOKEN_COMMENT) {
                    printf("#");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }

                if (tok->length && tok->value.string) {
                    printf("%.*s", tok->length, tok->value.string);
                }

                if (tok->type == TOKEN_IDENTIFIER) {
                    printf(":");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }
                break;
            } case TOKEN_INT: {
                printf("%d", tok->value.as_int);
                break;
            } case TOKEN_FLOAT: {
                printf("%f", tok->value.as_float);
                break;
            } case TOKEN_ARRAY_START: {
                printf("[");
                break;
            } case TOKEN_ARRAY_END: {
                printf("]");
                break;
            } case TOKEN_OBJECT_START: {
                printf("{");
                break;
            } case TOKEN_OBJECT_END: {
                printf("}");
                break;
            } case TOKEN_LIST_SEPARATOR: {
                printf(",");
                break;
            } default: {
                DLB_ASSERT(!"Unexpected token type, don't know how to print");
            }
        }
    }
    printf("\n");
}

static void tokens_print_debug(token *tokens)
{
    for (token *tok = tokens; tok != dlb_vec_end(tokens); tok++) {
        printf("%-16s", token_type_str(tok->type));
        switch (tok->type) {
            case TOKEN_EOF:
            {
                break;
            }
            case TOKEN_WHITESPACE:
            case TOKEN_NEWLINE:
            case TOKEN_INDENT:
            {
                break;
            }
            case TOKEN_COMMENT: case TOKEN_IDENTIFIER: case TOKEN_STRING:
            case TOKEN_KW_NULL: case TOKEN_KW_TRUE: case TOKEN_KW_FALSE:
            {
                if (tok->type == TOKEN_COMMENT) {
                    printf("#");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }

                if (tok->length && tok->value.string) {
                    printf("%.*s", tok->length, tok->value.string);
                }

                if (tok->type == TOKEN_IDENTIFIER) {
                    printf(":");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }
                break;
            } case TOKEN_INT: {
                printf("%d", tok->value.as_int);
                break;
            } case TOKEN_FLOAT: {
                printf("%f", tok->value.as_float);
                break;
            }
            case TOKEN_ARRAY_START: case TOKEN_ARRAY_END:
            case TOKEN_OBJECT_START: case TOKEN_OBJECT_END:
            case TOKEN_LIST_SEPARATOR:
            {
                break;
            } default: {
                DLB_ASSERT(!"Unexpected token type, don't know how to print");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void *scene_obj_init(ta_scene *scene, ta_field_type type)
{
    void *obj = 0;
    switch (type) {
        case F_TA_SUN_LIGHT: {
            ta_sun_light *light = dlb_vec_alloc(scene->sun_lights);
            light->scene = scene;
            obj = light;
            break;
        } case F_TA_POINT_LIGHT: {
            ta_point_light *light = dlb_vec_alloc(scene->point_lights);
            light->scene = scene;
            obj = light;
            break;
        } case F_TA_MATERIAL: {
            ta_material *material = dlb_vec_alloc(scene->materials);
            material->scene = scene;
            obj = material;
            break;
        } case F_TA_MESH: {
            ta_mesh *mesh = dlb_vec_alloc(scene->meshes);
            mesh->scene = scene;
            obj = mesh;
            break;
        } case F_TA_SHADER: {
            ta_shader *shader = dlb_vec_alloc(scene->shaders);
            shader->scene = scene;
            obj = shader;
            break;
        } case F_TA_TEXTURE: {
            ta_texture *texture = dlb_vec_alloc(scene->textures);
            texture->scene = scene;
            obj = texture;
            break;
        } case F_TA_ENTITY: {
            ta_entity *entity = dlb_vec_alloc(scene->entities);
            entity->scene = scene;
            obj = entity;
            break;
        } default: {
            DLB_ASSERT(!"Cannot initialize this type as a standalone object");
        }
    }
    return obj;
}

void scene_parse(ta_scene *scene, token *tokens)
{
    struct {
        ta_field_type type;
        const char *name;
        void *ptr;
        int indent;
    } stack[16] = { 0 };

    int indent = 0;  // Current line indent counter
    int level = 0;   // Current level of indentation
    int braces = 0;  // Current level of curly braces
    int sp = 0;      // "Stack pointer" index into stack

    for (token *tok = tokens; tok != dlb_vec_end(tokens); tok++) {
        switch (tok->type) {
            case TOKEN_EOF: {
                break;
            } case TOKEN_WHITESPACE: {
                break;
            } case TOKEN_NEWLINE: {
                indent = 0;
                break;
            } case TOKEN_INDENT: {
                indent++;
                break;
            } case TOKEN_COMMENT: {
                break;
            } case TOKEN_IDENTIFIER: {
                if (braces) {
                    DLB_ASSERT(level);
                    stack[sp].indent = 0;
                } else {
                    for (int i = level; i >= 0; i--) {
                        if (indent >= stack[i].indent) {
                            break;
                        }
                        DLB_ASSERT(level);
                        level--;
                        sp--;
                    }
                    stack[sp].indent = indent;
                }

                if (level) {
                    ta_schema_field *field = obj_field_find( stack[sp-1].type,
                        tok->value.string);
                    if (!field) {
                        PANIC("Unexpected field '%s' on %s '%s'\n",
                            tok->value.string, ta_field_type_str(stack[sp-1].type),
                            stack[sp-1].name);
                    }
                    DLB_ASSERT(field->type);
                    stack[sp].type = field->type;
                    stack[sp].name = field->name;
                    stack[sp].ptr = ((u8 *)stack[sp-1].ptr + field->offset);
                } else {
                    ta_schema *obj = dlb_hash_search(&tg_schemas_by_name,
                        tok->value.string, tok->length);
                    if (!obj) {
                        PANIC("Unexpected type name '%s'\n", tok->value.string);
                    }
                    DLB_ASSERT(obj->type);
                    DLB_ASSERT(obj->name == tok->value.string);
                    stack[sp].type = obj->type;
                    stack[sp].name = tok->value.string;
                    stack[sp].ptr = scene_obj_init(scene, obj->type);
                }
                if (!braces && stack[sp].type > F_ATOM_END) {
                    level++;
                    sp++;
                }
                break;
            } case TOKEN_KW_NULL: {
                DLB_ASSERT(stack[sp].type == F_ATOM_STRING);
                break;
            } case TOKEN_INT: {
                DLB_ASSERT(
                    stack[sp].type == F_ATOM_INT ||
                    stack[sp].type == F_ATOM_UINT
                );
                int *fp = stack[sp].ptr;
                *fp = tok->value.as_int;
                break;
            } case TOKEN_FLOAT: {
                DLB_ASSERT(stack[sp].type == F_ATOM_FLOAT);
                float *fp = stack[sp].ptr;
                *fp = tok->value.as_float;
                break;
            } case TOKEN_STRING: {
                DLB_ASSERT(stack[sp].type == F_ATOM_STRING);
                const char **fp = stack[sp].ptr;
                *fp = tok->value.string;
                if (stack[sp].name == sym_ident_name) {
                    scene_ref *ref = dlb_vec_alloc(scene->refs);
                    ref->type = stack[sp-1].type;
                    ref->ptr = stack[sp-1].ptr;
                    dlb_hash_insert(&scene->refs_by_name, tok->value.string, tok->length, ref);
                }
                break;
            } case TOKEN_OBJECT_START: {
                DLB_ASSERT(stack[sp-1].type > F_ATOM_END);
                level--;
                braces++;
                break;
            } case TOKEN_OBJECT_END: {
                DLB_ASSERT(braces);
                braces--;
                sp--;
                break;
            } case TOKEN_LIST_SEPARATOR: {
                DLB_ASSERT(braces);
                break;
            } default: {
                PANIC("Unexpected token %s\n", token_type_str(tok->type));
            }
        }
    }
}

ta_scene *scene_load(file *f)
{
    ta_scene *scene = scene_init(f->filename);
    token *tokens = scene_tokenize(f);
    //tokens_print(tokens);
    tokens_print_debug(tokens);
    scene_parse(scene, tokens);
    dlb_vec_free(tokens);
    return scene;
}

void scene_print(ta_scene *scene, FILE *hnd)
{
    // TODO: Register scene as a schema that has OBJ_ARRAY of entities
    printf("Scene name: %s\n", scene->name);
    for (ta_sun_light *o = scene->sun_lights; o != dlb_vec_end(scene->sun_lights); o++) {
        obj_print(hnd, F_TA_SUN_LIGHT, (void *)o, 0);
    }
    for (ta_point_light *o = scene->point_lights; o != dlb_vec_end(scene->point_lights); o++) {
        obj_print(hnd, F_TA_POINT_LIGHT, (void *)o, 0);
    }
    for (ta_entity *o = scene->entities; o != dlb_vec_end(scene->entities); o++) {
        obj_print(hnd, F_TA_ENTITY, (void *)o, 0);
    }
    for (ta_material *o = scene->materials; o != dlb_vec_end(scene->materials); o++) {
        obj_print(hnd, F_TA_MATERIAL, (void *)o, 0);
    }
    for (ta_mesh *o = scene->meshes; o != dlb_vec_end(scene->meshes); o++) {
        obj_print(hnd, F_TA_MESH, (void *)o, 0);
    }
    for (ta_shader *o = scene->shaders; o != dlb_vec_end(scene->shaders); o++) {
        obj_print(hnd, F_TA_SHADER, (void *)o, 0);
    }
    for (ta_texture *o = scene->textures; o != dlb_vec_end(scene->textures); o++) {
        obj_print(hnd, F_TA_TEXTURE, (void *)o, 0);
    }
    fflush(hnd);
}