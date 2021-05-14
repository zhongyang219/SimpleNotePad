
// SimpleNotePad.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "SimpleNotePadDlg.h"
#include "UpdateHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimpleNotePadApp

BEGIN_MESSAGE_MAP(CSimpleNotePadApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSimpleNotePadApp 构造

CSimpleNotePadApp::CSimpleNotePadApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

void CSimpleNotePadApp::DPIFromWindow(CWnd* pWnd)
{
    CWindowDC dc(pWnd);
    HDC hDC = dc.GetSafeHdc();
    m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
}

int CSimpleNotePadApp::DPI(int pixel)
{
    return pixel * m_dpi / 96;
}

SettingsData CSimpleNotePadApp::GetGeneralSettings() const
{
    return m_settings_data;
}

void CSimpleNotePadApp::SetGeneralSettings(const SettingsData & data)
{
    m_settings_data = data;
}

EditSettingData CSimpleNotePadApp::GetEditSettings() const
{
    return m_edit_settings_data;
}

void CSimpleNotePadApp::SetEditSettings(const EditSettingData & data)
{
    m_edit_settings_data = data;
}

void CSimpleNotePadApp::CheckUpdate(bool message)
{
    if (m_checking_update)      //如果还在检查更新，则直接返回
        return;
    CFlagLocker update_locker(m_checking_update);
    CWaitCursor wait_cursor;

    wstring version;        //程序版本
    wstring link;           //下载链接
    wstring contents_zh_cn; //更新内容（简体中文）
    //wstring contents_en;    //更新内容（English）
    CUpdateHelper update_helper;
    update_helper.SetUpdateSource(static_cast<CUpdateHelper::UpdateSource>(m_settings_data.update_source));
    if (!update_helper.CheckForUpdate())
    {
        if (message)
            AfxMessageBox(CCommon::LoadText(IDS_CHECK_UPDATE_FAILD), MB_OK | MB_ICONWARNING);
        return;
    }
    version = update_helper.GetVersion();
#ifdef _M_X64
    link = update_helper.GetLink64();
#else
    link = update_helper.GetLink();
#endif
    contents_zh_cn = update_helper.GetContentsZhCn();
    //contents_en = update_helper.GetContentsEn();
    if (version.empty() || link.empty())
    {
        if (message)
        {
            CString info = CCommon::LoadText(IDS_CHECK_UPDATE_ERROR);
            AfxMessageBox(info, MB_OK | MB_ICONWARNING);
        }
        return;
    }
    if (version > VERSION)      //如果服务器上的版本大于本地版本
    {
        CString info;
        //根据语言设置选择对应语言版本的更新内容
        //int language_code = _ttoi(CCommon::LoadText(IDS_LANGUAGE_CODE));
        wstring contents_lan;
        //switch (language_code)
        //{
        //case 2: contents_lan = contents_zh_cn; break;
        //default: contents_lan = contents_en; break;
        //}
        contents_lan = contents_zh_cn;

        if (contents_lan.empty())
            info.Format(CCommon::LoadText(IDS_UPDATE_AVLIABLE), version.c_str());
        else
            info.Format(CCommon::LoadText(IDS_UPDATE_AVLIABLE2), version.c_str(), contents_lan.c_str());

        if (AfxMessageBox(info, MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            ShellExecute(NULL, _T("open"), link.c_str(), NULL, NULL, SW_SHOW);      //转到下载链接
        }
    }
    else
    {
        if (message)
            AfxMessageBox(CCommon::LoadText(IDS_ALREADY_UPDATED), MB_OK | MB_ICONINFORMATION);
    }
}

void CSimpleNotePadApp::CheckUpdateInThread(bool message)
{
    AfxBeginThread(CheckUpdateThreadFunc, (LPVOID)message);
}

UINT CSimpleNotePadApp::CheckUpdateThreadFunc(LPVOID lpParam)
{
    //CCommon::SetThreadLanguage(theApp.m_general_data.language);     //设置线程语言
    theApp.CheckUpdate(lpParam);        //检查更新
    return 0;
}

void CSimpleNotePadApp::LoadConfig()
{
    //载入选项设置
    m_settings_data.default_code_page_selected = GetProfileInt(L"config", L"default_code_page_selected", 0);
    m_settings_data.default_code_page = GetProfileInt(L"config", L"default_code_page", 0);
    m_settings_data.update_source = GetProfileInt(L"config", L"update_source", 0);

    //载入编辑器设置
    m_edit_settings_data.current_line_highlight = (GetProfileInt(L"config", L"current_line_highlight", 0) != 0);
    m_edit_settings_data.current_line_highlight_color = GetProfileInt(L"config", L"current_line_highlight_color", RGB(234, 243, 253));
    m_edit_settings_data.background_color = GetProfileInt(_T("config"), _T("background_color"), RGB(255, 255, 255));
	m_edit_settings_data.selection_back_color = GetProfileInt(_T("config"), _T("selection_back_color"), RGB(192, 192, 192));
    m_edit_settings_data.font_name = GetProfileString(_T("config"), _T("font_name"), _T("微软雅黑"));
    m_edit_settings_data.font_size = GetProfileInt(_T("config"), _T("font_size"), 10);
    m_edit_settings_data.tab_width = GetProfileInt(_T("config"), _T("tab_width"), 4);

    //十六进制查看器
    m_edit_settings_data.font_name_hex = GetProfileString(_T("hex_editor"), _T("font_name"), _T("新宋体"));
    m_edit_settings_data.font_size_hex = GetProfileInt(_T("hex_editor"), _T("font_size"), 10);
    m_edit_settings_data.show_invisible_characters_hex = GetProfileInt(_T("hex_editor"), _T("show_invisible_characters"), 0);

}

void CSimpleNotePadApp::SaveConfig()
{
    //保存选项设置
    WriteProfileInt(L"config", L"default_code_page_selected", m_settings_data.default_code_page_selected);
    WriteProfileInt(L"config", L"default_code_page", m_settings_data.default_code_page);

    //保存编辑器设置
    WriteProfileInt(L"config", L"current_line_highlight", m_edit_settings_data.current_line_highlight);
    WriteProfileInt(L"config", L"current_line_highlight_color", m_edit_settings_data.current_line_highlight_color);
    WriteProfileInt(L"config", L"background_color", m_edit_settings_data.background_color);
    WriteProfileInt(L"config", L"selection_back_color", m_edit_settings_data.selection_back_color);
    WriteProfileString(_T("config"), _T("font_name"), m_edit_settings_data.font_name);
    WriteProfileInt(L"config", L"font_size", m_edit_settings_data.font_size);
    WriteProfileInt(_T("config"), _T("tab_width"), m_edit_settings_data.tab_width);

    //十六进制查看器
    WriteProfileString(_T("hex_editor"), _T("font_name"), m_edit_settings_data.font_name_hex);
    WriteProfileInt(L"hex_editor", L"font_size", m_edit_settings_data.font_size_hex);
    WriteProfileInt(_T("hex_editor"), _T("show_invisible_characters"), m_edit_settings_data.show_invisible_characters_hex);
}


// 唯一的一个 CSimpleNotePadApp 对象

CSimpleNotePadApp theApp;


// CSimpleNotePadApp 初始化

BOOL CSimpleNotePadApp::InitInstance()
{
	wstring cmd_line{ m_lpCmdLine };
	bool is_restart{ cmd_line.find(L"RestartByRestartManager") != wstring::npos };		//如果命令行参数中含有字符串“RestartByRestartManager”则说明程序是被Windows重新启动的
	if (is_restart)				//如果程序被重新启动，则直接退出程序
		return FALSE;

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	CString reg_key = _T("Apps By ZhongYang");
#ifdef _DEBUG
	reg_key += _T(" (Debug)");
#endif
	SetRegistryKey(reg_key);

    m_hScintillaModule = LoadLibrary(_T("SciLexer.dll"));
    if (m_hScintillaModule == NULL)
    {
        AfxMessageBox(_T("加载 SciLexer.dll 失败，程序即将退出！"), MB_ICONERROR | MB_OK);
        return FALSE;
    }

    LoadConfig();

    CSimpleNotePadDlg dlg(m_lpCmdLine);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: The dialog creation failed and the application will terminate unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: If you use MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

    SaveConfig();

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
