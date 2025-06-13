//--------------------------------------------------------------------------------------
# include <gtest/gtest.h>
#include <vector>
//--------------------------------------------------------------------------------------
#include "database/buffermanager/buffer_manager.h"
#include "database/buffermanager/buffer_replacement.h"
#include "file_api.h"
//--------------------------------------------------------------------------------------
// All tests for the different replacmenet policies which were implemented
//--------------------------------------------------------------------------------------
BufferPoolManager* BufferPoolManager::instance_ = nullptr;
std::mutex BufferPoolManager::singleton_mutex_;
//--------------------------------------------------------------------------------------

TEST(ReplacementTest, Init){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(2048, "test.db", BUFFER_REPLACEMENT::TQ);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
    }
    int free_frames = BufferPoolManager::getInstance()->getFreeList().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(2038, free_frames);
    EXPECT_EQ(10,page_table_size );
    BufferPoolManager::destroyInstance();
}

//--------------------------------------------------------------------------------------
TEST(ReplacementTest, TQPolicy){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(2048, "test.db", BUFFER_REPLACEMENT::TQ);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
    }
    int free_frames = BufferPoolManager::getInstance()->getFreeList().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(2038, free_frames);
    EXPECT_EQ(10,page_table_size );
    BufferPoolManager::destroyInstance();
}

//--------------------------------------------------------------------------------------
TEST(ReplacementTest, LRUPolicy){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(2048, "test.db", BUFFER_REPLACEMENT::LRU);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
    }
    int free_frames = BufferPoolManager::getInstance()->getFreeList().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(2038, free_frames);
    EXPECT_EQ(10,page_table_size );
    BufferPoolManager::destroyInstance();
}

//--------------------------------------------------------------------------------------
TEST(ReplacementTest, LFUPolicy){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(10, "test.db", BUFFER_REPLACEMENT::LFU);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
    }

    for(int i = 0; i < 10; i++){
        page_id = i;
        if(i != 6)BufferPoolManager::getInstance()->fetchPage(page_id);
    }
    page_id = 11;
    BufferPoolManager::getInstance()->newPage(&page_id);

    //--------------------------------------------------------------------------------------
    // Page 6 shouldn't be in the buffer pool anymore
    //--------------------------------------------------------------------------------------
    EXPECT_EQ(BufferPoolManager::getInstance()->getPageTable().find(6) == BufferPoolManager::getInstance()->getPageTable().end(), true);
    int free_frames = BufferPoolManager::getInstance()->getFreeList().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(0, free_frames);
    EXPECT_EQ(10,page_table_size );
    BufferPoolManager::destroyInstance();
}
//--------------------------------------------------------------------------------------
TEST(ReplacementTest, FIFOPolicy){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(9, "test.db", BUFFER_REPLACEMENT::FIFO);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
    }
    int free_frames = BufferPoolManager::getInstance()->getFreeList().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    std::unordered_map<page_id_t, Frame*> page_table_ = BufferPoolManager::getInstance()->getPageTable();
    EXPECT_EQ(0, free_frames);
    EXPECT_EQ(9,page_table_size );
    // The first page shouldnt be in the BufferPool anymore
    ASSERT_EQ(BufferPoolManager::getInstance()->getPageTable().find(0) ==  BufferPoolManager::getInstance()->getPageTable().end(), 1);
    BufferPoolManager::destroyInstance();
}
//--------------------------------------------------------------------------------------
TEST(ReplacementTest, ClockPolicy){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(9, "test.db", BUFFER_REPLACEMENT::CLOCK);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
    }
    int free_frames = BufferPoolManager::getInstance()->getFreeList().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(0, free_frames);
    EXPECT_EQ(9,page_table_size );
    BufferPoolManager::destroyInstance();
}