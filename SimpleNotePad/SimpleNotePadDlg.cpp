
// SimpleNotePadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "SimpleNotePadDlg.h"
#include "afxdialogex.h"
#include "HexViewDlg.h"
#include "FormatConvertDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSimpleNotePadDlg 对话框



CSimpleNotePadDlg::CSimpleNotePadDlg(CString file_path, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SIMPLENOTEPAD_DIALOG, pParent), m_file_path(file_path)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleNotePadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}

void CSimpleNotePadDlg::OpenFile(LPCTSTR file_path)
{
	////打开新文件前询问用户是否保存
	//if (!SaveInquiry())
	//	return;

	CWaitCursor wait_cursor;

	m_edit_str.clear();
	m_modified = false;
	ifstream file{ file_path, std::ios::binary };
	if (file.fail())
	{
		CString info;
		info.Format(_T("无法打开文件“%s”！"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		m_file_path.Empty();
		return;
	}
	while (!file.eof())
	{
		m_edit_str.push_back(file.get());
		if (m_edit_str.size() > MAX_FILE_SIZE)	//当文件大小超过MAX_FILE_SIZE时禁止打开
		{
			CString info;
			info.Format(_T("“%s”文件太大，将只加载文件前面%dMB的内容，要继续吗？"), file_path, MAX_FILE_SIZE / 1024 / 1024);
			if (MessageBox(info, NULL, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				break;
			}
			else
			{
				m_file_path.Empty();
				m_edit_str.clear();
				return;
			}
		}
	}
	m_edit_str.pop_back();

	JudgeCode();											//判断编码类型
	m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);	//转换成Unicode
	if (m_edit_wcs.size() < m_edit_str.size() / 4)		//如果以自动识别的格式转换成Unicode后，Unicode字符串的长度小于多字节字符串长度的1/4，则文本的编码格式可能是UTF16
	{
		m_code = CodeType::UTF16;
		m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);	//重新转换成Unicode
	}
	m_edit.SetWindowText(m_edit_wcs.c_str());				//将文件中的内容显示到编缉窗口中
	//m_flag = true;
	ShowStatusBar();										//更新状态栏
}

bool CSimpleNotePadDlg::SaveFile(LPCTSTR file_path, CodeType code)
{
	bool char_connot_convert;
	m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, char_connot_convert, code);
	if (char_connot_convert)	//当文件中包含Unicode字符时，询问用户是否要选择一个Unicode编码格式再保存
	{
		CString info;
		info.LoadString(IDS_STRING102);		//从string table载入字符串
		if (MessageBox(info, NULL, MB_OKCANCEL | MB_ICONWARNING) != IDOK) return false;		//如果用户点击了取消按钮，则返回false
	}
	ofstream file{ file_path, std::ios::binary };
	if (!file.good())		//如果无法保存文件，则返回false
		return false;
	file << m_edit_str;
	m_modified = false;
	ShowStatusBar();		//保存后刷新状态栏
	return true;
}

void CSimpleNotePadDlg::JudgeCode()
{
	if (m_edit_str.size() >= 3 && m_edit_str[0] == -17 && m_edit_str[1] == -69 && m_edit_str[2] == -65)
		m_code = CodeType::UTF8;
	else if (m_edit_str.size() >= 2 && m_edit_str[0] == -1 && m_edit_str[1] == -2)
		m_code = CodeType::UTF16;
	else if (CCommon::IsUTF8Bytes(m_edit_str.c_str()))
		m_code = CodeType::UTF8_NO_BOM;
	else m_code = CodeType::ANSI;
}

void CSimpleNotePadDlg::ShowStatusBar()
{
	//显示编码格式
	CString str{_T("编码格式：")};
	//if (!m_edit_wcs.empty())
	//{
		switch (m_code)
		{
		case CodeType::ANSI: str += _T("ANSI"); break;
		case CodeType::UTF8: str += _T("UTF8"); break;
		case CodeType::UTF8_NO_BOM: str += _T("UTF8无BOM"); break;
		case CodeType::UTF16: str += _T("UTF16"); break;
		}
	//}
	m_status_bar.SetText(str, 2, 0);

	//显示字符数
	//if (m_edit_wcs.empty())
	//	str.Empty();
	if (m_edit_str.size() < 1024)
		str.Format(_T("共%d个字节，%d个字符"), m_edit_str.size(), m_edit_wcs.size());
	else
		str.Format(_T("共%d个字节(%dKB)，%d个字符"), m_edit_str.size(), m_edit_str.size() / 1024, m_edit_wcs.size());
	m_status_bar.SetText(str, 0, 0);

	//显示是否修改
	m_status_bar.SetText(m_modified?_T("已修改"):_T("未修改"), 1, 0);
}

void CSimpleNotePadDlg::ChangeCode()
{
	m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);
	m_edit.SetWindowText(m_edit_wcs.c_str());
	//m_flag = true;
	ShowStatusBar();
}

bool CSimpleNotePadDlg::BeforeChangeCode()
{
	return SaveInquiry(_T("注意，如果更改编码格式，未保存的所有更改都将丢失！是否要保存？"));
}

