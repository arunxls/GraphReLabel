#include "stdafx.h"
#include "GraphReader.h"

template <typename T1, typename T2>
GraphReader<T1, T2>::GraphReader(char * file_name) 
{
    this->createNodeHash = createNodeHash;
    this->FR = new FileReader(file_name);

    //Save some extra buffer space for partial reads
    uint32 extra_buffer = sizeof(HeaderGraph<T1, T2>);
    
    this->alloc_start = new char[GRAPH_READ_BUFFER*_1_MB + extra_buffer];

    
    this->buffer_start = this->alloc_start + extra_buffer;
    this->buffer_end = this->buffer_start + GRAPH_READ_BUFFER*_1_MB;
    
    this->start = this->buffer_start;
    this->end = this->buffer_start;
}

template <typename T1, typename T2>
GraphReader<T1, T2>::GraphReader()
{
}

template <typename T1, typename T2>
GraphReader<T1, T2>::~GraphReader()
{
    delete[] this->alloc_start;
}

template<typename T1, typename T2>
bool GraphReader<T1, T2>::has_next()
{
    return this->FR->has_next() ? true : this->start < this->end;
}

template<typename T1, typename T2>
HeaderGraph<T1, T2> GraphReader<T1, T2>::currentHeader()
{
    uint32 sizeToRead = sizeof(HeaderGraph<T1, T2>);

    if (this->start + sizeToRead > this->end)
    {
        uint32 bytesToCopy = this->end - this->start;
        char* dst = this->buffer_start - bytesToCopy;
        memcpy(dst, this->start, bytesToCopy);
        this->start = dst;
        this->load();
    }

    return *(HeaderGraph<T1, T2>*) this->start;
}

template<typename T1, typename T2>
T2 GraphReader<T1, T2>::currentNeighbour()
{
    uint32 sizeToRead = sizeof(T2);

    if (this->start + sizeToRead > this->end)
    {
        uint32 bytesToCopy = this->end - this->start;
        char* dst = this->buffer_start - bytesToCopy;
        memcpy(dst, this->start, bytesToCopy);
        this->start = dst;
        this->load();
    }

    return *(T2*) this->start;
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::nextHeader()
{
    this->start += sizeof(HeaderGraph<T1, T2>);
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::nextNeighbour()
{
    this->start += sizeof(T2);
}

template<typename T1, typename T2>
uint32 GraphReader<T1, T2>::remainingBuffer()
{
    return (this->end - this->start);
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::load()
{
    uint32 bytesTransferred = 0;
    this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);
    this->end = this->buffer_start + bytesTransferred;
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::copyRange(char * dst)
{
    this->start -= sizeof(HeaderGraph<T1, T2>);
    uint32 sizeToCopy = this->currentHeader*sizeof(T2);
    if (this->start + sizeToCopy > this->end)
    {
        //memcopy(dst, )
    }
}
