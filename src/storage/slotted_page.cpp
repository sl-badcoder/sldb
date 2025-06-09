//
// Created by Senad Lemes Galera on 14.04.25.
//
// -------------------------------------------------------------------------------------
#include "slotted_page.h"
// -------------------------------------------------------------------------------------
#include <vector>
#include <cstring>
#include <algorithm>
// -------------------------------------------------------------------------------------
bool SlottedPage::hasEnoughSpace(uint16_t size) const
{
    const PageHeader* header = getHeader();
    uint16_t num_slots = header->num_slots;
    uint16_t free_space_ptr = *(header->free_space_ptr);

    uint16_t slot_dir_end = sizeof(PageHeader) + num_slots * sizeof(SlotDirectoryEntry);

    bool need_new_slot = true;
    for (uint16_t i = 0; i < num_slots; i++)
    {
        const SlotDirectoryEntry* slot = getSlot(i);
        if (slot->length == 0)
        {
            need_new_slot = false;
            break;
        }
    }

    uint16_t required_space = size;
    if (need_new_slot)
    {
        required_space += sizeof(SlotDirectoryEntry);
    }

    return (free_space_ptr - slot_dir_end) >= required_space;
}

uint16_t SlottedPage::insertRecord(const char* data, uint16_t length)
{
    PageHeader* header = getHeader();
    uint16_t num_slots = header->num_slots;
    uint16_t free_space_ptr = *(header->free_space_ptr);

    if (!hasEnoughSpace(length))
    {
        return INVALID_SLOT_NUM;
    }

    uint16_t slot_num = 0;
    bool found_free_slot = false;

    for (; slot_num < num_slots; slot_num++)
    {
        SlotDirectoryEntry* slot = getSlot(slot_num);
        if (slot->length == 0)
        {
            found_free_slot = true;
            break;
        }
    }

    if (!found_free_slot)
    {
        slot_num = num_slots;
        header->num_slots++;
    }

    free_space_ptr -= length;
    SlotDirectoryEntry* slot = getSlot(slot_num);
    slot->offset = free_space_ptr;
    slot->length = length;

    std::memcpy(data_ + free_space_ptr, data, length);

    is_dirty_ = true;
    return slot_num;
}

bool SlottedPage::updateRecord(uint16_t slot_num, const char* data, uint16_t length)
{
    PageHeader* header = getHeader();
    uint16_t num_slots = header->num_slots;

    if (slot_num >= num_slots)return false;

    SlotDirectoryEntry* slot = getSlot(slot_num);

    if (slot->length == 0)return false;

    if (slot->length == length)
    {
        memcpy(data_ + slot->offset, data, length);
        is_dirty_ = true;
        return true;
    }

    if (!deleteRecord(slot_num)) return false;

    uint16_t new_slot = insertRecord(data, length);
    if (new_slot == INVALID_SLOT_NUM)return false;

    if (new_slot != slot_num)
    {
        SlotDirectoryEntry* new_slot_entry = getSlot(new_slot);
        SlotDirectoryEntry* old_slot_entry = getSlot(slot_num);

        old_slot_entry->length = new_slot_entry->length;
        old_slot_entry->offset = old_slot_entry->offset;

        new_slot_entry->offset = 0;
        new_slot_entry->length = 0;
    }

    return true;
}

bool SlottedPage::deleteRecord(uint16_t slot_num)
{
    PageHeader* header = getHeader();
    uint16_t num_slots = header->num_slots;

    if (slot_num >= num_slots)return false;

    SlotDirectoryEntry* slot = getSlot(slot_num);

    if (slot->length == 0) return false;

    slot->length = 0;

    std::vector<std::pair<uint16_t, SlotDirectoryEntry>> valid_records;
    for (uint16_t i = 0; i < num_slots; i++)
    {
        SlotDirectoryEntry* curr_slot = getSlot(i);
        if (curr_slot->length > 0)valid_records.emplace_back(i, *curr_slot);
    }

    std::sort(valid_records.begin(), valid_records.end(), [](const auto& a, const auto& b)
    {
        return a.second.offset > b.second.offset;
    });

    uint16_t free_space_ptr = PAGE_SIZE;

    for (auto& records : valid_records)
    {
        uint16_t curr_slot_num = records.first;
        uint16_t curr_slot_offset = records.second.offset;
        uint16_t curr_slot_length = records.second.length;

        free_space_ptr -= curr_slot_length;

        if (free_space_ptr != curr_slot_offset)
        {
            memmove(data_ + free_space_ptr, data_ + curr_slot_offset, curr_slot_length);
            SlotDirectoryEntry* curr_slot = getSlot(curr_slot_num);
            curr_slot->offset = free_space_ptr;
        }
    }

    *(header->free_space_ptr) = free_space_ptr;

    is_dirty_ = true;

    return true;
}

char* SlottedPage::getRecord(uint16_t slot_num, uint16_t* length)
{
    PageHeader* header = getHeader();
    uint16_t num_slots = header->num_slots;

    if (slot_num >= num_slots)
    {
        if (length != nullptr)*length = 0;
        return nullptr;
    }
    SlotDirectoryEntry* slot = getSlot(slot_num);
    if (slot->length == 0)
    {
        if (length != nullptr) *length = 0;
        return nullptr;
    }

    if (length != nullptr)*length = slot->length;
    return data_ + slot->offset;
}

const char* SlottedPage::getRecord(uint16_t slot_num, uint16_t* length) const
{
    const PageHeader* header = getHeader();
    uint16_t num_slots = header->num_slots;

    if (slot_num >= num_slots)
    {
        if (length != nullptr)*length = 0;
        return nullptr;
    }
    const SlotDirectoryEntry* slot = getSlot(slot_num);
    if (slot->length == 0)
    {
        if (length != nullptr) *length = 0;
        return nullptr;
    }

    if (length != nullptr)*length = slot->length;
    return data_ + slot->offset;
}
// -------------------------------------------------------------------------------------