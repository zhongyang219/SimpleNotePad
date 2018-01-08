#pragma once
#include "Common.h"
#include "afxwin.h"
#include "afxcmn.h"

using std::string;

// CHexViewDlg 对话框

class CHexViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CHexViewDlg)

public:
	CHexViewDlg(string& data, CodeType code, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHexViewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HEX_VIEW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	//十六进制编辑的单位
	enum class EditUnit
	{
		BYTE,
		WORD,
		DWORD
	};

	//数据大小单位
	enum class SizeUnit
	{
		B,
		KB,
		MB
	};

	//修改过的项目
	struct Item
	{
		unsigned int address;
		unsigned int old_value;
		unsigned int value;
		//EditUnit edit_unit;
	};

	string& m_data;
	CodeType m_code;
	CFont m_font;

	CString m_str;		//文本编辑框中显示的字符串
	CEdit m_edit;

	unsigned int m_address;
	unsigned int m_value;

	vector<Item> m_modified_data;		//保存编辑过的数据
	bool m_modified;		//用于保存修改过的状态，如果被修改过，则为true

	EditUnit m_edit_unit{};
	SizeUnit m_size_unit{};

	void ShowHexData(bool ini = false);	//在文本编辑框中显示出十六进制数据(在初始化时调用要把参数设为true)
	unsigned int GetValueAndStr(unsigned int address, EditUnit edit_unit, CString& value_str);		//根据地址和编辑单位返回值，并转换成字符串保存到value_str中
	void ShowSizeInfo();		//在“修改文件大小”处的Static控件上显示文件大小

	void SaveConfig() const;
	void LoadConfig();

public:
	bool IsModified() { return m_modified; }

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEditAddress();
	afx_msg void OnBnClickedSearch();
//	afx_msg void OnBnClickedButtonPrevious();
//	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedModify();
	CListCtrl m_modified_list;
	CStatic m_static_head;
	afx_msg void OnBnClickedRefreshButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedRadioAnsi();
	afx_msg void OnBnClickedRadioUtf8();
	afx_msg void OnBnClickedRadioUtf16();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioByte();
	afx_msg void OnBnClickedRadioWord();
	afx_msg void OnBnClickedRadioDword();
	afx_msg void OnCbnSelchangeSizeUnitCombo();
	afx_msg void OnBnClickedModifySize();
	afx_msg void OnDestroy();
};
