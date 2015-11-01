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

template <typename T1, typename T2>
class RenamedHeaderGraph {
    T1 old;
    uint32 len;
    T2 renamed;
};
#pragma pack(pop)
