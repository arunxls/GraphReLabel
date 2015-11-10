#include "stdafx.h"
#include "TopN.h"
#include "GraphReader.cpp"
#include <algorithm>
#include <functional>
#include <queue>

typedef std::pair<uint32, uint32> pt;
struct comparator {
    bool operator()(pt& a, pt& b) {
        return a.first > b.first;
    }
};

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
            count[index]++;
            bound = std::max<int>(bound, index);
            this->graph->nextNeighbour();
        }
    }

    uint64 deviation = 0;
    std::priority_queue<std::pair<uint32, uint32>, std::vector<std::pair<uint32, uint32>>, comparator> queue;
    for (int i = 0; i < bound + 1; ++i)
    {
        if (count[i] > 0) {
            
            deviation += count[i] *(count[i] - 1) / 2;
            
            std::pair<uint32, uint32>p(count[i], i);
            if (queue.size() == 10) {
                std::pair<uint32, uint32>tmp = queue.top();
                if (tmp.first < p.first) {
                    queue.pop();
                    queue.emplace(p);
                }
            }
            else {
                queue.emplace(p);
            }
        }
    }

    std::deque<std::pair<uint32, uint32>> r_queue;
    while (queue.size() > 0) {
        r_queue.emplace_back(queue.top());
        queue.pop();
    }

    int index = 1;
    for (auto i = r_queue.rbegin(); i != r_queue.rend(); ++i) {
        printf("%d. %I32u %I32u\n", index++, (*i).second, (*i).first);
    }

    printf("Total sum Y_i(Y_i-1)/2 = %I64u\n", deviation);

    this->total_read += this->graph->total_read;
}