void CSimpleNotePadDlg::SaveConfig()
{
	//保存字体设置
	WritePrivateProfileString(_T("config"), _T("font_name"), m_font_name, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"font_size", m_font_size, theApp.m_config_path.c_str());
	//保存窗口大小
	CCommon::WritePrivateProfileInt(L"config", L"window_width", m_window_width, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"window_hight", m_window_hight, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"word_wrap", m_word_wrap, theApp.m_config_path.c_str());

	CCommon::WritePrivateProfileInt(L"config", L"find_no_case", m_find_no_case, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileInt(L"config", L"find_whole_word", m_find_whole_word, theApp.m_config_path.c_str());
}

void CSimpleNotePadDlg::LoadConfig()
{
	//载入字体设置
	GetPrivateProfileString(_T("config"), _T("font_name"), _T("微软雅黑"), m_font_name.GetBuffer(32), 32, theApp.m_config_path.c_str());
	m_font_size = GetPrivateProfileInt(_T("config"), _T("font_size"), 10, theApp.m_config_path.c_str());
	//载入窗口大小
	m_window_width = GetPrivateProfileInt(_T("config"), _T("window_width"), 560, theApp.m_config_path.c_str());
	m_window_hight = GetPrivateProfileInt(_T("config"), _T("window_hight"), 350, theApp.m_config_path.c_str());
	m_word_wrap = (GetPrivateProfileInt(_T("config"), _T("word_wrap"), 1, theApp.m_config_path.c_str()) != 0);

	m_find_no_case = (GetPrivateProfileInt(_T("config"), _T("find_no_case"), 0, theApp.m_config_path.c_str()) != 0);
	m_find_whole_word = (GetPrivateProfileInt(_T("config"), _T("find_whole_word"), 0, theApp.m_config_path.c_str()) != 0);
}

bool CSimpleNotePadDlg::SaveInquiry(LPCTSTR info)
{
	if (m_modified)
	{
		CString text;
		if (info == NULL)
		{
			if (m_file_path.IsEmpty())
				text = _T("无标题 中的内容已更改，是否保存？");
			else
				text.Format(_T("“%s”文件中的内容已更改，是否保存？"), m_file_path);
		}
		else
		{
			text = info;
		}

		int rtn = MessageBox(text, NULL, MB_YESNOCANCEL | MB_ICONWARNING);
		switch (rtn)
		{
		case IDYES: return _OnFileSave();
		case IDNO: m_modified = false; break;
		default: return false;
		}
	}
	return true;
}

void CSimpleNotePadDlg::SetTitle()
{
	if(!m_file_path.IsEmpty())
		SetWindowText(m_file_path + _T(" - SimpleNotePad"));
	else
		SetWindowText(_T("无标题 - SimpleNotePad"));
}

bool CSimpleNotePadDlg::_OnFileSave()
{
	if (m_modified)		//只有在已更改过之后才保存
	{
		//已经打开过一个文件时就直接保存，还没有打开一个文件就弹出“另存为”对话框
		if (!m_file_path.IsEmpty())
		{
			if (SaveFile(m_file_path, m_code))
				return true;
			else
				return _OnFileSaveAs();		//文件无法保存时弹出“另存为”对话框
		}
		else
		{
			return _OnFileSaveAs();
		}
	}
	return false;
}

bool CSimpleNotePadDlg::_OnFileSaveAs()
{
	//设置过滤器
	const wchar_t* szFilter = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	//设置另存为时的默认文件名
	wstring file_name;
	if (!m_file_path.IsEmpty())
	{
		wstring file_path(m_file_path);
		size_t index = file_path.find_last_of(L'\\');
		file_name = file_path.substr(index + 1);
	}
	else
	{
		file_name = L"无标题";
	}
	//构造保存文件对话框
	CFileDialog fileDlg(FALSE, _T("txt"), file_name.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	//为“另存为”对话框添加一个组合选择框
	fileDlg.AddComboBox(IDC_SAVE_COMBO_BOX);
	//为组合选择框添加项目
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 0, _T("以ANSI格式保存"));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 1, _T("以UTF-8格式保存"));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 2, _T("以UTF-8无BOM格式保存"));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 3, _T("以UTF-16格式保存"));
	//fileDlg.SetControlLabel(IDC_SAVE_COMBO_BOX, _T("编码类型："));
	//根据当前设置的另存为格式为组合选择框设置默认选中的项目
	fileDlg.SetSelectedControlItem(IDC_SAVE_COMBO_BOX, static_cast<DWORD>(m_save_code));

	////设置“另存为”对话框标题
	//CString str{ _T("另存为") };
	//switch (m_save_code)
	//{
	//case CodeType::ANSI: str += _T("ANSI"); break;
	//case CodeType::UTF8: str += _T("UTF8"); break;
	//case CodeType::UTF8_NO_BOM: str += _T("UTF8无BOM"); break;
	//case CodeType::UTF16: str += _T("UTF16"); break;
	//}
	//fileDlg.m_ofn.lpstrTitle = str.GetString();
	//显示保存文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		DWORD selected_item;
		fileDlg.GetSelectedControlItem(IDC_SAVE_COMBO_BOX, selected_item);	//获取“编码格式”中选中的项目
		m_save_code = static_cast<CodeType>(selected_item);
		if (SaveFile(fileDlg.GetPathName().GetString(), m_save_code))
		{
			m_file_path = fileDlg.GetPathName();	//另存为后，当前文件名为保存的文件名
			SetTitle();					//用新的文件名设置标题
			m_code = m_save_code;		//另存为后当前编码类型设置为另存为的编码类型
			ShowStatusBar();			//刷新状态栏
			return true;
		}
	}
	return false;
}

