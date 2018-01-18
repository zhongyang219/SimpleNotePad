// FileCompareDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "FileCompareDlg.h"
#include "afxdialogex.h"


// CFileCompareDlg 对话框

IMPLEMENT_DYNAMIC(CFileCompareDlg, CDialog)

CFileCompareDlg::CFileCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FILE_COMPARE_DIALOG, pParent)
{

}

CFileCompareDlg::~CFileCompareDlg()
{
}

UINT CFileCompareDlg::FileCompareThreadFun(LPVOID lpParam)
{
	CompareThreadInfo* pInfo = (CompareThreadInfo*)lpParam;
	size_t max_size = (pInfo->file1->size() > pInfo->file2->size() ? pInfo->file1->size() : pInfo->file2->size());
	size_t min_size = (pInfo->file1->size() < pInfo->file2->size() ? pInfo->file1->size() : pInfo->file2->size());
	pInfo->result_count = 0;
	//CString out_info;
	int progress;
	pInfo->out_info.Empty();
	for (unsigned int i{}; i < max_size; i++)
	{
		CString temp;
		if (i < pInfo->file1->size() && i < pInfo->file2->size())
		{
			if (pInfo->file1->at(i) != pInfo->file2->at(i))
			{
				temp.Format(_T("%.8x \t%.2x\t%.2x\r\n"), i, static_cast<unsigned char>(pInfo->file1->at(i)), static_cast<unsigned char>(pInfo->file2->at(i)));
				pInfo->result_count++;
			}
		}
		else if (i >= pInfo->file1->size())
		{
			temp.Format(_T("%.8x \t无数据\t%.2x\r\n"), i, static_cast<unsigned char>(pInfo->file2->at(i)));
			pInfo->result_count++;
		}
		else
		{
			temp.Format(_T("%.8x \t%.2x\t无数据\r\n"), i, static_cast<unsigned char>(pInfo->file1->at(i)));
			pInfo->result_count++;
		}
		temp.MakeUpper();
		pInfo->out_info += temp;

		//如果文件比较窗口已经退出，则退出线程
		if (theApp.m_compare_dialog_exit)
			return 0;

		//更新进度条
		progress = static_cast<int>(static_cast<__int64>(i) * 1000 / max_size);
		static int last_progress{};
		if (last_progress != progress)
		{
			::SendMessage(pInfo->hwnd, WM_COMPARE_PROGRESS, (WPARAM)progress, 0);		//将当前比较进度通过消息发送给对话框
			last_progress = progress;
		}
	}
	if (pInfo->out_info.IsEmpty()) pInfo->out_info = _T("两个文件完全相同！");
	::PostMessage(pInfo->hwnd, WM_COMPARE_COMPLATE, 0, 0);		//文件比较完成后发送一个比较完成消息
	return 0;
}

void CFileCompareDlg::OpenFile(LPCTSTR file_path, string & file)
{
	CWaitCursor wait_cursor;
	ifstream open_file{ file_path, std::ios::binary };
	if (open_file.fail())
	{
		CString info;
		info.Format(_T("无法打开文件“%s”！"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		return;
	}
	file.clear();
	while (!open_file.eof())
	{
		file.push_back(open_file.get());
		if (file.size() > MAX_COMPARE_SIZE)
		{
			CString info;
			info.Format(_T("“%s”文件太大，只读取了前面 %d 个字节！"), file_path, MAX_COMPARE_SIZE);
			MessageBox(info, NULL, MB_OK | MB_ICONWARNING);
			//file.clear();

			break;
		}
	}
	file.pop_back();
	ClearCompareResult();
}

void CFileCompareDlg::_OnExit()
{
	theApp.m_compare_dialog_exit = true;		//窗口关闭时将标志置为true
	if (m_pFileCompareThread != nullptr)
		WaitForSingleObject(m_pFileCompareThread->m_hThread, 1000);	//等待线程退出
}

void CFileCompareDlg::EnableButtons(bool enable)
{
	GetDlgItem(IDC_COMPARE_BUTTON)->EnableWindow(enable);
	GetDlgItem(IDC_OPEN_BUTTON1)->EnableWindow(enable);
	GetDlgItem(IDC_OPEN_BUTTON2)->EnableWindow(enable);
}

void CFileCompareDlg::ClearCompareResult()
{
	SetDlgItemText(IDC_EDIT_RESULT, _T(""));
	for(int i{}; i<=6; i++)
		m_result_info.SetItemText(i, 1, _T(""));
}

void CFileCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_result_info);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
}


