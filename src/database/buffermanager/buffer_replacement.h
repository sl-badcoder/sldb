//
// Created by Senad Lemes Galera on 09.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef BUFFER_REPLACEMENT_H
#define BUFFER_REPLACEMENT_H
// -------------------------------------------------------------------------------------
#include "basic_frame.h"
// -------------------------------------------------------------------------------------
#include <queue>
#include <set>
#include <list>
#include <unordered_map>
// -------------------------------------------------------------------------------------
enum class BUFFER_REPLACEMENT { FIFO, LRU, LFU, CLOCK, TQ };

// -------------------------------------------------------------------------------------
// Replacement Strategy
// -------------------------------------------------------------------------------------
class ReplacementStrategy
{
public:
    virtual ~ReplacementStrategy() = default;

    virtual void frameAllocated(uint64_t page_id) = 0;
    virtual void frameAccessed(uint64_t page_id) = 0;
    virtual void frameFreed(uint64_t page_id) = 0;
    virtual uint64_t findVictim() = 0;
};

// -------------------------------------------------------------------------------------
// FIFO Replacement Strategy
// -------------------------------------------------------------------------------------
class FIFOReplacementStrategy : public ReplacementStrategy
{
public:
    FIFOReplacementStrategy() = default;
    ~FIFOReplacementStrategy() override = default;

    void frameAllocated(uint64_t frame) override;
    void frameAccessed(uint64_t frame) override;
    void frameFreed(uint64_t frame) override;
    uint64_t findVictim() override;

private:
    std::queue<uint64_t> queue_;
    std::unordered_map<uint64_t, uint64_t> in_queue_;
};

// -------------------------------------------------------------------------------------
// LRU Replacement Strategy
// -------------------------------------------------------------------------------------
class LRUReplacementStrategy : public ReplacementStrategy
{
public:
    LRUReplacementStrategy() = default;
    ~LRUReplacementStrategy() override = default;

    void frameAllocated(uint64_t frame) override;
    void frameAccessed(uint64_t frame) override;
    void frameFreed(uint64_t frame) override;
    uint64_t findVictim() override;

private:
    std::list<uint64_t> lru_list_;
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> lru_map_;
};

// -------------------------------------------------------------------------------------
// LFU Replacement Strategy
// -------------------------------------------------------------------------------------
class LFUReplacementStrategy : public ReplacementStrategy
{
public:
    LFUReplacementStrategy() = default;
    ~LFUReplacementStrategy() override = default;

    void frameAllocated(uint64_t frame) override;
    void frameAccessed(uint64_t frame) override;
    void frameFreed(uint64_t frame) override;
    uint64_t findVictim() override;

private:

    std::unordered_map<uint64_t, uint64_t> page_freq_;
    std::unordered_map<uint64_t, std::list<uint64_t>> freq_list;
    uint64_t minFreq = 0;
};

// -------------------------------------------------------------------------------------
// CLOCK Replacement Strategy
// -------------------------------------------------------------------------------------
class ClockReplacementStrategy : public ReplacementStrategy
{
public:
    ClockReplacementStrategy()
    {
        clock_hand_ = 0;
        clock_buffer_ = {};
    };
    ~ClockReplacementStrategy() override = default;

    void frameAllocated(uint64_t frame) override;
    void frameAccessed(uint64_t frame) override;
    void frameFreed(uint64_t frame) override;
    uint64_t findVictim() override;

private:
    std::vector<uint64_t> clock_buffer_;
    std::unordered_map<uint64_t, bool> clock_freq_;
    size_t clock_hand_;
};

// -------------------------------------------------------------------------------------
// 2Q Replacement Strategy
// -------------------------------------------------------------------------------------
class TwoQueueReplacementStrategy : public ReplacementStrategy
{
public:
    TwoQueueReplacementStrategy() = default;
    ~TwoQueueReplacementStrategy() override = default;

    void frameAllocated(uint64_t frame) override;
    void frameAccessed(uint64_t frame) override;
    void frameFreed(uint64_t frame) override;
    uint64_t findVictim() override;

private:
    std::list<uint64_t> fifo_in_;
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> fifo_map_;

    std::list<uint64_t> lru_in_;
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> lru_map_;

};

// -------------------------------------------------------------------------------------
#endif //BUFFER_REPLACEMENT_H
// -------------------------------------------------------------------------------------
