
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
#include "AboutDlg.h"
#include "WIC.h"
#include "GoToLineDlg.h"
#include "FindReplaceTools.h"
#include "EditorHelper.h"
#include "WinVersionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSimpleNotePadDlg 对话框


CSimpleNotePadDlg::CSimpleNotePadDlg(CString file_path, CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_SIMPLENOTEPAD_DIALOG, pParent), m_file_path(file_path)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSimpleNotePadDlg::~CSimpleNotePadDlg()
{
}

CScintillaEditView* CSimpleNotePadDlg::GetEditView()
{
    return m_view;
}

const CSyntaxHighlight& CSimpleNotePadDlg::GetSyntaxHighlight() const
{
    return m_syntax_highlight;
}

CSimpleNotePadDlg* CSimpleNotePadDlg::Instanse()
{
    return dynamic_cast<CSimpleNotePadDlg*>(AfxGetMainWnd());
}

void CSimpleNotePadDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_EDIT1, m_edit);
}

void CSimpleNotePadDlg::ApplySettings(const SettingsData& general_data, const EditSettingData& eidt_data, const CUserDefinedLanguageStyle& language_data)
{
    bool highlight_changed{ theApp.GetEditSettings().current_line_highlight != eidt_data.current_line_highlight
        || theApp.GetEditSettings().current_line_highlight_color != eidt_data.current_line_highlight_color };
    bool background_color_changed{ theApp.GetEditSettings().background_color != eidt_data.background_color };
    bool selection_color_changed{ theApp.GetEditSettings().selection_back_color != eidt_data.selection_back_color };
    bool font_changed{ theApp.GetEditSettings().font_name != eidt_data.font_name || theApp.GetEditSettings().font_size != eidt_data.font_size };
    bool tab_width_changed{ theApp.GetEditSettings().tab_width != eidt_data.tab_width };
    bool clear_selection_mark{ !theApp.GetEditSettings().mark_same_words && eidt_data.mark_same_words };
    bool clear_html_mark{ !theApp.GetEditSettings().mark_matched_html_mark && eidt_data.mark_matched_html_mark };
    bool clear_matched_brackets{ !theApp.GetEditSettings().mark_matched_brackets && eidt_data.mark_matched_brackets };

    theApp.SetGeneralSettings(general_data);
    theApp.SetEditSettings(eidt_data);
    theApp.SetLanguageSettings(language_data);


    //如果当前行高亮设置发生了变化
    if (highlight_changed)
    {
        m_view->SetCurrentLineHighlightColor(theApp.GetEditSettings().current_line_highlight_color);
        m_view->SetCurrentLineHighlight(theApp.GetEditSettings().current_line_highlight);
    }

    //如果背景颜色设置发生了变化
    if (background_color_changed)
    {
        m_view->SetBackgroundColor(theApp.GetEditSettings().background_color);
        //SetEditorSyntaxHight();
    }

    if (selection_color_changed)
    {
        m_view->SetSelectionBackColor(theApp.GetEditSettings().selection_back_color);
    }

    //如果字体设置发生发变化
    if (font_changed)
    {
        //设置字体
        m_view->SetFontFaceW(theApp.GetEditSettings().font_name.GetString());
        m_view->SetFontSize(theApp.GetEditSettings().font_size);
        UpdateLineNumberWidth(true);
        //设置字体后重新设置一下语法高亮，以解决字体设置无法立即生效的问题
        //SetSyntaxHight(m_syntax_highlight.GetLanguage(m_cur_lan_index));
    }

    if (tab_width_changed)
    {
        m_view->SetTabSize(theApp.GetEditSettings().tab_width);
    }

    m_view->ShowIndentationGuides(theApp.GetEditSettings().show_indentation_guides);

    if (clear_selection_mark)
    {
        m_view->ClearAllMark(CScintillaEditView::MarkStyle::SELECTION_MARK);
    }

    if (clear_html_mark)
    {
        m_view->ClearAllMark(CScintillaEditView::MarkStyle::HTML_MARKS);
    }

    if (clear_matched_brackets)
    {
        m_view->ClearAllMark(CScintillaEditView::MarkStyle::MATCHED_BRACKETS);
    }
    //重新设置语法高亮
    SetEditorSyntaxHight();

    theApp.SaveConfig();
}

void CSimpleNotePadDlg::OpenFile()
{
    CWaitCursor wait_cursor;

    m_edit_str.clear();
    m_view->SetSavePoint();
    ifstream file{ m_file_path.GetString(), std::ios::binary };
    if (file.fail())
    {
        MessageBox(CCommon::LoadTextFormat(IDS_CANNOT_OPEN_FILE_WARNING, { m_file_path }), NULL, MB_OK | MB_ICONSTOP);
        theApp.RemoveFromRecentFileList(m_file_path);
        m_file_path.Empty();
        return;
    }

    //获取文件长度
    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);
    if (length > MAX_FILE_SIZE)	//当文件大小超过MAX_FILE_SIZE时禁止打开
    {
        CString info = CCommon::LoadTextFormat(IDS_FILE_TOO_LARGE_WARNING, { m_file_path, MAX_FILE_SIZE / 1024 / 1024 });
        if (MessageBox(info, NULL, MB_YESNO | MB_ICONQUESTION) != IDYES)
        {
            m_file_path.Empty();
            m_edit_str.clear();
            return;
        }
        length = MAX_FILE_SIZE;
    }

    //读取数据
    char* buff = new char[length];
    file.read(buff, length);
    file.close();
    m_edit_str.assign(buff, length);
    delete[] buff;

    bool code_confirm = JudgeCode();											//判断编码类型
    m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code, m_code_page);	//转换成Unicode
    if (!code_confirm && m_edit_wcs.size() < m_edit_str.size() / 4)		//如果以自动识别的格式转换成Unicode后，Unicode字符串的长度小于多字节字符串长度的1/4，则文本的编码格式可能是UTF16
    {
        m_code = CodeType::UTF16;
        m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);	//重新转换成Unicode
    }
    m_view->SetTextW(m_edit_wcs);				//将文件中的内容显示到编缉窗口中
    m_view->EmptyUndoBuffer();
    //m_flag = true;
    CScintillaEditView::eEolMode eol_mode = CScintillaEditView::JudgeEolMode(m_edit_wcs);
    m_view->SetEolMode(eol_mode);
    UpdateStatusBarInfo();										//更新状态栏

    if (PathFileExists(m_file_path))
        theApp.AddToRecentFileList(m_file_path);
    else
        theApp.RemoveFromRecentFileList(m_file_path);

    //文件打开后为编辑设置语法高亮
    SetEditorSyntaxHight();

    //保存打开文件的最后修改时间
    m_last_modified_time = CCommon::GetFileLastModified(m_file_path.GetString());
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
    UpdateStatusBarInfo();		//保存后刷新状态栏
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
    else if (m_edit_str.size() >= 2 && m_edit_str[0] == -2 && m_edit_str[1] == -1)
    {
        m_code = CodeType::UTF16BE;
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

void CSimpleNotePadDlg::UpdateStatusBarInfo()
{
    //显示编码格式
    CString str{/*_T("编码格式: ")*/ };
    //if (!m_edit_wcs.empty())
    //{
    switch (m_code)
    {
    case CodeType::ANSI:
    {
        switch (m_code_page)
        {
        case CODE_PAGE_CHS: str += CCommon::LoadText(IDS_CODE_PAGE_SIMPLIFIED_CHINESE); break;
        case CODE_PAGE_CHT: str += CCommon::LoadText(IDS_CODE_PAGE_TRADITIONAL_CHINESE); break;
        case CODE_PAGE_JP: str += CCommon::LoadText(IDS_CODE_PAGE_JAPANESE); break;
        case CODE_PAGE_EN: str += CCommon::LoadText(IDS_CODE_PAGE_WESTERN_EUROPE_LANGUAGE); break;
        case CODE_PAGE_KOR: str += CCommon::LoadText(IDS_CODE_PAGE_KOREAN); break;
        case CODE_PAGE_THAI: str += CCommon::LoadText(IDS_CODE_PAGE_THAI); break;
        case CODE_PAGE_VIET: str += CCommon::LoadText(IDS_CODE_PAGE_VIETNAMESE); break;
        default: str += _T("ANSI"); break;
        }
        str += _T(" (");
        if (m_code_page == 0)
            str += CCommon::LoadText(IDS_LOCAL_CODE_PAGE);
        else
            str += std::to_wstring(m_code_page).c_str();
        str += _T(")");
    }
    break;
    case CodeType::UTF8: str += CCommon::LoadText(IDS_UTF8_BOM); break;
    case CodeType::UTF8_NO_BOM: str += CCommon::LoadText(IDS_UTF8_NO_BOM); break;
    case CodeType::UTF16: str += _T("UTF16LE"); break;
    case CodeType::UTF16BE: str += _T("UTF16BE"); break;
    }
    //}

    m_status_bar.SetText(str, SP_CODE_TYPE, 0);

    //显示字符数
    //if (m_edit_wcs.empty())
    //	str.Empty();
    if (m_edit_str.size() < 1024)
        str = CCommon::LoadTextFormat(IDS_FILE_CHARACTOR_NUM_INFO, { m_edit_str.size(), m_edit_wcs.size() });
    else
        str = CCommon::LoadTextFormat(IDS_FILE_CHARACTOR_NUM_INFO2, { m_edit_str.size(), m_edit_str.size() / 1024, m_edit_wcs.size() });
    m_status_bar.SetText(str, SP_INFO, 0);

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
    m_status_bar.SetText(str_eol, SP_EOL_MODE, 0);

    //显示缩放比例
    int scale = 100 + theApp.MenuSettings().zoom * 10;
    CString str_zoom;
    str_zoom.Format(_T("%d%%"), scale);
    m_status_bar.SetText(str_zoom, SP_ZOOM, 0);

    //显示语言
    wstring cur_lan_name = m_syntax_highlight.GetLanguage(m_cur_lan_index).m_name;
    m_status_bar.SetText(cur_lan_name.empty() ? CCommon::LoadText(IDS_NORMAL_TEXT) : cur_lan_name.c_str(), SP_LANGUAGE, 0);

    //显示位置信息
    int row = m_view->GetRow() + 1;
    int col = m_view->GetColumn() + 1;
    int start{}, end{};
    CString pos_info;
    if (m_view->IsSelectionEmpty())
        pos_info = CCommon::LoadTextFormat(IDS_STATUS_BAR_POSITION_IFNO2, { row, col, m_view->GetCurrentIndex() + 1 });
    else
        pos_info = CCommon::LoadTextFormat(IDS_STATUS_BAR_POSITION_IFNO, { m_view->GetSelCount(), m_view->GetLinesSelected() });
    m_status_bar.SetText(pos_info, SP_POSITION_INDICATIOR, 0);
}

void CSimpleNotePadDlg::ChangeCode()
{
    m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code, m_code_page);
    m_view->SetTextW(m_edit_wcs);
    m_view->EmptyUndoBuffer();
    //m_flag = true;
    UpdateStatusBarInfo();
}

