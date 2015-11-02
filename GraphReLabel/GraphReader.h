#pragma once

#include "include.h"
#include "FileReader.h"
#include "OriginalNodeHash.h"

#define GRAPH_READ_BUFFER 32

template <typename T>
class GraphReader
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    uint64 count = 0;

    bool createNodeHash;
    FileReader* FR;
    OriginalNodeHash *nodeHash;

    GraphReader(char * file_name, bool createNodeHash);
    GraphReader();
    ~GraphReader();

    uint32 size() { return (this->createNodeHash ? this->nodeHash->size() + GRAPH_READ_BUFFER : GRAPH_READ_BUFFER)*_1_MB; }
    bool has_next();
    void next();
    T current();
private:
    uint32 length;
    void load();
};

