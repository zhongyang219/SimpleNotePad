#pragma once
#include "ScintillaEditView.h"
class CEditorHelper
{
public:
    CEditorHelper(CScintillaEditView* edit_view)
        : m_view(edit_view)
    {}

    //标记匹配的括号
    void MarkMatchedBrackets();

    //括号自动完成
    void BracketsAutoComp(char left_bracket);

    //删除左括号时自动删除括号对
    void BracketsAutoDelete(char ch);

private:
    CScintillaEditView* m_view{};
};
