// CodeConvertDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "CodeConvertDlg.h"
#include "afxdialogex.h"
#include "Common.h"


// CCodeConvertDlg 对话框

IMPLEMENT_DYNAMIC(CCodeConvertDlg, CBaseDialog)

CCodeConvertDlg::CCodeConvertDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_CODE_CONVERT_DIALOG, pParent)
{

}

CCodeConvertDlg::~CCodeConvertDlg()
{
}

void CCodeConvertDlg::InitComboBox(CComboBox& combo_box)
{
    combo_box.ResetContent();
    for (const auto& item : CONST_VAL::code_list)
    {
        combo_box.AddString(item.name);
    }
    if (!CONST_VAL::code_list.empty())
        combo_box.SetCurSel(0);
}

CString CCodeConvertDlg::GetDialogName() const
{
    return _T("CodeConvertDlg");
}

void CCodeConvertDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_INPUT_COMBO, m_input_combo);
    DDX_Control(pDX, IDC_OUTPUT_COMBO, m_output_combo);
}


BEGIN_MESSAGE_MAP(CCodeConvertDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &CCodeConvertDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCodeConvertDlg 消息处理程序


BOOL CCodeConvertDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    //初始化下拉列表
    InitComboBox(m_input_combo);
    InitComboBox(m_output_combo);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CCodeConvertDlg::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码

    int index = m_input_combo.GetCurSel();
    if (index < 0 || index >= static_cast<int>(CONST_VAL::code_list.size()))
        return;
    CodeType code_type_input{ CONST_VAL::code_list[index].code_type };
    UINT code_page_input{ CONST_VAL::code_list[index].code_page };
    if (code_page_input == CODE_PAGE_DEFAULT)
        code_page_input = theApp.m_settings_data.default_code_page;

    index = m_output_combo.GetCurSel();
    if (index < 0 || index >= static_cast<int>(CONST_VAL::code_list.size()))
        return;
    CodeType code_type_output{ CONST_VAL::code_list[index].code_type };
    UINT code_page_output{ CONST_VAL::code_list[index].code_page };
    if (code_page_output == CODE_PAGE_DEFAULT)
        code_page_output = theApp.m_settings_data.default_code_page;

    CString str_input;
    GetDlgItemText(IDC_INPUT_EDIT, str_input);
    if (str_input.IsEmpty())
        return;

    bool char_cannot_convert{};
    string row_data = CCommon::UnicodeToStr(str_input.GetString(), char_cannot_convert, code_type_input, code_page_input);
    wstring str_out_put = CCommon::StrToUnicode(row_data, code_type_output, code_page_output);

    SetDlgItemText(IDC_OUTPUT_EDIT, str_out_put.c_str());
}
