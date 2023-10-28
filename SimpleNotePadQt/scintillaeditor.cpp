#include "scintillaeditor.h"
#include "SciLexer.h"

ScintillaEditor::ScintillaEditor(QWidget *parent)
    : ScintillaEditBase(parent)
{
    send(SCI_SETCODEPAGE, SC_CP_UTF8);       //总是使用Unicode
    send(SCI_SETMARGINTYPEN, SCINTILLA_MARGIN_LINENUMBER, SC_MARGIN_NUMBER);

    send(SCI_SETSCROLLWIDTH, 100);
    send(SCI_SETSCROLLWIDTHTRACKING, 1);

    //设置仅点击加号或减号时才展开或收缩级别
    send(SCI_SETAUTOMATICFOLD, SC_AUTOMATICFOLD_CLICK);

    send(SCI_SETCARETWIDTH, 1);

    //设置标记样式
    send(SCI_INDICSETSTYLE, (uptr_t)(MarkStyle::MARK_ALL), INDIC_ROUNDBOX);
    send(SCI_INDICSETALPHA, (uptr_t)(MarkStyle::MARK_ALL), 140);
    send(SCI_INDICSETFORE, (uptr_t)(MarkStyle::MARK_ALL), 0x6b8fff);

    send(SCI_INDICSETSTYLE, (uptr_t)(MarkStyle::SELECTION_MARK), INDIC_ROUNDBOX);
    send(SCI_INDICSETALPHA, (uptr_t)(MarkStyle::SELECTION_MARK), 110);
    send(SCI_INDICSETFORE, (uptr_t)(MarkStyle::SELECTION_MARK), 0x14ee8c);

    send(SCI_INDICSETSTYLE, (uptr_t)(MarkStyle::MATCHED_BRACKETS), INDIC_ROUNDBOX);
    send(SCI_INDICSETALPHA, (uptr_t)(MarkStyle::MATCHED_BRACKETS), 110);
    send(SCI_INDICSETFORE, (uptr_t)(MarkStyle::MATCHED_BRACKETS), 0x3ed1ff);

    send(SCI_INDICSETSTYLE, (uptr_t)(MarkStyle::HTML_MARKS), INDIC_ROUNDBOX);
    send(SCI_INDICSETALPHA, (uptr_t)(MarkStyle::HTML_MARKS), 110);
    send(SCI_INDICSETFORE, (uptr_t)(MarkStyle::HTML_MARKS), 0xf3927d);

    send(SCI_SETINDENTGUIDWIDTH, 1);

}

void ScintillaEditor::SetText(const QString &text)
{
    m_change_notification_enable = false;       //确保正在执行SetText时不响应文本改变消息
    bool is_read_onle = IsReadOnly();
    //执行设置文件前，如果编辑器的只读的，则取消只读
    if (is_read_onle)
        SetReadOnly(false);
    QByteArray textData = text.toUtf8();
    if (textData.size() > 0)
    {
        send(SCI_SETTEXT, (uptr_t)textData.size(), (sptr_t)textData.constData());
    }
    else
    {
        send(SCI_SETTEXT, 0, (sptr_t)"");
    }
    //恢复只读状态
    if (is_read_onle)
        SetReadOnly(true);
    m_change_notification_enable = true;
}

void ScintillaEditor::GetText(QString &text)
{
    text.clear();
    int size{};
    const char* str = GetText(size);
    if (size == 0)
        return;
    text = QString::fromUtf8(str, size);
    delete[] str;
}

const char *ScintillaEditor::GetText(int &size)
{
    auto length = GetDocLength();
    size = length + 1;
    char* buf = new char[static_cast<size_t>(size)];
    send(SCI_GETTEXT, (uptr_t)(length + 1), (sptr_t)buf);
    return buf;
}

QString ScintillaEditor::GetText(int start, int end)
{
    if (start == end)
        return QString();
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
    send(SCI_GETTEXTRANGE, 0, (sptr_t)&text_range);
    QString str_selected = QString::fromUtf8(buff, length);
    delete[] buff;
    return str_selected;
}

int ScintillaEditor::GetDocLength()
{
    return send(SCI_GETLENGTH);
}

