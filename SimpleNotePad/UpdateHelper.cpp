#include "stdafx.h"
#include "UpdateHelper.h"
#include "SimpleXML.h"
#include "Common.h"


CUpdateHelper::CUpdateHelper()
{
}


CUpdateHelper::~CUpdateHelper()
{
}

void CUpdateHelper::SetUpdateSource(UpdateSource update_source)
{
    m_update_source = update_source;
}

bool CUpdateHelper::CheckForUpdate()
{
    wstring version_info;
    //使用GitHub更新源
    if (m_update_source == UpdateSource::GitHubSource)
    {
        if (!CCommon::GetURL(L"https://raw.githubusercontent.com/zhongyang219/SimpleNotePad/master/version.info", version_info, true))     //获取版本信息
            return false;
    }
    //使用Gitee更新源
    else
    {
        if (!CCommon::GetURL(L"https://gitee.com/zhongyang219/SimpleNotePad/raw/master/version.info", version_info, true))     //获取版本信息
            return false;
    }

    ParseUpdateInfo(version_info);

    return true;
}

void CUpdateHelper::ParseUpdateInfo(wstring version_info)
{
    CSimpleXML version_xml;
    version_xml.LoadXMLContentDirect(version_info);

    m_version = version_xml.GetNode(L"version");
    wstring str_source_tag = (m_update_source == UpdateSource::GitHubSource ? L"GitHub" : L"Gitee");
    wstring str_link_tag, str_link_tag_x64;
#ifdef WITHOUT_TEMPERATURE
    str_link_tag = L"link_without_temperature";
    str_link_tag_x64 = L"link_without_temperature_x64";
#else
    str_link_tag = L"link";
    str_link_tag_x64 = L"link_x64";
#endif
    m_link64 = version_xml.GetNode(str_link_tag_x64.c_str(), str_source_tag.c_str());
    m_link = version_xml.GetNode(str_link_tag.c_str(), str_source_tag.c_str());
    CString contents_zh_cn = version_xml.GetNode(L"contents_zh_cn", L"update_contents").c_str();
    //CString contents_en = version_xml.GetNode(L"contents_en", L"update_contents").c_str();
    contents_zh_cn.Replace(L"\\n", L"\r\n");
    //contents_en.Replace(L"\\n", L"\r\n");
    m_contents_zh_cn = contents_zh_cn;
    //m_contents_en = contents_en;
}

const std::wstring& CUpdateHelper::GetVersion() const
{
    return m_version;
}

const std::wstring& CUpdateHelper::GetLink() const
{
    return m_link;
}

const std::wstring& CUpdateHelper::GetLink64() const
{
    return m_link64;
}

//const std::wstring& CUpdateHelper::GetContentsEn() const
//{
//    return m_contents_en;
//}

const std::wstring& CUpdateHelper::GetContentsZhCn() const
{
    return m_contents_zh_cn;
}