bool CSimpleNotePadDlg::BeforeChangeCode()
{
    return SaveInquiry(CCommon::LoadText(IDS_ENCODE_CHANGE_WARNING));
}

void CSimpleNotePadDlg::ConvertToCode(CodeType code_type)
{
    if (m_code != code_type)
    {
        m_code = code_type;
        m_code_page = CP_ACP;
        m_view->EmptyUndoBuffer();
        m_view->SetModified();
        SetTitle();
        UpdateStatusBarInfo();
    }
}

void CSimpleNotePadDlg::SaveConfig() const
{
    CBaseDialog::SaveConfig();
    theApp.WriteProfileInt(L"config", L"always_on_top", m_always_on_top);
    SimplePack pack;
    pack << m_clipboard_items;
    theApp.WriteProfileBinary(L"config", L"clipboard_items", (LPBYTE)pack.data(), pack.size());
}

void CSimpleNotePadDlg::LoadConfig()
{
    CBaseDialog::LoadConfig();
    m_always_on_top = (theApp.GetProfileInt(_T("config"), _T("always_on_top"), 0) != 0);
    std::string clipboard_items = theApp.GetBinary(L"config", L"clipboard_items");
    if (!clipboard_items.empty())
    {
        SimpleUnpack unpack(clipboard_items.c_str(), clipboard_items.size());
        unpack >> m_clipboard_items;
    }
}

bool CSimpleNotePadDlg::SaveInquiry(LPCTSTR info, int* prtn)
{
    if (m_view->IsModified())
    {
        ShowWindow(SW_SHOW);
        SetActiveWindow();

        CString text;
        if (info == NULL)
        {
            if (m_file_path.IsEmpty())
                text = CCommon::LoadText(IDS_SAVE_INQUERY_INFO);
            else
                text = CCommon::LoadTextFormat(IDS_SAVE_INQUERY_INFO2, { m_file_path });
        }
        else
        {
            text = info;
        }

        int rtn = MessageBox(text, NULL, MB_YESNOCANCEL | MB_ICONWARNING);
        if (prtn != nullptr)
            *prtn = rtn;
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
    if (!m_file_path.IsEmpty())
        str_title += m_file_path;
    else
        str_title += CCommon::LoadText(IDS_NO_TITLE);
    str_title += _T(" - ");
    str_title += APP_NAME;

    if (m_monitor_mode)
        str_title += CCommon::LoadText(_T(" ("), IDS_MONITOR_MODE, _T(")"));

#ifdef _DEBUG
    str_title += _T(" (Debug)");
#endif
    SetWindowText(str_title);
}

void CSimpleNotePadDlg::ShowStatusbar(bool show)
{
    CRect rect;
    GetClientRect(&rect);

    if (show)
    {
        m_status_bar.ShowWindow(SW_SHOW);
        rect.bottom = rect.bottom - m_edit_bottom_space - theApp.DPI(2);
    }
    else
    {
        m_status_bar.ShowWindow(SW_HIDE);
    }
    m_view->MoveWindow(rect);
}


void CSimpleNotePadDlg::InitStatusbarWidth()
{
    //计算宽度
    std::vector<int> part_widths;
    part_widths.resize(STATUSBAR_MAX);

    std::vector<int> widths;
    widths.resize(STATUSBAR_MAX);
    widths[SP_POSITION_INDICATIOR] = theApp.DPI(200);
    widths[SP_LANGUAGE] = theApp.DPI(80);
    widths[SP_EOL_MODE] = theApp.DPI(40);
    widths[SP_ZOOM] = theApp.DPI(40);
    widths[SP_CODE_TYPE] = theApp.DPI(174);

    CRect rect;
    GetClientRect(rect);

    for (int i = 0; i < STATUSBAR_MAX - 1; i++)
    {
        part_widths[i] = rect.Width();
        for (int j = STATUSBAR_MAX - 1; j > i; j--)
        {
            part_widths[i] -= widths[j];
        }
    }
    part_widths[STATUSBAR_MAX - 1] = -1;

    m_status_bar.SetParts(part_widths.size(), part_widths.data()); //分割状态栏
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
    CString szFilter = GetOpenFileFilter();
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
        file_name = CCommon::LoadText(IDS_NO_TITLE);
    }
    //构造保存文件对话框
    CFileDialog fileDlg(FALSE, _T("txt"), file_name.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
    //为“另存为”对话框添加一个组合选择框
    fileDlg.AddComboBox(IDC_SAVE_COMBO_BOX);
    //为组合选择框添加项目

    for (size_t i{}; i < CONST_VAL->CodeList().size(); i++)
    {
        fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, i, CONST_VAL->CodeList()[i].name);
    }

    //fileDlg.SetControlLabel(IDC_SAVE_COMBO_BOX, _T("编码类型："));
    //根据当前设置的另存为格式为组合选择框设置默认选中的项目
    fileDlg.SetSelectedControlItem(IDC_SAVE_COMBO_BOX, static_cast<DWORD>(CONST_VAL->GetCodeTypeItemIndex(m_code, m_code_page)));

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
        if (selected_item >= 0 && selected_item < static_cast<DWORD>(CONST_VAL->CodeList().size()))
        {
            CodeType save_code = CONST_VAL->CodeList()[selected_item].code_type;
            UINT save_code_page = CONST_VAL->CodeList()[selected_item].code_page;
            if (save_code_page == CODE_PAGE_DEFAULT)
                save_code_page = theApp.GetGeneralSettings().default_code_page;
            if (SaveFile(fileDlg.GetPathName().GetString(), save_code, save_code_page))
            {
                m_file_path = fileDlg.GetPathName();	//另存为后，当前文件名为保存的文件名
                SetTitle();					//用新的文件名设置标题
                m_code = save_code;		    //另存为后当前编码类型设置为另存为的编码类型
                m_code_page = save_code_page;
                UpdateStatusBarInfo();			//刷新状态栏
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
        MessageBox(CCommon::LoadText(IDS_SAVE_FAILED_INFO), NULL, MB_ICONWARNING);
        return;
    }
    file << m_edit_str;
    m_view->SetSavePoint();
    SetTitle();
    UpdateStatusBarInfo();
    MessageBox(CCommon::LoadText(IDS_HEX_SAVED_INFO), NULL, MB_ICONINFORMATION);
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
        int style_index = 0;
        for (const auto& syntax_style : lan.m_syntax_list)
        {
            //查找是自定义样式是否存在
            CUserLanguage user_lan = theApp.GetLanguageSettings().GetLanguage(lan.m_name);
            if (user_lan.IsStyleExist(style_index))     //自定义的样式存在，使用自定义的样式
            {
                CUserLanguage::SyntaxStyle user_style = user_lan.GetStyle(style_index);
                m_view->SetSyntaxColor(syntax_style.id, user_style.color);
                m_view->SetSyntaxFontStyle(syntax_style.id, user_style.bold, user_style.italic);
            }
            else        //否则使用默认的样式
            {
                m_view->SetSyntaxColor(syntax_style.id, syntax_style.color);
                m_view->SetSyntaxFontStyle(syntax_style.id, syntax_style.bold, syntax_style.italic);
            }
            style_index++;
        }
        m_cur_lan_index = m_syntax_highlight.IndexOf(lan.m_name);
    }
    else
    {
        m_cur_lan_index = -1;
    }
    m_view->SetFold();
    UpdateStatusBarInfo();
}

