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
    SlottedPage* page = new SlottedPage(1024);
    page->allocate(10);
    PageHeader header = page->getHeader();
    EXPECT_EQ(header.free_space, 1024 - 10 - sizeof(PageHeader) - sizeof(SlotDirectoryEntry));
    EXPECT_EQ(header.num_slots, 1);
    EXPECT_EQ(header.data_start, 1024 - 10);
    EXPECT_EQ(header.first_free_slot, 1);
}

TEST(SlottedPageTest, Erase)
{
    SlottedPage* page = new SlottedPage(1024);
    PageHeader header = page->getHeader();
    page->allocate(10);
    page->erase(0);
    EXPECT_EQ(header.first_free_slot, 0);
    EXPECT_EQ(header.num_slots, 0);
}