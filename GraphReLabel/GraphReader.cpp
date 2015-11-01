#include "stdafx.h"
#include "GraphReader.h"

template <typename T>
GraphReader<T>::GraphReader(char * file_name, bool createNodeHash) 
{
    this->nodeHash = createNodeHash ? new OriginalNodeHash() : NULL;
    this->FR = new FileReader(file_name);

    this->length = 0;

    //Save some extra buffer space for partial reads
    uint32 extra_buffer = sizeof(HeaderGraph<T>) + sizeof(T);
    this->buffer_start = new char[GRAPH_READ_BUFFER*_1_MB + extra_buffer];
    
    this->buffer_start = this->buffer_start + extra_buffer;
    this->buffer_end = this->buffer_start + GRAPH_READ_BUFFER*_1_MB;
    
    this->start = this->buffer_start;
    this->end = this->buffer_start;
}

template <typename T>
GraphReader<T>::GraphReader()
{
}

template <typename T>
GraphReader<T>::~GraphReader()
{
    delete[](this->buffer_start - sizeof(HeaderGraph<T>));
}

template<typename T>
bool GraphReader<T>::has_next()
{
    return this->FR->has_next() ? true : this->start < this->end;
}

template<typename T>
void GraphReader<T>::next()
{
    this->length--;
    this->start += sizeof(T);
}

template<typename T>
T GraphReader<T>::current()
{
    if (this->start == this->end)
    {
        this->load();
        this->start = this->buffer_start;
    }

    uint32 sizeToRead = this->length == 0 ? sizeof(HeaderGraph<T>) + sizeof(T) : sizeof(T);

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
        this->nodeHash->put(((HeaderGraph<T>*) this->start)->hash);
        this->length = ((HeaderGraph<T>*) this->start)->len;
        this->start += sizeof(HeaderGraph<T>);
    }

    return *(T*) this->start;
}

template<typename T>
void GraphReader<T>::load()
{
    uint32 bytesTransferred = 0;
    this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);
    this->end = this->buffer_start + bytesTransferred;
}
