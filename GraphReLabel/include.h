#pragma once

#define _1_MB 1000000
#define _1_GB 1000000000
#define DEBUG 1

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef __int32 int32;
typedef unsigned short   ushort;

#pragma pack(push,1) // change struct packing to 1 byte
template <typename T1, typename T2>
class HeaderGraph {
public:
    T1 hash;
    uint32 len;

    bool operator==(const HeaderGraph<T1, T2>& a) const
    {
        return hash == a.hash;
    }

    HeaderGraph<T1, T2>& operator+=(const HeaderGraph<T1, T2>& rhs)
    {
        uint32 tmp_len = rhs.len;
        T2* dst = ((T2*)&this->len) + this->len + 1;
        T2* src = (T2*)(&rhs + 1);
        memcpy(dst, src, tmp_len*sizeof(T2));
        this->len += tmp_len;
        return *this;
    }

    uint32 size() {
        return sizeof(HeaderGraph) + sizeof(T2)*this->len;
    }

    HeaderGraph<T1, T2>& operator=(const HeaderGraph<T1, T2>& rhs)
    {
        hash = rhs.hash;
        len = rhs.len;
        return *this;
    }

    bool operator<(const HeaderGraph<T1, T2>& rhs)
    {
        return hash < rhs.hash;
    }
};

template <typename T>
class RenamedHeaderGraph {
public:
    T old;
    uint32 len;
    uint32 renamed;

    bool operator<(const RenamedHeaderGraph<T>& a) const
    {
        return old < a.old;
    }
};
#pragma pack(pop)
