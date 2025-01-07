#include "rowoperations.h"

int editorRowCxToRx(erow *row, int cx) {
    int rx = 0;
    for(int j = 0; j < cx; j++) {
        if(row->chars[j] == '\t')
            rx += (CONCRETE_TAB_STOP - 1) - (rx % CONCRETE_TAB_STOP);
        rx++;
    }
    return rx;
}

void editorUpdateRow(erow *row) {
    int tabs = 0;
    int j = 0;
    for(j = 0; j < row->size; j++)
        if(row->chars[j] == '\t')
            tabs++;

    free(row->render);
    row->render = malloc(row->size + tabs * (CONCRETE_TAB_STOP - 1) + 1);

    int idx = 0;
    for(j = 0; j < row->size; j++) {
        if(row->chars[j] == '\t') {
            row->render[idx++] = ' ';
            while(idx % CONCRETE_TAB_STOP != 0)
                row->render[idx++] = ' ';
        } else {
            row->render[idx++] = row->chars[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}

void editorAppendRow(char *s, size_t len) {
    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));

    int at = E.numrows;
    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL; 
    editorUpdateRow(&E.row[at]);

    E.numrows++;
    
}
