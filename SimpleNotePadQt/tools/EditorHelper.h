#pragma once
#include "scintillaeditor.h"
#include "SyntaxHighlight.h"

class CEditorHelper
{
public:
    CEditorHelper(ScintillaEditor* edit_view)
        : m_view(edit_view)
    {}

    //标记匹配的括号
    void MarkMatchedBrackets();

    //括号自动完成
    //  left_bracket: 输入的左括号
    void BracketsAutoComp(char left_bracket);

    //删除左括号时自动删除括号对
    //  ch: 删除的左括号
    void BracketsAutoDelete(char ch);

    //自动添加或删除注释
    void AddOrRemoveComment(const CLanguage::Comment& comment);

    //HTML标记自动完成
    //  character_input: 最后输入的字符
    void HtmlMarkAutoComp(char character_input);

    //标记匹配的HTML标签
    void MarkMatchedHtmlMarks();

    //自动显示代码完成列表
    void AutoShowCompList(const CLanguage& languange);

private:
    QString GetMatchedCompList(const CLanguage& languange, const QString& str);

private:
    ScintillaEditor* m_view{};
};
