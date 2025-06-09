//
// Created by Senad Lemes Galera on 13.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef ADVANCED_FRAME_H
#define ADVANCED_FRAME_H
// -------------------------------------------------------------------------------------
#include <mutex>
// -------------------------------------------------------------------------------------
enum class state_t {CLEAN, DIRTY, NEW};
// -------------------------------------------------------------------------------------
class Frame
{
public:
    static constexpr size_t PAGE_SIZE = 4096;

    Frame();
    ~Frame();

    bool getPageNo();
    int getLSN();
    state_t getState();
    bool getData();

private:
    bool pageNo_;
    std::mutex latch_;
    int LSN;
    state_t state;
    char data_[PAGE_SIZE];
};
// -------------------------------------------------------------------------------------
#endif //ADVANCED_FRAME_H
// -------------------------------------------------------------------------------------