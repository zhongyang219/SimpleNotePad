
// SimpleNotePadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "SimpleNotePadDlg.h"
#include "afxdialogex.h"
#include "HexViewDlg.h"
#include "FormatConvertDlg.h"
#include "InputDlg.h"
#include "CodeConvertDlg.h"
#include "Test.h"
#include "FilePathHelper.h"
#include "SettingsDlg.h"

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
	: CBaseDialog(IDD_SIMPLENOTEPAD_DIALOG, pParent), m_file_path(file_path)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSimpleNotePadDlg::~CSimpleNotePadDlg()
{
}

void CSimpleNotePadDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT1, m_edit);
}

void CSimpleNotePadDlg::OpenFile(LPCTSTR file_path)
{
	CWaitCursor wait_cursor;

	m_edit_str.clear();
    m_view->SetSavePoint();
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

	bool code_confirm = JudgeCode();											//判断编码类型
	m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code, m_code_page);	//转换成Unicode
	if (!code_confirm && m_edit_wcs.size() < m_edit_str.size() / 4)		//如果以自动识别的格式转换成Unicode后，Unicode字符串的长度小于多字节字符串长度的1/4，则文本的编码格式可能是UTF16
	{
		m_code = CodeType::UTF16;
		m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);	//重新转换成Unicode
	}
	m_view->SetText(m_edit_wcs);				//将文件中的内容显示到编缉窗口中
    m_view->EmptyUndoBuffer();
	//m_flag = true;
    CScintillaEditView::eEolMode eol_mode = CScintillaEditView::JudgeEolMode(m_edit_wcs);
    m_view->SetEolMode(eol_mode);
	ShowStatusBar();										//更新状态栏

    //文件打开后为编辑设置语法高亮
    SetEditorSyntaxHight();
}

bool CSimpleNotePadDlg::SaveFile(LPCTSTR file_path, CodeType code, UINT code_page)
{
	bool char_connot_convert;
    if (code_page == CODE_PAGE_DEFAULT)
        code_page = theApp.GetGeneralSettings().default_code_page;
    m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, char_connot_convert, code, code_page);
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
    m_view->SetSavePoint();
	ShowStatusBar();		//保存后刷新状态栏
    SetTitle();
	return true;
}

bool CSimpleNotePadDlg::JudgeCode()
{
	bool rtn = false;
	if (m_edit_str.size() >= 3 && m_edit_str[0] == -17 && m_edit_str[1] == -69 && m_edit_str[2] == -65)
	{
		m_code = CodeType::UTF8;
		rtn = true;
	}
	else if (m_edit_str.size() >= 2 && m_edit_str[0] == -1 && m_edit_str[1] == -2)
	{
		m_code = CodeType::UTF16;
		rtn = true;
	}
	else if (CCommon::IsUTF8Bytes(m_edit_str.c_str()))
	{
		m_code = CodeType::UTF8_NO_BOM;
		rtn = true;
	}
	else
	{
		m_code = CodeType::ANSI;
        m_code_page = theApp.GetGeneralSettings().default_code_page;
	}
	return rtn;
}

void CSimpleNotePadDlg::ShowStatusBar()
{
	//显示编码格式
	CString str{/*_T("编码格式: ")*/};
	//if (!m_edit_wcs.empty())
	//{
	switch (m_code)
	{
	case CodeType::ANSI:
	{
		switch (m_code_page)
		{
		case CODE_PAGE_CHS: str += _T("简体中文(GB2312) "); break;
		case CODE_PAGE_CHT: str += _T("繁体中文(Big5) "); break;
		case CODE_PAGE_JP: str += _T("日文(Shift-JIS) "); break;
		case CODE_PAGE_EN: str += _T("西欧语言 (Windows) "); break;
		case CODE_PAGE_KOR: str += _T("韩文 "); break;
		case CODE_PAGE_THAI: str += _T("泰文 "); break;
		case CODE_PAGE_VIET: str += _T("越南文 "); break;
		default: str += _T("ANSI "); break;
		}
		str += _T("(");
		if (m_code_page == 0)
            str += _T("本地代码页");
		else
			str += std::to_wstring(m_code_page).c_str();
        str += _T(")");
	}
		break;
	case CodeType::UTF8: str += _T("UTF8"); break;
	case CodeType::UTF8_NO_BOM: str += _T("UTF8无BOM"); break;
	case CodeType::UTF16: str += _T("UTF16"); break;
	}
	//}

	m_status_bar.SetText(str, 4, 0);

	//显示字符数
	//if (m_edit_wcs.empty())
	//	str.Empty();
	if (m_edit_str.size() < 1024)
		str.Format(_T("共%d个字节，%d个字符"), m_edit_str.size(), m_edit_wcs.size());
	else
		str.Format(_T("共%d个字节(%dKB)，%d个字符"), m_edit_str.size(), m_edit_str.size() / 1024, m_edit_wcs.size());
	m_status_bar.SetText(str, 0, 0);

	////显示是否修改
	//m_status_bar.SetText(m_modified?_T("已修改"):_T("未修改"), 1, 0);

    //显示换行符
    CScintillaEditView::eEolMode eol_mode = m_view->GetEolMode();
    CString str_eol;
    switch (eol_mode)
    {
    case CScintillaEditView::EOL_CRLF:
        str_eol = _T("CRLF");
        break;
    case CScintillaEditView::EOL_CR:
        str_eol = _T("CR");
        break;
    case CScintillaEditView::EOL_LF:
        str_eol = _T("LF");
        break;
    default:
        break;
    }
    m_status_bar.SetText(str_eol, 2, 0);

    //显示缩放比例
    int scale = 100 + m_zoom * 10;
    CString str_zoom;
    str_zoom.Format(_T("%d%%"), scale);
    m_status_bar.SetText(str_zoom, 3, 0);

    //显示语言
    wstring cur_lan_name = m_syntax_highlight.GetLanguage(m_cur_lan_index).m_name;
    m_status_bar.SetText(cur_lan_name.empty() ? _T("普通文本") : cur_lan_name.c_str(), 1, 0);

}

