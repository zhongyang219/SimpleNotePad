#pragma once
#include "tinyxml2.h"
#include <string>
#include <functional>

class CTinyXml2Helper
{
public:

    static bool LoadXmlFile(tinyxml2::XMLDocument& doc, const wchar_t* file_path);

    static void IterateChildNode(tinyxml2::XMLElement* ele, std::function<void(tinyxml2::XMLElement*)> fun);

    static const char* ElementAttribute(tinyxml2::XMLElement* ele, const char* attr);
};

