//
// Created by Senad Lemes Galera on 14.04.25.
//
// -------------------------------------------------------------------------------------
#include "buffer_replacement.h"
#include "basic_frame.h"
// -------------------------------------------------------------------------------------
// FIFO
// -------------------------------------------------------------------------------------
void FIFOReplacementStrategy::frameAllocated(uint64_t frame)
{
    in_queue_[frame] = frame;
    if(in_queue_.find(frame) != in_queue_.end())queue_.push(frame);
}

void FIFOReplacementStrategy::frameAccessed(uint64_t frame)
{
    // FIFO does nothing after accessing
}

void FIFOReplacementStrategy::frameFreed(uint64_t frame)
{
    // FIFO does nothing after freeing
}

uint64_t FIFOReplacementStrategy::findVictim()
{
    uint64_t victim = 0;
    if(!queue_.empty())
    {
        // get first element and remove it
        victim = queue_.front();
        queue_.pop();
        in_queue_.erase(victim);
    }
    return victim;
}
// -------------------------------------------------------------------------------------
// LRU
// -------------------------------------------------------------------------------------
void LRUReplacementStrategy::frameAllocated(uint64_t frame)
{
    auto it = lru_list_.insert(lru_list_.end(), frame);
    lru_map_[frame] = it;
}

void LRUReplacementStrategy::frameAccessed(uint64_t frame)
{
    if(lru_map_.find(frame) != lru_map_.end())
    {
        lru_list_.erase(lru_map_[frame]);
        auto it = lru_list_.insert(lru_list_.end(), frame);
        lru_map_[frame] = it;
    }
}

void LRUReplacementStrategy::frameFreed(uint64_t frame)
{
}

uint64_t LRUReplacementStrategy::findVictim()
{
    uint64_t victim = 0;
    victim = lru_list_.front();
    lru_list_.pop_front();
    return victim;
}

// -------------------------------------------------------------------------------------
// LFU
// -------------------------------------------------------------------------------------
void LFUReplacementStrategy::frameAllocated(uint64_t frame)
{
    page_freq_[frame] = 0;
    freq_list[0].push_back(frame);
}

void LFUReplacementStrategy::frameFreed(uint64_t frame)
{
}

void LFUReplacementStrategy::frameAccessed(uint64_t frame)
{
    if(page_freq_.find(frame) != page_freq_.end())
    {
        uint64_t freq = page_freq_[frame];
        page_freq_[frame] += 1;
        freq_list[freq].remove(frame);
        freq += 1;
        freq_list[freq].push_back(frame);
        while(freq_list[minFreq].empty())
        {
            if(freq_list.size() - 1 == minFreq)break;
            minFreq++;
        }
    }
}

uint64_t LFUReplacementStrategy::findVictim()
{
    uint64_t victim = freq_list[minFreq].front();
    freq_list[minFreq].pop_front();
    while(freq_list[minFreq].empty())
    {
        if(freq_list.size() - 1 == minFreq)
        {
            if(freq_list[minFreq + 1].empty())minFreq = 0;
            break;
        }
        minFreq++;
    }
    return victim;
}

// -------------------------------------------------------------------------------------
// CLOCK
// -------------------------------------------------------------------------------------
void ClockReplacementStrategy::frameAllocated(uint64_t frame)
{
    clock_buffer_.push_back(frame);
    clock_freq_[frame] = false;
}

void ClockReplacementStrategy::frameFreed(uint64_t frame)
{
    if(clock_freq_.find(frame) != clock_freq_.end())clock_freq_[frame] = false;
}

void ClockReplacementStrategy::frameAccessed(uint64_t frame)
{
    if(clock_freq_.find(frame) != clock_freq_.end())clock_freq_[frame] = true;
}

uint64_t ClockReplacementStrategy::findVictim()
{
    uint64_t victim = 0;
    while (true)
    {
        if(!clock_freq_[clock_buffer_[clock_hand_]])
        {
            victim = clock_buffer_[clock_hand_];
            clock_buffer_.erase(clock_buffer_.begin() + clock_hand_);
            clock_freq_.erase(victim);
            break;
        }
        clock_freq_[clock_buffer_[clock_hand_]] = false;
        clock_hand_++ % clock_buffer_.size();
    }
    return victim;
}
// -------------------------------------------------------------------------------------
// TWOQUEUE
// -------------------------------------------------------------------------------------
void TwoQueueReplacementStrategy::frameAllocated(uint64_t frame)
{
    auto it = fifo_in_.insert(fifo_in_.end(), frame);
    fifo_map_[frame] = it;
}

void TwoQueueReplacementStrategy::frameFreed(uint64_t frame)
{

}

void TwoQueueReplacementStrategy::frameAccessed(uint64_t frame)
{
    if(fifo_map_.find(frame) != fifo_map_.end())
    {
        fifo_in_.erase(fifo_map_[frame]);
        fifo_map_.erase(frame);

        auto it = lru_in_.insert(lru_in_.end(), frame);
        lru_map_[frame] = it;
    }else if(lru_map_.find(frame) != lru_map_.end())
    {
        lru_in_.erase(lru_map_[frame]);
        auto it = lru_in_.insert(lru_in_.end(),frame);
        lru_map_[frame] = it;
    }
}

uint64_t TwoQueueReplacementStrategy::findVictim()
{
    uint64_t victim = 0;
    if(!fifo_in_.empty())
    {
        victim = fifo_in_.front();
        fifo_in_.pop_front();
        fifo_map_.erase(victim);
    }else
    {
        victim = lru_in_.front();
        lru_in_.pop_front();
        lru_map_.erase(victim);
    }
    return victim;
}
// -------------------------------------------------------------------------------------
