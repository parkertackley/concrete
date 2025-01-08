#include "definestruct.h"

char *C_HL_extensions[] = {".c", ".c", ".cpp", NULL};

struct editorSyntax HLDB[] = {
    {
        "c",
        C_HL_extensions,
        "//",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
};

int getHLDBEntries() {
    return sizeof(HLDB) / sizeof(HLDB[0]);
};

struct editorConfig E;
