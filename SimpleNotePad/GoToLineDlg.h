#pragma once


// CGoToLineDlg 对话框

class CGoToLineDlg : public CDialog
{
	DECLARE_DYNAMIC(CGoToLineDlg)

public:
	CGoToLineDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGoToLineDlg();
    int GetLine();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GTO_DIALOG };
#endif

private:
    int m_line{};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void OnOK();
public:
    virtual BOOL OnInitDialog();
};
