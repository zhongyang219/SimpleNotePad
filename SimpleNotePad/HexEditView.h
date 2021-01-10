#pragma once
#include "ScintillaEditView.h"
class CHexEditView :
    public CScintillaEditView
{
    DECLARE_DYNCREATE(CHexEditView)

protected:
    CHexEditView();
public:
    void InitHexView();

    int GetFirstVisibleLine();
    void SetFirstVisibleLine(int line);
};

