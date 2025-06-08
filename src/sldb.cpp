// -------------------------------------------------------------------------------------
#include "database/database.h"
#include "database/datastructures/indexing.h"
#include "database/buffermanager/buffer_manager.h"
// -------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
// -------------------------------------------------------------------------------------
// Initialize static members
// -------------------------------------------------------------------------------------
BufferPoolManager* BufferPoolManager::instance_ = nullptr;
std::mutex BufferPoolManager::singleton_mutex_;
//--------------------------------------------------------------------------------------
int main()
{
    BufferPoolManager::initInstance(2048, "my_database.db", BUFFER_REPLACEMENT::LRU);
    BufferPoolManager::destroyInstance();
    return 0;
}
//--------------------------------------------------------------------------------------