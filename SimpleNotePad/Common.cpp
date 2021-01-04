#include "stdafx.h"
#include "Common.h"


CCommon::CCommon()
{
}


CCommon::~CCommon()
{
}


wstring CCommon::StrToUnicode(const string & str, CodeType code_type, UINT code_page)
{
	if (str.empty()) return wstring();
	wstring result;
	int size;
	if (code_type == CodeType::ANSI)
	{
		size = MultiByteToWideChar(code_page, 0, str.c_str(), -1, NULL, 0);
		if (size <= 0) return wstring();
		wchar_t* str_unicode = new wchar_t[size + 1];
		MultiByteToWideChar(code_page, 0, str.c_str(), -1, str_unicode, size);
		result.assign(str_unicode);
		delete[] str_unicode;
	}
	else if (code_type == CodeType::UTF8 || code_type == CodeType::UTF8_NO_BOM)
	{
		string temp;
		//如果前面有BOM，则去掉BOM
		if (str.size() >= 3 && str[0] == -17 && str[1] == -69 && str[2] == -65)
			temp = str.substr(3);
		else
			temp = str;
		size = MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, NULL, 0);
		if (size <= 0) return wstring();
		wchar_t* str_unicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, str_unicode, size);
		result.assign(str_unicode);
		delete[] str_unicode;
	}
	else if (code_type == CodeType::UTF16)
	{
		string temp;
		//如果前面有BOM，则去掉BOM
		if (str.size() >= 2 && str[0] == -1 && str[1] == -2)
			temp = str.substr(2);
		else
			temp = str;
		if (temp.size() % 2 == 1)
			temp.pop_back();
		temp.push_back('\0');
		result = (const wchar_t*)temp.c_str();
	}
	return result;
}

string CCommon::UnicodeToStr(const wstring & wstr, bool& char_cannot_convert, CodeType code_type, UINT code_page)
{
	if (wstr.empty()) return string();
	char_cannot_convert = false;
	BOOL UsedDefaultChar{ FALSE };
	string result;
	int size{ 0 };
	if (code_type == CodeType::ANSI)
	{
		size = WideCharToMultiByte(code_page, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (size <= 0) return string();
		char* str = new char[size + 1];
		WideCharToMultiByte(code_page, 0, wstr.c_str(), -1, str, size, NULL, &UsedDefaultChar);
		result.assign(str);
		delete[] str;
	}
	else if (code_type == CodeType::UTF8 || code_type == CodeType::UTF8_NO_BOM)
	{
		result.clear();
		if (code_type == CodeType::UTF8)
		{
			result.push_back(-17);
			result.push_back(-69);
			result.push_back(-65);
		}
		size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (size <= 0) return string();
		char* str = new char[size + 1];
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str, size, NULL, NULL);
		result.append(str);
		delete[] str;
	}
	else if (code_type == CodeType::UTF16)
	{
		result.clear();
		result.push_back(-1);	//在前面加上UTF16的BOM
		result.push_back(-2);
		result.append((const char*)wstr.c_str(), (const char*)wstr.c_str() + wstr.size() * 2);
		result.push_back('\0');
	}
	char_cannot_convert = (UsedDefaultChar != FALSE);
	return result;
}

bool CCommon::IsUTF8Bytes(const char * data)
{
	int charByteCounter = 1;  //计算当前正分析的字符应还有的字节数
	unsigned char curByte; //当前分析的字节.
	bool ascii = true;
	size_t length{ strlen(data) };
	for (int i = 0; i < length; i++)
	{
		curByte = static_cast<unsigned char>(data[i]);
		if (charByteCounter == 1)
		{
			if (curByte >= 0x80)
			{
				ascii = false;
				//判断当前
				while (((curByte <<= 1) & 0x80) != 0)
				{
					charByteCounter++;
				}
				//标记位首位若为非0 则至少以2个1开始 如:110XXXXX...........1111110X 
				if (charByteCounter == 1 || charByteCounter > 6)
				{
					return false;
				}
			}
		}
		else
		{
			//若是UTF-8 此时第一位必须为1
			if ((curByte & 0xC0) != 0x80)
			{
				return false;
			}
			charByteCounter--;
		}
	}
	if (ascii) return false;		//如果全是ASCII字符，返回false
	else return true;
}

void CCommon::WritePrivateProfileInt(const wchar_t * AppName, const wchar_t * KeyName, int value, const wchar_t * Path)
{
	wchar_t buff[16];
	_itow_s(value, buff, 10);
	WritePrivateProfileStringW(AppName, KeyName, buff, Path);
}

//bool CCommon::FileExist(const wchar_t * file)
//{
//	_wfinddata_t fileinfo;
//	return (_wfindfirst(file, &fileinfo) != -1);
//}

wstring CCommon::GetCurrentPath()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	size_t index;
	wstring current_path{ path };
	index = current_path.find_last_of(L'\\');
	current_path = current_path.substr(0, index + 1);
	return current_path;
}


