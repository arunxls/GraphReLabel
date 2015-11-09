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
    void read_async(LPVOID buffer, uint32 bytesTotransfer, uint32& bytesTrasferred);
    bool has_next();
    LONGLONG getFileSize();
    void getFileHandle();
    void CloseFileHandle();
private:
    bool FileHandleOpen;
    void readFile(char*, LPVOID, OVERLAPPED&, uint32&, uint32);
    void readFile_async(char*, LPVOID, OVERLAPPED&, uint32&, uint32);
};

