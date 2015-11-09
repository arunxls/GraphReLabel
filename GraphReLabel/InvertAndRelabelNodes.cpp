#include "stdafx.h"
#include "InvertAndRelabelNodes.h"
#include "GraphReader.cpp"
#include "RenamedGraphManager.cpp"
#include "RenamedGraphMerge.cpp"

template <typename T>
InvertAndRelabelNodes<T>::InvertAndRelabelNodes(char * file_name, uint32 buffer_size, bool createNodeHash)
{
    this->total_read = 0;
    this->total_write = 0;
    this->file_name = file_name;
    this->createNodeHash = createNodeHash;
    this->buffer = buffer_size;
    
    ghSortSemaphore = CreateSemaphore(NULL, 0, RENAME_BUCKETS, NULL);
    ghWriteSemaphore = CreateSemaphore(NULL, 0, RENAME_BUCKETS, NULL);
    gNeighbourSortSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
}

template <typename T>
InvertAndRelabelNodes<T>::~InvertAndRelabelNodes()
{
    CloseHandle(ghSortSemaphore);
    CloseHandle(ghWriteSemaphore);

    if (!this->createNodeHash)
    {
        DeleteFile(TEXT(this->file_name));
        DeleteFile(TEXT(this->nodesHash));
    }
}

template <typename T>
void InvertAndRelabelNodes<T>::execute()
{
    this->split();
    this->merge();
}

template <typename T>
void InvertAndRelabelNodes<T>::getRenamed(uint64& element, uint32& currentRenameCount, OriginalNodeHash* nodeHash)
{
    if (this->createNodeHash) {
        nodeHash->put(element);
        currentRenameCount++;
    }
    else 
    {
        currentRenameCount = nodeHash->get(element);
    }
}

template<typename T>
DWORD InvertAndRelabelNodes<T>::mergeExecute(LPVOID data)
{
    RenamedGraphMerge<T>* m = *(RenamedGraphMerge<T>**)data;
    m->execute();

    return 0;
}

template<typename T>
void InvertAndRelabelNodes<T>::split()
{
    uint32 buffer_size = this->buffer;
    GraphReader<uint64, T> graph(file_name);
    buffer_size -= graph.size();

    OriginalNodeHash* nodeHash = this->createNodeHash ? new OriginalNodeHash() : new OriginalNodeHash(this->nodesHash);
    buffer_size -= nodeHash->size();

    RenamedGraphManager<T> renamedGraphManager(buffer_size);
    if (!this->createNodeHash)
    {
        renamedGraphManager.rangeKey = nodeHash->overshoot;
    }

    uint32 currentRenameCount = -1;
    graph.load();
    while (graph.has_next()) {
        HeaderGraph<uint64, T> header = graph.currentHeader();
        
        this->getRenamed(header.hash, currentRenameCount, nodeHash);
        graph.nextHeader();
        //printf("%I32u\n", currentRenameCount);
        while (header.len-- > 0)
        {
            renamedGraphManager.put(currentRenameCount, graph.currentNeighbour());
            //printf("%I32u\n", graph.currentNeighbour());
            //graph.currentNeighbour();
            graph.nextNeighbour();
        }
    }

    renamedGraphManager.writeToDisk();

    this->total_read += renamedGraphManager.total_read + graph.total_read;
    this->total_write += renamedGraphManager.total_write + graph.total_write;

    this->output_files = std::move(renamedGraphManager.output_files);
    
    if (this->createNodeHash)
    {
        nodeHash->writeToDisk();
        this->nodesHash = nodeHash->FW->filename;
    }
    this->total_read += nodeHash->total_read;
    this->total_write += nodeHash->total_write;
        
    delete nodeHash;
}

template<typename T>
void InvertAndRelabelNodes<T>::merge()
{
   /* getNewOutputFile();
    for (int i = 0; i < 62; i++) {
        this->output_files.push_back(getNewOutputFile());
    }*/

    uint32 file_size = this->output_files.size() / 2;

    std::deque<char*>output_files1;
    {    
        RenamedGraphMerge<T>* merge[2];
        for (int i = 0; i < file_size; ++i) {
            output_files1.push_back(this->output_files.front());
            this->output_files.pop_front();
        }

        merge[0] = new RenamedGraphMerge<T>(&output_files1, this->buffer / 2);
        merge[1] = new RenamedGraphMerge<T>(&this->output_files, this->buffer / 2);

        DWORD   dwThreadIdArray[2];
        HANDLE  hThreadArray[2];
        for (int i = 0; i < 2; ++i) {
            hThreadArray[i] = CreateThread(
                NULL,                   // default security attributes
                0,                      // use default stack size  
                mergeExecute,         // thread function name
                (merge + i),  // argument to thread function 
                0,                      // use default creation flags 
                &dwThreadIdArray[i]);   // returns the thread identifier 

            if (DEBUG && hThreadArray[i] == NULL)
            {
                ExitProcess(3);
            }
        }

        WaitForMultipleObjects(2, hThreadArray, TRUE, INFINITE);

        // Close all thread handles.
        for (int i = 0; i < 2; ++i)
        {
            this->total_read += merge[i]->total_read;
            this->total_write += merge[i]->total_write;
            CloseHandle(hThreadArray[i]);
        }

        delete merge[0]; delete merge[1];
    }
    this->output_files.push_back(output_files1[0]);

    //this->output_files.push_back("tmp146");
    //this->output_files.push_back("tmp147");


    RenamedGraphMerge<T> m(&this->output_files, this->buffer);
    if (!this->createNodeHash)
    {
        m.sortNeighbour = true;
    }
    
    m.execute();

    this->total_read += m.total_read;
    this->total_write += m.total_write;

    return;

}
