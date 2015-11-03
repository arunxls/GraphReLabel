#include "stdafx.h"
#include "InvertAndRelabelNodes.h"
#include "GraphReader.cpp"
#include "RenamedNodes.cpp"
#include "RenamedGraphManager.cpp"

template <typename T>
InvertAndRelabelNodes<T>::InvertAndRelabelNodes(char * file_name, uint32 buffer_size, bool createNodeHash)
{
    this->total_read = 0;
    this->total_write = 0;
}

template <typename T>
InvertAndRelabelNodes<T>::~InvertAndRelabelNodes()
{
}

template <typename T>
void InvertAndRelabelNodes<T>::execute()
{
    //this->split();
    this->merge();
}

template <typename T>
uint32 InvertAndRelabelNodes<T>::getRenamed(T element, uint32& currentRenameCount)
{
    return currentRenameCount++;
}

template<typename T>
void InvertAndRelabelNodes<T>::split()
{
    GraphReader<T>* graph = new GraphReader<T>(file_name, createNodeHash);
    buffer_size -= graph->size();

    RenamedGraphManager<T>* renamedGraphManager = new RenamedGraphManager<T>(buffer_size);
    
    uint32 currentRenameCount = 0;
    
    while (graph->has_next()) {
        T element = graph->current();
        uint32 renamed = this->getRenamed(element, currrentRenameCount);
        renamedGraphManager->put(renamed, element);
        graph->next();
    }

    renamedGraphManager->writeToDisk();
    graph->nodeHash->writeToDisk();

    this->output_files = std::move(renamedGraphManager->output_files);
    this->nodesHash = graph->nodeHash->FW->filename;
}

template<typename T>
void InvertAndRelabelNodes<T>::merge()
{
    for (int i = 0; i < 64; i++) {
        this->output_files.push_back(getNewOutputFile());
    }



    printf("\n");
}