void CSimpleNotePadDlg::ChangeCode()
{
	m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code, m_code_page);
	m_view->SetText(m_edit_wcs);
    m_view->EmptyUndoBuffer();
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
	theApp.WriteProfileStringW(_T("config"), _T("font_name"), m_font_name);
    theApp.WriteProfileInt(L"config", L"font_size", m_font_size);
    theApp.WriteProfileInt(L"config", L"background_color", m_background_color);

	theApp.WriteProfileInt(L"config", L"word_wrap", m_word_wrap);
    theApp.WriteProfileInt(_T("config"), _T("word_wrap_mode"), m_word_wrap_mode);
	theApp.WriteProfileInt(L"config", L"always_on_top", m_always_on_top);
	theApp.WriteProfileInt(L"config", L"show_line_number", m_show_line_number);
    theApp.WriteProfileInt(L"config", L"show_eol", m_show_eol);

	theApp.WriteProfileInt(L"config", L"find_no_case", m_find_no_case);
	theApp.WriteProfileInt(L"config", L"find_whole_word", m_find_whole_word);

}

void CSimpleNotePadDlg::LoadConfig()
{
	//载入字体设置
	m_font_name = theApp.GetProfileStringW(_T("config"), _T("font_name"), _T("微软雅黑"));
	m_font_size = theApp.GetProfileInt(_T("config"), _T("font_size"), 10);
    m_background_color = theApp.GetProfileInt(_T("config"), _T("background_color"), RGB(255, 255, 255));
	////载入窗口大小
	//m_window_width = theApp.GetProfileInt(_T("config"), _T("window_width"), 560);
	//m_window_hight = theApp.GetProfileInt(_T("config"), _T("window_hight"), 350);
	m_word_wrap = (theApp.GetProfileInt(_T("config"), _T("word_wrap"), 1) != 0);
    m_word_wrap_mode = static_cast<CScintillaEditView::eWordWrapMode>(theApp.GetProfileInt(_T("config"), _T("word_wrap_mode"), CScintillaEditView::WW_WORD));
	m_always_on_top = (theApp.GetProfileInt(_T("config"), _T("always_on_top"), 0) != 0);
    m_show_line_number = (theApp.GetProfileInt(_T("config"), _T("show_line_number"), 0) != 0);
    m_show_eol = (theApp.GetProfileInt(_T("config"), _T("show_eol"), 0) != 0);

	m_find_no_case = (theApp.GetProfileInt(_T("config"), _T("find_no_case"), 0) != 0);
	m_find_whole_word = (theApp.GetProfileInt(_T("config"), _T("find_whole_word"), 0) != 0);

}

bool CSimpleNotePadDlg::SaveInquiry(LPCTSTR info)
{
	if (m_view->IsModified())
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
		case IDYES:
            return _OnFileSave();
		case IDNO:
            m_view->SetSavePoint();
            SetTitle();
            break;
		default:
            return false;
		}
	}
	return true;
}

void CSimpleNotePadDlg::SetTitle()
{
    CString str_title;
    if (m_view->IsModified())
        str_title += _T('*');
	if(!m_file_path.IsEmpty())
        str_title += m_file_path + _T(" - SimpleNotePad");
	else
        str_title += _T("无标题 - SimpleNotePad");
    SetWindowText(str_title);
}

void CSimpleNotePadDlg::GetStatusbarWidth(std::vector<int>& part_widths)
{
    const int PARTS = 5;
    part_widths.resize(PARTS);

    const int WIDTHS = PARTS - 1;
    std::vector<int> widths;
    widths.resize(WIDTHS);
    widths[WIDTHS - 1] = theApp.DPI(174);
    widths[WIDTHS - 2] = theApp.DPI(60);
    widths[WIDTHS - 3] = theApp.DPI(40);
    widths[WIDTHS - 4] = theApp.DPI(60);

    CRect rect;
    GetClientRect(rect);

    for (int i = 0; i < PARTS - 1; i++)
    {
        part_widths[i] = rect.Width();
        for (int j = WIDTHS - 1; j >= i; j--)
        {
            part_widths[i] -= widths[j];
        }
    }
    part_widths[PARTS - 1] = -1;
}