void CSimpleNotePadDlg::SaveHex()
{
	ofstream file{ m_file_path, std::ios::binary };
	if (file.fail())
	{
		MessageBox(_T("保存失败！"), NULL, MB_ICONWARNING);
		return;
	}
	file << m_edit_str;
	m_modified = false;
	ShowStatusBar();
	MessageBox(_T("十六进制编辑的更改已保存"), NULL, MB_ICONINFORMATION);
}

//void CSimpleNotePadDlg::SaveAsHex()
//{
//	//设置过滤器
//	const wchar_t* szFilter = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
//	//构造保存文件对话框
//	CFileDialog fileDlg(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
//	//设置“另存为”对话框标题
//	fileDlg.m_ofn.lpstrTitle = _T("十六进制保存");
//	//显示保存文件对话框
//	if (IDOK == fileDlg.DoModal())
//	{
//		m_file_path = fileDlg.GetPathName();	//另存为后，当前文件名为保存的文件名
//		SaveHex();
//		SetTitle();					//用新的文件名设置标题
//		ShowStatusBar();			//刷新状态栏
//	}
//}


BEGIN_MESSAGE_MAP(CSimpleNotePadDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_APP_ABOUT, &CSimpleNotePadDlg::OnAppAbout)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CSimpleNotePadDlg::OnFileOpen)
	ON_COMMAND(ID_CODE_ANSI, &CSimpleNotePadDlg::OnCodeAnsi)
	ON_COMMAND(ID_CODE_UTF8, &CSimpleNotePadDlg::OnCodeUtf8)
	ON_COMMAND(ID_CODE_UTF16, &CSimpleNotePadDlg::OnCodeUtf16)
//	ON_UPDATE_COMMAND_UI(ID_CODE_ANSI, &CSimpleNotePadDlg::OnUpdateCodeAnsi)
//	ON_UPDATE_COMMAND_UI(ID_CODE_UTF8, &CSimpleNotePadDlg::OnUpdateCodeUtf8)
//	ON_UPDATE_COMMAND_UI(ID_CODE_UTF16, &CSimpleNotePadDlg::OnUpdateCodeUtf16)
//	ON_WM_INITMENUPOPUP()
	ON_EN_CHANGE(IDC_EDIT1, &CSimpleNotePadDlg::OnEnChangeEdit1)
//	ON_COMMAND(ID_SAVE_ANSI, &CSimpleNotePadDlg::OnSaveAnsi)
//	ON_COMMAND(ID_SAVE_UTF8, &CSimpleNotePadDlg::OnSaveUtf8)
//	ON_COMMAND(ID_SAVE_UTF8_NOBOM, &CSimpleNotePadDlg::OnSaveUtf8Nobom)
//	ON_COMMAND(ID_SAVE_UTF16, &CSimpleNotePadDlg::OnSaveUtf16)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_ANSI, &CSimpleNotePadDlg::OnUpdateSaveAnsi)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_UTF8, &CSimpleNotePadDlg::OnUpdateSaveUtf8)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_UTF8_NOBOM, &CSimpleNotePadDlg::OnUpdateSaveUtf8Nobom)
//	ON_UPDATE_COMMAND_UI(ID_SAVE_UTF16, &CSimpleNotePadDlg::OnUpdateSaveUtf16)
	ON_COMMAND(ID_FILE_SAVE, &CSimpleNotePadDlg::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CSimpleNotePadDlg::OnFileSaveAs)
	ON_COMMAND(ID_FORMAT_FONT, &CSimpleNotePadDlg::OnFormatFont)
	ON_COMMAND(ID_EDIT_UNDO, &CSimpleNotePadDlg::OnEditUndo)
	ON_COMMAND(ID_EDIT_CUT, &CSimpleNotePadDlg::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CSimpleNotePadDlg::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CSimpleNotePadDlg::OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CSimpleNotePadDlg::OnEditSelectAll)
	ON_COMMAND(ID_HEX_VIEW, &CSimpleNotePadDlg::OnHexView)
//	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILE_NEW, &CSimpleNotePadDlg::OnFileNew)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_COMPARE, &CSimpleNotePadDlg::OnFileCompare)
	ON_COMMAND(ID_WORD_WRAP, &CSimpleNotePadDlg::OnWordWrap)
