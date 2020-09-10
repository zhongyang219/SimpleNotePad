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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeDefaultCodePageCombo();
};
