//--------------------------------------------------------------------------------------
# include <gtest/gtest.h>
#include <vector>
//--------------------------------------------------------------------------------------
#include "database/buffermanager/buffer_manager.h"
#include "database/buffermanager/buffer_replacement.h"
#include "file_api.h"
//--------------------------------------------------------------------------------------
// All tests for the different replacmenet policies which I implemented
//--------------------------------------------------------------------------------------
BufferPoolManager* BufferPoolManager::instance_ = nullptr;
std::mutex BufferPoolManager::singleton_mutex_;
//--------------------------------------------------------------------------------------

TEST(ReplacementTest, TQPolicy){
    // create three pages

    BufferPoolManager::initInstance(2048, "test.db", BUFFER_REPLACEMENT::TQ);

    BufferPoolManager::destroyInstance();
}