//	ON_UPDATE_COMMAND_UI(ID_WORD_WRAP, &CSimpleNotePadDlg::OnUpdateWordWrap)
	ON_WM_CLOSE()
	ON_COMMAND(ID_EDIT_FIND, &CSimpleNotePadDlg::OnFind)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
	ON_COMMAND(ID_FIND_NEXT, &CSimpleNotePadDlg::OnFindNext)
	ON_COMMAND(ID_MARK_ALL, &CSimpleNotePadDlg::OnMarkAll)
	ON_COMMAND(ID_EDIT_REPLACE, &CSimpleNotePadDlg::OnReplace)
	ON_WM_MENUSELECT()
	ON_WM_INITMENU()
	ON_COMMAND(ID_FORMAT_CONVERT, &CSimpleNotePadDlg::OnFormatConvert)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

// CSimpleNotePadDlg 消息处理程序

BOOL CSimpleNotePadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//获取配置文件的路径（当前exe文件所在路径\config.ini）
	//m_config_path = CCommon::GetCurrentPath() + L"config.ini";
	//载入设置
	LoadConfig();
	//初始化窗口大小
	CRect rect;
	rect.right = m_window_width;
	rect.bottom = m_window_hight;
	MoveWindow(rect);

	//根据当前系统DPI设置设置状态栏大小
	CWindowDC dc(this);
	HDC hDC = dc.GetSafeHdc();
	m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
	m_status_bar_hight = m_dpi * 20 / 96;
	m_edit_bottom_space = m_dpi * 22 / 96;
	m_status_bar_mid_width = m_dpi * 60 / 96;
	m_status_bar_right_width = m_dpi * 160 / 96;

	//初始化编辑框大小
	GetClientRect(&rect);
	//rect.bottom = rect.bottom - 22;
	rect.bottom = rect.bottom - m_edit_bottom_space;
	m_edit.MoveWindow(rect);
	
	//初始化状态栏
	GetClientRect(&rect);
	//rect.top = rect.bottom - 20;
	rect.top = rect.bottom - m_status_bar_hight;
	m_status_bar.Create(WS_VISIBLE | CBRS_BOTTOM, rect, this, 3);

	int nParts[3] = { rect.Width() - m_status_bar_right_width - m_status_bar_mid_width, rect.Width() - m_status_bar_right_width, -1 }; //分割尺寸
	m_status_bar.SetParts(3, nParts); //分割状态栏
	ShowStatusBar();

	//初始化字体
	m_font.CreatePointFont(m_font_size * 10, m_font_name);
	m_edit.SetFont(&m_font);

	//如果m_file_path获得了通过构造函数参数传递的、来自命令行的文件路径，则打开文件
	if (!m_file_path.IsEmpty())
	{
		//从命令行获取的文件路径如果包含空格，它的前后会有引号，如果有就把它们删除
		wstring file_path{ m_file_path.GetString() };
		if (file_path.front() == L'\"')
			file_path = file_path.substr(1);
		if (file_path.back() == L'\"')
			file_path.pop_back();
		m_file_path = file_path.c_str();
		//OpenFile(m_file_path);
		SetTimer(1234, 100, NULL);
	}
	SetTitle();

	//设置最大文本限制
	m_edit.SetLimitText(static_cast<UINT>(-1));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSimpleNotePadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSimpleNotePadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSimpleNotePadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSimpleNotePadDlg::OnAppAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CSimpleNotePadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect size;		//编辑框矩形区域
	size.right = cx;
	//size.bottom = cy - 22;		//窗口下方状态栏占20个像素高度
	size.bottom = cy - m_edit_bottom_space;		//窗口下方状态栏占20个像素高度
	if (nType != SIZE_MINIMIZED && m_edit.m_hWnd != NULL)
	{
		m_edit.MoveWindow(size);		//窗口大小改变时改变编辑框大小
	}

	CRect status_bar_size;
	status_bar_size.right = cx;
	status_bar_size.bottom = cy;
	//status_bar_size.top = cy - 20;
	status_bar_size.top = cy - m_status_bar_hight;
	if (nType != SIZE_MINIMIZED && m_status_bar.m_hWnd != NULL)
	{
		m_status_bar.MoveWindow(status_bar_size);
		int nParts[3] = { cx - m_status_bar_right_width - m_status_bar_mid_width, cx - m_status_bar_right_width, -1 }; //分割尺寸
		m_status_bar.SetParts(3, nParts); //分割状态栏
	}
	if (nType != SIZE_MAXIMIZED && nType != SIZE_MINIMIZED)
	{
		//m_window_width = cx;
		//m_window_hight = cy;
		CRect rect;
		GetWindowRect(&rect);
		m_window_width = rect.Width();
		m_window_hight = rect.Height();
	}
}


void CSimpleNotePadDlg::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	//打开新文件前询问用户是否保存
	if (!SaveInquiry())
		return;
	//设置过滤器
	LPCTSTR szFilter = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		m_file_path = fileDlg.GetPathName();	//获取打开的文件路径
		OpenFile(m_file_path);					//打开文件
		SetTitle();								//设置窗口标题
	}
}


