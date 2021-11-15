// FindReplaceDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "FindReplaceDlg.h"
#include "Common.h"


// CFindReplaceDlg 对话框

IMPLEMENT_DYNAMIC(CFindReplaceDlg, CBaseDialog)

CFindReplaceDlg::CFindReplaceDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_FIND_REPLACE_DIALOG, pParent)
{

}

CFindReplaceDlg::~CFindReplaceDlg()
{
}

void CFindReplaceDlg::SetMode(Mode mode)
{
    m_mode = mode;
    if (m_mode == Mode::FIND)
    {
        ShowDlgCtrl(IDC_REPLACE_AS_STATIC, false);
        ShowDlgCtrl(IDC_REPLACE_EDIT, false);
        ShowDlgCtrl(IDC_REPLACE_BUTTON, false);
        ShowDlgCtrl(IDC_REPLACE_ALL_BUTTON, false);
        CheckDlgButton(IDC_FIND_RADIO, true);
        CheckDlgButton(IDC_REPLACE_RADIO, false);
        SetWindowText(CCommon::LoadText(IDS_FIND));
    }
    else if (m_mode == Mode::REPLACE)
    {
        ShowDlgCtrl(IDC_REPLACE_AS_STATIC, true);
        ShowDlgCtrl(IDC_REPLACE_EDIT, true);
        ShowDlgCtrl(IDC_REPLACE_BUTTON, true);
        ShowDlgCtrl(IDC_REPLACE_ALL_BUTTON, true);
        CheckDlgButton(IDC_FIND_RADIO, false);
        CheckDlgButton(IDC_REPLACE_RADIO, true);
        SetWindowText(CCommon::LoadText(IDS_REPLACE));
    }
}

void CFindReplaceDlg::SetFindString(LPCTSTR str)
{
    SetDlgItemText(IDC_FIND_EDIT, str);
}

void CFindReplaceDlg::SetReplaceString(LPCTSTR str)
{
    SetDlgItemText(IDC_REPLACE_EDIT, str);
}

void CFindReplaceDlg::SetInfoString(LPCTSTR str)
{
    SetDlgItemText(IDC_INFO_STATIC, str);
}

void CFindReplaceDlg::ClearInfoString()
{
    SetDlgItemText(IDC_INFO_STATIC, _T(""));
}

void CFindReplaceDlg::LoadConfig()
{
    CBaseDialog::LoadConfig();

    m_options.match_case = theApp.GetProfileInt(L"find_replace", L"match_case", 0);
    m_options.match_whole_word = theApp.GetProfileInt(L"find_replace", L"match_whole_word", 0);
    m_options.find_loop = theApp.GetProfileInt(L"find_replace", L"find_loop", 0);
    m_options.find_mode = static_cast<FindMode>(theApp.GetProfileInt(L"find_replace", L"find_mode", 0));
}

void CFindReplaceDlg::SaveConfig() const
{
    CBaseDialog::SaveConfig();

    theApp.WriteProfileInt(L"find_replace", L"match_case", m_options.match_case);
    theApp.WriteProfileInt(L"find_replace", L"match_whole_word", m_options.match_whole_word);
    theApp.WriteProfileInt(L"find_replace", L"find_loop", m_options.find_loop);
    theApp.WriteProfileInt(L"find_replace", L"find_mode", static_cast<int>(m_options.find_mode));
}

void CFindReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
}


CString CFindReplaceDlg::GetDialogName() const
{
    return _T("FindReplaceDlg");
}

BEGIN_MESSAGE_MAP(CFindReplaceDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_FIND_PREVIOUS_BUTTON, &CFindReplaceDlg::OnBnClickedFindPreviousButton)
    ON_BN_CLICKED(IDC_FIND_NEXT_BUTTON, &CFindReplaceDlg::OnBnClickedFindNextButton)
    ON_BN_CLICKED(IDC_REPLACE_BUTTON, &CFindReplaceDlg::OnBnClickedReplaceButton)
    ON_BN_CLICKED(IDC_REPLACE_ALL_BUTTON, &CFindReplaceDlg::OnBnClickedReplaceAllButton)
    ON_BN_CLICKED(IDC_FIND_RADIO, &CFindReplaceDlg::OnBnClickedFindRadio)
    ON_BN_CLICKED(IDC_REPLACE_RADIO, &CFindReplaceDlg::OnBnClickedReplaceRadio)
    ON_EN_CHANGE(IDC_FIND_EDIT, &CFindReplaceDlg::OnEnChangeFindEdit)
    ON_EN_CHANGE(IDC_REPLACE_EDIT, &CFindReplaceDlg::OnEnChangeReplaceEdit)
    ON_BN_CLICKED(IDC_MATCH_WHOLE_WORD_CHECK, &CFindReplaceDlg::OnBnClickedMatchWholeWordCheck)
    ON_BN_CLICKED(IDC_MATCH_CASE_CHECK, &CFindReplaceDlg::OnBnClickedMatchCaseCheck)
    ON_BN_CLICKED(IDC_WRAP_AROUND_CHECK, &CFindReplaceDlg::OnBnClickedWrapAroundCheck)
    ON_BN_CLICKED(IDC_FIND_MODE_NORMAL_RADIO, &CFindReplaceDlg::OnBnClickedFindModeNormalRadio)
    ON_BN_CLICKED(IDC_FIND_MODE_EXTENDED_RADIO, &CFindReplaceDlg::OnBnClickedFindModeExtendedRadio)
    ON_BN_CLICKED(IDC_FIND_MODE_REGULAR_EXP_RADIO, &CFindReplaceDlg::OnBnClickedFindModeRegularExpRadio)
