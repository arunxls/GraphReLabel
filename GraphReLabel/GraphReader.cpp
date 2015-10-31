#include "stdafx.h"
#include "GraphReader.h"
#include "OriginalNodeHash.cpp"

template <typename T1, typename T2>
GraphReader<T1, T2>::GraphReader(char * file_name, bool createNodeHash) 
{
    this->nodeHash = createNodeHash ? new OriginalNodeHash() : NULL;
    this->FR = new FileReader(file_name);
}

template <typename T1, typename T2>
GraphReader<T1, T2>::GraphReader()
{
}

template <typename T1, typename T2>
GraphReader<T1, T2>::~GraphReader()
{
}
