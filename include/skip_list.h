#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include "skip_list_node.h" // Ensure this is the updated version with prev_pointer and SentinelNodeTag
#include <vector>
#include <memory>
#include <random>
#include <limits>
#include <algorithm> // For std::equal, std::lexicographical_compare
#include <stdexcept> // For std::out_of_range
#include <iterator>  // For std::reverse_iterator, std::bidirectional_iterator_tag
#include <cassert>   // For assert
#include <type_traits> // For std::enable_if_t, std::is_convertible_v, std::is_const_v, std::remove_pointer_t

// Forward declaration of the SkipListNode template
template <typename T_node> struct SkipListNode; 

/**
 * @brief Implementation of the SkipList iterator.
 * Supports bidirectional iteration.
 *
 * @tparam NodePtr Type of the pointer to the node (e.g., SkipListNode<T>* or const SkipListNode<T>*).
 * @tparam ValueType The type of the value stored in the list.
 * @tparam DifferenceType Type to represent the difference between two iterators.
 * @tparam Pointer Pointer to ValueType.
 * @tparam Reference Reference to ValueType.
 */
template <typename NodePtr, typename ValueType, typename DifferenceType, typename Pointer, typename Reference>
class SkipListIteratorImpl {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = ValueType;
    using difference_type   = DifferenceType;
    using pointer           = Pointer;
    using reference         = Reference;

private:
    NodePtr current_node;

public:
    SkipListIteratorImpl() : current_node(nullptr) {}
    explicit SkipListIteratorImpl(NodePtr node) : current_node(node) {}
    SkipListIteratorImpl(const SkipListIteratorImpl& other) = default;
    SkipListIteratorImpl& operator=(const SkipListIteratorImpl& other) = default;

    template <typename OtherNodePtr, typename OtherPointer, typename OtherReference,
              typename = std::enable_if_t<
                  std::is_convertible_v<OtherNodePtr, NodePtr> &&
                  std::is_const_v<std::remove_pointer_t<NodePtr>> &&
                  !std::is_const_v<std::remove_pointer_t<OtherNodePtr>>
              >>
    SkipListIteratorImpl(const SkipListIteratorImpl<OtherNodePtr, ValueType, DifferenceType, OtherPointer, OtherReference>& other)
        : current_node(other.get_node()) {}

    reference operator*() const {
        assert(current_node != nullptr && "Cannot dereference a null iterator.");
        return current_node->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    SkipListIteratorImpl& operator++() {
        assert(current_node != nullptr && "Cannot increment a null iterator.");
        current_node = current_node->forward_pointers[0];
        return *this;
    }

    SkipListIteratorImpl operator++(int) {
        SkipListIteratorImpl temp = *this;
        ++(*this);
        return temp;
    }

    SkipListIteratorImpl& operator--() {
        assert(current_node != nullptr && "Cannot decrement a null iterator.");
        assert(current_node->prev_pointer != nullptr && "Cannot decrement: predecessor is null (iterator might be at begin() or an invalid state).");
        current_node = current_node->prev_pointer;
        return *this;
    }

    SkipListIteratorImpl operator--(int) {
        SkipListIteratorImpl temp = *this;
        --(*this);
        return temp;
    }

    bool operator==(const SkipListIteratorImpl& other) const {
        return current_node == other.current_node;
    }

    bool operator!=(const SkipListIteratorImpl& other) const {
        return !(*this == other);
    }

    NodePtr get_node() const {
        return current_node;
    }
    
    template <typename NP = NodePtr, typename = std::enable_if_t<!std::is_const_v<std::remove_pointer_t<NP>>>>
    SkipListNode<ValueType>* get_node_mut() const {
        return const_cast<SkipListNode<ValueType>*>(current_node); 
    }
    
    template <typename U, typename Alloc> friend class SkipList;
};

/**
 * @brief A Skip List data structure with an interface similar to std::list.
 *
 * @tparam T The type of elements.
 * @tparam Allocator The allocator type to use for all memory allocations.
 */
template <typename T, typename Allocator = std::allocator<T>>
class SkipList {
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

private: 
    using Node = SkipListNode<T>;
    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

public:
    using iterator = SkipListIteratorImpl<Node*, value_type, difference_type, pointer, reference>;
    using const_iterator = SkipListIteratorImpl<const Node*, value_type, difference_type, const_pointer, const_reference>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    friend class SkipListIteratorImpl<Node*, value_type, difference_type, pointer, reference>;
    friend class SkipListIteratorImpl<const Node*, value_type, difference_type, const_pointer, const_reference>;

private:
    Node* head;
    Node* tail;
    size_type list_size;
    int current_max_level;
    float probability;
    
