// EditEx.cpp : 实现文件
//

#include "stdafx.h"
#include "EditEx.h"


// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{

}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CEditEx 消息处理程序




BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//按Ctrl+A全选
	if ((GetKeyState(VK_CONTROL) & 0x80) && (pMsg->wParam == 'A'))
	{
		SetSel(0, -1);
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}


void CEditEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString edit_str;
	GetWindowText(edit_str);
	int length{ edit_str.GetLength() };
	int cursor_pos = CharFromPos(point);		//获取光标位置
	cursor_pos = LOWORD(cursor_pos);
	int start_pos{ cursor_pos };		//选择的起始位置
	int end_pos{ cursor_pos };			//选择的结束位置
	while (start_pos >= 0 && CCommon::IsValidNameChar(edit_str[start_pos]))
	{
		start_pos--;
	}
	while (end_pos < length && CCommon::IsValidNameChar(edit_str[end_pos]))
	{
		end_pos++;
	}
	SetSel(start_pos + 1, end_pos);
	//CEdit::OnLButtonDblClk(nFlags, point);
}