bool CSimpleNotePadDlg::_OnFileSave()
{
	if (m_view->IsModified())		//只有在已更改过之后才保存
	{
		//已经打开过一个文件时就直接保存，还没有打开一个文件就弹出“另存为”对话框
		if (!m_file_path.IsEmpty())
		{
			if (SaveFile(m_file_path, m_code, m_code_page))
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

    for (size_t i{}; i < CONST_VAL::code_list.size(); i++)
    {
        fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, i, CONST_VAL::code_list[i].name);
    }

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
        if (selected_item >= 0 && selected_item < static_cast<DWORD>(CONST_VAL::code_list.size()))
        {
		    m_save_code = CONST_VAL::code_list[selected_item].code_type;
		    UINT save_code_page = CONST_VAL::code_list[selected_item].code_page;
            if (save_code_page == CODE_PAGE_DEFAULT)
                save_code_page = theApp.GetGeneralSettings().default_code_page;
		    if (SaveFile(fileDlg.GetPathName().GetString(), m_save_code, save_code_page))
		    {
			    m_file_path = fileDlg.GetPathName();	//另存为后，当前文件名为保存的文件名
			    SetTitle();					//用新的文件名设置标题
			    m_code = m_save_code;		//另存为后当前编码类型设置为另存为的编码类型
			    ShowStatusBar();			//刷新状态栏
			    return true;
		    }

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
    m_view->SetSavePoint();
    SetTitle();
	MessageBox(_T("十六进制编辑的更改已保存"), NULL, MB_ICONINFORMATION);
}

void CSimpleNotePadDlg::SetAlwaysOnTop()
{
    if (m_always_on_top)
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//设置置顶
    else
        SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//取消置顶
}

CString CSimpleNotePadDlg::GetDialogName() const
{
	return _T("MainWindow");
}

void CSimpleNotePadDlg::SetSyntaxHight(const CLanguage& lan)
{
    m_view->SetLexerNormalText();
    if (!lan.m_name.empty())
    {
        //设置语法解析器
        m_view->SetLexer(lan.m_id);
        //设置关键字
        for (const auto& keywords : lan.m_keywords_list)
        {
            m_view->SetKeywords(keywords.first, keywords.second.c_str());
        }
        //设置样式
        for (const auto& syntax_style : lan.m_syntax_list)
        {
            m_view->SetSyntaxColor(syntax_style.id, syntax_style.color);
            m_view->SetSyntaxFontStyle(syntax_style.id, syntax_style.bold, syntax_style.italic);
        }
        m_cur_lan_index = m_syntax_highlight.IndexOf(lan.m_name);
    }
    else
    {
        m_cur_lan_index = -1;
    }
    m_view->SetFold();
    ShowStatusBar();
}

void CSimpleNotePadDlg::SetEditorSyntaxHight()
{
    wstring wcs_file_path = m_file_path.GetString();
    CFilePathHelper helper(wcs_file_path);
    CLanguage lan = m_syntax_highlight.FindLanguageByFileName(helper.GetFileName());
    SetSyntaxHight(lan);
}

void CSimpleNotePadDlg::CreateFontObject()
{
    //如果m_font已经关联了一个字体资源对象，则释放它
    if (m_font.m_hObject)
    {
    	m_font.DeleteObject();
    }
    m_font.CreatePointFont(m_font_size * 10, m_font_name);
    m_pDC->SelectObject(&m_font);
}

void CSimpleNotePadDlg::UpdateLineNumberWidth(bool update)
{
    int current_line = m_view->GetFirstVisibleLine();
    int leng_height = m_view->GetLineHeight();
    int line_per_page = m_window_size.cy / leng_height;
    int line_number = current_line + line_per_page;
    int digits = static_cast<int>(std::log10(line_number)) + 1;
    if (digits < 2)
        digits = 2;
    static int last_digits{ -1 };
    if (update || last_digits != digits)
    {
        CString str_line_number = std::to_wstring(line_number).c_str();
        int width = m_pDC->GetTextExtent(str_line_number).cx + theApp.DPI(8);
        m_view->SetLineNumberWidth(width);
        m_view->ShowLineNumber(m_show_line_number);
    }
    last_digits = digits;
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


BEGIN_MESSAGE_MAP(CSimpleNotePadDlg, CBaseDialog)
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
	//ON_EN_CHANGE(IDC_EDIT1, &CSimpleNotePadDlg::OnEnChangeEdit1)
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
    ON_REGISTERED_MESSAGE(WM_FINDREPLACE, &CSimpleNotePadDlg::OnFindReplace)
    ON_COMMAND(ID_FIND_NEXT, &CSimpleNotePadDlg::OnFindNext)
	ON_COMMAND(ID_MARK_ALL, &CSimpleNotePadDlg::OnMarkAll)
	ON_COMMAND(ID_EDIT_REPLACE, &CSimpleNotePadDlg::OnReplace)
	ON_WM_MENUSELECT()
	ON_WM_INITMENU()
	ON_COMMAND(ID_FORMAT_CONVERT, &CSimpleNotePadDlg::OnFormatConvert)
//	ON_WM_GETMINMAXINFO()
    ON_COMMAND(ID_ALWAYS_ON_TOP, &CSimpleNotePadDlg::OnAlwaysOnTop)
	ON_COMMAND(ID_CODE_PAGE_CHS, &CSimpleNotePadDlg::OnCodePageChs)
	ON_COMMAND(ID_CODE_PAGE_CHT, &CSimpleNotePadDlg::OnCodePageCht)
	ON_COMMAND(ID_CODE_PAGE_JP, &CSimpleNotePadDlg::OnCodePageJp)
	ON_COMMAND(ID_CODE_PAGE_EN, &CSimpleNotePadDlg::OnCodePageEn)
	ON_COMMAND(ID_CODE_PAGE_KOR, &CSimpleNotePadDlg::OnCodePageKor)
	ON_COMMAND(ID_CODE_PAGE_THAI, &CSimpleNotePadDlg::OnCodePageThai)
	ON_COMMAND(ID_CODE_PAGE_VIET, &CSimpleNotePadDlg::OnCodePageViet)
	ON_COMMAND(ID_SEPCIFY_CODE_PAGE, &CSimpleNotePadDlg::OnSepcifyCodePage)
    //ON_COMMAND(ID_CODE_PAGE_LOCAL, &CSimpleNotePadDlg::OnCodePageLocal)
    ON_COMMAND(ID_TOOL_OPTIONS, &CSimpleNotePadDlg::OnToolOptions)
    ON_COMMAND(ID_CODE_CONVERT, &CSimpleNotePadDlg::OnCodeConvert)
    ON_COMMAND(ID_SHOW_LINE_NUMBER, &CSimpleNotePadDlg::OnShowLineNumber)
    ON_COMMAND(ID_EOL_CRLF, &CSimpleNotePadDlg::OnEolCrlf)
    ON_COMMAND(ID_EOL_CR, &CSimpleNotePadDlg::OnEolCr)
    ON_COMMAND(ID_EOL_LF, &CSimpleNotePadDlg::OnEolLf)
    ON_COMMAND(ID_SHOW_EOL, &CSimpleNotePadDlg::OnShowEol)
    ON_COMMAND(ID_CONVERT_TO_CAPITAL, &CSimpleNotePadDlg::OnConvertToCapital)
    ON_COMMAND(ID_CONVERT_TO_LOWER_CASE, &CSimpleNotePadDlg::OnConvertToLowerCase)
    ON_COMMAND(ID_CONVERT_TO_TITLE_CASE, &CSimpleNotePadDlg::OnConvertToTitleCase)
    ON_WM_DESTROY()
    ON_COMMAND(ID_FILE_OPEN_LOCATION, &CSimpleNotePadDlg::OnFileOpenLocation)
    ON_COMMAND(ID_WORD_WRAP_WORD, &CSimpleNotePadDlg::OnWordWrapWord)
    ON_COMMAND(ID_WORD_CHARACTER, &CSimpleNotePadDlg::OnWordCharacter)
    ON_COMMAND(ID_WORD_WRAP_WHITESPACE, &CSimpleNotePadDlg::OnWordWrapWhitespace)
END_MESSAGE_MAP()

// CSimpleNotePadDlg 消息处理程序

BOOL CSimpleNotePadDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

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
    m_pDC = GetDC();

    //载入设置
	LoadConfig();
	////初始化窗口大小
	//SetWindowPos(nullptr, 0, 0, m_window_width, m_window_hight, SWP_NOZORDER | SWP_NOMOVE);

    m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
    m_context_menu.LoadMenu(IDR_POPUP_MENU);

    //加载语法高亮设置
    wstring path;
#ifdef DEBUG
    path = L"./lang.xml";
#else
    path = CCommon::GetExePath() + L"lang.xml";
#endif
    m_syntax_highlight.LoadFromFile(path.c_str());

	//根据当前系统DPI设置设置状态栏大小
    theApp.DPIFromWindow(this);

	m_status_bar_hight = theApp.DPI(20);
	m_edit_bottom_space = theApp.DPI(22);

	SetMinSize(theApp.DPI(200), theApp.DPI(150));

	//初始化编辑框大小
	CRect rect;
	GetClientRect(&rect);
	//rect.bottom = rect.bottom - 22;
	rect.bottom = rect.bottom - m_edit_bottom_space - theApp.DPI(2);
	//m_edit.MoveWindow(rect);

    m_view = (CScintillaEditView*)RUNTIME_CLASS(CScintillaEditView)->CreateObject();
    m_view->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, this, 3000);
    m_view->OnInitialUpdate();
    m_view->ShowWindow(SW_SHOW);

    m_view->SetWordWrap(m_word_wrap, m_word_wrap_mode);

    CreateFontObject();
    UpdateLineNumberWidth();
    m_view->ShowLineNumber(m_show_line_number);
    m_view->SetLineNumberColor(RGB(75, 145, 175));
    m_view->SetViewEol(m_show_eol);
    m_view->SetBackgroundColor(m_background_color);
    m_view->SetContextMenu(m_context_menu.GetSubMenu(0), this);

	//初始化状态栏
	GetClientRect(&rect);
	//rect.top = rect.bottom - 20;
	rect.top = rect.bottom - m_status_bar_hight;
	m_status_bar.Create(WS_VISIBLE | CBRS_BOTTOM, rect, this, 3);

    vector<int> parts;
    GetStatusbarWidth(parts);
	m_status_bar.SetParts(parts.size(), parts.data()); //分割状态栏
	ShowStatusBar();

	//初始化字体
	//m_font.CreatePointFont(m_font_size * 10, m_font_name);
	//m_edit.SetFont(&m_font);
    m_view->SetFontFace(m_font_name.GetString());
    m_view->SetFontSize(m_font_size);

    m_view->SetLexerNormalText();

    //初始化语言菜单
    CMenu* pMenu = GetMenu();
    if (pMenu != nullptr)
    {
        CMenu* pLanguageMenu = pMenu->GetSubMenu(3)->GetSubMenu(7);
        ASSERT(pLanguageMenu != nullptr);
        if (pLanguageMenu != nullptr)
        {
            int index = 0;
            for (const auto& language_item : m_syntax_highlight.GetLanguageList())
            {
                pLanguageMenu->AppendMenuW(MF_STRING | MF_ENABLED, ID_LANGUAGE_NORMAL_TEXT + index + 1, language_item.m_name.c_str());
                index++;
            }
        }
    }

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
	//m_edit.SetLimitText(static_cast<UINT>(-1));

	//设置制表符宽度
    m_view->SetTabSize(4);

    SetAlwaysOnTop();

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
		CBaseDialog::OnSysCommand(nID, lParam);
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
		CBaseDialog::OnPaint();
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
	CBaseDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect size;		//编辑框矩形区域
	size.right = cx;
	//size.bottom = cy - 22;		//窗口下方状态栏占20个像素高度
	size.bottom = cy - m_edit_bottom_space;		//窗口下方状态栏占20个像素高度
	if (nType != SIZE_MINIMIZED && m_view->GetSafeHwnd() != NULL)
	{
        //窗口大小改变时改变编辑框大小
        m_view->SetWindowPos(nullptr, 0, 0, size.Width(), size.Height() - theApp.DPI(2), SWP_NOMOVE | SWP_NOZORDER);
	}

	CRect status_bar_size;
	status_bar_size.right = cx;
	status_bar_size.bottom = cy;
	//status_bar_size.top = cy - 20;
	status_bar_size.top = cy - m_status_bar_hight;
	if (nType != SIZE_MINIMIZED && m_status_bar.m_hWnd != NULL)
	{
		m_status_bar.MoveWindow(status_bar_size);
		//int nParts[3] = { cx - m_status_bar_right_width - m_status_bar_mid_width, cx - m_status_bar_right_width, -1 }; //分割尺寸
        vector<int> parts;
        GetStatusbarWidth(parts);

        m_status_bar.SetParts(parts.size(), parts.data()); //分割状态栏
	}
	//if (nType != SIZE_MAXIMIZED && nType != SIZE_MINIMIZED)
	//{
	//	//m_window_width = cx;
	//	//m_window_hight = cy;
	//	CRect rect;
	//	GetWindowRect(&rect);
	//	m_window_width = rect.Width();
	//	m_window_hight = rect.Height();
	//}
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
    m_code_page = CP_ACP;
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
	//m_font.GetLogFont(&lf);
	_tcscpy_s(lf.lfFaceName, LF_FACESIZE, m_font_name.GetString());	//将lf中的元素字体名设为“微软雅黑”
    lf.lfHeight = CCommon::FontSizeToLfHeight(m_font_size);
    CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
	if (IDOK == fontDlg.DoModal())     // 显示字体对话框
	{
		////如果m_font已经关联了一个字体资源对象，则释放它
		//if (m_font.m_hObject)
		//{
		//	m_font.DeleteObject();
		//}
		//使用选定字体的LOGFONT创建新的字体
		//m_font.CreateFontIndirect(fontDlg.m_cf.lpLogFont);
		//获取字体信息
		m_font_name = fontDlg.m_cf.lpLogFont->lfFaceName;
		m_font_size = fontDlg.m_cf.iPointSize / 10;
		//设置字体
        m_view->SetFontFace(m_font_name.GetString());
        m_view->SetFontSize(m_font_size);
        CreateFontObject();
        UpdateLineNumberWidth(true);
        //设置字体后重新设置一下语法高亮，以解决字体设置无法立即生效的问题
        SetSyntaxHight(m_syntax_highlight.GetLanguage(m_cur_lan_index));
		//将字体设置写入到ini文件
		SaveConfig();
	}
}


BOOL CSimpleNotePadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

    if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
    {
        //响应Accelerator中设置的快捷键
        if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
            return TRUE;
    }

	//屏蔽ESC键退出
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (GetKeyState(VK_CONTROL) & 0x80)
	{
#ifdef DEBUG
        //else if (pMsg->wParam == 'Q')
        //{
        //    CTest::Test();
        //    return TRUE;
        //}

#endif // DEBUG
	}

    return CBaseDialog::PreTranslateMessage(pMsg);
}


