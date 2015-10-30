#pragma once

#include <windows.h>
#include "include.h"

class FileWriter
{
public:
    char* filename;

    HANDLE hFile;

    FileWriter(char*);
    ~FileWriter();

    void write(void* start, uint32 bytesToWrite);

private:
    HANDLE getFileHandle();
};