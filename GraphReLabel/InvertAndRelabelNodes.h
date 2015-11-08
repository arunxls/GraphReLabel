#pragma once
#include "include.h"
#include "GraphReader.h"
#include "RenamedGraphManager.h"
#include "RenamedGraphMerge.h"
#include "OriginalNodeHash.h"
#include <deque>

template <typename T>
class InvertAndRelabelNodes
{
public:
    uint64 total_read;
    uint64 total_write;

    std::deque<char*> output_files;
    char* nodesHash;
    char* file_name;
    bool createNodeHash;
    uint32 buffer;

    InvertAndRelabelNodes(char* file_name, uint32 buffer_size, bool createNodeHash);
    ~InvertAndRelabelNodes();

    void execute();
    void getRenamed(uint64& element, uint32& currentRenameCount, OriginalNodeHash*);
    static DWORD WINAPI InvertAndRelabelNodes<T>::mergeExecute(LPVOID data);
private:
    void split();
    void merge();
};