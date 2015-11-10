#include "stdafx.h"
#include "OriginalNodeHash.h"
#include "utils.h"

OriginalNodeHash::OriginalNodeHash(char * file_name)
{
    this->buffer_start = new char[ORIGINAL_NODE_HASH_READER * _1_MB];
    this->buffer_end = this->buffer_start + ORIGINAL_NODE_HASH_READER * _1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_end;

    this->total_read = 0;
    this->total_write = 0;

    this->FW = NULL;
    this->FR = new FileReader(file_name);
    this->load();

    this->position = 0;
    this->overshoot = this->FR->size / sizeof(uint64);
}

OriginalNodeHash::OriginalNodeHash()
{
    this->buffer_start = new char[ORIGINAL_NODE_HASH_WRITER * _1_MB];
    this->buffer_end = this->buffer_start + ORIGINAL_NODE_HASH_WRITER * _1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_end;

    this->total_read = 0;
    this->total_write = 0;

    this->FR = NULL;
    this->FW = new FileWriter(getNewOutputFile());
}

OriginalNodeHash::~OriginalNodeHash()
{
    delete[] this->buffer_start;
    if (this->FR != NULL) delete this->FR;
    if (this->FW != NULL) delete this->FW;
}

void OriginalNodeHash::put(uint64 element)
{
    if (this->start + sizeof(uint64) > this->end) {
        this->writeToDisk();
    }

    *(uint64*) this->start = element;
    this->start += sizeof(uint64);
}

uint32 OriginalNodeHash::get(uint64 element)
{
    if (this->start >= this->end)
    {
        this->load();
    }

    while (element > *(uint64*)this->start) {
        this->start += sizeof(uint64);
        this->position++;
        
        if (this->start >= this->end)
        {
            this->load();
        }
    }

    if (*(uint64*)this->start == element)
    {
        return this->position;
    }
    else 
    {
        return ++this->overshoot;
    }
}



void OriginalNodeHash::writeToDisk()
{
    this->FW->write(this->buffer_start, (this->start - this->buffer_start));
    this->total_write += (this->start - this->buffer_start);
    this->start = this->buffer_start;
}

void OriginalNodeHash::load()
{
    uint32 bytesTransferred = 0;
    this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);
    this->end = this->buffer_start + bytesTransferred;
    this->total_read += bytesTransferred;
    this->start = this->buffer_start;
}
