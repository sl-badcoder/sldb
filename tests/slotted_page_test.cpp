//
// Created by Senad Lemes Galera on 17.06.25.
//
//--------------------------------------------------------------------------------------
#include <assert.h>
#include <gtest/gtest.h>
//--------------------------------------------------------------------------------------
#include "database/buffermanager/buffer_manager.h"
#include "database/buffermanager/buffer_replacement.h"
#include "storage/slotted_page.h"
#include "file_api.h"
//--------------------------------------------------------------------------------------
// All tests for the different replacmenet policies which were implemented
//--------------------------------------------------------------------------------------

TEST(SlottedPageTest, AllocateHeader){
    std::vector<std::byte> buffer;
    buffer.resize(1024);
    auto* page = new (&buffer[0]) SlottedPage(1024);
    page->allocate(10);
    EXPECT_EQ(page->header.free_space, 1024 - 10 - sizeof(PageHeader) - sizeof(SlotDirectoryEntry));
    EXPECT_EQ(page->header.num_slots, 1);
    EXPECT_EQ(page->header.data_start, 1024 - 10);
    EXPECT_EQ(page->header.first_free_slot, 1);
}

TEST(SlottedPageTest, Erase)
{
    std::vector<std::byte> buffer;
    buffer.resize(1024);
    auto* page = new (&buffer[0]) SlottedPage(1024);
    page->allocate(10);
    page->erase(0);
    EXPECT_EQ(page->header.first_free_slot, 0);
    EXPECT_EQ(page->header.num_slots, 0);
}

TEST(SlottedPageTest, Resize)
{
    std::vector<std::byte> buffer;
    buffer.resize(1024);
    auto* page = new (&buffer[0]) SlottedPage(1024);
    page->allocate(10);
    page->resize(0, 20);
    EXPECT_EQ(page->header.first_free_slot, 1);
    EXPECT_EQ(page->header.num_slots, 1);
    EXPECT_EQ(page->header.data_start, 1024 - 30);
}