void CSimpleNotePadDlg::OnCodeAnsi()
{
	// TODO: 在此添加命令处理程序代码
	//m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
	if(!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodeUtf8()
{
	// TODO: 在此添加命令处理程序代码
	//m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
	if (!BeforeChangeCode()) return;
	if(m_code != CodeType::UTF8_NO_BOM)
		m_code = CodeType::UTF8;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodeUtf16()
{
	// TODO: 在此添加命令处理程序代码
	//m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
	if (!BeforeChangeCode()) return;
	m_code = CodeType::UTF16;
	ChangeCode();
}


//void CSimpleNotePadDlg::OnUpdateCodeAnsi(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_code == CodeType::ANSI);
//}


//void CSimpleNotePadDlg::OnUpdateCodeUtf8(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_code == CodeType::UTF8 || m_code == CodeType::UTF8_NO_BOM);
//}


//void CSimpleNotePadDlg::OnUpdateCodeUtf16(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_code == CodeType::UTF16);
//}


//void CSimpleNotePadDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
//{
//	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
//
//	// TODO: 在此处添加消息处理程序代码
//	ASSERT(pPopupMenu != NULL);
//	// Check the enabled state of various menu items.
//
//	CCmdUI state;
//	state.m_pMenu = pPopupMenu;
//	ASSERT(state.m_pOther == NULL);
//	ASSERT(state.m_pParentMenu == NULL);
//
//	// Determine if menu is popup in top-level menu and set m_pOther to
//	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
//	HMENU hParentMenu;
//	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
//		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
//	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
//	{
//		CWnd* pParent = this;
//		// Child Windows dont have menus--need to go to the top!
//		if (pParent != NULL &&
//			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
//		{
//			int nIndexMax = ::GetMenuItemCount(hParentMenu);
//			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
//			{
//				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
//				{
//					// When popup is found, m_pParentMenu is containing menu.
//					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
//					break;
//				}
//			}
//		}
//	}
//
//	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
//	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
//		state.m_nIndex++)
//	{
//		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
//		if (state.m_nID == 0)
//			continue; // Menu separator or invalid cmd - ignore it.
//
//		ASSERT(state.m_pOther == NULL);
//		ASSERT(state.m_pMenu != NULL);
//		if (state.m_nID == (UINT)-1)
//		{
//			// Possibly a popup menu, route to first item of that popup.
//			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
//			if (state.m_pSubMenu == NULL ||
//				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
//				state.m_nID == (UINT)-1)
//			{
//				continue;       // First item of popup cant be routed to.
//			}
//			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
//		}
//		else
//		{
//			// Normal menu item.
//			// Auto enable/disable if frame window has m_bAutoMenuEnable
//			// set and command is _not_ a system command.
//			state.m_pSubMenu = NULL;
//			state.DoUpdate(this, FALSE);
//		}
//
//		// Adjust for menu deletions and additions.
//		UINT nCount = pPopupMenu->GetMenuItemCount();
//		if (nCount < state.m_nIndexMax)
//		{
//			state.m_nIndex -= (state.m_nIndexMax - nCount);
//			while (state.m_nIndex < nCount &&
//				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
//			{
//				state.m_nIndex++;
//			}
//		}
//		state.m_nIndexMax = nCount;
//	}
//}



void CSimpleNotePadDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString edit_text;
	m_edit.GetWindowText(edit_text);
	m_edit_wcs.assign(edit_text);
	//m_flag = false;
	//if (!m_flag)
	//{
		m_modified = true;
	//}
	ShowStatusBar();
}


//void CSimpleNotePadDlg::OnSaveAnsi()
//{
//	// TODO: 在此添加命令处理程序代码
//	m_save_code = CodeType::ANSI;
//}


//void CSimpleNotePadDlg::OnSaveUtf8()
//{
//	// TODO: 在此添加命令处理程序代码
//	m_save_code = CodeType::UTF8;
//}


//void CSimpleNotePadDlg::OnSaveUtf8Nobom()
//{
//	// TODO: 在此添加命令处理程序代码
//	m_save_code = CodeType::UTF8_NO_BOM;
//}


//void CSimpleNotePadDlg::OnSaveUtf16()
//{
//	// TODO: 在此添加命令处理程序代码
//	m_save_code = CodeType::UTF16;
//}


//void CSimpleNotePadDlg::OnUpdateSaveAnsi(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_save_code == CodeType::ANSI);
//}


//void CSimpleNotePadDlg::OnUpdateSaveUtf8(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_save_code == CodeType::UTF8);
//}


//void CSimpleNotePadDlg::OnUpdateSaveUtf8Nobom(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_save_code == CodeType::UTF8_NO_BOM);
//}


//void CSimpleNotePadDlg::OnUpdateSaveUtf16(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_save_code == CodeType::UTF16);
//}


void CSimpleNotePadDlg::OnFileSave()
{
	// TODO: 在此添加命令处理程序代码
	_OnFileSave();
}


void CSimpleNotePadDlg::OnFileSaveAs()
{
	// TODO: 在此添加命令处理程序代码
	_OnFileSaveAs();
}


void CSimpleNotePadDlg::OnFormatFont()
{
	// TODO: 在此添加命令处理程序代码
	LOGFONT lf{ 0 };             //LOGFONT变量
	m_font.GetLogFont(&lf);
	//_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("微软雅黑"));	//将lf中的元素字体名设为“微软雅黑”
	CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
	if (IDOK == fontDlg.DoModal())     // 显示字体对话框
	{
		//如果m_font已经关联了一个字体资源对象，则释放它
		if (m_font.m_hObject)
		{
			m_font.DeleteObject();
		}
		//使用选定字体的LOGFONT创建新的字体
		m_font.CreateFontIndirect(fontDlg.m_cf.lpLogFont);
		//设置字体
		m_edit.SetFont(&m_font);
		//获取字体信息
		m_font_name = fontDlg.m_cf.lpLogFont->lfFaceName;
		m_font_size = fontDlg.m_cf.iPointSize / 10;
		//将字体设置写入到ini文件
		SaveConfig();
	}
}


BOOL CSimpleNotePadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//屏蔽ESC键退出
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (GetKeyState(VK_CONTROL) & 0x80)
	{
		////设置按Ctr+A全选
		//if (pMsg->wParam == 'a' || pMsg->wParam == 'A')
		//{
		//	OnEditSelectAll();
		//	return TRUE;
		//}
		//设置Ctr+S保存
		if (pMsg->wParam == 's' || pMsg->wParam == 'S')
		{
			_OnFileSave();
			return TRUE;
		}
		//设置Ctr+O打开
		else if (pMsg->wParam == 'o' || pMsg->wParam == 'O')
		{
			OnFileOpen();
			return TRUE;
		}
		//设置Ctr+N新建
		else if (pMsg->wParam == 'n' || pMsg->wParam == 'N')
		{
			OnFileNew();
			return TRUE;
		}
		//设置Ctr+H打开十六进制查看
		else if (pMsg->wParam == 'e' || pMsg->wParam == 'E')
		{
			OnHexView();
			return TRUE;
		}
		//设置Ctr+F查找
		else if (pMsg->wParam == 'f' || pMsg->wParam == 'F')
		{
			OnFind();
			return TRUE;
		}
		//设置Ctr+H替换
		else if (pMsg->wParam == 'h' || pMsg->wParam == 'H')
		{
			OnReplace();
			return TRUE;
		}
	}
	//设置按F3查找下一个
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F3)
	{
		OnFindNext();
		return TRUE;
	}
	//处理Edit中的TAB键
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB && pMsg->hwnd == m_edit.GetSafeHwnd())
	{
		CString str;
		m_edit.GetWindowText(str);
		int nStart, nEnd;
		m_edit.GetSel(nStart, nEnd);
		if (nStart != nEnd)
		{
			str = str.Left(nStart) + str.Mid(nEnd);
		}
		str.Insert(nStart, _T("\t"));
		m_edit.SetWindowText(str);
		m_edit.SetSel(nStart + 1, nStart + 1);
		m_edit_wcs = str.GetString();
		ShowStatusBar();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CSimpleNotePadDlg::OnEditUndo()
{
	// TODO: 在此添加命令处理程序代码
	m_edit.Undo();
}


void CSimpleNotePadDlg::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码
	m_edit.Cut();
}


