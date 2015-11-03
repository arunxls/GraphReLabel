#pragma once
#include "include.h"
#include "GraphReader.h"
#include "RenamedGraphManager.h"
#include "RenamedNodes.h"
#include <deque>

template <typename T>
class InvertAndRelabelNodes
{
public:
    uint64 total_read;
    uint64 total_write;

    std::deque<char*> output_files;
    char* nodesHash;

    GraphReader<T>* graph;
    RenamedGraphManager<T>* renamedGraphManager;
    RenamedNodes<T>* renamedNodes;

    InvertAndRelabelNodes(char* file_name, uint32 buffer_size, bool createNodeHash);
    ~InvertAndRelabelNodes();

    void execute();
    uint32 getRenamed(T element);
private:
    uint32 currentRenameCount;
    void split();
    void merge();
};