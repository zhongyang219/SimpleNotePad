#include "stdafx.h"
#include "Common.h"
#include <afxinet.h>    //用于支持使用网络相关的类


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
		size = MultiByteToWideChar(code_page, 0, str.c_str(), str.size(), NULL, 0);
		if (size <= 0) return wstring();
		wchar_t* str_unicode = new wchar_t[size + 1];
		MultiByteToWideChar(code_page, 0, str.c_str(), str.size(), str_unicode, size);
		result.assign(str_unicode, size);
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
		size = MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), temp.size(), NULL, 0);
		if (size <= 0) return wstring();
		wchar_t* str_unicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), temp.size(), str_unicode, size);
		result.assign(str_unicode, size);
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
		result.assign((const wchar_t*)temp.c_str(), temp.size() / sizeof(wchar_t));
	}
    else if (code_type == CodeType::UTF16BE)
    {
        string temp;
        //如果前面有BOM，则去掉BOM
        if (str.size() >= 2 && str[0] == -2 && str[1] == -1)
            temp = str.substr(2);
        else
            temp = str;
        if (temp.size() % 2 == 1)
            temp.pop_back();
        temp.push_back('\0');
        wchar_t* p = (wchar_t*)temp.c_str();
        convertBE_LE(p, temp.size() >> 1);
        result.assign(p, temp.size() / sizeof(wchar_t));
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
		size = WideCharToMultiByte(code_page, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		if (size <= 0) return string();
		char* str = new char[size + 1];
		WideCharToMultiByte(code_page, 0, wstr.c_str(), wstr.size(), str, size, NULL, &UsedDefaultChar);
		result.assign(str, size);
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
		size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		if (size <= 0) return string();
		char* str = new char[size + 1];
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), str, size, NULL, NULL);
		result.append(str, size);
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
    else if (code_type == CodeType::UTF16BE)
    {
        result.clear();
        result.push_back(-2);	//在前面加上UTF16BE的BOM
        result.push_back(-1);
        wstring temp = wstr;
        wchar_t* p = (wchar_t*)temp.c_str();
        convertBE_LE(p, temp.size());
        result.append((const char*)temp.c_str(), (const char*)temp.c_str() + temp.size() * 2);
        result.push_back('\0');
    }
    char_cannot_convert = (UsedDefaultChar != FALSE);
	return result;
}

