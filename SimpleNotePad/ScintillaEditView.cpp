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
    m_change_notification_enable = false;       //确保正在执行SetText时不响应文本改变消息
    int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
    if (size <= 0) return;
    char* str = new char[size + 1];
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, str, size, NULL, NULL);
    SendMessage(SCI_SETTEXT, 0, (LPARAM)str);
    delete[] str;
    m_change_notification_enable = true;
}

void CScintillaEditView::GetText(wstring& text)
{
    auto length = SendMessage(SCI_GETLENGTH);
    char* buf = new char[length + 1];
    SendMessage(SCI_GETTEXT, length + 1, reinterpret_cast<LPARAM>(buf));

    int size = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
    if (size <= 0) return;
    wchar_t* str_unicode = new wchar_t[size + 1];
    MultiByteToWideChar(CP_UTF8, 0, buf, -1, str_unicode, size);
    text.assign(str_unicode);
    delete[] str_unicode;
    delete[] buf;
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

void CScintillaEditView::SetSel(int start, int end)
{
    SendMessage(SCI_SETSEL, start, end);
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

void CScintillaEditView::EmptyUndoBuffer()
{
    SendMessage(SCI_EMPTYUNDOBUFFER);
}

void CScintillaEditView::SetWordWrap(bool word_wrap)
{
    SendMessage(SCI_SETWRAPMODE, word_wrap ? SC_WRAP_WORD : SC_WRAP_NONE);
}


bool CScintillaEditView::IsEditChangeNotificationEnable()
{
    return m_change_notification_enable;
}

bool CScintillaEditView::CanUndo()
{
    return (SendMessage(SCI_CANUNDO) != 0);
}

bool CScintillaEditView::CanRedo()
{
    return (SendMessage(SCI_CANREDO) != 0);
}

bool CScintillaEditView::CanPaste()
{
    return (SendMessage(SCI_CANPASTE) != 0);
}

bool CScintillaEditView::IsSelectionEmpty()
{
    int anchor = SendMessage(SCI_GETANCHOR);
    int current_pos = SendMessage(SCI_GETCURRENTPOS);
    return anchor == current_pos;
}

void CScintillaEditView::SetLineNumberWidth(int width)
{
    m_line_number_width = width;
}

void CScintillaEditView::ShowLineNumber(bool show)
{
    if (show)
        SendMessage(SCI_SETMARGINWIDTHN, SCINTILLA_MARGIN_LINENUMBER, m_line_number_width);
    else
        SendMessage(SCI_SETMARGINWIDTHN, SCINTILLA_MARGIN_LINENUMBER, 0);
}

void CScintillaEditView::SetLineNumberColor(COLORREF color)
{
    SendMessage(SCI_STYLESETFORE, STYLE_LINENUMBER, color);
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


void CScintillaEditView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    // TODO: 在此添加专用代码和/或调用基类
    SendMessage(SCI_SETCODEPAGE, SC_CP_UTF8);       //总是使用Unicode

    SendMessage(SCI_SETMARGINTYPEN, SCINTILLA_MARGIN_LINENUMBER, SC_MARGIN_NUMBER);
}
