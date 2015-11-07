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
    //this->merge();
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
    getNewOutputFile();
    for (int i = 0; i < 64; i++) {
        this->output_files.push_back(getNewOutputFile());
    }

    RenamedGraphMerge<T> merge(this->output_files, this->buffer);
    merge.execute();
    
    printf("\n");

    return;

}
