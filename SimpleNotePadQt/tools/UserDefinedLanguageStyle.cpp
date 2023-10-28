#include "UserDefinedLanguageStyle.h"
#include "common.h"

#include <QFileInfo>

QString CUserLanguage::ExtListToString() const
{
    QString result;
    for (const auto& ext : m_ext)
    {
        result += ext;
        result.push_back(L' ');
    }
    if (!m_ext.empty())
        result.chop(1);
    return result;
}

void CUserLanguage::ExtListFromString(const QString &str)
{
    QStringList result = str.split(' ');
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
    m_style_map.remove(id);
}

bool CUserDefinedLanguageStyle::IsLanguageExist(const QString &language_name)
{
    return m_language_map.find(language_name) != m_language_map.end();
}

CUserLanguage& CUserDefinedLanguageStyle::GetLanguage(const QString &language_name)
{
    return m_language_map[language_name];
}

const CUserLanguage& CUserDefinedLanguageStyle::GetLanguage(const QString &language_name) const
{
    static CUserLanguage empty_lan;
    auto iter = m_language_map.find(language_name);
    if (iter == m_language_map.end())
        return empty_lan;
    else
        return iter.value();
}

QString CUserDefinedLanguageStyle::FindLanguageByFileName(const QString &file_name) const
{
    QFileInfo helper(file_name);
    QString ext = helper.completeSuffix();
    for (auto iter = m_language_map.begin(); iter != m_language_map.end(); ++iter)
    {
        if (iter.value().m_ext.find(L'$' + file_name) != iter.value().m_ext.end())
        {
            return iter.key();
        }

        if (iter.value().m_ext.find(ext) != iter.value().m_ext.end())
        {
            return iter.key();
        }
    }
    return QString();
}

void CUserDefinedLanguageStyle::RemoveLanguage(const QString &language_name)
{
    auto iter = m_language_map.find(language_name);
    if (iter != m_language_map.end())
        m_language_map.erase(iter);
}

void CUserDefinedLanguageStyle::RemoveAll()
{
    m_language_map.clear();
}
