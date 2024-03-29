﻿// ColorStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorStatic.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
{

}

CColorStatic::~CColorStatic()
{
}

void CColorStatic::SetFillColor(COLORREF fill_color)
{
	m_fill_color = fill_color;
	Invalidate();
}


void CColorStatic::EnableWindow(bool enable)
{
    CStatic::EnableWindow(enable);
    Invalidate(FALSE);
}

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CColorStatic 消息处理程序




void CColorStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
	CRect rect;
	GetClientRect(rect);
    if (IsWindowEnabled())
    {
        dc.FillSolidRect(rect, m_fill_color);
    }
    else
    {
        CBrush brush(HS_BDIAGONAL, RGB(190, 190, 190));
        dc.FillRect(rect, &brush);
    }
}
