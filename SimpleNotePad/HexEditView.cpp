#include "stdafx.h"
#include "HexEditView.h"

IMPLEMENT_DYNCREATE(CHexEditView, CView)

CHexEditView::CHexEditView()
{

}

void CHexEditView::InitHexView()
{
    SendMessage(SCI_SETHSCROLLBAR, 0);
    SetReadOnly(true);

    SendMessage(SCI_STYLECLEARALL); //如果没有这句，设置字体的粗体、任何等样式无效
    
    SetLexer(SCLEX_HEXVIEW);
    //设置颜色
    SetSyntaxColor(SCE_HEXVIEW_ADDR, 0xCD4230);
    SetSyntaxFontStyle(SCE_HEXVIEW_ADDR, true, false);
    SetSyntaxColor(SCE_HEXVIEW_BYTES, 0x794E41);
    SetSyntaxColor(SCE_HEXVIEW_DUMP, 0x737300);
    //去掉左侧的空白
    SendMessage(SCI_SETMARGINWIDTHN, 1, 0);
}

int CHexEditView::GetFirstVisibleLine()
{
    return SendMessage(SCI_GETFIRSTVISIBLELINE);
}

void CHexEditView::SetFirstVisibleLine(int line)
{
    SendMessage(SCI_SETFIRSTVISIBLELINE, line);
}
