//
// Created by Senad Lemes Galera on 14.04.25.
//
// -------------------------------------------------------------------------------------
#include "slotted_page.h"
// -------------------------------------------------------------------------------------
#include <vector>
#include <cstring>
#include <algorithm>
#include <iostream>
// -------------------------------------------------------------------------------------
uint16_t SlottedPage::allocate(size_t data_size)
{
    uint16_t slot_id = header.first_free_slot;
    header.data_start -= data_size;
    header.first_free_slot += 1;
    header.num_slots += 1;
    header.free_space -= data_size + sizeof(SlotDirectoryEntry);

    SlotDirectoryEntry* slot = getSlots() + slot_id;
    slot->setSlot(header.data_start, data_size);
    return slot_id;
}
//--------------------------------------------------------------------------------------
void SlottedPage::erase(uint16_t slot_id)
{
    SlotDirectoryEntry* slot = getSlots() + slot_id;
    uint16_t old_size = slot->length;
    slot->empty();

    header.num_slots -= 1;
    if(slot_id == header.first_free_slot - 1)
    {
        header.first_free_slot -= 1;
    }else
    {
        header.first_free_slot = slot_id;
    }
    header.free_space += old_size;
}
//--------------------------------------------------------------------------------------
void SlottedPage::resize(uint16_t slot_id, size_t data_size)
{
    header.data_start -= data_size;
    if(getSlots() != nullptr)
    {
        SlotDirectoryEntry* slot = getSlots() + slot_id;
        uint16_t old_size = slot->length;
        // if old size is smaller than new size just move the input else erase it
        if(old_size <= data_size)
        {
            std::memmove(getData() + header.data_start, getData() + slot->offset, old_size);
        }
        slot->length = data_size;
        slot->offset = header.data_start;
        header.free_space += old_size - data_size;
    }
}
//--------------------------------------------------------------------------------------
void SlottedPage::compactify(uint64_t page_size)
{
    int num_slots = header.num_slots;
    std::vector<SlotDirectoryEntry> valid_slots;
    valid_slots.reserve(num_slots);
    for(int i = 0; i < num_slots; i++)
    {
        SlotDirectoryEntry slot = getSlots()[i];
        if(!slot.isEmpty())valid_slots.push_back(slot);
    }
    header.data_start = page_size;
    header.num_slots = 0;
    header.first_free_slot = 0;
    header.free_space = page_size - sizeof(SlottedPage);
    for(int i =0 ; i < valid_slots.size(); i++)
    {
        getSlots()[i] = valid_slots[i];
        header.data_start -= getSlots()[i].length;
        getSlots()[i].offset = header.data_start;
        //TODO memmove data
        // store the data somewhere and then move it back else
        // could be some weird problems with overwritting data
        header.num_slots++;
        header.first_free_slot++;
        header.free_space -= getSlots()[i].length;
    }
}
//--------------------------------------------------------------------------------------
