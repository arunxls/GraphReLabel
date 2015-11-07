#pragma once

#include "include.h"
#include "FileReader.h"

#define GRAPH_HEADER_READ_BUFFER 32

template <typename T>
class GraphHeaderReader
{
public:
    char* buffer_start;
    char* buffer_end;

    char* header_start;
    char* neighbour_start;

    char* end;

    FileReader* FR;

    GraphHeaderReader();
    GraphHeaderReader(char* filename);
    ~GraphHeaderReader();

    HeaderGraph<T> currentHeader();
    uint32 currentNeighbour();
    void nextHeader();
    void nextNeighbour();
    bool hasNextHeader();
    bool hasNextNeighbour();
private:
    uint32 length;
    void load();
};

