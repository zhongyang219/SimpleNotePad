#pragma once

#include <QDomElement>
#include <QMap>
#include <QSet>
#include <QString>

class CLanguage
{
public:
    struct SyntaxStyle
    {
        QString name;
        int id{};
        unsigned int color;
        bool bold{};
        bool italic{};
    };
    QString m_name;
    int m_id{};
    QSet<QString> m_ext;
    QMap<int, QString> m_keywords_list;
    QList<SyntaxStyle> m_syntax_list;

    struct Comment
    {
        QString line;
        QString start;
        QString end;

        bool isMultiLeneCommentValid() const
        {
            return !start.isEmpty() && !end.isEmpty();
        }
    };
    Comment m_comment;

    //从配置文件“Language”节点解析语法高亮信息
    //ele: xml节点
    //syntax_from: 如果语法高亮颜色设置要从其他语言复制，则将该语言的名称保存在这里
    //comment_from: 如果注释设置要从其他语言复制，则将该语言的名称保存在这里
    void FromXmlElement(QDomElement* ele, QString& syntax_from, QString& comment_from);
};

class CSyntaxHighlight
{
public:
    void LoadFromFile(const QString& file_path);

    const CLanguage& FindLanguageByFileName(const QString& file_name) const;
    const CLanguage& FindLanguageByName(const QString& name) const;
    const CLanguage& GetLanguage(int index) const;
    int IndexOf(const QString& name) const;

    const QList<CLanguage>& GetLanguageList() const;

private:
    QList<CLanguage> m_language_list;
    CLanguage m_empty_language{};
};