void CCommon::AdjustListWidth(CListBox& list)
{
	CDC *pDC = list.GetDC();
	if (NULL == pDC)
	{
		return;
	}
	int nCount = list.GetCount();
	if (nCount < 1)
		return;
	int nMaxExtent = 0;
	CString szText;
	for (int i = 0; i < nCount; ++i)
	{
		list.GetText(i, szText);
		CSize &cs = pDC->GetTextExtent(szText);
		if (cs.cx > nMaxExtent)
		{
			nMaxExtent = cs.cx;
		}
	}
	list.SetHorizontalExtent(nMaxExtent);
}

void CCommon::ChangeStringLength(CString& str, int length, TCHAR ch)
{
	if (length > str.GetLength())
	{
		int size_deff{ length - str.GetLength() };
		CString insert_front(ch, size_deff);
		str = insert_front + str;
	}
	else if (length < str.GetLength())
	{
		str = str.Right(length);
	}
}

void CCommon::EditAppendString(const CString & str, HWND hWnd)
{
	int length = str.GetLength();
	for (int i = 0; i<length; )	//发送消息到 IDC_REPORT_EDIT
	{
		if (str[i]< 128)
		{
			::SendMessage(hWnd, WM_CHAR, str[i], 0);
		}
		else
		{
			::SendMessage(hWnd, WM_CHAR, str[i], 0);
			::SendMessage(hWnd, WM_CHAR, str[i + 1], 0);
			i++;
		}
		i++;
	}
}

bool CCommon::IsValidNameChar(wchar_t ch)
{
	return ((ch >= L'0'&&ch <= L'9') || (ch >= L'a'&&ch <= L'z') || (ch >= L'A'&&ch <= L'Z') || ch == L'_' || ch > 256);
}

bool CCommon::StringTransform(wstring & str, bool upper)
{
	if (str.empty()) return false;
	//if (upper)
	//	std::transform(str.begin(), str.end(), str.begin(), toupper);
	//else
	//	std::transform(str.begin(), str.end(), str.begin(), tolower);
	for (auto& ch : str)
	{
		if (upper)
		{
			if (ch >= 'a'&&ch <= 'z')
				ch -= 32;
		}
		else
		{
			if (ch >= 'A'&&ch <= 'Z')
				ch += 32;
		}
	}
	return true;
}

size_t CCommon::StringFindNoCase(const wstring & str, const wstring & find_str, bool find_down, size_t offset)
{
	wstring _str{ str }, _find_str{ find_str };
	StringTransform(_str, false);
	StringTransform(_find_str, false);
	if (find_down)
		return _str.find(_find_str, offset);
	else
		return _str.rfind(_find_str, offset);
}

size_t CCommon::StringFindWholeWord(const wstring & str, const wstring & find_str, bool no_case, bool find_down, size_t offset)
{
	wstring _str{ str }, _find_str{ find_str };
	if (no_case)
	{
		StringTransform(_str, false);
		StringTransform(_find_str, false);
	}
	if (find_down)
	{
		size_t index{ offset - 1 };
		int find_str_front_pos, find_str_back_pos;		//找到的字符串在原字符串中前面和后面一个字符的位置
		int size = _str.size();
		int find_str_size = _find_str.size();
		while (true)
		{
			index = _str.find(_find_str, index + 1);
			if (index == wstring::npos) break;
			find_str_front_pos = index - 1;
			find_str_back_pos = index + find_str_size;
			if ((find_str_front_pos < 0 || IsDivideChar(_str[find_str_front_pos])) && (find_str_back_pos >= size || IsDivideChar(_str[find_str_back_pos])))
				return index;
			else
				continue;
		}
	}
	else
	{
		int size = _str.size();
		size_t index{ offset + 1 };
		int find_str_front_pos, find_str_back_pos;		//找到的字符串在原字符串中前面和后面一个字符的位置
		int find_str_size = _find_str.size();
		while (true)
		{
			index = _str.rfind(_find_str, index - 1);
			if (index == wstring::npos) break;
			find_str_front_pos = index - 1;
			find_str_back_pos = index + find_str_size;
			if ((find_str_front_pos < 0 || IsDivideChar(_str[find_str_front_pos])) && (find_str_back_pos >= size || IsDivideChar(_str[find_str_back_pos])))
				return index;
			else
				continue;
		}
	}
	return -1;
}

bool CCommon::IsDivideChar(wchar_t ch)
{
	if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z') || ch > 255)
		return false;
	else
		return true;
}

size_t CCommon::StringFind(const wstring & str, const wstring & find_str, bool no_case, bool whole_word, bool find_down, size_t offset)
{
	if (!no_case && !whole_word)		//匹配大小写，且不全词匹配时
	{
		if (find_down)
			return str.find(find_str, offset);
		else
			return str.rfind(find_str, offset);
	}
	else if (no_case && !whole_word)		//不匹配大小写，且不全词匹配时
	{
		return StringFindNoCase(str, find_str, find_down, offset);
	}
	else									//全词匹配时
	{
		return StringFindWholeWord(str, find_str, no_case, find_down, offset);
	}
}

int CCommon::FontSizeToLfHeight(int font_size)
{
    HDC hDC = ::GetDC(HWND_DESKTOP);
    int lfHeight = -MulDiv(font_size, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    ::ReleaseDC(HWND_DESKTOP, hDC);
    return lfHeight;
}
