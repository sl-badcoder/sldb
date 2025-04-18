//
// Created by Senad Lemes Galera on 01.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef CRACKINGTHECODINGINTERVIEW_DATABASES_INDEXING_H_
#define CRACKINGTHECODINGINTERVIEW_DATABASES_INDEXING_H_
// -------------------------------------------------------------------------------------
#include "vector"
// -------------------------------------------------------------------------------------
typedef uint32_t page_id_t;
constexpr page_id_t INVALID_PAGE_ID = 0;
// -------------------------------------------------------------------------------------
struct PagePointer
{
    page_id_t page_id;
    uint32_t slot_num;
};
// -------------------------------------------------------------------------------------
struct IndexKey
{
    int key;

    bool operator<(const IndexKey& other) const
    {
        return key < other.key;
    }

    bool operator==(const IndexKey& other) const
    {
        return key == other.key;
    }

    bool operator>(const IndexKey& other) const
    {
        return key > other.key;
    }
};
// -------------------------------------------------------------------------------------
class BTreeNode
{
public:
    enum class NodeType { INTERNAL, LEAF };

    BTreeNode(NodeType type, int degree);
    // -------------------------------------------------------------------------------------
    [[nodiscard]]bool isLeaf() const { return type == NodeType::LEAF; }
    [[nodiscard]]bool isFull() const { return keys.size() == static_cast<size_t>(2 * degree - 1); }
    [[nodiscard]]page_id_t getParentPageId() const { return parent_page_id; };
    void setParentPageId(page_id_t page_id) { parent_page_id = page_id; }
    bool search(const IndexKey& key, int* index);
    bool insertInLeaf(const IndexKey& key, const PagePointer* value);
    bool insertNonFull(const IndexKey& key, const PagePointer& value, BTreeNode* child = nullptr);
    void splitChild(int index, BTreeNode* child);
    // -------------------------------------------------------------------------------------
    [[nodiscard]]int findIndex(const IndexKey& key) const;
    [[nodiscard]]BTreeNode* getChild(int index) const;
    void setChild(int index, BTreeNode* child);
    [[nodiscard]]const PagePointer* getValue(const IndexKey& key) const;
    void removeAt(int index);
    void borrowFromPrev(int index, BTreeNode* sibling);
    void borrowFromNext(int index, BTreeNode* sibling);
    void merge(int index, BTreeNode* sibling);
    bool remove(const IndexKey& key);
    // -------------------------------------------------------------------------------------
private:
    NodeType type;
    int degree;
    page_id_t page_id{};
    page_id_t parent_page_id;
    page_id_t child_page_id;

    std::vector<IndexKey> keys;
    std::vector<PagePointer> values;
    std::vector<page_id_t> children;

    friend class BTree;
};
// -------------------------------------------------------------------------------------
class BTree
{
public:
    explicit BTree(int degree);
    ~BTree();

    bool insert(const IndexKey& key, const PagePointer& value);
    bool remove(const IndexKey& key);
    [[nodiscard]] const PagePointer* search(const IndexKey& key)const;

private:
    int degree;
    BTreeNode* root;
    page_id_t next_page_id;

    page_id_t allocatePageId();
    BTreeNode* createNode(BTreeNode::NodeType type);
    [[nodiscard]] BTreeNode* findNode(const IndexKey& key) const;
};
// -------------------------------------------------------------------------------------
#endif //CRACKINGTHECODINGINTERVIEW_DATABASES_INDEXING_H_
// -------------------------------------------------------------------------------------