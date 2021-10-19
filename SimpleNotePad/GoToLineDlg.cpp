// GoToLineDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "GoToLineDlg.h"
#include "afxdialogex.h"


// CGoToLineDlg 对话框

IMPLEMENT_DYNAMIC(CGoToLineDlg, CDialog)

CGoToLineDlg::CGoToLineDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_GTO_DIALOG, pParent)
{

}

CGoToLineDlg::~CGoToLineDlg()
{
}

int CGoToLineDlg::GetLine()
{
    return m_line;
}

void CGoToLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGoToLineDlg, CDialog)
END_MESSAGE_MAP()


// CGoToLineDlg 消息处理程序


void CGoToLineDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    CString str;
    GetDlgItemText(IDC_EDIT1, str);
    m_line = _ttoi(str.GetString());

    CDialog::OnOK();
}


BOOL CGoToLineDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CWnd* pEdit = GetDlgItem(IDC_EDIT1);
    if (pEdit != nullptr)
        pEdit->SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
