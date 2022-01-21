#pragma once
#ifndef COMMON_DATA_H
#define COMMON_DATA_H
#include "simplemarshal.h"
using namespace dakuang;

//语言
enum class Language
{
    FOLLOWING_SYSTEM,       //跟随系统
    ENGLISH,                //英语
    SIMPLIFIED_CHINESE,     //简体中文
};

//菜单中的设置数据d
struct MenuSettingsData : public Marshallable
{
    bool word_wrap;		//是否自动换行
    bool show_statusbar{ true };    //是否显示状态栏
    bool show_line_number{ false }; //是否显示行号
    bool show_eol{ false };         //是否显示换行符
    int word_wrap_mode{ };          //自动换行模式
    int zoom{};

    // 通过 Marshallable 继承
    virtual void marshal(SimplePack&) const override;
    virtual void unmarshal(const SimpleUnpack&) override;
};

//常规设置
struct SettingsData : public Marshallable
{
    int default_code_page_selected{};
    UINT default_code_page{};
    bool check_update_when_start{};
    int update_source{};
    //语言
    Language language;

    // 通过 Marshallable 继承
    virtual void marshal(SimplePack&) const override;
    virtual void unmarshal(const SimpleUnpack&) override;
};

//编辑器设置
struct EditSettingData : public Marshallable
{
    bool current_line_highlight{};      //当前行高亮显示
    COLORREF current_line_highlight_color{ RGB(234, 243, 253) };    //当前行高亮颜色
    COLORREF background_color{ RGB(255, 255, 255) };  //背景颜色
    COLORREF selection_back_color{};        //选中的背景颜色
    CString font_name;	//字体名称
    int font_size{ 9 };		//字体大小
    int tab_width{ 4 };     //制表符宽度
    bool show_indentation_guides{};
    bool show_auto_comp_list{}; //显示自动完成列表
    bool mark_same_words;           //标记相同的单词
    bool mark_matched_brackets{};   //标记匹配的括号
    bool mark_matched_html_mark{};  //标记匹配的html标签
    bool brackets_auto_comp{};      //括号自动完成
    bool html_mark_auto_comp{};     //html标签自动完成

    //十六进制查看器
    CString font_name_hex;	//十六进制查看器的字体名称
    int font_size_hex{ 9 };		//十六进制查看器的字体大小
    bool show_invisible_characters_hex{ false };    //是否显示不可见字符

    // 通过 Marshallable 继承
    virtual void marshal(SimplePack&) const override;
    virtual void unmarshal(const SimpleUnpack&) override;
};

//自定义消息
static int GetUserDefinedMessgeId()
{
    static int current_msg_id = WM_USER + 100;
    current_msg_id++;
    return current_msg_id;
}

#define DECLARE_USER_MESSAGE_ID(id) const int id = GetUserDefinedMessgeId();

DECLARE_USER_MESSAGE_ID(WM_COLOR_SELECTED)     //响应颜色选择控件选择了颜色
DECLARE_USER_MESSAGE_ID(WM_NP_FIND_REPLACE)    //查找替换消息，WPARA为CFindReplaceDlg::FindMode类型
DECLARE_USER_MESSAGE_ID(WM_DELETE_CHAR)        //删除编辑器一个字符，wParam为删除位置


//通过构造函数传递一个bool变量的引用，在构造时将其置为true，析构时置为false
class CFlagLocker
{
public:
    CFlagLocker(bool& flag)
        : m_flag(flag)
    {
        m_flag = true;
    }

    ~CFlagLocker()
    {
        m_flag = false;
    }

private:
    bool& m_flag;
};

#endif // !COMMON_DATA_H
