#ifndef APPENDBUFFER_H
#define APPENDBUFFER_H

#include <stdlib.h>
#include <string.h>

struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);

#endif
