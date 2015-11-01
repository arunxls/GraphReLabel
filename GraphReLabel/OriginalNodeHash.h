#pragma once

#include "include.h"
#include "FileWriter.h"

#define ORIGINAL_NODE_HASH 2

template <typename T>
class OriginalNodeHash
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    uint64 total_read;
    uint64 total_write;

    FileWriter* FW;

    OriginalNodeHash();
    ~OriginalNodeHash();

    void put(T element);
    void writeToDisk();

    uint32 size() { return ORIGINAL_NODE_HASH; }
};

