#pragma once
#include "include.h"
#include "FileWriter.h"
#include <deque>
#include "GraphReader.h"

#define MERGE_WAY 4

template <typename T>
class RenamedGraphMerge
{
public:
    std::deque<char*>* output_files;
    uint32 buffer_size;

    RenamedGraphMerge();
    RenamedGraphMerge(std::deque<char*>*, uint32 buffer);
    ~RenamedGraphMerge();
    void execute();
    void put(char*& buffer_start, char*& buffer_end, char*& start, char*& prev, GraphReader<T, uint32>* graph, FileWriter& FW);
    void writeToDisk(char*& buffer_start, char*& start, char*& prev, FileWriter& FW);
private:
    GraphReader<T, uint32>* twoWayCompare(GraphReader<T, uint32>* graph1, GraphReader<T, uint32>* graph2);
    bool has_next(GraphReader<T, uint32>* graph[]);
};