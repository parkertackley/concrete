#ifndef ROWOPERATIONS_H
#define ROWOPERATIONS_H

#include <stddef.h>
#include <string.h>
#include "definestruct.h"

int editorRowCxToRx(erow *row, int cx);
void editorUpdateRow(erow *row);
void editorAppendRow(char *s, size_t len);

#endif