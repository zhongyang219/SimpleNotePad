#include "stdafx.h"
#include "EditorHelper.h"

const std::vector<std::pair<char, char>> matched_characters{ {'{', '}'},{'[', ']'},{'(', ')'}, {'\"', '\"'},{'\'', '\''} };
const std::vector<std::pair<char, char>> matched_brackets{ {'{', '}'},{'[', ']'},{'(', ')'} };

void CEditorHelper::MarkMatchedBrackets()
{
    m_view->ClearAllMark(CScintillaEditView::MarkStyle::MATCHED_BRACKETS);
    int pos = m_view->GetCurrentIndex();
    int length = m_view->GetDocLength();
    for (const auto& item : matched_brackets)
    {
        int left_bracket_pos{ -1 };
        //当前是左括号
        if (m_view->At(pos) == item.first)
            left_bracket_pos = pos;
        if (m_view->At(pos - 1) == item.first)
            left_bracket_pos = pos - 1;
        if (left_bracket_pos >= 0)
        {
            int left_bracket_count{};
            //向后查找右括号
            for (int i = left_bracket_pos + 1; i < length; i++)
            {
                char ch = m_view->At(i);
                if (ch == item.first)
                    left_bracket_count++;
                if (ch == item.second)
                {
                    if (left_bracket_count == 0)
                    {
                        //找到右括号，将括号对标记
                        m_view->SetMark(CScintillaEditView::MarkStyle::MATCHED_BRACKETS, left_bracket_pos, 1);
                        m_view->SetMark(CScintillaEditView::MarkStyle::MATCHED_BRACKETS, i, 1);
                        break;
                    }
                    left_bracket_count--;
                }
            }
        }

        int right_bracket_pos{ -1 };
        //当前是右括号
        if (m_view->At(pos) == item.second)
            right_bracket_pos = pos;
        if (m_view->At(pos - 1) == item.second)
            right_bracket_pos = pos - 1;
        if (right_bracket_pos >= 0)
        {
            int right_bracket_count{};
            //向前查找左括号
            for (int i = right_bracket_pos - 1; i >= 0; i--)
            {
                char ch = m_view->At(i);
                if (ch == item.second)
                    right_bracket_count++;

                if (ch == item.first)
                {
                    if (right_bracket_count == 0)
                    {
                        //找到左括号，将括号对标记
                        m_view->SetMark(CScintillaEditView::MarkStyle::MATCHED_BRACKETS, right_bracket_pos, 1);
                        m_view->SetMark(CScintillaEditView::MarkStyle::MATCHED_BRACKETS, i, 1);
                        break;
                    }
                    right_bracket_count--;
                }
            }
        }
    }
}

void CEditorHelper::BracketsAutoComp(char left_bracket)
{
    //如果键入了一个括号对的左半部分，则自动插入括号对的右半部分
    int pos = m_view->GetCurrentIndex();
    int start{}, end{};
    m_view->GetCurLinePos(start, end);
    //只有当光标所在的位置为一行的末尾时才进行此操作
    if (pos == end)
    {
        for (const auto& item : matched_characters)
        {
            if (left_bracket == item.first)
            {
                m_view->Paste(std::string(1, item.second));
                m_view->SendMessage(SCI_GOTOPOS, pos);  //光标移动到插入前的位置
            }
        }
    }
}

void CEditorHelper::BracketsAutoDelete(char ch)
{
    int pos = m_view->GetCurrentIndex();
    int start{}, end{};
    m_view->GetCurLinePos(start, end);
    //确保删除的括号对在一行的末尾
    if (pos == end - 1)
    {
        //如果删除的字符是括号对的左半边，则自动删除括号对的右半边
        for (const auto& item : matched_characters)
        {
            if (ch == item.first)
            {
                char next_ch = m_view->At(pos);
                if (next_ch == item.second)
                    //m_view->DeleteText(pos, 1);
                    AfxGetMainWnd()->PostMessage(WM_DELETE_CHAR, pos);   //在OnNotify函数里调用m_view->DeleteText(pos, 1)无效，因此这里通过PostMessage来确保在OnNotify返回后再调用
            }
        }
    }
}
