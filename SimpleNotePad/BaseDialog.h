﻿#pragma once


// CBaseDialog 对话框
//用于实现记住对话框大小
//将窗口初始大小设置为最小大小

class CBaseDialog : public CDialog
{
	DECLARE_DYNAMIC(CBaseDialog)

public:
	CBaseDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBaseDialog();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
//	enum { IDD = IDD_BASEDIALOG };
//#endif

	void SetMinSize(int cx, int cy);		//设置窗口的最小大小，如果未设置，则使用窗口的初始大小作为最小大小

protected:
	virtual void LoadConfig();
	virtual void SaveConfig() const;

protected:
	CSize m_min_size{};
	CSize m_window_size{ -1, -1 };

protected:
	virtual CString GetDialogName() const = 0;
    void SetButtonIcon(UINT id, HICON icon);
    void EnableDlgCtrl(UINT id, bool enable);
    void ShowDlgCtrl(UINT id, bool show);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
