
// SimpleNotePadDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "Common.h"
#include "FileCompareDlg.h"
#include "EditEx.h"
#include "BaseDialog.h"
#include "ScintillaEditView.h"
#include "SyntaxHighlight.h"
#include <deque>
#include "FindReplaceDlg.h"
#include "SettingsDlg.h"

//const int WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);	//将FINDMSGSTRING注册为WM_FINDREPLACE消息

// CSimpleNotePadDlg 对话框
class CSimpleNotePadDlg : public CBaseDialog
{
    // 构造
public:
    CSimpleNotePadDlg(CString file_path = _T(""), CWnd* pParent = NULL);	// 标准构造函数
    ~CSimpleNotePadDlg();

    CScintillaEditView* GetEditView();
    const CSyntaxHighlight& GetSyntaxHighlight() const;

    static CSimpleNotePadDlg* Instanse();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SIMPLENOTEPAD_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
    HICON m_hIcon;
    //CMenu m_menu;	//菜单
    HACCEL m_hAccel;
    CScintillaEditView* m_view;     //Scintilla编辑控件
    CStatusBarCtrl m_status_bar;	//状态栏
    CMenu m_context_menu;

    CString m_file_path;	//当前打开的文件路径
    string m_edit_str;		//储存从文件载入的内容
    wstring m_edit_wcs;		//储存显示到窗口的内容

    CodeType m_code{ CodeType::ANSI };		//编码格式
    UINT m_code_page{ CP_ACP };				//当编码格式指定为ANSI时的代码页
    CodeType m_save_code{ CodeType::ANSI };	//另存为时的编码格式
    //bool m_flag{ false };			//一个flag，当调用m_edit.SetWindowText()函数将文本显示在文本框内时，将它置为true

    //状态栏各部分的定义
    enum StatusbarParts
    {
        SP_INFO,
        SP_POSITION_INDICATIOR,
        SP_LANGUAGE,
        SP_EOL_MODE,
        SP_ZOOM,
        SP_CODE_TYPE,
        STATUSBAR_MAX
    };

    int m_status_bar_hight;		//状态栏高度
    int m_edit_bottom_space;	//编辑窗口底部距窗口底部的距离

    bool m_always_on_top{ false };
    int m_cur_lan_index{ -1 };      //当前选择的语言索引
    bool m_monitor_mode{ false };
    unsigned __int64 m_last_modified_time{};

    CFindReplaceDialog* m_pFindDlg{};	//查找对话框
    CFindReplaceDialog* m_pReplaceDlg{};	//替换对话框

    CFindReplaceDlg m_find_replace_dlg;

    CSyntaxHighlight m_syntax_highlight;
    std::deque<std::wstring> m_clipboard_items;
    bool m_marked{ false };
    bool m_text_changed{ false };           //如果已收到文本更改消息，则为true
    bool m_text_changed_thread_exit{ false };   //如果为true，则退出响应文本改变的线程
    CWinThread* m_text_change_thread{};         //响应文本改变的线程

protected:
    void ApplySettings(const SettingsData& general_data, const EditSettingData& eidt_data, const CUserDefinedLanguageStyle& language_data);

    void OpenFile();					//打开文件的处理

    bool SaveFile(LPCTSTR file_path, CodeType code, UINT code_page = CP_ACP);	//保存文件的处理
    bool JudgeCode();				//判断编码格式（如果编码格式可以完全确定，则返回true，否则返回false）
    void UpdateStatusBarInfo();			//刷新状态栏
    void ChangeCode();				//更改编码格式时的处理
    bool BeforeChangeCode();		//在更改编码格式前的处理
    void ConvertToCode(CodeType code_type); //转换为一种编码格式
    virtual void SaveConfig() const override;				//保存设置
    virtual void LoadConfig() override;				//载入设置
    bool SaveInquiry(LPCTSTR info = NULL, int* prtn = nullptr);	//询问用户是否保存，参数为提示信息(用户点击了取消后返回false，否则返回true。rtn：对话框的返回值)
    void SetTitle();				//设置窗口标题
    void ShowStatusbar(bool show);  //显示或隐藏状态栏

