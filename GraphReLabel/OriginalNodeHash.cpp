#include "stdafx.h"
#include "OriginalNodeHash.h"
#include "utils.h"

OriginalNodeHash::OriginalNodeHash()
{
    this->buffer_start = new char[ORIGINAL_NODE_HASH * _1_MB];
    this->buffer_end = this->buffer_start + ORIGINAL_NODE_HASH * _1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_end;

    this->total_read = 0;
    this->total_write = 0;

    this->FW = new FileWriter(getNewOutputFile());
}

OriginalNodeHash::~OriginalNodeHash()
{
}

void OriginalNodeHash::put(uint32 element)
{
    if (this->start + sizeof(uint32) > this->end) {
        this->writeToDisk();
    }

    *(uint32*) this->start = element;
    this->start += sizeof(uint32);
}

void OriginalNodeHash::writeToDisk()
{
    this->FW->write(this->buffer_start, (this->start - this->buffer_start));
    this->start = this->buffer_start;
}
