#include "stdafx.h"
#include "FindReplaceTools.h"
#include "Common.h"


bool FindReplaceTools::FindTexts(FindOption options, bool next, CScintillaEditView* pEditView)
{
    if (options.find_mode == FindMode::EXTENDED)
    {
        options.find_str = FindReplaceTools::convertExtendedToString(options.find_str);
    }

    UINT flags{ FindReplaceTools::buildSearchFlags(options) };
    static int find_result{};       //查找到的位置
    static bool last_next{ !next }; //上一次是查找一下个还是查找下一个
    Sci_TextToFind ttf;
    if (options.find_loop && find_result < 0)  //如果是循环查找，在找不到时重置查找
    {
        if (next)       //向后查找
        {
            ttf.chrg.cpMin = 0;
            ttf.chrg.cpMax = pEditView->SendMessage(SCI_GETLENGTH, 0, 0);
        }
        else            //向前查找
        {
            ttf.chrg.cpMin = pEditView->SendMessage(SCI_GETLENGTH, 0, 0);
            ttf.chrg.cpMax = 0;
        }
    }
    else
    {
        if (find_result < 0 && last_next == next)    //非循环查找，如果找不到则不再查找
            return false;
        //设置查找位置
        if (next)       //向后查找
        {
            ttf.chrg.cpMin = pEditView->SendMessage(SCI_GETCURRENTPOS);
            ttf.chrg.cpMax = pEditView->SendMessage(SCI_GETLENGTH);
        }
        else            //向前查找
        {
            ttf.chrg.cpMin = pEditView->SendMessage(SCI_GETANCHOR);
            ttf.chrg.cpMax = 0;
        }
    }
    bool char_cannot_convert{};
    std::string find_str = CCommon::UnicodeToStr(options.find_str, char_cannot_convert, CodeType::UTF8_NO_BOM);
    ttf.lpstrText = find_str.c_str();
    //查找文本
    find_result = pEditView->SendMessage(SCI_FINDTEXT, flags, (LPARAM)&ttf);
    //选中找到的文本
    pEditView->SetSelByBytes(find_result, find_result + find_str.size());

    last_next = next;
    return find_result >= 0;
}


bool FindReplaceTools::ReplaceTexts(FindOption options, CScintillaEditView* pEditView)
{
    if (pEditView->IsSelectionEmpty())
        return false;
    if (options.find_mode == FindMode::EXTENDED)
    {
        options.replace_str = FindReplaceTools::convertExtendedToString(options.replace_str);
        options.find_str = FindReplaceTools::convertExtendedToString(options.find_str);
    }
    if (pEditView->GetSelectedText() != options.find_str)
        return false;
    int start = pEditView->SendMessage(SCI_GETANCHOR);
    int end = pEditView->SendMessage(SCI_GETCURRENTPOS);
    pEditView->SendMessage(SCI_SETTARGETSTART, start);
    pEditView->SendMessage(SCI_SETTARGETEND, end);
    bool char_cannot_convert{};
    std::string replace_str = CCommon::UnicodeToStr(options.replace_str, char_cannot_convert, CodeType::UTF8_NO_BOM);
    UINT replace_msg = (options.find_mode == FindMode::REGULAR_EXPRESSION ? SCI_REPLACETARGETRE : SCI_REPLACETARGET);
    pEditView->SendMessage(replace_msg, replace_str.size(), (sptr_t)replace_str.c_str());
    pEditView->SendMessage(SCI_SETSEL, start, start + replace_str.size());
    return true;
}

int FindReplaceTools::ReplaceAll(FindOption options, CScintillaEditView* pEditView)
{
    return ReplaceInRange(0, pEditView->SendMessage(SCI_GETLENGTH), options, pEditView);
}

int FindReplaceTools::ReplaceSelection(FindOption options, CScintillaEditView* pEditView)
{
    int start = pEditView->SendMessage(SCI_GETANCHOR);
    int end = pEditView->SendMessage(SCI_GETCURRENTPOS);
    if (start != end)
        return ReplaceInRange(start, end, options, pEditView);
    return 0;
}

