#include "definestruct.h"
#include "terminal.h"
#include "appendbuffer.h"
#include "outputfile.h"
#include "inputfile.h"

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
