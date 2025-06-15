//
// Created by Senad Lemes Galera on 01.04.25.
//
// -------------------------------------------------------------------------------------
#include <unistd.h>
#include <fcntl.h>
// -------------------------------------------------------------------------------------
#include "buffer_manager.h"

#include <cassert>

#include "storage/slotted_page.h"
#include "basic_frame.h"
// -------------------------------------------------------------------------------------
//TODO: implement different versions of latching
// -------------------------------------------------------------------------------------
SlottedPage* BufferPoolManager::pinPage(page_id_t page_id)
{
    std::lock_guard<std::mutex> guard(latch_);

    if (page_table_.find(page_id) != page_table_.end())
    {
        Frame* frame = page_table_[page_id];
        SlottedPage* page = frame->getPage();

        replacer_->frameAccessed(page_id);

        return page;
    }

    Frame* frame = nullptr;
    if (!free_list_.empty())
    {
        frame = free_list_.front();
        free_list_.pop_front();
    }
    else
    {
        uint64_t page_id = findVictimFrame();
        frame = page_table_[page_id];
        if (!frame)return nullptr;
        if (!evictPage(page_id))return nullptr;
    }

    auto page = std::make_unique<SlottedPage>(page_id);

    frame->setPage(std::move(page));
    page_table_[page_id] = frame;

    return frame->getPage();
}

bool BufferPoolManager::unpinPage(page_id_t page_id, bool is_dirty)
{
    std::lock_guard<std::mutex> guard(latch_);

    if (page_table_.find(page_id) == page_table_.end())
    {
        return false;
    }

    // update Frame values
    Frame* frame = page_table_[page_id];
    frame->setReferenced(false);
    frame->setDirty(is_dirty);

    replacer_->frameAllocated(page_id);

    return true;
}

bool BufferPoolManager::flushPage(page_id_t page_id)
{
    std::lock_guard<std::mutex> guard(latch_);

    if (page_table_.find(page_id) == page_table_.end())return false;

    Frame* frame = page_table_[page_id];

    return true;
}

SlottedPage* BufferPoolManager::newPage(page_id_t* page_id)
{
    std::lock_guard<std::mutex> guard(latch_);

    Frame* frame = new Frame();
    if (!free_list_.empty())
    {
        frame = free_list_.front();
        free_list_.pop_front();
    }
    else
    {
        uint64_t page_id = findVictimFrame();
        frame = page_table_[page_id];
        if (!frame)return nullptr;
        if (!evictPage(page_id))return nullptr;
    }

    *page_id = next_page_id_++;

    auto new_page = std::make_unique<SlottedPage>(*page_id);
    assert(page_id != nullptr);
    frame->setPage(std::move(new_page));
    uint32_t* page_id_ = new uint32_t(*page_id);
    page_table_[*page_id_] =  frame;
    //replacer_->frameAllocated(*page_id);
    return frame->getPage();
}

bool BufferPoolManager::deletePage(page_id_t page_id)
{
    std::lock_guard<std::mutex> guard(latch_);
    if (page_table_.find(page_id) == page_table_.end())return true;

    Frame* frame = page_table_[page_id];
    SlottedPage* page = frame->getPage();

    page_table_.erase(page_id);
    replacer_->frameFreed(page_id);

    delete page;

    frame->reset();
    free_list_.push_back(frame);

    return true;
}

bool BufferPoolManager::flushAllPages()
{
    std::lock_guard<std::mutex> guard(latch_);
    bool success = true;

    for (auto& pair : page_table_)
    {
        if (!flushPage(pair.first))
        {
            return false;
        }
    }

    return success;
}

bool BufferPoolManager::evictPage(page_id_t page_id)
{
    page_table_.erase(page_id);
    return true;
}


int64_t BufferPoolManager::getCurrentTimestamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
        .count();
}
