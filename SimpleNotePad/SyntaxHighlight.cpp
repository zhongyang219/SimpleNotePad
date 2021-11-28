#include "stdafx.h"
#include "SyntaxHighlight.h"
#include "TinyXml2Helper.h"
#include "Common.h"
#include <algorithm>
#include "FilePathHelper.h"

void CLanguage::FromXmlElement(tinyxml2::XMLElement* ele, wstring& syntax_from, wstring& comment_from)
{
    syntax_from.clear();
    m_name = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(ele, "name"), CodeType::UTF8_NO_BOM);
    m_id = atoi(CTinyXml2Helper::ElementAttribute(ele, "lexId"));
    wstring ext_list = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(ele, "ext"), CodeType::UTF8_NO_BOM);
    std::vector<wstring> ext_vec;
    CCommon::StringSplit(ext_list, L' ', ext_vec);
    for (const auto& ext : ext_vec)
    {
        m_ext.insert(ext);
    }

    //遍历子节点
    CTinyXml2Helper::IterateChildNode(ele, [&](tinyxml2::XMLElement* child)
    {
        string node_name = CTinyXml2Helper::ElementName(child);
        if (node_name == "Keywords")
        {
            int keywords_id = atoi(CTinyXml2Helper::ElementAttribute(child, "id"));
            m_keywords_list[keywords_id] = CTinyXml2Helper::ElementText(child);
        }
        else if (node_name == "Comment")
        {
            string str_comment_from = CTinyXml2Helper::ElementAttribute(child, "copyFrom");
            if (!str_comment_from.empty())
            {
                comment_from = CCommon::StrToUnicode(str_comment_from, CodeType::UTF8_NO_BOM);
            }
            else
            {
                m_comment.line = CTinyXml2Helper::ElementAttribute(child, "commentLine");
                m_comment.start = CTinyXml2Helper::ElementAttribute(child, "commentStart");
                m_comment.end = CTinyXml2Helper::ElementAttribute(child, "commentEnd");
            }
        }
        else if (node_name == "SyntaxList")
        {
            string str_syntax_from = CTinyXml2Helper::ElementAttribute(child, "copyFrom");
            if (!str_syntax_from.empty())
            {
                syntax_from = CCommon::StrToUnicode(str_syntax_from, CodeType::UTF8_NO_BOM);
            }
            else
            {
                CTinyXml2Helper::IterateChildNode(child, [&](tinyxml2::XMLElement* syntax_node)
                {
                    SyntaxStyle syntax_style;
                    syntax_style.name = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(syntax_node, "name"), CodeType::UTF8_NO_BOM);
                    syntax_style.id = atoi(CTinyXml2Helper::ElementAttribute(syntax_node, "id"));
                    sscanf_s(CTinyXml2Helper::ElementAttribute(syntax_node, "color"), "%x", &syntax_style.color);
                    syntax_style.bold = (atoi(CTinyXml2Helper::ElementAttribute(syntax_node, "bold")) != 0);
                    syntax_style.italic = (atoi(CTinyXml2Helper::ElementAttribute(syntax_node, "italic")) != 0);
                    m_syntax_list.push_back(syntax_style);
                });
            }
        }
    });
}

void CSyntaxHighlight::LoadFromFile(const wchar_t* file_path)
{
    tinyxml2::XMLDocument doc;
    if (CTinyXml2Helper::LoadXmlFile(doc, file_path))
    {
        CTinyXml2Helper::IterateChildNode(doc.FirstChildElement(), [this](tinyxml2::XMLElement* child)
        {
            CLanguage lan;
            wstring syntax_from;
            wstring comment_from;
            lan.FromXmlElement(child, syntax_from, comment_from);
            if (!syntax_from.empty())       //如果语言的语法高亮颜色设置从其他语言复制，则在已读取列表中找到该语言
            {
                CLanguage lan_copy = FindLanguageByName(syntax_from.c_str());
                if (!lan_copy.m_name.empty())
                {
                    lan.m_syntax_list = lan_copy.m_syntax_list;
                }
            }
            if (!comment_from.empty())       //如果语言的注释设置从其他语言复制，则在已读取列表中找到该语言
            {
                CLanguage lan_copy = FindLanguageByName(comment_from.c_str());
                if (!lan_copy.m_name.empty())
                {
                    lan.m_comment = lan_copy.m_comment;
                }
            }
            m_language_list.push_back(lan);
        });
    }
    //将列表按名称排序
    std::sort(m_language_list.begin(), m_language_list.end(), [](const CLanguage& a, const CLanguage& b)
    {
        return a.m_name < b.m_name;
    });
}

const CLanguage& CSyntaxHighlight::FindLanguageByFileName(const wstring& file_name) const
{
    CFilePathHelper helper(file_name);
    wstring ext = helper.GetFileExtension();
    for (const auto& lan : m_language_list)
    {
        if (lan.m_ext.find(L'$' + file_name) != lan.m_ext.end())
        {
            return lan;
        }

        if (lan.m_ext.find(ext) != lan.m_ext.end())
        {
            return lan;
        }
    }
    return m_empty_language;
}

const CLanguage& CSyntaxHighlight::FindLanguageByName(const wchar_t* name) const
{
    for (const auto& lan : m_language_list)
    {
        if (lan.m_name == name)
        {
            return lan;
        }
    }
    return m_empty_language;
}

const CLanguage& CSyntaxHighlight::GetLanguage(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_language_list.size()))
        return m_language_list[index];
    else
        return m_empty_language;
}

int CSyntaxHighlight::IndexOf(const wstring& name) const
{
    int index = 0;
    for (const auto& lan : m_language_list)
    {
        if (lan.m_name == name)
            return index;
        index++;
    }
    return -1;
}

const std::vector<CLanguage>& CSyntaxHighlight::GetLanguageList() const
{
    return m_language_list;
}
