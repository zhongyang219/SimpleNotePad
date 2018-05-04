#include "stdafx.h"
#include "Common.h"


CCommon::CCommon()
{
}


CCommon::~CCommon()
{
}


wstring CCommon::StrToUnicode(const string & str, CodeType code_type)
{
	if (str.empty()) return wstring();
	wstring result;
	int size;
	if (code_type == CodeType::ANSI)
	{
		size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		if (size <= 0) return wstring();
		wchar_t* str_unicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str_unicode, size);
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

string CCommon::UnicodeToStr(const wstring & wstr, bool& char_cannot_convert, CodeType code_type)
{
	if (wstr.empty()) return string();
	char_cannot_convert = false;
	BOOL UsedDefaultChar{ FALSE };
	string result;
	int size{ 0 };
	if (code_type == CodeType::ANSI)
	{
		size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (size <= 0) return string();
		char* str = new char[size + 1];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, str, size, NULL, &UsedDefaultChar);
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
	wchar_t buff[11];
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
