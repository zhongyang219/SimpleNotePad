// LanguageSettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "LanguageSettingsDlg.h"
#include "afxdialogex.h"
#include "SimpleNotePadDlg.h"
#include "Common.h"


// CLanguageSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CLanguageSettingsDlg, CTabDlg)

CLanguageSettingsDlg::CLanguageSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_LANGUAGE_SETTING_DIALOG, pParent)
{
}

CLanguageSettingsDlg::~CLanguageSettingsDlg()
{
}

void CLanguageSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_language_list_box);
    DDX_Control(pDX, IDC_STYLE_LIST, m_style_list_box);
    DDX_Control(pDX, IDC_TEXT_COLOR_STATIC, m_text_color_static);
}


void CLanguageSettingsDlg::EnableControl()
{
    bool lan_enable = m_language_list_box.GetCurSel() >= 0;
    bool style_enable = m_style_list_box.GetCurSel() >= 0;
    m_style_list_box.EnableWindow(lan_enable);
    EnableDlgCtrl(IDC_USER_EXT_EDIT, lan_enable);
    EnableDlgCtrl(IDC_BOLD_CHECK, lan_enable && style_enable);
    EnableDlgCtrl(IDC_ITALIC_CHECK, lan_enable && style_enable);
    m_text_color_static.EnableWindow(lan_enable && style_enable);
    //EnableDlgCtrl(IDC_RESTORE_DEFAULT_BUTTON, lan_enable && style_enable);
}

void CLanguageSettingsDlg::CurStyleFromUI()
{
    int lan_index = m_language_list_box.GetCurSel();
    int style_index = m_style_list_box.GetCurSel();
    if (lan_index >= 0 && style_index >= 0)
    {
        CUserLanguage::SyntaxStyle user_style;
        user_style.bold = (IsDlgButtonChecked(IDC_BOLD_CHECK) != 0);
        user_style.italic = (IsDlgButtonChecked(IDC_ITALIC_CHECK) != 0);
        user_style.color = m_text_color_static.GetFillColor();
        CUserLanguage& user_lan = m_data.GetLanguage(m_cur_lan.GetString());
        user_lan.SetStyle(style_index, user_style);
    }
}

BEGIN_MESSAGE_MAP(CLanguageSettingsDlg, CTabDlg)
    ON_LBN_SELCHANGE(IDC_LIST1, &CLanguageSettingsDlg::OnLbnSelchangeList1)
    ON_LBN_SELCHANGE(IDC_STYLE_LIST, &CLanguageSettingsDlg::OnLbnSelchangeStyleList)
    ON_BN_CLICKED(IDC_BOLD_CHECK, &CLanguageSettingsDlg::OnBnClickedBoldCheck)
    ON_BN_CLICKED(IDC_ITALIC_CHECK, &CLanguageSettingsDlg::OnBnClickedItalicCheck)
    ON_MESSAGE(WM_COLOR_SELECTED, &CLanguageSettingsDlg::OnColorSelected)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CLanguageSettingsDlg::OnBnClickedRestoreDefaultButton)
    ON_EN_CHANGE(IDC_USER_EXT_EDIT, &CLanguageSettingsDlg::OnEnChangeUserExtEdit)
    ON_COMMAND(ID_RESTORE_SELECTED_STYLE, &CLanguageSettingsDlg::OnRestoreSelectedStyle)
    ON_COMMAND(ID_RESTORE_SELECTED_LANGUANGE, &CLanguageSettingsDlg::OnRestoreSelectedLanguange)
    ON_COMMAND(ID_RESTORE_ALL, &CLanguageSettingsDlg::OnRestoreAll)
    ON_WM_INITMENU()
END_MESSAGE_MAP()


// CLanguageSettingsDlg 消息处理程序


BOOL CLanguageSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_restore_default_menu.LoadMenu(IDR_RESTORE_DEFAULT_MENU);

    //初始化语言列表
    CSimpleNotePadDlg* main_wnd = dynamic_cast<CSimpleNotePadDlg*>(AfxGetMainWnd());
    for (const auto& lan : main_wnd->GetSyntaxHighlight().GetLanguageList())
    {
        m_language_list_box.AddString(lan.m_name.c_str());
    }

    EnableControl();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CLanguageSettingsDlg::OnLbnSelchangeList1()
{
    // TODO: 在此添加控件通知处理程序代码
    m_language_list_box.GetText(m_language_list_box.GetCurSel(), m_cur_lan);

    CSimpleNotePadDlg* main_wnd = CSimpleNotePadDlg::Instanse();
    //显示样式列表
    m_style_list_box.ResetContent();
    CLanguage cur_lan = main_wnd->GetSyntaxHighlight().FindLanguageByName(m_cur_lan.GetString());
    for (const auto& style_item : cur_lan.m_syntax_list)
    {
        m_style_list_box.AddString(style_item.name.c_str());
        m_style_list_box.SetItemData(m_style_list_box.GetCount() - 1, (DWORD_PTR)style_item.id);
    }

    //显示文件扩展名
    std::wstring str_ext;
    for (const auto& ext : cur_lan.m_ext)
    {
        str_ext += ext;
        str_ext.push_back(L' ');
    }
    if (!cur_lan.m_ext.empty())
        str_ext.pop_back();
    SetDlgItemText(IDC_DEFAULT_EXT_STATIC, str_ext.c_str());

    //显示自定义扩展名
    CUserLanguage& cur_user_lan = m_data.GetLanguage(m_cur_lan.GetString());
    SetDlgItemText(IDC_USER_EXT_EDIT, cur_user_lan.ExtListToString().c_str());

    EnableControl();
}


