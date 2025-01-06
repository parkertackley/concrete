#include "outputfile.h"

void editorScroll() {
    if(E.cy < E.rowoff) {
        E.rowoff = E.cy;
    }
    if(E.cy >= E.rowoff + E.screenrows) {
        E.rowoff = E.cy - E.screenrows + 1;
    }
    if(E.cx < E.coloff) {
        E.coloff = E.cx;
    }
    if(E.cx >= E.coloff + E.screencols) {
        E.coloff = E.cx - E.screencols + 1;
    }
}

void editorDrawsRows(struct abuf *ab) {
    for(int y = 0; y < E.screenrows; ++y) {
        int filerow = y + E.rowoff;
        if(filerow >= E.numrows) {
            if(E.numrows == 0 && y == E.screenrows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "Concrete Editor -- version %s", CONCRETE_VERSION);
                if(welcomelen > E.screencols)
                    welcomelen = E.screencols;

                int padding = (E.screencols - welcomelen) / 2;
                if(padding) {
                    abAppend(ab, "~", 1);
                    --padding;
                }

                while(--padding) {
                    abAppend(ab, " ", 1);
                }
                abAppend(ab, welcome, welcomelen);
            } else {
                abAppend(ab, "~", 1);
            }
        } else {
            int len = E.row[filerow].size - E.coloff;
            if(len < 0)
                len = 0;
            if(len > E.screencols) {
                len = E.screencols;
            }
            abAppend(ab, &E.row[filerow].chars[E.coloff], len);
        }
        

        abAppend(ab, "\x1b[K", 3);      // clears each line for better optimization
        if(y < E.screenrows - 1)
            abAppend(ab, "\r\n", 2);
    }
}

void editorRefreshScreen() {
    editorScroll();

    struct abuf ab = ABUF_INIT;

    abAppend(&ab, "\x1b[?25l", 6);  // hides the cursor
    abAppend(&ab, "\x1b[H", 3);     // cursor to top right    

    editorDrawsRows(&ab);

    char buf[32];                                                                                   // 
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.cx - E.coloff) + 1);        // these lines move the cursor to the position stored
    abAppend(&ab, buf, strlen(buf));                                                                // in cx, cy but as 1-indexed like the terminal uses

    abAppend(&ab, "\x1b[?25h", 6);

    write(STDIN_FILENO, ab.b, ab.len);
    abFree(&ab);
}
