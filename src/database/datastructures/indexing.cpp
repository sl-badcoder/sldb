//
// Created by Senad Lemes Galera on 01.04.25.
//
// -------------------------------------------------------------------------------------
#include "database/buffermanager/buffer_manager.h"
// -------------------------------------------------------------------------------------
#include "indexing.h"
// -------------------------------------------------------------------------------------
BTreeNode::BTreeNode(const NodeType type, const int degree): type(type), degree(degree),
                                                             parent_page_id(INVALID_PAGE_ID),
                                                             child_page_id(INVALID_PAGE_ID)
{
    keys.reserve(2 * degree - 1);
    values.reserve(2 * degree - 1);
    if (type == NodeType::INTERNAL)
        children.reserve(2 * degree - 1);
}

bool BTreeNode::search(const IndexKey& key, int* index)
{
    const auto it = std::lower_bound(keys.begin(), keys.end(), key);
    *index = it - keys.begin();

    return (it != keys.end() && *it == key);
}

bool BTreeNode::insertNonFull(const IndexKey& key, const PagePointer& value, BTreeNode* child)
{
    int i = keys.size() - 1;
    if (isLeaf())
    {
        while (i >= 0 && keys[i] > key)
        {
            i--;
        }
        if (i >= 0 && keys[i] == key)
        {
            values[i] = value;
            return true;
        }
        keys.insert(keys.begin() + i + 1, key);
        values.insert(values.begin() + i + 1, value);
    }
    else
    {
        while (i >= 0 && keys[i] > key)
        {
            i--;
        }

        if (i >= 0 && keys[i] == key)
        {
            values[i] = value;
            return true;
        }
        i++;

        BTreeNode* child = getChild(i);

        if (child && child->isFull())
        {
            splitChild(i, child);
            if (keys[i] < key)
            {
                i++;
                child = getChild(i);
            }
            if (child)return child->insertNonFull(key, value);
        }
    }
    return true;
}

bool BTreeNode::insertInLeaf(const IndexKey& key, const PagePointer* value)
{
    if (!isLeaf())return false;
    if (isFull())return false;

    int i = keys.size() - 1;
    while (i >= 0 && key < keys[i])
    {
        --i;
    }
    keys.insert(keys.begin() + i + 1, key);
    values.insert(values.begin() + i + 1, *value);
    return true;
}


void BTreeNode::splitChild(const int index, BTreeNode* child)
{
    auto newNode = std::make_unique<BTreeNode>(child->type, degree);
    newNode->parent_page_id = page_id;

    for (int j = 0; j < degree - 1; j++)
    {
        newNode->keys.push_back(child->keys[j + degree]);
        newNode->values.push_back(child->values[j + degree]);
    }
    if (!child->isLeaf())
    {
        for (int j = 0; j < degree; j++)
        {
            newNode->children.push_back(child->children[j + degree]);
        }
    }

    child->keys.resize(degree - 1);
    child->values.resize(degree - 1);

    if (!child->isLeaf())
    {
        child->children.resize(degree);
    }

    children.insert(children.begin() + index + 1, newNode->page_id);
    keys.insert(keys.begin() + index, child->keys[degree - 1]);
    values.insert(values.begin() + index, child->values[degree - 1]);
}

int BTreeNode::findIndex(const IndexKey& key) const
{
    int index = 0;
    while (index < keys.size() && keys[index] < key)
    {
        ++index;
    }
    return index;
}

BTreeNode* BTreeNode::getChild(int index) const
{
    if (index < 0 || index >= static_cast<int>(children.size()))return nullptr;

    page_id_t child_page_id = children[index];

    BufferPoolManager* buffer_pool_manager = BufferPoolManager::getInstance();

    SlottedPage* child_page = buffer_pool_manager->fetchPage(child_page_id);
    if (!child_page)return nullptr;

    BTreeNode* child_node = BTreeNodeSerializer::deserializeNode(child_page, degree);
    return child_node;
}

void BTreeNode::setChild(int index, BTreeNode* child)
{
    if (isLeaf() || index < 0 || index > children.size())return;
    if (index == children.size())children.push_back(child->page_id);
    else children[index] = child->page_id;
    child->parent_page_id = page_id;
}

const PagePointer* BTreeNode::getValue(const IndexKey& key) const
{
    for (size_t i = 0; i < keys.size(); i++)
    {
        if (keys[i] == key)return &values[i];
    }
    return nullptr;
}

void BTreeNode::removeAt(int index)
{
    if (index < 0 || index >= keys.size())return;

    keys.erase(keys.begin() + index);
    values.erase(values.begin() + index);

    if (!isLeaf() && index < children.size() - 1)
    {
        children.erase(children.begin() + index + 1);
    }
}

void BTreeNode::borrowFromPrev(int index, BTreeNode* sibling)
{
    BTreeNode* child = getChild(index);

    child->keys.insert(child->keys.begin(), keys[index - 1]);
    child->values.insert(child->values.begin(), values[index - 1]);

    keys[index - 1] = sibling->keys.back();
    values[index - 1] = sibling->values.back();

    if (!sibling->isLeaf())
    {
        child->children.insert(child->children.begin(), sibling->children.back());
        sibling->children.pop_back();
    }
    sibling->keys.pop_back();
    sibling->values.pop_back();
}

