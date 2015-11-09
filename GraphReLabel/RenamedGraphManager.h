#pragma once
#include "include.h"
#include "RenamedGraph.h"
#include <deque>

#define RENAME_BUCKETS 8

static HANDLE ghSortSemaphore;
static HANDLE ghWriteSemaphore;

template <typename T>
class RenamedGraphManager
{
public:
    RenamedGraph<T>* bucket;

    uint64 total_write;
    uint64 total_read;
    uint32 rangeKey;

    std::deque<char*> output_files;

    RenamedGraphManager(uint32 buffer_size);
    RenamedGraphManager();
    ~RenamedGraphManager();

    static DWORD WINAPI RenamedGraphManager<T>::sortAndCompact(LPVOID data);
    void put(uint32 renamed, T original);
    void writeToDisk();
private:
    HANDLE  hThreadArray[RENAME_BUCKETS];
    void init_threads();
};

