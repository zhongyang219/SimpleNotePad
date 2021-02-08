
// SimpleNotePad.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "CommonData.h"

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

    SettingsData GetGeneralSettings() const;
    void SetGeneralSettings(const SettingsData& data);

    EditSettingData GetEditSettings() const;
    void SetEditSettings(const EditSettingData& data);

private:
    void LoadConfig();
    void SaveConfig();

private:
    HMODULE m_hScintillaModule;
    int m_dpi;		//当前显示器的DPI设置100%时为96

    //选项设置数据
    SettingsData m_settings_data;   //常规设置
    EditSettingData m_edit_settings_data;   //编辑器设置


// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CSimpleNotePadApp theApp;