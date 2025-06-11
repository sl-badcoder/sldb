//
// Created by Senad Lemes Galera on 14.04.25.
//
// -------------------------------------------------------------------------------------
#include "buffer_replacement.h"
#include "basic_frame.h"
// -------------------------------------------------------------------------------------
// FIFO
// -------------------------------------------------------------------------------------
void FIFOReplacementStrategy::frameAllocated(Frame* frame)
{
    if (in_queue_.find(frame) == in_queue_.end())
    {
        queue_.push(frame);
        in_queue_.insert(frame);
    }
}

void FIFOReplacementStrategy::frameAccessed(Frame* frame)
{
    // FIFO doesnt nothing after accessing
}

void FIFOReplacementStrategy::frameFreed(Frame* frame)
{
    // FIFO doesnt nothing after freeing
}

Frame* FIFOReplacementStrategy::findVictim()
{
    Frame* victim = nullptr;

    if (!queue_.empty())
    {
        Frame* frame = queue_.front();
        SlottedPage* page = frame->getPage();


            victim = frame;
            queue_.pop();
            in_queue_.erase(frame);

    }

    return victim;
}
// -------------------------------------------------------------------------------------
// LRU
// -------------------------------------------------------------------------------------
void LRUReplacementStrategy::frameAllocated(Frame* frame)
{
    lru_list_.push_front(frame);
    lru_map_[frame] = lru_list_.begin();
}

void LRUReplacementStrategy::frameAccessed(Frame* frame)
{
    auto it = lru_map_.find(frame);
    if (it != lru_map_.end())
    {
        lru_list_.erase(it->second);
        lru_list_.push_front(frame);
        lru_map_[frame] = lru_list_.begin();
    }
}

void LRUReplacementStrategy::frameFreed(Frame* frame)
{
}

Frame* LRUReplacementStrategy::findVictim()
{
    Frame* victim = nullptr;

    for (auto it = lru_list_.rbegin(); it != lru_list_.rend(); ++it)
    {
        Frame* frame = *it;
        SlottedPage* page = frame->getPage();

        if (page && page->getPinCount() == 0)
        {
            // maybe look here if lru doesnt work right
            lru_list_.erase(lru_map_[frame]);
            lru_map_.erase(frame);
            return frame;
        }
    }

    return victim;
}

// -------------------------------------------------------------------------------------
// LFU
// -------------------------------------------------------------------------------------
void LFUReplacementStrategy::frameAllocated(Frame* frame)
{
    if (freq_list_.empty() || freq_list_.begin()->frequency > 1)
    {
        freq_list_.emplace_front(1);
    }

    auto freq_it = freq_list_.begin();
    freq_it->frames.push_back(frame);
    auto frame_it = prev(freq_it->frames.end());

    frame_map_[frame] = {freq_it, frame_it};
}

void LFUReplacementStrategy::frameFreed(Frame* frame)
{
}

void LFUReplacementStrategy::frameAccessed(Frame* frame)
{
    // find frequencynode -> delete frame -> input frame in next frequency node
    auto it = frame_map_[frame].first;
    it->frames.erase(frame_map_[frame].second);
    uint32_t freq = it->frequency;
    ++it;
    if (it != freq_list_.end())
    {
        it->frames.push_back(frame);
    }

    else
    {
        freq_list_.emplace_back(freq + 1);
        it->frames.push_back(frame);
    }
}

Frame* LFUReplacementStrategy::findVictim()
{
    if (!freq_list_.empty())
    {
        for (auto& it : freq_list_)
        {
            if (!it.frames.empty())
            {
                it.frames.erase(it.frames.begin());
                return it.frames.front();
            }
        }
    }
    return nullptr;
}

// -------------------------------------------------------------------------------------
// CLOCK
// -------------------------------------------------------------------------------------
void ClockReplacementStrategy::frameAllocated(Frame* frame)
{
    clock_buffer_.push_back(frame);
    frame->setReferenced(true);
}

void ClockReplacementStrategy::frameFreed(Frame* frame)
{
    for (auto it = clock_buffer_.begin(); it != clock_buffer_.end(); ++it)
    {
        if (*it == frame)
        {
            if (clock_hand_ >= std::distance(clock_buffer_.begin(), it))
            {
                clock_hand_--;
            }

            clock_buffer_.erase(it);

            if (!clock_buffer_.empty())
            {
                clock_hand_ %= clock_buffer_.size();
            }
            else
            {
                clock_hand_ = 0;
            }

            break;
        }
    }
}

