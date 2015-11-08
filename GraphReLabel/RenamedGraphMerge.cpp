#include "stdafx.h"
#include "RenamedGraphMerge.h"

template <typename T>
RenamedGraphMerge<T>::RenamedGraphMerge()
{
}

template <typename T>
RenamedGraphMerge<T>::RenamedGraphMerge(std::deque<char*>* output_files, uint32 buffer)
{
    this->output_files = output_files;
    this->buffer_size = buffer;
}

template <typename T>
RenamedGraphMerge<T>::~RenamedGraphMerge()
{
}

template <typename T>
void RenamedGraphMerge<T>::execute()
{
    uint32 buffer = this->buffer_size;
    
    buffer -= std::min(MERGE_WAY, this->output_files->size()) * GRAPH_READ_BUFFER*_1_MB;
    
    char* buffer_start = new char[buffer];
    char* buffer_end = buffer_start + buffer;
    char* start = buffer_start;
    char* prev = start;

    while (this->output_files->size() > 1) 
    {
        char* files[MERGE_WAY];
        GraphReader<T, uint32>* graph[MERGE_WAY];
        for (int i = 0; i < MERGE_WAY; ++i)
        {
            files[i] = NULL;
            if (this->output_files->size() > 0)
            {
                files[i] = this->output_files->front();
                this->output_files->pop_front();
            }

            graph[i] = NULL;
            if (files[i] != NULL)
            {
                graph[i] = new GraphReader<T, uint32>(files[i]);
                graph[i]->load();
            }
        }

        char* output = getNewOutputFile();
        FileWriter FW(output);

        while (this->has_next(graph)) 
        {
            GraphReader<T, uint32>* min = graph[0];
            for (int i = 1; i < MERGE_WAY; ++i)
            {
                min = twoWayCompare(min, graph[i]);
            }

            put(buffer_start, buffer_end, start, prev, min, FW);
        }

        this->writeToDisk(buffer_start, start, prev, FW);
        this->output_files->push_back(output);

        //this->total_read = this->read_1->total_read + this->read_2->total_read + this->write_merged->total_read;
        //this->total_write = this->read_1->total_write + this->read_2->total_write + this->write_merged->total_write;

        for (int i = 0; i < MERGE_WAY; ++i) 
        {
            if (graph[i] != NULL)
            {
                delete graph[i];
                DeleteFile(TEXT(files[i]));
            }
        }
    }

    delete[] buffer_start;
    return;
}

template<typename T>
void RenamedGraphMerge<T>::put(char *& buffer_start, char*& buffer_end, char *& start, char*& prev, GraphReader<T, uint32>* graph, FileWriter & FW)
{
    HeaderGraph<T, uint32> h = graph->currentHeader();
    uint32 sizeToCopy = sizeof(HeaderGraph<T, uint32>) + h.len*sizeof(uint32);
    
    //printf("%I64u - %I32u\n", h.hash, h.len);
    if (start + sizeToCopy >= buffer_end)
    {
        this->writeToDisk(buffer_start, start, prev, FW);
    }

    if (start == buffer_start)
    {
        ((HeaderGraph<T, uint32>*) start)->hash = h.hash;
        ((HeaderGraph<T, uint32>*) start)->len = 0;
        start += sizeof(HeaderGraph<T, uint32>);
    }

    if (*(HeaderGraph<T, uint32>*) prev == h) 
    {
        ((HeaderGraph<T, uint32>*) prev)->len += h.len;
    }
    else 
    {
        *((HeaderGraph<T, uint32>*) start) = h;
        prev = start;
        start += sizeof(HeaderGraph<T, uint32>);
    }

    graph->copyRange(start);
}

template<typename T>
void RenamedGraphMerge<T>::writeToDisk(char *& buffer_start, char *& start, char*& prev, FileWriter & FW)
{
    FW.write(buffer_start, (start - buffer_start));
    start = buffer_start;
    prev = start;
}

template<typename T>
GraphReader<T, uint32>* RenamedGraphMerge<T>::twoWayCompare(GraphReader<T, uint32>* graph1, GraphReader<T, uint32>* graph2)
{
    if (graph1 == NULL) {
        return graph2;
    }
    else if (graph2 == NULL) {
        return graph1;
    }

    if (graph1->has_next() && graph2->has_next()) {
        HeaderGraph<T, uint32> h1 = graph1->currentHeader();
        HeaderGraph<T, uint32> h2 = graph2->currentHeader();

        if (h1 < h2) {
            return graph1;
        }
        else {
            return graph2;
        }
    }
    else if (graph1->has_next()) {
        return graph1;
    }
    else {
        return graph2;
    }
}

template<typename T>
bool RenamedGraphMerge<T>::has_next(GraphReader<T, uint32>* graph[])
{
    for (int i = 0; i < MERGE_WAY; ++i)
    {
        if (graph[i] != NULL && graph[i]->has_next()) return true;
    }

    return false;
}

