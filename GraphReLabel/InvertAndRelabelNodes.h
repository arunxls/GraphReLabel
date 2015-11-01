#pragma once
#include "include.h"
#include "GraphReader.h"
#include "RenamedGraphManager.h"

template <typename T1, typename T2>
class InvertAndRelabelNodes
{
public:
    uint64 total_read;
    uint64 total_write;

    GraphReader<T1, T2>* graph;
    RenamedGraphManager* renamedGraphManager;

    InvertAndRelabelNodes(char* file_name, uint32 buffer_size, bool createNodeHash);
    ~InvertAndRelabelNodes();

    void execute();
};