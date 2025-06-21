//
// Created by Senad Lemes Galera on 13.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef FRAME_H
#define FRAME_H
// -------------------------------------------------------------------------------------
#include "../src/storage/slotted_page.h"
// -------------------------------------------------------------------------------------
class Frame
{
public:
    Frame() = default;
    ~Frame() = default;

    void setPage(SlottedPage* page);
    SlottedPage* getPage(){return page_;};
    [[nodiscard]] bool isOccupied() const;

    [[nodiscard]] bool getReferenced() const { return referenced_; };
    void setReferenced(bool referenced);
    void setDirty(bool is_dirty);

    page_id_t getPageId()
    {
        return page_no;
    }

private:
    SlottedPage* page_;
    page_id_t page_no;
    bool referenced_;
    bool is_dirty_;
};
// -------------------------------------------------------------------------------------
// Implementations below here
// -------------------------------------------------------------------------------------
inline void Frame::setPage(SlottedPage* page)
{
    page_ = page;
    referenced_ = true;
}


inline bool Frame::isOccupied() const
{
    return page_ != nullptr;
}

inline void Frame::setReferenced(bool referenced)
{
    referenced_ = referenced;
}

inline void Frame::setDirty(bool is_dirty)
{
    is_dirty_ = is_dirty;
}

// -------------------------------------------------------------------------------------
#endif //FRAME_H
// -------------------------------------------------------------------------------------