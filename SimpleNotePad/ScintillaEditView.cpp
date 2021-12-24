// ScintillaEditView.cpp: 实现文件
//

#include "stdafx.h"
#include "ScintillaEditView.h"
#include "Common.h"
#include "SimpleNotePad.h"
#include <algorithm>


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
    ON_WM_RBUTTONUP()
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


void CScintillaEditView::SetTextW(const wstring& text)
{
    m_change_notification_enable = false;       //确保正在执行SetText时不响应文本改变消息
    bool is_read_onle = IsReadOnly();
    //执行设置文件前，如果编辑器的只读的，则取消只读
    if (is_read_onle)
        SetReadOnly(false);
    int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), NULL, 0, NULL, NULL);
    if (size > 0)
    {
        char* str = new char[size + 1];
        WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), str, size, NULL, NULL);
        SendMessage(SCI_SETTEXT, size, (LPARAM)str);
        delete[] str;
    }
    else
    {
        SendMessage(SCI_SETTEXT, 0, (LPARAM)"");
    }
    //恢复只读状态
    if (is_read_onle)
        SetReadOnly(true);
    m_change_notification_enable = true;
}

void CScintillaEditView::GetTextW(wstring& text)
{
    text.clear();
    int size{};
    const wchar_t* str_unicode = GetTextW(size);
    if (size == 0)
        return;
    text.assign(str_unicode, size);
    delete[] str_unicode;
}

const wchar_t* CScintillaEditView::GetTextW(int& size)
{
    auto length = GetDocLength();
    char* buf = new char[length + 1];
    SendMessage(SCI_GETTEXT, length + 1, reinterpret_cast<LPARAM>(buf));

    size = MultiByteToWideChar(CP_UTF8, 0, buf, length, NULL, 0);
    if (size <= 0) return nullptr;
    wchar_t* str_unicode = new wchar_t[size + 1];
    MultiByteToWideChar(CP_UTF8, 0, buf, length, str_unicode, size);
    //text.assign(str_unicode, size);
    //delete[] str_unicode;
    delete[] buf;
    return str_unicode;
}

const char * CScintillaEditView::GetText(int & size)
{
    size = GetDocLength();
    char* buf = new char[size + 1];
    SendMessage(SCI_GETTEXT, size + 1, reinterpret_cast<LPARAM>(buf));
    return buf;
}

std::string CScintillaEditView::GetText(int start, int end)
{
    if (start == end)
        return std::string();
    Sci_TextRange text_range;
    //获取选中范围
    text_range.chrg.cpMin = start;
    text_range.chrg.cpMax = end;
    if (text_range.chrg.cpMax < text_range.chrg.cpMin)
        std::swap(text_range.chrg.cpMin, text_range.chrg.cpMax);
    //选中范围长度
    int length = text_range.chrg.cpMax - text_range.chrg.cpMin;
    //初始化接收字符串缓冲区
    char* buff = new char[length + 1];
    text_range.lpstrText = buff;
    //获取选中部分文本
    SendMessage(SCI_GETTEXTRANGE, 0, (LPARAM)&text_range);
    std::string str_selected(buff, length);
    delete[] buff;
    return str_selected;
}

int CScintillaEditView::GetDocLength()
{
    return SendMessage(SCI_GETLENGTH);
}

int CScintillaEditView::GetLines()
{
    int pos = GetDocLength();
    return SendMessage(SCI_LINEFROMPOSITION, pos);
}

void CScintillaEditView::SetFontFaceW(const wchar_t* font_face)
{
    bool char_connot_convert;
    string str_font_face = CCommon::UnicodeToStr(font_face, char_connot_convert, CodeType::UTF8_NO_BOM);
    SendMessage(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)str_font_face.c_str());
}

void CScintillaEditView::SetFontSize(int font_size)
{
    SendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, font_size);
}

void CScintillaEditView::SetTabSize(int tab_size)
{
    SendMessage(SCI_SETTABWIDTH, tab_size);
}

