#pragma once
#include "stdafx.h"
#include "tinyxml2.h"

class CLanguage
{
public:
    struct SyntaxColor
    {
        std::wstring name;
        int id;
        COLORREF color;
    };
    std::wstring m_name;
    int m_id;
    std::set<wstring> m_ext;
    std::map<int, std::string> m_keywords_list;
    std::vector<SyntaxColor> m_syntax_list;

    void FromXmlElement(tinyxml2::XMLElement* ele);
};

class CSyntaxHighlight
{
public:
    void LoadFromFile(const char* file_path);

    CLanguage FindLanguageByExt(const wchar_t* ext);
    CLanguage FindLanguageByName(const wchar_t* name);
    const std::vector<CLanguage>& GetLanguageList();

private:
    std::vector<CLanguage> m_language_list;
};

