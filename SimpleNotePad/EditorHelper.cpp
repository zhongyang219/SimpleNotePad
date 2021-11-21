#include "stdafx.h"
#include "EditorHelper.h"
#include "xmlMatchedTagsHighlighter/xmlMatchedTagsHighlighter.h"
#include "Common.h"

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
                m_view->GotoPos(pos);  //光标移动到插入前的位置
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

void CEditorHelper::AddOrRemoveComment(const CLanguage::Comment& comment)
{
    CScintillaEditView::UndoRedoActionLocker locker(m_view->GetSafeHwnd());

    //添加/删除单行注释（使用单行注释的条件是当前语言有单行注释，并且编辑器没有选中内容）
    if (!comment.line.empty() && m_view->IsSelectionEmpty())
    {
        int start{}, end{};
        m_view->GetCurLinePos(start, end);
        //查找单行注释
        int comment_pos = m_view->Find(comment.line, start, end);
        if (comment_pos < 0)    //单行注释不存在，添加单行注释
        {
            m_view->InserText(comment.line, start);
        }
        else    //单行注释存在，删除单行注释
        {
            m_view->DeleteText(comment_pos, comment.line.size());
        }
    }
    //如果选中的是整行，则对每行使用单行注释
    else if (!comment.line.empty() && m_view->IsFullLineSelected())
    {
        int first_line{}, last_line{};
        m_view->GetLineSelected(first_line, last_line);
        //判断应该对这此行添加还是删除单行注释
        bool comment_exist{ true };
        //在每一行中查找单行注释，如果每行都有单行注释，则删除注释，否则添加注释
        for (int i{ first_line }; i < last_line; i++)
        {
            int start{}, end{};
            m_view->GetLinePos(i, start, end);
            int comment_pos = m_view->Find(comment.line, start, end);
            if (comment_pos < 0)
                comment_exist = false;
        }
        //对每一行执行添加或删除注释
        for (int i{ first_line }; i < last_line; i++)
        {
            int start{}, end{};
            m_view->GetLinePos(i, start, end);
            if (comment_exist)
            {
                //删除注释
                int comment_pos = m_view->Find(comment.line, start, end);
                if (comment_pos >= 0)
                {
                    m_view->DeleteText(comment_pos, comment.line.size());
                }
            }
            else
            {
                //添加注释
                int start{}, end{};
                m_view->GetLinePos(i, start, end);
                m_view->InserText(comment.line, start);
            }
        }
    }
    //添加/删除多行注释
    else if (comment.isMultiLeneCommentValid())
    {
        int start{}, end{};
        m_view->GetSel(start, end);
        if (start == end)
            m_view->GetCurLinePos(start, end);
        //查找多行注释
        bool comment_exist{ false };
        int comment_start = m_view->Find(comment.start, start, end);
        if (comment_start >= 0)
        {
            comment_exist = true;
            m_view->DeleteText(comment_start, comment.start.size());
        }
        int comment_end = m_view->Find(comment.end, end, start);
        if (comment_end >= 0)
        {
            comment_exist = true;
            m_view->DeleteText(comment_end, comment.end.size());
        }
        //多行注释不存在，添加注释
        if (!comment_exist)
        {
            m_view->InserText(comment.start, start);
            m_view->InserText(comment.end, end + comment.start.size()); //由于上一步已经插入了多行注释的开头，所以需要加上comment.start.size()以修正插入的位置
        }
    }
}

void CEditorHelper::HtmlMarkAutoComp(char character_input)
{
    //当用户输入了“>”时
    if (character_input == '>')
    {
        //查找左侧HTML标记
        int cur_pos = m_view->GetCurrentIndex();
        int left_bracket_pos = m_view->Find("<", cur_pos, 0);
        if (left_bracket_pos >= 0)
        {
            std::string str_mark;
            int mark_end_pos = m_view->Find(" ", left_bracket_pos + 1, cur_pos - 1);
            if (mark_end_pos > 0)
                str_mark = m_view->GetText(left_bracket_pos + 1, mark_end_pos);
            else
                str_mark = m_view->GetText(left_bracket_pos + 1, cur_pos - 1);
            
            //生成右侧HTML标记
            std::string str_right_mark{"</"};
            str_right_mark += str_mark;
            str_right_mark.push_back('>');

            //插入右侧html标记
            m_view->Paste(str_right_mark);
            //光标移动到插入前的位置
            m_view->GotoPos(cur_pos);
        }
    }
}

void CEditorHelper::MarkMatchedHtmlMarks()
{
    //m_view->ClearAllMark(CScintillaEditView::MarkStyle::HTML_MARKS);
    XmlMatchedTagsHighlighter highter(m_view);
    highter.tagMatch(false);

}

void CEditorHelper::AutoShowCompList(const CLanguage& languange)
{
    int pos = m_view->GetCurrentIndex(); //取得当前位置 
    int startpos = m_view->SendMessage(SCI_WORDSTARTPOSITION, pos - 1);//当前单词起始位置 
    int endpos = m_view->SendMessage(SCI_WORDENDPOSITION, pos - 1);//当前单词终止位置 
    std::string word = m_view->GetText(startpos, endpos);
    string matched_list = GetMatchedCompList(languange, word);
    if (!matched_list.empty() && !word.empty())
    {
        m_view->SendMessage(SCI_AUTOCSHOW, word.size(), sptr_t(matched_list.c_str()));
    }
}

string CEditorHelper::GetMatchedCompList(const CLanguage& languange, const std::string& str)
{
    //拆分所有关键字
    std::set<std::string> keywords_list;
    for (const auto& keywords : languange.m_keywords_list)
    {
        CCommon::StringSplit(keywords.second, std::string(1, ' '), [&](const std::string& str)
            {
                keywords_list.insert(str);
            });
    }
    //查找匹配列表
    std::string result;
    for (const auto& keyword : keywords_list)
    {
        if (keyword.substr(0, str.size()) == str)
        {
            result += keyword;
            result.push_back(' ');
        }
    }
    if (!result.empty())
        result.pop_back();
    return result;
}
