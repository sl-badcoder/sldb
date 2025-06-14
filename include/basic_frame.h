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

    void setPage(std::unique_ptr<SlottedPage> page);
    SlottedPage* getPage();
    [[nodiscard]] bool isOccupied() const;
    void reset();

    [[nodiscard]] bool getReferenced() const { return referenced_; };
    void setReferenced(bool referenced);

    std::unique_ptr<SlottedPage> releasePage()
    {
        return std::move(page_);
    }
    page_id_t getPageId()
    {
        return page_no;
    }

private:
    std::unique_ptr<SlottedPage> page_;
    page_id_t page_no;
    bool referenced_;
};
// -------------------------------------------------------------------------------------
// Implementations below here
// -------------------------------------------------------------------------------------
inline void Frame::setPage(std::unique_ptr<SlottedPage> page)
{
    page_ = std::move(page);
    referenced_ = true;
}

inline SlottedPage* Frame::getPage()
{
    return page_.get();
}

inline bool Frame::isOccupied() const
{
    return page_ != nullptr;
}

inline void Frame::reset()
{
    page_.reset();
    referenced_ = true;
}

inline void Frame::setReferenced(bool referenced)
{
    referenced_ = referenced;
}
// -------------------------------------------------------------------------------------
#endif //FRAME_H
// -------------------------------------------------------------------------------------