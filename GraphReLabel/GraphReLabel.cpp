// GraphReLabel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <deque>
#include <chrono>
#include <windows.h>
#include <psapi.h>
#include <string>
#include <Windows.h>
#include "include.h"
#include "InvertAndRelabelNodes.h"
#include "InvertAndRelabelNodes.cpp"
#include "GraphReader.h"
#include "TopN.h"

#define BUFFERSIZE 496

//no except
void __cdecl _tmain(int argc, TCHAR *argv[]) noexcept
{
    if (argc != 2 && argc != 3)
    {
        printf("Usage Error: Incorrect number of arguments\n\n");
        _tprintf("Usage:\n\t%s <mode> <map_file_name>\n", argv[0]);
        return;
    }

    uint64 total_read = 0;
    uint64 total_write = 0;

    clock_t begin, end;

    std::chrono::high_resolution_clock::time_point bStart = std::chrono::high_resolution_clock::now();

    if (strcmp(argv[1], "r") == 0){
        printf("Starting 1st Inversion\n");

        char* outputFile;
        char* nodeHash;
        std::chrono::high_resolution_clock::time_point b1 = std::chrono::high_resolution_clock::now();
        {
            InvertAndRelabelNodes<uint64> graph(argv[2], BUFFERSIZE * _1_MB, true);
            graph.execute();

            outputFile = graph.output_files[0];
            nodeHash = graph.nodesHash;

            printf("Total IO: read - %.2f GB; write - %.2f GB\n", (float)graph.total_read / _1_GB, (float)graph.total_write / _1_GB);
            total_read += graph.total_read;
            total_write += graph.total_write;
        }
        std::chrono::high_resolution_clock::time_point e1 = std::chrono::high_resolution_clock::now();
        printf("Took %lld seconds\n", std::chrono::duration_cast<std::chrono::seconds>(e1 - b1).count());
        printf("Ending 1st Inversion\n");

        printf("\nStarting 2nd Inversion\n");

        std::chrono::high_resolution_clock::time_point b2 = std::chrono::high_resolution_clock::now();
        {
            InvertAndRelabelNodes<uint32> graph(outputFile, BUFFERSIZE * _1_MB, false);
            graph.nodesHash = nodeHash;
            graph.execute();
            printf("Total IO: read - %.2f GB; write - %.2f GB\n", (float)graph.total_read / _1_GB, (float)graph.total_write / _1_GB);
            total_read += graph.total_read;
            total_write += graph.total_write;
        }
        std::chrono::high_resolution_clock::time_point e2 = std::chrono::high_resolution_clock::now();
        printf("Took %lld seconds\n", std::chrono::duration_cast<std::chrono::seconds>(e2 - b2).count());
        printf("Ending 2nd Inversion\n");
    }
    else if (strcmp(argv[1], "c") == 0) {
        printf("\nStarting Top 10\n");
        std::chrono::high_resolution_clock::time_point b3 = std::chrono::high_resolution_clock::now();
        {
            TopN topN("PLD-out-relabeled.dat");
            topN.execute();
            printf("Total IO: read - %.2f GB; write - %.2f GB\n", (float)topN.total_read / _1_GB, (float)topN.total_write / _1_GB);
            total_read += topN.total_read;
            total_write += topN.total_write;
        }
        std::chrono::high_resolution_clock::time_point e3 = std::chrono::high_resolution_clock::now();
        printf("Took %lld seconds\n", std::chrono::duration_cast<std::chrono::seconds>(e3 - b3).count());
        printf("\Ending Top 10\n");
    }
    else {
        printf("Usage Error: Incorrect number of arguments\n\n");
        _tprintf("Usage:\n\t%s <mode> <map_file_name>\n", argv[0]);
        return;
    }

    std::chrono::high_resolution_clock::time_point eEnd = std::chrono::high_resolution_clock::now();

    printf("\n");

    printf("Overall stats - RunTime: %lld seconds; Total read %.2f GB; Total write %.2f GB\n",
        std::chrono::duration_cast<std::chrono::seconds>(eEnd - bStart).count(),
        (float)total_read / _1_GB, (float)total_write / _1_GB);

    {
        PROCESS_MEMORY_COUNTERS pp;
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
            PROCESS_VM_READ,
            FALSE, GetCurrentProcessId());
        GetProcessMemoryInfo(hProcess, &pp, sizeof(PROCESS_MEMORY_COUNTERS));
        printf("Peak working set : %.2f MB\n", (double)pp.PeakWorkingSetSize / _1_MB);
    }

    printf("DONE!\n");
    return;
}

