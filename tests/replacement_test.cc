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
    int number_of_frames = BufferPoolManager::getInstance()->getFrames().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(2048, number_of_frames);
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
    int number_of_frames = BufferPoolManager::getInstance()->getFrames().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(2048, number_of_frames);
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
    int number_of_frames = BufferPoolManager::getInstance()->getFrames().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(2048, number_of_frames);
    EXPECT_EQ(10,page_table_size );
    BufferPoolManager::destroyInstance();
}

//--------------------------------------------------------------------------------------
TEST(ReplacementTest, LFUPolicy){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(2048, "test.db", BUFFER_REPLACEMENT::LFU);
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
//--------------------------------------------------------------------------------------
TEST(ReplacementTest, FIFOPolicy){
    // create three pages
    page_id_t page_id;
    BufferPoolManager::initInstance(9, "test.db", BUFFER_REPLACEMENT::FIFO);
    for (int i = 0; i < 10; i++) {
        page_id = i;
        BufferPoolManager::getInstance()->newPage(&page_id);
        std::cout << "i: " << i << std::endl;
    }
    int number_of_frames = BufferPoolManager::getInstance()->getFrames().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    std::unordered_map<page_id_t, Frame*> page_table_ = BufferPoolManager::getInstance()->getPageTable();
    EXPECT_EQ(9, number_of_frames);
    EXPECT_EQ(9,page_table_size );
    // TODO: update page_table_ logic there are invalid values stored
    /**
    for(int i = 0; i < page_table_size; i++)
    {
        SlottedPage* page = std::move(BufferPoolManager::getInstance()->getPageTable().at(i)->getPage());
        if(page)
        {
            std::cout << page->getPageId() << std::endl;
        }
    }**/
    // The first page shouldnt be in the BufferPool anymore
    ASSERT_EQ(BufferPoolManager::getInstance()->getPageTable().find(1) !=  BufferPoolManager::getInstance()->getPageTable().end(), 1);
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
    int number_of_frames = BufferPoolManager::getInstance()->getFrames().size();
    int page_table_size = BufferPoolManager::getInstance()->getPageTable().size();
    EXPECT_EQ(9, number_of_frames);
    EXPECT_EQ(9,page_table_size );
    BufferPoolManager::destroyInstance();
}