void CSimpleNotePadDlg::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	m_edit.Copy();
}


void CSimpleNotePadDlg::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
	m_edit.Paste();
}


void CSimpleNotePadDlg::OnEditSelectAll()
{
	// TODO: 在此添加命令处理程序代码
	m_edit.SetSel(0, -1);
}


void CSimpleNotePadDlg::OnHexView()
{
	// TODO: 在此添加命令处理程序代码
	string edit_str{ m_edit_str };		//进行十六进制编辑前先保存编辑前的数据
	CHexViewDlg hex_view_dlg(m_edit_str, m_code, m_file_path);
	ShowWindow(SW_HIDE);
	hex_view_dlg.DoModal();
	ShowWindow(SW_SHOW);
	//if (!m_file_path.IsEmpty() && !hex_view_dlg.m_modified_data.empty())
	if (!m_file_path.IsEmpty() && hex_view_dlg.IsModified())
	{
		if (m_modified)
		{
			CString info;
			info.LoadString(IDS_STRING103);
			if (MessageBox(info, NULL, MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				return;
			}
		}
		if (MessageBox(_T("是否要保存十六进制编辑的更改？"), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);
			m_edit.SetWindowText(m_edit_wcs.c_str());
			SaveHex();
		}
		else
		{
			//如果没有保存十六进制编辑的更改，则恢复原来的数据
			m_edit_str = edit_str;
		}
	}
}


//void CSimpleNotePadDlg::OnDestroy()
//{
//	CDialog::OnDestroy();
//
//	// TODO: 在此处添加消息处理程序代码
//}


void CSimpleNotePadDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//打开新文件前询问用户是否保存
	if (!SaveInquiry())
		return;
	wchar_t file_path[MAX_PATH];
	DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);
	m_file_path = file_path;
	OpenFile(m_file_path);	//打开文件
	SetTitle();				//设置窗口标题
	DragFinish(hDropInfo);  //拖放结束后,释放内存

	CDialog::OnDropFiles(hDropInfo);
}


