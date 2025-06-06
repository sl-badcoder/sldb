//
// Created by Senad Lemes Galera on 09.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef BUFFER_REPLACEMENT_H
#define BUFFER_REPLACEMENT_H
// -------------------------------------------------------------------------------------
#include "src/storage/basic_frame.h"
// -------------------------------------------------------------------------------------
#include <queue>
#include <set>
#include <list>

// -------------------------------------------------------------------------------------
enum class BUFFER_REPLACEMENT { FIFO, LRU, LFU, CLOCK, TQ };

// -------------------------------------------------------------------------------------
// Replacement Strategy
// -------------------------------------------------------------------------------------
class ReplacementStrategy
{
public:
    virtual ~ReplacementStrategy() = default;

    virtual void frameAllocated(Frame* frame) = 0;
    virtual void frameAccessed(Frame* frame) = 0;
    virtual void frameFreed(Frame* frame) = 0;
    virtual Frame* findVictim() = 0;
};

// -------------------------------------------------------------------------------------
// FIFO Replacement Strategy
// -------------------------------------------------------------------------------------
class FIFOReplacementStrategy : public ReplacementStrategy
{
public:
    FIFOReplacementStrategy() = default;
    ~FIFOReplacementStrategy() override = default;

    void frameAllocated(Frame* frame) override;
    void frameAccessed(Frame* frame) override;
    void frameFreed(Frame* frame) override;
    Frame* findVictim() override;

private:
    std::queue<Frame*> queue_;
    std::set<Frame*> in_queue_;
};

// -------------------------------------------------------------------------------------
// LRU Replacement Strategy
// -------------------------------------------------------------------------------------
class LRUReplacementStrategy : public ReplacementStrategy
{
public:
    LRUReplacementStrategy() = default;
    ~LRUReplacementStrategy() override = default;

    void frameAllocated(Frame* frame) override;
    void frameAccessed(Frame* frame) override;
    void frameFreed(Frame* frame) override;
    Frame* findVictim() override;

private:
    std::list<Frame*> lru_list_;
    std::unordered_map<Frame*, std::list<Frame*>::iterator> lru_map_; // For O(1) access
};

// -------------------------------------------------------------------------------------
// LFU Replacement Strategy
// -------------------------------------------------------------------------------------
class LFUReplacementStrategy : public ReplacementStrategy
{
public:
    LFUReplacementStrategy() = default;
    ~LFUReplacementStrategy() override = default;

    void frameAllocated(Frame* frame) override;
    void frameAccessed(Frame* frame) override;
    void frameFreed(Frame* frame) override;
    Frame* findVictim() override;

private:
    struct FrequencyNode
    {
        uint32_t frequency;
        std::list<Frame*> frames;

        explicit FrequencyNode(uint32_t freq) : frequency(freq)
        {
        }
    };

    std::list<FrequencyNode> freq_list_; // Sorted by frequency
    std::unordered_map<Frame*, std::pair<std::list<FrequencyNode>::iterator, std::list<Frame*>::iterator>> frame_map_;
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

    void frameAllocated(Frame* frame) override;
    void frameAccessed(Frame* frame) override;
    void frameFreed(Frame* frame) override;
    Frame* findVictim() override;

private:
    std::vector<Frame*> clock_buffer_;
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

    void frameAllocated(Frame* frame) override;
    void frameAccessed(Frame* frame) override;
    void frameFreed(Frame* frame) override;
    Frame* findVictim() override;

private:
    std::list<Frame*> a1in_queue_;
    std::unordered_map<Frame*, std::list<Frame*>::iterator> a1in_map_;

    std::list<Frame*> am_queue_;
    std::unordered_map<Frame*, std::list<Frame*>::iterator> am_map_;

    std::set<page_id_t> a1out_set_;

    size_t a1in_size_ratio_;
    size_t a1out_size_ratio_;
    size_t total_frames_;
};

// -------------------------------------------------------------------------------------
#endif //BUFFER_REPLACEMENT_H
// -------------------------------------------------------------------------------------
