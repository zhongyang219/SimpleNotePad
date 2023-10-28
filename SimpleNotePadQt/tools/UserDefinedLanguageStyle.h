#include <QMap>
#include <QSet>
#include <QString>

#pragma once
//
class CUserLanguage
{
public:
    struct SyntaxStyle
    {
        unsigned int color{};
        bool bold{};
        bool italic{};
    };

    QSet<QString> m_ext;   //扩展名列表

    QString ExtListToString() const;    //将扩展名列表转换成字符串
    void ExtListFromString(const QString& str);

    bool IsStyleExist(int id);
    SyntaxStyle& GetStyle(int id);
    void SetStyle(int id, const SyntaxStyle& style);
    void RemoveStyle(int id);

private:
    QMap<int, SyntaxStyle> m_style_map;     //保存一种语言的用户自定义列表。key: 样式的序号，value: 用户自定义样式
};

//所有用户自定义的语言格式
class CUserDefinedLanguageStyle
{
public:
    bool IsLanguageExist(const QString& language_name);
    CUserLanguage& GetLanguage(const QString& language_name);
    const CUserLanguage& GetLanguage(const QString& language_name) const;
    QString FindLanguageByFileName(const QString& file_name) const;
    void RemoveLanguage(const QString& language_name);
    void RemoveAll();

private:
    QMap<QString, CUserLanguage> m_language_map;   //保存所有语言的用户自定义列表。key: 语言的名称，vlaue: 该语言的用户自定义列表
};

