#pragma once

#include "include.h"
#include "FileWriter.h"
#include "FileReader.h"

#define ORIGINAL_NODE_HASH_WRITER 2
#define ORIGINAL_NODE_HASH_READER 8

class OriginalNodeHash
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    uint64 total_read;
    uint64 total_write;
    
    FileReader* FR;
    FileWriter* FW;
    
    uint32 overshoot;

    OriginalNodeHash(char* file_name);
    OriginalNodeHash();
    ~OriginalNodeHash();

    void put(uint64 element);
    uint32 get(uint64 element);
    void writeToDisk();

    uint32 size() { return this->buffer_end - this->buffer_start; }
private:
    void load();
    uint32 position;
};