    std::mt19937 random_engine;
    NodeAllocator node_alloc;

    static constexpr int MAX_LEVEL = 16; 

    int internal_random_level_gen() {
        int lvl = 0;
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        while (dist(random_engine) < probability && lvl < MAX_LEVEL - 1) {
            lvl++;
        }
        return lvl;
    }

    Node* internal_create_data_node(const_reference value, int level) {
        Node* new_node = std::allocator_traits<NodeAllocator>::allocate(node_alloc, 1);
        try {
            std::allocator_traits<NodeAllocator>::construct(node_alloc, new_node, value, level);
        } catch (...) {
            std::allocator_traits<NodeAllocator>::deallocate(node_alloc, new_node, 1);
            throw;
        }
        return new_node;
    }

    void internal_delete_node(Node* node) {
        if (node) {
            std::allocator_traits<NodeAllocator>::destroy(node_alloc, node);
            std::allocator_traits<NodeAllocator>::deallocate(node_alloc, node, 1);
        }
    }
    
    void internal_find_predecessors_for_insert(const_reference value, std::vector<Node*>& update_vec) const {
        Node* current = head;
        for (int i = current_max_level; i >= 0; --i) {
            while (static_cast<size_t>(i) < current->forward_pointers.size() && 
                   current->forward_pointers[i] != tail && 
                   current->forward_pointers[i]->value <= value) { // Use <= for stable insert of duplicates
                current = current->forward_pointers[i];
            }
            if (static_cast<size_t>(i) < update_vec.size()) { 
                update_vec[i] = current;
            }
        }
    }

    void internal_find_predecessors(const_reference value, std::vector<Node*>& update_vec) const {
        Node* current = head;
        for (int i = current_max_level; i >= 0; --i) {
            while (static_cast<size_t>(i) < current->forward_pointers.size() && 
                   current->forward_pointers[i] != tail && 
                   current->forward_pointers[i]->value < value) {
                current = current->forward_pointers[i];
            }
            if (static_cast<size_t>(i) < update_vec.size()) { 
                update_vec[i] = current;
            }
        }
    }

public:
    explicit SkipList(float prob = 0.5f, const Allocator& alloc = Allocator())
        : list_size(0),
          current_max_level(0),
          probability(prob),
          random_engine(std::random_device{}()),
          node_alloc(alloc) {
        head = std::allocator_traits<NodeAllocator>::allocate(node_alloc, 1);
        std::allocator_traits<NodeAllocator>::construct(node_alloc, head, MAX_LEVEL - 1, SentinelNodeTag{}, T{}); 
        
        tail = std::allocator_traits<NodeAllocator>::allocate(node_alloc, 1);
        std::allocator_traits<NodeAllocator>::construct(node_alloc, tail, 0, SentinelNodeTag{}, T{});

        for (int i = 0; i < MAX_LEVEL; ++i) {
            if (static_cast<size_t>(i) < head->forward_pointers.size()) {
                 head->forward_pointers[i] = tail;
            }
        }
        head->prev_pointer = nullptr;
        tail->prev_pointer = head; 
    }

    ~SkipList() {
        clear(); 
        internal_delete_node(tail);
        internal_delete_node(head);
    }
    
    /**
     * @brief Copy constructor. Creates a deep copy of the other list.
     * @param other The SkipList to copy from.
     */
    SkipList(const SkipList& other) // Removed explicit
        : list_size(0), 
          current_max_level(0), 
          probability(other.probability),
          random_engine(std::random_device{}()), // Re-initialize, or copy other.random_engine for deterministic copies
          node_alloc(std::allocator_traits<NodeAllocator>::select_on_container_copy_construction(other.node_alloc)) {
        
        head = std::allocator_traits<NodeAllocator>::allocate(node_alloc, 1);
        std::allocator_traits<NodeAllocator>::construct(node_alloc, head, MAX_LEVEL - 1, SentinelNodeTag{}, T{});
        
        tail = std::allocator_traits<NodeAllocator>::allocate(node_alloc, 1);
        std::allocator_traits<NodeAllocator>::construct(node_alloc, tail, 0, SentinelNodeTag{}, T{});

        for (int i = 0; i < MAX_LEVEL; ++i) {
            if (static_cast<size_t>(i) < head->forward_pointers.size()) {
                head->forward_pointers[i] = tail;
            }
        }
        head->prev_pointer = nullptr;
        tail->prev_pointer = head;

        if (other.head && other.tail) { 
            for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
                // Use this->insert to ensure it uses the current object's context
                // Need to get the value from the iterator.
                this->insert(it.get_node()->value); 
            }
        }
    }

