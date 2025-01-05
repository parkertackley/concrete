#include "terminal.h"

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
                        case '1':
                            return HOME_KEY;
                        case '2':
                            return END_KEY;
                        case '3':
                            return DEL_KEY;
                        case '5':
                            return PAGE_UP;
                        case '6':
                            return PAGE_DOWN;
                        case '7':
                            return HOME_KEY;
                        case '8':
                            return END_KEY;
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
                    case 'H':
                        return ARROW_RIGHT;
                    case 'F':
                        return ARROW_LEFT;
            }
        }
    } else if(seq[0] == 'O') {
        switch (seq[1]) {
            case 'H':
                return HOME_KEY;
            case 'F':
            return END_KEY;
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