void CSimpleNotePadDlg::OnFileNew()
{
	// TODO: 在此添加命令处理程序代码
	//询问是否保存
	if (!SaveInquiry())
		return;

	m_edit_str.clear();
	m_edit_wcs.clear();
	m_file_path.Empty();
	m_edit.SetWindowText(_T(""));
	m_code = CodeType::ANSI;
	m_modified = false;
	ShowStatusBar();
	SetWindowText(_T("无标题 - SimpleNotePad"));
}


void CSimpleNotePadDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OpenFile(m_file_path);		//如果文件是通过命令行打开的，则延时100毫秒再打开
	KillTimer(1234);		//定时器响应一次后就将其销毁

	CDialog::OnTimer(nIDEvent);
}


void CSimpleNotePadDlg::OnFileCompare()
{
	// TODO: 在此添加命令处理程序代码
	CFileCompareDlg aDlg;
	ShowWindow(SW_HIDE);
	aDlg.DoModal();
	ShowWindow(SW_SHOW);
}


void CSimpleNotePadDlg::OnWordWrap()
{
	// TODO: 在此添加命令处理程序代码
	if (m_word_wrap)
	{
		//m_edit.ModifyStyle(WS_HSCROLL, ES_AUTOHSCROLL);
		m_word_wrap = false;
	}
	else
	{
		//m_edit.ModifyStyle(ES_AUTOHSCROLL, WS_HSCROLL);
		m_word_wrap = true;
	}
}


//void CSimpleNotePadDlg::OnUpdateWordWrap(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_word_wrap);
//}


void CSimpleNotePadDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//询问是否保存
	if(!SaveInquiry()) return;
	//保存窗口大小
	//if (!IsZoomed())
	//{
	//	CRect rect;
	//	GetWindowRect(&rect);
	//	m_window_width = rect.Width();
	//	m_window_hight = rect.Height();
	//}
	SaveConfig();

	CDialog::OnClose();
}


void CSimpleNotePadDlg::OnFind()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pFindDlg == nullptr)
	{
		m_pFindDlg = new CFindReplaceDialog;
		//初始化“查找”对话框的状态
		if (m_find_no_case)
			m_pFindDlg->m_fr.Flags &= (~FR_MATCHCASE);
		else
			m_pFindDlg->m_fr.Flags |= FR_MATCHCASE;
		if (m_find_whole_word)
			m_pFindDlg->m_fr.Flags |= FR_WHOLEWORD;
		else
			m_pFindDlg->m_fr.Flags &= (~FR_WHOLEWORD);
		m_pFindDlg->Create(TRUE, NULL, NULL, FR_DOWN/* | FR_HIDEWHOLEWORD | FR_HIDEMATCHCASE*/, this);
	}
	m_pFindDlg->ShowWindow(SW_SHOW);
	m_pFindDlg->SetActiveWindow();
}

LONG CSimpleNotePadDlg::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	//m_pFindDlg = CFindReplaceDialog::GetNotifier(lParam);
	if (m_pFindDlg != nullptr)
	{
		m_find_str = m_pFindDlg->GetFindString();
		m_find_down = (m_pFindDlg->SearchDown() != 0);
		m_find_no_case = (m_pFindDlg->MatchCase() == 0);
		m_find_whole_word = (m_pFindDlg->MatchWholeWord() != 0);
		if (m_pFindDlg->FindNext())		//查找下一个时
		{
			OnFindNext();
		}
		if (m_pFindDlg->IsTerminating())	//关闭窗口时
		{
			//m_pFindDlg->DestroyWindow();
			m_pFindDlg = nullptr;
		}
	}
	//delete m_pFindDlg;

	if (m_pReplaceDlg != nullptr)
	{
		m_find_str = m_pReplaceDlg->GetFindString();
		m_replace_str = m_pReplaceDlg->GetReplaceString();
		if (m_pReplaceDlg->FindNext())		//查找下一个时
		{
			OnFindNext();
		}
		if (m_pReplaceDlg->ReplaceCurrent())	//替换当前时
		{
			if (m_find_flag)
			{
				m_edit_wcs.replace(m_find_index, m_find_str.size(), m_replace_str.c_str(), m_replace_str.size());	//替换找到的字符串
				m_edit.SetWindowText(m_edit_wcs.c_str());
				m_modified = true;
				ShowStatusBar();
				OnFindNext();
				m_edit.SetSel(m_find_index, m_find_index + m_find_str.size());	//选中替换的字符串
				SetActiveWindow();		//将编辑器窗口设置活动窗口
			}
			else
			{
				OnFindNext();
			}
		}
		if (m_pReplaceDlg->ReplaceAll())		//替换全部时
		{
			int replace_count{};	//统计替换的字符串的个数
			while (true)
			{
				m_find_index = m_edit_wcs.find(m_find_str, m_find_index + 1);	//查找字符串
				if (m_find_index == string::npos)
					break;
				m_edit_wcs.replace(m_find_index, m_find_str.size(), m_replace_str.c_str(), m_replace_str.size());	//替换找到的字符串
				replace_count++;
			}
			m_edit.SetWindowText(m_edit_wcs.c_str());
			m_modified = true;
			ShowStatusBar();
			if (replace_count != 0)
			{
				CString info;
				info.Format(_T("替换完成，共替换%d个字符串。"),replace_count);
				MessageBox(info, NULL, MB_ICONINFORMATION);
			}
		}
		if (m_pReplaceDlg->IsTerminating())
		{
			m_pReplaceDlg = nullptr;
		}
	}
	return 0;
}


