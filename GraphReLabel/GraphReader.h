#pragma once

#include "include.h"
#include "FileReader.h"
#include "OriginalNodeHash.h"

#define ORIGINAL_NODE_HASH 2
#define GRAPH_READ_BUFFER 32

template <typename T1, typename T2>
class GraphReader
{
public:
    bool createNodeHash;
    FileReader* FR;
    OriginalNodeHash<T1> *nodeHash;

    GraphReader(char * file_name, bool createNodeHash);
    GraphReader();
    ~GraphReader();

    uint32 size() { return this->createNodeHash ? ORIGINAL_NODE_HASH + GRAPH_READ_BUFFER : GRAPH_READ_BUFFER; }
};

