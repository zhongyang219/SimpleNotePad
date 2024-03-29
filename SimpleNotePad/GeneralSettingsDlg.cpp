﻿// SettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "GeneralSettingsDlg.h"
#include "afxdialogex.h"
#include "Common.h"
#include "InputDlg.h"


// CSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CGeneralSettingsDlg, CTabDlg)

CGeneralSettingsDlg::CGeneralSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_SETTINGS_DIALOG, pParent)
{

}

CGeneralSettingsDlg::~CGeneralSettingsDlg()
{
}

void CGeneralSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DEFAULT_CODE_PAGE_COMBO, m_default_page_code_combo);
    DDX_Control(pDX, IDC_LANGUAGE_COMBO, m_language_combo);
    DDX_Control(pDX, IDC_DEFAULT_CODE_TYPE_COMBO, m_default_code_type_combo);
}


BEGIN_MESSAGE_MAP(CGeneralSettingsDlg, CTabDlg)
    ON_CBN_SELCHANGE(IDC_DEFAULT_CODE_PAGE_COMBO, &CGeneralSettingsDlg::OnCbnSelchangeDefaultCodePageCombo)
    ON_BN_CLICKED(IDC_GITHUB_RADIO, &CGeneralSettingsDlg::OnBnClickedGithubRadio)
    ON_BN_CLICKED(IDC_GITEE_RADIO, &CGeneralSettingsDlg::OnBnClickedGiteeRadio)
    ON_CBN_SELCHANGE(IDC_LANGUAGE_COMBO, &CGeneralSettingsDlg::OnCbnSelchangeLanguageCombo)
END_MESSAGE_MAP()


// CSettingsDlg 消息处理程序


BOOL CGeneralSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CheckDlgButton(IDC_CHECK_UPDATE_CHECK, m_data.check_update_when_start);
    if (m_data.update_source == 0)
        CheckDlgButton(IDC_GITHUB_RADIO, TRUE);
    else
        CheckDlgButton(IDC_GITEE_RADIO, TRUE);

    //初始化下拉列表
    for (size_t i{}; i < CONST_VAL->CodePageList().size(); i++)
    {
        m_default_page_code_combo.AddString(CONST_VAL->CodePageList()[i].name);
    }
    m_default_page_code_combo.SetCurSel(m_data.default_code_page_selected);

    SetDlgItemText(IDC_CODE_PAGE_STATIC, std::to_wstring(m_data.default_code_page).c_str());

    m_language_combo.AddString(CCommon::LoadText(IDS_FOLLOWING_SYSTEM));
    m_language_combo.AddString(_T("English"));
    m_language_combo.AddString(_T("简体中文"));
    m_language_combo.SetCurSel(static_cast<int>(m_data.language));

    m_default_code_type_combo.AddString(_T("ANSI"));
    m_default_code_type_combo.AddString(CCommon::LoadText(IDS_UTF8_BOM));
    m_default_code_type_combo.AddString(CCommon::LoadText(IDS_UTF8_NO_BOM));
    m_default_code_type_combo.AddString(_T("UTF16"));
    m_default_code_type_combo.AddString(_T("UTF16 Big Ending"));
    m_default_code_type_combo.SetCurSel(static_cast<int>(m_data.default_code));

    m_explorer_context_menu_exist = theApp.IsExplorerContextMenuExist();
    CheckDlgButton(IDC_EXCLORER_CONTEXT_MENU_CHECK, m_explorer_context_menu_exist);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CGeneralSettingsDlg::OnCbnSelchangeDefaultCodePageCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_default_page_code_combo.GetCurSel();
    m_data.default_code_page_selected = index;
    if (index >= 0 && index < CONST_VAL->CodePageList().size())
        m_data.default_code_page = CONST_VAL->CodePageList()[index].code_page;

    if (index == CONST_VAL->CodePageList().size() - 1)
    {
        CInputDlg inputDlg;
        inputDlg.SetTitle(CCommon::LoadText(IDS_INPUT_CODE_PAGE));
        inputDlg.SetInfoText(CCommon::LoadText(IDS_PLEASE_INPUT_CODE_PAGE));
        if (inputDlg.DoModal() == IDOK)
        {
            m_data.default_code_page = _ttoi(inputDlg.GetEditText().GetString());
        }
    }

    SetDlgItemText(IDC_CODE_PAGE_STATIC, std::to_wstring(m_data.default_code_page).c_str());
}


void CGeneralSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_data.check_update_when_start = (IsDlgButtonChecked(IDC_CHECK_UPDATE_CHECK) != 0);
    //获取语言的设置
    m_data.language = static_cast<Language>(m_language_combo.GetCurSel());
    if (m_data.language != theApp.GetGeneralSettings().language)
    {
        MessageBox(CCommon::LoadText(IDS_LANGUAGE_CHANGE_INFO), NULL, MB_ICONINFORMATION | MB_OK);
    }

    bool explorer_context_menu_checked = (IsDlgButtonChecked(IDC_EXCLORER_CONTEXT_MENU_CHECK) != 0);
    if (m_explorer_context_menu_exist != explorer_context_menu_checked)
    {
        if (explorer_context_menu_checked)
            theApp.AddExplorerContextMenuItem();
        else
            theApp.RemoveExplorerContextMenuItem();
    }

    m_data.default_code = static_cast<CodeType>(m_default_code_type_combo.GetCurSel());

    CTabDlg::OnOK();
}


void CGeneralSettingsDlg::OnBnClickedGithubRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = 0;
}


void CGeneralSettingsDlg::OnBnClickedGiteeRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = 1;
}


void CGeneralSettingsDlg::OnCbnSelchangeLanguageCombo()
{
    // TODO: 在此添加控件通知处理程序代码
}
