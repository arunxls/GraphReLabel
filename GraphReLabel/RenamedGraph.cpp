#include "stdafx.h"
#include <algorithm>
#include "RenamedGraph.h"

template <typename T>
RenamedGraph<T>::RenamedGraph()
{
}

template <typename T>
RenamedGraph<T>::~RenamedGraph()
{
    delete[] this->buffer_start;
}

template<typename T>
void RenamedGraph<T>::init(uint32 buffer_size)
{
    this->buffer_start = new char[buffer_size];
    this->buffer_end = this->buffer_start + buffer_size;

    this->start = this->buffer_start;
    this->end = this->buffer_start;
}

template<typename T>
bool RenamedGraph<T>::hasNext()
{
    return this->start + sizeof(RenamedHeaderGraph<T>) < this->buffer_end;
}

template<typename T>
void RenamedGraph<T>::put(RenamedHeaderGraph<T> * element)
{
    ((RenamedHeaderGraph<T>*) this->start)->len = element->len;
    ((RenamedHeaderGraph<T>*) this->start)->old = element->old;
    ((RenamedHeaderGraph<T>*) this->start)->renamed = element->renamed;

    this->start += sizeof(RenamedHeaderGraph<T>);
}

template<typename T>
void RenamedGraph<T>::sort()
{
    std::sort((RenamedHeaderGraph<T>*) this->buffer_start, (RenamedHeaderGraph<T>*) this->start);
}

template<typename T>
void RenamedGraph<T>::compact()
{
    HeaderGraph<T, uint32>* prev = (HeaderGraph<T, uint32>*) this->buffer_start;
    HeaderGraph<T, uint32>* current = (HeaderGraph<T, uint32>*) (((char*)prev) + prev->size());
    while ((char*)current < this->start)
    {
        uint32 current_size = current->size();
        uint32 prev_size = prev->size();

        if (prev < current) 
        {
            if (*current == *prev) {
                *prev += *current;
            }
            else {
                prev = (HeaderGraph<T, uint32>*) (((char*)prev) + prev_size);
                if (prev < current) {
                    *prev = *current;
                }
            }
        }
        current = (HeaderGraph<T, uint32>*) (((char*)current) + current_size);
    }

    this->start = (char*) (((char*)prev) + prev->size());
}
