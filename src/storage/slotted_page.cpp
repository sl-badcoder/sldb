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


void SlottedPage::resize(uint16_t slot_id, size_t data_size)
{
    header.data_start -= data_size;
    if(getSlots() != nullptr)
    {
        SlotDirectoryEntry* slot = getSlots() + slot_id;
        uint16_t old_size = slot->length;
        slot->length = data_size;
        slot->offset = header.data_start;
        header.free_space += old_size - data_size;
    }
}
