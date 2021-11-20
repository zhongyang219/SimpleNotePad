// FindReplaceDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "FindReplaceDlg.h"
#include "Common.h"
#include "SimpleNotePadDlg.h"

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
        ShowDlgCtrl(IDC_REPLACE_COMBO, false);
        ShowDlgCtrl(IDC_REPLACE_BUTTON, false);
        ShowDlgCtrl(IDC_REPLACE_ALL_BUTTON, false);
        ShowDlgCtrl(IDC_REPLACE_SELECTE_BUTTON, false);
        ShowDlgCtrl(IDC_MARK_ALL_BUTTON, true);
        CheckDlgButton(IDC_FIND_RADIO, true);
        CheckDlgButton(IDC_REPLACE_RADIO, false);
        SetWindowText(CCommon::LoadText(IDS_FIND));
        SetIcon(theApp.GetMenuIcon(IDI_FIND), FALSE);
    }
    else if (m_mode == Mode::REPLACE)
    {
        ShowDlgCtrl(IDC_REPLACE_AS_STATIC, true);
        ShowDlgCtrl(IDC_REPLACE_COMBO, true);
        ShowDlgCtrl(IDC_REPLACE_BUTTON, true);
        ShowDlgCtrl(IDC_REPLACE_ALL_BUTTON, true);
        ShowDlgCtrl(IDC_REPLACE_SELECTE_BUTTON, true);
        ShowDlgCtrl(IDC_MARK_ALL_BUTTON, false);
        CheckDlgButton(IDC_FIND_RADIO, false);
        CheckDlgButton(IDC_REPLACE_RADIO, true);
        SetWindowText(CCommon::LoadText(IDS_REPLACE));
        SetIcon(theApp.GetMenuIcon(IDI_REPLACE), FALSE);
    }
    m_find_combo.SetFocus();
}

void CFindReplaceDlg::SetFindString(LPCTSTR str)
{
    SetDlgItemText(IDC_FIND_COMBO, str);
    m_options.find_str = str;
    EnableControl();
}

void CFindReplaceDlg::SetReplaceString(LPCTSTR str)
{
    SetDlgItemText(IDC_REPLACE_COMBO, str);
    m_options.replace_str = str;
    EnableControl();
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
    std::vector<std::wstring> temp;
    theApp.GetStringList(L"find_replace", L"find_history", temp);
    for (const auto& str : temp)
        m_find_history.push_back(str);
    theApp.GetStringList(L"find_replace", L"replace_history", temp);
    for (const auto& str : temp)
        m_replace_history.push_back(str);
}

void CFindReplaceDlg::SaveConfig() const
{
    CBaseDialog::SaveConfig();

    theApp.WriteProfileInt(L"find_replace", L"match_case", m_options.match_case);
    theApp.WriteProfileInt(L"find_replace", L"match_whole_word", m_options.match_whole_word);
    theApp.WriteProfileInt(L"find_replace", L"find_loop", m_options.find_loop);
    theApp.WriteProfileInt(L"find_replace", L"find_mode", static_cast<int>(m_options.find_mode));
    std::vector<std::wstring> temp;
    for (const auto& str : m_find_history)
        temp.push_back(str);
    theApp.WriteStringList(L"find_replace", L"find_history", temp);
    temp.clear();
    for (const auto& str : m_replace_history)
        temp.push_back(str);
    theApp.WriteStringList(L"find_replace", L"replace_history", temp);
}

bool CFindReplaceDlg::AppendStringToHistory(std::deque<std::wstring>& history, const std::wstring& str)
{
    if (!str.empty())
    {
        auto iter = std::find(history.begin(), history.end(), str);
        if (iter != history.end())  //如果文本已存在，则将它删除
            history.erase(iter);
        history.push_front(str);
        if (history.size() > FIND_REPLACE_HISTORY_MAX)
            history.pop_back();
        return true;
    }
    return false;
}

void CFindReplaceDlg::AppendFindStringToHistory()
{
    AppendStringToHistory(m_find_history, m_options.find_str);
    InitFindCombo();
}

void CFindReplaceDlg::AppendReplaceStringToHistory()
{
    AppendStringToHistory(m_replace_history, m_options.replace_str);
    InitReplaceCombo();
}

void CFindReplaceDlg::InitFindCombo()
{
    ClearComboboxItems(m_find_combo);
    for (const auto& str : m_find_history)
        m_find_combo.AddString(str.c_str());
}

