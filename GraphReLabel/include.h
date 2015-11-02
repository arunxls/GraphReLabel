#pragma once

#define _1_MB 1000000
#define _1_GB 1000000000
#define DEBUG 1

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
