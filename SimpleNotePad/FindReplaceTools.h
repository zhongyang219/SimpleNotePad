#pragma once
#include "ScintillaEditView.h"


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
    bool match_while_word{};
    bool match_case{};
    bool find_loop{};
    FindMode find_mode{};
};

class FindReplaceTools
{
public:
    static bool FindTexts(FindOption options, bool next, CScintillaEditView* pEditView);
    static void ReplaceTexts(FindOption options, CScintillaEditView* pEditView);
    static int ReplaceAll(FindOption options, CScintillaEditView* pEditView);

private:
    static std::wstring convertExtendedToString(const std::wstring query);
    static unsigned int buildSearchFlags(const FindOption& option);
    static bool readBase(const TCHAR* str, int* value, int base, int size);
};
