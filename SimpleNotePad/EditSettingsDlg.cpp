﻿// EditSettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "EditSettingsDlg.h"
#include "afxdialogex.h"
#include "Common.h"


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
    DDX_Control(pDX, IDC_FONE_SIZE_EDIT, m_font_size_edit);
    DDX_Control(pDX, IDC_HIGHLIGHT_COLOR_STATIC, m_highlight_color_static);
    DDX_Control(pDX, IDC_TAB_WIDTH_EDIT, m_tab_width_edit);
    DDX_Control(pDX, IDC_SELECTION_BACKGROUND_COLOR_STATIC, m_selection_back_color_static);
}


BEGIN_MESSAGE_MAP(CEditSettingsDlg, CTabDlg)
    ON_MESSAGE(WM_COLOR_SELECTED, &CEditSettingsDlg::OnColorSelected)
    ON_BN_CLICKED(IDC_CHOOSE_FONT_BUTTON, &CEditSettingsDlg::OnBnClickedChooseFontButton)
    ON_BN_CLICKED(IDC_HEX_SET_FONT_BUTTON, &CEditSettingsDlg::OnBnClickedHexSetFontButton)
END_MESSAGE_MAP()


// CEditSettingsDlg 消息处理程序


BOOL CEditSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CheckDlgButton(IDC_CURRENT_LINE_HIGHLIGHT_CHECK, m_data.current_line_highlight);
    m_background_color_static.SetFillColor(m_data.background_color);
    m_highlight_color_static.SetFillColor(m_data.current_line_highlight_color);
    m_selection_back_color_static.SetFillColor(m_data.selection_back_color);

    SetDlgItemText(IDC_FONE_NAME_EDIT, m_data.font_name);
    m_font_size_edit.SetRange(4, 72);
    m_font_size_edit.SetValue(m_data.font_size);

    m_tab_width_edit.SetRange(1, 16);
    m_tab_width_edit.SetValue(m_data.tab_width);

    CheckDlgButton(IDC_SHOW_INVISIBLE_CHARACTOR_CHECK, m_data.show_invisible_characters_hex);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CEditSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_data.current_line_highlight = (IsDlgButtonChecked(IDC_CURRENT_LINE_HIGHLIGHT_CHECK) != 0);
    GetDlgItemText(IDC_FONE_NAME_EDIT, m_data.font_name);
    m_data.font_size = m_font_size_edit.GetValue();
    m_data.tab_width = m_tab_width_edit.GetValue();

    m_data.show_invisible_characters_hex = (IsDlgButtonChecked(IDC_SHOW_INVISIBLE_CHARACTOR_CHECK) != 0);

    CTabDlg::OnOK();
}


afx_msg LRESULT CEditSettingsDlg::OnColorSelected(WPARAM wParam, LPARAM lParam)
{
    CColorStaticEx* pWnd = (CColorStaticEx*)wParam;
    COLORREF color_selected = (COLORREF)lParam;
    if (pWnd == &m_background_color_static)
    {
        m_data.background_color = color_selected;
    }
    else if (pWnd == &m_highlight_color_static)
    {
        m_data.current_line_highlight_color = color_selected;
    }
    else if (pWnd == &m_selection_back_color_static)
    {
        m_data.selection_back_color = color_selected;
    }
    return 0;
}


void CEditSettingsDlg::OnBnClickedChooseFontButton()
{
    // TODO: 在此添加控件通知处理程序代码
    LOGFONT lf{ 0 };
    CString font_name;
    int font_size;
    GetDlgItemText(IDC_FONE_NAME_EDIT, font_name);
    font_size = m_font_size_edit.GetValue();
    _tcscpy_s(lf.lfFaceName, LF_FACESIZE, font_name.GetString());
    lf.lfHeight = CCommon::FontSizeToLfHeight(font_size);
    CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        font_name = fontDlg.m_cf.lpLogFont->lfFaceName;
        font_size = fontDlg.m_cf.iPointSize / 10;
        //设置字体
        SetDlgItemText(IDC_FONE_NAME_EDIT, font_name);
        m_font_size_edit.SetValue(font_size);
    }
}


void CEditSettingsDlg::OnBnClickedHexSetFontButton()
{
    // TODO: 在此添加控件通知处理程序代码
    LOGFONT lf{ 0 };
    _tcscpy_s(lf.lfFaceName, LF_FACESIZE, m_data.font_name_hex.GetString());
    lf.lfHeight = CCommon::FontSizeToLfHeight(m_data.font_size_hex);
    CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        m_data.font_name_hex = fontDlg.m_cf.lpLogFont->lfFaceName;
        m_data.font_size_hex = fontDlg.m_cf.iPointSize / 10;
    }
}
