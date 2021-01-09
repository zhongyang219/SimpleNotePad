#include "stdafx.h"
#include "SyntaxHighlight.h"
#include "TinyXml2Helper.h"
#include "Common.h"

void CLanguage::FromXmlElement(tinyxml2::XMLElement* ele, wstring& syntax_from)
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
                    SyntaxColor syntax_color;
                    syntax_color.name = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(syntax_node, "name"), CodeType::UTF8_NO_BOM);
                    syntax_color.id = atoi(CTinyXml2Helper::ElementAttribute(syntax_node, "id"));
                    sscanf_s(CTinyXml2Helper::ElementAttribute(syntax_node, "color"), "%x", &syntax_color.color);
                    m_syntax_list.push_back(syntax_color);
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
            lan.FromXmlElement(child, syntax_from);
            if (!syntax_from.empty())       //如果语言的语法高亮颜色设置从其他语言复制，则在已读取列表中找到该语言
            {
                CLanguage lan_copy = FindLanguageByName(syntax_from.c_str());
                if (!lan_copy.m_name.empty())
                {
                    lan.m_syntax_list = lan_copy.m_syntax_list;
                }
            }
            m_language_list.push_back(lan);
        });
    }
}

CLanguage CSyntaxHighlight::FindLanguageByExt(const wchar_t* ext)
{
    for (const auto& lan : m_language_list)
    {
        if (lan.m_ext.find(wstring(ext)) != lan.m_ext.end())
        {
            return lan;
        }
    }
    return CLanguage();
}

CLanguage CSyntaxHighlight::FindLanguageByName(const wchar_t* name)
{
    for (const auto& lan : m_language_list)
    {
        if (lan.m_name == name)
        {
            return lan;
        }
    }
    return CLanguage();
}

CLanguage CSyntaxHighlight::GetLanguage(int index)
{
    if (index >= 0 && index < static_cast<int>(m_language_list.size()))
        return m_language_list[index];
    else
        return CLanguage();
}

int CSyntaxHighlight::IndexOf(const wstring& name)
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

const std::vector<CLanguage>& CSyntaxHighlight::GetLanguageList()
{
    return m_language_list;
}
