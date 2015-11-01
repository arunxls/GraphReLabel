#include "stdafx.h"
#include "RenamedGraph.h"

template <typename T>
RenamedGraph<T>::RenamedGraph()
{
}



template <typename T>
RenamedGraph<T>::~RenamedGraph()
{
}

template<typename T>
void RenamedGraph<T>::init(uint32 buffer_size)
{
    this->buffer_start = new char[buffer_size*_1_MB];
    this->buffer_end = this->buffer_start + buffer_size*_1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_start;
}
