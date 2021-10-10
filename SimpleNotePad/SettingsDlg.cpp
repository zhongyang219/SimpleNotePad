// SettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"

// CSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CSettingsDlg, CBaseDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_SETTINGS_DIALOG, pParent)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::LoadSettings()
{
    m_general_settings_dlg.m_data = theApp.GetGeneralSettings();
    m_edit_settings_dlg.m_data = theApp.GetEditSettings();
}

void CSettingsDlg::SaveSettings()
{
    theApp.SetGeneralSettings(m_general_settings_dlg.m_data);
    theApp.SetEditSettings(m_edit_settings_dlg.m_data);
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab_ctrl);
}


CString CSettingsDlg::GetDialogName() const
{
    return _T("SettingsDlg");
}

BEGIN_MESSAGE_MAP(CSettingsDlg, CBaseDialog)
END_MESSAGE_MAP()


// CSettingsDlg 消息处理程序


BOOL CSettingsDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(theApp.GetMenuIcon(IDI_SETTINGS), FALSE);		// 设置小图标

    //创建子对话框
    m_general_settings_dlg.Create(IDD_GENERAL_SETTINGS_DIALOG);
    m_edit_settings_dlg.Create(IDD_EDIT_SETTINGS_DIALOG);

    //添加子对话框
    m_tab_ctrl.AddWindow(&m_general_settings_dlg, _T("常规设置"));
    m_tab_ctrl.AddWindow(&m_edit_settings_dlg, _T("编辑器设置"));

    m_tab_ctrl.SetCurTab(1);


    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_general_settings_dlg.OnOK();
    m_edit_settings_dlg.OnOK();

    CBaseDialog::OnOK();
}