void CSimpleNotePadDlg::OnFindNext()
{
	// TODO: 在此添加命令处理程序代码
	//if (m_find_down)
	//	m_find_index = m_edit_wcs.find(m_find_str, m_find_index + 1);	//向后查找
	//else
	//	m_find_index = m_edit_wcs.rfind(m_find_str, m_find_index - 1);	//向前查找
	m_find_index = CCommon::StringFind(m_edit_wcs, m_find_str, m_find_no_case, m_find_whole_word, m_find_down, (m_find_down ? (m_find_index + 1) : (m_find_index - 1)));
	if (m_find_index == string::npos)
	{
		CString info;
		info.Format(_T("找不到“%s”"), m_find_str.c_str());
		MessageBox(info, NULL, MB_OK | MB_ICONINFORMATION);
		m_find_flag = false;
	}
	else
	{
		m_edit.SetSel(m_find_index, m_find_index + m_find_str.size());		//选中找到的字符串
		SetActiveWindow();		//将编辑器窗口设为活动窗口
		m_find_flag = true;
	}
}

//该函数无效
void CSimpleNotePadDlg::OnMarkAll()
{
	// TODO: 在此添加命令处理程序代码
	if (!m_find_str.empty())
	{
		while (true)
		{
			m_find_index = m_edit_wcs.find(m_find_str, m_find_index + 1);
			if (m_find_index == string::npos) return;
			m_edit.SetHighlight(m_find_index, m_find_index + m_find_str.size());
		}
	}
}


void CSimpleNotePadDlg::OnReplace()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pReplaceDlg == nullptr)
	{
		m_pReplaceDlg = new CFindReplaceDialog;
		m_pReplaceDlg->Create(FALSE, NULL, NULL, FR_DOWN | FR_HIDEWHOLEWORD | FR_HIDEMATCHCASE, this);
	}
	m_pReplaceDlg->ShowWindow(SW_SHOW);
	m_pReplaceDlg->SetActiveWindow();
}


void CSimpleNotePadDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);

	// TODO: 在此处添加消息处理程序代码
	CString menu_tip;
	menu_tip.LoadString(nItemID);
	//int index = menu_tip.Find(_T('\n'));
	//if (index != -1) menu_tip = menu_tip.Left(index);
	if (!menu_tip.IsEmpty())
		m_status_bar.SetText(menu_tip, 0, 0);
	else
		ShowStatusBar();
}


void CSimpleNotePadDlg::OnInitMenu(CMenu* pMenu)
{
	CDialog::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	switch (m_code)
	{
	case CodeType::ANSI: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_ANSI, MF_BYCOMMAND | MF_CHECKED); break;
	case CodeType::UTF8: case CodeType::UTF8_NO_BOM: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_UTF8, MF_BYCOMMAND | MF_CHECKED); break;
	case CodeType::UTF16: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_UTF16, MF_BYCOMMAND | MF_CHECKED); break;
	}
	//switch (m_save_code)
	//{
	//case CodeType::ANSI: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_ANSI, MF_BYCOMMAND | MF_CHECKED); break;
	//case CodeType::UTF8: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_UTF8, MF_BYCOMMAND | MF_CHECKED); break;
	//case CodeType::UTF8_NO_BOM: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_UTF8_NOBOM, MF_BYCOMMAND | MF_CHECKED); break;
	//case CodeType::UTF16: pMenu->CheckMenuRadioItem(ID_SAVE_ANSI, ID_SAVE_UTF16, ID_SAVE_UTF16, MF_BYCOMMAND | MF_CHECKED); break;
	//default: break;
	//}
	pMenu->CheckMenuItem(ID_WORD_WRAP, MF_BYCOMMAND | (m_word_wrap ? MF_CHECKED : MF_UNCHECKED));
}



void CSimpleNotePadDlg::OnFormatConvert()
{
	// TODO: 在此添加命令处理程序代码
	CFormatConvertDlg formatConvertDlg;
	ShowWindow(SW_HIDE);
	formatConvertDlg.DoModal();
	ShowWindow(SW_SHOW);
}


void CSimpleNotePadDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = 200 * m_dpi / 96;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = 150 * m_dpi / 96;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}
