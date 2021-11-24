// FileRelateDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "FileRelateDlg.h"
#include "afxdialogex.h"
#include "RegFileRelate.h"
#include "SimpleNotePadDlg.h"


void IterateAllFormat(std::function<void(const std::wstring&, const std::wstring&)> func)
{
    CSimpleNotePadDlg* dlg = dynamic_cast<CSimpleNotePadDlg*>(AfxGetMainWnd());
    if (dlg != nullptr)
    {
        for (const auto& item : dlg->GetSyntaxHighlight().GetLanguageList())
        {
            for (const auto& ext : item.m_ext)
            {
                if (ext.find(L'$') == std::wstring::npos)
                    func(item.m_name, ext);
            }
        }
    }
    func(std::wstring(CCommon::LoadText(IDS_TEXT_FILE).GetString()), L"txt");
}



// CFileRelateDlg 对话框

IMPLEMENT_DYNAMIC(CFileRelateDlg, CTabDlg)

CFileRelateDlg::CFileRelateDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_FILE_RELATE_DIALOG, pParent)
{

}

CFileRelateDlg::~CFileRelateDlg()
{
}

void CFileRelateDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


void CFileRelateDlg::RefreshList()
{
    m_list_ctrl.DeleteAllItems();
    int index = 0;
    bool checked{ false }, unchecked{ false };
    CRegFileRelate reg_file;
    IterateAllFormat([&](const std::wstring& name, const std::wstring ext)
        {
            if (ext == L"cc")
            {
                int a = 0;
            }
            extensions[ext] = name;
        });
    std::vector<wstring> related_extensions;
    reg_file.GetAllRelatedExtensions(related_extensions);
    for (auto item : related_extensions)
    {
        if (!item.empty() && item[0] == L'.')
            item = item.substr(1);
        if (extensions.find(item) == extensions.end())
            extensions[item] = L"";
    }

    for (const auto& item : extensions)
    {
        //if(item == L"cue")
        //    continue;
        m_list_ctrl.InsertItem(index, item.first.c_str());
        wstring description = item.second;
        m_list_ctrl.SetItemText(index, 1, description.c_str());
        bool related = reg_file.IsFileTypeRelated(item.first.c_str());
        m_list_ctrl.SetCheck(index, related);
        if (related)
            checked = true;
        else
            unchecked = true;

        index++;
    }

    CButton* pBtn = (CButton*)GetDlgItem(IDC_SELECT_ALL_CHECK);
    if (pBtn != nullptr)
    {
        if (checked && !unchecked)
            pBtn->SetCheck(BST_CHECKED);
        else if (checked && unchecked)
            pBtn->SetCheck(BST_INDETERMINATE);
        else
            pBtn->SetCheck(BST_UNCHECKED);
    }
}


BEGIN_MESSAGE_MAP(CFileRelateDlg, CTabDlg)
    ON_BN_CLICKED(IDC_SELECT_ALL_CHECK, &CFileRelateDlg::OnBnClickedSelectAllCheck)
END_MESSAGE_MAP()


// CFileRelateDlg 消息处理程序

BOOL CFileRelateDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CRect rect;
    m_list_ctrl.GetWindowRect(rect);

    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
    int width0 = theApp.DPI(120);
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FORMAT), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_DESCRIPTION), LVCFMT_LEFT, width1);

    RefreshList();

    //m_list_ctrl.SetRowHeight(theApp.DPI(22));
    //m_list_ctrl.FillLeftSpaceAfterPaint(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFileRelateDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    int list_count = m_list_ctrl.GetItemCount();
    for (int i = 0; i < list_count; i++)
    {
        bool checked = m_list_ctrl.GetCheck(i) != FALSE;
        CRegFileRelate reg_file;
        if (checked)
        {
            CString file_ext = m_list_ctrl.GetItemText(i, 0);
            wstring description = extensions[wstring(file_ext)];

            reg_file.AddFileTypeRelate(file_ext, 1, false, description.c_str());
        }
        else
        {
            reg_file.DeleteFileTypeRelate(m_list_ctrl.GetItemText(i, 0));
        }
    }

    CTabDlg::OnOK();
}


void CFileRelateDlg::OnBnClickedSelectAllCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    CButton* pBtn = (CButton*)GetDlgItem(IDC_SELECT_ALL_CHECK);
    if (pBtn == nullptr)
        return;
    BOOL checked{ pBtn->GetCheck() };
    int list_count{ m_list_ctrl.GetItemCount() };
    for (int i = 0; i < list_count; i++)
        m_list_ctrl.SetCheck(i, checked);
}


//void CFileRelateDlg::OnBnClickedDefaultButton()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    std::set<wstring> default_selected;
//    for (auto item : CAudioCommon::m_all_surpported_extensions)
//    {
//        if (!item.empty() && item[0] == L'.')
//            item = item.substr(1);
//        if (item != L"mp1" && item != L"mp2" && item != L"mp4")
//            default_selected.insert(item);
//    }
//    int list_count{ m_list_ctrl.GetItemCount() };
//    for (int i = 0; i < list_count; i++)
//    {
//        wstring item_str = m_list_ctrl.GetItemText(i, 0).GetString();
//        auto iter = default_selected.find(item_str);
//        if (iter != default_selected.end())
//            m_list_ctrl.SetCheck(i, TRUE);
//        else
//            m_list_ctrl.SetCheck(i, FALSE);
//    }
//
//}