void CSimpleNotePadDlg::SetEditorSyntaxHight()
{
    CLanguage lan;
    //如果文件的前5个字符是“<?xml”则认为是xml格式
    if (m_edit_wcs.substr(0, 5) == L"<?xml")
        lan = m_syntax_highlight.FindLanguageByName(L"XML");
    if (lan.m_name.empty())
    {
        wstring wcs_file_path = m_file_path.GetString();
        CFilePathHelper helper(wcs_file_path);
        lan = m_syntax_highlight.FindLanguageByFileName(helper.GetFileName());
        if (lan.m_name.empty()) //如果根据当前文件的扩展名无法在默认语言列表中找到对应的语言，则在自定义语言列表中查找
        {
            wstring language_name = theApp.GetLanguageSettings().FindLanguageByFileName(helper.GetFileName());
            lan = m_syntax_highlight.FindLanguageByName(language_name.c_str());
        }
    }
    SetSyntaxHight(lan);
}

bool CSimpleNotePadDlg::IsCommentEnable()
{
    CLanguage::Comment comment = m_syntax_highlight.GetLanguage(m_cur_lan_index).m_comment;
    return !comment.line.empty() || comment.isMultiLeneCommentValid();
}

void CSimpleNotePadDlg::UpdateLineNumberWidth(bool update)
{
    int current_line = m_view->GetFirstVisibleLine();
    int leng_height = m_view->GetLineHeight();
    int line_per_page = m_window_size.cy / leng_height;
    int line_number = current_line + line_per_page;
    static int last_line_number{ -1 };
    if (update || last_line_number != line_number)
    {
        int width = m_view->GetTextWidth(std::to_string(line_number)) + theApp.DPI(8);
        m_view->SetLineNumberWidth(width);
        m_view->ShowLineNumber(theApp.MenuSettings().show_line_number);
    }
    last_line_number = line_number;
}

CString CSimpleNotePadDlg::GetOpenFileFilter()
{
    CString filter;
    filter += CCommon::LoadText(IDS_TEXT_FILE, _T("(*.txt)|*.txt|"));
    for (const auto& lan : m_syntax_highlight.GetLanguageList())
    {
        filter += lan.m_name.c_str();
        wstring ext_list;
        bool full_name = false;
        for (const auto& ext : lan.m_ext)
        {
            if (!ext.empty() && ext.front() == L'$')
            {
                ext_list += ext.substr(1);
                full_name = true;
            }
            else
            {
                ext_list += L"*.";
                ext_list += ext;
            }
            ext_list += L";";
        }
        if (!lan.m_ext.empty())
            ext_list.pop_back();
        if (!full_name)
        {
            filter += _T(" (");
            filter += ext_list.c_str();
            filter += _T(")");
        }
        filter += _T("|");
        filter += ext_list.c_str();
        filter += _T("|");
    }
    filter += CCommon::LoadText(IDS_ALL_FILES);
    filter += _T("(*.*)|*.*||");
    return filter;
}

void CSimpleNotePadDlg::InitMenuIcon()
{
    HMENU menu{ GetMenu()->GetSafeHmenu() };
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_OPEN, FALSE, theApp.GetMenuIcon(IDI_OPEN));
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_NEW, FALSE, theApp.GetMenuIcon(IDI_NEW));
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_NEW_WINDOW, FALSE, theApp.GetMenuIcon(IDI_NEW_WINDOW));
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_SAVE, FALSE, theApp.GetMenuIcon(IDI_SAVE));
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_SAVE_AS, FALSE, theApp.GetMenuIcon(IDI_SAVE_AS));
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_OPEN_LOCATION, FALSE, theApp.GetMenuIcon(IDI_EXPLORE));
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_RELOAD, FALSE, theApp.GetMenuIcon(IDI_RELOAD));
    CMenuIcon::AddIconToMenuItem(GetMenu()->GetSubMenu(0)->GetSafeHmenu(), 7, TRUE, theApp.GetMenuIcon(IDI_RECENT_FILES));
    CMenuIcon::AddIconToMenuItem(menu, ID_APP_EXIT, FALSE, theApp.GetMenuIcon(IDI_EXIT));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_UNDO, FALSE, theApp.GetMenuIcon(IDI_UNDO));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_REDO, FALSE, theApp.GetMenuIcon(IDI_REDO));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_COPY, FALSE, theApp.GetMenuIcon(IDI_COPY));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_CUT, FALSE, theApp.GetMenuIcon(IDI_CUT));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_PASTE, FALSE, theApp.GetMenuIcon(IDI_PASTE));
    CMenuIcon::AddIconToMenuItem(GetMenu()->GetSubMenu(1)->GetSafeHmenu(), 6, TRUE, theApp.GetMenuIcon(IDI_CLIPBOARD));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_FIND, FALSE, theApp.GetMenuIcon(IDI_FIND));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_REPLACE, FALSE, theApp.GetMenuIcon(IDI_REPLACE));
    CMenuIcon::AddIconToMenuItem(menu, ID_FIND_PRIVIOUS, FALSE, theApp.GetMenuIcon(IDI_PREVIOUS));
    CMenuIcon::AddIconToMenuItem(menu, ID_FIND_NEXT, FALSE, theApp.GetMenuIcon(IDI_NEXT));
    CMenuIcon::AddIconToMenuItem(menu, ID_EDIT_SELECT_ALL, FALSE, theApp.GetMenuIcon(IDI_SELECT_ALL));
    CMenuIcon::AddIconToMenuItem(menu, ID_CONVERT_TO_CAPITAL, FALSE, theApp.GetMenuIcon(IDI_FONT));
    CMenuIcon::AddIconToMenuItem(menu, ID_CONVERT_TO_LOWER_CASE, FALSE, theApp.GetMenuIcon(IDI_LOWER_CASE));
    CMenuIcon::AddIconToMenuItem(menu, ID_CONVERT_TO_TITLE_CASE, FALSE, theApp.GetMenuIcon(IDI_CAPITAL));
    CMenuIcon::AddIconToMenuItem(menu, ID_FORMAT_FONT, FALSE, theApp.GetMenuIcon(IDI_FONT));
    CMenuIcon::AddIconToMenuItem(GetMenu()->GetSubMenu(3)->GetSafeHmenu(), 7, TRUE, theApp.GetMenuIcon(IDI_LANGUAGE));
    if (!CWinVersionHelper::IsWindows7OrLater())
        CMenuIcon::AddIconToMenuItem(menu, ID_ALWAYS_ON_TOP, FALSE, theApp.GetMenuIcon(IDI_PIN));
    CMenuIcon::AddIconToMenuItem(GetMenu()->GetSubMenu(4)->GetSafeHmenu(), 1, TRUE, theApp.GetMenuIcon(IDI_ZOOM_IN));
    CMenuIcon::AddIconToMenuItem(menu, ID_VIEW_ZOOM_IN, FALSE, theApp.GetMenuIcon(IDI_ZOOM_IN));
    CMenuIcon::AddIconToMenuItem(menu, ID_VIEW_ZOOM_OUT, FALSE, theApp.GetMenuIcon(IDI_ZOOM_OUT));
    CMenuIcon::AddIconToMenuItem(menu, ID_HEX_VIEW, FALSE, theApp.GetMenuIcon(IDI_HEX_VIEW));
    CMenuIcon::AddIconToMenuItem(menu, ID_FILE_COMPARE, FALSE, theApp.GetMenuIcon(IDI_COMPARE));
    CMenuIcon::AddIconToMenuItem(menu, ID_FORMAT_CONVERT, FALSE, theApp.GetMenuIcon(IDI_CODE_BATCH));
    CMenuIcon::AddIconToMenuItem(menu, ID_CODE_CONVERT, FALSE, theApp.GetMenuIcon(IDI_CODE));
    CMenuIcon::AddIconToMenuItem(menu, ID_TOOL_OPTIONS, FALSE, theApp.GetMenuIcon(IDI_SETTINGS));
    CMenuIcon::AddIconToMenuItem(menu, ID_HELP, FALSE, theApp.GetMenuIcon(IDI_HELP));
    CMenuIcon::AddIconToMenuItem(menu, ID_APP_ABOUT, FALSE, theApp.GetMenuIcon(IDR_MAINFRAME));

    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_EDIT_UNDO, FALSE, theApp.GetMenuIcon(IDI_UNDO));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_EDIT_REDO, FALSE, theApp.GetMenuIcon(IDI_REDO));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_EDIT_COPY, FALSE, theApp.GetMenuIcon(IDI_COPY));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_EDIT_CUT, FALSE, theApp.GetMenuIcon(IDI_CUT));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_EDIT_PASTE, FALSE, theApp.GetMenuIcon(IDI_PASTE));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSubMenu(0)->GetSafeHmenu(), 6, TRUE, theApp.GetMenuIcon(IDI_CLIPBOARD));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_EDIT_SELECT_ALL, FALSE, theApp.GetMenuIcon(IDI_SELECT_ALL));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_CONVERT_TO_CAPITAL, FALSE, theApp.GetMenuIcon(IDI_FONT));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_CONVERT_TO_LOWER_CASE, FALSE, theApp.GetMenuIcon(IDI_LOWER_CASE));
    CMenuIcon::AddIconToMenuItem(m_context_menu.GetSafeHmenu(), ID_CONVERT_TO_TITLE_CASE, FALSE, theApp.GetMenuIcon(IDI_CAPITAL));
}

