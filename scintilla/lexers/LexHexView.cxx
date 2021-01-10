// Scintilla source code edit control
/** @file HexView.cxx
 ** Lexer for no language. Used for plain text and unrecognized files.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

//用于解析十六进制查看器界面

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;

static inline bool AtEOL(Accessor &styler, Sci_PositionU i) {
    return (styler[i] == '\n') ||
        ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static void ColouriseMakeLine(char *lineBuffer, Sci_PositionU lengthLine, Sci_PositionU startLine, Sci_PositionU endPos, Accessor &styler)
{
    const int addr_length = 9;
    const int bytes_length = 49;
    styler.ColourTo(startLine + addr_length, SCE_HEXVIEW_ADDR);
    styler.ColourTo(startLine + addr_length + bytes_length, SCE_HEXVIEW_BYTES);
    styler.ColourTo(endPos, SCE_HEXVIEW_DUMP);
}

static void ColouriseHexViewDoc(Sci_PositionU startPos, Sci_Position length, int, WordList *[],
                            Accessor &styler) {
    char lineBuffer[1024];
    styler.StartAt(startPos);
    styler.StartSegment(startPos);
    Sci_PositionU linePos = 0;
    Sci_PositionU startLine = startPos;
    for (Sci_PositionU i = startPos; i < startPos + length; i++) {
        lineBuffer[linePos++] = styler[i];
        if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
            // End of line (or of line buffer) met, colourise it
            lineBuffer[linePos] = '\0';
            ColouriseMakeLine(lineBuffer, linePos, startLine, i, styler);
            linePos = 0;
            startLine = i + 1;
        }
    }
    if (linePos > 0) {	// Last line does not have ending characters
        ColouriseMakeLine(lineBuffer, linePos, startLine, startPos + length - 1, styler);
    }
}

LexerModule lmHexView(SCLEX_HEXVIEW, ColouriseHexViewDoc, "null");
