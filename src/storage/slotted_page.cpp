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
    header.free_space -= data_size - sizeof(SlotDirectoryEntry);

    //TODO: add actual slot insert logic
}


void SlottedPage::erase(TID tid)
{
    //TODO: add function to get slots and release slot
}


void SlottedPage::resize(TID tid, size_t data_size)
{
    //TODO: get slot check if it fits on current page and then resize it
}