void CCommon::convertBE_LE(wchar_t* bigEndianBuffer, unsigned int length)
{
    for (unsigned int i = 0; i < length; ++i)
    {
        unsigned char* chr = (unsigned char*)(bigEndianBuffer + i);
        unsigned char temp = *chr;
        *chr = *(chr + 1);
        *(chr + 1) = temp;
    }
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

void CCommon::ConvertCharCase(wchar_t& ch, bool upper)
{
    if (upper)      //转换成大写
    {
        if (ch >= L'a' && ch <= L'z')
            ch -= 32;
    }
    else            //转换成小写
    {
        if (ch >= L'A' && ch <= L'Z')
            ch += 32;
    }
}

bool CCommon::IsLetter(wchar_t ch)
{
    return (ch >= L'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
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

bool CCommon::IsCharactorIdentifier(char ch)
{
    return (ch >= 'a' && ch <= 'z')
        || (ch >= 'A' && ch <= 'Z')
        || (ch >= '0' && ch <= '9')
        || ch == '_'
        || ch < 0;
}

bool CCommon::IsStringIdentifier(const std::string& str)
{
    for (const auto& ch : str)
    {
        if (!IsCharactorIdentifier(ch))
            return false;
    }
    return true;
}

bool CCommon::GetFileContent(const wchar_t* file_path, string& contents_buff)
{
    std::ifstream file{ file_path, std::ios::binary };
    if (file.fail())
        return false;
    //获取文件长度
    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);

    char* buff = new char[length];
    file.read(buff, length);
    file.close();

    contents_buff.assign(buff, length);
    delete[] buff;

    return true;
}

const char* CCommon::GetFileContent(const wchar_t* file_path, size_t& length)
{
    std::ifstream file{ file_path, std::ios::binary };
    length = 0;
    if (file.fail())
        return nullptr;
    //获取文件长度
    file.seekg(0, file.end);
    length = file.tellg();
    file.seekg(0, file.beg);

    char* buff = new char[length];
    file.read(buff, length);
    file.close();

    return buff;
}

wstring CCommon::GetExePath()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    size_t index;
    wstring current_path{ path };
    index = current_path.find_last_of(L'\\');
    current_path = current_path.substr(0, index + 1);
    return current_path;
}

bool CCommon::GetURL(const wstring& url, wstring& result, bool utf8, const wstring& user_agent)
{
    bool succeed{ false };
    CInternetSession* pSession{};
    CHttpFile* pfile{};
    try
    {
        pSession = new CInternetSession(user_agent.c_str());
        pfile = (CHttpFile*)pSession->OpenURL(url.c_str());
        DWORD dwStatusCode;
        pfile->QueryInfoStatusCode(dwStatusCode);
        if (dwStatusCode == HTTP_STATUS_OK)
        {
            CString content;
            CString data;
            while (pfile->ReadString(data))
            {
                content += data;
            }
            result = StrToUnicode((const char*)content.GetString(), utf8 ? CodeType::UTF8 : CodeType::ANSI);
            succeed = true;
        }
        pfile->Close();
        delete pfile;
        pSession->Close();
    }
    catch (CInternetException* e)
    {
        //写入错误日志
        //CString info = CCommon::LoadTextFormat(IDS_GET_URL_ERROR_LOG_INFO, { url, static_cast<size_t>(e->m_dwError) });
        //CCommon::WriteLog(info, theApp.m_log_path.c_str());
        if (pfile != nullptr)
        {
            pfile->Close();
            delete pfile;
        }
        if (pSession != nullptr)
            pSession->Close();
        succeed = false;
        e->Delete();        //没有这句会造成内存泄露
        SAFE_DELETE(pSession);
    }
    SAFE_DELETE(pSession);
    return succeed;
}

CString CCommon::LoadText(UINT id, LPCTSTR back_str /*= nullptr*/)
{
    CString str;
    str.LoadString(id);
    if (back_str != nullptr)
        str += back_str;
    return str;
}

CString CCommon::LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str /*= nullptr*/)
{
    CString str;
    str.LoadString(id);
    if (back_str != nullptr)
        str += back_str;
    if (front_str != nullptr)
        str = front_str + str;
    return str;
}

CString CCommon::StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras)
{
    CString str_rtn = format_str;
    int index = 1;
    for (const auto& para : paras)
    {
        CString para_str = para.ToString();
        CString format_para;
        format_para.Format(_T("<%%%d%%>"), index);
        str_rtn.Replace(format_para, para_str);

        index++;
    }
    return str_rtn;
}

CString CCommon::LoadTextFormat(UINT id, const std::initializer_list<CVariant>& paras)
{
    CString str;
    str.LoadString(id);
    return StringFormat(str.GetString(), paras);
}

void CCommon::SetThreadLanguage(Language language)
{
    switch (language)
    {
    case Language::ENGLISH: SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)); break;
    case Language::SIMPLIFIED_CHINESE: SetThreadUILanguage(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)); break;
    default: break;
    }
}

void CCommon::SendProcessMessage(HWND hwnd, ProcessMsgType msg_id, const std::wstring& msg_data)
{
    //通过WM_COPYDATA消息向已有进程传递消息
    COPYDATASTRUCT copy_data;
    copy_data.dwData = static_cast<UINT>(msg_id);
    copy_data.cbData = msg_data.size() * sizeof(wchar_t);
    copy_data.lpData = (const PVOID)msg_data.c_str();
    ::SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&copy_data);
}

