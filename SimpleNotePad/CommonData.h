#pragma once

//常规设置
struct SettingsData
{
    int default_code_page_selected{};
    UINT default_code_page{};
};

//编辑器设置
struct EditSettingData
{
    bool current_line_highlight{};      //当前行高亮显示
    COLORREF background_color{ RGB(255, 255, 255) };  //背景颜色

};

////自定义消息
//inline int GetUserDefinedMessgeId()
//{
//    static int current_msg_id = WM_USER + 100;
//    current_msg_id++;
//    return current_msg_id;
//}
//
//const int WM_COLOR_SELECTED = GetUserDefinedMessgeId();     //响应颜色选择控件选择了颜色
#define WM_COLOR_SELECTED (WM_USER + 100)     //响应颜色选择控件选择了颜色