//
// Created by Senad Lemes Galera on 24.06.25.
//

#ifndef SP_SEGMENT_H
#define SP_SEGMENT_H

#include <cstdint>
#include "slotted_page.h"

struct SP_SEGMENT{
    void allocate(uint16_t data_size);
    void erase(TID tid);
    uint64_t segment_id;
};

#endif //SP_SEGMENT_H