void CCommon::ParseProcessMessage(COPYDATASTRUCT* copy_data, ProcessMsgType& msg_id, std::wstring& msg_data)
{
    if (copy_data != nullptr)
    {
        msg_id = static_cast<ProcessMsgType>(copy_data->dwData);
        int length = copy_data->cbData / sizeof(wchar_t);
        const wchar_t* str = (const wchar_t*)copy_data->lpData;
        msg_data.assign(str, length);
    }
}

void CCommon::FindAllWindow(LPCTSTR class_name, std::vector<HWND>& result)
{
    struct ParaData
    {
        LPCTSTR class_name;
        std::vector<HWND>& result;
        ParaData(LPCTSTR _class_name, std::vector<HWND>& _result)
            : class_name(_class_name), result(_result)
        {}
    };
    //由于EnumChildWindows的回调函数中无法获取当前函数的参数，因此构造了一个结构体ParaData，通过该结构体向回调函数传递数据
    ParaData para_data(class_name, result);
    ::EnumChildWindows(NULL, [](HWND hWnd, LPARAM lparam)->BOOL
        {
            TCHAR buff[256]{};
            ::GetClassName(hWnd, buff, 256);
            ParaData* para_data = (ParaData*)lparam;
            if (wstring(para_data->class_name) == buff)
                para_data->result.push_back(hWnd);
            return TRUE;

        }, (LPARAM)&para_data);
}


bool CCommon::OpenRegItem(CRegKey& key, LPCTSTR item_str)
{
    if (key.Open(HKEY_CURRENT_USER, item_str) != ERROR_SUCCESS)
    {
        //如果项不存在，则创建
        if (key.Create(HKEY_CURRENT_USER, item_str) != ERROR_SUCCESS)
            return false;
        //再次打开
        if (key.Open(HKEY_CURRENT_USER, item_str) != ERROR_SUCCESS)
            return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
ConstVal* ConstVal::Instance()
{
    static std::shared_ptr<ConstVal> instance;
    if (instance == nullptr)
    {
        instance = std::make_shared<ConstVal>();
        instance->Init();
    }
    return instance.get();
}

void ConstVal::Init()
{
    code_list.emplace_back(CCommon::LoadText(_T("ANSI ("), IDS_LOCAL_CODE_PAGE, _T(")")), CodeType::ANSI, CP_ACP);
    code_list.emplace_back(CCommon::LoadText(IDS_UTF8_BOM), CodeType::UTF8, CP_ACP);
    code_list.emplace_back(CCommon::LoadText(IDS_UTF8_NO_BOM), CodeType::UTF8_NO_BOM, CP_ACP);
    code_list.emplace_back(_T("UTF-16"), CodeType::UTF16, CP_ACP);
    code_list.emplace_back(_T("UTF-16 Big Ending"), CodeType::UTF16BE, CP_ACP);
    InitCommonParts(code_list);
    code_list.emplace_back(CCommon::LoadText(IDS_DEFAULT_CODE_PAGE_FOR_NONE_UNICODE_IN_SETTINGS), CodeType::ANSI, CODE_PAGE_DEFAULT);

    code_page_list.emplace_back(CCommon::LoadText(IDS_LOCAL_CODE_PAGE), CP_ACP);
    InitCommonParts(code_page_list);
    code_page_list.emplace_back(CCommon::LoadText(IDS_SPECIFY_CODE_PAGE_MANUALLY), 0);
}

void ConstVal::InitCommonParts(vector<CodeTypeItem>& item_list)
{
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_SIMPLIFIED_CHINESE), CODE_PAGE_CHS);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_TRADITIONAL_CHINESE), CODE_PAGE_CHT);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_JAPANESE), CODE_PAGE_JP);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_WESTERN_EUROPE_LANGUAGE), CODE_PAGE_EN);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_KOREAN), CODE_PAGE_KOR);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_THAI), CODE_PAGE_THAI);
    item_list.emplace_back(CCommon::LoadText(IDS_CODE_PAGE_VIETNAMESE), CODE_PAGE_VIET);
}
