#pragma once

#include "include.h"
#include "FileReader.h"
#include "OriginalNodeHash.h"

#define GRAPH_READ_BUFFER 32

template <typename T1, typename T2>
class GraphReader
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    bool createNodeHash;
    FileReader* FR;
    OriginalNodeHash *nodeHash;

    GraphReader(char * file_name, bool createNodeHash);
    GraphReader();
    ~GraphReader();

    uint32 size() { return (this->createNodeHash ? this->nodeHash->size() + GRAPH_READ_BUFFER : GRAPH_READ_BUFFER)*_1_MB; }
    bool has_next();
    HeaderGraph<T1, T2> currentHeader();
    T2 currentNeighbour();
    void nextHeader();
    void nextNeighbour();
    uint32 remainingBuffer();
    void copyRange(char* dst);
    void load();
private:
    char* alloc_start;
};

