
// SimpleNotePad.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "CommonData.h"
#include "UserDefinedLanguageStyle.h"

// CSimpleNotePadApp:
// 有关此类的实现，请参阅 SimpleNotePad.cpp
//

class CSimpleNotePadApp : public CWinApp
{
public:
	CSimpleNotePadApp();

	bool m_compare_dialog_exit;	//用于指示文件比较对话框是否已退出

    void DPIFromWindow(CWnd* pWnd);

    int DPI(int pixel);
    int DPI(double pixel);

    SettingsData GetGeneralSettings() const;
    void SetGeneralSettings(const SettingsData& data);

    const EditSettingData& GetEditSettings() const;
    void SetEditSettings(const EditSettingData& data);

    const CUserDefinedLanguageStyle& GetLanguageSettings() const;
    void SetLanguageSettings(const CUserDefinedLanguageStyle& data);

    void CheckUpdate(bool message);     //检查更新，如果message为true，则在检查时弹出提示信息
    void CheckUpdateInThread(bool message); //在后台线程中检查更新
    //启动时检查更新线程函数
    static UINT CheckUpdateThreadFunc(LPVOID lpParam);
    bool IsCheckingForUpdate() const { return m_checking_update; }      //是否正在检查更新

    //获取一个图标资源，如果资源还未加载，会自动加载。
    //由于本函数中使用了DPI函数，因此本函数必须确保在DPIFromWindow之后调用
    HICON GetMenuIcon(UINT id);

    //获取最近打开文件列表
    const std::vector<CString>& GetRecentFileList();

    void RemoveFromRecentFileList(LPCTSTR file_path);

    void WriteStringList(LPCTSTR app_name, LPCTSTR key_name, const std::vector<std::wstring>& string_list); //将一个字符串列表保存到配置
    void GetStringList(LPCTSTR app_name, LPCTSTR key_name, std::vector<std::wstring>& string_list);         //从配置读取一个字符串列表

    bool AddExplorerContextMenuItem();      //在资源管理器右键菜单中添加“使用SimpleNotePad打开”菜单项
    bool RemoveExplorerContextMenuItem();   //删除资源管理器右键菜单中的“使用SimpleNotePad打开”菜单项
    bool IsExplorerContextMenuExist();      //资源管理器右键菜单中的“使用SimpleNotePad打开”菜单项是否存在

private:
    void LoadConfig();
    void SaveConfig();

private:
    HMODULE m_hScintillaModule;
    int m_dpi;		//当前显示器的DPI设置100%时为96

    //选项设置数据
    SettingsData m_settings_data;   //常规设置
    EditSettingData m_edit_settings_data;   //编辑器设置
    CUserDefinedLanguageStyle m_lanugage_settings_data;

    bool m_checking_update{ false };        //是否正在检查更新
    std::map<UINT, HICON> m_menu_icons;      //菜单图标资源。key是图标资源的ID，vlaue是图标的句柄
    std::vector<CString> m_recent_file_list;     //最近打开文件列表

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
    afx_msg void OnFileNewWindow();
};

extern CSimpleNotePadApp theApp;
