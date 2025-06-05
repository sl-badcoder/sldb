//
// Created by Senad Lemes Galera on 05.06.25.
//

#ifndef TESTE_H
#include <assert.h>
#include <File.h>
#define TESTE_H

void CHECK_RW()
{
    File file = File("test.db", true);
    constexpr int CHAR_SIZE = 10;
    char in[CHAR_SIZE];
    for(int i = 0; i < CHAR_SIZE; i++)
    {
        in[i] = 'A';
    }
    file.write_block(in, 0, CHAR_SIZE);
    char out[CHAR_SIZE];
    file.read_block(0, CHAR_SIZE, out);

    for (int i = 0; i < CHAR_SIZE; i++)
    {
        assert(in[i] == out[i]);
    }
    std::cout << "[INFO]: CHECK_RW PASSED" << std::endl;
}

void CHECK_OFFSET_RW()
{
    File file = File("test.db", true);
    char in[1] = {'Z'};
    file.write_block(in, 3, 1);
    char out[1];
    file.read_block(3, 1, out);
    assert(in[0] == out[0]);
    std::cout << "[INFO]: CHECK_OFFSET_RW PASSED" << std::endl;
}
#endif //TESTE_H
