#pragma once
#include "TabDlg.h"
#include "UserDefinedLanguageStyle.h"
#include "ColorStaticEx.h"

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

public:
    CUserDefinedLanguageStyle m_data;

private:
    CString m_cur_lan;
    CMenu m_restore_default_menu;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void EnableControl();
    void CurStyleFromUI();      //从界面获取当前样式设置

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CListBox m_language_list_box;
    CListBox m_style_list_box;
    CColorStaticEx m_text_color_static;
    afx_msg void OnLbnSelchangeList1();
    afx_msg void OnLbnSelchangeStyleList();
    afx_msg void OnBnClickedBoldCheck();
    afx_msg void OnBnClickedItalicCheck();
protected:
    afx_msg LRESULT OnColorSelected(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedRestoreDefaultButton();
    afx_msg void OnEnChangeUserExtEdit();
    afx_msg void OnRestoreSelectedStyle();
    afx_msg void OnRestoreSelectedLanguange();
    afx_msg void OnRestoreAll();
    afx_msg void OnInitMenu(CMenu* pMenu);
};
