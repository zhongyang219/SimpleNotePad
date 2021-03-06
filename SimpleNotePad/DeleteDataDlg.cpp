﻿// DeleteDataDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "DeleteDataDlg.h"
#include "afxdialogex.h"


// CDeleteDataDlg 对话框

IMPLEMENT_DYNAMIC(CDeleteDataDlg, CDialog)

CDeleteDataDlg::CDeleteDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DELETE_DATA_DIALOG, pParent)
{

}

CDeleteDataDlg::~CDeleteDataDlg()
{
}

void CDeleteDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDeleteDataDlg, CDialog)
END_MESSAGE_MAP()


// CDeleteDataDlg 消息处理程序


void CDeleteDataDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CString strTmp;
	GetDlgItemText(IDC_DELETE_ADDR_EDIT, strTmp);
	m_address = wcstoul(strTmp, nullptr, 16);

	GetDlgItemText(IDC_DELETE_SIZE_EDIT, strTmp);
	m_size = _ttoi(strTmp);

	CDialog::OnOK();
}