int ScintillaEditor::GetLines()
{
    int pos = GetDocLength();
    return send(SCI_LINEFROMPOSITION, (uptr_t)pos);
}

void ScintillaEditor::SetFontFace(const QString& font_face)
{
    send(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)font_face.toUtf8().constData());
}

void ScintillaEditor::SetFontSize(int font_size)
{
    send(SCI_STYLESETSIZE, STYLE_DEFAULT, font_size);
}

void ScintillaEditor::SetTabSize(int tab_size)
{
    send(SCI_SETTABWIDTH, (uptr_t)tab_size);
}

void ScintillaEditor::SetSel(int start, int end)
{
    send(SCI_SETSEL, (uptr_t)start, end);
}

void ScintillaEditor::GetSel(int &start, int &end)
{
    start = send(SCI_GETANCHOR);
    end = send(SCI_GETCURRENTPOS);
    if (end < start)
        std::swap(start, end);
}

int ScintillaEditor::GetColumn()
{
    int pos = send(SCI_GETCURRENTPOS);
    return send(SCI_GETCOLUMN, (uptr_t)pos);
}

int ScintillaEditor::GetRow()
{
    int pos = send(SCI_GETCURRENTPOS);
    return send(SCI_LINEFROMPOSITION, (uptr_t)pos);
}

int ScintillaEditor::GetSelCount()
{
    int start{}, end{};
    GetSel(start, end);
    return send(SCI_COUNTCHARACTERS, (uptr_t)start, end);
}

void ScintillaEditor::SetBackgroundColor(unsigned int color)
{
    m_background_color = color;
}

void ScintillaEditor::SetReadOnly(bool read_only)
{
    send(SCI_SETREADONLY, read_only);
}

bool ScintillaEditor::IsReadOnly()
{
    return (send(SCI_GETREADONLY) != 0);
}

void ScintillaEditor::ShowIndentationGuides(bool show)
{
    send(SCI_SETINDENTATIONGUIDES, show ? SC_IV_LOOKBOTH : SC_IV_NONE);
}

int ScintillaEditor::GetCurrentIndex()
{
    return send(SCI_GETCURRENTPOS);
}

QString ScintillaEditor::GetSelectedText()
{
    Sci_TextRange text_range;
    //获取选中范围
    text_range.chrg.cpMin = send(SCI_GETANCHOR);
    text_range.chrg.cpMax = send(SCI_GETCURRENTPOS);
    if (text_range.chrg.cpMin == text_range.chrg.cpMax)
        return QString();
    if (text_range.chrg.cpMax < text_range.chrg.cpMin)
        std::swap(text_range.chrg.cpMin, text_range.chrg.cpMax);
    //选中范围长度
    int length = text_range.chrg.cpMax - text_range.chrg.cpMin;
    //初始化接收字符串缓冲区
    char* buff = new char[static_cast<size_t>(length + 1)];
    text_range.lpstrText = buff;
    //获取选中部分文本
    send(SCI_GETTEXTRANGE, 0, (sptr_t)&text_range);
    QString str_selected = QString::fromUtf8(buff, length);
    delete[] buff;
    return str_selected;
}

QPoint ScintillaEditor::GetCursorPosition()
{
    int cur_index = GetCurrentIndex();
    QPoint point{};
    point.setX(send(SCI_POINTXFROMPOSITION, 0, cur_index));
    point.setY(send(SCI_POINTYFROMPOSITION, 0, cur_index));
    return point;
}

char ScintillaEditor::At(int index)
{
    int ch = send(SCI_GETCHARAT, (uptr_t)index);
    return static_cast<char>(ch);
}

