#include "stdafx.h"
#include <algorithm>
#include "RenamedGraphMerge.h"

template <typename T>
RenamedGraphMerge<T>::RenamedGraphMerge()
{
    this->total_read = 0;
    this->total_write = 0;
    this->sortNeighbour = false;
}

template <typename T>
RenamedGraphMerge<T>::RenamedGraphMerge(std::deque<char*>* output_files, uint32 buffer)
{
    this->total_read = 0;
    this->total_write = 0;

    this->output_files = output_files;
    this->buffer_size = buffer;
    this->sortNeighbour = false;
}

template <typename T>
RenamedGraphMerge<T>::~RenamedGraphMerge()
{
}

template <typename T>
void RenamedGraphMerge<T>::execute()
{
    //DWORD   dwThreadIdArray[SORT_THREADS];
    //HANDLE  hThreadArray[SORT_THREADS];
    //
    //if (this->sortNeighbour)
    //{
    //    for (int i = 0; i < SORT_THREADS; ++i) {
    //        hThreadArray[i] = CreateThread(
    //            NULL,                   // default security attributes
    //            0,                      // use default stack size  
    //            sortList,         // thread function name
    //            &this->sortOffsets,  // argument to thread function 
    //            0,                      // use default creation flags 
    //            &dwThreadIdArray[i]);   // returns the thread identifier
    //    }
    //}

    uint32 buffer = this->buffer_size;
    
    buffer -= std::min<int>(MERGE_WAY, this->output_files->size()) * GRAPH_READ_BUFFER*_1_MB;
    
    char* buffer_start = new char[buffer];
    char* buffer_end = buffer_start + buffer;
    char* start = buffer_start;
    char* prev = start;

    while (this->output_files->size() > 1) 
    {
        char* files[MERGE_WAY];
        GraphReader<T, uint32>* graph[MERGE_WAY];
        this->readers.clear();
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
                graph[i] = new GraphReader<T, uint32>(files[i], &this->readers);
                graph[i]->load();
            }
        }

        char* output;
        if (this->sortNeighbour) {
            output = "PLD-out-relabeled.dat";
        }
        else {
            output = getNewOutputFile();
        }

        while (this->has_next(graph)) 
        {
            GraphReader<T, uint32>* min = graph[0];
            for (int i = 1; i < MERGE_WAY; ++i)
            {
                min = twoWayCompare(min, graph[i]);
            }

            put(buffer_start, buffer_end, start, prev, min, output);
        }

        this->writeToDisk(buffer_start, start, prev, output);
        this->output_files->push_back(output);

        //for (int i = 0; i < SORT_THREADS; ++i)
        //{
        //    CloseHandle(hThreadArray[i]);
        //}

        for (int i = 0; i < MERGE_WAY; ++i) 
        {
            if (graph[i] != NULL)
            {
                this->total_read += graph[i]->total_read;
                delete graph[i];
                DeleteFile(TEXT(files[i]));
            }
        }
    }

    delete[] buffer_start;
    return;
}

template<typename T>
void RenamedGraphMerge<T>::put(char *& buffer_start, char*& buffer_end, char *& start, char*& prev, GraphReader<T, uint32>* graph, char*& output)
{
    HeaderGraph<T, uint32> h = graph->currentHeader();

    uint32 sizeToCopy = h.size();

    if (start + sizeToCopy > buffer_end)
    {
        this->writeToDisk(buffer_start, start, prev, output);
    }

    ((HeaderGraph<T, uint32>*) start)->hash = h.hash;
    ((HeaderGraph<T, uint32>*) start)->len = h.len;

    start += sizeof(HeaderGraph<T, uint32>);

    graph->copyRange(start);

    return;
}

template<typename T>
void RenamedGraphMerge<T>::writeToDisk(char *& buffer_start, char *& start, char*& prev, char*& output)
{
    this->compact(buffer_start, start);
    WaitForSingleObject(gWriteFileSemaphone, INFINITE);
    {
        FileWriter FW(output);
        FW.write(buffer_start, (start - buffer_start));
        this->total_write += (start - buffer_start);
        start = buffer_start;
        prev = start;
    }
    ReleaseSemaphore(gWriteFileSemaphone, 1, NULL);
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

template<typename T>
DWORD RenamedGraphMerge<T>::sortList(LPVOID data)
{
    Sleep(500);
    std::deque<std::pair<uint32*, uint32*>>* sortOffsets = (std::deque<std::pair<uint32*, uint32*>>*) data;
    DWORD dwWaitResult;
    while (1)
    {
        if (sortOffsets->size() != 0)
        {
            dwWaitResult = WaitForSingleObject(gNeighbourSortSemaphore, 0);
            if (dwWaitResult == WAIT_OBJECT_0) {
                if (sortOffsets->size() != 0) {
                    printf("HERE1!");
                    std::pair<uint32*, uint32*> bounds = sortOffsets->front();
                    printf("HERE2!");
                    sortOffsets->pop_front();

                    bool flag = ReleaseSemaphore(gNeighbourSortSemaphore, 1, NULL);
                    if (DEBUG && !flag)
                    {
                        DisplayError(TEXT("SetEvent"));
                    }

                    uint32* start = bounds.first;
                    uint32* end = bounds.second;

                    std::sort(start, end);
                    printf("SORTED %d %p, %p\n", sortOffsets->size(), start, end);
                    //delete[] bounds;
                }
                else {
                    Sleep(100);
                }
            }
            else {
                ReleaseSemaphore(gNeighbourSortSemaphore, 1, NULL);
            }
        }
        //printf("!!SORTED %d\n", sortOffsets->size());
    }
    
    return 0;
}

template<typename T>
void RenamedGraphMerge<T>::sortAdjacencyList(char * prev, char * start)
{
    
    //std::pair<uint32*, uint32*> foo;
    //foo.first = (uint32*)(prev + sizeof(HeaderGraph<T, uint32>));
    //foo.second = (uint32*)start;
    if (this->sortNeighbour) {
        std::sort((uint32*)prev, (uint32*)start);
    }

    //while (true) {
    //    if (this->sortOffsets.size() < 10000)
    //    {
    //        //printf("INSERT!\n");
    //        WaitForSingleObject(gNeighbourSortSemaphore, INFINITY);
    //        this->sortOffsets.emplace_back(foo);
    //        ReleaseSemaphore(gNeighbourSortSemaphore, 1, NULL);
    //        return;
    //    }
    //    else {
    //        //printf("FULL!\n");
    //    }
    //}
}

template<typename T>
void RenamedGraphMerge<T>::compact(char*& buffer_start, char*& start)
{
    HeaderGraph<T, uint32>* prev = (HeaderGraph<T, uint32>*) buffer_start;
    HeaderGraph<T, uint32>* current = (HeaderGraph<T, uint32>*) (((char*)prev) + prev->size());
    while ((char*)current < start)
    {
        uint32 current_size = current->size();
        uint32 prev_size = prev->size();

        if (*current == *prev) {
            *prev += *current;
        }
        else {
            this->sortAdjacencyList((char*)(prev+1), (((char*)prev) + prev_size));
            prev = (HeaderGraph<T, uint32>*) (((char*)prev) + prev_size);
            if (prev < current) {
                *prev = *current;
            }
        }

        current = (HeaderGraph<T, uint32>*) (((char*)current) + current_size);
    }

    start = (char*)(((char*)prev) + prev->size());
}
