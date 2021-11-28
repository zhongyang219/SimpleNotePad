#pragma once
#include "BaseDialog.h"
#include "TabCtrlEx.h"
#include "GeneralSettingsDlg.h"
#include "EditSettingsDlg.h"
#include "FileRelateDlg.h"
#include "LanguageSettingsDlg.h"

// CSettingsDlg1 对话框

class CSettingsDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSettingsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS_DIALOG };
#endif

    //从外部获取选项设置数据
    void LoadSettings();

    //将对话框中的数据传送给外部
    void SaveSettings();

private:
    CTabCtrlEx m_tab_ctrl;
    CGeneralSettingsDlg m_general_settings_dlg;
    CEditSettingsDlg m_edit_settings_dlg;
    CFileRelateDlg m_file_relate_dlg;
    CLanguageSettingsDlg m_language_settings_dlg;
    static int m_tab_selected;
    std::vector<CTabDlg*> m_tab_vect;
    std::vector<int> m_tab_height;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

    virtual CString GetDialogName() const override;

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
