#ifndef DEFINESTRUCT_H
#define DEFINESTRUCT_H

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <termios.h>
#include <stdlib.h>

#define CONCRETE_VERSION "0.0.1"
#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
    ARROW_LEFT = 1000,  // rest of values get incrementing values
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

typedef struct erow {
    int size;
    char *chars;
} erow;

struct editorConfig {   // saves the original terminal flags
    int cx, cy;         // cursors coords
    int rowoff;
    int coloff;
    int screenrows, screencols;
    int numrows;
    erow *row;
    struct termios orig_termios;
};

extern struct editorConfig E;

#endif
