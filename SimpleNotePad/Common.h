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
};

