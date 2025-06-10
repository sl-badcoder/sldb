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
    page_id_t page_id;
    BufferPoolManager::initInstance(2048, "test.db", BUFFER_REPLACEMENT::TQ);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
    }
    int number_of_frames = BufferPoolManager::getInstance()->getFrames().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(2048, number_of_frames);
    EXPECT_EQ(10,page_table_size );
    BufferPoolManager::destroyInstance();
}
