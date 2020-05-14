
// SimpleNotePad.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSimpleNotePadApp: 
// 有关此类的实现，请参阅 SimpleNotePad.cpp
//

class CSimpleNotePadApp : public CWinApp
{
public:
	CSimpleNotePadApp();

	wstring m_config_path;		//配置文件所在的路径

	bool m_compare_dialog_exit;	//用于指示文件比较对话框是否已退出

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CSimpleNotePadApp theApp;