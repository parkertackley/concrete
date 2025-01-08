#ifndef OUTPUTFILE_H
#define OUTPUTFILE_H

#include "appendbuffer.h"
#include "definestruct.h"
#include "rowoperations.h"
#include "syntaxhighlighting.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

void editorScroll();
void editorDrawsRows(struct abuf *ab);
void editorDrawStatusBar(struct abuf *ab);
void editorDrawMessageBar(struct abuf *ab);
void editorRefreshScreen();
void editorSetStatusMessage(const char *fmt, ...);

#endif
