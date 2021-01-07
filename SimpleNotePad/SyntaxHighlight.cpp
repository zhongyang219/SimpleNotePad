#include "stdafx.h"
#include "SyntaxHighlight.h"
#include "TinyXml2Helper.h"
#include "Common.h"

void CLanguage::FromXmlElement(tinyxml2::XMLElement* ele)
{
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
        string node_name = child->Name();
        if (node_name == "Keywords")
        {
            int keywords_id = atoi(CTinyXml2Helper::ElementAttribute(child, "id"));
            m_keywords_list[keywords_id] = child->GetText();
        }
        else if (node_name == "SyntaxList")
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
    });
}

void CSyntaxHighlight::LoadFromFile(const char* file_path)
{
    tinyxml2::XMLDocument doc;
    auto err = doc.LoadFile(file_path);
    if (!err)
    {
        CTinyXml2Helper::IterateChildNode(doc.FirstChildElement(), [this](tinyxml2::XMLElement* child)
        {
            CLanguage lan;
            lan.FromXmlElement(child);
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

const std::vector<CLanguage>& CSyntaxHighlight::GetLanguageList()
{
    return m_language_list;
}