void CSimpleNotePadDlg::OnEditUndo()
{
	// TODO: 在此添加命令处理程序代码
	m_view->Undo();
}


void CSimpleNotePadDlg::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码
    m_view->Cut();
}


void CSimpleNotePadDlg::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
    m_view->Copy();
}


void CSimpleNotePadDlg::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
    m_view->Paste();
}


void CSimpleNotePadDlg::OnEditSelectAll()
{
	// TODO: 在此添加命令处理程序代码
    m_view->SelectAll();
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
		if (m_view->IsModified())
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
            m_view->SetText(m_edit_wcs);
            m_view->EmptyUndoBuffer();
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
//	CBaseDialog::OnDestroy();
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

	CBaseDialog::OnDropFiles(hDropInfo);
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
    m_view->SetText(L"");
    m_view->EmptyUndoBuffer();
    m_code = CodeType::ANSI;
    m_view->SetSavePoint();
    ShowStatusBar();
	SetWindowText(_T("无标题 - SimpleNotePad"));
}


void CSimpleNotePadDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1234)
	{
		KillTimer(1234);		//定时器响应一次后就将其销毁
		OpenFile(m_file_path);		//如果文件是通过命令行打开的，则延时100毫秒再打开
	}

	CBaseDialog::OnTimer(nIDEvent);
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
    m_word_wrap = !m_word_wrap;
    m_view->SetWordWrap(m_word_wrap, m_word_wrap_mode);
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

	CBaseDialog::OnClose();
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