void CLanguageSettingsDlg::OnLbnSelchangeStyleList()
{
    // TODO: 在此添加控件通知处理程序代码
    CSimpleNotePadDlg* main_wnd = CSimpleNotePadDlg::Instanse();
    CLanguage cur_lan = main_wnd->GetSyntaxHighlight().FindLanguageByName(m_cur_lan.GetString());
    
    int style_index = m_style_list_box.GetCurSel();
    if (style_index >= 0 && style_index < cur_lan.m_syntax_list.size())
    {
        CUserLanguage user_lan = m_data.GetLanguage(m_cur_lan.GetString());
        if (user_lan.IsStyleExist(style_index))
        {
            CUserLanguage::SyntaxStyle user_style = user_lan.GetStyle(style_index);
            CheckDlgButton(IDC_BOLD_CHECK, user_style.bold);
            CheckDlgButton(IDC_ITALIC_CHECK, user_style.italic);
            m_text_color_static.SetFillColor(user_style.color);
        }
        else
        {
            CLanguage::SyntaxStyle style = cur_lan.m_syntax_list[style_index];
            CheckDlgButton(IDC_BOLD_CHECK, style.bold);
            CheckDlgButton(IDC_ITALIC_CHECK, style.italic);
            m_text_color_static.SetFillColor(style.color);
        }
    }

    EnableControl();
}


void CLanguageSettingsDlg::OnBnClickedBoldCheck()
{
    CurStyleFromUI();
}


void CLanguageSettingsDlg::OnBnClickedItalicCheck()
{
    CurStyleFromUI();
}


afx_msg LRESULT CLanguageSettingsDlg::OnColorSelected(WPARAM wParam, LPARAM lParam)
{
    CWnd* ctrl = (CWnd*)wParam;
    if (ctrl == &m_text_color_static)
    {
        CurStyleFromUI();
    }
    return 0;
}


void CLanguageSettingsDlg::OnBnClickedRestoreDefaultButton()
{
    CWnd* pBtn = GetDlgItem(IDC_RESTORE_DEFAULT_BUTTON);
    CPoint point;
    if (pBtn != nullptr)
    {
        CRect rect;
        pBtn->GetWindowRect(rect);
        point.x = rect.left;
        point.y = rect.bottom;
        m_restore_default_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}


void CLanguageSettingsDlg::OnEnChangeUserExtEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString str;
    GetDlgItemText(IDC_USER_EXT_EDIT, str);
    CUserLanguage& user_lan = m_data.GetLanguage(m_cur_lan.GetString());
    user_lan.ExtListFromString(str.GetString());
}


void CLanguageSettingsDlg::OnRestoreSelectedStyle()
{
    int style_index = m_style_list_box.GetCurSel();
    CUserLanguage& user_lan = m_data.GetLanguage(m_cur_lan.GetString());
    user_lan.RemoveStyle(style_index);

    CSimpleNotePadDlg* main_wnd = CSimpleNotePadDlg::Instanse();
    CLanguage cur_lan = main_wnd->GetSyntaxHighlight().FindLanguageByName(m_cur_lan.GetString());
    CLanguage::SyntaxStyle style = cur_lan.m_syntax_list[style_index];
    CheckDlgButton(IDC_BOLD_CHECK, style.bold);
    CheckDlgButton(IDC_ITALIC_CHECK, style.italic);
    m_text_color_static.SetFillColor(style.color);
}


void CLanguageSettingsDlg::OnRestoreSelectedLanguange()
{
    m_data.RemoveLanguage(m_cur_lan.GetString());

    m_style_list_box.SetCurSel(-1);
    EnableControl();
}


void CLanguageSettingsDlg::OnRestoreAll()
{
    m_data.RemoveAll();

    m_style_list_box.SetCurSel(-1);
    m_language_list_box.SetCurSel(-1);
    EnableControl();
}


void CLanguageSettingsDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    bool lan_enable = m_language_list_box.GetCurSel() >= 0;
    bool style_enable = m_style_list_box.GetCurSel() >= 0;
    pMenu->EnableMenuItem(ID_RESTORE_SELECTED_STYLE, MF_BYCOMMAND | (lan_enable && style_enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RESTORE_SELECTED_LANGUANGE, MF_BYCOMMAND | (lan_enable ? MF_ENABLED : MF_GRAYED));
}
