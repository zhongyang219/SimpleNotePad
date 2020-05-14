//CEdit的派生类
#pragma once
#include "Common.h"

// CEditEx

class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)

public:
	CEditEx();
	virtual ~CEditEx();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


