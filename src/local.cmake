add_library(bufferlib
        src/database/buffermanager/buffer_manager.cpp
        src/database/buffermanager/buffer_replacement.cpp
        src/database/database.cpp
        src/database/datastructures/indexing.cpp
        src/storage/slotted_page.cpp
        src/sldb.cpp
)

target_include_directories(bufferlib PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)