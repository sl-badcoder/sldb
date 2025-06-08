//--------------------------------------------------------------------------------------
# include <gtest/gtest.h>
#include <vector>
//--------------------------------------------------------------------------------------
#include "../src/database/buffermanager/buffer_manager.h"
#include "../src/database/buffermanager/buffer_replacement.h"
#include "../include/File.h"
//--------------------------------------------------------------------------------------
// All tests for the different replacmenet policies which I implemented
//--------------------------------------------------------------------------------------
BufferPoolManager* BufferPoolManager::instance_ = nullptr;
std::mutex BufferPoolManager::singleton_mutex_;
//--------------------------------------------------------------------------------------
using TQ = BUFFER_REPLACEMENT::TQ;

TEST(ReplacementTest, TQPolicy){
    // create three pages
    std::vector<File> files;
    for(int i = 0; i < 3; i++){
        files.push_back(new File(reinterpret_cast<char*>(i), true))
    }

    BufferPoolManager::initInstnace(2048, "test.db", TQ);

    BufferPoolManager::destroyInstance();
}
