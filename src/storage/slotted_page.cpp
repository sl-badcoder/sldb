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

void SlottedPage::allocate(size_t data_size)
{
    header.data_start -= data_size;
    header.first_free_slot += 1;
    header.num_slots += 1;
    header.free_space -= data_size + sizeof(SlotDirectoryEntry);

    //TODO: add actual slot insert logic
}


void SlottedPage::erase(uint16_t slot_id)
{
    //TODO: add function to get slots and release slot
    // iff offset and length = 0 the slot is invalid
    SlotDirectoryEntry* slot = new SlotDirectoryEntry();
    slot->length = 0;
    slot->offset = 0;

    header.num_slots -= 1;
    if(slot_id == header.first_free_slot - 1)
    {
        header.first_free_slot -= 1;
    }else
    {
        header.first_free_slot = slot_id;
    }
}


void SlottedPage::resize(uint16_t slot_id, size_t data_size)
{
    //TODO: get slot check if it fits on current page and then resize it
}
