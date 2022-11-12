#pragma once
#include "Common.h"
#include "afxwin.h"
#include "BaseDialog.h"

// CFormatConvertDlg 对话框

class CFormatConvertDlg : public CBaseDialog
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
    UINT m_code_page{};             //非unicode时的默认代码页
    bool m_input_auto_detect{};     //输入编码格式自动识别

	vector<wstring> m_file_list;	//要转换的文件列表
	wstring m_output_path;			//输出路径

	CComboBox m_input_box;
	CComboBox m_output_box;
    CComboBox m_input_codepage_box;
    CListBox m_list_box;

	string m_input_string;		//输入文件中的文本
	wstring m_temp_string;		//转换成Unicode之后的文本
	string m_output_string;		//转换成目标格式后用于输出的文本

    void LoadConfig();
    void SaveConfig() const;

	void ShowFileList();

	void JudgeCode();
	bool OpenFile(LPCTSTR file_path);
	bool SaveFile(LPCTSTR file_path);

    void EnableControl();
    bool ConvertSingleFile(const std::wstring& file_path);

	virtual CString GetDialogName() const override;

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
    afx_msg void OnBnClickedCoverOriFileCheck();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedAddFolderButton();
};
