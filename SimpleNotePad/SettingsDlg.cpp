// SettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"

// CSettingsDlg 对话框

int CSettingsDlg::m_tab_selected = 0;

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
    m_language_settings_dlg.m_data = theApp.GetLanguageSettings();
}

void CSettingsDlg::SaveSettings()
{
    theApp.SetGeneralSettings(m_general_settings_dlg.m_data);
    theApp.SetEditSettings(m_edit_settings_dlg.m_data);
    theApp.SetLanguageSettings(m_language_settings_dlg.m_data);
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
    ON_WM_DESTROY()
    ON_WM_SIZE()
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
    m_file_relate_dlg.Create(IDD_FILE_RELATE_DIALOG);
    m_language_settings_dlg.Create(IDD_LANGUAGE_SETTING_DIALOG);

    //保存子对话框
    m_tab_vect.push_back(&m_general_settings_dlg);
    m_tab_vect.push_back(&m_edit_settings_dlg);
    m_tab_vect.push_back(&m_file_relate_dlg);
    m_tab_vect.push_back(&m_language_settings_dlg);

    //获取子对话框的初始高度
    for (const auto* pDlg : m_tab_vect)
    {
        CRect rect;
        pDlg->GetWindowRect(rect);
        m_tab_height.push_back(rect.Height());
    }

    //添加子对话框
    m_tab_ctrl.AddWindow(&m_general_settings_dlg, CCommon::LoadText(IDS_GENERAL_SETTINGS));
    m_tab_ctrl.AddWindow(&m_edit_settings_dlg, CCommon::LoadText(IDS_EDITOR_SETTINGS));
    m_tab_ctrl.AddWindow(&m_file_relate_dlg, CCommon::LoadText(IDS_FILE_RELATE));
    m_tab_ctrl.AddWindow(&m_language_settings_dlg, CCommon::LoadText(IDS_LANGUAGE_SETTINGS));

    //为每个子窗口设置滚动信息
    for (size_t i = 0; i < m_tab_vect.size(); i++)
    {
        m_tab_vect[i]->SetScrollbarInfo(m_tab_ctrl.m_tab_rect.Height(), m_tab_height[i]);
    }

    if (m_tab_selected < 0 || m_tab_selected >= m_tab_ctrl.GetItemCount())
        m_tab_selected = 0;
    m_tab_ctrl.SetCurTab(m_tab_selected);


    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_general_settings_dlg.OnOK();
    m_edit_settings_dlg.OnOK();
    m_file_relate_dlg.OnOK();
    m_language_settings_dlg.OnOK();

    CBaseDialog::OnOK();
}


void CSettingsDlg::OnDestroy()
{
    CBaseDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    m_tab_selected = m_tab_ctrl.GetCurSel();
}


void CSettingsDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    if (nType != SIZE_MINIMIZED)
    {
        //为每个子窗口设置滚动信息
        for (size_t i = 0; i < m_tab_vect.size(); i++)
        {
            m_tab_vect[i]->ResetScroll();
            m_tab_vect[i]->SetScrollbarInfo(m_tab_ctrl.m_tab_rect.Height(), m_tab_height[i]);
        }
    }
}
