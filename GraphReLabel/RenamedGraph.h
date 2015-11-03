#pragma once

#include "include.h"

template <typename T>
class RenamedGraph
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    RenamedGraph();
    ~RenamedGraph();
    void init(uint32 buffer_size);
    bool hasNext();
    void put(RenamedHeaderGraph<T>* element);
    void sort();
    void compact();
};

