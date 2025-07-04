//
// Created by Senad Lemes Galera on 24.06.25.
//

#ifndef SP_SEGMENT_H
#define SP_SEGMENT_H

#include <cstdint>
#include <database/buffermanager/buffer_manager.h>

#include "slotted_page.h"

struct SP_SEGMENT{
    SP_SEGMENT(BufferPoolManager& buffer_pool_manager):buffer_pool_manager(buffer_pool_manager){};
    void allocate(uint16_t data_size);
    void erase(TID tid);
    BufferPoolManager& buffer_pool_manager;
    uint64_t segment_id;
};


struct FSI_SEGMENT
{
    page_id_t find(uint64_t data_size);
    uint8_t encode(uint32_t free_space);
    uint32_t decode(uint8_t free_space);
    uint64_t segment_id;
};
#endif //SP_SEGMENT_H
