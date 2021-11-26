#include "stdafx.h"
#include "UserDefinedLanguageStyle.h"

bool CUserLanguage::IsStyleExist(int id)
{
    return m_style_map.find(id) != m_style_map.end();
}

CUserLanguage::SyntaxStyle& CUserLanguage::GetStyle(int id)
{
    return m_style_map[id];
}

bool CUserDefinedLanguageStyle::IsLanguageExist(const std::wstring& language_name)
{
    return m_language_map.find(language_name) != m_language_map.end();
}

CUserLanguage& CUserDefinedLanguageStyle::GetLanguage(const std::wstring& language_name)
{
    return m_language_map[language_name];
}

void CUserDefinedLanguageStyle::Load()
{
}

void CUserDefinedLanguageStyle::Save()
{
}