    /**
     * @brief Copy assignment operator.
     * @param other The SkipList to assign from.
     * @return SkipList& Reference to this SkipList.
     */
    SkipList& operator=(const SkipList& other) {
        if (this == &other) {
            return *this; 
        }
        SkipList temp(other); 
        swap(temp);          
        return *this;
    }

    // Consider adding move constructor and move assignment operator in the future
    // SkipList(SkipList&& other) noexcept;
    // SkipList& operator=(SkipList&& other) noexcept;

    iterator begin() noexcept { return iterator(head->forward_pointers[0]); }
    const_iterator begin() const noexcept { return const_iterator(head->forward_pointers[0]); }
    const_iterator cbegin() const noexcept { return const_iterator(head->forward_pointers[0]); }
    iterator end() noexcept { return iterator(tail); }
    const_iterator end() const noexcept { return const_iterator(tail); }
    const_iterator cend() const noexcept { return const_iterator(tail); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    [[nodiscard]] bool empty() const noexcept { return list_size == 0; }
    size_type size() const noexcept { return list_size; }
    size_type max_size() const noexcept { return std::allocator_traits<NodeAllocator>::max_size(node_alloc); }
    allocator_type get_allocator() const noexcept { return node_alloc; }

    reference front() {
        if (empty()) throw std::out_of_range("front() on empty SkipList");
        return head->forward_pointers[0]->value;
    }
    const_reference front() const {
        if (empty()) throw std::out_of_range("front() on empty SkipList");
        return head->forward_pointers[0]->value;
    }
    reference back() {
        if (empty()) throw std::out_of_range("back() on empty SkipList");
        assert(tail->prev_pointer != nullptr && tail->prev_pointer != head && "back() called on empty or inconsistent list (prev_pointer invalid)");
        return tail->prev_pointer->value;
    }
    const_reference back() const {
        if (empty()) throw std::out_of_range("back() on empty SkipList");
        assert(tail->prev_pointer != nullptr && tail->prev_pointer != head && "back() called on empty or inconsistent list (prev_pointer invalid)");
        return tail->prev_pointer->value;
    }

    void push_front(const value_type& value) { insert(value); }
    void pop_front() { if (!empty()) erase(cbegin()); }
    void push_back(const value_type& value) { insert(value); }
    void pop_back() {
        if (!empty()) {
             assert(tail->prev_pointer != nullptr && tail->prev_pointer != head && "pop_back() on a list where tail->prev_pointer is invalid, but not empty");
            erase(const_iterator(tail->prev_pointer));
        }
    }

    void clear() noexcept {
        Node* current = head->forward_pointers[0];
        while (current != tail) {
            Node* next = current->forward_pointers[0];
            internal_delete_node(current);
            current = next;
        }
        for (int i = 0; i < MAX_LEVEL; ++i) {
             if (static_cast<size_t>(i) < head->forward_pointers.size()) { 
                head->forward_pointers[i] = tail;
            }
        }
        tail->prev_pointer = head;
        list_size = 0;
        current_max_level = 0;
    }

    iterator insert(const value_type& value) {
        std::vector<Node*> update_vec(MAX_LEVEL, nullptr);
        internal_find_predecessors_for_insert(value, update_vec);

        int new_node_lvl = internal_random_level_gen();

        if (new_node_lvl > current_max_level) {
            for (int i = current_max_level + 1; i <= new_node_lvl; ++i) {
                if (static_cast<size_t>(i) < update_vec.size()) {
                    update_vec[i] = head;
                }
            }
            current_max_level = new_node_lvl;
        }

        Node* new_node = internal_create_data_node(value, new_node_lvl); 
        Node* predecessor_at_level0 = update_vec[0];
        Node* successor_at_level0 = predecessor_at_level0->forward_pointers[0];

        new_node->forward_pointers[0] = successor_at_level0;
        new_node->prev_pointer = predecessor_at_level0;
        predecessor_at_level0->forward_pointers[0] = new_node;
        if (successor_at_level0 != tail) {
            successor_at_level0->prev_pointer = new_node;
        } else { 
            tail->prev_pointer = new_node;
        }
        
        for (int i = 1; i <= new_node_lvl; ++i) {
            if (static_cast<size_t>(i) < new_node->forward_pointers.size() && 
                static_cast<size_t>(i) < update_vec.size() && update_vec[i] &&
                static_cast<size_t>(i) < update_vec[i]->forward_pointers.size()) {
                new_node->forward_pointers[i] = update_vec[i]->forward_pointers[i];
                update_vec[i]->forward_pointers[i] = new_node;
            } else {
                assert(false && "Error in insert: update_vec issue or node level issue for higher levels.");
                if (static_cast<size_t>(i) < new_node->forward_pointers.size()) new_node->forward_pointers[i] = tail;
            }
        }
        list_size++;
        return iterator(new_node);
    }

    iterator insert(const_iterator hint, const value_type& value) {
        (void)hint; 
        return insert(value);
    }
    iterator erase(const_iterator pos) {
        if (pos == cend() || pos.get_node() == nullptr || pos.get_node() == head || pos.get_node() == tail) {
            return end(); 
        }

        Node* node_to_delete = const_cast<Node*>(pos.get_node());
        
        std::vector<Node*> update_vec(MAX_LEVEL, nullptr);
        Node* current_search = head;
        
        // Find predecessors for the specific node to delete
        for (int i = current_max_level; i >= 0; --i) {
            while (current_search && static_cast<size_t>(i) < current_search->forward_pointers.size() &&
                   current_search->forward_pointers[i] != tail &&
                   current_search->forward_pointers[i] != node_to_delete && 
                   (current_search->forward_pointers[i]->value < node_to_delete->value || 
                    (current_search->forward_pointers[i]->value == node_to_delete->value && 
                     current_search->forward_pointers[i] != node_to_delete) 
                   )) {
                current_search = current_search->forward_pointers[i];
            }
            if (static_cast<size_t>(i) < update_vec.size()) {
                update_vec[i] = current_search;
            } else {
                 assert(false && "update_vec out of bounds in erase predecessor search");
            }
        }
        
        assert(node_to_delete != nullptr && "erase(pos): node_to_delete is null!");
        assert(update_vec[0] != nullptr && "erase(pos): update_vec[0] (predecessor at L0) is null!");
        if (update_vec[0]) { 
            assert(static_cast<size_t>(0) < update_vec[0]->forward_pointers.size() && "erase(pos): update_vec[0] has no forward_pointers at L0!");
        }

        // Critical check: if this fails, node was not properly found or list is inconsistent
        if (!update_vec[0] || 
            static_cast<size_t>(0) >= update_vec[0]->forward_pointers.size() || 
            update_vec[0]->forward_pointers[0] != node_to_delete) {
            return end(); // Node not found via predecessor scan, possibly inconsistent state or bad iterator
        }

        Node* successor_node_ptr = node_to_delete->forward_pointers[0];
        iterator next_iter(successor_node_ptr); 

        // Unlink forward pointers - THIS IS THE CORRECTED LOOP
        int node_actual_level = static_cast<int>(node_to_delete->forward_pointers.size()) - 1;
        for (int i = 0; i <= node_actual_level; ++i) { 
            if (static_cast<size_t>(i) < update_vec.size() && update_vec[i] &&
                static_cast<size_t>(i) < update_vec[i]->forward_pointers.size() &&
                update_vec[i]->forward_pointers[i] == node_to_delete) {
                // Accessing node_to_delete->forward_pointers[i] is safe now because i <= node_actual_level
                update_vec[i]->forward_pointers[i] = node_to_delete->forward_pointers[i];
            }
        }

        Node* predecessor_lvl0 = node_to_delete->prev_pointer; 
        Node* successor_lvl0 = successor_node_ptr; 

        if (predecessor_lvl0) { 
            predecessor_lvl0->forward_pointers[0] = successor_lvl0;
        }

        if (successor_lvl0 != tail) {
            assert(successor_lvl0 != nullptr && "Successor at L0 is null but not tail in erase");
            successor_lvl0->prev_pointer = predecessor_lvl0;
        } else { 
            tail->prev_pointer = predecessor_lvl0;
        }
        
        internal_delete_node(node_to_delete);
        list_size--;

        // Adjust current_max_level if necessary
        while (current_max_level > 0 &&
               head && static_cast<size_t>(current_max_level) < head->forward_pointers.size() && 
               head->forward_pointers[current_max_level] == tail) {
            current_max_level--;
        }
        return next_iter;
    }

    size_type erase(const value_type& value) {
        size_type num_erased = 0;
        iterator it = this->lower_bound(value);

        while (it != this->end() && it.get_node() != tail && it.get_node()->value == value) {
            it = this->erase(static_cast<const_iterator>(it)); 
            num_erased++;
        }
        return num_erased;
    }

    iterator lower_bound(const value_type& value) {
        std::vector<Node*> update_vec(MAX_LEVEL, nullptr);
        internal_find_predecessors(value, update_vec);
        if (!update_vec[0] || static_cast<size_t>(0) >= update_vec[0]->forward_pointers.size()) {
            return end(); 
        }
        return iterator(update_vec[0]->forward_pointers[0]);
    }

    const_iterator lower_bound(const value_type& value) const {
        std::vector<Node*> update_vec(MAX_LEVEL, nullptr);
        internal_find_predecessors(value, update_vec);
        if (!update_vec[0] || static_cast<size_t>(0) >= update_vec[0]->forward_pointers.size()) {
            return cend();
        }
        return const_iterator(update_vec[0]->forward_pointers[0]);
    }

    iterator find(const value_type& value) {
        iterator it = lower_bound(value);
        if (it != end() && it.get_node() != tail && it.get_node()->value == value) { 
            return it;
        }
        return end();
    }

    const_iterator find(const value_type& value) const {
        const_iterator it = lower_bound(value);
        if (it != cend() && it.get_node() != tail && it.get_node()->value == value) { 
            return it;
        }
        return cend();
    }

    bool contains(const_reference value) const {
        Node* current = head;
        for (int i = current_max_level; i >= 0; --i) {
            while (static_cast<size_t>(i) < current->forward_pointers.size() &&
                   current->forward_pointers[i] != tail &&
                   current->forward_pointers[i]->value < value) {
                current = current->forward_pointers[i];
            }
        }
        current = current->forward_pointers[0]; 
        return (current != tail && current->value == value);
    }

    void swap(SkipList& other) noexcept {
        if (this == &other) {
            return; 
        }
        using std::swap;
        swap(head, other.head);
        swap(tail, other.tail);
        swap(list_size, other.list_size);
        swap(current_max_level, other.current_max_level);
        swap(probability, other.probability);
        swap(random_engine, other.random_engine); 
        
        if (std::allocator_traits<NodeAllocator>::propagate_on_container_swap::value) {
            swap(node_alloc, other.node_alloc);
        } else {
            assert(node_alloc == other.node_alloc && "Allocators must be equal if not propagated on swap and POCMA is false");
        }
    }
};

template <typename T, typename Allocator>
bool operator==(const SkipList<T, Allocator>& lhs, const SkipList<T, Allocator>& rhs) {
    if (lhs.size() != rhs.size()) return false;
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template <typename T, typename Allocator>
bool operator!=(const SkipList<T, Allocator>& lhs, const SkipList<T, Allocator>& rhs) {
    return !(lhs == rhs);
}

template <typename T, typename Allocator>
bool operator<(const SkipList<T, Allocator>& lhs, const SkipList<T, Allocator>& rhs) {
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, typename Allocator>
bool operator<=(const SkipList<T, Allocator>& lhs, const SkipList<T, Allocator>& rhs) {
    return !(rhs < lhs);
}

template <typename T, typename Allocator>
bool operator>(const SkipList<T, Allocator>& lhs, const SkipList<T, Allocator>& rhs) {
    return rhs < lhs;
}

template <typename T, typename Allocator>
bool operator>=(const SkipList<T, Allocator>& lhs, const SkipList<T, Allocator>& rhs) {
    return !(lhs < rhs);
}

namespace std {
    template <typename T, typename Allocator>
    void swap(SkipList<T, Allocator>& lhs, SkipList<T, Allocator>& rhs) noexcept {
        lhs.swap(rhs);
    }
} 

#endif // SKIP_LIST_H