bool ScintillaEditor::AutoSelectWord()
{
    if (!IsSelectionEmpty())
        return false;

    int pos = GetCurrentIndex();
    int start = send(SCI_WORDSTARTPOSITION, (uptr_t)pos, true);
    int end = send(SCI_WORDENDPOSITION, (uptr_t)pos, true);

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

void ScintillaEditor::Undo()
{
    send(SCI_UNDO);
}

void ScintillaEditor::Redo()
{
    send(SCI_REDO);
}

void ScintillaEditor::Cut()
{
    send(SCI_CUT);
}

void ScintillaEditor::Copy()
{
    send(SCI_COPY);
}

void ScintillaEditor::Paste()
{
    send(SCI_PASTE);
}

void ScintillaEditor::Paste(const QString &text)
{
    //判断是否有选中文本
    if (IsSelectionEmpty())
        InserText(text, -1);
    else
        ReplaceSelection(text);
}

void ScintillaEditor::SelectAll()
{
    send(SCI_SELECTALL);
}

void ScintillaEditor::EmptyUndoBuffer()
{
    send(SCI_EMPTYUNDOBUFFER);
}

void ScintillaEditor::ReplaceSelection(const QString &replace_str)
{
    send(SCI_REPLACESEL, 0, (sptr_t)replace_str.toUtf8().constData());
}

void ScintillaEditor::InserText(const QString &str, int pos)
{
    send(SCI_INSERTTEXT, (uptr_t)pos, (sptr_t)str.toUtf8().constData());
}

void ScintillaEditor::DeleteText(int pos, int length)
{
    send(SCI_DELETERANGE, (uptr_t)pos, length);
}

void ScintillaEditor::SetWordWrap(bool word_wrap, ScintillaEditor::eWordWrapMode mode)
{
    if (!word_wrap)
    {
        send(SCI_SETWRAPMODE, SC_WRAP_NONE);
    }
    else
    {
        switch (mode)
        {
        case ScintillaEditor::WW_WORD:
            send(SCI_SETWRAPMODE, SC_WRAP_WORD);
            break;
        case ScintillaEditor::WW_CHARACTER:
            send(SCI_SETWRAPMODE, SC_WRAP_CHAR);
            break;
        case ScintillaEditor::WW_WHITESPACE:
            send(SCI_SETWRAPMODE, SC_WRAP_WHITESPACE);
            break;
        }
    }
}

bool ScintillaEditor::IsEditChangeNotificationEnable()
{
    return m_change_notification_enable;
}

void ScintillaEditor::SetEditChangeNotificationEnable(bool enable)
{
    m_change_notification_enable = enable;
}

bool ScintillaEditor::CanUndo()
{
    return (send(SCI_CANUNDO) != 0);
}

bool ScintillaEditor::CanRedo()
{
    return (send(SCI_CANREDO) != 0);
}

bool ScintillaEditor::CanPaste()
{
    return (send(SCI_CANPASTE) != 0);
}

bool ScintillaEditor::IsSelectionEmpty()
{
    int anchor = send(SCI_GETANCHOR);
    int current_pos = send(SCI_GETCURRENTPOS);
    return anchor == current_pos;
}

bool ScintillaEditor::IsModified()
{
    return m_modified || (send(SCI_GETMODIFY) != 0);
}

void ScintillaEditor::SetModified()
{
    m_modified = true;
}

void ScintillaEditor::SetSavePoint()
{
    m_modified = false;
    send(SCI_SETSAVEPOINT);
}

void ScintillaEditor::SetLineNumberWidth(int width)
{
    m_line_number_width = width;
}

void ScintillaEditor::ShowLineNumber(bool show)
{
    if (show)
        send(SCI_SETMARGINWIDTHN, SCINTILLA_MARGIN_LINENUMBER, m_line_number_width);
    else
        send(SCI_SETMARGINWIDTHN, SCINTILLA_MARGIN_LINENUMBER, 0);
}

void ScintillaEditor::SetLineNumberColor(unsigned int color)
{
    m_line_number_color = color;
}

int ScintillaEditor::GetZoom()
{
    return send(SCI_GETZOOM);
}

void ScintillaEditor::SetZoom(int zoom)
{
    send(SCI_SETZOOM, (uptr_t)zoom);
}

void ScintillaEditor::Zoom(bool in)
{
    if (in)
        send(SCI_ZOOMIN);
    else
        send(SCI_ZOOMOUT);
}

void ScintillaEditor::SetEolMode(ScintillaEditor::eEolMode eolMode)
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
    }
    send(SCI_SETEOLMODE, (uptr_t)mode);
}