void CFindReplaceDlg::InitReplaceCombo()
{
    ClearComboboxItems(m_replace_combo);
    for (const auto& str : m_replace_history)
        m_replace_combo.AddString(str.c_str());
}

void CFindReplaceDlg::ClearComboboxItems(CComboBox& combobox)
{
    CString str;
    combobox.GetWindowText(str);
    combobox.ResetContent();
    combobox.SetWindowText(str);
}

void CFindReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FIND_COMBO, m_find_combo);
    DDX_Control(pDX, IDC_REPLACE_COMBO, m_replace_combo);
}


CString CFindReplaceDlg::GetDialogName() const
{
    return _T("FindReplaceDlg");
}

void CFindReplaceDlg::EnableControl()
{
    bool find_enable{ !m_options.find_str.empty() };

    EnableDlgCtrl(IDC_FIND_PREVIOUS_BUTTON, find_enable);
    EnableDlgCtrl(IDC_FIND_NEXT_BUTTON, find_enable);
    EnableDlgCtrl(IDC_MARK_ALL_BUTTON, find_enable);

    CSimpleNotePadDlg* main_window = dynamic_cast<CSimpleNotePadDlg*>(theApp.m_pMainWnd);
    CScintillaEditView* edit_view = main_window->GetEditView();
    bool select_valid = !edit_view->IsSelectionEmpty();
    EnableDlgCtrl(IDC_REPLACE_BUTTON, find_enable || select_valid);
    EnableDlgCtrl(IDC_REPLACE_ALL_BUTTON, find_enable);
    EnableDlgCtrl(IDC_REPLACE_SELECTE_BUTTON, find_enable && select_valid);

    EnableDlgCtrl(IDC_MATCH_WHOLE_WORD_CHECK, m_options.find_mode != FindMode::REGULAR_EXPRESSION);
}

BEGIN_MESSAGE_MAP(CFindReplaceDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_FIND_PREVIOUS_BUTTON, &CFindReplaceDlg::OnBnClickedFindPreviousButton)
    ON_BN_CLICKED(IDC_FIND_NEXT_BUTTON, &CFindReplaceDlg::OnBnClickedFindNextButton)
    ON_BN_CLICKED(IDC_REPLACE_BUTTON, &CFindReplaceDlg::OnBnClickedReplaceButton)
    ON_BN_CLICKED(IDC_REPLACE_ALL_BUTTON, &CFindReplaceDlg::OnBnClickedReplaceAllButton)
    ON_BN_CLICKED(IDC_FIND_RADIO, &CFindReplaceDlg::OnBnClickedFindRadio)
    ON_BN_CLICKED(IDC_REPLACE_RADIO, &CFindReplaceDlg::OnBnClickedReplaceRadio)
    ON_BN_CLICKED(IDC_MATCH_WHOLE_WORD_CHECK, &CFindReplaceDlg::OnBnClickedMatchWholeWordCheck)
    ON_BN_CLICKED(IDC_MATCH_CASE_CHECK, &CFindReplaceDlg::OnBnClickedMatchCaseCheck)
    ON_BN_CLICKED(IDC_WRAP_AROUND_CHECK, &CFindReplaceDlg::OnBnClickedWrapAroundCheck)
    ON_BN_CLICKED(IDC_FIND_MODE_NORMAL_RADIO, &CFindReplaceDlg::OnBnClickedFindModeNormalRadio)
    ON_BN_CLICKED(IDC_FIND_MODE_EXTENDED_RADIO, &CFindReplaceDlg::OnBnClickedFindModeExtendedRadio)
    ON_BN_CLICKED(IDC_FIND_MODE_REGULAR_EXP_RADIO, &CFindReplaceDlg::OnBnClickedFindModeRegularExpRadio)
    ON_BN_CLICKED(IDC_REPLACE_SELECTE_BUTTON, &CFindReplaceDlg::OnBnClickedReplaceSelecteButton)
    ON_CBN_EDITCHANGE(IDC_FIND_COMBO, &CFindReplaceDlg::OnCbnEditchangeFindCombo)
    ON_CBN_EDITCHANGE(IDC_REPLACE_COMBO, &CFindReplaceDlg::OnCbnEditchangeReplaceCombo)
    ON_CBN_SELCHANGE(IDC_FIND_COMBO, &CFindReplaceDlg::OnCbnSelchangeFindCombo)
    ON_CBN_SELCHANGE(IDC_REPLACE_COMBO, &CFindReplaceDlg::OnCbnSelchangeReplaceCombo)
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_MARK_ALL_BUTTON, &CFindReplaceDlg::OnBnClickedMarkAllButton)
END_MESSAGE_MAP()


