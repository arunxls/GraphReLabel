#pragma once

#include "include.h"
#include "GraphReader.h"

#define COUNT_SIZE 360

class TopN
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    uint64 total_read;
    uint64 total_write;

    GraphReader<uint32, uint32>* graph;

    TopN(char* file_name);
    TopN();
    ~TopN();

    void execute();
};

