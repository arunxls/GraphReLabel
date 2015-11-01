#include "stdafx.h"
#include "GraphReader.h"
#include "OriginalNodeHash.cpp"

template <typename T1, typename T2>
GraphReader<T1, T2>::GraphReader(char * file_name, bool createNodeHash) 
{
    this->nodeHash = createNodeHash ? new OriginalNodeHash<T1>() : NULL;
    this->FR = new FileReader(file_name);

    this->length = 0;

    //Save some extra buffer space for partial reads
    uint32 extra_buffer = sizeof(OriginalHeaderGraph<T1>) + sizeof(T2);
    this->buffer_start = new char[GRAPH_READ_BUFFER*_1_MB + extra_buffer];
    
    this->buffer_start = this->buffer_start + extra_buffer;
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
    delete[](this->buffer_start - sizeof(OriginalHeaderGraph<T1>));
}

template<typename T1, typename T2>
bool GraphReader<T1, T2>::has_next()
{
    return this->FR->has_next() ? true : this->start < this->end;
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::next()
{
    this->length--;
    this->start += sizeof(T2);
}

template<typename T1, typename T2>
T2 GraphReader<T1, T2>::current()
{
    if (this->start == this->end)
    {
        this->load();
        this->start = this->buffer_start;
    }

    uint32 sizeToRead = this->length == 0 ? sizeof(OriginalHeaderGraph<T1>) + sizeof(T2) : sizeof(T2);

    if (this->start + sizeToRead > this->end)
    {
        uint32 bytesToCopy = this->end - this->start;
        char* dst = this->buffer_start - bytesToCopy;
        memcpy(dst, this->start, bytesToCopy);
        this->start = dst;
        this->load();
    }

    if (this->length == 0)
    {
        this->nodeHash->put(((OriginalHeaderGraph<T1>*) this->start)->hash);
        this->length = ((OriginalHeaderGraph<T1>*) this->start)->len;
        this->start += sizeof(OriginalHeaderGraph<T1>);
    }

    return *(T2*) this->start;
}

template<typename T1, typename T2>
void GraphReader<T1, T2>::load()
{
    uint32 bytesTransferred = 0;
    this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);
    this->end = this->buffer_start + bytesTransferred;
}
