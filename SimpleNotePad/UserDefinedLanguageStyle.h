#pragma once
#include "simplemarshal.h"
using namespace dakuang;
//
class CUserLanguage : public Marshallable
{
public:
    struct SyntaxStyle : public Marshallable
    {
        COLORREF color{};
        bool bold{};
        bool italic{};

        // 通过 Marshallable 继承
        virtual void marshal(SimplePack&) const override;
        virtual void unmarshal(const SimpleUnpack&) override;
    };

    std::set<std::wstring> m_ext;   //扩展名列表

    std::wstring ExtListToString() const;    //将扩展名列表转换成字符串
    void ExtListFromString(const std::wstring& str);

    bool IsStyleExist(int id);
    SyntaxStyle& GetStyle(int id);
    void SetStyle(int id, const SyntaxStyle& style);
    void RemoveStyle(int id);

    // 通过 Marshallable 继承
    virtual void marshal(SimplePack&) const override;
    virtual void unmarshal(const SimpleUnpack&) override;

private:
    std::map<int, SyntaxStyle> m_style_map;     //保存一种语言的用户自定义列表。key: 样式的序号，value: 用户自定义样式
};

//所有用户自定义的语言格式
class CUserDefinedLanguageStyle : public Marshallable
{
public:
    bool IsLanguageExist(const std::wstring& language_name);
    CUserLanguage& GetLanguage(const std::wstring& language_name);
    const CUserLanguage& GetLanguage(const std::wstring& language_name) const;
    std::wstring FindLanguageByFileName(const std::wstring& file_name) const;

    //void Load();
    //void Save();

    // 通过 Marshallable 继承
    virtual void marshal(SimplePack&) const override;
    virtual void unmarshal(const SimpleUnpack&) override;

private:
    std::map<std::wstring, CUserLanguage> m_language_map;   //保存所有语言的用户自定义列表。key: 语言的名称，vlaue: 该语言的用户自定义列表
};

