#include "stdafx.h"
#include "OriginalNodeHash.h"

template <typename T>
OriginalNodeHash<T>::OriginalNodeHash()
{
    this->buffer_start = new char[ORIGINAL_NODE_HASH * _1_MB];
    this->buffer_end = this->buffer_start + ORIGINAL_NODE_HASH * _1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_start;
}

template <typename T>
OriginalNodeHash<T>::~OriginalNodeHash()
{
}

template<typename T>
void OriginalNodeHash<T>::put(T element)
{
}
