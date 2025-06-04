#ifndef SKIP_LIST_NODE_H
#define SKIP_LIST_NODE_H

#include <vector>
#include <cstddef> 

// Tag for sentinel node constructor to resolve ambiguity
struct SentinelNodeTag {};

/**
 * @brief Node structure for the SkipList.
 * @tparam T The type of data stored in the node.
 */
template <typename T>
struct SkipListNode {
    T value; ///< Data stored in the node.
    std::vector<SkipListNode<T>*> forward_pointers; ///< Vector of forward pointers for each level.
    SkipListNode<T>* prev_pointer; ///< Pointer to the previous node at level 0 for bidirectional iteration.

    /**
     * @brief Construct a new Skip List Node object for data.
     * @param val The value to be stored in the node.
     * @param level The number of levels this node will participate in (0-indexed).
     * The forward_pointers vector will have size 'level + 1'.
     */
    explicit SkipListNode(const T& val, int level) 
        : value(val), 
          forward_pointers(static_cast<size_t>(level) + 1, nullptr), 
          prev_pointer(nullptr) {}

    /**
     * @brief Construct a new Skip List Node object for sentinel nodes (head/tail).
     * Uses a tag to disambiguate from the data node constructor.
     * @param level The number of levels this node will participate in (0-indexed).
     * @param tag SentinelNodeTag to identify this constructor.
     * @param default_val A default value for the node (typically T{}).
     */
    explicit SkipListNode(int level, SentinelNodeTag /*tag*/, const T& default_val = T{}) 
        : value(default_val), 
          forward_pointers(static_cast<size_t>(level) + 1, nullptr), 
          prev_pointer(nullptr) {}

    // Prevent copying and moving, as nodes are managed by the SkipList's allocator.
    SkipListNode(const SkipListNode&) = delete;
    SkipListNode& operator=(const SkipListNode&) = delete;
    SkipListNode(SkipListNode&&) = delete;
    SkipListNode& operator=(SkipListNode&&) = delete;
};

#endif // SKIP_LIST_NODE_H