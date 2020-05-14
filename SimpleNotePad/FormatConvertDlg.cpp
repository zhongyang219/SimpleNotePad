// FormatConvertDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "FormatConvertDlg.h"
#include "afxdialogex.h"


// CFormatConvertDlg 对话框

IMPLEMENT_DYNAMIC(CFormatConvertDlg, CDialog)

CFormatConvertDlg::CFormatConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FORMAT_CONVERT_DIALOG, pParent)
{

}

CFormatConvertDlg::~CFormatConvertDlg()
{
}

void CFormatConvertDlg::ShowFileList()
{
	m_list_box.ResetContent();
	for (const auto& item : m_file_list)
	{
		m_list_box.AddString(item.c_str());
	}
	CCommon::AdjustListWidth(m_list_box);
}


void CFormatConvertDlg::JudgeCode()
{
	if (m_input_string.size() >= 3 && m_input_string[0] == -17 && m_input_string[1] == -69 && m_input_string[2] == -65)
		m_input_format = CodeType::UTF8;
	else if (m_input_string.size() >= 2 && m_input_string[0] == -1 && m_input_string[1] == -2)
		m_input_format = CodeType::UTF16;
	else if (CCommon::IsUTF8Bytes(m_input_string.c_str()))
		m_input_format = CodeType::UTF8_NO_BOM;
	else m_input_format = CodeType::ANSI;
}

bool CFormatConvertDlg::OpenFile(LPCTSTR file_path)
{
	m_input_string.clear();
	ifstream file{ file_path, std::ios::binary };
	if (file.fail())
	{
		CString info;
		info.Format(_T("无法打开文件“%s”！"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		return false;
	}
	while (!file.eof())
	{
		m_input_string.push_back(file.get());
	}
	m_input_string.pop_back();

	if (m_input_format != CodeType::AUTO)
	{
		m_temp_string = CCommon::StrToUnicode(m_input_string, m_input_format);	//转换成Unicode
	}
	else		//输入编码格式为“自动”时，自动判断编码类型
	{
		JudgeCode();											//判断编码类型
		m_temp_string = CCommon::StrToUnicode(m_input_string, m_input_format);	//转换成Unicode
		if (m_temp_string.size() < m_input_string.size() / 4)		//如果以自动识别的格式转换成Unicode后，Unicode字符串的长度小于多字节字符串长度的1/4，则文本的编码格式可能是UTF16
		{
			m_input_format = CodeType::UTF16;
			m_temp_string = CCommon::StrToUnicode(m_input_string, m_input_format);	//重新转换成Unicode
		}
	}
	return true;
}

bool CFormatConvertDlg::SaveFile(LPCTSTR file_path)
{
	bool char_connot_convert;
	m_output_string = CCommon::UnicodeToStr(m_temp_string, char_connot_convert, m_output_format);
	if (char_connot_convert)	//当文件中包含Unicode字符时，提示有些字符可能无法转换
	{
		CString info;
		info.Format(_T("警告：%s文件中存在无法转换的字符，这些字符已丢失。要保留这些字符，应该选择转换成一种Unicode格式的编码。"), file_path);
		MessageBox(info, NULL, MB_ICONWARNING);
	}
	ofstream file{ file_path, std::ios::binary };
	if (file.fail())
	{
		CString info;
		info.Format(_T("“%s文件无法保存，请检查输出路径是否正确”！"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		return false;
	}
	file << m_output_string;
	return true;
}



void CFormatConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUT_COMBO, m_input_box);
	DDX_Control(pDX, IDC_OUTPUT_COMBO, m_output_box);
	DDX_Control(pDX, IDC_FILE_LIST, m_list_box);
}


BEGIN_MESSAGE_MAP(CFormatConvertDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CFormatConvertDlg::OnBnClickedAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CFormatConvertDlg::OnBnClickedRemoveButton)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CFormatConvertDlg::OnBnClickedClearButton)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CFormatConvertDlg::OnBnClickedBrowseButton)
	ON_BN_CLICKED(IDC_CONVERT_BUTTON, &CFormatConvertDlg::OnBnClickedConvertButton)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CFormatConvertDlg 消息处理程序


