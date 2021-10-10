// HexView.cpp : 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "HexViewDlg.h"
#include "afxdialogex.h"
#include "InsertDataDlg.h"
#include "DeleteDataDlg.h"

// CHexViewDlg 对话框


IMPLEMENT_DYNAMIC(CHexViewDlg, CBaseDialog)

CHexViewDlg::CHexViewDlg(string& data, CodeType code, const CString& file_path, CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_HEX_VIEW_DIALOG, pParent), m_data(data), m_code(code), m_file_path(file_path)
{
}

CHexViewDlg::~CHexViewDlg()
{
}

void CHexViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODIFIED_LIST, m_modified_list);
	DDX_Control(pDX, IDC_STATIC_HEAD, m_static_head);
}


BEGIN_MESSAGE_MAP(CHexViewDlg, CBaseDialog)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_ADDRESS, &CHexViewDlg::OnEnChangeEditAddress)
	ON_BN_CLICKED(IDC_SEARCH, &CHexViewDlg::OnBnClickedSearch)
	//ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, &CHexViewDlg::OnBnClickedButtonPrevious)
	//ON_BN_CLICKED(IDC_BUTTON_NEXT, &CHexViewDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_MODIFY, &CHexViewDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CHexViewDlg::OnBnClickedRefreshButton)
	ON_BN_CLICKED(IDC_RADIO_ANSI, &CHexViewDlg::OnBnClickedRadioAnsi)
	ON_BN_CLICKED(IDC_RADIO_UTF8, &CHexViewDlg::OnBnClickedRadioUtf8)
	ON_BN_CLICKED(IDC_RADIO_UTF16, &CHexViewDlg::OnBnClickedRadioUtf16)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CHexViewDlg::OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_RADIO_BYTE, &CHexViewDlg::OnBnClickedRadioByte)
	ON_BN_CLICKED(IDC_RADIO_WORD, &CHexViewDlg::OnBnClickedRadioWord)
	ON_BN_CLICKED(IDC_RADIO_DWORD, &CHexViewDlg::OnBnClickedRadioDword)
	ON_CBN_SELCHANGE(IDC_SIZE_UNIT_COMBO, &CHexViewDlg::OnCbnSelchangeSizeUnitCombo)
	ON_BN_CLICKED(IDC_MODIFY_SIZE, &CHexViewDlg::OnBnClickedModifySize)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_INSERT_DATA_BUTTON, &CHexViewDlg::OnBnClickedInsertDataButton)
	ON_BN_CLICKED(IDC_DELETE_DATA_BUTTON, &CHexViewDlg::OnBnClickedDeleteDataButton)
    ON_WM_SIZE()
END_MESSAGE_MAP()

// CHexViewDlg 消息处理程序


void CHexViewDlg::ShowHexData()
{
	CWaitCursor wait_cursor;
	string a_line_str;
	wstring a_line;
	CString temp;
	size_t max{ ((m_data.size() - 1) / 16 + 1) * 16 };

	for (size_t i{ 0 }; i <= max; i++)
	{
		if (i % 16 == 0)
		{
			//显示Dump区域的字符串
			if (i > 0)
			{
				a_line_str = m_data.substr(i - 16, 16);
				if (m_code == CodeType::UTF16)	//如果编码是UTF16
				{
					for (int i{}; i < a_line_str.size() - 1; i++)
					{
						if (i % 2 == 0 && a_line_str[i + 1] == 0)	//第奇数个字节是0，即这是一个ASCII字符
						{
                            ConvertDumpChar(a_line_str[i]);
						}
					}
				}
				else
				{
					for (auto& ch : a_line_str)	
                        ConvertDumpChar(ch);
				}
				a_line = CCommon::StrToUnicode(a_line_str, m_code);
				m_str += _T("   ");
				m_str += a_line;
				m_str += _T("\r\n");
			}
			//显示地址
			if (i < max)
			{
				temp.Format(_T("%.8x  "), i);
				temp.MakeUpper();
				m_str += temp;
			}
		}
		//显示数据
		if (i < max)
		{
			if (i < m_data.size())
			{
				temp.Format(_T("%.2x "), static_cast<unsigned char>(m_data[i]));
				temp.MakeUpper();
			}
			else
			{
				temp = _T("   ");
			}
			m_str += temp;
		}
	}
    m_view->SetText(m_str);

}

