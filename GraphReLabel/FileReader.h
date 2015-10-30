#pragma once

#include <windows.h>
#include "include.h"
#include <string>

class FileReader
{
public:
    char* filename;
    uint64 offset_overall;
    uint32 offset_current_read;
    uint64 size;

    HANDLE hFile;

    FileReader() {};
    FileReader(char*& filename);
    ~FileReader();

    void read(LPVOID buffer, uint32 bytesTotransfer, uint32& bytesTrasferred);
    bool has_next();
    LONGLONG getFileSize();
    void reduceOffset(uint32);

private:
    void readFile(char*, LPVOID, OVERLAPPED&, uint32&, uint32);
    HANDLE getFileHandle();
};

