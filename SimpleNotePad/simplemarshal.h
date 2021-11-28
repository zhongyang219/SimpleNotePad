#ifndef __SIMPLEMARSHAL_H__
#define __SIMPLEMARSHAL_H__

// 简单结构体序列化实现 =》

#include <stdint.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace dakuang
{

// 本地字节序 -> 网络字节序
inline uint16_t HTONS(uint16_t u16)
{
    return ( (u16 << 8) | (u16 >> 8) );
}
inline uint32_t HTONL(uint32_t u32)
{
    return ( (uint32_t(HTONS(uint16_t(u32))) << 16) | HTONS(uint16_t(u32 >> 16)) );
}
inline uint64_t HTONLL(uint64_t u64)
{
    return ( (uint64_t(HTONL(uint32_t(u64))) << 32) | HTONL(uint32_t(u64 >> 32)) );
}

// 网络字节序 -> 本地字节序
#define NTOHS HTONS
#define NTOHL HTONL
#define NTOHLL HTONLL

// 定义序列化操作类
class SimplePack
{
private:
    std::string m_strBuffer;

    SimplePack (const SimplePack & o);
    SimplePack & operator = (const SimplePack& o);

public:
    static uint16_t xhtons(uint16_t u16) { return HTONS(u16); }
    static uint32_t xhtonl(uint32_t u32) { return HTONL(u32); }
    static uint64_t xhtonll(uint64_t u64) { return HTONLL(u64); }

    SimplePack() {}
    virtual ~SimplePack() {}

    const char * data() const { return m_strBuffer.data(); }
    size_t size() const { return m_strBuffer.size(); }

    SimplePack & push(const void * pData, size_t nSize)
    {
        m_strBuffer.append((const char*)pData, nSize);
        return *this;
    }

    SimplePack & push_uint8(uint8_t u8) { return push(&u8, 1); }
    SimplePack & push_uint16(uint16_t u16) { u16 = xhtons(u16); return push(&u16, 2); }
    SimplePack & push_uint32(uint32_t u32) { u32 = xhtonl(u32); return push(&u32, 4); }
    SimplePack & push_uint64(uint64_t u64) { u64 = xhtonll(u64); return push(&u64, 8); }
    SimplePack & push_string(const void * pData, size_t nSize)
    {
        if (nSize > 0xFFFF) throw std::runtime_error("[SimplePack::push_string] string too big");
        return push_uint16(uint16_t(nSize)).push(pData, nSize);
    }
    SimplePack & push_string32(const void * pData, size_t nSize)
    {
        if (nSize > 0xFFFFFFFF) throw std::runtime_error("[SimplePack::push_string32] string too big");
        return push_uint32(uint32_t(nSize)).push(pData, nSize);
    }

    SimplePack & replace(size_t nPos, const void * pData, size_t nSize)
    {
        m_strBuffer.replace(nPos, nSize, (const char*)pData, nSize);
        return *this;
    }

    SimplePack & replace_uint8(size_t nPos, uint8_t u8) { return replace(nPos, &u8, 1); }
    SimplePack & replace_uint16(size_t nPos, uint16_t u16) { u16 = xhtons(u16); return replace(nPos, &u16, 2); }
    SimplePack & replace_uint32(size_t nPos, uint32_t u32) { u32 = xhtonl(u32); return replace(nPos, &u32, 4); }
    SimplePack & replace_uint64(size_t nPos, uint64_t u64) { u64 = xhtonll(u64); return replace(nPos, &u64, 8); }
    SimplePack & replace_string(size_t nPos, const void * pData, size_t nSize) { return replace_uint16(nPos, uint16_t(nSize)).replace(nPos + 2, pData, nSize); }
    SimplePack & replace_string32(size_t nPos, const void * pData, size_t nSize) { return replace_uint32(nPos, uint32_t(nSize)).replace(nPos + 4, pData, nSize); }
};

// 定义反序列化操作类
class SimpleUnpack
{
private:
    mutable const char * m_pData;
    mutable size_t m_nSize;

public:
    static uint16_t xntohs(uint16_t u16) { return NTOHS(u16); }
    static uint32_t xntohl(uint32_t u32) { return NTOHL(u32); }
    static uint64_t xntohll(uint64_t u64) { return NTOHLL(u64); }

    SimpleUnpack(const void * pData, size_t nSize)
    {
        reset(pData, nSize);
    }
    virtual ~SimpleUnpack()
    {
        reset(NULL, 0);
    }

    operator const void *() const { return m_pData; }
    bool operator!() const { return (NULL == m_pData); }

    void reset(const void * pData, size_t nSize) const
    {
        m_pData = (const char *)pData;
        m_nSize = nSize;
    }

    const char * data() const { return m_pData; }
    size_t size() const	  { return m_nSize; }

    bool empty() const	  { return size() == 0; }

    void finish() const
    {
        if (!empty())
            throw std::runtime_error("[DSUnpack::finish] too much data");
    }

    const char * pop_fetch_ptr(size_t nSize, bool bPeek = false) const
    {
        if (m_nSize < nSize)
            throw std::runtime_error("[SimpleUnpack::pop_fetch_ptr] not enough data");

        const char * pData = m_pData;

        if (!bPeek)
        {
            m_pData += nSize;
            m_nSize -= nSize;
        }

        return pData;
    }

    uint8_t pop_uint8(bool bPeek = false) const { return *(uint8_t*)pop_fetch_ptr(1, bPeek); }
    uint16_t pop_uint16(bool bPeek = false) const { uint16_t u16 = *(uint16_t*)pop_fetch_ptr(2, bPeek); return xntohs(u16); }
    uint32_t pop_uint32(bool bPeek = false) const { uint32_t u32 = *(uint32_t*)pop_fetch_ptr(4, bPeek); return xntohl(u32); }
    uint64_t pop_uint64(bool bPeek = false) const { uint64_t u64 = *(uint64_t*)pop_fetch_ptr(8, bPeek); return xntohll(u64); }

    const char * pop_string(size_t & nSize) const
    {
        nSize = pop_uint16();
        return pop_fetch_ptr(nSize);
    }
    const char * pop_string32(size_t & nSize) const
    {
        nSize = pop_uint32();
        return pop_fetch_ptr(nSize);
    }
    std::string pop_string() const
    {
        size_t nSize = pop_uint16();
        const char* pData = pop_fetch_ptr(nSize);
        return std::string(pData, nSize);
    }
    std::string pop_string32() const
    {
        size_t nSize = pop_uint32();
        const char* pData = pop_fetch_ptr(nSize);
        return std::string(pData, nSize);
    }

};

// 基础数据类型的序列化与反序列化 =>

inline SimplePack & operator << (SimplePack & p, bool b)
{
    p.push_uint8(b ? 1 : 0);
    return p;
}

inline SimplePack & operator << (SimplePack & p, uint8_t u8)
{
    p.push_uint8(u8);
    return p;
}

inline SimplePack & operator << (SimplePack & p, uint16_t u16)
{
    p.push_uint16(u16);
    return p;
}

inline SimplePack & operator << (SimplePack & p, uint32_t u32)
{
    p.push_uint32(u32);
    return p;
}

inline SimplePack & operator << (SimplePack & p, uint64_t u64)
{
    p.push_uint64(u64);
    return p;
}

inline SimplePack & operator << (SimplePack & p, int8_t i8)
{
    p.push_uint8((uint8_t)i8);
    return p;
}

inline SimplePack & operator << (SimplePack & p, int16_t i16)
{
    p.push_uint16((uint16_t)i16);
    return p;
}

inline SimplePack & operator << (SimplePack & p, int32_t i32)
{
    p.push_uint32((uint32_t)i32);
    return p;
}

inline SimplePack & operator << (SimplePack & p, int64_t i64)
{
    p.push_uint64((uint64_t)i64);
    return p;
}

inline SimplePack & operator << (SimplePack & p, const std::string & str)
{
    p.push_string(str.data(), str.size());
    return p;
}

inline SimplePack& operator << (SimplePack& p, const std::wstring& str)
{
    p.push_string(str.data(), str.size() * sizeof(wchar_t));
    return p;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, bool & b)
{
    b = (up.pop_uint8() == 0) ? false : true;
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, uint8_t & u8)
{
    u8 = up.pop_uint8();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, uint16_t & u16)
{
    u16 = up.pop_uint16();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, uint32_t & u32)
{
    u32 = up.pop_uint32();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, uint64_t & u64)
{
    u64 = up.pop_uint64();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, int8_t & i8)
{
    i8 = (int8_t)up.pop_uint8();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, int16_t & i16)
{
    i16 = (int16_t)up.pop_uint16();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, int32_t & i32)
{
    i32 = (int32_t)up.pop_uint32();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, int64_t & i64)
{
    i64 = (int64_t)up.pop_uint64();
    return up;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & up, std::string & str)
{
    size_t nSize = 0;
    const char* pData = up.pop_string(nSize);
    str.assign(pData, nSize);
    return up;
}

inline const SimpleUnpack& operator >> (const SimpleUnpack& up, std::wstring& str)
{
    size_t nSize = 0;
    const char* pData = up.pop_string(nSize);
    str.assign((const wchar_t*)pData, nSize / sizeof(wchar_t));
    return up;
}

// 容器类型的序列化与反序列化 =>

template < typename ContainerClass >
inline void marshal_container(SimplePack & p, const ContainerClass & c)
{
    p.push_uint32( uint32_t(c.size()) );
    for (typename ContainerClass::const_iterator i = c.begin(); i != c.end(); ++i)
    {
        p << *i;
    }
}

template < typename OutputIterator >
inline void unmarshal_container(const SimpleUnpack & up, OutputIterator i)
{
    for (uint32_t count = up.pop_uint32(); count > 0; --count)
    {
        typename OutputIterator::container_type::value_type tmp;
        up >> tmp;
        *i = tmp;
        ++i;
    }
}

template < typename OutputContainer >
inline void unmarshal_container2(const SimpleUnpack & p, OutputContainer & c)
{
    for (uint32_t count = p.pop_uint32(); count > 0; --count)
    {
        typename OutputContainer::value_type tmp;
        p >> tmp;
        c.push_back(tmp);
    }
}

template <class T1, class T2>
inline SimplePack & operator << (SimplePack & p, const std::pair<T1, T2> & pair)
{
    p << pair.first << pair.second;
    return p;
}

template <class T1, class T2>
inline const SimpleUnpack & operator >> (const SimpleUnpack & up, std::pair<const T1, T2> & pair)
{
    const T1 & m = pair.first;
    T1 & m2 = const_cast<T1 &>(m);
    up >> m2 >> pair.second;
    return up;
}

template <class T1, class T2>
inline const SimpleUnpack & operator >> (const SimpleUnpack & up, std::pair<T1, T2> & pair)
{
    up >> pair.first >> pair.second;
    return up;
}

template <class T>
inline SimplePack & operator << (SimplePack & p, const std::vector<T> & vec)
{
    marshal_container(p, vec);
    return p;
}

template <class T>
inline const SimpleUnpack & operator >> (const SimpleUnpack & up, std::vector<T> & vec)
{
    unmarshal_container(up, std::back_inserter(vec));
    return up;
}

template <class T>
inline SimplePack & operator << (SimplePack & p, const std::set<T> & set)
{
    marshal_container(p, set);
    return p;
}

template <class T>
inline const SimpleUnpack & operator >> (const SimpleUnpack & up, std::set<T> & set)
{
    unmarshal_container(up, std::inserter(set, set.begin()));
    return up;
}

template <class T1, class T2>
inline SimplePack & operator << (SimplePack & p, const std::map<T1, T2> & map)
{
    marshal_container(p, map);
    return p;
}

template <class T1, class T2>
inline const SimpleUnpack & operator >> (const SimpleUnpack & up, std::map<T1, T2> & map)
{
    unmarshal_container(up, std::inserter(map, map.begin()));
    return up;
}

// 结构类型的序列化与反序列化 =>

struct Marshallable
{
    virtual ~Marshallable() {}

    virtual void marshal(SimplePack &) const = 0;
    virtual void unmarshal(const SimpleUnpack &) = 0;
};

struct Voidmable : public Marshallable
{
    virtual void marshal(SimplePack &) const {}
    virtual void unmarshal(const SimpleUnpack &) {}
};

inline SimplePack & operator << (SimplePack & p, const Marshallable & m)
{
    m.marshal(p);
    return p;
}

inline const SimpleUnpack & operator >> (const SimpleUnpack & p, Marshallable & m)
{
    m.unmarshal(p);
    return p;
}

inline void Object2String(const Marshallable & obj, std::string & str)
{
    SimplePack pack;

    obj.marshal(pack);
    str.assign(pack.data(), pack.size());
}

inline bool String2Object(const std::string & str, Marshallable & obj)
{
    try
    {
        SimpleUnpack unpack(str.data(), str.size());

        obj.unmarshal(unpack);
    }
    catch (const std::runtime_error & e)
    {
        return false;
    }

    return true;
}

}

#endif // __SIMPLEMARSHAL_H__
