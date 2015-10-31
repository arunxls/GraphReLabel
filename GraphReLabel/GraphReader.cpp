#include "stdafx.h"
#include "GraphReader.h"
#include "OriginalNodeHash.cpp"

template <typename T1, typename T2>
GraphReader<T1, T2>::GraphReader(char * file_name, bool createNodeHash) 
{
    this->nodeHash = createNodeHash ? new OriginalNodeHash<T1>() : NULL;
    this->FR = new FileReader(file_name);

    //Save some extra buffer space for partial reads
    this->buffer_start = new char[GRAPH_READ_BUFFER*_1_MB + sizeof(OriginalHeaderGraph<T1>)];
    this->buffer_start = this->buffer_start + sizeof(OriginalHeaderGraph<T1>);
    this->buffer_end = this->buffer_start + GRAPH_READ_BUFFER*_1_MB;
    this->start = this->buffer_start;
    this->buffer_end = this->buffer_start;
}

template <typename T1, typename T2>
GraphReader<T1, T2>::GraphReader()
{
}

template <typename T1, typename T2>
GraphReader<T1, T2>::~GraphReader()
{
    delete[](this->buffer_start - sizeof(OriginalHeaderGraph<T1>));
}

template<typename T1, typename T2>
bool GraphReader<T1, T2>::has_next()
{
    return false;
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::next()
{
    
}

template<typename T1, typename T2>
T2 GraphReader<T1, T2>::current()
{
    return T2();
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::load()
{
}
