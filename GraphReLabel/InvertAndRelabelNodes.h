#pragma once
#include "include.h"
#include "GraphReader.h"
#include "HashManager.h"

#define ORIGINAL_GRAPH_READER_SIZE 34

template <typename T1, typename T2>
class InvertAndRelabelNodes
{
public:
    uint32 total_read;
    uint32 total_write;

    GraphReader<T1, T2>* graph;
    //HashManager* hashManager;

    InvertAndRelabelNodes(char* file_name, uint32 buffer_size, bool createNodeHash);
    ~InvertAndRelabelNodes();

    void execute();
};