#include "stdafx.h"
#include "GraphHeaderReader.h"

template <typename T>
GraphHeaderReader<T>::GraphHeaderReader()
{
}

template<typename T>
GraphHeaderReader<T>::GraphHeaderReader(char * filename)
{
    this->FR = new FileReader(filename);

    this->length = 0;

    //Save some extra buffer space for partial reads
    uint32 extra_buffer = sizeof(HeaderGraph<T>);
    this->buffer_start = new char[GRAPH_HEADER_READ_BUFFER*_1_MB + extra_buffer];

    this->buffer_start = this->buffer_start + extra_buffer;
    this->buffer_end = this->buffer_start + GRAPH_HEADER_READ_BUFFER*_1_MB;

    this->header_start = this->buffer_start;
    this->neighbour_start = this->header_start + sizeof(HeaderGraph<T>);
    this->end = this->buffer_start;
}


template <typename T>
GraphHeaderReader<T>::~GraphHeaderReader()
{
}

template<typename T>
HeaderGraph<T> GraphHeaderReader<T>::currentHeader()
{
    if (this->header_start == this->end)
    {
        this->load();
        this->header_start = this->buffer_start;
    }

    uint32 sizeToRead = sizeof(HeaderGraph<T>);

    if (this->header_start + sizeToRead > this->end)
    {
        uint32 bytesToCopy = this->end - this->header_start;
        char* dst = this->buffer_start - bytesToCopy;
        memcpy(dst, this->header_start, bytesToCopy);
        this->header_start = dst;
        this->load();
    }

    return *(HeaderGraph<T>*) this->header_start;
}

template<typename T>
uint32 GraphHeaderReader<T>::currentNeighbour()
{
    return uint32();
}

template<typename T>
void GraphHeaderReader<T>::nextHeader()
{
}

template<typename T>
void GraphHeaderReader<T>::nextNeighbour()
{
}

template<typename T>
bool GraphHeaderReader<T>::hasNextHeader()
{
    return this->FR->has_next() ? true : this->header_start < this->end;
}

template<typename T>
bool GraphHeaderReader<T>::hasNextNeighbour()
{
    return this->FR->has_next() ? true : this->neighbour_start < this->end;
}


template<typename T>
void GraphHeaderReader<T>::load()
{
    uint32 bytesTransferred = 0;
    this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);
    this->end = this->buffer_start + bytesTransferred;
}
