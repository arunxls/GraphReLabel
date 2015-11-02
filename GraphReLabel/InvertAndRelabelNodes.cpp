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
    while (this->graph->has_next()) {
        T element = this->graph->current();
        uint32 renamed = this->getRenamed(element);
        this->renamedGraphManager->put(renamed, element);
        this->graph->next();
    }

    printf("%I64u\n", this->graph->count);

    //renamedGraphManager->dumpToNewFile();
}

template <typename T>
uint32 InvertAndRelabelNodes<T>::getRenamed(T element)
{
    return this->currentRenameCount++;
}