BOOL CFormatConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//设置该对话框在任务栏显示
	ModifyStyleEx(0, WS_EX_APPWINDOW);

	m_input_box.AddString(_T("自动识别"));
	m_input_box.AddString(_T("ANSI"));
	m_input_box.AddString(_T("UTF8"));
	m_input_box.AddString(_T("UTF16"));
	m_input_box.SetCurSel(0);

	m_output_box.AddString(_T("ANSI"));
	m_output_box.AddString(_T("UTF8"));
	m_output_box.AddString(_T("UTF16"));
	m_output_box.SetCurSel(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CFormatConvertDlg::OnBnClickedAddButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置过滤器
	LPCTSTR szFilter = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, szFilter, this);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		POSITION posFile = fileDlg.GetStartPosition();
		while (posFile != NULL)
		{
			m_file_list.push_back(fileDlg.GetNextPathName(posFile).GetString());
		}
		ShowFileList();
	}
}


void CFormatConvertDlg::OnBnClickedRemoveButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int select;
	select = m_list_box.GetCurSel();		//获取当前选中项序号
	if (select >= 0 && select < m_file_list.size())
	{
		m_file_list.erase(m_file_list.begin() + select);	//删除选中项
		ShowFileList();
	}
}


void CFormatConvertDlg::OnBnClickedClearButton()
{
	// TODO: 在此添加控件通知处理程序代码
	m_file_list.clear();
	ShowFileList();
}


void CFormatConvertDlg::OnBnClickedBrowseButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CFolderPickerDialog folderPickerDlg;
	if (folderPickerDlg.DoModal() == IDOK)
	{
		m_output_path = folderPickerDlg.GetPathName();
		SetDlgItemText(IDC_FOLDER_EDIT, m_output_path.c_str());
	}
}


void CFormatConvertDlg::OnBnClickedConvertButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_file_list.empty())
	{
		MessageBox(_T("请添加要转换的文件！"), NULL, MB_ICONWARNING);
		return;
	}
	if (m_output_path.empty())
	{
		MessageBox(_T("请选择输出文件夹！"), NULL, MB_ICONWARNING);
		return;
	}

	CWaitCursor wait_cursor;
	switch (m_output_box.GetCurSel())
	{
	case 0: m_output_format = CodeType::ANSI; break;
	case 1: m_output_format = CodeType::UTF8; break;
	case 2: m_output_format = CodeType::UTF16; break;
	default:
		break;
	}

	int convert_cnt{};
	for (const auto& item : m_file_list)
	{
		switch (m_input_box.GetCurSel())
		{
		case 0: m_input_format = CodeType::AUTO; break;
		case 1: m_input_format = CodeType::ANSI; break;
		case 2: m_input_format = CodeType::UTF8; break;
		case 3: m_input_format = CodeType::UTF16; break;
		default: break;
		}
		if(!OpenFile(item.c_str())) continue;		//如果当前文件无法打开，就跳过它
		
		wstring file_name;
		size_t index;
		index = item.find_last_of(L'\\');
		if (index == string::npos) continue;
		file_name = item.substr(index);
		if(!SaveFile((m_output_path + file_name).c_str())) continue;
		convert_cnt++;
	}

	CString info;
	info.Format(_T("转换完成，共转换%d个文件。"), convert_cnt);
	MessageBox(info, NULL, MB_ICONINFORMATION);
}


void CFormatConvertDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR file_path[MAX_PATH];
	int drop_count = DragQueryFile(hDropInfo, -1, NULL, 0);		//取得被拖动文件的数目
	for (int i{}; i < drop_count; i++)
	{
		DragQueryFile(hDropInfo, i, file_path, MAX_PATH);
		m_file_list.emplace_back(file_path);
	}
	ShowFileList();
	CDialog::OnDropFiles(hDropInfo);
}