void CSimpleNotePadDlg::AddItemToClipboardHistory(const std::wstring& str)
{
    //判断要添加的文本是否在剪贴板历史记录中存在
    auto iter = std::find(m_clipboard_items.begin(), m_clipboard_items.end(), str);
    if (iter == m_clipboard_items.end())
    {
        m_clipboard_items.push_front(str);
        if (m_clipboard_items.size() > CLIPBOARD_ITEM_MAX)
            m_clipboard_items.pop_back();
        InitClipboardHistoryMenu();
    }
}

void CSimpleNotePadDlg::InitClipboardHistoryMenu()
{
    auto initClipboardHistoryMenu = [&](CMenu* pMenu)
    {
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            //清空子目录
            if (!m_clipboard_items.empty())
            {
                while (pMenu->GetMenuItemCount() > 0)
                {
                    pMenu->DeleteMenu(0, MF_BYPOSITION);
                }
            }
            //添加剪贴板项目
            int index = 0;
            for (const auto& item_name : m_clipboard_items)
            {
                if (index >= ID_CLIPBOARD_ITEM_MAX - ID_CLIPBOARD_ITEM_START)
                    break;
                //生成要在剪贴板历史记录菜单中显示的文本
                CString menu_item = item_name.c_str();
                //去掉换行符
                menu_item.Remove(_T('\r'));
                menu_item.Remove(_T('\n'));
                menu_item.Replace(_T('\t'), _T(' '));
                //截取前面指定长度的字符，并添加省略号
                if (menu_item.GetLength() > 32)
                {
                    menu_item = menu_item.Left(32);
                    menu_item += _T("...");
                }
                //添加序号
                CString str_menu_item;
                str_menu_item.Format(_T("&%d. %s"), index + 1, menu_item.GetString());
                pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_CLIPBOARD_ITEM_START + index, str_menu_item);
                index++;
            }
        }
    };

    //初始化主菜单和右键菜单中的剪贴板历史记录菜单
    initClipboardHistoryMenu(GetClipboardHistoryMenu(true));
    initClipboardHistoryMenu(GetClipboardHistoryMenu(false));

}

CMenu* CSimpleNotePadDlg::GetClipboardHistoryMenu(bool context_menu)
{
    CMenu* pMenu{};
    if (context_menu)
        pMenu = m_context_menu.GetSubMenu(0)->GetSubMenu(6);
    else
        pMenu = GetMenu()->GetSubMenu(1)->GetSubMenu(6);
    ASSERT(pMenu != nullptr);
    return pMenu;
}

void CSimpleNotePadDlg::AddClipboardDataToAllProcess(const std::wstring str)
{
    //查找所有SimpleNotePad窗口句柄
    std::vector<HWND> handles;
    CCommon::FindAllWindow(APP_CLASS_NAME, handles);
    for (auto handle : handles)
    {
        if (handle != m_hWnd)
            CCommon::SendProcessMessage(handle, CCommon::ProcessMsgType::CLIP_BOARD, str);
    }
}

void CSimpleNotePadDlg::UpdateOptionSettingsToAllProcess()
{
    std::string str_data;
    SimplePack pack;
    pack << theApp.GetGeneralSettings() << theApp.GetEditSettings() << theApp.GetLanguageSettings();
    str_data.assign(pack.data(), pack.size());
    std::vector<HWND> handles;
    CCommon::FindAllWindow(APP_CLASS_NAME, handles);
    for (auto handle : handles)
    {
        if (handle != m_hWnd)
            CCommon::SendProcessMessage(handle, CCommon::ProcessMsgType::SETTINGS_DATA, str_data);
    }
}


void CSimpleNotePadDlg::UpdateMenuSettingsToAllProcess()
{
    std::string str_data;
    Object2String(theApp.MenuSettings(), str_data);
    std::vector<HWND> handles;
    CCommon::FindAllWindow(APP_CLASS_NAME, handles);
    for (auto handle : handles)
    {
        if (handle != m_hWnd)
            CCommon::SendProcessMessage(handle, CCommon::ProcessMsgType::MENU_SETTINGS_DATA, str_data);
    }
}

void CSimpleNotePadDlg::FillFindText()
{
    //将选中文本设置到查找对话框中“查找”文本框
    std::wstring str_selected = m_view->GetSelectedTextW();
    if (str_selected.size() <= FIND_REPLACE_AUTO_FILL_MAX_LENGTH)
        m_find_replace_dlg.SetFindString(str_selected.c_str());
    else
        m_find_replace_dlg.SetFindString(L"");
}

