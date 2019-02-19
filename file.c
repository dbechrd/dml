#include "file.h"
#include "dlb_memory.h"

file *file_open(const char *filename, file_mode mode) {
    FILE *hnd = fopen(filename, file_mode_str(mode));
    if (!hnd) {
        perror("fopen error");
        PANIC("Failed to open file: %s\n", filename);
    }
    file *file = dlb_calloc(1, sizeof(*file));
    file->filename = filename;
    file->mode = mode;
    file->hnd = hnd;
    file->pos.line = 1;
    file->pos.column = 1;
    return file;
}

int file_look_char(file *f, const char *chars, int times, bool required) {
    DLB_ASSERT(times || !required);
    int count = 0;
    for (int i = 0; times == 0 || i < times; i++) {
        char c = file_char(f);
        char found = str_find_char(chars, c);
        if (found) {
            count++;
        } else {
            if (required) {
                PANIC_FILE(f, "[PARSE_ERROR] Missing expected character [%s]. Found '%c' instead.\n", chars, c);
            }
            f->replay = true;
            break;
        }
    }
    return count;
}

void file_expect_string(file *f, const char *str) {
    const char *s = str;
    while (*s) {
        char c = file_char(f);
        if (c != *s) {
            PANIC_FILE(f, "[PARSE_ERROR] Missing expected character %c of string %s. Found %c instead.\n", *s, str, c);
        }
        s++;
    }
}

//char *file_read_all(file *f) {
//    DLB_ASSERT(!fseek(f->hnd, 0, SEEK_END));
//    long pos = ftell(f->hnd);
//    DLB_ASSERT(pos <= SIZE_MAX);
//    DLB_ASSERT(!fseek(f->hnd, 0, SEEK_SET));
//
//    size_t end = (size_t)pos;
//    char *buf = dlb_malloc(end + 1);
//    fread(buf, end, 1, f->hnd);
//    buf[end] = 0;
//    return buf;
//}