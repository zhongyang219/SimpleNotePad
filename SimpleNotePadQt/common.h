#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QDomElement>
#include <QString>
#include "configdata.h"

class Common
{
public:
    Common();

    static bool HasUtf8Bom(const QByteArray& data);
    static bool HasUtf16Bom(const QByteArray& data);
    static bool HasUtf16BeBom(const QByteArray& data);

    static QByteArray DeleteBom(const QByteArray& data);

    static CodeType JudgeCode(const QByteArray& data);
    static bool IsUTF8Bytes(const char* data);
    //进行UTF16的BE、LE之间的转换
    static void convertBE_LE(wchar_t* bigEndianBuffer, unsigned int length);

    static QString BytesToString(const QByteArray& bytesData, CodeType code);

    static QByteArray StringToBytes(const QString& str, CodeType code);

    /**
     * @brief       遍历一个xml节点
     * @param[in]	root 要遍历的xml节点
     * @param[in]	func 一个回调函数或lambda表达式，函数原型为void(QDomElement*)
     * @return      返回值
     */
    template <class Func>
    static void IterateXmlElement(QDomElement* root, Func func)
    {
        if (root != nullptr)
        {
            func(root);
            QDomNodeList childNode = root->childNodes();
            for(int i = 0; i < childNode.count(); i++)
            {
                QDomElement nodeInfo = childNode.at(i).toElement();
                IterateXmlElement(&nodeInfo, func);
            }
        }
    }

    //判断一个字符是否是标签标识符（字母、数字和下划线）
    static bool IsCharactorIdentifier(char ch);

    //判断一个字符串是否是标签标识符（只含字母、数字和下划线）
    static bool IsStringIdentifier(const QString& str);


    static unsigned int QColorToColorref(const QColor &color);

    static QColor ColorrefToQColor(unsigned int ucolor);

};

#endif // COMMON_H