int FindReplaceTools::MarkAll(FindOption options, CScintillaEditView* pEditView)
{
    if (options.find_str.empty() || pEditView->SendMessage(SCI_GETLENGTH) <= 0)
        return false;
    int count{};
    if (options.find_mode == FindMode::EXTENDED)
    {
        options.find_str = FindReplaceTools::convertExtendedToString(options.find_str);
    }
    bool char_cannot_convert{};
    std::string find_str = CCommon::UnicodeToStr(options.find_str, char_cannot_convert, CodeType::UTF8_NO_BOM);

    UINT flags{ FindReplaceTools::buildSearchFlags(options) };
    Sci_TextToFind ttf;
    ttf.chrg.cpMin = 0;
    ttf.chrg.cpMax = pEditView->SendMessage(SCI_GETLENGTH);
    ttf.lpstrText = find_str.c_str();
    while (true)
    {
        //查找
        int find_result = pEditView->SendMessage(SCI_FINDTEXT, flags, (LPARAM)&ttf);
        if (find_result < 0)
            break;
        ttf.chrg.cpMin = find_result + find_str.size();
        //标记
        pEditView->SetMark(CScintillaEditView::MarkStyle::MARK_ALL, find_result, find_str.size());
        count++;
    }
    pEditView->SetEditChangeNotificationEnable(true);
    return count;
}

void FindReplaceTools::MarkSameWord(const std::wstring& str, CScintillaEditView::MarkStyle mark_style, CScintillaEditView* pEditView)
{
    bool char_cannot_convert{};
    std::string find_str = CCommon::UnicodeToStr(str, char_cannot_convert, CodeType::UTF8_NO_BOM);
    MarkSameWord(find_str, mark_style, pEditView);
}

void FindReplaceTools::MarkSameWord(const std::string& str, CScintillaEditView::MarkStyle mark_style, CScintillaEditView* pEditView)
{
    if (str.empty() || pEditView->SendMessage(SCI_GETLENGTH) <= 0)
        return;

    Sci_TextToFind ttf;
    ttf.chrg.cpMin = 0;
    ttf.chrg.cpMax = pEditView->SendMessage(SCI_GETLENGTH);
    ttf.lpstrText = str.c_str();
    int mark_count{};   //统计匹配次数
    int last_find_result{}; //上次匹配的位置
    while (true)
    {
        //查找
        int find_result = pEditView->SendMessage(SCI_FINDTEXT, SCFIND_WHOLEWORD, (LPARAM)&ttf);
        if (find_result < 0)
            break;
        ttf.chrg.cpMin = find_result + str.size();
        //标记
        pEditView->SetMark(CScintillaEditView::MarkStyle::SELECTION_MARK, find_result, str.size());
        last_find_result = find_result;
        mark_count++;
    }
    //如果只匹配到1次，则清除标记
    if (mark_count == 1)
        pEditView->ClearMark(CScintillaEditView::MarkStyle::SELECTION_MARK, last_find_result, str.size());
}

int FindReplaceTools::ReplaceInRange(int start, int end, FindOption options, CScintillaEditView* pEditView)
{
    if (options.find_str.empty() || pEditView->SendMessage(SCI_GETLENGTH) <= 0)
        return false;
    pEditView->SetEditChangeNotificationEnable(false);
    int replaced_count{};
    if (options.find_mode == FindMode::EXTENDED)
    {
        options.find_str = FindReplaceTools::convertExtendedToString(options.find_str);
        options.replace_str = FindReplaceTools::convertExtendedToString(options.replace_str);
    }
    bool char_cannot_convert{};
    std::string find_str = CCommon::UnicodeToStr(options.find_str, char_cannot_convert, CodeType::UTF8_NO_BOM);
    std::string replace_str = CCommon::UnicodeToStr(options.replace_str, char_cannot_convert, CodeType::UTF8_NO_BOM);

    UINT flags{ FindReplaceTools::buildSearchFlags(options) };
    Sci_TextToFind ttf;
    ttf.chrg.cpMin = start;
    ttf.chrg.cpMax = end;
    ttf.lpstrText = find_str.c_str();
    while (true)
    {
        //查找
        int find_result = pEditView->SendMessage(SCI_FINDTEXT, flags, (LPARAM)&ttf);
        if (find_result < 0)
            break;
        ttf.chrg.cpMin = find_result + find_str.size();
        //替换
        pEditView->SendMessage(SCI_SETTARGETSTART, find_result);
        pEditView->SendMessage(SCI_SETTARGETEND, find_result + find_str.size());
        UINT replace_msg = (options.find_mode == FindMode::REGULAR_EXPRESSION ? SCI_REPLACETARGETRE : SCI_REPLACETARGET);
        pEditView->SendMessage(replace_msg, replace_str.size(), (sptr_t)replace_str.c_str());
        replaced_count++;
    }
    pEditView->SetEditChangeNotificationEnable(true);
    return replaced_count;
}

