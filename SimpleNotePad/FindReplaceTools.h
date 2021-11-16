#pragma once
#include "ScintillaEditView.h"
#include <functional>


enum class FindMode
{
    NORMAL,
    EXTENDED,
    REGULAR_EXPRESSION
};

//查找选项
struct FindOption
{
    std::wstring find_str;
    std::wstring replace_str;
    bool match_whole_word{};
    bool match_case{};
    bool find_loop{};
    FindMode find_mode{};
};

class FindReplaceTools
{
public:
    static bool FindTexts(FindOption options, bool next, CScintillaEditView* pEditView);
    static bool ReplaceTexts(FindOption options, CScintillaEditView* pEditView);
    static int ReplaceAll(FindOption options, CScintillaEditView* pEditView);
    static int ReplaceSelection(FindOption options, CScintillaEditView* pEditView);
    static int MarkAll(FindOption options, CScintillaEditView* pEditView);

private:
    static int ReplaceInRange(int start, int end, FindOption options, CScintillaEditView* pEditView);
    
    static std::wstring convertExtendedToString(const std::wstring query);
    static unsigned int buildSearchFlags(const FindOption& option);
    static bool readBase(const TCHAR* str, int* value, int base, int size);
};
