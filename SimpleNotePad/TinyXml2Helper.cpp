#include "stdafx.h"
#include "TinyXml2Helper.h"

void CTinyXml2Helper::LoadXmlFile(tinyxml2::XMLDocument& doc, const wchar_t* file_path)
{
    FILE* pFile = nullptr;
    const errno_t err = _wfopen_s(&pFile, file_path, L"rb");
    doc.LoadFile(pFile);
    fclose(pFile);
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

