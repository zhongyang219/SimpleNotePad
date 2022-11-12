#pragma once
#include "CVariant.h"
#include "CommonData.h"
#include "resource.h"
#include <functional>
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
    UTF16BE,
    AUTO
};

struct CodeTypeItem
{
    CString name;
    CodeType code_type{};
    UINT code_page{};

    CodeTypeItem() {}
    CodeTypeItem(const CString& _name, CodeType _code_type, UINT _code_page)
        : name(_name), code_type(_code_type), code_page(_code_page)
    {}
    CodeTypeItem(const CString& _name, UINT _code_page)
        : name(_name), code_type(CodeType::ANSI), code_page(_code_page)
    {}
};


class CCommon
{
public:
    CCommon();
    ~CCommon();

    //将string类型的字符串转换成Unicode编码的wstring字符串，code_type指定转换方式，code_page指定当code_type为ANSI时的代码页
    static wstring StrToUnicode(const string& str, CodeType code_type = CodeType::ANSI, UINT code_page = CP_ACP);

    //将Unicode编码的wstring字符串转换成string类型的字符串(如果至少有一个字符无法转换，则将char_cannot_convert置为true)
    static string UnicodeToStr(const wstring& wstr, bool& char_cannot_convert, CodeType code_type = CodeType::ANSI, UINT code_page = CP_ACP);

    //进行UTF16的BE、LE之间的转换
    static void convertBE_LE(wchar_t* bigEndianBuffer, unsigned int length);

    //判断字符串是否UTF8编码
    static bool IsUTF8Bytes(const char* data);

    //字符大小写转换
    static void ConvertCharCase(wchar_t& ch, bool upper);

    //判断字符是否是一个字母
    static bool IsLetter(wchar_t ch);

    static void WritePrivateProfileInt(const wchar_t* AppName, const wchar_t* KeyName, int value, const wchar_t* Path);

    //static bool FileExist(const wchar_t* file);

    static unsigned __int64 GetFileLastModified(const wstring& file_path);

    //获取当前exe文件的路径（不含文件名）
    static wstring GetCurrentPath();

    //调整CListBox控件的宽度
    static void AdjustListWidth(CListBox& list);

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
    template<class T>
    static void StringSplit(const T& str, wchar_t div_ch, vector<T>& results, bool skip_empty = true)
    {
        results.clear();
        size_t split_index = -1;
        size_t last_split_index = -1;
        while (true)
        {
            split_index = str.find(div_ch, split_index + 1);
            T split_str = str.substr(last_split_index + 1, split_index - last_split_index - 1);
            if (!split_str.empty() || !skip_empty)
                results.push_back(split_str);
            if (split_index == string::npos)
                break;
            last_split_index = split_index;
        }

    }

    template<class T>
    static void StringSplit(const T& str, const T& div_str, vector<T>& results, bool skip_empty = true)
    {
        results.clear();
        size_t split_index = 0 - div_str.size();
        size_t last_split_index = 0 - div_str.size();
        while (true)
        {
            split_index = str.find(div_str, split_index + div_str.size());
            T split_str = str.substr(last_split_index + div_str.size(), split_index - last_split_index - div_str.size());
            if (!split_str.empty() || !skip_empty)
                results.push_back(split_str);
            if (split_index == string::npos)
                break;
            last_split_index = split_index;
        }
    }

    //template<class T>
    static void StringSplit(const std::string& str, const std::string& div_str, std::function<void(const std::string&)> func, bool skip_empty = true)
    {
        size_t split_index = 0 - div_str.size();
        size_t last_split_index = 0 - div_str.size();
        while (true)
        {
            split_index = str.find(div_str, split_index + div_str.size());
            std::string split_str = str.substr(last_split_index + div_str.size(), split_index - last_split_index - div_str.size());
            if (!split_str.empty() || !skip_empty)
                func(split_str);
            if (split_index == string::npos)
                break;
            last_split_index = split_index;
        }
    }

    template<class T>
    static T StringMerge(const vector<T>& str_list, wchar_t connector, bool skip_empty = true)
    {
        T result;
        for (const auto& str : str_list)
        {
            if (skip_empty && str.empty())
                continue;
            result += str;
            result.push_back(connector);
        }
        if (!str_list.empty())
            result.pop_back();
        return result;
    }

    //判断一个字符是否是标签标识符（字母、数字和下划线）
    static bool IsCharactorIdentifier(char ch);

    //判断一个字符串是否是标签标识符（只含字母、数字和下划线）
    static bool IsStringIdentifier(const std::string& str);

    //读取文件内容
    static bool GetFileContent(const wchar_t* file_path, string& contents_buff);

    //读取文件内容
    //file_path: 文件的路径
    //length: 文件的长度
    //返回值: 读取成功返回读取到的文件内容的const char类型的指针，在使用完毕后这个指针需要自行使用delete释放。读取失败返回nullptr
    static const char* GetFileContent(const wchar_t* file_path, size_t& length);

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

    enum class ProcessMsgType
    {
        CLIP_BOARD,
        SETTINGS_DATA,
        MENU_SETTINGS_DATA,
    };

    //向其他进程发送WM_COPYDATA消息
    static void SendProcessMessage(HWND hwnd, ProcessMsgType msg_id, const std::wstring& msg_data);
    static void SendProcessMessage(HWND hwnd, ProcessMsgType msg_id, const std::string& msg_data);

    static void ParseProcessMessage(COPYDATASTRUCT* copy_data, ProcessMsgType& msg_id, std::wstring& msg_data);
    static void ParseProcessMessage(COPYDATASTRUCT* copy_data, ProcessMsgType& msg_id, std::string& msg_data);

    //当前是否为Debug模式
    static bool IsDebugMode();

    //判断一个SimpleNotePad的窗口是否为Debug模式窗口
    static bool IsWindowDebugMode(HWND hWnd);

    //根据类名查找所有句柄
    static void FindAllWindow(LPCTSTR class_name, std::vector<HWND>& result);

    static bool OpenRegItem(CRegKey& key, LPCTSTR item_str);

};



struct ConstVal
{
public:
    const vector<CodeTypeItem>& CodeList() { return code_list; }
    const vector<CodeTypeItem>& CodePageList() { return code_page_list; }
    int GetCodeTypeItemIndex(CodeType code_type, UINT code_page);
    static ConstVal* Instance();

private:
    vector<CodeTypeItem> code_list;           //所有编码格式列表
    vector<CodeTypeItem> code_page_list;      //仅ANSI格式列表
    void Init();
    void InitCommonParts(vector<CodeTypeItem>& item_list);
};

#define CONST_VAL ConstVal::Instance()
