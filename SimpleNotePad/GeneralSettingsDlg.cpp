// SettingsDlg.cpp: 实现文件
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
    for (size_t i{}; i < CONST_VAL::code_page_list.size(); i++)
    {
        m_default_page_code_combo.AddString(CONST_VAL::code_page_list[i].name);
    }
    m_default_page_code_combo.SetCurSel(m_data.default_code_page_selected);

    SetDlgItemText(IDC_CODE_PAGE_STATIC, std::to_wstring(m_data.default_code_page).c_str());

    m_language_combo.AddString(CCommon::LoadText(IDS_FOLLOWING_SYSTEM));
    m_language_combo.AddString(_T("English"));
    m_language_combo.AddString(_T("简体中文"));
    m_language_combo.SetCurSel(static_cast<int>(m_data.language));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CGeneralSettingsDlg::OnCbnSelchangeDefaultCodePageCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_default_page_code_combo.GetCurSel();
    m_data.default_code_page_selected = index;
    if (index >= 0 && index < CONST_VAL::code_page_list.size())
        m_data.default_code_page = CONST_VAL::code_page_list[index].code_page;

    if (index == CONST_VAL::code_page_list.size() - 1)
    {
        CInputDlg inputDlg;
        inputDlg.SetTitle(_T("输入代码页"));
        inputDlg.SetInfoText(_T("请输入代码页："));
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
