#include "inputfile.h"
#include "definestruct.h"

#include <unistd.h>

void editorMoveCursor(int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key) {
        case ARROW_LEFT:
            if(E.cx != 0) {
                E.cx--;
            }
            break;
        case ARROW_RIGHT:
            if(row && E.cx < row->size) {
                E.cx++;
            }
            break;
        case ARROW_DOWN:
            if(E.cy < E.numrows) {
                E.cy++;
            }
            break;
        case ARROW_UP:
            if(E.cy != 0) {
                E.cy--;
            }
            break;
    }
}

void editorProcessKeypress() {
    int c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);     // clears screen
            write(STDOUT_FILENO, "\x1b[H", 3);      // cursor to top right
            exit(0);
            break;

        case PAGE_UP:
        case PAGE_DOWN:
            {
                int times = E.screenrows;
                while(--times)
                    editorMoveCursor(c == PAGE_UP ? ARROW_UP : PAGE_DOWN);
            }
        
        case ARROW_UP:
        case ARROW_LEFT:
        case ARROW_RIGHT:
        case ARROW_DOWN:
            editorMoveCursor(c);
            break;
    }
}
