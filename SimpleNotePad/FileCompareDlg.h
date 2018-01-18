#pragma once
#include "afxcmn.h"
#include "Common.h"

// CFileCompareDlg 对话框

class CFileCompareDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileCompareDlg)

public:
	CFileCompareDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileCompareDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_COMPARE_DIALOG };
#endif

#define WM_COMPARE_COMPLATE (WM_USER+101)		//文件比较完成消息
#define WM_COMPARE_PROGRESS (WM_USER+102)		//文件比较进度消息

protected:
	//struct ResultItem
	//{
	//	unsigned int address;	//地址
	//	unsigned char data1;	//文件1中该地址上的数据
	//	unsigned char data2;	//文件2中该地址上的数据
	//	bool no_data1;		//文件1中该地址上没有数据
	//	bool no_data2;		//文件2中该地址上没有数据
	//	//构造函数
	//	ResultItem(){}
	//	ResultItem(unsigned int _address, unsigned char _data1, unsigned char _data2, bool _no_data1, bool _no_data2) :
	//		address{ _address }, data1{ _data1 }, data2{ _data2 }, no_data1{ _no_data1 }, no_data2{ _no_data2 }
	//	{}
	//};
	//vector<ResultItem> m_compare_result;

	//用于向文件比较工作线程传递信息的结构体
	struct CompareThreadInfo
	{
		HWND hwnd;		//对话框的句柄
		string* file1;		//要比较的文件1的指针
		string* file2;		//要比较的文件2的指针
		CString out_info;		//储存比较结果的输出信息
		int result_count;		//储存不同的字节数
		//HWND edit_handle;		//Edit控件的句柄
		//int complete_ratio;		//完成的比例，取值为0~1000
	};

	static UINT FileCompareThreadFun(LPVOID lpParam);

	CompareThreadInfo m_thread_info;
	CWinThread* m_pFileCompareThread;		//文件比较的线程

	CListCtrl m_result_info;
	CProgressCtrl m_progress_ctrl;

	string m_file1;		//文件1的内容
	string m_file2;		//文件2的内容

	wstring m_file_path1;	//文件1的路径
	wstring m_file_path2;	//文件2的路径

	void OpenFile(LPCTSTR file_path,string& file);

	void _OnExit();

	void EnableButtons(bool enable = true);
	void ClearCompareResult();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenButton1();
	afx_msg void OnBnClickedOpenButton2();
	afx_msg void OnBnClickedCompareButton();
	afx_msg void OnDropFiles(HDROP hDropInfo);
public:
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnCompareComplate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCompareProgress(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
};
