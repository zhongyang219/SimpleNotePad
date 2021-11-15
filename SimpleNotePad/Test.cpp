#include "stdafx.h"
#include "Test.h"
#include "TinyXml2Helper.h"
#include "SyntaxHighlight.h"
#include "SimpleNotePad.h"

void CTest::Test()
{
    //TestXml();
    TextSaveStringList();
}

void CTest::TestXml()
{
    //tinyxml2::XMLDocument doc;

    //CTinyXml2Helper::LoadXmlFile(doc, L"./lang.xml");

    //CTinyXml2Helper::IterateChildNode(doc.FirstChildElement(), [](tinyxml2::XMLElement* child)
    //{
    //    int a = 0;
    //});

    //CSyntaxHighlight syntax;
    //syntax.LoadFromFile("./lang.xml");
    int a = 0;
}

void CTest::TextSaveStringList()
{
    std::vector<std::wstring> test;
    test.push_back(L"123");
    test.push_back(L"abc");
    test.push_back(L"测试");
    theApp.WriteStringList(L"test", L"test_string_list", test);
    std::vector<std::wstring> test_result;
    theApp.GetStringList(L"test", L"test_string_list", test_result);
    int a = 0;
}
