#include "stdafx.h"
#include "RenamedGraphManager.h"
#include "RenamedGraph.cpp"

template<typename T>
RenamedGraphManager<T>::RenamedGraphManager(uint32 buffer_size)
{
    uint32 splitSize = (buffer_size) / RENAME_BUCKETS;
    this->bucket = new RenamedGraph[RENAME_BUCKETS];
    for (int i = 0; i < RENAME_BUCKETS; i++) {
        RenamedGraph* tmp = this->bucket + i;
        tmp->init(splitSize);
    }
}

template <typename T>
RenamedGraphManager<T>::RenamedGraphManager()
{
}

template <typename T>
RenamedGraphManager<T>::~RenamedGraphManager()
{
}

template<typename T>
void RenamedGraphManager<T>::put(uint32 renamed, T original)
{
    RenamedHeaderGraph<T> tmp;
    tmp.renamed = renamed;
    tmp.len = 1;
    tmp.old = original;

    //Get top 2 bits
    int key = ((original & 0xc000000000000000) >> 62);
    RenamedGraph<T>* targetBucket = (this->bucket + key);
}