unsigned int CHexViewDlg::GetValueAndStr(unsigned int address, EditUnit edit_unit, CString& value_str)
{
	unsigned int value;
	switch (edit_unit)
	{
	case EditUnit::BYTE:
		value = (m_address < m_data.size() ? m_data[address] : 0);
		value_str.Format(_T("%.2x"), static_cast<unsigned char>(value));
		break;
	case EditUnit::WORD:
		value = (m_address < m_data.size() ? static_cast<unsigned char>(m_data[address]) : 0)
			+ (m_address + 1 < m_data.size() ? 0x100 * static_cast<unsigned char>(m_data[address + 1]) : 0);
		value_str.Format(_T("%.4x"), static_cast<unsigned short>(value));
		break;
	case EditUnit::DWORD:
		value = (m_address < m_data.size() ? static_cast<unsigned char>(m_data[address]) : 0)
			+ (m_address + 1 < m_data.size() ? 0x100 * static_cast<unsigned char>(m_data[address + 1]) : 0)
			+ (m_address + 2 < m_data.size() ? 0x10000 * static_cast<unsigned char>(m_data[address + 2]) : 0)
			+ (m_address + 3 < m_data.size() ? 0x1000000 * static_cast<unsigned char>(m_data[address + 3]) : 0);
		value_str.Format(_T("%.8x"), static_cast<unsigned long>(value));
		break;
	default:
		break;
	}
	return value;
}

void CHexViewDlg::ShowSizeInfo()
{
	CString tmp;
	switch (m_size_unit)
	{
	case SizeUnit::B:
		tmp.Format(_T("修改文件大小：（当前：%d B）"), m_data.size());
		break;
	case SizeUnit::KB:
		tmp.Format(_T("修改文件大小：（当前：%.2f KB）"), static_cast<double>(m_data.size()) / 1024.0);
		break;
	case SizeUnit::MB:
		tmp.Format(_T("修改文件大小：（当前：%.2f MB）"), static_cast<double>(m_data.size()) / 1024.0 / 1024.0);
		break;
	}
	SetDlgItemText(IDC_STATIC_FILESIZE, tmp);
}

void CHexViewDlg::SaveConfig() const
{
    theApp.WriteProfileInt(_T("hex_editor"), _T("edit_unit"), static_cast<int>(m_edit_unit));
    theApp.WriteProfileInt(_T("hex_editor"), _T("size_unit"), static_cast<int>(m_size_unit));
}

void CHexViewDlg::LoadConfig()
{
	m_edit_unit = static_cast<EditUnit>(theApp.GetProfileInt(_T("hex_editor"), _T("edit_unit"), 0));
	m_size_unit = static_cast<SizeUnit>(theApp.GetProfileInt(_T("hex_editor"), _T("size_unit"), 0));
}

CString CHexViewDlg::GetDialogName() const
{
	return _T("HexViewDlg");
}

void CHexViewDlg::SetHexViewPos()
{
    CRect rc_top;
    ::GetWindowRect(GetDlgItem(IDC_STATIC_HEAD)->GetSafeHwnd(), rc_top);
    ScreenToClient(rc_top);

    CRect rc_group;
    ::GetWindowRect(GetDlgItem(IDC_HEX_GROUP_STATIC)->GetSafeHwnd(), rc_group);
    ScreenToClient(rc_group);
    int width = rc_group.Width() - theApp.DPI(24);

    int height = rc_group.bottom - rc_top.bottom - theApp.DPI(12);

    m_view->SetWindowPos(nullptr, rc_top.left - theApp.DPI(2), rc_top.bottom + theApp.DPI(4), width, height, SWP_NOZORDER);

}

void CHexViewDlg::ConvertDumpChar(char & ch)
{
    if (theApp.GetEditSettings().show_invisible_characters_hex)
    {
        if (ch == '\r' || ch == '\n')
            ch = ' ';
    }
    else
    {
        if (ch >= 0 && ch < ' ')
            ch = ' ';
    }
}