END_MESSAGE_MAP()


// CFindReplaceDlg 消息处理程序


BOOL CFindReplaceDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CenterWindow();
    SetIcon(theApp.GetMenuIcon(IDI_FIND), FALSE);
    SetButtonIcon(IDC_FIND_PREVIOUS_BUTTON, theApp.GetMenuIcon(IDI_PREVIOUS));
    SetButtonIcon(IDC_FIND_NEXT_BUTTON, theApp.GetMenuIcon(IDI_NEXT));

    switch (m_options.find_mode)
    {
    case FindMode::NORMAL:
        CheckDlgButton(IDC_FIND_MODE_NORMAL_RADIO, true);
        break;
    case FindMode::EXTENDED:
        CheckDlgButton(IDC_FIND_MODE_EXTENDED_RADIO, true);
        break;
    case FindMode::REGULAR_EXPRESSION:
        CheckDlgButton(IDC_FIND_MODE_REGULAR_EXP_RADIO, true);
        break;
    default:
        break;
    }
    CheckDlgButton(IDC_MATCH_CASE_CHECK, m_options.match_case);
    CheckDlgButton(IDC_MATCH_WHOLE_WORD_CHECK, m_options.match_whole_word);
    CheckDlgButton(IDC_WRAP_AROUND_CHECK, m_options.find_loop);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFindReplaceDlg::OnBnClickedFindPreviousButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::FIND_PREVIOUS), 0);
}


void CFindReplaceDlg::OnBnClickedFindNextButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::FIND_NEXT), 0);
}


void CFindReplaceDlg::OnBnClickedReplaceButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::REPLACE), 0);
}


void CFindReplaceDlg::OnBnClickedReplaceAllButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::REPLACE_ALL), 0);
}


void CFindReplaceDlg::OnBnClickedFindRadio()
{
    SetMode(Mode::FIND);
}


void CFindReplaceDlg::OnBnClickedReplaceRadio()
{
    SetMode(Mode::REPLACE);
}


void CFindReplaceDlg::OnEnChangeFindEdit()
{
    CString str;
    GetDlgItemText(IDC_FIND_EDIT, str);
    m_options.find_str = str.GetString();
}


void CFindReplaceDlg::OnEnChangeReplaceEdit()
{
    CString str;
    GetDlgItemText(IDC_REPLACE_EDIT, str);
    m_options.replace_str = str.GetString();
}


void CFindReplaceDlg::OnBnClickedMatchWholeWordCheck()
{
    m_options.match_whole_word = (IsDlgButtonChecked(IDC_MATCH_WHOLE_WORD_CHECK) != 0);
}


void CFindReplaceDlg::OnBnClickedMatchCaseCheck()
{
    m_options.match_case = (IsDlgButtonChecked(IDC_MATCH_CASE_CHECK) != 0);
}


void CFindReplaceDlg::OnBnClickedWrapAroundCheck()
{
    m_options.find_loop = (IsDlgButtonChecked(IDC_WRAP_AROUND_CHECK) != 0);
}


void CFindReplaceDlg::OnBnClickedFindModeNormalRadio()
{
    m_options.find_mode = FindMode::NORMAL;
}


void CFindReplaceDlg::OnBnClickedFindModeExtendedRadio()
{
    m_options.find_mode = FindMode::EXTENDED;
}


void CFindReplaceDlg::OnBnClickedFindModeRegularExpRadio()
{
    m_options.find_mode = FindMode::REGULAR_EXPRESSION;
}


void CFindReplaceDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    CBaseDialog::OnCancel();
    //ShowWindow(SW_HIDE);
}


BOOL CFindReplaceDlg::Create(CWnd* pParentWnd)
{
    // TODO: 在此添加专用代码和/或调用基类

    return CBaseDialog::Create(IDD_FIND_REPLACE_DIALOG, pParentWnd);
}