void CScintillaEditView::SetSelW(int start, int end, const wstring& edit_str)
{
    int byte_start = CharactorPosToBytePos(start, edit_str.c_str(), edit_str.size());
    int byte_end = CharactorPosToBytePos(end, edit_str.c_str(), edit_str.size());
    SetSel(byte_start, byte_end);
}

void CScintillaEditView::GetSelW(int & start, int & end)
{
    int byte_start{};
    int byte_end{};
    GetSel(byte_start, byte_end);
    int size{};
    const char* str = GetText(size);
    start = BytePosToCharactorPos(byte_start, str, size);
    end = BytePosToCharactorPos(byte_end, str, size);
    delete[] str;
}

void CScintillaEditView::SetSel(int start, int end)
{
    SendMessage(SCI_SETSEL, start, end);
}

void CScintillaEditView::GetSel(int& start, int& end)
{
    start = SendMessage(SCI_GETANCHOR);
    end = SendMessage(SCI_GETCURRENTPOS);
    if (end < start)
        std::swap(start, end);
}

int CScintillaEditView::GetColumn()
{
    int pos = SendMessage(SCI_GETCURRENTPOS);
    return SendMessage(SCI_GETCOLUMN, pos);
}

int CScintillaEditView::GetRow()
{
    int pos = SendMessage(SCI_GETCURRENTPOS);
    return SendMessage(SCI_LINEFROMPOSITION, pos);
}

int CScintillaEditView::GetSelCount()
{
    int start{}, end{};
    GetSel(start, end);
    return SendMessage(SCI_COUNTCHARACTERS, start, end);
}

void CScintillaEditView::SetBackgroundColor(COLORREF color)
{
    m_background_color = color;
    //SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, m_background_color);
}

void CScintillaEditView::SetReadOnly(bool read_only)
{
    SendMessage(SCI_SETREADONLY, read_only);
}

bool CScintillaEditView::IsReadOnly()
{
    return (SendMessage(SCI_GETREADONLY) != 0);
}

void CScintillaEditView::ShowIndentationGuides(bool show)
{
    SendMessage(SCI_SETINDENTATIONGUIDES, show ? SC_IV_LOOKBOTH : SC_IV_NONE);
}

int CScintillaEditView::GetCurrentIndex()
{
    return SendMessage(SCI_GETCURRENTPOS);
}

std::wstring CScintillaEditView::GetSelectedTextW()
{
    std::string str_selected = GetSelectedText();
    return CCommon::StrToUnicode(str_selected, CodeType::UTF8_NO_BOM);
}

std::string CScintillaEditView::GetSelectedText()
{
    Sci_TextRange text_range;
    //获取选中范围
    text_range.chrg.cpMin = SendMessage(SCI_GETANCHOR);
    text_range.chrg.cpMax = SendMessage(SCI_GETCURRENTPOS);
    if (text_range.chrg.cpMin == text_range.chrg.cpMax)
        return std::string();
    if (text_range.chrg.cpMax < text_range.chrg.cpMin)
        std::swap(text_range.chrg.cpMin, text_range.chrg.cpMax);
    //选中范围长度
    int length = text_range.chrg.cpMax - text_range.chrg.cpMin;
    //初始化接收字符串缓冲区
    char* buff = new char[length + 1];
    text_range.lpstrText = buff;
    //获取选中部分文本
    SendMessage(SCI_GETTEXTRANGE, 0, (LPARAM)&text_range);
    std::string str_selected(buff, length);
    delete[] buff;
    return str_selected;
}

CPoint CScintillaEditView::GetCursorPosition()
{
    int cur_index = GetCurrentIndex();
    CPoint point{};
    point.x = SendMessage(SCI_POINTXFROMPOSITION, 0, cur_index);
    point.y = SendMessage(SCI_POINTYFROMPOSITION, 0, cur_index);
    return point;
}

char CScintillaEditView::At(int index)
{
    int ch = SendMessage(SCI_GETCHARAT, index);
    return static_cast<char>(ch);
}