BOOL CHexViewDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
    SetIcon(theApp.GetMenuIcon(IDI_HEX_VIEW), FALSE);

	LoadConfig();
	if(!m_file_path.IsEmpty())
		SetWindowText(m_file_path + _T(" - 十六进制查看"));

	m_modified_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    CRect rect;
	m_modified_list.GetClientRect(rect);
	size_t width1 = rect.Width() * 3 / 10;		//列的宽度：列表宽度的3/10
	//size_t width2 = rect.Width() / 4;		//第2列的宽度：列表宽度的1/4
	m_modified_list.InsertColumn(0, _T("地址"), LVCFMT_LEFT, width1);		//插入第1列
	m_modified_list.InsertColumn(1, _T("修改前"), LVCFMT_LEFT, width1);		//插入第2列
	m_modified_list.InsertColumn(2, _T("修改后"), LVCFMT_LEFT, width1);		//插入第3列

    //初始化编辑器
    m_view = (CHexEditView*)RUNTIME_CLASS(CHexEditView)->CreateObject();
    m_view->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, this, 3001);
    m_view->OnInitialUpdate();
    m_view->ShowWindow(SW_SHOW);

	//设置字体
    m_view->SetFontFace(theApp.GetEditSettings().font_name_hex.GetString());
    m_view->SetFontSize(theApp.GetEditSettings().font_size_hex);
    m_font.CreatePointFont(theApp.GetEditSettings().font_size_hex * 10, theApp.GetEditSettings().font_name_hex);
    m_static_head.SetFont(&m_font);

    m_view->InitHexView();

    ShowHexData();

	//设置单选控件的初始状态
	switch (m_code)
	{
	case CodeType::ANSI: ((CButton*)GetDlgItem(IDC_RADIO_ANSI))->SetCheck(TRUE); break;
	case CodeType::UTF8: case CodeType::UTF8_NO_BOM: ((CButton*)GetDlgItem(IDC_RADIO_UTF8))->SetCheck(TRUE); break;
	case CodeType::UTF16: ((CButton*)GetDlgItem(IDC_RADIO_UTF16))->SetCheck(TRUE); break;
	}

	switch (m_edit_unit)
	{
	case EditUnit::BYTE: ((CButton*)GetDlgItem(IDC_RADIO_BYTE))->SetCheck(TRUE); break;
	case EditUnit::WORD: ((CButton*)GetDlgItem(IDC_RADIO_WORD))->SetCheck(TRUE); break;
	case EditUnit::DWORD: ((CButton*)GetDlgItem(IDC_RADIO_DWORD))->SetCheck(TRUE); break;
	default: break;
	}

	//为组合框添加项目
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->AddString(_T("B"));
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->AddString(_T("KB"));
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->AddString(_T("MB"));
	((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->SetCurSel(static_cast<int>(m_size_unit));

	ShowSizeInfo();

	//设置该对话框在任务栏显示
	ModifyStyleEx(0, WS_EX_APPWINDOW);

    SetHexViewPos();

	m_modified = false;

    AfxGetMainWnd()->ShowWindow(SW_HIDE);       //打开此对话框时隐藏主窗口

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CHexViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CBaseDialog::OnTimer(nIDEvent);
}


void CHexViewDlg::OnEnChangeEditAddress()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBaseDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString address;
	GetDlgItemText(IDC_EDIT_ADDRESS, address);
	m_address = wcstoul(address, NULL, 16);
	if (m_edit_unit == EditUnit::WORD)		//如果编辑的单位是WORD，则地址只有是2的倍数
	{
		m_address /= 2;
		m_address *= 2;
	}
	else if (m_edit_unit == EditUnit::DWORD)		//如果编辑的单位是DWORD，则地址只有是4的倍数
	{
		m_address /= 4;
		m_address *= 4;
	}
}


