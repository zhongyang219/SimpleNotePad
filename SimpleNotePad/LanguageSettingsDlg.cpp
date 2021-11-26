// LanguageSettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "LanguageSettingsDlg.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(CLanguageSettingsDlg, CTabDlg)
END_MESSAGE_MAP()


// CLanguageSettingsDlg 消息处理程序


BOOL CLanguageSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
