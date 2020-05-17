// SettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"
#include "Common.h"
#include "InputDlg.h"


// CSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SETTINGS_DIALOG, pParent)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DEFAULT_CODE_PAGE_COMBO, m_default_page_code_combo);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_DEFAULT_CODE_PAGE_COMBO, &CSettingsDlg::OnCbnSelchangeDefaultCodePageCombo)
END_MESSAGE_MAP()


// CSettingsDlg 消息处理程序


BOOL CSettingsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化下拉列表
    for (size_t i{}; i < m_combo_list.size(); i++)
    {
        m_default_page_code_combo.AddString(m_combo_list[i].first);
    }
    m_default_page_code_combo.SetCurSel(m_data.default_code_page_selected);

    SetDlgItemText(IDC_CODE_PAGE_STATIC, std::to_wstring(m_data.default_code_page).c_str());

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CSettingsDlg::OnCbnSelchangeDefaultCodePageCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_default_page_code_combo.GetCurSel();
    m_data.default_code_page_selected = index;
    if (index >= 0 && index < m_combo_list.size())
        m_data.default_code_page = m_combo_list[index].second;

    if (index == m_combo_list.size() - 1)
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
