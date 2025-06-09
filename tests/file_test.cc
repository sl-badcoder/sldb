//
// Created by Senad Lemes Galera on 05.06.25.
//
//--------------------------------------------------------------------------------------
#include <assert.h>
#include "file_api.h"
#include <gtest/gtest.h>
//--------------------------------------------------------------------------------------
TEST(FileTest, READWRITE_TEST) {
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
        EXPECT_EQ(in[i], out[i]);
    }
}
//--------------------------------------------------------------------------------------
TEST(FileTest, OFFSET_TEST) {
    File file = File("test.db", true);
    char in[1] = {'Z'};
    file.write_block(in, 3, 1);
    char out[1];
    file.read_block(3, 1, out);
    EXPECT_EQ(in[0], out[0]);
}
//--------------------------------------------------------------------------------------