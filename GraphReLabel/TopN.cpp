#include "stdafx.h"
#include "TopN.h"
#include "GraphReader.cpp"
#include <algorithm>

TopN::TopN(char * file_name)
{
    this->graph = new GraphReader<uint32, uint32>(file_name, 96);
    this->total_read = 0;
    this->total_write = 0;
}

TopN::TopN()
{
    this->total_read = 0;
    this->total_write = 0;
}


TopN::~TopN()
{
    delete this->graph;
}

void TopN::execute()
{
    this->graph->load();

    uint32 elements = COUNT_SIZE / sizeof(uint32) * _1_MB;
    uint32* count = new uint32[elements]();
    while (graph->has_next()) {
        HeaderGraph<uint32, uint32> header = graph->currentHeader();

        graph->nextHeader();
        while (header.len-- > 0)
        {
            uint32 index = this->graph->currentNeighbour();
            count[index] += 1;
            this->graph->nextNeighbour();
        }
    }

    uint32 max = 0;
    for (int i = 0; i < elements; ++i) 
    {
        max = std::max<uint32>(count[i], max);
    }

    printf("%I64\n", max);
}
