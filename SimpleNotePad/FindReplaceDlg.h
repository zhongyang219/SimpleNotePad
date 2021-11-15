#pragma once
#include "BaseDialog.h"
#include "FindReplaceTools.h"

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
        REPLACE_SELECTION
    };

    void SetFindString(LPCTSTR str);
    void SetReplaceString(LPCTSTR str);
    void SetInfoString(LPCTSTR str);
    void ClearInfoString();

private:
    Mode m_mode{};
    FindOption m_options;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual CString GetDialogName() const override;
    virtual void LoadConfig() override;
    virtual void SaveConfig() const override;

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedFindPreviousButton();
    afx_msg void OnBnClickedFindNextButton();
    afx_msg void OnBnClickedReplaceButton();
    afx_msg void OnBnClickedReplaceAllButton();
    afx_msg void OnBnClickedFindRadio();
    afx_msg void OnBnClickedReplaceRadio();
    afx_msg void OnEnChangeFindEdit();
    afx_msg void OnEnChangeReplaceEdit();
    afx_msg void OnBnClickedMatchWholeWordCheck();
    afx_msg void OnBnClickedMatchCaseCheck();
    afx_msg void OnBnClickedWrapAroundCheck();
    afx_msg void OnBnClickedFindModeNormalRadio();
    afx_msg void OnBnClickedFindModeExtendedRadio();
    afx_msg void OnBnClickedFindModeRegularExpRadio();
    virtual void OnCancel();
    virtual BOOL Create(CWnd* pParentWnd = NULL);
    afx_msg void OnBnClickedReplaceSelecteButton();
};
