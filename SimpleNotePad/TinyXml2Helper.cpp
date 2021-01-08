#include "stdafx.h"
#include "TinyXml2Helper.h"

bool CTinyXml2Helper::LoadXmlFile(tinyxml2::XMLDocument& doc, const wchar_t* file_path)
{
    FILE* pFile = nullptr;
    const errno_t err = _wfopen_s(&pFile, file_path, L"rb");
    if (err || pFile == nullptr)
        return false;
    doc.LoadFile(pFile);
    fclose(pFile);
    return true;
}

void CTinyXml2Helper::IterateChildNode(tinyxml2::XMLElement* ele, std::function<void(tinyxml2::XMLElement*)> fun)
{
    if (ele == nullptr)
        return;

    tinyxml2::XMLElement* child = ele->FirstChildElement();
    if (child == nullptr)
        return;
    fun(child);
    while (true)
    {
        child = child->NextSiblingElement();
        if (child != nullptr)
            fun(child);
        else
            break;
    }
}

const char * CTinyXml2Helper::ElementAttribute(tinyxml2::XMLElement * ele, const char * attr)
{
    if (ele != nullptr)
    {
        const char* str = ele->Attribute(attr);
        if (str != nullptr)
            return str;
    }
    return "";
}

const char* CTinyXml2Helper::ElementName(tinyxml2::XMLElement* ele)
{
    if (ele != nullptr)
    {
        const char* str = ele->Name();
        if (str != nullptr)
            return str;
    }
    return "";
}

const char* CTinyXml2Helper::ElementText(tinyxml2::XMLElement* ele)
{
    if (ele != nullptr)
    {
        const char* str = ele->GetText();
        if (str != nullptr)
            return str;
    }
    return "";
}

