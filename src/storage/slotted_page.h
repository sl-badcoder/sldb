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
struct TID
{
    uint16_t page_id;
    uint16_t slot_id;
};

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
    explicit SlottedPage(uint64_t page_size);
    ~SlottedPage() = default;
    uint16_t allocate(size_t data_size);
    void erase(uint16_t slot_id);
    void resize(uint16_t slot_id, size_t data_size);

    char* getData()
    {
        return data;
    }
    char* getSlots()
    {
        return data + sizeof(PageHeader) + header.first_free_slot * sizeof(SlotDirectoryEntry);
    }
    PageHeader getHeader()
    {
        return header;
    }

private:
    PageHeader header;
    char* data;
};
// -------------------------------------------------------------------------------------
// Implementation details below here
// -------------------------------------------------------------------------------------
inline SlottedPage::SlottedPage(uint64_t page_size)
{
    header.data_start = page_size;
    header.first_free_slot = 0;
    header.free_space = page_size - sizeof(PageHeader);
    header.num_slots = 0;
}
// -------------------------------------------------------------------------------------
#endif //SLOTTED_PAGE_H
// -------------------------------------------------------------------------------------