#ifndef CONFIGDATA_H
#define CONFIGDATA_H

#include <QColor>
#include "tools/UserDefinedLanguageStyle.h"

enum class CodeType
{
    ANSI,
    UTF8,
    UTF8_NO_BOM,
    UTF16,
    UTF16BE,
    AUTO
};

//菜单中的设置数据d
struct MenuSettingsData
{
    bool word_wrap{ true };		//是否自动换行
    bool show_statusbar{ true };    //是否显示状态栏
    bool show_line_number{ false }; //是否显示行号
    bool show_eol{ false };         //是否显示换行符
    int word_wrap_mode{ };          //自动换行模式
    int zoom{};
};

//常规设置
struct SettingsData
{
    int default_code_page_selected{};
    unsigned int default_code_page{};
    bool check_update_when_start{};
    int update_source{};

    CodeType default_code{};

};

//编辑器设置
struct EditSettingData
{
    bool current_line_highlight{};      //当前行高亮显示
    QColor current_line_highlight_color{ "#eaf3fd" };    //当前行高亮颜色
    QColor background_color{ "#ffffff" };  //背景颜色
    QColor selection_back_color{};        //选中的背景颜色
    QString font_name;	//字体名称
    int font_size{ 9 };		//字体大小
    int tab_width{ 4 };     //制表符宽度
    bool show_indentation_guides{};
    bool remember_clipboard_history{};  //在程序退出后记住剪贴板历史记录

    bool show_auto_comp_list{}; //显示自动完成列表
    bool mark_same_words{ true };           //标记相同的单词
    bool mark_matched_brackets{};   //标记匹配的括号
    bool mark_matched_html_mark{};  //标记匹配的html标签
    bool brackets_auto_comp{};      //括号自动完成
    bool html_mark_auto_comp{};     //html标签自动完成

    //十六进制查看器
    QString font_name_hex;	//十六进制查看器的字体名称
    int font_size_hex{ 9 };		//十六进制查看器的字体大小
    bool show_invisible_characters_hex{ false };    //是否显示不可见字符

};

//////////////////////////////////////////////////////////////////////////
#define g_config ConfigData::Instance()
class ConfigData
{
public:
    ConfigData();
    static ConfigData& Instance();

    void LoadConfig();
    void SaveConfig() const;

    MenuSettingsData& MenuSettings();

    const SettingsData& GetGeneralSettings() const;
    void SetGeneralSettings(const SettingsData& data);

    const EditSettingData& GetEditSettings() const;
    void SetEditSettings(const EditSettingData& data);

    const CUserDefinedLanguageStyle& GetLanguageSettings() const;
    void SetLanguageSettings(const CUserDefinedLanguageStyle& data);

private:
    static ConfigData m_ins;
    //选项设置数据
    MenuSettingsData m_menu_settings_data;  //菜单中的设置
    SettingsData m_settings_data;   //常规设置
    EditSettingData m_edit_settings_data;   //编辑器设置
    CUserDefinedLanguageStyle m_lanugage_settings_data;
};

#endif // CONFIGDATA_H
