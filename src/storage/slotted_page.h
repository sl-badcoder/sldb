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
#include <cstring>
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
    void setSlot(uint16_t offset, uint16_t length) {
        this->offset = offset;
        this->length = length;
    }
};
// -------------------------------------------------------------------------------------
struct SlottedPage
{
    // -------------------------------------------------------------------------------------
    explicit SlottedPage(uint64_t page_size);
    ~SlottedPage() = default;
    uint16_t allocate(size_t data_size);
    void erase(uint16_t slot_id);
    void resize(uint16_t slot_id, size_t data_size);
    //-------------------------------------------------------------------------------------
    std::byte *getData();
    const std::byte *getData() const;
    SlotDirectoryEntry *getSlots();
    const SlotDirectoryEntry* getSlots() const;
    //-------------------------------------------------------------------------------------
    PageHeader header;
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
    std::memset(getData() + sizeof(SlottedPage), 0x00, page_size - sizeof(SlottedPage));
}

inline std::byte* SlottedPage::getData() {
    return reinterpret_cast<std::byte*>(this);
}

inline const std::byte* SlottedPage::getData() const {
    return reinterpret_cast<const std::byte*>(this);
}

inline SlotDirectoryEntry *SlottedPage::getSlots() {
    return reinterpret_cast<SlotDirectoryEntry *>(getData() + sizeof(SlottedPage));
}

inline const SlotDirectoryEntry *SlottedPage::getSlots() const {
    return reinterpret_cast<const SlotDirectoryEntry *>(getData() + sizeof(SlottedPage));
}
// -------------------------------------------------------------------------------------
#endif //SLOTTED_PAGE_H
// -------------------------------------------------------------------------------------