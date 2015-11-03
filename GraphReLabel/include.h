#pragma once

#define _1_MB 1000000
#define _1_GB 1000000000
#define DEBUG 0

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef __int32 int32;
typedef unsigned short   ushort;

#pragma pack(push,1) // change struct packing to 1 byte
template <typename T>
class HeaderGraph {
public:
    T hash;
    uint32 len;

    bool operator==(const HeaderGraph<T>& a) const
    {
        return hash == a.hash;
    }

    HeaderGraph<T>& operator+=(const HeaderGraph<T>& rhs)
    {
        uint32 tmp_len = rhs.len;
        uint32* dst = ((uint32*)&this->len) + this->len + 1;
        uint32* src = (uint32*)(&rhs + 1);
        memcpy(dst, src, tmp_len*sizeof(uint32));
        this->len += tmp_len;
        return *this;
    }

    uint32 size() {
        return sizeof(HeaderGraph) + sizeof(uint32)*this->len;
    }

    HeaderGraph<T>& operator=(const HeaderGraph<T>& rhs)
    {
        hash = rhs.hash;
        len = rhs.len;
        return *this;
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
