#include "stdafx.h"
#include "SimpleNotePad.h"
#include "AboutDlg.h"

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_STATIC_MAIL, &CAboutDlg::OnNMClickStaticMail)
	ON_NOTIFY(NM_CLICK, IDC_STATIC_CHECK_FOR_UPDATE, &CAboutDlg::OnNMClickStaticCheckForUpdate)
	ON_NOTIFY(NM_CLICK, IDC_STATIC_GITHUB, &CAboutDlg::OnNMClickStaticGithub)
	ON_NOTIFY(NM_CLICK, IDC_STATIC_GITEE, &CAboutDlg::OnNMClickStaticGitee)
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


void CAboutDlg::OnNMClickStaticMail(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
    //点击了“联系作者”
    ShellExecute(NULL, _T("open"), _T("mailto:zhongyang219@hotmail.com"), NULL, NULL, SW_SHOW);	//打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickStaticCheckForUpdate(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
    //点击了“检查更新”
    theApp.CheckUpdateInThread(true);
	*pResult = 0;
}


void CAboutDlg::OnNMClickStaticGithub(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/SimpleNotePad/"), NULL, NULL, SW_SHOW);	//打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickStaticGitee(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://gitee.com/zhongyang219/SimpleNotePad/"), NULL, NULL, SW_SHOW);	//打开超链接
    *pResult = 0;
}
