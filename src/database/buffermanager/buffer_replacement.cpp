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
}

void LRUReplacementStrategy::frameAccessed(uint64_t frame)
{
}

void LRUReplacementStrategy::frameFreed(uint64_t frame)
{
}

uint64_t LRUReplacementStrategy::findVictim()
{
    return 0;
}

// -------------------------------------------------------------------------------------
// LFU
// -------------------------------------------------------------------------------------
void LFUReplacementStrategy::frameAllocated(uint64_t frame)
{

}

void LFUReplacementStrategy::frameFreed(uint64_t frame)
{
}

void LFUReplacementStrategy::frameAccessed(uint64_t frame)
{
    // find frequencynode -> delete frame -> input frame in next frequency node
}

uint64_t LFUReplacementStrategy::findVictim()
{
    return 0;
}

// -------------------------------------------------------------------------------------
// CLOCK
// -------------------------------------------------------------------------------------
void ClockReplacementStrategy::frameAllocated(uint64_t frame)
{
}

void ClockReplacementStrategy::frameFreed(uint64_t frame)
{
}

void ClockReplacementStrategy::frameAccessed(uint64_t frame)
{
}

uint64_t ClockReplacementStrategy::findVictim()
{
    return 0;
}
// -------------------------------------------------------------------------------------
// TWOQUEUE
// -------------------------------------------------------------------------------------
void TwoQueueReplacementStrategy::frameAllocated(uint64_t frame)
{
}

void TwoQueueReplacementStrategy::frameFreed(uint64_t frame)
{

}

void TwoQueueReplacementStrategy::frameAccessed(uint64_t frame)
{
}

uint64_t TwoQueueReplacementStrategy::findVictim()
{
    return 0;
}
// -------------------------------------------------------------------------------------
