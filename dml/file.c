#include "file.h"
#include "dlb_memory.h"

file *file_open(const char *filename, file_mode mode) {
    FILE *hnd = fopen(filename, file_mode_str(mode));
    if (!hnd) {
        perror("fopen error");
        fprintf(stderr, "Failed to open file: %s\n", filename);
        exit(1);
    }
    file *file = dlb_calloc(1, sizeof(*file));
    file->filename = filename;
    file->mode = mode;
    file->hnd = hnd;
    file->line_number = 1;
    file->line_column = 1;
    return file;
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