#pragma once

#include "include.h"
#include "FileWriter.h"

#define ORIGINAL_NODE_HASH 2

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

    void put(uint64 element);
    void writeToDisk();

    uint32 size() { return ORIGINAL_NODE_HASH; }
};