void ClockReplacementStrategy::frameAccessed(Frame* frame)
{
    frame->setReferenced(true);
}

Frame* ClockReplacementStrategy::findVictim()
{
    if (clock_buffer_.empty())
    {
        return nullptr;
    }

    size_t start_position = clock_hand_;

    while (clock_buffer_[clock_hand_]->getReferenced())
    {
        clock_buffer_[clock_hand_]->setReferenced(false);
        clock_hand_ = (clock_hand_ + 1) % clock_buffer_.size();

        if (clock_hand_ == start_position)
        {
            break;
        }
    }

    Frame* victim = clock_buffer_[clock_hand_];

    clock_hand_ = (clock_hand_ + 1) % clock_buffer_.size();

    return victim;
}
// -------------------------------------------------------------------------------------
// TWOQUEUE
// -------------------------------------------------------------------------------------
void TwoQueueReplacementStrategy::frameAllocated(Frame* frame)
{
    total_frames_++;
    a1in_queue_.push_back(frame);
    a1in_map_[frame] = std::prev(a1in_queue_.end());
}

void TwoQueueReplacementStrategy::frameFreed(Frame* frame)
{
    if (total_frames_ > 0)total_frames_--;

    auto a1in_it = a1in_map_.find(frame);
    if (a1in_it != a1in_map_.end())
    {
        a1in_queue_.erase(a1in_it->second);
        a1in_map_.erase(a1in_it);
        return;
    }

    auto am_it = am_map_.find(frame);
    if (am_it != am_map_.end())
    {
        am_queue_.erase(am_it->second);
        am_map_.erase(am_it);
        return;
    }
}

void TwoQueueReplacementStrategy::frameAccessed(Frame* frame)
{
    page_id_t page_id = frame->getPage()->getPageId();

    auto a1in_it = a1in_map_.find(frame);
    if (a1in_it != a1in_map_.end())
    {
        return;
    }

    auto am_it = am_map_.find(frame);
    if (am_it != am_map_.end())
    {
        Frame* accessed_frame = *(am_it->second);
        am_queue_.erase(am_it->second);
        am_queue_.push_back(accessed_frame);
        am_map_[frame] = std::prev(a1in_queue_.end());
        return;
    }

    if (a1out_set_.find(page_id) != a1out_set_.end())
    {
        a1out_set_.erase(page_id);

        am_queue_.push_back(frame);
        am_map_[frame] = std::prev(am_queue_.end());
        return;
    }

    a1in_queue_.push_back(frame);
    a1in_map_[frame] = std::prev(a1in_queue_.end());
}

Frame* TwoQueueReplacementStrategy::findVictim()
{
    Frame* victim = nullptr;

    size_t a1in_limit = total_frames_ * a1in_size_ratio_;
    size_t a1out_limit = total_frames_ * a1out_size_ratio_;

    if (a1in_queue_.size() > a1in_limit && !a1in_queue_.empty())
    {
        victim = a1in_queue_.front();

        if (const SlottedPage* page = victim->getPage())
        {
            a1out_set_.insert(page->getPageId());
            if (a1out_set_.size() > a1out_limit && !a1out_set_.empty())
            {
                a1out_set_.erase(page->getPageId());
            }
        }

        a1in_queue_.pop_front();
        a1in_map_.erase(victim);
        return victim;
    }

    if (!am_queue_.empty())
    {
        victim = am_queue_.back();
        am_queue_.pop_front();
        am_map_.erase(victim);

        return victim;
    }

    if (!a1in_queue_.empty())
    {
        victim = a1in_queue_.front();

        if (const SlottedPage* page = victim->getPage())
        {
            a1out_set_.insert(page->getPageId());
            if (a1out_set_.size() > a1out_limit && !a1out_set_.empty())
            {
                a1out_set_.erase(page->getPageId());
            }
            a1in_queue_.pop_front();
            a1in_map_.erase(victim);
            return victim;
        }
    }

    return victim;
}
// -------------------------------------------------------------------------------------