    void InitStatusbarWidth();      //设置状态栏每部分的宽度

    bool _OnFileSave();		//如果已经执行了保存操作，则返回true，否则返回false
    bool _OnFileSaveAs();	//

    void SaveHex();
    //void SaveAsHex();

    void SetAlwaysOnTop();
    virtual CString GetDialogName() const override;

    void SetSyntaxHight(const CLanguage& lan);
    void SetEditorSyntaxHight();
    bool IsCommentEnable();     //当前语言的“添加/删除注释”功能是否可用

    //void CreateFontObject();

    //数据当前行号的位数自动设置行号的宽度
    void UpdateLineNumberWidth(bool update = false);

    //获取打开文件对话框中的筛选器
    CString GetOpenFileFilter();

    void InitMenuIcon();
    void AddItemToClipboardHistory(const std::wstring& str);
    void InitClipboardHistoryMenu();
    CMenu* GetClipboardHistoryMenu(bool context_menu);
    void AddClipboardDataToAllProcess(const std::wstring str);    //将一个剪贴板数据添加到所有SimpleNotePad窗口
    void UpdateOptionSettingsToAllProcess();        //通知所有进程更新选项设置
    void UpdateMenuSettingsToAllProcess();          //通知所有进程更新菜单中的设置

    void FillFindText();    //向查找替换对话框中的查找文本框填充选中文本

    static UINT TextChangeThreadCallback(LPVOID dwUser);

private:

protected:
    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnAppAbout();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileOpen();
    afx_msg void OnCodeAnsi();
    afx_msg void OnCodeUtf8();
    afx_msg void OnCodeUtf16();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnFormatFont();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnEditUndo();
    afx_msg void OnEditCut();
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
    afx_msg void OnEditSelectAll();
    afx_msg void OnHexView();
    //	afx_msg void OnDestroy();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnFileNew();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnFileCompare();
    afx_msg void OnWordWrap();
    afx_msg void OnClose();
    afx_msg void OnFind();
    afx_msg void OnFindPrivious();
    afx_msg void OnFindNext();
    afx_msg void OnMarkAll();
    afx_msg void OnReplace();
    afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnFormatConvert();
    afx_msg void OnAlwaysOnTop();
    afx_msg void OnCodePageChs();
    afx_msg void OnCodePageCht();
    afx_msg void OnCodePageJp();
    afx_msg void OnCodePageEn();
    afx_msg void OnCodePageKor();
    afx_msg void OnCodePageThai();
    afx_msg void OnCodePageViet();
    afx_msg void OnSepcifyCodePage();
    //afx_msg void OnCodePageLocal();
    afx_msg void OnToolOptions();
    afx_msg void OnCodeConvert();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    afx_msg void OnShowLineNumber();
    afx_msg void OnEolCrlf();
    afx_msg void OnEolCr();
    afx_msg void OnEolLf();
    afx_msg void OnShowEol();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnConvertToCapital();
    afx_msg void OnConvertToLowerCase();
    afx_msg void OnConvertToTitleCase();
    afx_msg void OnDestroy();
    afx_msg void OnFileOpenLocation();
    afx_msg void OnWordWrapWord();
    afx_msg void OnWordCharacter();
    afx_msg void OnWordWrapWhitespace();
    afx_msg void OnShowStatusbar();
    afx_msg void OnGotoLine();
    afx_msg void OnCodeUtf16be();
    afx_msg void OnConvertToAnsi();
    afx_msg void OnConvertToUtf8Bom();
    afx_msg void OnConvertToUtf8NoBom();
    afx_msg void OnConvertToUtf16();
    afx_msg void OnConvertToUtf16be();
protected:
    afx_msg LRESULT OnNpFindReplace(WPARAM wParam, LPARAM lParam);
public:
    afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
    afx_msg void OnAddDeleteComment();
    afx_msg void OnViewZoomIn();
    afx_msg void OnViewZoomOut();
    afx_msg void OnViewZoomDefault();
protected:
    afx_msg LRESULT OnDeleteChar(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnMonitorMode();
    afx_msg void OnFileReload();
};
