#pragma once
#include "CVariant.h"
#include "CommonData.h"
//#define MAX_STR_SIZE 1024;
using std::wstring;
using std::string;

#define CODE_PAGE_CHS 936
#define CODE_PAGE_CHT 950
#define CODE_PAGE_JP 932
#define CODE_PAGE_EN 1252
#define CODE_PAGE_KOR 949
#define CODE_PAGE_THAI 874
#define CODE_PAGE_VIET 1258

#define CODE_PAGE_DEFAULT (UINT32_MAX)

enum class CodeType
{
    ANSI,
    UTF8,
    UTF8_NO_BOM,
    UTF16,
    AUTO
};

struct CodeTypeItem
{
    CString name;
    CodeType code_type{};
    UINT code_page{};
};

struct CodePageItem
{
    CString name;
    UINT code_page{};
};


namespace CONST_VAL
{
    const vector<CodeTypeItem> code_list{ {_T("ANSI (本地代码页)"), CodeType::ANSI, CP_ACP},
        {_T("UTF-8"), CodeType::UTF8, CP_ACP},
        {_T("UTF-8无BOM"), CodeType::UTF8_NO_BOM, CP_ACP},
        {_T("UTF-16"), CodeType::UTF16, CP_ACP},
        {_T("简体中文 (GB2312)"), CodeType::ANSI, CODE_PAGE_CHS},
        {_T("繁体中文 (Big5)"), CodeType::ANSI, CODE_PAGE_CHT},
        {_T("日文 (Shift-JIS)"), CodeType::ANSI, CODE_PAGE_JP},
        {_T("西欧语言 (Windows)"), CodeType::ANSI, CODE_PAGE_EN},
        {_T("韩文"), CodeType::ANSI, CODE_PAGE_KOR},
        {_T("泰文"), CodeType::ANSI, CODE_PAGE_THAI},
        {_T("越南文"), CodeType::ANSI, CODE_PAGE_VIET},
        {_T("设置中指定的非Unicode默认代码页"), CodeType::ANSI, CODE_PAGE_DEFAULT},
    };

    const vector<CodePageItem> code_page_list{
        {_T("本地代码页"), CP_ACP},
        {_T("简体中文 (GB2312)"), CODE_PAGE_CHS},
        {_T("繁体中文 (Big5)"), CODE_PAGE_CHT},
        {_T("日文 (Shift-JIS)"), CODE_PAGE_JP},
        {_T("西欧语言 (Windows)"), CODE_PAGE_EN},
        {_T("韩文"), CODE_PAGE_KOR},
        {_T("泰文"), CODE_PAGE_THAI},
        {_T("越南文"), CODE_PAGE_VIET},
        {_T("手动指定代码页"), 0}
    };

}


class CCommon
{
public:
	CCommon();
	~CCommon();

	//将string类型的字符串转换成Unicode编码的wstring字符串，code_type指定转换方式，code_page指定当code_type为ANSI时的代码页
	static wstring StrToUnicode(const string& str, CodeType code_type = CodeType::ANSI, UINT code_page = CP_ACP);

	//将Unicode编码的wstring字符串转换成string类型的字符串(如果至少有一个字符无法转换，则将char_cannot_convert置为true)
	static string UnicodeToStr(const wstring& wstr, bool& char_cannot_convert, CodeType code_type = CodeType::ANSI, UINT code_page = CP_ACP);

	//判断字符串是否UTF8编码
	static bool IsUTF8Bytes(const char* data);

    //字符大小写转换
    static void ConvertCharCase(wchar_t& ch, bool upper);

    //判断字符是否是一个字母
    static bool IsLetter(wchar_t ch);

	static void WritePrivateProfileInt(const wchar_t* AppName, const wchar_t* KeyName, int value, const wchar_t* Path);

	//static bool FileExist(const wchar_t* file);

	//获取当前exe文件的路径（不含文件名）
	static wstring GetCurrentPath();

	//调整CListBox控件的宽度
	static void AdjustListWidth(CListBox & list);

	//修改字符串的长度，如果新的长度比原来的短，则保留右边的字符，如果新的长度比原来的长，则在左边添加指定的字符
	static void ChangeStringLength(CString& str, int length, TCHAR ch);

	//向一个Edit box添加字符串
	static void EditAppendString(const CString& str, HWND hWnd);

	//判断一个字符是不是合法的标识符
	static bool IsValidNameChar(wchar_t ch);

	//转换字符串大小写，如果upper为true，则转换成大写，否则转换成小写
	static bool StringTransform(wstring& str, bool upper);

	//字符串查找，忽略大小写（find_down：是否向后查找）
	static size_t StringFindNoCase(const wstring& str, const wstring& find_str, bool find_down, size_t offset);

	//字符串查找，全词匹配（no_case：是否忽略大小写）
	static size_t StringFindWholeWord(const wstring& str, const wstring& find_str, bool no_case, bool find_down, size_t offset);

	//判断一个字符是否是在全词匹配时的单词分割字符（除了字母、数字和256以上的Unicode字符外的字符）
	static bool IsDivideChar(wchar_t ch);

	static size_t StringFind(const wstring& str, const wstring& find_str, bool no_case, bool whole_word, bool find_down, size_t offset);

    //将字号转成LOGFONT结构中的lfHeight
    static int FontSizeToLfHeight(int font_size);

    //将一个字符串分割成若干个字符串
    //str: 原始字符串
    //div_ch: 用于分割的字符
    //result: 接收分割后的结果
    static void StringSplit(const wstring& str, wchar_t div_ch, vector<wstring>& results, bool skip_empty = true);

    //读取文件内容
    static bool GetFileContent(const wchar_t* file_path, string& contents_buff, bool binary = true);

    //读取文件内容
    //file_path: 文件的路径
    //length: 文件的长度
    //binary: 是否以进制方式读取
    //返回值: 读取成功返回读取到的文件内容的const char类型的指针，在使用完毕后这个指针需要自行使用delete释放。读取失败返回nullptr
    static const char* GetFileContent(const wchar_t* file_path, size_t& length, bool binary = true);

    //获取当前进程exe文件所在目录
    static wstring GetExePath();

    //获取URL的内容
    static bool GetURL(const wstring& url, wstring& result, bool utf8 = false, const wstring& user_agent = wstring());

    //从资源文件载入字符串。其中，front_str、back_str为载入字符串时需要在前面或后面添加的字符串
    static CString LoadText(UINT id, LPCTSTR back_str = nullptr);
    static CString LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str = nullptr);

    //安全的格式化字符串，将format_str中形如<%序号%>的字符串替换成初始化列表paras中的元素，元素支持int/double/LPCTSTR/CString格式，序号从1开始
    static CString StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras);

    //从资源文件中载入字符串，并将资源字符串中形如<%序号%>的字符串替换成可变参数列表中的参数
    static CString LoadTextFormat(UINT id, const std::initializer_list<CVariant>& paras);

    //设置线程语言
    static void SetThreadLanguage(Language language);

};
