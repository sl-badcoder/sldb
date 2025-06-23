//
// Created by Senad Lemes Galera on 05.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef SLDB_DATABASE_BUFFER_MANAGER_H
#define SLDB_DATABASE_BUFFER_MANAGER_H
// -------------------------------------------------------------------------------------
#include "database/datastructures/indexing.h"
#include "buffer_replacement.h"
#include "basic_frame.h"
#include "storage/slotted_page.h"
#include "file_api.h"
// -------------------------------------------------------------------------------------
#include <unordered_map>
#include <vector>
#include <list>
#include <string>
#include <mutex>
#include <memory>
// -------------------------------------------------------------------------------------
// Define constants for default parameters
// -------------------------------------------------------------------------------------
static constexpr int INVALID_FILE_FD = -1;
constexpr size_t DEFAULT_POOL_SIZE = 1024;
//char* DEFAULT_DB_FILENAME = "sldb.db";
// -------------------------------------------------------------------------------------
inline std::unique_ptr<ReplacementStrategy> createReplacementStrategy(BUFFER_REPLACEMENT strategy)
{
    switch (strategy)
    {
    case BUFFER_REPLACEMENT::FIFO:
        return std::make_unique<FIFOReplacementStrategy>();
    case BUFFER_REPLACEMENT::LRU:
        return std::make_unique<LRUReplacementStrategy>();
    case BUFFER_REPLACEMENT::LFU:
        return std::make_unique<LFUReplacementStrategy>();
    case BUFFER_REPLACEMENT::CLOCK:
        return std::make_unique<ClockReplacementStrategy>();
    case BUFFER_REPLACEMENT::TQ:
        return std::make_unique<TwoQueueReplacementStrategy>();
    default:
        return std::make_unique<LRUReplacementStrategy>();
    }
}
// -------------------------------------------------------------------------------------
class BufferPoolManager
{
private:
    //! Constructor
    BufferPoolManager(size_t pool_size, char* db_file_name, uint64_t page_size,
                      BUFFER_REPLACEMENT strategy = BUFFER_REPLACEMENT::LRU): file(db_file_name, true)
    {
        pool_size_ = pool_size;
        db_file_name_ = db_file_name;
        replacer_ = createReplacementStrategy(strategy);
        next_page_id_ = 0;
        page_size_ = page_size;

        for (size_t i{}; i < pool_size; i++)
        {
            free_list_.push_back(new Frame);
        }
    }
    ;
    // Static instance variable
    static BufferPoolManager* instance_;
    static std::mutex singleton_mutex_;
public:
    //! Destructor
    ~BufferPoolManager() = default;
    // -------------------------------------------------------------------------------------
    // Deleted constructors
    //! Copy constructor
    BufferPoolManager(const BufferPoolManager& other) = delete;
    //! Move constructor
    BufferPoolManager(BufferPoolManager&& other) noexcept = delete;
    //! Copy assignment constructor
    BufferPoolManager& operator=(const BufferPoolManager& other) = delete;
    //! Move assignment constructo
    BufferPoolManager& operator=(BufferPoolManager&& other) noexcept = delete;
    // -------------------------------------------------------------------------------------
    // Change replacement strategy at runtime
    void setReplacementStrategy(BUFFER_REPLACEMENT strategy)
    {
        replacer_ = createReplacementStrategy(strategy);
    };
    // -------------------------------------------------------------------------------------
    static BufferPoolManager* getInstance() {
        // Double-checked locking pattern for thread safety
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(singleton_mutex_);
            if (instance_ == nullptr) {
                // Default parameters - you can customize these or make them configurable
                instance_ = new BufferPoolManager(DEFAULT_POOL_SIZE, "sldb.db", 4096);
            }
        }
        return instance_;
    }

    // Initialize singleton with specific parameters
    static void initInstance(size_t pool_size, char* db_file_name, uint64_t page_size,
                           BUFFER_REPLACEMENT strategy = BUFFER_REPLACEMENT::LRU) {
        std::lock_guard<std::mutex> lock(singleton_mutex_);
        if (instance_ != nullptr) {
            delete instance_;
        }
        instance_ = new BufferPoolManager(pool_size, db_file_name, page_size, strategy);
    }

    // Cleanup method
    static void destroyInstance() {
        std::lock_guard<std::mutex> lock(singleton_mutex_);
        if (instance_ != nullptr) {
            delete instance_;
            instance_ = nullptr;
        }
    }
    // -------------------------------------------------------------------------------------

    // Core API
    SlottedPage* pinPage(page_id_t page_id);
    bool unpinPage(page_id_t page_id, bool is_dirty);
    bool flushPage(page_id_t page_id);
    SlottedPage* newPage(page_id_t* page_id);
    bool deletePage(page_id_t page_id);
    bool flushAllPages();

    std::unordered_map<page_id_t, Frame*> getPageTable() {
        return page_table_;
    }
    std::list<Frame*> getFreeList()
    {
        return free_list_;
    }
    std::unique_ptr<ReplacementStrategy> getReplacer()
    {
        return std::move(replacer_);
    }
private:
    // Helper methods
    uint64_t findVictimFrame() { return replacer_->findVictim(); };
    bool evictPage(page_id_t page_id);
    void updatePageMetadata(Frame* frame, SlottedPage* page);
    static int64_t getCurrentTimestamp();

    // Storage for pages
    size_t pool_size_;
    std::unordered_map<page_id_t, Frame*> page_table_;

    // Replacement strategy
    std::unique_ptr<ReplacementStrategy> replacer_;

    // Free frames list
    std::list<Frame*> free_list_;

    // For accessing the actual disk file
    std::string db_file_name_;
    File file;

    // For managing page IDs
    page_id_t next_page_id_;

    // Thread safety
    std::mutex latch_;

    uint64_t page_size_;
};
// -------------------------------------------------------------------------------------
#endif //SLDB_DATABASE_BUFFER_MANAGER_H
// -------------------------------------------------------------------------------------