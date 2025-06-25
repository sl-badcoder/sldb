//
// Created by Senad Lemes Galera on 24.06.25.
//
#include "sp_segment.h"

void SP_SEGMENT::erase(TID tid)
{
    auto& frame = buffer_pool_manager.getPageTable()[tid.page_id];
    frame->getPage()->erase(tid.slot_id);
}

void SP_SEGMENT::allocate(uint16_t data_size)
{
    // find free page
    page_id_t page_id = 0;
    auto& frame = buffer_pool_manager.getPageTable()[page_id];
    frame->getPage()->allocate(data_size);
}
