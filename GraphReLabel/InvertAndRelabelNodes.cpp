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
}

template <typename T>
InvertAndRelabelNodes<T>::~InvertAndRelabelNodes()
{
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

    OriginalNodeHash* nodeHash = NULL;
    if (this->createNodeHash) {
        nodeHash = new OriginalNodeHash();
        buffer_size -= nodeHash->size();
    }

    RenamedGraphManager<T> renamedGraphManager(buffer_size);
    
    uint32 currentRenameCount = -1;
    graph.load();
    while (graph.has_next()) {
        HeaderGraph<uint64, T> header = graph.currentHeader();
        this->getRenamed(header.hash, currentRenameCount, nodeHash);
        graph.nextHeader();
        while (header.len-- > 0)
        {
            renamedGraphManager.put(currentRenameCount, graph.currentNeighbour());
            graph.nextNeighbour();
        }
    }

    renamedGraphManager.writeToDisk();
    nodeHash->writeToDisk();

    this->output_files = std::move(renamedGraphManager.output_files);
    this->nodesHash = nodeHash->FW->filename;
}

template<typename T>
void InvertAndRelabelNodes<T>::merge()
{
    //getNewOutputFile();
    //for (int i = 0; i < 62; i++) {
    //    this->output_files.push_back(getNewOutputFile());
    //}

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
            CloseHandle(hThreadArray[i]);
        }

        delete merge[0]; delete merge[1];
    }
    this->output_files.push_back(output_files1[0]);
    RenamedGraphMerge<T> m(&this->output_files, this->buffer);
    m.execute();
    //this->total_read += merge.total_read;
    //this->total_write += merge.total_write;

    return;

}