ScintillaEditor::eEolMode ScintillaEditor::GetEolMode()
{
    int mode = send(SCI_GETEOLMODE);
    switch (mode)
    {
    case SC_EOL_CR:
        return EOL_CR;
    case SC_EOL_LF:
        return EOL_LF;
    default:
        return EOL_CRLF;
    }
}

void ScintillaEditor::ConvertEolMode(ScintillaEditor::eEolMode eolMode)
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
    }
    send(SCI_CONVERTEOLS, (uptr_t)mode);
}

void ScintillaEditor::SetViewEol(bool show)
{
    send(SCI_SETVIEWEOL, show);
}

int ScintillaEditor::GetFirstVisibleLine()
{
    return send(SCI_GETFIRSTVISIBLELINE);
}

void ScintillaEditor::SetFirstVisibleLine(int line)
{
    send(SCI_SETFIRSTVISIBLELINE, (uptr_t)line);
}

int ScintillaEditor::GetLineHeight()
{
    return send(SCI_TEXTHEIGHT, 0);
}

void ScintillaEditor::SetCurrentLineHighlight(bool highlight)
{
    if (highlight)
    {
        //设置当前行背景色
        send(SCI_SETCARETLINEVISIBLE, 1);
        send(SCI_SETCARETLINEBACK, m_current_line_highlight_color);
    }
    else
    {
        send(SCI_SETCARETLINEVISIBLE, 0);
    }
}

void ScintillaEditor::SetCurrentLineHighlightColor(unsigned int color)
{
    m_current_line_highlight_color = color;
}

void ScintillaEditor::SetSelectionBackColor(unsigned int color)
{
    send(SCI_SETSELBACK, 1, color);
}

void ScintillaEditor::GotoLine(int line)
{
    send(SCI_GOTOLINE, (uptr_t)line);
}

void ScintillaEditor::SetMark(ScintillaEditor::MarkStyle mark_style, int start, int length)
{
    send(SCI_SETINDICATORCURRENT, (uptr_t)mark_style);
    send(SCI_INDICATORFILLRANGE, (uptr_t)start, length);
}

void ScintillaEditor::ClearMark(ScintillaEditor::MarkStyle mark_style, int start, int length)
{
    send(SCI_SETINDICATORCURRENT, (uptr_t)mark_style);
    send(SCI_INDICATORCLEARRANGE, (uptr_t)start, length);
}

void ScintillaEditor::ClearAllMark(ScintillaEditor::MarkStyle mark_style)
{
    ClearMark(mark_style, 0, GetDocLength());
}

void ScintillaEditor::SetLexer(int lexer)
{
    send(SCI_SETLEXER, (uptr_t)lexer);
}

void ScintillaEditor::SetKeywords(int id, const char *keywords)
{
    send(SCI_SETKEYWORDS, (uptr_t)id, (sptr_t)keywords);
}

void ScintillaEditor::SetSyntaxColor(int id, unsigned int color)
{
    send(SCI_STYLESETFORE, (uptr_t)id, color);
}

void ScintillaEditor::SetSyntaxFontStyle(int id, bool bold, bool italic)
{
    send(SCI_STYLESETBOLD, (uptr_t)id, bold);
    send(SCI_STYLESETITALIC, (uptr_t)id, italic);
}

void ScintillaEditor::SetLexerNormalText()
{
    SetLexer(SCLEX_NULL);
    send(SCI_STYLESETFORE, STYLE_DEFAULT, 0);
    send(SCI_STYLESETBACK, STYLE_DEFAULT, m_background_color);
    send(SCI_STYLESETBOLD, STYLE_DEFAULT, 0);
    send(SCI_STYLESETBOLD, STYLE_DEFAULT, 0);
    send(SCI_STYLECLEARALL);

    send(SCI_STYLESETFORE, STYLE_LINENUMBER, m_line_number_color);
    send(SCI_STYLESETFORE, STYLE_INDENTGUIDE, 0xf0bb8f);
}

ScintillaEditor::eEolMode ScintillaEditor::JudgeEolMode(const QString &str)
{
    int index = str.indexOf("\r\n");
    if (index >= 0)
        return EOL_CRLF;

    index = str.indexOf('\n');
    if (index >= 0)
        return EOL_LF;

    index = str.indexOf('\r');
    if (index >= 0)
        return EOL_CR;

    return EOL_CRLF;
}

