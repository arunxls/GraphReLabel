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

    this->graph = new GraphReader<T>(file_name, createNodeHash);
    buffer_size -= graph->size();

    this->currentRenameCount = 0;
    this->renamedGraphManager = new RenamedGraphManager<T>(buffer_size);
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
uint32 InvertAndRelabelNodes<T>::getRenamed(T element)
{
    return this->currentRenameCount++;
}

template<typename T>
void InvertAndRelabelNodes<T>::split()
{
    while (this->graph->has_next()) {
        T element = this->graph->current();
        uint32 renamed = this->getRenamed(element);
        this->renamedGraphManager->put(renamed, element);
        this->graph->next();
    }

    this->renamedGraphManager->writeToDisk();
    this->graph->nodeHash->writeToDisk();

    this->output_files = std::move(this->renamedGraphManager->output_files);
    this->nodesHash = this->graph->nodeHash->FW->filename;
}

template<typename T>
void InvertAndRelabelNodes<T>::merge()
{
    for (int i = 0; i < 64; i++) {
        this->output_files.push_back(getNewOutputFile());
    }



    printf("\n");
}