void CHexViewDlg::OnBnClickedSearch()
{
	// TODO: 在此添加控件通知处理程序代码

	//点击查找后重新显示地址框中的值
	CString address_str;
	address_str.Format(_T("%.8x"), m_address);
	address_str.MakeUpper();
	SetDlgItemText(IDC_EDIT_ADDRESS, address_str);

	CString value_str;
	if (m_address < m_data.size())
	{
		m_value = GetValueAndStr(m_address, m_edit_unit, value_str);
		value_str.MakeUpper();

		//查找后滚动到所在行
		//int first_line = m_edit.GetFirstVisibleLine();	//获取编辑框中最上面可见行的行号
		//int scroll_line = m_address / 16 - first_line;	//计算要向后滚动的行数
		//m_edit.LineScroll(scroll_line);
        m_view->SetFirstVisibleLine(m_address / 16);
	}
	else
	{
		value_str = _T("无数据");
	}
	SetDlgItemText(IDC_EDIT_VALUE, value_str);
}


//void CHexViewDlg::OnBnClickedButtonPrevious()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CString address_str;
//	GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//获取查找框中的字符串
//	m_address = wcstoul(address_str, NULL, 16);
//	m_address--;		//查找框原来的地址减1
//	address_str.Format(_T("%.8x"), m_address);
//	address_str.MakeUpper();
//	SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
//	OnBnClickedSearch();
//}


//void CHexViewDlg::OnBnClickedButtonNext()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CString address_str;
//	GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//获取查找框中的字符串
//	m_address = wcstoul(address_str, NULL, 16);
//	m_address++;		//查找框原来的地址加1
//	address_str.Format(_T("%.8x"), m_address);
//	address_str.MakeUpper();
//	SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
//	OnBnClickedSearch();
//}


void CHexViewDlg::OnBnClickedModify()
{
	// TODO: 在此添加控件通知处理程序代码
	CString out_info;
	if (m_address < m_data.size())
	{
		Item item;
		CString value_str;
		CString old_value_str;
		item.address = m_address;
		//根据地址获取原来的值
		item.old_value = GetValueAndStr(m_address, m_edit_unit, old_value_str);
		old_value_str.MakeUpper();
		//获取编辑框中的修改的值
		GetDlgItemText(IDC_EDIT_VALUE, value_str);
		value_str.MakeUpper();
		switch (m_edit_unit)
		{
		case EditUnit::BYTE: CCommon::ChangeStringLength(value_str, 2, _T('0')); break;
		case EditUnit::WORD: CCommon::ChangeStringLength(value_str, 4, _T('0')); break;
		case EditUnit::DWORD: CCommon::ChangeStringLength(value_str, 8, _T('0')); break;
		default: break;
		}
		item.value = wcstoul(value_str, NULL, 16);

		if (item.old_value != item.value)
		{
			//将新的值写入数据
			//m_data[m_address] = item.value;
			switch (m_edit_unit)
			{
			case EditUnit::BYTE:
				m_data[m_address] = static_cast<unsigned int>(item.value);
				break;
			case EditUnit::WORD:
				m_data[m_address] = static_cast<unsigned int>(item.value % 0x100);
				m_data[m_address + 1] = static_cast<unsigned int>(item.value / 0x100 % 0x100);
				break;
			case EditUnit::DWORD:
				m_data[m_address] = static_cast<unsigned int>(item.value % 0x100);
				m_data[m_address + 1] = static_cast<unsigned int>(item.value / 0x100 % 0x100);
				m_data[m_address + 2] = static_cast<unsigned int>(item.value / 0x10000 % 0x100);
				m_data[m_address + 3] = static_cast<unsigned int>(item.value / 0x1000000 % 0x100);
				break;
			default: break;
			}

			m_modified_data.push_back(item);
			m_modified = true;

			//在修改数据列表添加项目
			//添加地址
			CString str;
			str.Format(_T("%.8x"), item.address);
			str.MakeUpper();
			m_modified_list.InsertItem(m_modified_data.size() - 1, str);
			//添加修改前的值
			m_modified_list.SetItemText(m_modified_data.size() - 1, 1, old_value_str);
			//添加修改后的值
			m_modified_list.SetItemText(m_modified_data.size() - 1, 2, value_str);
		}
		out_info.Format(_T("%.8x:%s->%s"), item.address, old_value_str, value_str);
		out_info.MakeUpper();
	}
	else
	{
		out_info = _T("地址超出范围");
	}
	SetDlgItemText(IDC_STATIC_OUT, out_info);
}


