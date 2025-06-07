//
// Created by Senad Lemes Galera on 05.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef SLDB_DATABASE_BUFFER_MANAGER_H
#define SLDB_DATABASE_BUFFER_MANAGER_H
// -------------------------------------------------------------------------------------
#include "src/database/datastructures/indexing.h"
#include "src/database/buffermanager/buffer_replacement.h"
#include "src/storage/basic_frame.h"
#include "src/storage/slotted_page.h"
#include "File.h"
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
    BufferPoolManager(size_t pool_size, char* db_file_name,
                      BUFFER_REPLACEMENT strategy = BUFFER_REPLACEMENT::LRU): file(db_file_name, true)
    {
        pool_size_ = pool_size;
        frames_.reserve(pool_size);
        db_file_name_ = db_file_name;
        replacer_ = createReplacementStrategy(strategy);
        next_page_id_ = 0;

        for (size_t i{}; i < pool_size; i++)
        {
            frames_.push_back(std::make_unique<Frame>());
            free_list_.push_back(frames_[i].get());
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
                instance_ = new BufferPoolManager(DEFAULT_POOL_SIZE, "sldb.db");
            }
        }
        return instance_;
    }

    // Initialize singleton with specific parameters
    static void initInstance(size_t pool_size, char* db_file_name,
                           BUFFER_REPLACEMENT strategy = BUFFER_REPLACEMENT::LRU) {
        std::lock_guard<std::mutex> lock(singleton_mutex_);
        if (instance_ != nullptr) {
            delete instance_;
        }
        instance_ = new BufferPoolManager(pool_size, db_file_name, strategy);
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
    SlottedPage* fetchPage(page_id_t page_id);
    bool unpinPage(page_id_t page_id, bool is_dirty);
    bool flushPage(page_id_t page_id);
    SlottedPage* newPage(page_id_t* page_id);
    bool deletePage(page_id_t page_id);
    bool flushAllPages();

private:
    // Helper methods
    Frame* findVictimFrame() { return replacer_->findVictim(); };
    bool evictPage(Frame* frame);
    void updatePageMetadata(Frame* frame, SlottedPage* page);
    static int64_t getCurrentTimestamp();

    // Storage for pages
    size_t pool_size_;
    std::vector<std::unique_ptr<Frame>> frames_;
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


};
// -------------------------------------------------------------------------------------
// B-tree node disk serialization helpers with slotted page awareness
// -------------------------------------------------------------------------------------
class BTreeNodeSerializer
{
public:
    static bool serializeNode(const BTreeNode* node, SlottedPage* page);
    static BTreeNode* deserializeNode(const SlottedPage* page, int degree){return nullptr;};

private:
    // Helper methods for encoding/decoding node structures within slots
    static void serializeNodeHeader(const BTreeNode* node, char* buffer, size_t* size);
    static void serializeKeys(const BTreeNode* node, char* buffer, size_t* size);
    static void serializeValues(const BTreeNode* node, char* buffer, size_t* size);
    static void serializeChildren(const BTreeNode* node, char* buffer, size_t* size);
};

// -------------------------------------------------------------------------------------
// Modified B-tree implementation that uses the buffer manager with slotted pages
// -------------------------------------------------------------------------------------
class DiskBTree
{
public:
    DiskBTree(int degree, BufferPoolManager* buffer_pool_manager);
    ~DiskBTree();

    bool insert(const IndexKey& key, const PagePointer& value);
    bool remove(const IndexKey& key);
    const PagePointer* search(const IndexKey& key) const;

private:
    int degree_;
    page_id_t root_page_id_;
    BufferPoolManager* buffer_pool_manager_;

    // Helper methods to work with nodes via the buffer pool
    BTreeNode* fetchNode(page_id_t page_id) const;
    void unpinNode(BTreeNode* node, bool is_dirty) const;
    BTreeNode* createNewNode(BTreeNode::NodeType type);
    bool deleteNode(page_id_t page_id);

    // Modified to map node pages to slot IDs for node components
    std::unordered_map<page_id_t, std::vector<SlotId>> node_component_map_;
};
// -------------------------------------------------------------------------------------
#endif //SLDB_DATABASE_BUFFER_MANAGER_H
// -------------------------------------------------------------------------------------