#pragma once
#include "BaseDialog.h"


// CCodeConvertDlg 对话框

class CCodeConvertDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CCodeConvertDlg)

public:
	CCodeConvertDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCodeConvertDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CODE_CONVERT_DIALOG };
#endif

protected:
    void InitComboBox(CComboBox& combo_box);

    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CComboBox m_input_combo;
    CComboBox m_output_combo;
    afx_msg void OnBnClickedButton1();
};
