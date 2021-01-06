#include "stdafx.h"
#include "SyntaxHighlight.h"
#include "TinyXml2Helper.h"
#include "Common.h"

void CLanguage::FromXmlElement(tinyxml2::XMLElement* ele)
{
    m_mane = CCommon::StrToUnicode(ele->Attribute("name"), CodeType::UTF8_NO_BOM);
    wstring ext_list = CCommon::StrToUnicode(ele->Attribute("ext"), CodeType::UTF8_NO_BOM);
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
            int keywords_id = atoi(child->Attribute("id"));
            m_keywords_list[keywords_id] = child->GetText();
        }
        else if (node_name == "SyntaxList")
        {
            CTinyXml2Helper::IterateChildNode(child, [&](tinyxml2::XMLElement* syntax_node)
            {
                SyntaxColor syntax_color;
                syntax_color.name = CCommon::StrToUnicode(syntax_node->Attribute("name"), CodeType::UTF8_NO_BOM);
                syntax_color.id = atoi(syntax_node->Attribute("id"));
                sscanf_s(syntax_node->Attribute("color"), "%x", &syntax_color.color);
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
