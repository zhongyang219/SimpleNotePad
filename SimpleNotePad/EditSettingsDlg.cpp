// EditSettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "EditSettingsDlg.h"
#include "afxdialogex.h"


// CEditSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CEditSettingsDlg, CTabDlg)

CEditSettingsDlg::CEditSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_EDIT_SETTINGS_DIALOG, pParent)
{

}

CEditSettingsDlg::~CEditSettingsDlg()
{
}

void CEditSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BACKGROUND_COLOR_STATIC, m_background_color_static);
}


BEGIN_MESSAGE_MAP(CEditSettingsDlg, CTabDlg)
    ON_MESSAGE(WM_COLOR_SELECTED, &CEditSettingsDlg::OnColorSelected)
END_MESSAGE_MAP()


// CEditSettingsDlg 消息处理程序


BOOL CEditSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CheckDlgButton(IDC_CURRENT_LINE_HIGHLIGHT_CHECK, m_data.current_line_highlight);
    m_background_color_static.SetFillColor(m_data.background_color);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CEditSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_data.current_line_highlight = (IsDlgButtonChecked(IDC_CURRENT_LINE_HIGHLIGHT_CHECK) != 0);

    CTabDlg::OnOK();
}


afx_msg LRESULT CEditSettingsDlg::OnColorSelected(WPARAM wParam, LPARAM lParam)
{
    COLORREF color_selected = (COLORREF)lParam;
    m_data.background_color = color_selected;
    return 0;
}
