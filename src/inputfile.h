#ifndef INPUTFILE_H
#define INPUTFILE_H

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "terminal.h"
#include "definestruct.h"
#include "editoroperations.h"
#include "fileio.h"

char *editorPrompt(char *prompt);
void editorMoveCursor(int key);
void editorProcessKeypress();

#endif
