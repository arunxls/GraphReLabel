#pragma once
#include "include.h"
#include "FileWriter.h"
#include <deque>
#include "GraphReader.h"

template <typename T>
class RenamedGraphMerge
{
public:
    std::deque<char*> output_files;
    uint32 buffer_size;

    RenamedGraphMerge();
    RenamedGraphMerge(std::deque<char*>&, uint32 buffer);
    ~RenamedGraphMerge();
    void execute();
    void put(char*& buffer_start, char*& buffer_end, char*& start, GraphReader<T, uint32>& graph, FileWriter& FW);
};