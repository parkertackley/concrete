#ifndef TERMINAL_H
#define TERMINAL_H

#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "definestruct.h"

void die(const char *s);
void disbaleRawMode();
void enableRawMode();
int editorReadKey();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);

#endif
