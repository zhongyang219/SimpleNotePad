#pragma once


class CUserLanguage
{
public:
    struct SyntaxStyle
    {
        COLORREF color{};
        bool bold{};
        bool italic{};
    };

    std::set<std::wstring> m_ext;

    bool IsStyleExist(int id);
    SyntaxStyle& GetStyle(int id);

private:
    std::map<int, SyntaxStyle> m_style_map;
};

class CUserDefinedLanguageStyle
{
public:
    bool IsLanguageExist(const std::wstring& language_name);
    CUserLanguage& GetLanguage(const std::wstring& language_name);

    void Load();
    void Save();

private:
    std::map<std::wstring, CUserLanguage> m_language_map;
};