BEGIN_MESSAGE_MAP(CFileCompareDlg, CDialog)
	ON_BN_CLICKED(IDC_OPEN_BUTTON1, &CFileCompareDlg::OnBnClickedOpenButton1)
	ON_BN_CLICKED(IDC_OPEN_BUTTON2, &CFileCompareDlg::OnBnClickedOpenButton2)
	ON_BN_CLICKED(IDC_COMPARE_BUTTON, &CFileCompareDlg::OnBnClickedCompareButton)
	ON_WM_DROPFILES()
	ON_MESSAGE(WM_COMPARE_COMPLATE, &CFileCompareDlg::OnCompareComplate)
	ON_MESSAGE(WM_COMPARE_PROGRESS, &CFileCompareDlg::OnCompareProgress)
END_MESSAGE_MAP()


BOOL CFileCompareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_result_info.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rect;
	m_result_info.GetClientRect(rect);
	size_t width1 = rect.Width() * 3 / 5;		//第1列的宽度：列表宽度的3/5
	size_t width2 = rect.Width() - width1 - (rect.Width() * 20 / 408);	//第2列的宽度：列表宽度-第1列的宽度-垂直滚动条的宽度
	m_result_info.InsertColumn(0, _T("项目"), LVCFMT_LEFT, width1);		//插入第1列
	m_result_info.InsertColumn(1, _T("值"), LVCFMT_LEFT, width2);		//插入第2列
	m_result_info.InsertItem(0, _T("文件1字节数"));
	m_result_info.InsertItem(1, _T("文件2字节数"));
	m_result_info.InsertItem(2, _T("两个文件大小相差字节数"));
	m_result_info.InsertItem(3, _T("相同的字节数"));
	m_result_info.InsertItem(4, _T("不同的字节数"));
	m_result_info.InsertItem(5, _T("相同的字节数占百分比"));
	m_result_info.InsertItem(6, _T("不同的字节数占百分比"));

	m_progress_ctrl.SetRange(0, 1000);

	theApp.m_compare_dialog_exit = false;

	//设置该对话框在任务栏显示
	ModifyStyleEx(0, WS_EX_APPWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CFileCompareDlg 消息处理程序


void CFileCompareDlg::OnBnClickedOpenButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置过滤器
	LPCTSTR szFilter = _T("所有文件(*.*)|*.*||");
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		//CString file_path;
		m_file_path1 = fileDlg.GetPathName();	//获取打开的文件路径
		OpenFile(m_file_path1.c_str(), m_file1);		//打开文件
		SetDlgItemText(IDC_EDIT_OPEN1, m_file_path1.c_str());
	}
}


void CFileCompareDlg::OnBnClickedOpenButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置过滤器
	LPCTSTR szFilter = _T("所有文件(*.*)|*.*||");
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		//CString file_path;
		m_file_path2 = fileDlg.GetPathName();	//获取打开的文件路径
		OpenFile(m_file_path2.c_str(), m_file2);		//打开文件
		SetDlgItemText(IDC_EDIT_OPEN2, m_file_path2.c_str());
	}
}


void CFileCompareDlg::OnBnClickedCompareButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_compare_result.clear();
	if (m_file_path1.empty())
	{
		MessageBox(_T("请打开文件1！"), NULL, MB_ICONWARNING);
		return;
	}
	if (m_file_path2.empty())
	{
		MessageBox(_T("请打开文件2！"), NULL, MB_ICONWARNING);
		return;
	}
	if (m_file_path1 == m_file_path2)
	{
		MessageBox(_T("比较的是同一个文件！"), NULL, MB_ICONWARNING);
		return;
	}

	m_thread_info.file1 = &m_file1;
	m_thread_info.file2 = &m_file2;
	m_thread_info.hwnd = m_hWnd;
	m_pFileCompareThread = AfxBeginThread(FileCompareThreadFun, &m_thread_info);
	EnableButtons(false);
	ClearCompareResult();
}


void CFileCompareDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR file_path[MAX_PATH];
	int drop_count = DragQueryFile(hDropInfo, -1, NULL, 0);		//取得被拖动文件的数目
	if (drop_count <= 1)		//如果拖动进来的文件只有1个
	{
		DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);		//获取该文件
		if (m_file1.empty())		//如果第1个文件没有打开
		{
			OpenFile(file_path, m_file1);		//打开第1个文件
			SetDlgItemText(IDC_EDIT_OPEN1, file_path);
			m_file_path1 = file_path;
		}
		else
		{
			OpenFile(file_path, m_file2);		//打开第2个文件
			SetDlgItemText(IDC_EDIT_OPEN2, file_path);
			m_file_path2 = file_path;
		}
	}
	else		//如果拖动进来的文件为1个以上
	{
		DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);		//获取第1个文件
		OpenFile(file_path, m_file1);		//打开第1个文件
		SetDlgItemText(IDC_EDIT_OPEN1, file_path);
		m_file_path1 = file_path;

		DragQueryFile(hDropInfo, 1, file_path, MAX_PATH);		//获取第2个文件
		OpenFile(file_path, m_file2);		//打开第2个文件
		SetDlgItemText(IDC_EDIT_OPEN2, file_path);
		m_file_path2 = file_path;
	}
	DragFinish(hDropInfo); //拖放结束后,释放内存

	CDialog::OnDropFiles(hDropInfo);
}




afx_msg LRESULT CFileCompareDlg::OnCompareComplate(WPARAM wParam, LPARAM lParam)
{
	CWaitCursor wait_cursor;
	SetDlgItemText(IDC_PROGRESS_STATIC, _T("比较完成，正在输出结果，请稍候……"));
	const int MAX_LENGTH{ 50000000 };
	if (m_thread_info.out_info.GetLength() > MAX_LENGTH)
	{
		m_thread_info.out_info = m_thread_info.out_info.Left(MAX_LENGTH);
		m_thread_info.out_info += _T("\r\n输出结果太长，可能无法完全显示。");
	}

	SetDlgItemText(IDC_EDIT_RESULT, m_thread_info.out_info);

	size_t max_size = (m_file1.size() > m_file2.size() ? m_file1.size() : m_file2.size());
	size_t min_size = (m_file1.size() < m_file2.size() ? m_file1.size() : m_file2.size());
	int size_difference;
	size_difference = abs(static_cast<int>(m_file1.size() - m_file2.size()));

	CString str;
	//添加第0项数据"文件1字节数"
	str.Format(_T("%u"), m_file1.size());
	m_result_info.SetItemText(0, 1, str);
	//添加第1项数据"文件2字节数"
	str.Format(_T("%u"), m_file2.size());
	m_result_info.SetItemText(1, 1, str);
	//添加第2项数据"两个文件大小相差字节数"
	str.Format(_T("%d"), size_difference);
	m_result_info.SetItemText(2, 1, str);
	//添加第3项数据"相同的字节数"
	str.Format(_T("%u"), max_size - m_thread_info.result_count);
	m_result_info.SetItemText(3, 1, str);
	//添加第4项数据"不同的字节数"
	str.Format(_T("%u"), m_thread_info.result_count - size_difference);
	m_result_info.SetItemText(4, 1, str);
	//添加第5项数据"相同的字节数占百分比"
	str.Format(_T("%.2f%%"), (max_size - m_thread_info.result_count) * 100 / static_cast<float>(min_size));
	m_result_info.SetItemText(5, 1, str);
	//添加第6项数据"不同的字节数占百分比"
	str.Format(_T("%.2f%%"), (m_thread_info.result_count - size_difference) * 100 / static_cast<float>(min_size));
	m_result_info.SetItemText(6, 1, str);
	SetDlgItemText(IDC_PROGRESS_STATIC, _T("比较完成。"));
	EnableButtons(true);
	return 0;
}


afx_msg LRESULT CFileCompareDlg::OnCompareProgress(WPARAM wParam, LPARAM lParam)
{
	int progress = wParam;
	m_progress_ctrl.SetPos(progress);
	CString info;
	info.Format(_T("正在比较，请稍候…已完成%.1f%%"), static_cast<float>(progress) / 10.0);
	SetDlgItemText(IDC_PROGRESS_STATIC, info);
	return 0;
}


void CFileCompareDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	_OnExit();
	CDialog::OnOK();
}


void CFileCompareDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	_OnExit();
	CDialog::OnCancel();
}
