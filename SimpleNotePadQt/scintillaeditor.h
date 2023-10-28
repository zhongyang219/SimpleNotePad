#ifndef SCINTILLAEDITOR_H
#define SCINTILLAEDITOR_H

#include "ScintillaEditBase.h"

#include <QMenu>
#define SCINTILLA_MARGIN_LINENUMBER 0
#define MARGIN_FOLD_INDEX 1

class ScintillaEditor : public ScintillaEditBase
{
public:
    ScintillaEditor(QWidget *parent = nullptr);

public:
    struct KeepCurrentLine
    {
        KeepCurrentLine(ScintillaEditor* view)
            : m_view(view)
        {
            //保存当前行
            current_line = m_view->GetFirstVisibleLine();
        }
        ~KeepCurrentLine()
        {
            //恢复当前行
            m_view->SetFirstVisibleLine(current_line);
        }

        ScintillaEditor* m_view{};
        int current_line{};
    };

    void SetText(const QString& text);
    void GetText(QString& text);
    const char* GetText(int& size);      //获取UTF8格式文本（返回字符串指针，需要自行释放内存）
    QString GetText(int start, int end);
    int GetDocLength();
    int GetLines();
    void SetFontFace(const QString& font_face);
    void SetFontSize(int font_size);
    void SetTabSize(int tab_size);
    void SetSel(int start, int end);     //设置选中范围（以字节为单位）
    void GetSel(int& start, int& end);     //获取选中范围（以字节为单位）
    int GetColumn();
    int GetRow();
    int GetSelCount();
    void SetBackgroundColor(unsigned int color);
    void SetReadOnly(bool read_only);
    bool IsReadOnly();
    void ShowIndentationGuides(bool show);  //显示缩进指示
    int GetCurrentIndex();       //获取光标位置
    QString GetSelectedText();  //获取UTF8格式的选中文本
    QPoint GetCursorPosition();   //获取光标的坐标
    char At(int index);         //获取指定位置的一个字符
    bool AutoSelectWord();      //在未选中任何文本状态下，自动选中光标处的一个单词

    void Undo();
    void Redo();
    void Cut();
    void Copy();
    void Paste();
    void Paste(const QString& text);
    void SelectAll();
    void EmptyUndoBuffer();     //清空撤销缓存

    void ReplaceSelection(const QString& replace_str);   //替换选中的字符串
    void InserText(const QString& str, int pos);
    void DeleteText(int pos, int length);

    enum eWordWrapMode
    {
        WW_WORD,
        WW_CHARACTER,
        WW_WHITESPACE
    };
    void SetWordWrap(bool word_wrap, eWordWrapMode mode = WW_WORD);

    bool IsEditChangeNotificationEnable();
    void SetEditChangeNotificationEnable(bool enable);

    //需要将多个操作标记为一组操作，并将它们撤销为一个操作时使用此结构体，
    //在对象构造时标记为操作开始，析构时标记为操作结束
    struct UndoRedoActionLocker
    {
        UndoRedoActionLocker(ScintillaEditor* _editor)
            : editor(_editor)
        {
            editor->send(SCI_BEGINUNDOACTION, 0, 0);
        }
        ~UndoRedoActionLocker()
        {
            editor->send(SCI_ENDUNDOACTION, 0, 0);
        }
    private:
        ScintillaEditor* editor;
    };

    bool CanUndo();
    bool CanRedo();
    bool CanPaste();
    bool IsSelectionEmpty();
    bool IsModified();
    void SetModified();
    void SetSavePoint();

    void SetLineNumberWidth(int width);
    void ShowLineNumber(bool show);
    void SetLineNumberColor(unsigned int color);

    int GetZoom();
    void SetZoom(int zoom);
    void Zoom(bool in);

    enum eEolMode
    {
        EOL_CRLF,
        EOL_CR,
        EOL_LF
    };
    void SetEolMode(eEolMode eolMode);
    eEolMode GetEolMode();

    void ConvertEolMode(eEolMode eolMode);

    void SetViewEol(bool show);

    int GetFirstVisibleLine();
    void SetFirstVisibleLine(int line);

    int GetLineHeight();

    void SetCurrentLineHighlight(bool highlight);   //设置当前行高亮
    void SetCurrentLineHighlightColor(unsigned int color);  //设置当前行高亮颜色

    void SetSelectionBackColor(unsigned int color);     //设置选中部分的背景色

    void GotoLine(int line);    //跳转到行

    //标记样式
    enum class MarkStyle
    {
        MARK_ALL = 1,       //使用“标记全部”功能时的标记样式
        SELECTION_MARK,     //标记相同单词的标记样式
        MATCHED_BRACKETS,      //匹配括号对的标记样式
        HTML_MARKS          //匹配的HTML标签标记样式
    };
    void SetMark(MarkStyle mark_style, int start, int length);
    void ClearMark(MarkStyle mark_style, int start, int length);
    void ClearAllMark(MarkStyle mark_style);

    //语法解析
    void SetLexer(int lexer);
    void SetKeywords(int id, const char* keywords);
    void SetSyntaxColor(int id, unsigned int color);
    void SetSyntaxFontStyle(int id, bool bold, bool italic);
    void SetLexerNormalText();

    static eEolMode JudgeEolMode(const QString& str);
//    static int CharactorPosToBytePos(int pos, const wchar_t* str, size_t size);     //将字符的位置转换成字节的位置（使用UTF8编码）
//    static int BytePosToCharactorPos(int pos, const char* str, size_t size);     //将字节的位置转换成字符的位置（使用UTF8编码）

    void SetFold();

    void SetContextMenu(QMenu* pMenu, QWidget* pMenuOwner);

    int Find(QString str, int start, int end = -1);     //在指定范围内查找一个字符串，返回位置
    void GetLinePos(int line, int& start, int& end);        //获取某一行的开始和结束位置
    void GetCurLinePos(int& start, int& end);               //获取当前行的开始和结束位置
    bool IsFullLineSelected();      //当前是否选中了整行
    void GetLineSelected(int& first_line, int& last_line);  //获取选中区域所在的行范围。first_line：选中区域的第一行，last_line：选中区域的最后一行
    int GetLinesSelected();     //获取选中的行数
    void GotoPos(int pos);

    int GetTextWidth(const std::string& str);

private:

private:
    bool m_change_notification_enable = true;      //如果为false，则不响应文本改变消息
    int m_line_number_width = 36;
    unsigned int m_line_number_color{};
    unsigned int m_background_color{ 0xffffff };
    unsigned int m_current_line_highlight_color{ 0xfdf3ea };

    QMenu* m_pMenu{};
    QWidget* m_pContextMenuOwner{};
    bool m_modified{};


};

#endif // SCINTILLAEDITOR_H
