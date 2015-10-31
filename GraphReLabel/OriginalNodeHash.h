#pragma once

#include "include.h"

#define ORIGINAL_NODE_HASH 2

template <typename T>
class OriginalNodeHash
{
public:
    OriginalNodeHash();
    ~OriginalNodeHash();

    uint32 size() { return ORIGINAL_NODE_HASH; }
};