afx_msg LRESULT CSimpleNotePadDlg::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	m_pFindDlg = CFindReplaceDialog::GetNotifier(lParam);
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
                m_view->SetText(m_edit_wcs);
				//m_modified = true;
				SetTitle();
				OnFindNext();
				m_view->SetSel(m_find_index, m_find_index + m_find_str.size(), m_edit_wcs);	//选中替换的字符串
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
            m_view->SetText(m_edit_wcs);
            //m_modified = true;
            SetTitle();
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
        m_view->SetSel(m_find_index, m_find_index + m_find_str.size(), m_edit_wcs);		//选中找到的字符串
		SetActiveWindow();		//将编辑器窗口设为活动窗口
		m_find_flag = true;
	}
}

//该函数无效
void CSimpleNotePadDlg::OnMarkAll()
{
	// TODO: 在此添加命令处理程序代码
	//if (!m_find_str.empty())
	//{
	//	while (true)
	//	{
	//		m_find_index = m_edit_wcs.find(m_find_str, m_find_index + 1);
	//		if (m_find_index == string::npos) return;
	//		m_edit.SetHighlight(m_find_index, m_find_index + m_find_str.size());
	//	}
	//}
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
	CBaseDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);

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
	CBaseDialog::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
    pMenu->EnableMenuItem(ID_FILE_OPEN_LOCATION, m_file_path.IsEmpty() ? MF_GRAYED : MF_ENABLED);

    if (m_code == CodeType::ANSI && m_code_page != CP_ACP)
    {
        pMenu->CheckMenuItem(ID_CODE_ANSI, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_CODE_UTF8, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_CODE_UTF16, MF_UNCHECKED);
    }
    else
    {
	    switch (m_code)
	    {
	    case CodeType::ANSI: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_ANSI, MF_BYCOMMAND | MF_CHECKED); break;
	    case CodeType::UTF8: case CodeType::UTF8_NO_BOM: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_UTF8, MF_BYCOMMAND | MF_CHECKED); break;
	    case CodeType::UTF16: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16, ID_CODE_UTF16, MF_BYCOMMAND | MF_CHECKED); break;
	    }
    }

	if (m_code == CodeType::ANSI)
	{
		switch (m_code_page)
		{
		case CODE_PAGE_CHS: pMenu->CheckMenuRadioItem(ID_CODE_PAGE_CHS, ID_CODE_PAGE_VIET, ID_CODE_PAGE_CHS, MF_BYCOMMAND | MF_CHECKED); break;
		case CODE_PAGE_CHT: pMenu->CheckMenuRadioItem(ID_CODE_PAGE_CHS, ID_CODE_PAGE_VIET, ID_CODE_PAGE_CHT, MF_BYCOMMAND | MF_CHECKED); break;
		case CODE_PAGE_JP: pMenu->CheckMenuRadioItem(ID_CODE_PAGE_CHS, ID_CODE_PAGE_VIET, ID_CODE_PAGE_JP, MF_BYCOMMAND | MF_CHECKED); break;
		case CODE_PAGE_EN: pMenu->CheckMenuRadioItem(ID_CODE_PAGE_CHS, ID_CODE_PAGE_VIET, ID_CODE_PAGE_EN, MF_BYCOMMAND | MF_CHECKED); break;
		case CODE_PAGE_KOR: pMenu->CheckMenuRadioItem(ID_CODE_PAGE_CHS, ID_CODE_PAGE_VIET, ID_CODE_PAGE_KOR, MF_BYCOMMAND | MF_CHECKED); break;
		case CODE_PAGE_THAI: pMenu->CheckMenuRadioItem(ID_CODE_PAGE_CHS, ID_CODE_PAGE_VIET, ID_CODE_PAGE_THAI, MF_BYCOMMAND | MF_CHECKED); break;
		case CODE_PAGE_VIET: pMenu->CheckMenuRadioItem(ID_CODE_PAGE_CHS, ID_CODE_PAGE_VIET, ID_CODE_PAGE_VIET, MF_BYCOMMAND | MF_CHECKED); break;
        default: goto clear_selected;

		}
	}
	else
	{
        clear_selected:
		ASSERT(ID_CODE_PAGE_CHS < ID_CODE_PAGE_VIET);
		for (UINT i = ID_CODE_PAGE_CHS; i <= ID_CODE_PAGE_VIET; i++)
		{
			pMenu->CheckMenuItem(i, MF_UNCHECKED);
			//pMenu->CheckMenuRadioItem(ID_CODE_PAGE_LOCAL, ID_CODE_PAGE_VIET, i, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}

	pMenu->CheckMenuItem(ID_WORD_WRAP, MF_BYCOMMAND | (m_word_wrap ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_ALWAYS_ON_TOP, MF_BYCOMMAND | (m_always_on_top ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_LINE_NUMBER, MF_BYCOMMAND | (m_show_line_number ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_EOL, MF_BYCOMMAND | (m_show_eol ? MF_CHECKED : MF_UNCHECKED));

    bool is_selection_empty = m_view->IsSelectionEmpty();
    pMenu->EnableMenuItem(ID_EDIT_COPY, is_selection_empty ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_EDIT_CUT, is_selection_empty ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_EDIT_UNDO, m_view->CanUndo() ? MF_ENABLED : MF_GRAYED);
    pMenu->EnableMenuItem(ID_EDIT_REDO, m_view->CanRedo() ? MF_ENABLED : MF_GRAYED);
    pMenu->EnableMenuItem(ID_EDIT_PASTE, m_view->CanPaste() ? MF_ENABLED : MF_GRAYED);
    pMenu->EnableMenuItem(ID_CONVERT_TO_CAPITAL, is_selection_empty ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_CONVERT_TO_LOWER_CASE, is_selection_empty ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_CONVERT_TO_TITLE_CASE, is_selection_empty ? MF_GRAYED : MF_ENABLED);

    //pMenu->EnableMenuItem(ID_WORD_WRAP, MF_GRAYED);

    CScintillaEditView::eEolMode eolMode = m_view->GetEolMode();
    switch (eolMode)
    {
    case CScintillaEditView::EOL_CRLF:
        pMenu->CheckMenuRadioItem(ID_EOL_CRLF, ID_EOL_LF, ID_EOL_CRLF, MF_BYCOMMAND | MF_CHECKED);
        break;
    case CScintillaEditView::EOL_CR:
        pMenu->CheckMenuRadioItem(ID_EOL_CRLF, ID_EOL_LF, ID_EOL_CR, MF_BYCOMMAND | MF_CHECKED);
        break;
    case CScintillaEditView::EOL_LF:
        pMenu->CheckMenuRadioItem(ID_EOL_CRLF, ID_EOL_LF, ID_EOL_LF, MF_BYCOMMAND | MF_CHECKED);
        break;
    }

    switch (m_word_wrap_mode)
    {
    case CScintillaEditView::WW_WORD:
        pMenu->CheckMenuRadioItem(ID_WORD_WRAP_WORD, ID_WORD_WRAP_WHITESPACE, ID_WORD_WRAP_WORD, MF_BYCOMMAND | MF_CHECKED);
        break;
    case CScintillaEditView::WW_CHARACTER:
        pMenu->CheckMenuRadioItem(ID_WORD_WRAP_WORD, ID_WORD_WRAP_WHITESPACE, ID_WORD_CHARACTER, MF_BYCOMMAND | MF_CHECKED);
        break;
    case CScintillaEditView::WW_WHITESPACE:
         pMenu->CheckMenuRadioItem(ID_WORD_WRAP_WORD, ID_WORD_WRAP_WHITESPACE, ID_WORD_WRAP_WHITESPACE, MF_BYCOMMAND | MF_CHECKED);
       break;
    default:
        break;
    }

    //设置语言子菜单下的选中
    int id;
    if (m_cur_lan_index < 0)
        id = 0;
    else
        id = m_cur_lan_index + 1;
    pMenu->CheckMenuRadioItem(ID_LANGUAGE_NORMAL_TEXT, ID_LANGUAGE_NORMAL_TEXT + m_syntax_highlight.GetLanguageList().size(), ID_LANGUAGE_NORMAL_TEXT + id, MF_BYCOMMAND | MF_CHECKED);
}



void CSimpleNotePadDlg::OnFormatConvert()
{
	// TODO: 在此添加命令处理程序代码
	CFormatConvertDlg formatConvertDlg;
	ShowWindow(SW_HIDE);
	formatConvertDlg.DoModal();
	ShowWindow(SW_SHOW);
}


//void CSimpleNotePadDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	lpMMI->ptMinTrackSize.x = 200 * m_dpi / 96;		//设置最小宽度
//	lpMMI->ptMinTrackSize.y = 150 * m_dpi / 96;		//设置最小高度
//
//	CBaseDialog::OnGetMinMaxInfo(lpMMI);
//}


void CSimpleNotePadDlg::OnAlwaysOnTop()
{
    // TODO: 在此添加命令处理程序代码
    m_always_on_top = !m_always_on_top;
    SetAlwaysOnTop();
}


void CSimpleNotePadDlg::OnCodePageChs()
{
	// TODO: 在此添加命令处理程序代码
	if (!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	m_code_page = CODE_PAGE_CHS;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodePageCht()
{
	// TODO: 在此添加命令处理程序代码
	if (!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	m_code_page = CODE_PAGE_CHT;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodePageJp()
{
	// TODO: 在此添加命令处理程序代码
	if (!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	m_code_page = CODE_PAGE_JP;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodePageEn()
{
	// TODO: 在此添加命令处理程序代码
	if (!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	m_code_page = CODE_PAGE_EN;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodePageKor()
{
	// TODO: 在此添加命令处理程序代码
	if (!BeforeChangeCode()) return;
	if (!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	m_code_page = CODE_PAGE_KOR;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodePageThai()
{
	// TODO: 在此添加命令处理程序代码
	if (!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	m_code_page = CODE_PAGE_THAI;
	ChangeCode();
}


void CSimpleNotePadDlg::OnCodePageViet()
{
	// TODO: 在此添加命令处理程序代码
	if (!BeforeChangeCode()) return;
	m_code = CodeType::ANSI;
	m_code_page = CODE_PAGE_VIET;
	ChangeCode();
}


void CSimpleNotePadDlg::OnSepcifyCodePage()
{
	// TODO: 在此添加命令处理程序代码
	CInputDlg inputDlg;
	inputDlg.SetTitle(_T("输入代码页"));
	inputDlg.SetInfoText(_T("请输入代码页："));
	if (inputDlg.DoModal() == IDOK)
	{
		if (!BeforeChangeCode()) return;
		m_code = CodeType::ANSI;
		m_code_page = _ttoi(inputDlg.GetEditText().GetString());
		ChangeCode();
	}
}


//void CSimpleNotePadDlg::OnCodePageLocal()
//{
//    // TODO: 在此添加命令处理程序代码
//    if (!BeforeChangeCode()) return;
//	m_code = CodeType::ANSI;
//	m_code_page = CP_ACP;
//    ChangeCode();
//
//}


void CSimpleNotePadDlg::OnToolOptions()
{
    // TODO: 在此添加命令处理程序代码
    CSettingsDlg dlg;
    dlg.LoadSettings();
    if (dlg.DoModal() == IDOK)
    {
        dlg.SaveSettings();
    }
}


void CSimpleNotePadDlg::OnCodeConvert()
{
    // TODO: 在此添加命令处理程序代码
    CCodeConvertDlg dlg;
    ShowWindow(SW_HIDE);
    dlg.DoModal();
    ShowWindow(SW_SHOW);
}


BOOL CSimpleNotePadDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: 在此添加专用代码和/或调用基类
    SCNotification *notification = reinterpret_cast<SCNotification *>(lParam);
    if (notification->nmhdr.hwndFrom == m_view->GetSafeHwnd())
    {
        //响应编辑器文本变化
        if (notification->nmhdr.code == SCN_MODIFIED && m_view->IsEditChangeNotificationEnable())
        {
            UINT marsk = (SC_MOD_DELETETEXT | SC_MOD_INSERTTEXT | SC_PERFORMED_UNDO | SC_PERFORMED_REDO);
            if ((notification->modificationType & marsk) != 0)
            {
                m_view->GetText(m_edit_wcs);
                ShowStatusBar();
                SetTitle();
            }
        }
        else if (notification->nmhdr.code == SCN_ZOOM)
        {
            m_zoom = m_view->GetZoom();
            ShowStatusBar();
        }
        else if (notification->nmhdr.code == SCN_MARGINCLICK)
        {
            const int line_number = m_view->SendMessage(SCI_LINEFROMPOSITION, notification->position);
            m_view->SendMessage(SCI_TOGGLEFOLD, line_number);
        }
        else if (notification->nmhdr.code == SCN_UPDATEUI)
        {
            //垂直滚动条位置变化
            if ((notification->updated & SC_UPDATE_V_SCROLL) != 0)
            {
                UpdateLineNumberWidth();
            }
        }
    }

    return CBaseDialog::OnNotify(wParam, lParam, pResult);
}


void CSimpleNotePadDlg::OnShowLineNumber()
{
    // TODO: 在此添加命令处理程序代码
    m_show_line_number = !m_show_line_number;
    m_view->ShowLineNumber(m_show_line_number);
}


void CSimpleNotePadDlg::OnEolCrlf()
{
    // TODO: 在此添加命令处理程序代码
    m_view->SetEolMode(CScintillaEditView::EOL_CRLF);
    m_view->ConvertEolMode(CScintillaEditView::EOL_CRLF);
    ShowStatusBar();
}


void CSimpleNotePadDlg::OnEolCr()
{
    // TODO: 在此添加命令处理程序代码
    m_view->SetEolMode(CScintillaEditView::EOL_CR);
    m_view->ConvertEolMode(CScintillaEditView::EOL_CR);
    ShowStatusBar();
}


void CSimpleNotePadDlg::OnEolLf()
{
    // TODO: 在此添加命令处理程序代码
    m_view->SetEolMode(CScintillaEditView::EOL_LF);
    m_view->ConvertEolMode(CScintillaEditView::EOL_LF);
    ShowStatusBar();
}


void CSimpleNotePadDlg::OnShowEol()
{
    // TODO: 在此添加命令处理程序代码
    m_show_eol = !m_show_eol;
    m_view->SetViewEol(m_show_eol);
}


BOOL CSimpleNotePadDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    WORD command = LOWORD(wParam);
    if (command > ID_LANGUAGE_NORMAL_TEXT && command < ID_LANGUAGE_MAX)
    {
        CString lan_name;
        GetMenu()->GetMenuStringW(command, lan_name, 0);
        if (!lan_name.IsEmpty())
        {
            CLanguage lan = m_syntax_highlight.FindLanguageByName(lan_name);
            if (!lan.m_name.empty())
            {
                SetSyntaxHight(lan);
            }
        }
    }
    if (command == ID_LANGUAGE_NORMAL_TEXT)
    {
        SetSyntaxHight(CLanguage());
        m_cur_lan_index = -1;
        ShowStatusBar();
    }

    return CBaseDialog::OnCommand(wParam, lParam);
}


void CSimpleNotePadDlg::OnConvertToCapital()
{
    // TODO: 在此添加命令处理程序代码
    if (!m_view->IsSelectionEmpty())
    {
        CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
        int start{}, end{};
        m_view->GetSel(start, end);
        for (int i = start; i < end; i++)
        {
            if (i < static_cast<int>(m_edit_wcs.size()))
                CCommon::ConvertCharCase(m_edit_wcs[i], true);
        }
        m_view->SetText(m_edit_wcs);
        m_view->SetSel(start, end, m_edit_wcs);
        SetTitle();
    }
}


void CSimpleNotePadDlg::OnConvertToLowerCase()
{
    // TODO: 在此添加命令处理程序代码
    if (!m_view->IsSelectionEmpty())
    {
        CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
        int start{}, end{};
        m_view->GetSel(start, end);
        for (int i = start; i < end; i++)
        {
            if (i < static_cast<int>(m_edit_wcs.size()))
                CCommon::ConvertCharCase(m_edit_wcs[i], false);
        }
        m_view->SetText(m_edit_wcs);
        m_view->SetSel(start, end, m_edit_wcs);
        SetTitle();
    }
}


void CSimpleNotePadDlg::OnConvertToTitleCase()
{
    // TODO: 在此添加命令处理程序代码
    if (!m_view->IsSelectionEmpty())
    {
        CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
        int start{}, end{};
        m_view->GetSel(start, end);
        for (int i = start; i < end; i++)
        {
            //如果当前字符是字母
            if (i < static_cast<int>(m_edit_wcs.size()) && i >= 0 && CCommon::IsLetter(m_edit_wcs[i]))
            {
                //如果当前是第0个字符或前一个字符不是字母，则当前字符转换成大写
                if (i == 0 || !CCommon::IsLetter(m_edit_wcs[i - 1]))
                    CCommon::ConvertCharCase(m_edit_wcs[i], true);
                //其他的字符转换成小写
                else
                    CCommon::ConvertCharCase(m_edit_wcs[i], false);
            }
        }
        m_view->SetText(m_edit_wcs);
        m_view->SetSel(start, end, m_edit_wcs);
        SetTitle();
    }
}


void CSimpleNotePadDlg::OnDestroy()
{
    CBaseDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    ReleaseDC(m_pDC);
}


void CSimpleNotePadDlg::OnFileOpenLocation()
{
    // TODO: 在此添加命令处理程序代码
    CString str;
    str.Format(_T("/select,\"%s\""), m_file_path.GetString());
    ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
}


void CSimpleNotePadDlg::OnWordWrapWord()
{
    // TODO: 在此添加命令处理程序代码
    m_word_wrap_mode = CScintillaEditView::WW_WORD;
    m_view->SetWordWrap(m_word_wrap, m_word_wrap_mode);
}


void CSimpleNotePadDlg::OnWordCharacter()
{
    // TODO: 在此添加命令处理程序代码
    m_word_wrap_mode = CScintillaEditView::WW_CHARACTER;
    m_view->SetWordWrap(m_word_wrap, m_word_wrap_mode);
}


void CSimpleNotePadDlg::OnWordWrapWhitespace()
{
    // TODO: 在此添加命令处理程序代码
    m_word_wrap_mode = CScintillaEditView::WW_WHITESPACE;
    m_view->SetWordWrap(m_word_wrap, m_word_wrap_mode);
}
