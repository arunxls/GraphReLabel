#include "stdafx.h"
#include "TopN.h"
#include "GraphReader.cpp"
#include <algorithm>
#include <functional>

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

    int bound = 0;

    while (graph->has_next()) {
        HeaderGraph<uint32, uint32> header = graph->currentHeader();

        graph->nextHeader();
        while (header.len-- > 0)
        {
            uint32 index = this->graph->currentNeighbour();
            count[index] += 1;
            bound = std::max<int>(bound, index);
            this->graph->nextNeighbour();
        }
    }

    std::partial_sort(count, count + bound - 9, count + bound+1);

    uint32 max = 0;
    std::vector<uint32> top;

    for (int i = bound - 9; i < bound + 1; ++i)
    {
        printf("%I32u\n", count[i]);
    }

    printf("%I32u %I64\n", bound, max);
}