void CHexViewDlg::OnBnClickedRefreshButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int first_line = m_view->GetFirstVisibleLine();	//刷新前获取最上方可见行的行号
	m_str.clear();
	ShowHexData();
    m_view->SetFirstVisibleLine(first_line);		//刷新后滚动到刷新前的位置
}


BOOL CHexViewDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedSearch();
		return TRUE;
	}

	return CBaseDialog::PreTranslateMessage(pMsg);
}


void CHexViewDlg::OnBnClickedRadioAnsi()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_code != CodeType::ANSI)
	{
		m_code = CodeType::ANSI;
		OnBnClickedRefreshButton();
	}
}


void CHexViewDlg::OnBnClickedRadioUtf8()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_code != CodeType::UTF8 && m_code != CodeType::UTF8_NO_BOM)
	{
		m_code = CodeType::UTF8;
		OnBnClickedRefreshButton();
	}
}


void CHexViewDlg::OnBnClickedRadioUtf16()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_code != CodeType::UTF16)
	{
		m_code = CodeType::UTF16;
		OnBnClickedRefreshButton();
	}
}


void CHexViewDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMUpDown->iDelta > 0)		//点击了Spin的往下箭头
	{
		//CString address_str;
		//GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//获取查找框中的字符串
		//m_address = wcstoul(address_str, NULL, 16);
		switch (m_edit_unit)
		{
		case CHexViewDlg::EditUnit::BYTE: m_address--;		//查找框原来的地址减1
			break;
		case CHexViewDlg::EditUnit::WORD: m_address -= 2;		//查找框原来的地址减2
			break;
		case CHexViewDlg::EditUnit::DWORD: m_address -= 4;		//查找框原来的地址减4
			break;
		default:
			break;
		}
		
		//address_str.Format(_T("%.8x"), m_address);
		//address_str.MakeUpper();
		//SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
		OnBnClickedSearch();
	}
	else		//点击了Spin的往上箭头
	{
		//CString address_str;
		//GetDlgItemText(IDC_EDIT_ADDRESS, address_str);	//获取查找框中的字符串
		//m_address = wcstoul(address_str, NULL, 16);
		switch (m_edit_unit)
		{
		case CHexViewDlg::EditUnit::BYTE: m_address++;		//查找框原来的地址加1
			break;
		case CHexViewDlg::EditUnit::WORD: m_address += 2;		//查找框原来的地址加2
			break;
		case CHexViewDlg::EditUnit::DWORD: m_address += 4;		//查找框原来的地址加4
			break;
		default:
			break;
		}
		//address_str.Format(_T("%.8x"), m_address);
		//address_str.MakeUpper();
		//SetDlgItemText(IDC_EDIT_ADDRESS, address_str);
		OnBnClickedSearch();
	}
	*pResult = 0;
}


void CHexViewDlg::OnBnClickedRadioByte()
{
	// TODO: 在此添加控件通知处理程序代码
	m_edit_unit = EditUnit::BYTE;
	OnEnChangeEditAddress();
}


void CHexViewDlg::OnBnClickedRadioWord()
{
	// TODO: 在此添加控件通知处理程序代码
	m_edit_unit = EditUnit::WORD;
	OnEnChangeEditAddress();
}


void CHexViewDlg::OnBnClickedRadioDword()
{
	// TODO: 在此添加控件通知处理程序代码
	m_edit_unit = EditUnit::DWORD;
	OnEnChangeEditAddress();
}


void CHexViewDlg::OnCbnSelchangeSizeUnitCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_size_unit = static_cast<SizeUnit>(((CComboBox*)GetDlgItem(IDC_SIZE_UNIT_COMBO))->GetCurSel());
	ShowSizeInfo();
}


