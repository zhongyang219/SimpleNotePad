#include "SyntaxHighlight.h"
#include "common.h"
#include <algorithm>
#include <QFileInfo>

void CLanguage::FromXmlElement(QDomElement* ele, QString& syntax_from, QString& comment_from)
{
    syntax_from.clear();
    m_name = ele->attribute("name");
    m_id = ele->attribute("lexId").toInt();
    QString ext_list = ele->attribute("ext");
    QStringList ext_vec = ext_list.split(' ');
    for (const auto& ext : ext_vec)
    {
        m_ext.insert(ext);
    }

    //遍历子节点
    Common::IterateXmlElement(ele, [&](QDomElement* child)
    {
        QString node_name = child->tagName();
        if (node_name == "Keywords")
        {
            int keywords_id = child->attribute("id").toInt();
            m_keywords_list[keywords_id] = child->text();
        }
        else if (node_name == "Comment")
        {
            QString str_comment_from = child->attribute("copyFrom");
            if (!str_comment_from.isEmpty())
            {
                comment_from = str_comment_from;
            }
            else
            {
                m_comment.line = child->attribute("commentLine");
                m_comment.start = child->attribute("commentStart");
                m_comment.end = child->attribute("commentEnd");
            }
        }
        else if (node_name == "SyntaxList")
        {
            QString str_syntax_from = child->attribute("copyFrom");
            if (!str_syntax_from.isEmpty())
            {
                syntax_from = str_syntax_from;
            }
            else
            {
                Common::IterateXmlElement(child, [&](QDomElement* syntax_node)
                {
                    SyntaxStyle syntax_style;
                    syntax_style.name = syntax_node->attribute("name");
                    syntax_style.id = syntax_node->attribute("id").toInt();
                    syntax_style.color = syntax_node->attribute("color").toUInt(nullptr, 16);
                    syntax_style.bold = (syntax_node->attribute("bold").toInt() != 0);
                    syntax_style.italic = (syntax_node->attribute("italic").toInt() != 0);
                    m_syntax_list.push_back(syntax_style);
                });
            }
        }
    });
}

void CSyntaxHighlight::LoadFromFile(const QString &file_path)
{
    QFile file(file_path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file, false))
        return;

    QDomElement root = doc.documentElement();
    if (root.isNull())
    {
        return;
    }

    Common::IterateXmlElement(&root, [this](QDomElement* child)
    {
        CLanguage lan;
        QString syntax_from;
        QString comment_from;
        lan.FromXmlElement(child, syntax_from, comment_from);
        if (!syntax_from.isEmpty())       //如果语言的语法高亮颜色设置从其他语言复制，则在已读取列表中找到该语言
        {
            CLanguage lan_copy = FindLanguageByName(syntax_from);
            if (!lan_copy.m_name.isEmpty())
            {
                lan.m_syntax_list = lan_copy.m_syntax_list;
            }
        }
        if (!comment_from.isEmpty())       //如果语言的注释设置从其他语言复制，则在已读取列表中找到该语言
        {
            CLanguage lan_copy = FindLanguageByName(comment_from);
            if (!lan_copy.m_name.isEmpty())
            {
                lan.m_comment = lan_copy.m_comment;
            }
        }
        m_language_list.push_back(lan);
    });

    //将列表按名称排序
    std::sort(m_language_list.begin(), m_language_list.end(), [](const CLanguage& a, const CLanguage& b)
    {
        return a.m_name < b.m_name;
    });
}

const CLanguage& CSyntaxHighlight::FindLanguageByFileName(const QString& file_name) const
{
    QFileInfo helper(file_name);
    QString ext = helper.completeSuffix();
    for (const auto& lan : m_language_list)
    {
        if (lan.m_ext.find('$' + file_name) != lan.m_ext.end())
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

const CLanguage& CSyntaxHighlight::FindLanguageByName(const QString &name) const
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

int CSyntaxHighlight::IndexOf(const QString &name) const
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

const QList<CLanguage> &CSyntaxHighlight::GetLanguageList() const
{
    return m_language_list;
}
