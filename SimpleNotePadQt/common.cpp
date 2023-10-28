#include "common.h"

Common::Common()
{

}

bool Common::HasUtf8Bom(const QByteArray &data)
{
    return (data.size() >= 3 && data[0] == -17 && data[1] == -69 && data[2] == -65);
}

bool Common::HasUtf16Bom(const QByteArray &data)
{
    return (data.size() >= 2 && data[0] == -1 && data[1] == -2);
}

bool Common::HasUtf16BeBom(const QByteArray &data)
{
    return (data.size() >= 2 && data[0] == -2 && data[1] == -1);
}

QByteArray Common::DeleteBom(const QByteArray &data)
{
    if (HasUtf8Bom(data))
        return data.mid(3);
    else if (HasUtf16Bom(data) || HasUtf16BeBom(data))
        return data.mid(2);
    return data;
}

CodeType Common::JudgeCode(const QByteArray &data)
{
    if (HasUtf8Bom(data))
    {
        return CodeType::UTF8;
    }
    else if (HasUtf16Bom(data))
    {
        return CodeType::UTF16;
    }
    else if (HasUtf16BeBom(data))
    {
        return CodeType::UTF16BE;
    }
    else if (IsUTF8Bytes(data.constData()))
    {
        return CodeType::UTF8_NO_BOM;
    }
    else
    {
        return CodeType::ANSI;
    }
}

bool Common::IsUTF8Bytes(const char *data)
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

void Common::convertBE_LE(wchar_t *bigEndianBuffer, unsigned int length)
{
    for (unsigned int i = 0; i < length; ++i)
    {
        unsigned char* chr = (unsigned char*)(bigEndianBuffer + i);
        unsigned char temp = *chr;
        *chr = *(chr + 1);
        *(chr + 1) = temp;
    }
}

QString Common::BytesToString(const QByteArray &bytesData, CodeType code)
{
    if (code == CodeType::AUTO)
        code = JudgeCode(bytesData);
    QString text;
    switch (code)
    {
    case CodeType::ANSI:
        text = QString::fromLocal8Bit(bytesData);
        break;
    case CodeType::UTF8:
        text = QString::fromUtf8(DeleteBom(bytesData));
        break;
    case CodeType::UTF8_NO_BOM:
        text = QString::fromUtf8(bytesData);
        break;
    case CodeType::UTF16:
        text = QString::fromWCharArray((const wchar_t*)DeleteBom(bytesData).constData());
        break;
    case CodeType::UTF16BE:
    {
        QByteArray bytesDataNew = DeleteBom(bytesData);
        wchar_t* p = (wchar_t*)bytesDataNew.data();
        convertBE_LE(p, bytesData.size() >> 1);
        text = QString::fromWCharArray(p, bytesData.size() / sizeof(wchar_t));
    }
        break;
    default:
        text = bytesData;
        break;
    }
    return text;
}

QByteArray Common::StringToBytes(const QString &str, CodeType code)
{
    switch (code)
    {
    case CodeType::ANSI:
        return str.toLocal8Bit();
    case CodeType::UTF8_NO_BOM:
        return str.toUtf8();
    case CodeType::UTF8:
    {
        QByteArray bytesData;
        bytesData.push_back(-17);
        bytesData.push_back(-69);
        bytesData.push_back(-65);
        bytesData += str.toUtf8();
        return bytesData;
    }
    case CodeType::UTF16:
    {
        QByteArray bytesData;
        bytesData.push_back(-1);	//在前面加上UTF16的BOM
        bytesData.push_back(-2);
        std::wstring wstr = str.toStdWString();
        bytesData.append((const char*)wstr.c_str(), wstr.size() * 2);
        return bytesData;
    }
    case CodeType::UTF16BE:
    {
        QByteArray bytesData;
        bytesData.push_back(-2);	//在前面加上UTF16BE的BOM
        bytesData.push_back(-1);
        std::wstring wstr = str.toStdWString();
        std::wstring temp = wstr;
        wchar_t* p = (wchar_t*)temp.c_str();
        convertBE_LE(p, temp.size());
        bytesData.append((const char*)temp.c_str(), temp.size() * 2);
        return bytesData;
    }

    }
    return str.toLatin1();
}

bool Common::IsCharactorIdentifier(char ch)
{
    return (ch >= 'a' && ch <= 'z')
        || (ch >= 'A' && ch <= 'Z')
        || (ch >= '0' && ch <= '9')
        || ch == '_'
        || ch < 0;
}

bool Common::IsStringIdentifier(const QString &str)
{
    for (const auto& ch : str)
    {
        if (!IsCharactorIdentifier(ch.toLatin1()))
            return false;
    }
    return true;
}

unsigned int Common::QColorToColorref(const QColor &color)
{
    return static_cast<unsigned int>((color.red() << 16) + (color.green() << 8) + color.blue());
}

QColor Common::ColorrefToQColor(unsigned int ucolor)
{
    return QColor((ucolor >> 16) & 0xff, (ucolor >> 8) & 0xff, ucolor & 0xff);
}