void CHexViewDlg::OnBnClickedModifySize()
{
	// TODO: 在此添加控件通知处理程序代码
	//从编辑框获取要更改的文件大小
	CString size_str;
	GetDlgItemText(IDC_EDIT_SIZE, size_str);
	//根据选择的单位计算文件大小的字节数
	int file_size_byte;
	double file_size;
	switch (m_size_unit)
	{
	case SizeUnit::B:
		file_size_byte = _wtoi(size_str);
		file_size = file_size_byte;
		break;
	case SizeUnit::KB:
		file_size = _wtof(size_str);
		file_size_byte = static_cast<int>(file_size * 1024);
		break;
	case SizeUnit::MB:
		file_size = _wtof(size_str);
		file_size_byte = static_cast<int>(file_size * 1024 * 1024);
		break;
	default:
		break;
	}

	if (file_size_byte < 0)
	{
		MessageBox(_T("请输入正确的文件大小！"), NULL, MB_OK | MB_ICONWARNING);
		return;
	}
	else if (file_size_byte > MAX_FILE_SIZE)
	{
		MessageBox(_T("大小不能超过50MB！"), NULL, MB_OK | MB_ICONWARNING);
		return;
	}

	CString info;
	if (file_size_byte < m_data.size())
	{
		info.Format(_T("你要更改的大小小于原来的大小，多余的部分将被舍弃，你确定要将文件的大小从%u字节更改为%u字节吗？"), m_data.size(), file_size_byte);
	}
	else
	{
		info.Format(_T("你确定要将文件的大小从%u字节更改为%u字节吗？"), m_data.size(), file_size_byte);
	}
	if (MessageBox(info, NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		//更新提示框的信息
		info.Format(_T("文件大小: %u Byte -> %u Byte"), m_data.size(), file_size_byte);
		SetDlgItemText(IDC_STATIC_OUT, info);
		//更改大小
		m_data.resize(file_size_byte);
		ShowSizeInfo();
		m_modified = true;
	}
}


void CHexViewDlg::OnDestroy()
{
	CBaseDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	SaveConfig();

    AfxGetMainWnd()->ShowWindow(SW_SHOW);       //打开此对话框时隐藏主窗口

}


void CHexViewDlg::OnBnClickedInsertDataButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CInsertDataDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		unsigned int address = dlg.GetInsertAddress();
		unsigned int size = dlg.GetInsertSize();

		if (size <= 0)
		{
			MessageBox(_T("数据的长度必须为正数！"), NULL, MB_ICONWARNING | MB_OK);
			return;
		}

		CString info;
		if (address >= m_data.size())
			info.Format(_T("地址 %08x 超出范围，要在文件末尾插入 %d 个字节的数据吗？"), address, size);
		else
			info.Format(_T("确实要在 %08x 地址前插入 %d 个字节的数据吗？"), address, size);
		if (MessageBox(info, NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
		{
			//更新提示框的信息
			info.Format(_T("地址 %08x 处插入 %d 字节"), address, size);
			SetDlgItemText(IDC_STATIC_OUT, info);
			//插入数据
			if (address >= m_data.size())
				m_data.resize(m_data.size() + size);
			else
				m_data.insert(address, size, 0);
			ShowSizeInfo();
			m_modified = true;
		}
	}
}


void CHexViewDlg::OnBnClickedDeleteDataButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CDeleteDataDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		unsigned int address = dlg.GetDeleteAddress();
		unsigned int size = dlg.GetDeleteSize();

		if (size <= 0)
		{
			MessageBox(_T("数据的长度必须为正数！"), NULL, MB_ICONWARNING | MB_OK);
			return;
		}

		CString info;
		if (address >= m_data.size())
		{
			info.Format(_T("地址 %08x 超出范围!"), address);
			MessageBox(info, NULL, MB_ICONWARNING | MB_OK);
			return;
		}

		info.Format(_T("确实要删除地址为 %08x 开始的 %d 个字节的数据吗？"), address, size);
		if (MessageBox(info, NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
		{
			//更新提示框的信息
			info.Format(_T("地址 %08x 处删除 %d 字节"), address, size);
			SetDlgItemText(IDC_STATIC_OUT, info);
			//删除数据
			m_data.erase(address, size);
			ShowSizeInfo();
			m_modified = true;
		}
	}
}


void CHexViewDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MINIMIZED && m_view->GetSafeHwnd() != NULL)
    {
        SetHexViewPos();
    }
}
