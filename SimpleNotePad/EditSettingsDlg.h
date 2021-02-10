#pragma once
#include "TabDlg.h"
#include "CommonData.h"
#include "ColorStaticEx.h"
#include "SpinEdit.h"

// CEditSettingsDlg 对话框

class CEditSettingsDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CEditSettingsDlg)

public:
	CEditSettingsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditSettingsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_SETTINGS_DIALOG };
#endif

public:
    EditSettingData m_data;

private:
    CColorStaticEx m_background_color_static;
    CSpinEdit m_font_size_edit;
    CColorStaticEx m_highlight_color_static;
    CSpinEdit m_tab_width_edit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
protected:
    afx_msg LRESULT OnColorSelected(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedChooseFontButton();
    afx_msg void OnBnClickedHexSetFontButton();
};
