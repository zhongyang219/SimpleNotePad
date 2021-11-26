#pragma once
#include "TabDlg.h"

// CLanguageSettingsDlg 对话框

class CLanguageSettingsDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CLanguageSettingsDlg)

public:
	CLanguageSettingsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLanguageSettingsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LANGUAGE_SETTING_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
