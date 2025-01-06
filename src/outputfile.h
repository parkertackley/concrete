#ifndef OUTPUTFILE_H
#define OUTPUTFILE_H

#include "appendbuffer.h"
#include "definestruct.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void editorScroll();
void editorDrawsRows(struct abuf *ab);
void editorRefreshScreen();

#endif
