#pragma once
#include <functional>
#include "scintillaeditor.h"


enum class FindMode
{
    NORMAL,
    EXTENDED,
    REGULAR_EXPRESSION
};

//查找选项
struct FindOption
{
    QString find_str;
    QString replace_str;
    bool match_whole_word{};
    bool match_case{};
    bool find_loop{};
    FindMode find_mode{};
};

class FindReplaceTools
{
public:
    static bool FindTexts(FindOption options, bool next, ScintillaEditor* pEditView);
    static bool ReplaceTexts(FindOption options, ScintillaEditor* pEditView);
    static int ReplaceAll(FindOption options, ScintillaEditor* pEditView);
    static int ReplaceSelection(FindOption options, ScintillaEditor* pEditView);
    static int MarkAll(FindOption options, ScintillaEditor* pEditView);
    static bool MarkSameWord(const QString& str, ScintillaEditor::MarkStyle mark_style, ScintillaEditor* pEditView);

private:
    static int ReplaceInRange(int start, int end, FindOption options, ScintillaEditor* pEditView);

    static QString convertExtendedToString(const QString query);
    static unsigned int buildSearchFlags(const FindOption& option);
    static bool readBase(const char* str, int* value, int base, int size);
};
