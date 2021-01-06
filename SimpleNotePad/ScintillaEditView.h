#pragma once

#include "../scintilla/include/SciLexer.h"
#include "../scintilla/include/Scintilla.h"

// CScintillaEditView 视图

#define SCINTILLA_MARGIN_LINENUMBER 0

class CScintillaEditView : public CView
{
	DECLARE_DYNCREATE(CScintillaEditView)

protected:
	CScintillaEditView();           // 动态创建所使用的受保护的构造函数
	virtual ~CScintillaEditView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

    void SetText(const wstring& text);
    void GetText(wstring& text);
    void SetFontFace(const wchar_t* font_face);
    void SetFontSize(int font_size);
    void SetTabSize(int tab_size);
    void SetSel(int start, int end);        //设置选中范围（位置以字节为单位而非字符为单位）

    void Undo();
    void Redo();
    void Cut();
    void Copy();
    void Paste();
    void SelectAll();
    void EmptyUndoBuffer();     //清空撤销缓存

    void SetWordWrap(bool word_wrap);

    bool IsEditChangeNotificationEnable();

    bool CanUndo();
    bool CanRedo();
    bool CanPaste();
    bool IsSelectionEmpty();
    bool IsModified();
    void SetSavePoint();

    void SetLineNumberWidth(int width);
    void ShowLineNumber(bool show);
    void SetLineNumberColor(COLORREF color);

    int GetZoom();
    void SetZoom(int zoom);

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

    //语法解析
    void SetLexer(int lexer);
    void SetKeywords(int id, const char* keywords);
    void SetSyntaxColor(int id, COLORREF color);

    static eEolMode JudgeEolMode(const wstring& str);

private:

private:
    bool m_change_notification_enable = true;      //如果为false，则不响应文本改变消息
    int m_line_number_width = 36;

protected:
	DECLARE_MESSAGE_MAP()
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    afx_msg void OnPaint();
    virtual void PreSubclassWindow();
    virtual void OnInitialUpdate();
};


