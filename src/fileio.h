#ifndef FILEIO_H
#define FILEIO_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "inputfile.h"
#include "definestruct.h"
#include "rowoperations.h"
#include "terminal.h"
#include "outputfile.h"

char *editorRowsToString(int *buflen);
void editorOpen();
void editorSave();
#endif
