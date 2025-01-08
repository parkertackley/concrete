#include "find.h"

void editorFindCallback(char *query, int key) {
    if(key == '\r' || key == '\x1b') {
        return;
    }

    for(int i = 0; i < E.numrows; i++) {
        erow *row = &E.row[i];
        char *match = strstr(row->render, query);
        if(match) {
            E.cy = i;
            E.cx = editorRowRxToCx(row, match - row->render);
            E.rowoff = E.numrows;
            break;
        }
    }
}

void editorFind() {
    char *query = editorPrompt("Search: %s (ESC to cancel)", editorFindCallback);
    if(query)
        free(query);
}
