#pragma once
#include "Common.h"
#include "afxwin.h"

// CFormatConvertDlg 对话框

class CFormatConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CFormatConvertDlg)

public:
	CFormatConvertDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFormatConvertDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMAT_CONVERT_DIALOG };
#endif

protected:
	CodeType m_input_format{};		//输入的编码格式
	CodeType m_output_format{};		//输出的编码格式

	vector<wstring> m_file_list;	//要转换的文件列表
	wstring m_output_path;			//输出路径

	CComboBox m_input_box;
	CComboBox m_output_box;
	CListBox m_list_box;

	string m_input_string;		//输入文件中的文本
	wstring m_temp_string;		//转换成Unicode之后的文本
	string m_output_string;		//转换成目标格式后用于输出的文本

	void ShowFileList();

	void JudgeCode();
	bool OpenFile(LPCTSTR file_path);
	bool SaveFile(LPCTSTR file_path);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddButton();
public:
	afx_msg void OnBnClickedRemoveButton();
	afx_msg void OnBnClickedClearButton();
	afx_msg void OnBnClickedBrowseButton();
	afx_msg void OnBnClickedConvertButton();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
