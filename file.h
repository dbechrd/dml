#pragma once
#include "helpers.h"
#include "dlb_types.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum file_mode {
    FILE_READ,
    FILE_WRITE,
} file_mode;

static inline const char *file_mode_str(file_mode mode) {
	const char *str = "";
    switch(mode) {
    case FILE_READ: str = "rb"; break;
    case FILE_WRITE: str = "wb"; break;
    default: DLB_ASSERT(0);
    }
    return str;
}

typedef struct file_pos {
    size_t line;
    size_t column;
} file_pos;

typedef struct file {
    const char *filename;
    file_mode mode;
    FILE *hnd;
    char last;
    bool replay;
    file_pos pos;
} file;

file *file_open(const char *filename, file_mode mode);
int file_look_char(file *f, const char *chars, int times, bool required);
void file_expect_string(file *f, const char *str);

static inline char file_char(file *f) {
    if (f->replay) {
        f->replay = false;
        return f->last;
    }
    if (f->last == '\n') {
        f->pos.line++;
        f->pos.column = 0;
    }
    f->pos.column++;
    f->last = fgetc(f->hnd);
    return f->last;
}

static inline char file_peek(file *f) {
    char c = file_char(f);
    f->replay = true;
    return c;
}

static inline void file_close(file *f) {
    fclose(f->hnd);
    free(f);
}

static inline int file_expect_char(file *f, const char *chars, int times) {
    int count = file_look_char(f, chars, times, true);
    return count;
}

static inline int file_allow_char(file *f, const char *chars, int times) {
    int count = file_look_char(f, chars, times, false);
    //if (!count) {
    //    f->replay = true;  // HACK: Seemed simpler than fseek...
    //}
    return count;
}