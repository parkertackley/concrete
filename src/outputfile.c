#include "outputfile.h"

void editorScroll() {
    E.rx = 0;
    if(E.cy < E.numrows) {
        E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);
    }

    if(E.cy < E.rowoff) {
        E.rowoff = E.cy;
    }
    if(E.cy >= E.rowoff + E.screenrows) {
        E.rowoff = E.cy - E.screenrows + 1;
    }
    if(E.rx < E.coloff) {
        E.coloff = E.rx;
    }
    if(E.rx >= E.coloff + E.screencols) {
        E.coloff = E.rx - E.screencols + 1;
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
            int len = E.row[filerow].rsize - E.coloff;
            if(len < 0)
                len = 0;
            if(len > E.screencols)
                len = E.screencols;
                char *c = &E.row[filerow].render[E.coloff];
                unsigned char *hl = &E.row[filerow].hl[E.coloff];
                int current_color = -1;
                for(int j = 0; j < len; j++) {
                    if(hl[j] == HL_NORMAL) {
                        if(current_color != -1) {
                            abAppend(ab, "\x1b[39m", 5);
                            current_color = -1;
                        }
                        abAppend(ab, &c[j], 1);
                    } else {
                        int color = editorSyntaxToColor(hl[j]);
                        if(color != current_color) {
                            current_color = color;
                            char buf[16];
                            int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
                            abAppend(ab, buf, clen);
                        }
                        abAppend(ab, &c[j], 1);
                    }
                }
            abAppend(ab, "\x1b[39m", 5);
        }
        

        abAppend(ab, "\x1b[K", 3);      // clears each line for better optimization
        abAppend(ab, "\r\n", 2);
    }
}

void editorDrawStatusBar(struct abuf *ab) {
    abAppend(ab, "\x1b[7m", 4);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.50s - %d lines %s", E.filename ? E.filename : "[No Name]", E.numrows, E.dirty ? "(modified)" : "");
        
    int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d", E.syntax ? E.syntax->filetype : "no ft", E.cy + 1, E.numrows + 1);

    if(len > E.screencols)
        len = E.screencols;
    abAppend(ab, status, len);
    while(len < E.screencols) {
        if(E.screencols - len == rlen) {
            abAppend(ab, rstatus, rlen);
            break;
        } else {
            abAppend(ab, " ", 1);
            len++;
        }
    }
    abAppend(ab, "\x1b[m", 3);
    abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
    abAppend(ab, "\x1b[K", 3);
    int msglen = strlen(E.statusmsg);
    if(msglen > E.screencols)
        msglen = E.screencols;
    
    if(msglen && time(NULL) - E.statusmsg_time < 5)
        abAppend(ab, E.statusmsg, msglen);
}

void editorRefreshScreen() {
    editorScroll();

    struct abuf ab = ABUF_INIT;

    abAppend(&ab, "\x1b[?25l", 6);  // hides the cursor
    abAppend(&ab, "\x1b[H", 3);     // cursor to top right    

    editorDrawsRows(&ab);
    editorDrawStatusBar(&ab);
    editorDrawMessageBar(&ab);

    char buf[32];                                                                                   // 
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.rx - E.coloff) + 1);        // these lines move the cursor to the position stored
    abAppend(&ab, buf, strlen(buf));                                                                // in cx, cy but as 1-indexed like the terminal uses

    abAppend(&ab, "\x1b[?25h", 6);

    write(STDIN_FILENO, ab.b, ab.len);
    abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
    va_end(ap);
    E.statusmsg_time = time(NULL);
}
