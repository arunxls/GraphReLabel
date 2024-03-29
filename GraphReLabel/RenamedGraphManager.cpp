#include "stdafx.h"
#include <windows.h>
#include "RenamedGraphManager.h"
#include "RenamedGraph.cpp"
#include "utils.h"
#include "FileWriter.h"
#include <algorithm>

template<typename T>
RenamedGraphManager<T>::RenamedGraphManager(uint32 buffer_size)
{
    uint32 splitSize = (buffer_size) / RENAME_BUCKETS;
    this->bucket = new RenamedGraph<T>[RENAME_BUCKETS];
    for (int i = 0; i < RENAME_BUCKETS; i++) {
        RenamedGraph<T>* tmp = this->bucket + i;
        tmp->init(splitSize);
    }

    this->rangeKey = 0;
    this->total_read = 0;
    this->total_write = 0;
    this->init_threads();
}

template <typename T>
RenamedGraphManager<T>::RenamedGraphManager()
{
    this->rangeKey = 0;
}

template <typename T>
RenamedGraphManager<T>::~RenamedGraphManager()
{
    delete[] this->bucket;
    //CloseHandle(ghSortSemaphore);
    //CloseHandle(ghWriteSemaphore);
    for (int i = 0; i < RENAME_BUCKETS; ++i) {
        CloseHandle(this->hThreadArray[i]);
    }
}

template <typename T>
DWORD WINAPI RenamedGraphManager<T>::sortAndCompact(LPVOID data)
{
    while (1) {
        WaitForSingleObject(ghSortSemaphore, INFINITE);
        WaitForSingleObject(ghWriteSemaphore, INFINITE);

        RenamedGraph<T>* hCount = (RenamedGraph<T>*)data;
        hCount->sort();
        hCount->compact();
        bool flag = ReleaseSemaphore(ghWriteSemaphore, 1, NULL);
        if (DEBUG && !flag)
        {
            DisplayError(TEXT("SetEvent"));
            return 1;
        }
    }

    return 0;
}

template<typename T>
void RenamedGraphManager<T>::put(uint32 renamed, T original)
{
    RenamedHeaderGraph<T> tmp;
    tmp.old = original;
    tmp.len = 1;
    tmp.renamed = renamed;

    int key;

    if (this->rangeKey != 0) 
    {
        key = original / ((this->rangeKey + RENAME_BUCKETS) / RENAME_BUCKETS);
    }
    else 
    {
        //Get top 3 bits
        key = original >> 61;
    }
    
    RenamedGraph<T>* targetBucket = (this->bucket + key);
    if (targetBucket->hasNext()) {
        targetBucket->put(&tmp);
    }
    else {
        //for (int i = 0; i < RENAME_BUCKETS; i++) {
        //    RenamedGraph<T>* tmp = (this->bucket + i);
        //    printf("%d - %d\n", i, (tmp->start - tmp->buffer_start) / sizeof(RenamedHeaderGraph<T>));
        //}

        //printf("\n------\n");
        this->writeToDisk();
    }
}

template<typename T>
void RenamedGraphManager<T>::writeToDisk()
{
    DWORD   dwThreadIdArray[RENAME_BUCKETS];
    HANDLE  hThreadArray[RENAME_BUCKETS];

    for (int i = 0; i < RENAME_BUCKETS; ++i) {
        bool flag = ReleaseSemaphore(ghSortSemaphore, 1, NULL);
        if (DEBUG && !flag)
        {
            DisplayError(TEXT("SetEvent"));
        }

        flag = ReleaseSemaphore(ghWriteSemaphore, 1, NULL);
        if (DEBUG && !flag)
        {
            DisplayError(TEXT("SetEvent"));
        }
    }

    Sleep(500);
    for (int i = 0; i < RENAME_BUCKETS; ++i)
    {
        WaitForSingleObject(ghWriteSemaphore, INFINITE);
    }

    char* fileName = getNewOutputFile();
    output_files.push_back(fileName);
    FileWriter FW(fileName);
    for (int i = 0; i < RENAME_BUCKETS; ++i)
    {
        FW.write((this->bucket + i)->buffer_start, (this->bucket + i)->start - (this->bucket + i)->buffer_start);
        this->total_write += (this->bucket + i)->start - (this->bucket + i)->buffer_start;
        
        (this->bucket + i)->start = (this->bucket + i)->buffer_start;
    }
}

template<typename T>
void RenamedGraphManager<T>::init_threads()
{
    DWORD   dwThreadIdArray[RENAME_BUCKETS];
    for (int i = 0; i < RENAME_BUCKETS; ++i) 
    {
        this->hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            RenamedGraphManager<T>::sortAndCompact,         // thread function name
            (this->bucket + i),  // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 

                                    //SetThreadPriority(this->hThreadArray[i], THREAD_PRIORITY_HIGHEST);

        if (DEBUG && hThreadArray[i] == NULL)
        {
            //ErrorHandler(TEXT("CreateThread"));
            ExitProcess(3);
        }
    }
}