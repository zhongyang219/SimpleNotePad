#pragma once
//#define MAX_STR_SIZE 1024;
using std::wstring;
using std::string;

enum class CodeType
{
	ANSI,
	UTF8,
	UTF8_NO_BOM,
	UTF16,
	AUTO
};


class CCommon
{
public:
	CCommon();
	~CCommon();

	//将string类型的字符串转换成Unicode编码的wstring字符串
	static wstring StrToUnicode(const string& str, CodeType code_type = CodeType::ANSI);

	//将Unicode编码的wstring字符串转换成string类型的字符串(如果至少有一个字符无法转换，则将char_cannot_convert置为true)
	static string UnicodeToStr(const wstring& wstr, bool& char_cannot_convert, CodeType code_type = CodeType::ANSI);

	//判断字符串是否UTF8编码
	static bool IsUTF8Bytes(const char* data);

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
};

