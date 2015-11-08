#pragma once

#include "include.h"
#include "FileReader.h"

#define GRAPH_READ_BUFFER 16

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

    GraphReader(char * file_name);
    GraphReader();
    ~GraphReader();

    uint32 size() { return GRAPH_READ_BUFFER*_1_MB; }
    bool has_next();
    HeaderGraph<T1, T2> currentHeader();
    T2 currentNeighbour();
    void nextHeader();
    void nextNeighbour();
    uint32 remainingBuffer();
    void copyRange(char*& dst);
    void load();
private:
    char* alloc_start;
};

