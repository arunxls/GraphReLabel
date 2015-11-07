#include "stdafx.h"
#include "RenamedGraphMerge.h"

template <typename T>
RenamedGraphMerge<T>::RenamedGraphMerge()
{
}

template <typename T>
RenamedGraphMerge<T>::RenamedGraphMerge(std::deque<char*>& output_files, uint32 buffer)
{
    this->output_files = output_files;
    this->buffer_size = buffer;
}

template <typename T>
RenamedGraphMerge<T>::~RenamedGraphMerge()
{
}

template <typename T>
void RenamedGraphMerge<T>::execute()
{
    uint32 buffer = this->buffer_size;
    buffer -= 2 * GRAPH_READ_BUFFER*_1_MB;
    
    char* buffer_start = new char[buffer];
    char* buffer_end = buffer_start + buffer;
    char* start = buffer_start;

    while (this->output_files.size() > 1) 
    {
        char* str1 = this->output_files.front();
        this->output_files.pop_front();

        char* str2 = this->output_files.front();
        this->output_files.pop_front();

        {
            GraphReader<T, uint32> read_1(str1, false);
            GraphReader<T, uint32> read_2(str2, false);
           
            char* str3 = getNewOutputFile();
            FileWriter FW(str3);

            read_1.init();
            read_2.init();

            while (read_1.has_next() || read_2.has_next()) 
            {
                HeaderGraph<T> h;
                if (read_1.has_next() && read_2.has_next()) {
                    HeaderGraph<T> h1 = read_1.currentHeader;
                    HeaderGraph<T> h2 = read_2.currentHeader;

                    if (h1 < h2) {
                        put(buffer_start, buffer_end, start, read_1, FW);
                    }
                    else {
                        put(buffer_start, buffer_end, start, read_2, FW);
                    }
                }
                else if (read_1.has_next()) {
                    put(buffer_start, buffer_end, start, read_1, FW);
                }
                else {
                    put(buffer_start, buffer_end, start, read_2, FW);
                }
            }

    //    //    this->write_merged->compact();
    //    //    this->write_merged->writeToDisk(str3);
            this->output_files.push_back(str3);

    //    //    this->total_read = this->read_1->total_read + this->read_2->total_read + this->write_merged->total_read;
    //    //    this->total_write = this->read_1->total_write + this->read_2->total_write + this->write_merged->total_write;
        }

    //    //DeleteFile(TEXT(str1.c_str()));
    //    //DeleteFile(TEXT(str2.c_str()));
    }
}

template<typename T>
void RenamedGraphMerge<T>::put(char *& buffer_start, char*& buffer_end, char *& start, GraphReader<T, uint32>& graph, FileWriter & FW)
{
    HeaderGraph<T> h = graph.currentHeader;
    uint32 sizeToCopy = sizeof(HeaderGraph<T, uint32>) + h.len*sizeof(uint32);
    
    if (start + sizeToCopy >= buffer_end)
    {
        FW.write(buffer_start, (start - buffer_start));
        start = buffer_start;
    }

    graph.copyRange(start);
}

