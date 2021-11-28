#include "stdafx.h"
#include "UserDefinedLanguageStyle.h"
#include "Common.h"
#include "FilePathHelper.h"

std::wstring CUserLanguage::ExtListToString() const
{
    std::wstring result;
    for (const auto& ext : m_ext)
    {
        result += ext;
        result.push_back(L' ');
    }
    if (!m_ext.empty())
        result.pop_back();
    return result;
}

void CUserLanguage::ExtListFromString(const std::wstring& str)
{
    std::vector<std::wstring> result;
    CCommon::StringSplit(str, L' ', result);
    m_ext.clear();
    for (const auto& ext : result)
    {
        m_ext.insert(ext);
    }
}

bool CUserLanguage::IsStyleExist(int id)
{
    return m_style_map.find(id) != m_style_map.end();
}

CUserLanguage::SyntaxStyle& CUserLanguage::GetStyle(int id)
{
    return m_style_map[id];
}

void CUserLanguage::SetStyle(int id, const SyntaxStyle& style)
{
    m_style_map[id] = style;
}

void CUserLanguage::RemoveStyle(int id)
{
    m_style_map.erase(id);
}

bool CUserDefinedLanguageStyle::IsLanguageExist(const std::wstring& language_name)
{
    return m_language_map.find(language_name) != m_language_map.end();
}

CUserLanguage& CUserDefinedLanguageStyle::GetLanguage(const std::wstring& language_name)
{
    return m_language_map[language_name];
}

const CUserLanguage& CUserDefinedLanguageStyle::GetLanguage(const std::wstring& language_name) const
{
    static CUserLanguage empty_lan;
    auto iter = m_language_map.find(language_name);
    if (iter == m_language_map.end())
        return empty_lan;
    else
        return iter->second;
}

std::wstring CUserDefinedLanguageStyle::FindLanguageByFileName(const std::wstring& file_name) const
{
    CFilePathHelper helper(file_name);
    wstring ext = helper.GetFileExtension();
    for (const auto& lan : m_language_map)
    {
        if (lan.second.m_ext.find(L'$' + file_name) != lan.second.m_ext.end())
        {
            return lan.first;
        }

        if (lan.second.m_ext.find(ext) != lan.second.m_ext.end())
        {
            return lan.first;
        }
    }
    return std::wstring();
}

void CUserDefinedLanguageStyle::Load()
{

}

void CUserDefinedLanguageStyle::Save()
{
}
