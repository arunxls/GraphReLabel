#pragma once
#include "include.h"

#define RENAME_BUCKETS 4

template <typename T1, typename T2>
class RenamedGraphManager
{
public:
    RenamedGraphManager(uint32 buffer_size);
    RenamedGraphManager();
    ~RenamedGraphManager();

    void put(T2 renamed, T1 original);
};