bool CScintillaEditView::AutoSelectWord()
{
    if (!IsSelectionEmpty())
        return false;

    int pos = GetCurrentIndex();
    int start = SendMessage(SCI_WORDSTARTPOSITION, pos, true);
    int end = SendMessage(SCI_WORDENDPOSITION, pos, true);

    if (start == end)
    {
        return false;
    }
    else
    {
        SetSel(start, end);
        return true;
    }
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

void CScintillaEditView::Paste(const string& text)
{
    //插入要粘贴的文本
    SendMessage(SCI_INSERTTEXT, -1, (sptr_t)text.c_str());
}

void CScintillaEditView::PasteW(const wstring& text)
{
    bool b;
    std::string str_paste = CCommon::UnicodeToStr(text, b, CodeType::UTF8_NO_BOM);
    Paste(str_paste);
}

void CScintillaEditView::SelectAll()
{
    SendMessage(SCI_SELECTALL);
}

void CScintillaEditView::EmptyUndoBuffer()
{
    SendMessage(SCI_EMPTYUNDOBUFFER);
}

void CScintillaEditView::ReplaceSelectedW(const wstring& replace_str)
{
    bool noused;
    string replaced_str = CCommon::UnicodeToStr(replace_str, noused, CodeType::UTF8_NO_BOM);
    SendMessage(SCI_REPLACESEL, 0, (sptr_t)replaced_str.c_str());
}

void CScintillaEditView::InserText(const std::string& str, int pos)
{
    SendMessage(SCI_INSERTTEXT, pos, (sptr_t)str.c_str());
}

void CScintillaEditView::DeleteText(int pos, int length)
{
    SendMessage(SCI_DELETERANGE, pos, length);
}

void CScintillaEditView::SetWordWrap(bool word_wrap, eWordWrapMode mode)
{
    if (!word_wrap)
    {
        SendMessage(SCI_SETWRAPMODE, SC_WRAP_NONE);
    }
    else
    {
        switch (mode)
        {
        case CScintillaEditView::WW_WORD:
            SendMessage(SCI_SETWRAPMODE, SC_WRAP_WORD);
            break;
        case CScintillaEditView::WW_CHARACTER:
            SendMessage(SCI_SETWRAPMODE, SC_WRAP_CHAR);
            break;
        case CScintillaEditView::WW_WHITESPACE:
            SendMessage(SCI_SETWRAPMODE, SC_WRAP_WHITESPACE);
            break;
        default:
            break;
        }
    }
}

bool CScintillaEditView::IsEditChangeNotificationEnable()
{
    return m_change_notification_enable;
}

void CScintillaEditView::SetEditChangeNotificationEnable(bool enable)
{
    m_change_notification_enable = enable;
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

bool CScintillaEditView::IsModified()
{
    return m_modified || (SendMessage(SCI_GETMODIFY) != 0);
}

void CScintillaEditView::SetModified()
{
    m_modified = true;
}

void CScintillaEditView::SetSavePoint()
{
    m_modified = false;
    SendMessage(SCI_SETSAVEPOINT);
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
    //SendMessage(SCI_STYLESETFORE, STYLE_LINENUMBER, color);
    m_line_number_color = color;
}

int CScintillaEditView::GetZoom()
{
    return SendMessage(SCI_GETZOOM);
}

void CScintillaEditView::SetZoom(int zoom)
{
    SendMessage(SCI_SETZOOM, zoom);
}

void CScintillaEditView::Zoom(bool in)
{
    if (in)
        SendMessage(SCI_ZOOMIN);
    else
        SendMessage(SCI_ZOOMOUT);
}

void CScintillaEditView::SetEolMode(eEolMode eolMode)
{
    int mode = 0;
    switch (eolMode)
    {
    case EOL_CRLF:
        mode = SC_EOL_CRLF;
        break;
    case EOL_CR:
        mode = SC_EOL_CR;
        break;
    case EOL_LF:
        mode = SC_EOL_LF;
        break;
    default:
        break;
    }
    SendMessage(SCI_SETEOLMODE, mode);
}

CScintillaEditView::eEolMode CScintillaEditView::GetEolMode()
{
    int mode = SendMessage(SCI_GETEOLMODE);
    switch (mode)
    {
    case SC_EOL_CR:
        return EOL_CR;
    case SC_EOL_LF:
        return EOL_LF;
    default:
        return EOL_CRLF;
        break;
    }
}

void CScintillaEditView::ConvertEolMode(eEolMode eolMode)
{
    int mode = 0;
    switch (eolMode)
    {
    case EOL_CRLF:
        mode = SC_EOL_CRLF;
        break;
    case EOL_CR:
        mode = SC_EOL_CR;
        break;
    case EOL_LF:
        mode = SC_EOL_LF;
        break;
    default:
        break;
    }
    SendMessage(SCI_CONVERTEOLS, mode);
}

void CScintillaEditView::SetViewEol(bool show)
{
    SendMessage(SCI_SETVIEWEOL, show);
}

int CScintillaEditView::GetFirstVisibleLine()
{
    return SendMessage(SCI_GETFIRSTVISIBLELINE);
}

void CScintillaEditView::SetFirstVisibleLine(int line)
{
    SendMessage(SCI_SETFIRSTVISIBLELINE, line);
}

int CScintillaEditView::GetLineHeight()
{
    return SendMessage(SCI_TEXTHEIGHT, 0);
}

void CScintillaEditView::SetCurrentLineHighlight(bool highlight)
{
    if (highlight)
    {
        //设置当前行背景色
        SendMessage(SCI_SETCARETLINEVISIBLE, TRUE);
        SendMessage(SCI_SETCARETLINEBACK, m_current_line_highlight_color);
    }
    else
    {
        SendMessage(SCI_SETCARETLINEVISIBLE, FALSE);
    }
}

void CScintillaEditView::SetCurrentLineHighlightColor(COLORREF color)
{
    m_current_line_highlight_color = color;
}

void CScintillaEditView::SetSelectionBackColor(COLORREF color)
{
    SendMessage(SCI_SETSELBACK, 1, color);
}

void CScintillaEditView::GotoLine(int line)
{
    SendMessage(SCI_GOTOLINE, line);
}

void CScintillaEditView::SetMark(MarkStyle mark_style, int start, int length)
{
    SendMessage(SCI_SETINDICATORCURRENT, static_cast<WPARAM>(mark_style));
    SendMessage(SCI_INDICATORFILLRANGE, start, length);
}

void CScintillaEditView::ClearMark(MarkStyle mark_style, int start, int length)
{
    SendMessage(SCI_SETINDICATORCURRENT, static_cast<WPARAM>(mark_style));
    SendMessage(SCI_INDICATORCLEARRANGE, start, length);
}

void CScintillaEditView::ClearAllMark(MarkStyle mark_style)
{
    ClearMark(mark_style, 0, GetDocLength());
}

void CScintillaEditView::SetLexer(int lexer)
{
    SendMessage(SCI_SETLEXER, lexer);
}

void CScintillaEditView::SetKeywords(int id, const char* keywords)
{
    SendMessage(SCI_SETKEYWORDS, id, (sptr_t)keywords);
}

void CScintillaEditView::SetSyntaxColor(int id, COLORREF color)
{
    SendMessage(SCI_STYLESETFORE, id, color);
}

void CScintillaEditView::SetSyntaxFontStyle(int id, bool bold, bool italic)
{
    SendMessage(SCI_STYLESETBOLD, id, bold);
    SendMessage(SCI_STYLESETITALIC, id, italic);
}

void CScintillaEditView::SetLexerNormalText()
{
    SetLexer(SCLEX_NULL);
    SendMessage(SCI_STYLESETFORE, STYLE_DEFAULT, RGB(0, 0, 0));
    SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, m_background_color);
    SendMessage(SCI_STYLESETBOLD, STYLE_DEFAULT, 0);
    SendMessage(SCI_STYLESETBOLD, STYLE_DEFAULT, 0);
    SendMessage(SCI_STYLECLEARALL);

    SendMessage(SCI_STYLESETFORE, STYLE_LINENUMBER, m_line_number_color);
    SendMessage(SCI_STYLESETFORE, STYLE_INDENTGUIDE, RGB(143, 187, 240));

}

CScintillaEditView::eEolMode CScintillaEditView::JudgeEolMode(const wstring& str)
{
    size_t index = str.find(L"\r\n");
    if (index != wstring::npos)
        return EOL_CRLF;

    index = str.find(L'\n');
    if (index != wstring::npos)
        return EOL_LF;

    index = str.find(L'\r');
    if (index != wstring::npos)
        return EOL_CR;

    return EOL_CRLF;

}

int CScintillaEditView::CharactorPosToBytePos(int pos, const wchar_t * str, size_t size)
{
    if (pos >= size)
        return size;
    else
        return WideCharToMultiByte(CP_UTF8, 0, str, pos, NULL, 0, NULL, NULL);
}

int CScintillaEditView::BytePosToCharactorPos(int pos, const char * str, size_t size)
{
    if (pos >= size)
        return size;
    else
        return MultiByteToWideChar(CP_UTF8, 0, str, pos, NULL, 0);
}

void CScintillaEditView::SetFold()
{
    SendMessage(SCI_SETPROPERTY, (sptr_t)"fold", (sptr_t)"1");

    SendMessage(SCI_SETMARGINTYPEN, MARGIN_FOLD_INDEX, SC_MARGIN_SYMBOL);//页边类型
    SendMessage(SCI_SETMARGINMASKN, MARGIN_FOLD_INDEX, SC_MASK_FOLDERS); //页边掩码
    SendMessage(SCI_SETMARGINSENSITIVEN, MARGIN_FOLD_INDEX, TRUE); //响应鼠标消息

    // 折叠标签样式
    SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
    SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
    SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
    SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);
    SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);

    // 折叠标签颜色
    SendMessage(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB, 0xa0a0a0);
    SendMessage(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL, 0xa0a0a0);
    SendMessage(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL, 0xa0a0a0);

    SendMessage(SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_CONTRACTED, 0); //如果折叠就在折叠行的下面画一条横线

	//SendMessage(SCI_SETMARGINWIDTHN, MARGIN_FOLD_INDEX, std::min(GetLineHeight(), theApp.DPI(16)));
}

