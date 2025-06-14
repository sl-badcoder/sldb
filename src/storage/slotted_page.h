//
// Created by Senad Lemes Galera on 13.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef SLOTTED_PAGE_H
#define SLOTTED_PAGE_H
// -------------------------------------------------------------------------------------
#include <cstdint>
#include <memory>
#include <chrono>
// -------------------------------------------------------------------------------------
static constexpr uint16_t INVALID_SLOT_NUM = UINT16_MAX;
typedef uint32_t page_id_t;
// -------------------------------------------------------------------------------------
struct PageHeader
{
    uint16_t num_slots;
    uint16_t first_free_slot;
    uint16_t data_start;
    uint16_t free_space;
};

// -------------------------------------------------------------------------------------
struct SlotDirectoryEntry
{
    uint16_t offset;
    uint16_t length;

    SlotDirectoryEntry() : offset(0), length(0)
    {}

    SlotDirectoryEntry(uint16_t offset, uint16_t length)
        : offset(offset), length(length)
    {}
};
// -------------------------------------------------------------------------------------
class SlottedPage
{
public:
    // -------------------------------------------------------------------------------------
    explicit SlottedPage(page_id_t page_id);
    ~SlottedPage() = default;
};
// -------------------------------------------------------------------------------------
// Implementation details below here
// -------------------------------------------------------------------------------------
inline SlottedPage::SlottedPage(page_id_t page_id)
{
}
// -------------------------------------------------------------------------------------
#endif //SLOTTED_PAGE_H
// -------------------------------------------------------------------------------------