//
// Created by Senad Lemes Galera on 13.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef SLOTTED_PAGE_H
#define SLOTTED_PAGE_H
// -------------------------------------------------------------------------------------
#include <cstdint>
#include <memory>
// -------------------------------------------------------------------------------------
static constexpr uint16_t INVALID_SLOT_NUM = UINT16_MAX;
typedef uint32_t page_id_t;
// -------------------------------------------------------------------------------------
struct PageHeader
{
    uint16_t num_slots;
    std::unique_ptr<uint16_t> free_space_ptr;
};

// -------------------------------------------------------------------------------------
struct SlotDirectoryEntry
{
    uint16_t offset;
    uint16_t length;

    SlotDirectoryEntry() : offset(0), length(0)
    {
    }

    SlotDirectoryEntry(uint16_t offset, uint16_t length)
        : offset(offset), length(length)
    {
    }
};
// -------------------------------------------------------------------------------------
struct SlotId
{
    page_id_t page_id;
    uint16_t slot_num;

    bool operator==(const SlotId& other) const
    {
        return page_id == other.page_id && slot_num == other.slot_num;
    }
};
// -------------------------------------------------------------------------------------
namespace std
{
    template <>
    struct hash<SlotId>
    {
        size_t operator()(const SlotId& id) const
        {
            return hash<page_id_t>()(id.page_id) ^ hash<uint16_t>()(id.slot_num);
        }
    };
}
// -------------------------------------------------------------------------------------
class SlottedPage
{
public:
    static constexpr size_t PAGE_SIZE = 4096;
    // -------------------------------------------------------------------------------------
    explicit SlottedPage(page_id_t page_id);
    ~SlottedPage() = default;
    // -------------------------------------------------------------------------------------
    [[nodiscard]] page_id_t getPageId() const { return page_id_; };
    [[nodiscard]] bool isDirty() const { return is_dirty_; };
    void setDirty(bool dirty) { is_dirty_ = true; };
    [[nodiscard]] uint32_t getPinCount() const { return pin_count_; };
    void incrementPinCount() { pin_count_++; };
    void decrementPinCount() { pin_count_--; };
    // -------------------------------------------------------------------------------------
    [[nodiscard]] uint32_t getAccessCount() const { return access_count_; };
    void incrementAccessCount() { access_count_++; };
    [[nodiscard]] int64_t getLastAccessTime() const { return last_access_time_; };
    void updateAccessTime(int64_t time) { last_access_time_ = time; };
    // -------------------------------------------------------------------------------------
    [[nodiscard]] bool hasEnoughSpace(uint16_t size) const;
    uint16_t insertRecord(const char* data, uint16_t length);
    bool updateRecord(uint16_t slot_num, const char* data, uint16_t length);
    bool deleteRecord(uint16_t slot_num);
    char* getRecord(uint16_t slot_num, uint16_t* length);
    const char* getRecord(uint16_t slot_num, uint16_t* length) const;
    char* getData() { return data_; };
    const char* getData() const { return data_; }
    PageHeader* getHeader();
    const PageHeader* getHeader() const;
    SlotDirectoryEntry* getSlot(uint16_t index);
    const SlotDirectoryEntry* getSlot(uint16_t index) const;
    // -------------------------------------------------------------------------------------
private:
    page_id_t page_id_;
    char data_[PAGE_SIZE]{};
    bool is_dirty_;
    uint32_t pin_count_;

    uint32_t access_count_;
    int64_t last_access_time_;
};
// -------------------------------------------------------------------------------------
// Implementation details below here
// -------------------------------------------------------------------------------------
inline const PageHeader* SlottedPage::getHeader() const
{
    return reinterpret_cast<const PageHeader*>(data_);
}

inline PageHeader* SlottedPage::getHeader()
{
    return reinterpret_cast<PageHeader*>(data_);
}

inline const SlotDirectoryEntry* SlottedPage::getSlot(uint16_t index) const
{
    return reinterpret_cast<const SlotDirectoryEntry*>(
        data_ + sizeof(PageHeader) + index * sizeof(SlotDirectoryEntry));
}

inline SlotDirectoryEntry* SlottedPage::getSlot(uint16_t index)
{
    return reinterpret_cast<SlotDirectoryEntry*>(
        data_ + sizeof(PageHeader) + index * sizeof(SlotDirectoryEntry));
}


inline SlottedPage::SlottedPage(page_id_t page_id)
{
    is_dirty_ = false;
    pin_count_ = 0;
    access_count_ = 0;
    last_access_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
        .count();
    page_id_ = page_id;
}

// -------------------------------------------------------------------------------------
#endif //SLOTTED_PAGE_H
// -------------------------------------------------------------------------------------