UINT CSimpleNotePadDlg::TextChangeThreadCallback(LPVOID dwUser)
{
    CSimpleNotePadDlg* pThis = dynamic_cast<CSimpleNotePadDlg*>(theApp.m_pMainWnd);
    while (true)
    {
        if (pThis->m_text_changed_thread_exit)
            break;

        //响应编辑器文本变化
        if (pThis->m_text_changed)
        {
            pThis->m_view->GetTextW(pThis->m_edit_wcs);
            pThis->UpdateStatusBarInfo();
            pThis->SetTitle();
            pThis->m_text_changed = false;
        }
        Sleep(100);
    }
    return 0;
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
//		UpdateStatusBarInfo();			//刷新状态栏
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
    ON_COMMAND(ID_SHOW_STATUSBAR, &CSimpleNotePadDlg::OnShowStatusbar)
    ON_COMMAND(ID_GOTO_LINE, &CSimpleNotePadDlg::OnGotoLine)
    ON_COMMAND(ID_CODE_UTF16BE, &CSimpleNotePadDlg::OnCodeUtf16be)
    ON_COMMAND(ID_CONVERT_TO_ANSI, &CSimpleNotePadDlg::OnConvertToAnsi)
    ON_COMMAND(ID_CONVERT_TO_UTF8_BOM, &CSimpleNotePadDlg::OnConvertToUtf8Bom)
    ON_COMMAND(ID_CONVERT_TO_UTF8_NO_BOM, &CSimpleNotePadDlg::OnConvertToUtf8NoBom)
    ON_COMMAND(ID_CONVERT_TO_UTF16, &CSimpleNotePadDlg::OnConvertToUtf16)
    ON_COMMAND(ID_CONVERT_TO_UTF16BE, &CSimpleNotePadDlg::OnConvertToUtf16be)
    ON_MESSAGE(WM_NP_FIND_REPLACE, &CSimpleNotePadDlg::OnNpFindReplace)
    ON_COMMAND(ID_FIND_PRIVIOUS, &CSimpleNotePadDlg::OnFindPrivious)
    ON_WM_COPYDATA()
    ON_COMMAND(ID_ADD_DELETE_COMMENT, &CSimpleNotePadDlg::OnAddDeleteComment)
    ON_COMMAND(ID_VIEW_ZOOM_IN, &CSimpleNotePadDlg::OnViewZoomIn)
    ON_COMMAND(ID_VIEW_ZOOM_OUT, &CSimpleNotePadDlg::OnViewZoomOut)
    ON_COMMAND(ID_VIEW_ZOOM_DEFAULT, &CSimpleNotePadDlg::OnViewZoomDefault)
    ON_MESSAGE(WM_DELETE_CHAR, &CSimpleNotePadDlg::OnDeleteChar)
    ON_COMMAND(ID_MONITOR_MODE, &CSimpleNotePadDlg::OnMonitorMode)
    ON_COMMAND(ID_FILE_RELOAD, &CSimpleNotePadDlg::OnFileReload)
    ON_COMMAND(ID_EDIT_TIME_DATE, &CSimpleNotePadDlg::OnEditTimeDate)
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

    //根据当前系统DPI设置设置状态栏大小
    theApp.DPIFromWindow(this);

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(theApp.GetMenuIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    ////初始化窗口大小
    //SetWindowPos(nullptr, 0, 0, m_window_width, m_window_hight, SWP_NOZORDER | SWP_NOMOVE);

    m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
    m_context_menu.LoadMenu(IDR_POPUP_MENU);

    m_code = theApp.GetGeneralSettings().default_code;
    m_code_page = theApp.GetGeneralSettings().default_code_page;

    //加载语法高亮设置
    wstring path;
#ifdef DEBUG
    path = L"./lang.xml";
#else
    path = CCommon::GetExePath() + L"lang.xml";
#endif
    m_syntax_highlight.LoadFromFile(path.c_str());

    m_status_bar_hight = theApp.DPI(20);
    m_edit_bottom_space = theApp.DPI(22);

    SetMinSize(theApp.DPI(200), theApp.DPI(150));

    //初始化编辑框大小
    CRect rect;
    GetClientRect(&rect);
    //rect.bottom = rect.bottom - 22;
    if (theApp.MenuSettings().show_statusbar)
        rect.bottom = rect.bottom - m_edit_bottom_space - theApp.DPI(2);
    //m_edit.MoveWindow(rect);

    m_view = (CScintillaEditView*)RUNTIME_CLASS(CScintillaEditView)->CreateObject();
    m_view->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, this, 3000);
    m_view->OnInitialUpdate();
    m_view->ShowWindow(SW_SHOW);

    m_view->SetWordWrap(theApp.MenuSettings().word_wrap, static_cast<CScintillaEditView::eWordWrapMode>(theApp.MenuSettings().word_wrap_mode));

    UpdateLineNumberWidth();
    m_view->ShowLineNumber(theApp.MenuSettings().show_line_number);
    m_view->SetLineNumberColor(RGB(75, 145, 175));
    m_view->SetViewEol(theApp.MenuSettings().show_eol);
    m_view->SetBackgroundColor(theApp.GetEditSettings().background_color);
    m_view->SetSelectionBackColor(theApp.GetEditSettings().selection_back_color);
    m_view->ShowIndentationGuides(theApp.GetEditSettings().show_indentation_guides);
    m_view->SetContextMenu(m_context_menu.GetSubMenu(0), this);

    //初始化状态栏
    GetClientRect(&rect);
    //rect.top = rect.bottom - 20;
    rect.top = rect.bottom - m_status_bar_hight;
    m_status_bar.Create(WS_VISIBLE | CBRS_BOTTOM, rect, this, 3);

    InitStatusbarWidth();
    UpdateStatusBarInfo();
    ShowStatusbar(theApp.MenuSettings().show_statusbar);

    m_view->SetZoom(theApp.MenuSettings().zoom);

    //初始化字体
    //m_font.CreatePointFont(m_font_size * 10, m_font_name);
    //m_edit.SetFont(&m_font);
    m_view->SetFontFaceW(theApp.GetEditSettings().font_name.GetString());
    m_view->SetFontSize(theApp.GetEditSettings().font_size);

    m_view->SetLexerNormalText();

    //初始化菜单
    CMenu* pMenu = GetMenu();
    if (pMenu != nullptr)
    {
        //初始化语言菜单
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

        //初始化最近打开文件列表
        CMenu* pRecentFileMenu = pMenu->GetSubMenu(0)->GetSubMenu(7);
        ASSERT(pRecentFileMenu != nullptr);
        if (pRecentFileMenu != nullptr)
        {
            //清空子目录
            if (!theApp.GetRecentFileList().empty())
            {
                while (pRecentFileMenu->GetMenuItemCount() > 0)
                {
                    pRecentFileMenu->DeleteMenu(0, MF_BYPOSITION);
                }
            }
            //添加最近打开文件
            int index = 0;
            for (const auto& file_path : theApp.GetRecentFileList())
            {
                pRecentFileMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_FILE_MRU_FILE1 + index, file_path);
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
    m_view->SetTabSize(theApp.GetEditSettings().tab_width);

    //设置当前行高亮
    m_view->SetCurrentLineHighlightColor(theApp.GetEditSettings().current_line_highlight_color);
    m_view->SetCurrentLineHighlight(theApp.GetEditSettings().current_line_highlight);

    SetAlwaysOnTop();

    InitMenuIcon();

    InitClipboardHistoryMenu();

    //初始化查找替换对话框
    m_find_replace_dlg.Create(this);

    SetTimer(TIMER_ID_MONITOR, 1000, NULL);

    m_text_change_thread = AfxBeginThread(TextChangeThreadCallback, nullptr);

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
    size.bottom = cy - (theApp.MenuSettings().show_statusbar ? m_edit_bottom_space : -theApp.DPI(2));		//窗口下方状态栏占20个像素高度
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
        InitStatusbarWidth();
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
    CString szFilter = GetOpenFileFilter();
    //构造打开文件对话框
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
    //显示打开文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        m_file_path = fileDlg.GetPathName();	//获取打开的文件路径
        OpenFile();					//打开文件
        SetTitle();								//设置窗口标题
    }
}


void CSimpleNotePadDlg::OnCodeAnsi()
{
    // TODO: 在此添加命令处理程序代码
    //m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
    if (!BeforeChangeCode()) return;
    m_code = CodeType::ANSI;
    m_code_page = CP_ACP;
    ChangeCode();
}


void CSimpleNotePadDlg::OnCodeUtf8()
{
    // TODO: 在此添加命令处理程序代码
    //m_edit_str = CCommon::UnicodeToStr(m_edit_wcs, m_code);
    if (!BeforeChangeCode()) return;
    if (m_code != CodeType::UTF8_NO_BOM)
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
    _tcscpy_s(lf.lfFaceName, LF_FACESIZE, theApp.GetEditSettings().font_name.GetString());	//将lf中的元素字体名设为“微软雅黑”
    lf.lfHeight = CCommon::FontSizeToLfHeight(theApp.GetEditSettings().font_size);
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
        auto edit_settings = theApp.GetEditSettings();
        edit_settings.font_name = fontDlg.m_cf.lpLogFont->lfFaceName;
        edit_settings.font_size = fontDlg.m_cf.iPointSize / 10;
        theApp.SetEditSettings(edit_settings);
        //设置字体
        m_view->SetFontFaceW(theApp.GetEditSettings().font_name.GetString());
        m_view->SetFontSize(theApp.GetEditSettings().font_size);
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

    if (pMsg->message == WM_KEYDOWN)
    {
        //按下Esc键清除所有标记
        if (pMsg->wParam == VK_ESCAPE)
        {
            if (m_marked)
            {
                m_view->ClearAllMark(CScintillaEditView::MarkStyle::MARK_ALL);
                m_find_replace_dlg.SetInfoString(_T(""));
                m_marked = false;
            }
            m_view->ClearAllMark(CScintillaEditView::MarkStyle::SELECTION_MARK);
            return TRUE;
        }
        //按下Ctrl键时
        if (GetKeyState(VK_CONTROL) & 0x80)
        {
            //按下Shift键时
            if (GetKeyState(VK_SHIFT) & 0x8000)
            {
                if (pMsg->wParam == 'V')
                {
                    //按下Ctrl+Shift+V打开剪贴板历史记录
                    CMenu* pClipboardHistory = GetClipboardHistoryMenu(false);
                    CPoint point = m_view->GetCursorPosition();
                    point.y += m_view->GetLineHeight();
                    ClientToScreen(&point);
                    pClipboardHistory->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this); //在指定位置显示弹出菜单
                    return TRUE;
                }
            }
#ifdef _DEBUG
            //if (pMsg->wParam != VK_CONTROL)
            //{
            //    int a = 0;
            //}
            if (pMsg->wParam == 'Q')
            {
                //m_view->AutoSelectWord();
                int pos = m_view->GetCurrentIndex();
                m_view->DeleteText(pos, 1);

                return TRUE;
            }
#endif
        }
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
    hex_view_dlg.DoModal();
    if (hex_view_dlg.IsModified())
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
        if (MessageBox(CCommon::LoadText(IDS_HEX_SAVE_INQUIRY), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            m_edit_wcs = CCommon::StrToUnicode(m_edit_str, m_code);
            m_view->SetTextW(m_edit_wcs);
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
    OpenFile();	//打开文件
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
    m_view->SetTextW(L"");
    m_view->EmptyUndoBuffer();
    m_code = theApp.GetGeneralSettings().default_code;
    m_code_page = theApp.GetGeneralSettings().default_code_page;
    m_view->SetSavePoint();
    UpdateStatusBarInfo();
    SetWindowText(CCommon::LoadText(IDS_NO_TITLE, _T(" - SimpleNotePad")));
}


void CSimpleNotePadDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1234)
    {
        KillTimer(1234);		//定时器响应一次后就将其销毁
        OpenFile();		//如果文件是通过命令行打开的，则延时100毫秒再打开
    }
    if (nIDEvent == TIMER_ID_MONITOR)
    {
        if (m_monitor_mode && !m_file_path.IsEmpty())
        {
            //监视模式下，检查文件是否更新，如果有更新，则重新加载
            unsigned __int64 modified_time = CCommon::GetFileLastModified(m_file_path.GetString());
            if (modified_time > m_last_modified_time)
            {
                CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
                OpenFile();
            }
            m_last_modified_time = modified_time;
        }
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
    theApp.MenuSettings().word_wrap = !theApp.MenuSettings().word_wrap;
    m_view->SetWordWrap(theApp.MenuSettings().word_wrap, static_cast<CScintillaEditView::eWordWrapMode>(theApp.MenuSettings().word_wrap_mode));
    UpdateMenuSettingsToAllProcess();
}


//void CSimpleNotePadDlg::OnUpdateWordWrap(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(theApp.MenuSettings().word_wrap);
//}


void CSimpleNotePadDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //询问是否保存
    if (!SaveInquiry()) return;

    CBaseDialog::OnClose();
}


void CSimpleNotePadDlg::OnFind()
{
    m_find_replace_dlg.ShowWindow(SW_SHOW);
    m_find_replace_dlg.SetActiveWindow();
    m_find_replace_dlg.SetMode(CFindReplaceDlg::Mode::FIND);
    m_view->AutoSelectWord();   //自动选中光标处的一个单词
    FillFindText();
}


void CSimpleNotePadDlg::OnFindPrivious()
{
    if (!m_find_replace_dlg.GetOptions().find_str.empty())
    {
        bool res = FindReplaceTools::FindTexts(m_find_replace_dlg.GetOptions(), false, m_view);
        if (!res)
        {
            CString info = CCommon::LoadTextFormat(IDS_CANNOT_FIND_INFO, { m_find_replace_dlg.GetOptions().find_str });
            m_find_replace_dlg.SetInfoString(info);
        }
        else
        {
            m_find_replace_dlg.ClearInfoString();
        }
    }
}

void CSimpleNotePadDlg::OnFindNext()
{
    if (!m_find_replace_dlg.GetOptions().find_str.empty())
    {
        bool res = FindReplaceTools::FindTexts(m_find_replace_dlg.GetOptions(), true, m_view);
        if (!res)
        {
            CString info = CCommon::LoadTextFormat(IDS_CANNOT_FIND_INFO, { m_find_replace_dlg.GetOptions().find_str });
            m_find_replace_dlg.SetInfoString(info);
        }
        else
        {
            m_find_replace_dlg.ClearInfoString();
        }
    }
}

void CSimpleNotePadDlg::OnMarkAll()
{
    if (!m_marked)
    {
        int mark_count = FindReplaceTools::MarkAll(m_find_replace_dlg.GetOptions(), m_view);
        if (mark_count > 0)
        {
            CString info = CCommon::LoadTextFormat(IDS_MARK_ALL_INFO, { mark_count });
            m_find_replace_dlg.SetInfoString(info);
        }
    }
    else
    {
        m_view->ClearAllMark(CScintillaEditView::MarkStyle::MARK_ALL);
        m_find_replace_dlg.SetInfoString(_T(""));
    }
    m_marked = !m_marked;
}


void CSimpleNotePadDlg::OnReplace()
{
    // TODO: 在此添加命令处理程序代码
    m_find_replace_dlg.ShowWindow(SW_SHOW);
    m_find_replace_dlg.SetActiveWindow();
    m_find_replace_dlg.SetMode(CFindReplaceDlg::Mode::REPLACE);
    m_view->AutoSelectWord();   //自动选中光标处的一个单词
    FillFindText();
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
        UpdateStatusBarInfo();
}


void CSimpleNotePadDlg::OnInitMenu(CMenu* pMenu)
{
    CBaseDialog::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    pMenu->EnableMenuItem(ID_FILE_OPEN_LOCATION, m_file_path.IsEmpty() ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_FILE_RELOAD, m_file_path.IsEmpty() ? MF_GRAYED : MF_ENABLED);

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
        case CodeType::ANSI: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16BE, ID_CODE_ANSI, MF_BYCOMMAND | MF_CHECKED); break;
        case CodeType::UTF8: case CodeType::UTF8_NO_BOM: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16BE, ID_CODE_UTF8, MF_BYCOMMAND | MF_CHECKED); break;
        case CodeType::UTF16: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16BE, ID_CODE_UTF16, MF_BYCOMMAND | MF_CHECKED); break;
        case CodeType::UTF16BE: pMenu->CheckMenuRadioItem(ID_CODE_ANSI, ID_CODE_UTF16BE, ID_CODE_UTF16BE, MF_BYCOMMAND | MF_CHECKED); break;
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

    pMenu->CheckMenuItem(ID_WORD_WRAP, MF_BYCOMMAND | (theApp.MenuSettings().word_wrap ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_ALWAYS_ON_TOP, MF_BYCOMMAND | (m_always_on_top ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_STATUSBAR, MF_BYCOMMAND | (theApp.MenuSettings().show_statusbar ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_LINE_NUMBER, MF_BYCOMMAND | (theApp.MenuSettings().show_line_number ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_EOL, MF_BYCOMMAND | (theApp.MenuSettings().show_eol ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_MONITOR_MODE, MF_BYCOMMAND | (m_monitor_mode ? MF_CHECKED : MF_UNCHECKED));

    bool is_selection_empty = m_view->IsSelectionEmpty();
    pMenu->EnableMenuItem(ID_EDIT_COPY, is_selection_empty ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_EDIT_CUT, is_selection_empty || m_view->IsReadOnly() ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_EDIT_UNDO, m_view->CanUndo() ? MF_ENABLED : MF_GRAYED);
    pMenu->EnableMenuItem(ID_EDIT_REDO, m_view->CanRedo() ? MF_ENABLED : MF_GRAYED);
    pMenu->EnableMenuItem(ID_EDIT_PASTE, m_view->CanPaste() ? MF_ENABLED : MF_GRAYED);
    pMenu->EnableMenuItem(ID_CONVERT_TO_CAPITAL, is_selection_empty || m_view->IsReadOnly() ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_CONVERT_TO_LOWER_CASE, is_selection_empty || m_view->IsReadOnly() ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_CONVERT_TO_TITLE_CASE, is_selection_empty || m_view->IsReadOnly() ? MF_GRAYED : MF_ENABLED);
    pMenu->EnableMenuItem(ID_ADD_DELETE_COMMENT, IsCommentEnable() && !m_view->IsReadOnly() ? MF_ENABLED : MF_GRAYED);
    pMenu->EnableMenuItem(ID_MONITOR_MODE, PathFileExists(m_file_path) ? MF_ENABLED : MF_GRAYED);

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

    switch (theApp.MenuSettings().word_wrap_mode)
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
    inputDlg.SetTitle(CCommon::LoadText(IDS_INPUT_CODE_PAGE));
    inputDlg.SetInfoText(CCommon::LoadText(IDS_PLEASE_INPUT_CODE_PAGE));
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
    dlg.m_general_settings_dlg.m_data = theApp.GetGeneralSettings();
    dlg.m_edit_settings_dlg.m_data = theApp.GetEditSettings();
    dlg.m_language_settings_dlg.m_data = theApp.GetLanguageSettings();
    if (dlg.DoModal() == IDOK)
    {
        ApplySettings(dlg.m_general_settings_dlg.m_data, dlg.m_edit_settings_dlg.m_data, dlg.m_language_settings_dlg.m_data);
        UpdateOptionSettingsToAllProcess();
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
    SCNotification* notification = reinterpret_cast<SCNotification*>(lParam);
    if (notification->nmhdr.hwndFrom == m_view->GetSafeHwnd())
    {
        //响应编辑器文本变化
        if (notification->nmhdr.code == SCN_MODIFIED && m_view->IsEditChangeNotificationEnable())
        {
            UINT marsk = (SC_MOD_DELETETEXT | SC_MOD_INSERTTEXT | SC_PERFORMED_UNDO | SC_PERFORMED_REDO);
            if ((notification->modificationType & marsk) != 0)
            {
                m_text_changed = true;
            }
            //当删除了字符时
            if (notification->modificationType == (SC_MOD_DELETETEXT | SC_PERFORMED_USER)
                || notification->modificationType == (SC_MOD_DELETETEXT | SC_PERFORMED_USER | SC_STARTACTION))
            {
                char ch{};
                if (notification->text != nullptr)
                    ch = notification->text[0];
                CEditorHelper helper(m_view);
                if (theApp.GetEditSettings().brackets_auto_comp)
                    helper.BracketsAutoDelete(ch);

                //显示自动完成列表
                if (theApp.GetEditSettings().show_auto_comp_list)
                {
                    const CLanguage& language = m_syntax_highlight.GetLanguage(m_cur_lan_index);
                    helper.AutoShowCompList(language);
                }
            }
        }
        else if (notification->nmhdr.code == SCN_ZOOM)
        {
            theApp.MenuSettings().zoom = m_view->GetZoom();
            UpdateStatusBarInfo();
            UpdateMenuSettingsToAllProcess();
            UpdateLineNumberWidth();
        }
        else if (notification->nmhdr.code == SCN_COPY)
        {
            std::wstring str = CCommon::StrToUnicode(notification->text, CodeType::UTF8_NO_BOM);
            AddItemToClipboardHistory(str);
            AddClipboardDataToAllProcess(str);
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
            //选择区域变化
            if ((notification->updated & SC_UPDATE_SELECTION) != 0)
            {
                //标记文档中与选中部分相同单词
                if (theApp.GetEditSettings().mark_same_words)
                {
                    //先清除标记
                    m_view->ClearAllMark(CScintillaEditView::MarkStyle::SELECTION_MARK);
                    //获取选中部分文本
                    std::string selected_text = m_view->GetSelectedText();
                    if (!selected_text.empty() && CCommon::IsStringIdentifier(selected_text))
                    {
                        //标记相同单词
                        if (FindReplaceTools::MarkSameWord(selected_text, CScintillaEditView::MarkStyle::SELECTION_MARK, m_view))
                        {
                            //如果成功标记，则将已标记单词填充到查找替换对话框中的查找文本框中
                            std::wstring selected_text_wcs = CCommon::StrToUnicode(selected_text, CodeType::UTF8_NO_BOM);
                            m_find_replace_dlg.SetFindString(selected_text_wcs.c_str());
                        }
                    }
                }

                //标记匹配的括号
                CEditorHelper helper(m_view);
                if (theApp.GetEditSettings().mark_matched_brackets)
                    helper.MarkMatchedBrackets();

                //标记匹配的html标记
                if (theApp.GetEditSettings().mark_matched_html_mark)
                {
                    const CLanguage& language = m_syntax_highlight.GetLanguage(m_cur_lan_index);
                    if (language.m_id == SCLEX_XML || language.m_id == SCLEX_HTML)
                        helper.MarkMatchedHtmlMarks();
                }

                m_find_replace_dlg.EnableControl();
                UpdateStatusBarInfo();
            }
        }
        else if (notification->nmhdr.code == SCN_CHARADDED)
        {
            if (notification->characterSource == SC_CHARACTERSOURCE_DIRECT_INPUT)
            {
                char ch = static_cast<char>(notification->ch);
                CEditorHelper helper(m_view);
                //括号自动完成
                if (theApp.GetEditSettings().brackets_auto_comp)
                    helper.BracketsAutoComp(ch);

                const CLanguage& language = m_syntax_highlight.GetLanguage(m_cur_lan_index);
                //HTML标记自动完成
                if (theApp.GetEditSettings().html_mark_auto_comp)
                {
                    if (language.m_id == SCLEX_XML || language.m_id == SCLEX_HTML)
                    {
                        helper.HtmlMarkAutoComp(ch);
                    }
                }

                //显示自动完成列表
                if (theApp.GetEditSettings().show_auto_comp_list)
                    helper.AutoShowCompList(language);
            }
        }
    }

    return CBaseDialog::OnNotify(wParam, lParam, pResult);
}


void CSimpleNotePadDlg::OnShowLineNumber()
{
    // TODO: 在此添加命令处理程序代码
    theApp.MenuSettings().show_line_number = !theApp.MenuSettings().show_line_number;
    m_view->ShowLineNumber(theApp.MenuSettings().show_line_number);
    UpdateMenuSettingsToAllProcess();
}


void CSimpleNotePadDlg::OnEolCrlf()
{
    // TODO: 在此添加命令处理程序代码
    m_view->SetEolMode(CScintillaEditView::EOL_CRLF);
    m_view->ConvertEolMode(CScintillaEditView::EOL_CRLF);
    UpdateStatusBarInfo();
}


void CSimpleNotePadDlg::OnEolCr()
{
    // TODO: 在此添加命令处理程序代码
    m_view->SetEolMode(CScintillaEditView::EOL_CR);
    m_view->ConvertEolMode(CScintillaEditView::EOL_CR);
    UpdateStatusBarInfo();
}


void CSimpleNotePadDlg::OnEolLf()
{
    // TODO: 在此添加命令处理程序代码
    m_view->SetEolMode(CScintillaEditView::EOL_LF);
    m_view->ConvertEolMode(CScintillaEditView::EOL_LF);
    UpdateStatusBarInfo();
}


void CSimpleNotePadDlg::OnShowEol()
{
    // TODO: 在此添加命令处理程序代码
    theApp.MenuSettings().show_eol = !theApp.MenuSettings().show_eol;
    m_view->SetViewEol(theApp.MenuSettings().show_eol);
    UpdateMenuSettingsToAllProcess();
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
        UpdateStatusBarInfo();
    }
    //响应最近打开文件
    if (command >= ID_FILE_MRU_FILE1 && command < ID_FILE_MRU_FILE1 + RECENT_FILE_LIST_MAX_SIZE)
    {
        int recent_file_index = command - ID_FILE_MRU_FILE1;
        if (recent_file_index >= 0 && recent_file_index < static_cast<int>(theApp.GetRecentFileList().size()))
        {
            CString file_path = theApp.GetRecentFileList()[recent_file_index];
            if (SaveInquiry())              //询问是否要保存更改
            {
                m_file_path = file_path;	//获取打开的文件路径
                OpenFile();					//打开文件
                SetTitle();								//设置窗口标题
            }
        }
        return TRUE;
    }
    //响应剪贴板历史记录
    if (command >= ID_CLIPBOARD_ITEM_START && command < ID_CLIPBOARD_ITEM_MAX)
    {
        int index = command - ID_CLIPBOARD_ITEM_START;
        if (index >= 0 && index < m_clipboard_items.size())
        {
            std::wstring str = m_clipboard_items.at(index);
            if (!str.empty())
            {
                //粘贴选中的文本
                m_view->PasteW(str);
                //将选中的文本移动到列表的顶端
                m_clipboard_items.erase(m_clipboard_items.begin() + index);
                m_clipboard_items.push_front(str);
                InitClipboardHistoryMenu();
            }
        }
    }

    return CBaseDialog::OnCommand(wParam, lParam);
}


void CSimpleNotePadDlg::OnConvertToCapital()
{
    // TODO: 在此添加命令处理程序代码
    if (!m_view->IsSelectionEmpty())
    {
        //CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
        int start{}, end{};
        m_view->GetSelW(start, end);
        if (end > start)
        {
            //获取选中部分的文本
            wstring str_selected = m_edit_wcs.substr(start, end - start);
            //大小写转换
            for (auto& ch : str_selected)
            {
                CCommon::ConvertCharCase(ch, true);
            }
            //替换成转换后的文本
            m_view->ReplaceSelectionW(str_selected);
            m_view->SetSelW(start, end, m_edit_wcs);
            SetTitle();
        }
    }
}


void CSimpleNotePadDlg::OnConvertToLowerCase()
{
    // TODO: 在此添加命令处理程序代码
    if (!m_view->IsSelectionEmpty())
    {
        //CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
        int start{}, end{};
        m_view->GetSelW(start, end);
        if (end > start)
        {
            //获取选中部分的文本
            wstring str_selected = m_edit_wcs.substr(start, end - start);
            //大小写转换
            for (auto& ch : str_selected)
            {
                CCommon::ConvertCharCase(ch, false);
            }
            //替换成转换后的文本
            m_view->ReplaceSelectionW(str_selected);
            m_view->SetSelW(start, end, m_edit_wcs);
            SetTitle();
        }
    }
}


void CSimpleNotePadDlg::OnConvertToTitleCase()
{
    // TODO: 在此添加命令处理程序代码
    if (!m_view->IsSelectionEmpty())
    {
        //CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
        int start{}, end{};
        m_view->GetSelW(start, end);
        if (end > start)
        {
            //获取选中部分的文本
            wstring str_selected = m_edit_wcs.substr(start, end - start);
            for (int i = 0; i < static_cast<int>(str_selected.size()); i++)
            {
                //如果当前字符是字母
                if (CCommon::IsLetter(str_selected[i]))
                {
                    //如果当前是第0个字符或前一个字符不是字母，则当前字符转换成大写
                    if (i == 0 || !CCommon::IsLetter(str_selected[i - 1]))
                        CCommon::ConvertCharCase(str_selected[i], true);
                    //其他的字符转换成小写
                    else
                        CCommon::ConvertCharCase(str_selected[i], false);
                }
            }
            //替换成转换后的文本
            m_view->ReplaceSelectionW(str_selected);
            m_view->SetSelW(start, end, m_edit_wcs);
            SetTitle();
        }
    }
}


void CSimpleNotePadDlg::OnDestroy()
{
    CBaseDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    m_text_changed_thread_exit = true;
    if (m_text_change_thread != nullptr)
        WaitForSingleObject(m_text_change_thread->m_hThread, 2000);   //等待线程退出

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
    theApp.MenuSettings().word_wrap_mode = CScintillaEditView::WW_WORD;
    m_view->SetWordWrap(theApp.MenuSettings().word_wrap, static_cast<CScintillaEditView::eWordWrapMode>(theApp.MenuSettings().word_wrap_mode));
    UpdateMenuSettingsToAllProcess();
}


void CSimpleNotePadDlg::OnWordCharacter()
{
    // TODO: 在此添加命令处理程序代码
    theApp.MenuSettings().word_wrap_mode = CScintillaEditView::WW_CHARACTER;
    m_view->SetWordWrap(theApp.MenuSettings().word_wrap, static_cast<CScintillaEditView::eWordWrapMode>(theApp.MenuSettings().word_wrap_mode));
    UpdateMenuSettingsToAllProcess();
}


void CSimpleNotePadDlg::OnWordWrapWhitespace()
{
    // TODO: 在此添加命令处理程序代码
    theApp.MenuSettings().word_wrap_mode = CScintillaEditView::WW_WHITESPACE;
    m_view->SetWordWrap(theApp.MenuSettings().word_wrap, static_cast<CScintillaEditView::eWordWrapMode>(theApp.MenuSettings().word_wrap_mode));
    UpdateMenuSettingsToAllProcess();
}


void CSimpleNotePadDlg::OnShowStatusbar()
{
    // TODO: 在此添加命令处理程序代码
    theApp.MenuSettings().show_statusbar = !theApp.MenuSettings().show_statusbar;
    ShowStatusbar(theApp.MenuSettings().show_statusbar);
    UpdateMenuSettingsToAllProcess();
}


void CSimpleNotePadDlg::OnGotoLine()
{
    // TODO: 在此添加命令处理程序代码
    CGoToLineDlg dlg;
    if (dlg.DoModal() == IDOK)
    {
        int line = dlg.GetLine();
        m_view->GotoLine(line - 1);
    }
}


void CSimpleNotePadDlg::OnCodeUtf16be()
{
    if (!BeforeChangeCode()) return;
    m_code = CodeType::UTF16BE;
    ChangeCode();
}


void CSimpleNotePadDlg::OnConvertToAnsi()
{
    ConvertToCode(CodeType::ANSI);
}


void CSimpleNotePadDlg::OnConvertToUtf8Bom()
{
    ConvertToCode(CodeType::UTF8);
}


void CSimpleNotePadDlg::OnConvertToUtf8NoBom()
{
    ConvertToCode(CodeType::UTF8_NO_BOM);
}


void CSimpleNotePadDlg::OnConvertToUtf16()
{
    ConvertToCode(CodeType::UTF16);
}


void CSimpleNotePadDlg::OnConvertToUtf16be()
{
    ConvertToCode(CodeType::UTF16BE);
}


afx_msg LRESULT CSimpleNotePadDlg::OnNpFindReplace(WPARAM wParam, LPARAM lParam)
{
    CFindReplaceDlg::Command cmd = static_cast<CFindReplaceDlg::Command>(wParam);
    switch (cmd)
    {
    case CFindReplaceDlg::Command::FIND_PREVIOUS:
        OnFindPrivious();
        break;
    case CFindReplaceDlg::Command::FIND_NEXT:
        OnFindNext();
        break;
    case CFindReplaceDlg::Command::REPLACE:
        FindReplaceTools::ReplaceTexts(m_find_replace_dlg.GetOptions(), m_view);
        OnFindNext();
        break;
    case CFindReplaceDlg::Command::REPLACE_ALL:
    {
        int replaced_count = FindReplaceTools::ReplaceAll(m_find_replace_dlg.GetOptions(), m_view);
        CString info;
        if (replaced_count != 0)
            info = CCommon::LoadTextFormat(IDS_REPLACED_INFO, { replaced_count });
        else if (!m_find_replace_dlg.GetOptions().find_str.empty())
            info = CCommon::LoadTextFormat(IDS_CANNOT_FIND_INFO, { m_find_replace_dlg.GetOptions().find_str });
        m_find_replace_dlg.SetInfoString(info);
        if (replaced_count > 0)
        {
            m_view->GetTextW(m_edit_wcs);
            UpdateStatusBarInfo();
            SetTitle();
        }
    }
    break;
    case CFindReplaceDlg::Command::REPLACE_SELECTION:
    {
        int replaced_count = FindReplaceTools::ReplaceSelection(m_find_replace_dlg.GetOptions(), m_view);
        CString info;
        if (replaced_count != 0)
            info = CCommon::LoadTextFormat(IDS_REPLACED_INFO, { replaced_count });
        else if (!m_find_replace_dlg.GetOptions().find_str.empty())
            info = CCommon::LoadTextFormat(IDS_CANNOT_FIND_INFO, { m_find_replace_dlg.GetOptions().find_str });
        m_find_replace_dlg.SetInfoString(info);
        if (replaced_count > 0)
        {
            m_view->GetTextW(m_edit_wcs);
            UpdateStatusBarInfo();
            SetTitle();
        }
    }
    break;
    case CFindReplaceDlg::Command::MARK_ALL_CLEAR:
        OnMarkAll();
        break;
    default:
        break;
    }

    return 0;
}


BOOL CSimpleNotePadDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CCommon::ProcessMsgType msg_id = static_cast<CCommon::ProcessMsgType>(pCopyDataStruct->dwData);
    if (msg_id == CCommon::ProcessMsgType::CLIP_BOARD)
    {
        std::wstring mdg_data;
        CCommon::ParseProcessMessage(pCopyDataStruct, msg_id, mdg_data);
        //收到其他进程传递的剪贴板消息，将字符串添加到当前剪贴板历史记录
        AddItemToClipboardHistory(mdg_data);
    }
    else if (msg_id == CCommon::ProcessMsgType::SETTINGS_DATA)
    {
        std::string mdg_data;
        CCommon::ParseProcessMessage(pCopyDataStruct, msg_id, mdg_data);
        try
        {
            SimpleUnpack unpack(mdg_data.data(), mdg_data.size());
            SettingsData settings_data;   //常规设置
            EditSettingData edit_settings_data;   //编辑器设置
            CUserDefinedLanguageStyle lanugage_settings_data;
            unpack >> settings_data >> edit_settings_data >> lanugage_settings_data;
            ApplySettings(settings_data, edit_settings_data, lanugage_settings_data);

        }
        catch (const std::runtime_error&)
        {
            return FALSE;
        }
    }
    else if (msg_id == CCommon::ProcessMsgType::MENU_SETTINGS_DATA)
    {
        std::string mdg_data;
        CCommon::ParseProcessMessage(pCopyDataStruct, msg_id, mdg_data);
        try
        {
            String2Object(mdg_data, theApp.MenuSettings());
            //应用设置
            m_view->SetWordWrap(theApp.MenuSettings().word_wrap, static_cast<CScintillaEditView::eWordWrapMode>(theApp.MenuSettings().word_wrap_mode));
            ShowStatusbar(theApp.MenuSettings().show_statusbar);
            m_view->ShowLineNumber(theApp.MenuSettings().show_line_number);
            m_view->SetViewEol(theApp.MenuSettings().show_eol);
            m_view->SetZoom(theApp.MenuSettings().zoom);
        }
        catch (const std::runtime_error& e)
        {
            return FALSE;
        }

    }

    return CBaseDialog::OnCopyData(pWnd, pCopyDataStruct);
}


void CSimpleNotePadDlg::OnAddDeleteComment()
{
    CLanguage::Comment comment = m_syntax_highlight.GetLanguage(m_cur_lan_index).m_comment;
    CEditorHelper helper(m_view);
    helper.AddOrRemoveComment(comment);
}


void CSimpleNotePadDlg::OnViewZoomIn()
{
    m_view->Zoom(true);
    UpdateMenuSettingsToAllProcess();
}


void CSimpleNotePadDlg::OnViewZoomOut()
{
    m_view->Zoom(false);
    UpdateMenuSettingsToAllProcess();
}


void CSimpleNotePadDlg::OnViewZoomDefault()
{
    m_view->SetZoom(0);
    UpdateMenuSettingsToAllProcess();
}


afx_msg LRESULT CSimpleNotePadDlg::OnDeleteChar(WPARAM wParam, LPARAM lParam)
{
    int pos = static_cast<int>(wParam);
    m_view->DeleteText(pos, 1);
    return 0;
}


void CSimpleNotePadDlg::OnMonitorMode()
{
    if (PathFileExists(m_file_path))
    {
        if (!m_monitor_mode)
        {
            int rtn = SHMessageBoxCheck(m_hWnd, CCommon::LoadText(IDS_MONITOR_MODE_INQUERY), APP_NAME, MB_OKCANCEL | MB_ICONWARNING, IDOK, _T("{3BEAC8DD-040F-43E6-9CC6-E25309E4A42B}"));
            if (rtn != IDOK)
                return;

            SaveInquiry(nullptr, &rtn);
            if (rtn == IDNO)     //打开监视模式前询问用户是否保存
            {
                OpenFile();         //如果不保存，则重新加载文件
                return;
            }
        }
        m_monitor_mode = !m_monitor_mode;
    }
    else
    {
        m_monitor_mode = false;
    }
    SetTitle();
    m_view->SetReadOnly(m_monitor_mode);
}


void CSimpleNotePadDlg::OnFileReload()
{
    if (!m_file_path.IsEmpty())
    {
        if (SaveInquiry())
        {
            CScintillaEditView::KeepCurrentLine keep_current_line(m_view);
            OpenFile();
        }
    }
}


void CSimpleNotePadDlg::OnEditTimeDate()
{
    //插入当前时间/日期
    CString time_data;
    SYSTEMTIME t{};
    GetLocalTime(&t);
    time_data.Format(_T("%02d:%02d %04d/%02d/%02d"), t.wHour, t.wMinute, t.wYear, t.wMonth, t.wDay);
    m_view->PasteW(time_data.GetString());
}
