
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持


#include<string>
#include<iostream>
#include<fstream>
using std::string;
using std::wstring;
using std::ifstream;
using std::ofstream;
#include<vector>
#include <afxwin.h>
#include <afxcontrolbars.h>
#include <afxwin.h>
#include <afxwin.h>
using std::vector;
#include <map>
#include <set>
#include <afxwin.h>
#include <afxwin.h>





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#ifdef UNICODE
typedef std::wstring _tstring;
typedef wchar_t _tchar;
#else
typedef std::string _tstring;
typedef char _tchar;
#endif // !UNICODE

#define VERSION L"1.42"

#define HEX_LOG_PATH _T(".\\hex_edit.log")

#define IDC_SAVE_COMBO_BOX 1990		//定义添加到“另存为”对话框中的组合框的ID

#define MAX_FILE_SIZE 10485760		//定义可以打开的最大文件大小
#define MAX_COMPARE_SIZE 4194304		//定义文件二进制比较时的最大文件大小

#define RECENT_FILE_LIST_MAX_SIZE 12
#define CLIPBOARD_ITEM_MAX 8
#define FIND_REPLACE_AUTO_FILL_MAX_LENGTH 32    //将选中文本自动填充到查找或替换对话框中的查找框中的最大长度

#define WM_NP_FIND_REPLACE (WM_USER + 100)  //查找替换消息，WPARA为CFindReplaceDlg::FindMode类型

#define WM_NEXT_USER_MSG (WM_USER + 101)

#undef min
#undef max

#define SAFE_DELETE(p) do \
{\
    if(p != nullptr) \
    { \
        delete p; \
        p = nullptr; \
    } \
} while (false)
