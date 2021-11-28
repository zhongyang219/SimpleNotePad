#pragma once
#include "stdafx.h"
#include "tinyxml2.h"

class CLanguage
{
public:
    struct SyntaxStyle
    {
        std::wstring name;
        int id{};
        COLORREF color;
        bool bold{};
        bool italic{};
    };
    std::wstring m_name;
    int m_id{};
    std::set<wstring> m_ext;
    std::map<int, std::string> m_keywords_list;
    std::vector<SyntaxStyle> m_syntax_list;

    struct Comment
    {
        std::string line;
        std::string start;
        std::string end;

        bool isMultiLeneCommentValid() const
        {
            return !start.empty() && !end.empty();
        }
    };
    Comment m_comment;

    //从配置文件“Language”节点解析语法高亮信息
    //ele: xml节点
    //syntax_from: 如果语法高亮颜色设置要从其他语言复制，则将该语言的名称保存在这里
    //comment_from: 如果注释设置要从其他语言复制，则将该语言的名称保存在这里
    void FromXmlElement(tinyxml2::XMLElement* ele, wstring& syntax_from, wstring& comment_from);
};

class CSyntaxHighlight
{
public:
    void LoadFromFile(const wchar_t* file_path);

    const CLanguage& FindLanguageByFileName(const wstring& file_name) const;
    const CLanguage& FindLanguageByName(const wchar_t* name) const;
    const CLanguage& GetLanguage(int index) const;
    int IndexOf(const wstring& name) const;

    const std::vector<CLanguage>& GetLanguageList() const;

private:
    std::vector<CLanguage> m_language_list;
    CLanguage m_empty_language{};
};
