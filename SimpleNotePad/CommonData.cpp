#include "stdafx.h"
#include "CommonData.h"
#include "Common.h"

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int ConstVal::GetCodeTypeItemIndex(CodeType code_type, UINT code_page)
{
    for (size_t i = 0; i < code_list.size(); i++)
    {
        if (code_list[i].code_type == code_type && code_list[i].code_page == code_page)
            return static_cast<int>(i);
    }
    return -1;
}

ConstVal* ConstVal::Instance()
{
    static std::shared_ptr<ConstVal> instance;
    if (instance == nullptr)
    {
        instance = std::make_shared<ConstVal>();
        instance->Init();
    }
    return instance.get();
}

void ConstVal::Init()
{
    code_list.emplace_back(CCommon::LoadText(_T("ANSI ("), IDS_LOCAL_CODE_PAGE, _T(")")), CodeType::ANSI, CP_ACP);
    code_list.emplace_back(CCommon::LoadText(IDS_UTF8_BOM), CodeType::UTF8, CP_ACP);
    code_list.emplace_back(CCommon::LoadText(IDS_UTF8_NO_BOM), CodeType::UTF8_NO_BOM, CP_ACP);
    code_list.emplace_back(_T("UTF-16"), CodeType::UTF16, CP_ACP);
    code_list.emplace_back(_T("UTF-16 Big Ending"), CodeType::UTF16BE, CP_ACP);
    InitCommonParts(code_list);
    code_list.emplace_back(CCommon::LoadText(IDS_DEFAULT_CODE_PAGE_FOR_NONE_UNICODE_IN_SETTINGS), CodeType::ANSI, CODE_PAGE_DEFAULT);

    code_page_list.emplace_back(CCommon::LoadText(IDS_LOCAL_CODE_PAGE), CP_ACP);
    InitCommonParts(code_page_list);
    code_page_list.emplace_back(CCommon::LoadText(IDS_SPECIFY_CODE_PAGE_MANUALLY), 0);
}

void ConstVal::InitCommonParts(vector<CodeTypeItem>& item_list)
{
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_SIMPLIFIED_CHINESE), CODE_PAGE_CHS);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_TRADITIONAL_CHINESE), CODE_PAGE_CHT);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_JAPANESE), CODE_PAGE_JP);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_WESTERN_EUROPE_LANGUAGE), CODE_PAGE_EN);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_KOREAN), CODE_PAGE_KOR);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_THAI), CODE_PAGE_THAI);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_VIETNAMESE), CODE_PAGE_VIET);
}


void MenuSettingsData::marshal(SimplePack& sp) const
{
    sp << 1 << word_wrap << show_statusbar << show_line_number << show_eol << word_wrap_mode << zoom;
}

void MenuSettingsData::unmarshal(const SimpleUnpack& su)
{
    int version{};
    su >> version >> word_wrap >> show_statusbar >> show_line_number >> show_eol >> word_wrap_mode >> zoom;
}

void SettingsData::marshal(SimplePack& sp) const
{
    sp << 2 << default_code_page_selected << default_code_page << check_update_when_start << update_source << static_cast<int>(language)
        << static_cast<int>(default_code);
}

void SettingsData::unmarshal(const SimpleUnpack& su)
{
    int version{};
    int lan{};
    int _default_code{};
    su >> version >> default_code_page_selected >> default_code_page >> check_update_when_start >> update_source >> lan;
    if (version >= 2)
        su >> _default_code;
    language = static_cast<Language>(lan);
    default_code = static_cast<CodeType>(_default_code);
}

void EditSettingData::marshal(SimplePack& sp) const
{
    sp << 2
        << current_line_highlight
        << static_cast<uint32_t>(current_line_highlight_color)
        << static_cast<uint32_t>(background_color)
        << static_cast<uint32_t>(selection_back_color)
        << std::wstring(font_name.GetString())
        << font_size
        << tab_width
        << show_indentation_guides
        << show_auto_comp_list
        << mark_same_words
        << mark_matched_brackets
        << mark_matched_html_mark
        << brackets_auto_comp
        << html_mark_auto_comp
        << std::wstring(font_name_hex.GetString())
        << font_size_hex
        << show_invisible_characters_hex
        << remember_clipboard_history
        ;
}

void EditSettingData::unmarshal(const SimpleUnpack& su)
{
    int version;
    su >> version;
    su >> current_line_highlight;
    uint32_t _current_line_highlight_color{};
    su >> _current_line_highlight_color;
    current_line_highlight_color = _current_line_highlight_color;
    uint32_t _background_color{};
    su >> _background_color;
    background_color = _background_color;
    uint32_t _selection_back_color{};
    su >> _selection_back_color;
    selection_back_color = _selection_back_color;
    std::wstring _font_name;
    su >> _font_name;
    font_name = _font_name.c_str();
    su >> font_size;
    su >> tab_width;
    su >> show_indentation_guides;
    su >> show_auto_comp_list;
    su >> mark_same_words;
    su >> mark_matched_brackets;
    su >> mark_matched_html_mark;
    su >> brackets_auto_comp;
    su >> html_mark_auto_comp;
    std::wstring _font_name_hex;
    su >> _font_name_hex;
    font_name_hex = _font_name_hex.c_str();
    su >> font_size_hex;
    su >> show_invisible_characters_hex;
    if (version >= 0)
        su >> remember_clipboard_history;
}
