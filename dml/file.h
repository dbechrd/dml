#pragma once
#include "dlb_types.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum file_mode {
    FILE_READ,
    FILE_WRITE,
} file_mode;

static inline const char *file_mode_str(file_mode mode) {
    const char *str;
    switch(mode) {
    case FILE_READ: str = "rb"; break;
    case FILE_WRITE: str = "wb"; break;
    default: DLB_ASSERT(0);
    }
    return str;
}

typedef struct file {
    const char *filename;
    file_mode mode;
    FILE *hnd;
    char last;
} file;

file *file_open(const char *filename, file_mode mode);

static inline char file_getc(file *f) {
    f->last = fgetc(f->hnd);
    return f->last;
}

static inline void file_expect(file *f, char chr) {
    char c = file_getc(f);
    DLB_ASSERT(c == chr);
}

static inline void file_close(file *f) {
    fclose(f->hnd);
    free(f);
}