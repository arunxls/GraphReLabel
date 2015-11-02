#include "stdafx.h"
#include <windows.h>
#include "RenamedGraphManager.h"
#include "RenamedGraph.cpp"
#include "utils.h"

template<typename T>
RenamedGraphManager<T>::RenamedGraphManager(uint32 buffer_size)
{
    uint32 splitSize = (buffer_size) / RENAME_BUCKETS;
    this->bucket = new RenamedGraph<T>[RENAME_BUCKETS];
    for (int i = 0; i < RENAME_BUCKETS; i++) {
        RenamedGraph<T>* tmp = this->bucket + i;
        tmp->init(splitSize);
    }

    ghSortSemaphore = CreateSemaphore(NULL, 0, RENAME_BUCKETS, NULL);
    ghWriteSemaphore = CreateSemaphore(NULL, 0, RENAME_BUCKETS, NULL);

    this->total_read = 0;
    this->total_write = 0;
    this->init_threads();
}

template <typename T>
RenamedGraphManager<T>::RenamedGraphManager()
{
}

template <typename T>
RenamedGraphManager<T>::~RenamedGraphManager()
{
}

template <typename T>
DWORD WINAPI RenamedGraphManager<T>::sortAndCompact(LPVOID data)
{
    while (1) {
        WaitForSingleObject(ghSortSemaphore, INFINITE);
        WaitForSingleObject(ghWriteSemaphore, INFINITE);

        RenamedGraph<T>* hCount = (RenamedGraph<T>*)data;
        hCount->sort();
        //hCount->compact();
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
    tmp.renamed = renamed;
    tmp.len = 1;
    tmp.old = original;

    //Get top 2 bits
    int key = ((original & 0xe000000000000000) >> 61);
    RenamedGraph<T>* targetBucket = (this->bucket + key);
    if (targetBucket->hasNext()) {
        targetBucket->put(&tmp);
    }
    else {
        for (int i = 0; i < RENAME_BUCKETS; i++) {
            RenamedGraph<T>* tmp = (this->bucket + i);
            printf("%d - %d\n", i, (tmp->start - tmp->buffer_start) / sizeof(RenamedHeaderGraph<T>));
        }
        
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

    /*for (int i = 0; i < RENAME_BUCKETS; ++i)
    {
        (this->bucket + i)->writeToDisk(file);
    }*/
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