// ScintillaEditView.cpp: 实现文件
//

#include "stdafx.h"
#include "ScintillaEditView.h"
#include "Common.h"


// CScintillaEditView

IMPLEMENT_DYNCREATE(CScintillaEditView, CView)

CScintillaEditView::CScintillaEditView()
{

}

CScintillaEditView::~CScintillaEditView()
{
}

BEGIN_MESSAGE_MAP(CScintillaEditView, CView)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CScintillaEditView 绘图

void CScintillaEditView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// CScintillaEditView 诊断

#ifdef _DEBUG
void CScintillaEditView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CScintillaEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif
#endif //_DEBUG


void CScintillaEditView::SetText(const wstring& text)
{
    bool char_connot_connvert;
    string str_utf8 = CCommon::UnicodeToStr(text, char_connot_connvert, CodeType::UTF8_NO_BOM);
    SendMessage(SCI_SETTEXT, 0, (LPARAM)str_utf8.c_str());
}

void CScintillaEditView::SetFontFace(const char * font_face)
{
    SendMessage(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)font_face);
}

void CScintillaEditView::SetFontFace(const wchar_t* font_face)
{
    bool char_connot_convert;
    string str_font_face = CCommon::UnicodeToStr(font_face, char_connot_convert);
    SetFontFace(str_font_face.c_str());
}

void CScintillaEditView::SetFontSize(int font_size)
{
    SendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, font_size);
}

void CScintillaEditView::SetTabSize(int tab_size)
{
    SendMessage(SCI_SETTABWIDTH, tab_size);
}

void CScintillaEditView::Undo()
{
    SendMessage(SCI_UNDO);
}

void CScintillaEditView::Redo()
{
    SendMessage(SCI_REDO);
}

void CScintillaEditView::Cut()
{
    SendMessage(SCI_CUT);
}

void CScintillaEditView::Copy()
{
    SendMessage(SCI_COPY);
}

void CScintillaEditView::Paste()
{
    SendMessage(SCI_PASTE);
}

void CScintillaEditView::SelectAll()
{
    SendMessage(SCI_SELECTALL);
}

void CScintillaEditView::SetWordWrap(bool word_wrap)
{
    SendMessage(SCI_SETWRAPMODE, word_wrap ? SC_WRAP_WORD : SC_WRAP_NONE);
}


// CScintillaEditView 消息处理程序


BOOL CScintillaEditView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此添加专用代码和/或调用基类
    cs.lpszClass = _T("Scintilla");


    return CView::PreCreateWindow(cs);
}


void CScintillaEditView::OnPaint()
{
    //CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CView::OnPaint()
    Default();
}


void CScintillaEditView::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    CView::PreSubclassWindow();
}
