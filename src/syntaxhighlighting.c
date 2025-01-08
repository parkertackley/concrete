#include "syntaxhighlighting.h"

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

int is_separator(int c) {
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

void editorUpdateSyntax(erow *row) {
    row->hl = realloc(row->hl, row->size);
    memset(row->hl, HL_NORMAL, row->size);

    if(E.syntax == NULL)
        return;

    int prev_sep = 1;

    int i = 0;
    while(i < row->rsize) {
        char c = row->render[i];
        unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

        if(E.syntax->flags & HL_HIGHLIGHT_NUMBERS) {
            if((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)) {
                row->hl[i] = HL_NUMBER;
                i++;
                prev_sep = 0;
                continue;
            }
        }
            

        prev_sep = is_separator(c);
        i++;
    }
}

int editorSyntaxToColor(int hl) {
    switch(hl){
        case HL_NUMBER:
            return 31;
        case HL_MATCH:
            return 34;
        default:
            return 37;
    }
}

void editorSelectSyntaxHighlight() {
    E.syntax = NULL;
    if(E.filename == NULL)
        return;

    char *ext = strrchr(E.filename, '.');

    for(unsigned int j = 0; j < getHLDBEntries(); j++) {
        struct editorSyntax *s = &HLDB[j];
        unsigned int i = 0;
        while(s->filematch[i]) {
            int is_ext = (s->filematch[i][0] == '.');
            if((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(E.filename, s->filematch[i]))) {
                E.syntax = s;
                return;
            }
            i++;
        }
    }
}
