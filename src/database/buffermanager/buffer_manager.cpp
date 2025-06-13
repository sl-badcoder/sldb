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
SlottedPage* BufferPoolManager::fetchPage(page_id_t page_id)
{
    std::lock_guard<std::mutex> guard(latch_);

    if (page_table_.find(page_id) != page_table_.end())
    {
        Frame* frame = page_table_[page_id];
        SlottedPage* page = frame->getPage();

        page->incrementPinCount();
        page->updateAccessTime(getCurrentTimestamp());
        page->incrementAccessCount();

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
        if (!evictPage(frame))return nullptr;
    }

    auto page = std::make_unique<SlottedPage>(page_id);


    off_t offset = page_id * SlottedPage::PAGE_SIZE;
    file.read_block(offset, SlottedPage::PAGE_SIZE, page->getData());

    frame->setPage(std::move(page));
    page->incrementPinCount();
    page->updateAccessTime(getCurrentTimestamp());

    page_table_[page_id] = frame;
    replacer_->frameAllocated(page_id);

    return frame->getPage();
}

bool BufferPoolManager::unpinPage(page_id_t page_id, bool is_dirty)
{
    std::lock_guard<std::mutex> guard(latch_);

    if (page_table_.find(page_id) == page_table_.end())
    {
        return false;
    }

    Frame* frame = page_table_[page_id];
    SlottedPage* page = frame->getPage();

    if (page->getPinCount() <= 0)return false;

    page->decrementPinCount();

    if (is_dirty)page->setDirty(true);

    if (page->getPinCount() == 0)frame->setReferenced(false);

    return true;
}

bool BufferPoolManager::flushPage(page_id_t page_id)
{
    std::lock_guard<std::mutex> guard(latch_);

    if (page_table_.find(page_id) == page_table_.end())return false;

    Frame* frame = page_table_[page_id];
    SlottedPage* page = frame->getPage();

    off_t offset = page_id * SlottedPage::PAGE_SIZE;
    file.write_block(page->getData(), offset, SlottedPage::PAGE_SIZE);
    page->setDirty(false);

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
        if (!evictPage(frame))return nullptr;
    }

    *page_id = next_page_id_++;

    auto new_page = std::make_unique<SlottedPage>(*page_id);
    new_page->incrementPinCount();
    new_page->updateAccessTime(getCurrentTimestamp());
    new_page->incrementAccessCount();
    assert(page_id != nullptr);
    frame->setPage(std::move(new_page));
    uint32_t* page_id_ = new uint32_t(*page_id);
    page_table_[*page_id_] =  frame;
    replacer_->frameAllocated(*page_id);
    return frame->getPage();
}

bool BufferPoolManager::deletePage(page_id_t page_id)
{
    std::lock_guard<std::mutex> guard(latch_);
    if (page_table_.find(page_id) == page_table_.end())return true;

    Frame* frame = page_table_[page_id];
    SlottedPage* page = frame->getPage();

    if (page->getPinCount() > 0)return false;

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

bool BufferPoolManager::evictPage(Frame* frame)
{
    SlottedPage* page = frame->getPage();

    if (!page)return true;

    page_id_t page_id = page->getPageId();

    //if (page->getPinCount() > 0)return false;

    if (page->isDirty())
    {
        off_t offset = page_id * SlottedPage::PAGE_SIZE;
        file.write_block(page->getData(), offset, SlottedPage::PAGE_SIZE);;
    }

    page_table_.erase(page_id);

    //delete page;
    frame->reset();

    return true;
}


int64_t BufferPoolManager::getCurrentTimestamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
        .count();
}