std::wstring FindReplaceTools::convertExtendedToString(const std::wstring query)
{
    std::wstring result;
    result.resize(query.size() + 1);
    int length = query.size();
    //query may equal to result, since it always gets smaller
    int i = 0, j = 0;
    int charLeft = length;
    TCHAR current;
    while (i < length)
    {	//because the backslash escape quences always reduce the size of the generic_string, no overflow checks have to be made for target, assuming parameters are correct
        current = query[i];
        --charLeft;
        if (current == '\\' && charLeft)
        {	//possible escape sequence
            ++i;
            --charLeft;
            current = query[i];
            switch (current)
            {
            case 'r':
                result[j] = '\r';
                break;
            case 'n':
                result[j] = '\n';
                break;
            case '0':
                result[j] = '\0';
                break;
            case 't':
                result[j] = '\t';
                break;
            case '\\':
                result[j] = '\\';
                break;
            case 'b':
            case 'd':
            case 'o':
            case 'x':
            case 'u':
            {
                int size = 0, base = 0;
                if (current == 'b')
                {	//11111111
                    size = 8, base = 2;
                }
                else if (current == 'o')
                {	//377
                    size = 3, base = 8;
                }
                else if (current == 'd')
                {	//255
                    size = 3, base = 10;
                }
                else if (current == 'x')
                {	//0xFF
                    size = 2, base = 16;
                }
                else if (current == 'u')
                {	//0xCDCD
                    size = 4, base = 16;
                }

                if (charLeft >= size)
                {
                    int res = 0;
                    if (FindReplaceTools::readBase(query.c_str() + (i + 1), &res, base, size))
                    {
                        result[j] = static_cast<TCHAR>(res);
                        i += size;
                        break;
                    }
                }
                //not enough chars to make parameter, use default method as fallback
            }

            default:
            {	//unknown sequence, treat as regular text
                result[j] = '\\';
                ++j;
                result[j] = current;
                break;
            }
            }
        }
        else
        {
            result[j] = query[i];
        }
        ++i;
        ++j;
    }
    result.resize(j);
    return result;
}

unsigned int FindReplaceTools::buildSearchFlags(const FindOption& option)
{
    return	(option.match_whole_word ? SCFIND_WHOLEWORD : 0) |
        (option.match_case ? SCFIND_MATCHCASE : 0) |
        (option.find_mode == FindMode::REGULAR_EXPRESSION ? SCFIND_REGEXP | SCFIND_POSIX : 0);
};

bool FindReplaceTools::readBase(const TCHAR* str, int* value, int base, int size)
{
    int i = 0, temp = 0;
    *value = 0;
    TCHAR max = '0' + static_cast<TCHAR>(base) - 1;
    TCHAR current;
    while (i < size)
    {
        current = str[i];
        if (current >= 'A')
        {
            current &= 0xdf;
            current -= ('A' - '0' - 10);
        }
        else if (current > '9')
            return false;

        if (current >= '0' && current <= max)
        {
            temp *= base;
            temp += (current - '0');
        }
        else
        {
            return false;
        }
        ++i;
    }
    *value = temp;
    return true;
}
