//--------------------------------------------------------------------------------------
//
// Created by Senad Lemes Galera on 05.06.25.
//
//--------------------------------------------------------------------------------------
#ifndef FILE_H
#define FILE_H
#include <cstdint>
#include <unistd.h>
#include <sys/fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>

//--------------------------------------------------------------------------------------
class File
{
public:
    File(char* file_name, bool write)
    {
        if (write)
        {
            if ((file = std::fopen(file_name, "r+b")) == NULL)
            {
                std::perror("File couldn't open");
                return;
            }
        }
        else if ((file = std::fopen(file_name, "rb")) == NULL)
        {
            std::perror("File couldn't open");
            return;
        };
        this->file_name = file_name;
    }

    //--------------------------------------------------------------------------------------
    ~File()
    {
        std::fclose(file);
    }

    //--------------------------------------------------------------------------------------
    inline void read_block(size_t offset, size_t size, char* dest)
    {
        fseek(file, offset, SEEK_SET);
        if (!feof(file))
        {
            size_t result = fread(dest, sizeof(char), size, file);
            if(result != size)
            {
                std::perror("elements are not read correctly");
            }
        }
    }

    //--------------------------------------------------------------------------------------
    inline void write_block(char* src, size_t offset, size_t size)
    {
        fseek(file, offset, SEEK_SET);
        if (file->_flags == 0)
        {
            std::cout << "File is read only" << std::endl;
            return;
        }
        if(feof(file))
        {
            std::perror("File does not exist");
        }
        fwrite(src, sizeof(char), size, file);
    }

    char* get_file_name()
    {
        return file_name;
    }

    //--------------------------------------------------------------------------------------
private:
    FILE* file;
    char* file_name;
};


#endif //FILE_H
//--------------------------------------------------------------------------------------
