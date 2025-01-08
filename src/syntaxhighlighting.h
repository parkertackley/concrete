#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#include <ctype.h>
#include <string.h>

#include "definestruct.h"

void editorUpdateSyntax(erow *row);
int editorSyntaxToColor(int hl);

#endif