//int ScintillaEditor::CharactorPosToBytePos(int pos, const wchar_t *str, size_t size)
//{

//}

//int ScintillaEditor::BytePosToCharactorPos(int pos, const char *str, size_t size)
//{

//}

void ScintillaEditor::SetFold()
{
    send(SCI_SETPROPERTY, (uptr_t)"fold", (sptr_t)"1");

    send(SCI_SETMARGINTYPEN, MARGIN_FOLD_INDEX, SC_MARGIN_SYMBOL);//页边类型
    send(SCI_SETMARGINMASKN, MARGIN_FOLD_INDEX, SC_MASK_FOLDERS); //页边掩码
    send(SCI_SETMARGINSENSITIVEN, MARGIN_FOLD_INDEX, 1); //响应鼠标消息

    // 折叠标签样式
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);

    // 折叠标签颜色
    send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB, 0xa0a0a0);
    send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL, 0xa0a0a0);
    send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL, 0xa0a0a0);

    send(SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_CONTRACTED, 0); //如果折叠就在折叠行的下面画一条横线

    //send(SCI_SETMARGINWIDTHN, MARGIN_FOLD_INDEX, std::min(GetLineHeight(), theApp.DPI(16)));
}

void ScintillaEditor::SetContextMenu(QMenu *pMenu, QWidget *pMenuOwner)
{
    if (pMenu != nullptr)
    {
        m_pMenu = pMenu;
        m_pContextMenuOwner = pMenuOwner;
        send(SCI_USEPOPUP, SC_POPUP_NEVER);
    }
}

int ScintillaEditor::Find(QString str, int start, int end)
{
    if (end < 0)
        end = GetDocLength();
    Sci_TextToFind ttf;
    ttf.chrg.cpMin = start;
    ttf.chrg.cpMax = end;
    QByteArray strData = str.toUtf8();
    ttf.lpstrText = strData.constData();
    return send(SCI_FINDTEXT, SCFIND_MATCHCASE, (sptr_t)&ttf);
}

void ScintillaEditor::GetLinePos(int line, int &start, int &end)
{
    start = send(SCI_POSITIONFROMLINE, (uptr_t)line);
    end = send(SCI_GETLINEENDPOSITION, (uptr_t)line);
    int doc_length = GetDocLength();
    if (start < 0 && start > doc_length)
        start = 0;
    if (end < 0 || end > doc_length)
        end = doc_length;
}

void ScintillaEditor::GetCurLinePos(int &start, int &end)
{
    int cur_pos = send(SCI_GETCURRENTPOS);
    int cur_line = send(SCI_LINEFROMPOSITION, (uptr_t)cur_pos);
    GetLinePos(cur_line, start, end);
}

bool ScintillaEditor::IsFullLineSelected()
{
    int start{}, end{};
    GetSel(start, end);
    if (start == end)
        return false;
    int first_line_colume = send(SCI_GETCOLUMN, (uptr_t)start);
    int last_line_colume = send(SCI_GETCOLUMN, (uptr_t)end);
    return first_line_colume == 0 && (last_line_colume == 0 || end == GetDocLength());
}

void ScintillaEditor::GetLineSelected(int &first_line, int &last_line)
{
    int start{}, end{};
    GetSel(start, end);
    first_line = send(SCI_LINEFROMPOSITION, (uptr_t)start);
    last_line = send(SCI_LINEFROMPOSITION, (uptr_t)end);
    if (send(SCI_GETCOLUMN, (uptr_t)end) == 0)       //如果最后一行没有字符，则不算该行
        last_line--;
}

int ScintillaEditor::GetLinesSelected()
{
    int first_line{}, last_line{};
    GetLineSelected(first_line, last_line);
    return last_line - first_line + 1;
}

void ScintillaEditor::GotoPos(int pos)
{
    send(SCI_GOTOPOS, (uptr_t)pos);
}

int ScintillaEditor::GetTextWidth(const std::string &str)
{
    return send(SCI_TEXTWIDTH, STYLE_DEFAULT, (sptr_t)str.c_str());
}
