#pragma once

#include "Scintilla/SciLexer.h"
#include "Scintilla/Scintilla.h"

// CScintillaEditView 视图

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
    void SetFontFace(const char* font_face);
    void SetFontFace(const wchar_t* font_face);
    void SetFontSize(int font_size);
    void SetTabSize(int tab_size);

    void Undo();
    void Redo();
    void Cut();
    void Copy();
    void Paste();
    void SelectAll();

    void SetWordWrap(bool word_wrap);

protected:
	DECLARE_MESSAGE_MAP()
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    afx_msg void OnPaint();
    virtual void PreSubclassWindow();
};


