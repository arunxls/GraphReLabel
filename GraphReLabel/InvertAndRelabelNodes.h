#pragma once
#include "include.h"
#include "OriginalEightByteHashes.h"
#include "OriginalGraphReader.h"

class InvertAndRelabelNodes
{
public:
    uint32 total_read;
    uint32 total_write;

    OriginalEightByteHashes* hashes;
    OriginalGraphReader* graph;

    InvertAndRelabelNodes(char* file_name, uint32 buffer_size);
    ~InvertAndRelabelNodes();

    void execute();
};

