#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

/* Defines */
#define CONCRETE_VERSION "0.0.1"
#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
    ARROW_LEFT = 1000,  // rest of values get incrementing values
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    PAGE_DOWN
};

/* Data */
struct editorConfig {   // saves the original terminal flags
    int cx, cy;         // cursors coords
    int screenrows, screencols;
    struct termios orig_termios;
};

struct editorConfig E;

/* terminal */
void die(const char *s) {   // error handling
    write(STDOUT_FILENO, "\x1b[2J", 4);     // clears screen
    write(STDOUT_FILENO, "\x1b[H", 3);      // cursor to top right

    perror(s);
    exit(1);
}

void disbaleRawMode() {     // sets terminal back to normal
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) // set terminal back to normal settings
        die("tcseattr");
}

void enableRawMode() {      // sets terminal up for concrete controls
    if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)    // get attributes of terminal and store in orig
        die("tcsetattr");  
    atexit(disbaleRawMode);

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON); // misc, icrnl disables ctrl-m/carriage return, ixon diables ctrl-s/ctrl-q
    raw.c_oflag &= ~(OPOST);    // 
    raw.c_cflag &= ~(CS8);      // misc
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // echo mode shows whats typing, icanon reads every byte inputted rather than each line, isig disables ctr-c/ctrl-z, iexten disables ctrl-v and ctrl-c
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)  // set new terminal attributes
        die("tcsetattr");   

}

int editorReadKey() {
    int nread;
    char c;
    while((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if(nread == -1 && errno != EAGAIN)
            die("read");
    } 

    if(c == '\x1b') {
        char seq[3];

        if(read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\x1b';
        if(read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\x1b';
        
        if(seq[0] == '[') {
            if(seq[1] >= '0' && seq[1] <= '9') {
                if(read(STDIN_FILENO, &seq[2], 1)!= 1)
                    return '\x1b';
                if(seq[2] == '~') {
                    switch(seq[1]) {
                        case '5':
                            return PAGE_UP;
                        case '6':
                            return PAGE_DOWN;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A':
                        return ARROW_UP;
                    case 'B':
                        return ARROW_DOWN;
                    case 'C':
                        return ARROW_RIGHT;
                    case 'D':
                        return ARROW_LEFT;
            }
        }
    }
        return '\x1b';
    } else {
        return c;
    }
}

int getCursorPosition(int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;

    while(i < sizeof(buf) - 1) {
        if(read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if(buf[i] == 'R')
            break;
        ++i;
    }

    buf[i] = '\0';

    if(buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if(sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)    // send cursor to bottom right of screen to get dimensions
            return -1;
        return getCursorPosition(rows, cols);
    } else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
        return 0;
    }
}

/* terminal */

/* append buffer */
struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b, ab->len + len);

    if(new == NULL)
        return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab) {
    free(ab->b);
}

/* output */
void editorDrawsRows(struct abuf *ab) {
    for(int y = 0; y < E.screenrows; ++y) {
        if(y == E.screenrows / 3) {
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

        abAppend(ab, "\x1b[K", 3);      // clears each line for better optimization
        if(y < E.screenrows - 1)
            abAppend(ab, "\r\n", 2);
    }
}

void editorRefreshScreen() {
    struct abuf ab = ABUF_INIT;

    abAppend(&ab, "\x1b[?25l", 6);  // hides the cursor
    abAppend(&ab, "\x1b[H", 3);     // cursor to top right    

    editorDrawsRows(&ab);

    char buf[32];                                                       // 
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);      // these lines move the cursor to the position stored
    abAppend(&ab, buf, strlen(buf));                                    // in cx, cy but as 1-indexed like the terminal uses

    abAppend(&ab, "\x1b[?25h", 6);

    write(STDIN_FILENO, ab.b, ab.len);
    abFree(&ab);
}

/* input */
void editorMoveCursor(int key) {
    switch (key) {
        case ARROW_LEFT:
            if(E.cx != 0) {
                E.cx--;
            }
            break;
        case ARROW_RIGHT:
            if(E.cx != E.screencols - 1) {
                E.cx++;
            }
            break;
        case ARROW_DOWN:
            if(E.cy != E.screenrows - 1) {
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

void editorProcessKeypress() {  // 
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

/* init */
void initEditor() {
    E.cx = 0;
    E.cy = 0;

    if(getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("getWindowSize");
}

int main() {
    enableRawMode();
    initEditor();

    while(1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    
    return 0;
}