void CScintillaEditView::SetContextMenu(CMenu* pMenu, CWnd* pMenuOwner)
{
    if (pMenu != nullptr)
    {
        m_pMenu = pMenu;
        m_pContextMenuOwner = pMenuOwner;
        SendMessage(SCI_USEPOPUP, SC_POPUP_NEVER);
    }

}

int CScintillaEditView::Find(std::string str, int start, int end)
{
    if (end < 0)
        end = GetDocLength();
    Sci_TextToFind ttf;
    ttf.chrg.cpMin = start;
    ttf.chrg.cpMax = end;
    ttf.lpstrText = str.c_str();
    return SendMessage(SCI_FINDTEXT, SCFIND_MATCHCASE, (LPARAM)&ttf);
}

void CScintillaEditView::GetLinePos(int line, int& start, int& end)
{
    start = SendMessage(SCI_POSITIONFROMLINE, line);
    end = SendMessage(SCI_GETLINEENDPOSITION, line);
    int doc_length = GetDocLength();
    if (start < 0 && start > doc_length)
        start = 0;
    if (end < 0 || end > doc_length)
        end = doc_length;
}

void CScintillaEditView::GetCurLinePos(int& start, int& end)
{
    int cur_pos = SendMessage(SCI_GETCURRENTPOS);
    int cur_line = SendMessage(SCI_LINEFROMPOSITION, cur_pos);
    GetLinePos(cur_line, start, end);
}

