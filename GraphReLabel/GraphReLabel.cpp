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

#define BUFFERSIZE 496

//no except
void __cdecl _tmain(int argc, TCHAR *argv[]) noexcept
{
    if (argc != 3)
    {
        printf("Usage Error: Incorrect number of arguments\n\n");
        _tprintf("Usage:\n\t%s <graph_file_name> <map_file_name>\n", argv[0]);
        return;
    }

    uint64 total_read = 0;
    uint64 total_write = 0;

    clock_t begin, end;

    printf("Starting 1st Inversion\n");

    char* outputFile;
    char* nodeHash;
    std::chrono::high_resolution_clock::time_point b1 = std::chrono::high_resolution_clock::now();
    {
        InvertAndRelabelNodes<uint64> graph(argv[1], BUFFERSIZE * _1_MB, true);
        /*graph.execute();

        outputFile = graph.output_files[0];
        nodeHash = graph.nodesHash;*/
        
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
        nodeHash = "tmp1";
        outputFile = "tmp84";

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

    //std::chrono::high_resolution_clock::time_point b3 = std::chrono::high_resolution_clock::now();
    //{
    //    char* tmp = "foo64";
    //    GraphReader<uint32, uint32> graph(tmp);
    //    graph.load();
    //    HeaderGraph<uint32, uint32> h = graph.currentHeader();
    //    //graph.execute();
    //    printf("Total IO: read - %.2f GB; write - %.2f GB\n", (float)graph.total_read / _1_GB, (float)graph.total_write / _1_GB);
    //    total_read += graph.total_read;
    //    total_write += graph.total_write;
    //}

    printf("\n");

    printf("Overall stats - RunTime: %lld seconds; Total read %.2f GB; Total write %.2f GB\n",
        std::chrono::duration_cast<std::chrono::seconds>(e2 - b1).count(),
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

