#pragma once
#include "Common.h"


// CSettingsDlg 对话框

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSettingsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS_DIALOG };
#endif

public:
    SettingsData m_data;

private:
    CComboBox m_default_page_code_combo;

    const vector<std::pair<CString, UINT>> m_combo_list{
    {_T("本地代码页"), CP_ACP},
    {_T("简体中文 (GB2312)"), CODE_PAGE_CHS},
    {_T("繁体中文 (Big5)"), CODE_PAGE_CHT},
    {_T("日文 (Shift-JIS)"), CODE_PAGE_JP},
    {_T("西欧语言 (Windows)"), CODE_PAGE_EN},
    {_T("韩文"), CODE_PAGE_KOR},
    {_T("泰文"), CODE_PAGE_THAI},
    {_T("越南文"), CODE_PAGE_VIET},
    {_T("手动指定代码页"), 0},
    };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeDefaultCodePageCombo();
};