bool CScintillaEditView::IsFullLineSelected()
{
    int start{}, end{};
    GetSel(start, end);
    if (start == end)
        return false;
    int first_line_colume = SendMessage(SCI_GETCOLUMN, start);
    int last_line_colume = SendMessage(SCI_GETCOLUMN, end);
    return first_line_colume == 0 && (last_line_colume == 0 || end == GetDocLength());
}

void CScintillaEditView::GetLineSelected(int& first_line, int& last_line)
{
    int start{}, end{};
    GetSel(start, end);
    first_line = SendMessage(SCI_LINEFROMPOSITION, start);
    last_line = SendMessage(SCI_LINEFROMPOSITION, end);
    if (SendMessage(SCI_GETCOLUMN, end) == 0)       //如果最后一行没有字符，则不算该行
        last_line--;
}

int CScintillaEditView::GetLinesSelected()
{
    int first_line{}, last_line{};
    GetLineSelected(first_line, last_line);
    return last_line - first_line + 1;
}

void CScintillaEditView::GotoPos(int pos)
{
    SendMessage(SCI_GOTOPOS, pos);
}

int CScintillaEditView::GetTextWidth(const std::string& str)
{
    return SendMessage(SCI_TEXTWIDTH, STYLE_DEFAULT, (sptr_t)str.c_str());
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

    SendMessage(SCI_SETSCROLLWIDTH, 100);
    SendMessage(SCI_SETSCROLLWIDTHTRACKING, TRUE);

    //设置仅点击加号或减号时才展开或收缩级别
    SendMessage(SCI_SETAUTOMATICFOLD, SC_AUTOMATICFOLD_CLICK);

    SendMessage(SCI_SETCARETWIDTH, theApp.DPI(1.2));

    //设置标记样式
    SendMessage(SCI_INDICSETSTYLE, static_cast<WPARAM>(MarkStyle::MARK_ALL), INDIC_ROUNDBOX);
    SendMessage(SCI_INDICSETALPHA, static_cast<WPARAM>(MarkStyle::MARK_ALL), 140);
    SendMessage(SCI_INDICSETFORE, static_cast<WPARAM>(MarkStyle::MARK_ALL), RGB(255, 143, 107));
    
    SendMessage(SCI_INDICSETSTYLE, static_cast<WPARAM>(MarkStyle::SELECTION_MARK), INDIC_ROUNDBOX);
    SendMessage(SCI_INDICSETALPHA, static_cast<WPARAM>(MarkStyle::SELECTION_MARK), 110);
    SendMessage(SCI_INDICSETFORE, static_cast<WPARAM>(MarkStyle::SELECTION_MARK), RGB(140, 238, 20));
    
    SendMessage(SCI_INDICSETSTYLE, static_cast<WPARAM>(MarkStyle::MATCHED_BRACKETS), INDIC_ROUNDBOX);
    SendMessage(SCI_INDICSETALPHA, static_cast<WPARAM>(MarkStyle::MATCHED_BRACKETS), 110);
    SendMessage(SCI_INDICSETFORE, static_cast<WPARAM>(MarkStyle::MATCHED_BRACKETS), RGB(255, 209, 62));

    SendMessage(SCI_INDICSETSTYLE, static_cast<WPARAM>(MarkStyle::HTML_MARKS), INDIC_ROUNDBOX);
    SendMessage(SCI_INDICSETALPHA, static_cast<WPARAM>(MarkStyle::HTML_MARKS), 110);
    SendMessage(SCI_INDICSETFORE, static_cast<WPARAM>(MarkStyle::HTML_MARKS), RGB(125, 146, 243));

    SendMessage(SCI_SETINDENTGUIDWIDTH, theApp.DPI(1.2));
}


void CScintillaEditView::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (m_pMenu != nullptr)
    {
        CPoint point1;
        GetCursorPos(&point1);
        m_pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, m_pContextMenuOwner);
    }


    CView::OnRButtonUp(nFlags, point);
}