void BTreeNode::borrowFromNext(int index, BTreeNode* sibling)
{
    BTreeNode* child = getChild(index);

    child->keys.push_back(keys[index]);
    child->values.push_back(values[index]);

    keys[index] = sibling->keys.front();
    values[index] = sibling->values.front();

    if (!sibling->isLeaf())
    {
        child->children.push_back(sibling->children.front());
        sibling->children.erase(sibling->children.begin());
    }

    sibling->keys.erase(sibling->keys.begin());
    sibling->values.erase(sibling->values.begin());
}

void BTreeNode::merge(int index, BTreeNode* sibling)
{
    BTreeNode* child = getChild(index);

    child->keys.push_back(keys[index]);
    child->values.push_back(values[index]);

    child->keys.insert(child->keys.end(), sibling->keys.begin(), sibling->keys.end());
    child->values.insert(child->values.end(), sibling->values.begin(), sibling->values.end());

    if (!child->isLeaf())
    {
        child->children.insert(child->children.end(), sibling->children.begin(), sibling->children.end());
    }

    keys.erase(keys.begin() + index);
    values.erase(values.begin() + index);
    children.erase(children.begin() + index + 1);
}

bool BTreeNode::remove(const IndexKey& key)
{
    int index = findIndex(key);

    if (index < keys.size() && keys[index] == key)
    {
        if (isLeaf())
        {
            removeAt(index);
            return true;
        }
        else
        {
            BTreeNode* leftChild = getChild(index);
            BTreeNode* rightChild = getChild(index + 1);

            if (leftChild && leftChild->keys.size() >= degree)
            {
                BTreeNode* pred = leftChild;
                while (!pred->isLeaf())
                {
                    pred = pred->getChild(pred->children.size() - 1);
                }
                keys[index] = pred->keys.back();
                values[index] = pred->values.back();
                return leftChild->remove(pred->keys.back());
            }
            else if (rightChild && rightChild->keys.size() >= degree)
            {
                BTreeNode* succ = rightChild;
                while (!succ->isLeaf())
                {
                    succ = succ->getChild(0);
                }
                keys[index] = succ->keys.front();
                values[index] = succ->values.front();

                return rightChild->remove(succ->keys.front());
            }
            else if (leftChild && rightChild)
            {
                merge(index, rightChild);
                return leftChild->remove(key);
            }
        }
    }
    else if (!isLeaf())
    {
        BTreeNode* child = getChild(index);
        if (child && child->keys.size() < degree)
        {
            BTreeNode* leftSibling = index > 0 ? getChild(index - 1) : nullptr;
            BTreeNode* rightSibling = index < children.size() ? getChild(index + 1) : nullptr;

            if (leftSibling && leftSibling->keys.size() >= degree)
            {
                borrowFromPrev(index, leftSibling);
            }
            else if (rightSibling && rightSibling->keys.size() >= degree)
            {
                borrowFromNext(index, rightSibling);
            }
            else if (leftSibling)
            {
                merge(index - 1, leftSibling);
                child = leftSibling;
            }
            else if (rightSibling)
            {
                merge(index, rightSibling);
            }
        }
        return child->remove(key);
    }
    return false;
}

page_id_t BTree::allocatePageId()
{
    return next_page_id++;
}

BTreeNode* BTree::createNode(BTreeNode::NodeType type)
{
    BTreeNode* node = new BTreeNode(type, degree);
    node->page_id = allocatePageId();
    return node;
}

bool BTree::insert(const IndexKey& key, const PagePointer& value)
{
    if (root->isFull())
    {
        BTreeNode* newRoot = createNode(BTreeNode::NodeType::INTERNAL);
        newRoot->children.push_back(root->page_id);
        root->setParentPageId(newRoot->page_id);


        newRoot->splitChild(0, root);

        root = newRoot;

        return root->insertNonFull(key, value);
    }
    else
    {
        return root->insertNonFull(key, value);
    }
}

bool BTree::remove(const IndexKey& key)
{
    if (!root)
    {
        return false;
    }

    bool result = root->remove(key);

    if (root->keys.empty() && !root->isLeaf())
    {
        BTreeNode* oldRoot = root;
        root = root->getChild(0);
        if (root)
        {
            root->setParentPageId(INVALID_PAGE_ID);
        }
        delete oldRoot;
    }

    return result;
}

const PagePointer* BTree::search(const IndexKey& key) const
{
    BTreeNode* node = findNode(key);
    if (!node)
    {
        return nullptr;
    }

    return node->getValue(key);
}

BTreeNode* BTree::findNode(const IndexKey& key) const
{
    if (!root)
    {
        return nullptr;
    }

    BTreeNode* current = root;
    while (current)
    {
        int index;
        bool found = current->search(key, &index);

        if (found)
        {
            return current;
        }

        if (current->isLeaf())
        {
            break;
        }

        current = current->getChild(index);
    }

    return nullptr;
}
// -------------------------------------------------------------------------------------