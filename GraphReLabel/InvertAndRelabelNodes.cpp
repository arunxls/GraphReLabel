#include "stdafx.h"
#include "InvertAndRelabelNodes.h"
#include "GraphReader.cpp"

template <typename T1, typename T2>
InvertAndRelabelNodes<T1, T2>::InvertAndRelabelNodes(char * file_name, uint32 buffer_size, bool createNodeHash)
{
    this->total_read = 0;
    this->total_write = 0;

    this->graph = new GraphReader<T1, T2>(file_name, createNodeHash);
    buffer_size -= graph->size();

}

template <typename T1, typename T2>
InvertAndRelabelNodes<T1, T2>::~InvertAndRelabelNodes()
{
}

template <typename T1, typename T2>
void InvertAndRelabelNodes<T1, T2>::execute()
{
    while (this->graph->has_next()) {
        //hashes->put(graph->current());
        this->graph->current();
        this->graph->next();
    }

    //hashes->dumpToNewFile();
}
