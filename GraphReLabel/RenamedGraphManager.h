#pragma once
#include "include.h"
#include "RenamedGraph.h"

#define RENAME_BUCKETS 4

template <typename T>
class RenamedGraphManager
{
public:
    RenamedGraph<T>* bucket;

    RenamedGraphManager(uint32 buffer_size);
    RenamedGraphManager();
    ~RenamedGraphManager();

    void put(uint32 renamed, T original);
};

