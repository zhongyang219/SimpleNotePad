#pragma once
#ifndef COMMON_DATA_H
#define COMMON_DATA_H

//语言
enum class Language
{
    FOLLOWING_SYSTEM,       //跟随系统
    ENGLISH,                //英语
    SIMPLIFIED_CHINESE,     //简体中文
};

//常规设置
struct SettingsData
{
    int default_code_page_selected{};
    UINT default_code_page{};
    bool check_update_when_start{};
    int update_source{};
    //语言
    Language language;
};

//编辑器设置
struct EditSettingData
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

    //十六进制查看器
    CString font_name_hex;	//十六进制查看器的字体名称
    int font_size_hex{ 9 };		//十六进制查看器的字体大小
    bool show_invisible_characters_hex{ false };    //是否显示不可见字符
};

//自定义消息
inline int GetUserDefinedMessgeId()
{
    static int current_msg_id = WM_USER + 100;
    current_msg_id++;
    return current_msg_id;
}

namespace MessageId
{
    const int a = GetUserDefinedMessgeId();
};

//static const int WM_COLOR_SELECTED = GetUserDefinedMessgeId();     //响应颜色选择控件选择了颜色
#define WM_COLOR_SELECTED (WM_USER + 100)     //响应颜色选择控件选择了颜色

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
