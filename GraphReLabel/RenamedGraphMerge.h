#pragma once
#include "include.h"
#include "FileWriter.h"
#include <deque>
#include "GraphReader.h"
#include <unordered_set>
#include <windows.h>

#define MERGE_WAY 4
#define SORT_THREADS 1

static HANDLE gNeighbourSortSemaphore;

template <typename T>
class RenamedGraphMerge
{
public:
    std::deque<char*>* output_files;
    std::deque<std::pair<uint32*, uint32*>> sortOffsets;
    uint32 buffer_size;

    uint64 total_read;
    uint64 total_write;

    bool sortNeighbour;

    RenamedGraphMerge();
    RenamedGraphMerge(std::deque<char*>*, uint32 buffer);
    ~RenamedGraphMerge();
    void execute();
    void put(char*& buffer_start, char*& buffer_end, char*& start, char*& prev, GraphReader<T, uint32>* graph, char*& output);
    void writeToDisk(char*& buffer_start, char*& start, char*& prev, char*& output);
private:
    GraphReader<T, uint32>* twoWayCompare(GraphReader<T, uint32>* graph1, GraphReader<T, uint32>* graph2);
    bool has_next(GraphReader<T, uint32>* graph[]);
    std::unordered_set<GraphReader<T, uint32>*> readers;
    static DWORD WINAPI RenamedGraphMerge<T>::sortList(LPVOID data);
    void sortAdjacencyList(char* prev, char* start);
    void compact();
};