// CFindReplaceDlg 消息处理程序


BOOL CFindReplaceDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CenterWindow();
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

    InitFindCombo();
    InitReplaceCombo();

    m_find_combo.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFindReplaceDlg::OnBnClickedFindPreviousButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::FIND_PREVIOUS), 0);
    AppendFindStringToHistory();
}


void CFindReplaceDlg::OnBnClickedFindNextButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::FIND_NEXT), 0);
    AppendFindStringToHistory();
}


void CFindReplaceDlg::OnBnClickedReplaceButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::REPLACE), 0);
    AppendFindStringToHistory();
    AppendReplaceStringToHistory();
}


void CFindReplaceDlg::OnBnClickedReplaceAllButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::REPLACE_ALL), 0);
    AppendFindStringToHistory();
    AppendReplaceStringToHistory();
}


void CFindReplaceDlg::OnBnClickedFindRadio()
{
    SetMode(Mode::FIND);
}


void CFindReplaceDlg::OnBnClickedReplaceRadio()
{
    SetMode(Mode::REPLACE);
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
    EnableControl();
}


void CFindReplaceDlg::OnBnClickedFindModeExtendedRadio()
{
    m_options.find_mode = FindMode::EXTENDED;
    EnableControl();
}


void CFindReplaceDlg::OnBnClickedFindModeRegularExpRadio()
{
    m_options.find_mode = FindMode::REGULAR_EXPRESSION;
    EnableControl();
}


BOOL CFindReplaceDlg::Create(CWnd* pParentWnd)
{
    // TODO: 在此添加专用代码和/或调用基类

    return CBaseDialog::Create(IDD_FIND_REPLACE_DIALOG, pParentWnd);
}


void CFindReplaceDlg::OnBnClickedReplaceSelecteButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::REPLACE_SELECTION), 0);
    AppendFindStringToHistory();
    AppendReplaceStringToHistory();
}


void CFindReplaceDlg::OnCbnEditchangeFindCombo()
{
    CString str;
    m_find_combo.GetWindowText(str);
    m_options.find_str = str.GetString();
    EnableControl();
}


void CFindReplaceDlg::OnCbnEditchangeReplaceCombo()
{
    CString str;
    m_replace_combo.GetWindowText(str);
    m_options.replace_str = str.GetString();
    EnableControl();
}


void CFindReplaceDlg::OnCbnSelchangeFindCombo()
{
    CString str;
    m_find_combo.GetLBText(m_find_combo.GetCurSel(), str);
    m_options.find_str = str.GetString();
    EnableControl();
}


void CFindReplaceDlg::OnCbnSelchangeReplaceCombo()
{
    CString str;
    m_replace_combo.GetLBText(m_replace_combo.GetCurSel(), str);
    m_options.replace_str = str.GetString();
    EnableControl();
}


void CFindReplaceDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CBaseDialog::OnShowWindow(bShow, nStatus);

    m_find_combo.SetFocus();
}


void CFindReplaceDlg::OnBnClickedMarkAllButton()
{
    theApp.m_pMainWnd->SendMessage(WM_NP_FIND_REPLACE, static_cast<WPARAM>(Command::MARK_ALL_CLEAR), 0);
    AppendFindStringToHistory();
}


BOOL CFindReplaceDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN)
    {
        //按下Ctrl键时
        if (GetKeyState(VK_CONTROL) & 0x80)
        {
            if (pMsg->wParam == 'F')
            {
                SetMode(Mode::FIND);
                return TRUE;
            }
            else if (pMsg->wParam == 'H')
            {
                SetMode(Mode::REPLACE);
                return TRUE;
            }
            else if (pMsg->wParam == 'M')
            {
                OnBnClickedMarkAllButton();
                return TRUE;
            }
        }
        //按下Shift键时
        else if (GetKeyState(VK_SHIFT) & 0x8000)
        {
            if (pMsg->wParam == VK_F3)
            {
                OnBnClickedFindPreviousButton();
                return TRUE;
            }
        }

        else
        {
            if (pMsg->wParam == VK_F3)
            {
                OnBnClickedFindNextButton();
                return TRUE;
            }
        }
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}


void CFindReplaceDlg::OnOK()
{
    OnBnClickedFindNextButton();
    //CBaseDialog::OnOK();
}
