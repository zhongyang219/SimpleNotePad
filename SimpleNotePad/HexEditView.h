#pragma once
#include "ScintillaEditView.h"
class CHexEditView :
    public CScintillaEditView
{
public:
    void InitHexView();

    int GetFirstVisibleLine();
    void SetFirstVisibleLine(int line);
};

