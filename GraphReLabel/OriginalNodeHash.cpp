#include "stdafx.h"
#include "OriginalNodeHash.h"
#include "utils.h"

template <typename T>
OriginalNodeHash<T>::OriginalNodeHash()
{
    this->buffer_start = new char[ORIGINAL_NODE_HASH * _1_MB];
    this->buffer_end = this->buffer_start + ORIGINAL_NODE_HASH * _1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_end;

    this->total_read = 0;
    this->total_write = 0;

    this->FW = new FileWriter(getNewOutputFile());
}

template <typename T>
OriginalNodeHash<T>::~OriginalNodeHash()
{
}

template<typename T>
void OriginalNodeHash<T>::put(T element)
{
    if (this->start + sizeof(T) > this->end) {
        this->writeToDisk();
    }

    *(T*) this->start = element;
    this->start += sizeof(T);
}

template<typename T>
void OriginalNodeHash<T>::writeToDisk()
{
    this->FW->write(this->buffer_start, (this->start - this->buffer_start));
    this->start = this->buffer_start;
}
