#ifndef ROWOPERATIONS_H
#define ROWOPERATIONS_H

#include <stddef.h>
#include <string.h>
#include "definestruct.h"
#include "editoroperations.h"

int editorRowCxToRx(erow *row, int cx);
void editorUpdateRow(erow *row);
void editorAppendRow(char *s, size_t len);
void editorFreeRow(erow *row);
void editorDelRow(int at);
void editorRowInsertChar(erow *row, int at, int c);
void editorRowAppendString(erow *row, char *s, size_t len);
void editorRowDelChar(erow *row, int at);

#endif
