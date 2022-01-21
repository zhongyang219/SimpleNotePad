#include "stdafx.h"
#include "CommonData.h"

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
    sp << 1 << default_code_page_selected << default_code_page << check_update_when_start << update_source << static_cast<int>(language);
}

void SettingsData::unmarshal(const SimpleUnpack& su)
{
    int version{};
    int lan{};
    su >> version >> default_code_page_selected >> default_code_page >> check_update_when_start >> update_source >> lan;
    language = static_cast<Language>(lan);
}

void EditSettingData::marshal(SimplePack& sp) const
{
    sp << 1
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

}
