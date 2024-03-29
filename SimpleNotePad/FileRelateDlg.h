﻿#pragma once
#include "TabDlg.h"


// CFileRelateDlg 对话框

class CFileRelateDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CFileRelateDlg)

public:
    CFileRelateDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CFileRelateDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FILE_RELATE_DIALOG };
#endif

private:
    std::map<wstring, wstring> extensions;
    std::map<int, bool> m_list_checked_state;		//保存初始时列表中勾选状态

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void RefreshList();
    bool IsModified();

    DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_list_ctrl;
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedSelectAllCheck();
    afx_msg void OnBnClickedDefaultButton();
};
