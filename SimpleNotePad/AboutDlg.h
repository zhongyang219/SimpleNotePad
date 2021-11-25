#pragma once
//#include "DonateDlg.h"
//#include "LinkStatic.h"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnNMClickStaticMail(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickStaticCheckForUpdate(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickStaticGithub(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickStaticGitee(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL OnInitDialog();
    afx_msg void OnNMClickStaticDonate(NMHDR* pNMHDR, LRESULT* pResult);
};
