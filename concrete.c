#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* Data */
struct termios orig_termios;    // saves the original terminal flags

/* Functions */
void die(const char *s) {
    perror(s);
    exit(1);
}

void disbaleRawMode() {
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) // set terminal back to normal settings
        die("tcseattr");
}

void enableRawMode() {
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1)    // get attributes of terminal and store in orig
        die("tcsetattr");  
    atexit(disbaleRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON); // misc, icrnl disables ctrl-m/carriage return, ixon diables ctrl-s/ctrl-q
    raw.c_oflag &= ~(OPOST);    // 
    raw.c_cflag &= ~(CS8);      // misc
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // echo mode shows whats typing, icanon reads every byte inputted rather than each line, isig disables ctr-c/ctrl-z, iexten disables ctrl-v and ctrl-c
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)  // set new terminal attributes
        die("tcsetattr");   

}

/* Main function */
int main() {
    enableRawMode();

    while(1) {     // reads line of input in the terminal
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");
        if(iscntrl(c)) {    
            printf("%d\r\n", c);  // if control char, print byte
        } else {
            printf("%d (%c)\r\n", c, c);    // print the byte along with char
        }
        if(c == 'q')
            break;
    }  
    return 0;
}
