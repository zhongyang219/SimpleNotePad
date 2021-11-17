#pragma once
#include "BaseDialog.h"
#include "FindReplaceTools.h"
#include <deque>

// CFindReplaceDlg 对话框

class CFindReplaceDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CFindReplaceDlg)

public:
	CFindReplaceDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFindReplaceDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIND_REPLACE_DIALOG };
#endif

    //对话框模式
    enum class Mode
    {
        FIND,
        REPLACE
    };

    //设置对话框模式（查找还是替换）
    void SetMode(Mode mode);

    FindOption GetOptions() const { return m_options; }

    enum class Command
    {
        FIND_PREVIOUS,
        FIND_NEXT,
        REPLACE,
        REPLACE_ALL,
        REPLACE_SELECTION,
        MARK_ALL_CLEAR
    };

    void SetFindString(LPCTSTR str);
    void SetReplaceString(LPCTSTR str);
    void SetInfoString(LPCTSTR str);
    void ClearInfoString();

private:
    Mode m_mode{};
    FindOption m_options;
    std::deque<std::wstring> m_find_history;
    std::deque<std::wstring> m_replace_history;

    CComboBox m_find_combo;
    CComboBox m_replace_combo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual CString GetDialogName() const override;
    virtual void LoadConfig() override;
    virtual void SaveConfig() const override;
    bool AppendStringToHistory(std::deque<std::wstring>& history, const std::wstring& str);
    void AppendFindStringToHistory();
    void AppendReplaceStringToHistory();
    void InitFindCombo();
    void InitReplaceCombo();
    static void ClearComboboxItems(CComboBox& combobox);

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedFindPreviousButton();
    afx_msg void OnBnClickedFindNextButton();
    afx_msg void OnBnClickedReplaceButton();
    afx_msg void OnBnClickedReplaceAllButton();
    afx_msg void OnBnClickedFindRadio();
    afx_msg void OnBnClickedReplaceRadio();
    afx_msg void OnBnClickedMatchWholeWordCheck();
    afx_msg void OnBnClickedMatchCaseCheck();
    afx_msg void OnBnClickedWrapAroundCheck();
    afx_msg void OnBnClickedFindModeNormalRadio();
    afx_msg void OnBnClickedFindModeExtendedRadio();
    afx_msg void OnBnClickedFindModeRegularExpRadio();
    virtual BOOL Create(CWnd* pParentWnd = NULL);
    afx_msg void OnBnClickedReplaceSelecteButton();
    afx_msg void OnCbnEditchangeFindCombo();
    afx_msg void OnCbnEditchangeReplaceCombo();
    afx_msg void OnCbnSelchangeFindCombo();
    afx_msg void OnCbnSelchangeReplaceCombo();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnBnClickedMarkAllButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnOK();
};
