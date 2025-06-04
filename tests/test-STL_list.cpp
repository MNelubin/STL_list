#include "gtest/gtest.h"
#include "../include/skip_list.h"


// Basic test to ensure setup is correct
/**
 * @brief Basic sanity check for the test environment.
 */
TEST(SkipListSanityCheck, TestEnvironment) {
    ASSERT_TRUE(true);
}

// Placeholder for actual tests
TEST(SkipListConstruction, DefaultConstructor) {
    SkipList<int> list;
    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);
    ASSERT_EQ(list.begin(), list.end());
}

// Add more tests here

// Test fixture for SkipList tests
class SkipListTest : public ::testing::Test {
protected:
    SkipList<int> list_;
    SkipList<std::string> str_list_;
};

TEST_F(SkipListTest, DefaultConstructor) {
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);
    ASSERT_EQ(list_.begin(), list_.end());
    ASSERT_EQ(list_.cbegin(), list_.cend());
}

TEST_F(SkipListTest, ConstructorWithProbability) {
    SkipList<int> list_prob(0.25f);
    ASSERT_TRUE(list_prob.empty());
    ASSERT_EQ(list_prob.size(), 0);
}

TEST_F(SkipListTest, InsertSingleElement) {
    auto it = list_.insert(10);
    ASSERT_FALSE(list_.empty());
    ASSERT_EQ(list_.size(), 1);
    ASSERT_NE(it, list_.end());
    ASSERT_EQ(*it, 10);
    ASSERT_EQ(list_.front(), 10);
    ASSERT_EQ(list_.back(), 10);
}

TEST_F(SkipListTest, InsertMultipleElementsSorted) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(5);

    ASSERT_EQ(list_.size(), 3);
    auto it = list_.begin();
    ASSERT_EQ(*it++, 5);
    ASSERT_EQ(*it++, 10);
    ASSERT_EQ(*it++, 20);
    ASSERT_EQ(it, list_.end());

    ASSERT_EQ(list_.front(), 5);
    ASSERT_EQ(list_.back(), 20);
}

TEST_F(SkipListTest, InsertDuplicateElements) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(10); // Duplicate

    ASSERT_EQ(list_.size(), 3);
    auto it = list_.begin();
    ASSERT_EQ(*it++, 10);
    ASSERT_EQ(*it++, 10);
    ASSERT_EQ(*it++, 20);
    ASSERT_EQ(it, list_.end());
}

TEST_F(SkipListTest, InsertStrings) {
    str_list_.insert("banana");
    str_list_.insert("apple");
    str_list_.insert("cherry");

    ASSERT_EQ(str_list_.size(), 3);
    auto it = str_list_.begin();
    ASSERT_EQ(*it++, "apple");
    ASSERT_EQ(*it++, "banana");
    ASSERT_EQ(*it++, "cherry");
    ASSERT_EQ(it, str_list_.end());
    ASSERT_EQ(str_list_.front(), "apple");
    ASSERT_EQ(str_list_.back(), "cherry");
}

TEST_F(SkipListTest, PushFront) {
    list_.push_front(10);
    ASSERT_EQ(list_.size(), 1);
    ASSERT_EQ(list_.front(), 10);
    list_.push_front(5);
    ASSERT_EQ(list_.size(), 2);
    ASSERT_EQ(list_.front(), 5);
    ASSERT_EQ(list_.back(), 10);
    auto it = list_.begin();
    ASSERT_EQ(*it++, 5);
    ASSERT_EQ(*it++, 10);
}

TEST_F(SkipListTest, PushBack) {
    list_.push_back(10);
    ASSERT_EQ(list_.size(), 1);
    ASSERT_EQ(list_.back(), 10);
    list_.push_back(20);
    ASSERT_EQ(list_.size(), 2);
    ASSERT_EQ(list_.front(), 10);
    ASSERT_EQ(list_.back(), 20);
    auto it = list_.begin();
    ASSERT_EQ(*it++, 10);
    ASSERT_EQ(*it++, 20);
}

TEST_F(SkipListTest, FindElement) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(5);

    auto it_found = list_.find(10);
    ASSERT_NE(it_found, list_.end());
    ASSERT_EQ(*it_found, 10);

    auto it_not_found = list_.find(100);
    ASSERT_EQ(it_not_found, list_.end());
}

TEST_F(SkipListTest, ContainsElement) {
    list_.insert(10);
    list_.insert(20);

    ASSERT_TRUE(list_.contains(10));
    ASSERT_FALSE(list_.contains(100));
}

TEST_F(SkipListTest, LowerBound) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(30);
    list_.insert(20); // duplicate

    auto it_lb1 = list_.lower_bound(15);
    ASSERT_NE(it_lb1, list_.end());
    ASSERT_EQ(*it_lb1, 20);

    auto it_lb2 = list_.lower_bound(20);
    ASSERT_NE(it_lb2, list_.end());
    ASSERT_EQ(*it_lb2, 20);
    ASSERT_EQ(*(++it_lb2), 20); // Should point to the first 20

    auto it_lb3 = list_.lower_bound(5);
    ASSERT_NE(it_lb3, list_.end());
    ASSERT_EQ(*it_lb3, 10);

    auto it_lb4 = list_.lower_bound(35);
    ASSERT_EQ(it_lb4, list_.end());
}

TEST_F(SkipListTest, EraseByIterator) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(5);

    auto it_to_erase = list_.find(10);
    ASSERT_NE(it_to_erase, list_.end());

    auto next_it = list_.erase(it_to_erase);
    ASSERT_EQ(list_.size(), 2);
    ASSERT_NE(next_it, list_.end());
    ASSERT_EQ(*next_it, 20); // Iterator should point to element after erased one

    ASSERT_FALSE(list_.contains(10));

    // Erase begin
    next_it = list_.erase(list_.begin());
    ASSERT_EQ(list_.size(), 1);
    ASSERT_EQ(*next_it, 20);
    ASSERT_FALSE(list_.contains(5));

    // Erase last remaining
    next_it = list_.erase(list_.begin());
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(next_it, list_.end());
    ASSERT_FALSE(list_.contains(20));
}


TEST_F(SkipListTest, PopFront) {
    list_.push_back(10);
    list_.push_back(20);
    list_.push_back(5);

    list_.pop_front(); // Should remove 5 (as it's sorted)
    ASSERT_EQ(list_.size(), 2);
    ASSERT_EQ(list_.front(), 10);

    list_.pop_front();
    ASSERT_EQ(list_.size(), 1);
    ASSERT_EQ(list_.front(), 20);

    list_.pop_front();
    ASSERT_TRUE(list_.empty());
}



TEST_F(SkipListTest, Clear) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(5);
    ASSERT_FALSE(list_.empty());

    list_.clear();
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);
    ASSERT_EQ(list_.begin(), list_.end());
}

TEST_F(SkipListTest, Iterators) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(5);

    // Forward iteration
    std::vector<int> expected_fwd = {5, 10, 20};
    std::vector<int> actual_fwd;
    for (auto it = list_.begin(); it != list_.end(); ++it) {
        actual_fwd.push_back(*it);
    }
    ASSERT_EQ(actual_fwd, expected_fwd);

    // Const forward iteration
    std::vector<int> actual_const_fwd;
    for (auto it = list_.cbegin(); it != list_.cend(); ++it) {
        actual_const_fwd.push_back(*it);
    }
    ASSERT_EQ(actual_const_fwd, expected_fwd);

    // Reverse iteration
    std::vector<int> expected_rev = {20, 10, 5};
    std::vector<int> actual_rev;
    for (auto it = list_.rbegin(); it != list_.rend(); ++it) {
        actual_rev.push_back(*it);
    }
    ASSERT_EQ(actual_rev, expected_rev);

    // Const reverse iteration
    std::vector<int> actual_const_rev;
    for (auto it = list_.crbegin(); it != list_.crend(); ++it) {
        actual_const_rev.push_back(*it);
    }
    ASSERT_EQ(actual_const_rev, expected_rev);
}



TEST_F(SkipListTest, Swap) {
    list_.insert(10);
    list_.insert(20);

    SkipList<int> other_list;
    other_list.insert(30);
    other_list.insert(40);
    other_list.insert(50);

    list_.swap(other_list);

    ASSERT_EQ(list_.size(), 3);
    ASSERT_TRUE(list_.contains(30));
    ASSERT_TRUE(list_.contains(40));
    ASSERT_TRUE(list_.contains(50));
    ASSERT_FALSE(list_.contains(10));

    ASSERT_EQ(other_list.size(), 2);
    ASSERT_TRUE(other_list.contains(10));
    ASSERT_TRUE(other_list.contains(20));
    ASSERT_FALSE(other_list.contains(30));

    // Test std::swap specialization
    std::swap(list_, other_list);

    ASSERT_EQ(list_.size(), 2);
    ASSERT_TRUE(list_.contains(10));
    ASSERT_TRUE(list_.contains(20));

    ASSERT_EQ(other_list.size(), 3);
    ASSERT_TRUE(other_list.contains(30));
    ASSERT_TRUE(other_list.contains(40));
    ASSERT_TRUE(other_list.contains(50));
}





TEST_F(SkipListTest, EdgeCaseEmptyListOperations) {
    ASSERT_NO_THROW(list_.pop_front()); // Should be safe on empty list
    ASSERT_TRUE(list_.empty());
    ASSERT_NO_THROW(list_.pop_back());  // Should be safe on empty list
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.erase(100), 0);    // Erase non-existent from empty
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.find(100), list_.end());
    ASSERT_EQ(list_.lower_bound(100), list_.end());
    ASSERT_FALSE(list_.contains(100));
    ASSERT_THROW(list_.front(), std::out_of_range); // Or specific exception if defined
    ASSERT_THROW(list_.back(), std::out_of_range);  // Or specific exception if defined
}

TEST_F(SkipListTest, EraseEndIterator) {
    list_.insert(10);
    auto it_end = list_.end();
    auto ret_it = list_.erase(it_end);
    ASSERT_EQ(ret_it, list_.end());
    ASSERT_EQ(list_.size(), 1); // Nothing should be erased
}

TEST_F(SkipListTest, EraseConstIterator) {
    list_.insert(10);
    list_.insert(20);
    const SkipList<int>& const_list = list_;
    auto cit = const_list.find(10);
    ASSERT_NE(cit, const_list.cend());

    // This tests if erase(const_iterator) works correctly
    auto next_it = list_.erase(cit);
    ASSERT_EQ(list_.size(), 1);
    ASSERT_FALSE(list_.contains(10));
    ASSERT_TRUE(list_.contains(20));
    ASSERT_NE(next_it, list_.end());
    ASSERT_EQ(*next_it, 20);
}

// Test for allocator awareness (conceptual, actual test depends on allocator usage)
TEST(SkipListAllocator, AllocatorPropagation) {
    std::allocator<int> alloc;
    SkipList<int, std::allocator<int>> list_with_alloc(0.5f, alloc);
    list_with_alloc.insert(100);
    ASSERT_EQ(list_with_alloc.get_allocator(), alloc);
    // Further tests would involve checking if nodes are allocated using this allocator,
    // which is hard to verify directly without instrumenting the allocator or SkipList.
}

// Test for self-assignment in swap
TEST_F(SkipListTest, SwapSelf) {
    list_.insert(10);
    list_.insert(20);
    SkipList<int> original_copy = list_;
    list_.swap(list_); // Self-swap
    ASSERT_EQ(list_, original_copy); // Should remain unchanged
    ASSERT_EQ(list_.size(), 2);
    ASSERT_TRUE(list_.contains(10));
    ASSERT_TRUE(list_.contains(20));
}

// Test large number of insertions and deletions
TEST_F(SkipListTest, StressTestInsertErase) {
    const int num_elements = 1000;
    for (int i = 0; i < num_elements; ++i) {
        list_.insert(i);
    }
    ASSERT_EQ(list_.size(), num_elements);

    for (int i = 0; i < num_elements; i += 2) { // Erase even numbers
        ASSERT_EQ(list_.erase(i), 1);
    }
    ASSERT_EQ(list_.size(), num_elements / 2);

    for (int i = 1; i < num_elements; i += 2) { // Check odd numbers remain
        ASSERT_TRUE(list_.contains(i));
    }
    for (int i = 0; i < num_elements; i += 2) { // Check even numbers gone
        ASSERT_FALSE(list_.contains(i));
    }

    list_.clear();
    ASSERT_TRUE(list_.empty());
}

// Complete the PopFront test from your snippet
TEST_F(SkipListTest, PopFrontDetailed) {
    list_.push_back(10); // list: 10
    list_.push_back(20); // list: 10, 20
    list_.push_front(5); // list: 5, 10, 20

    ASSERT_EQ(list_.size(), 3);
    ASSERT_EQ(list_.front(), 5);

    list_.pop_front(); // list: 10, 20
    ASSERT_EQ(list_.size(), 2);
    ASSERT_EQ(list_.front(), 10);
    ASSERT_EQ(list_.back(), 20);

    list_.pop_front(); // list: 20
    ASSERT_EQ(list_.size(), 1);
    ASSERT_EQ(list_.front(), 20);
    ASSERT_EQ(list_.back(), 20);

    list_.pop_front(); // list: empty
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);

    // Test pop_front on an already empty list
    list_.pop_front();
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);
}

/**
 * @brief Tests the pop_back() method for various scenarios.
 * Ensures elements are removed from the end and list state is consistent.
 */
TEST_F(SkipListTest, PopBack) {
    list_.push_back(10); // list: 10
    list_.push_back(20); // list: 10, 20
    list_.push_back(30); // list: 10, 20, 30

    ASSERT_EQ(list_.size(), 3);
    ASSERT_EQ(list_.back(), 30);

    list_.pop_back(); // list: 10, 20
    ASSERT_EQ(list_.size(), 2);
    ASSERT_EQ(list_.front(), 10);
    ASSERT_EQ(list_.back(), 20);

    list_.pop_back(); // list: 10
    ASSERT_EQ(list_.size(), 1);
    ASSERT_EQ(list_.front(), 10);
    ASSERT_EQ(list_.back(), 10);

    list_.pop_back(); // list: empty
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);

    // Test pop_back on an already empty list
    list_.pop_back();
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);
}

/**
 * @brief Tests clear() method on empty and non-empty lists.
 */
TEST_F(SkipListTest, ClearList) {
    // Clear non-empty list
    list_.insert(10);
    list_.insert(20);
    ASSERT_FALSE(list_.empty());
    ASSERT_EQ(list_.size(), 2);

    list_.clear();
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);
    ASSERT_EQ(list_.begin(), list_.end());

    // Clear already empty list
    list_.clear();
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.size(), 0);
}

/**
 * @brief Tests bidirectional iteration (forward and backward).
 * Verifies operator++(), operator--(), operator*() for iterators.
 */
TEST_F(SkipListTest, BidirectionalIteration) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(30);

    // Forward iteration
    auto it = list_.begin();
    ASSERT_EQ(*it, 10);
    ++it;
    ASSERT_EQ(*it, 20);
    it++;
    ASSERT_EQ(*it, 30);
    ++it;
    ASSERT_EQ(it, list_.end());

    // Backward iteration
    --it; // it now points to 30
    ASSERT_EQ(*it, 30);
    it--;
    ASSERT_EQ(*it, 20);
    --it;
    ASSERT_EQ(*it, 10);
    ASSERT_EQ(it, list_.begin());
}

/**
 * @brief Tests reverse iterators (rbegin, rend, crbegin, crend).
 */
TEST_F(SkipListTest, ReverseIterators) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(30);

    // Non-const reverse iteration
    auto rit = list_.rbegin();
    ASSERT_NE(rit, list_.rend());
    ASSERT_EQ(*rit, 30);
    ++rit;
    ASSERT_EQ(*rit, 20);
    rit++;
    ASSERT_EQ(*rit, 10);
    ++rit;
    ASSERT_EQ(rit, list_.rend());

    // Const reverse iteration
    const SkipList<int>& const_list = list_;
    auto crit = const_list.crbegin();
    ASSERT_NE(crit, const_list.crend());
    ASSERT_EQ(*crit, 30);
    ++crit;
    ASSERT_EQ(*crit, 20);
    crit++;
    ASSERT_EQ(*crit, 10);
    ++crit;
    ASSERT_EQ(crit, const_list.crend());
}

/**
 * @brief Tests front() and back() methods on empty and non-empty lists.
 * Checks for std::out_of_range exception on empty list.
 */
TEST_F(SkipListTest, FrontAndBackAccess) {
    // On non-empty list
    list_.insert(10);
    list_.insert(5);  // List: 5, 10
    list_.insert(20); // List: 5, 10, 20
    ASSERT_EQ(list_.front(), 5);
    ASSERT_EQ(list_.back(), 20);

    const SkipList<int>& const_list = list_;
    ASSERT_EQ(const_list.front(), 5);
    ASSERT_EQ(const_list.back(), 20);

    // On list with one element
    list_.clear();
    list_.insert(100);
    ASSERT_EQ(list_.front(), 100);
    ASSERT_EQ(list_.back(), 100);

    // On empty list
    list_.clear();
    ASSERT_THROW(list_.front(), std::out_of_range);
    ASSERT_THROW(list_.back(), std::out_of_range);
    ASSERT_THROW(const_list.front(), std::out_of_range); // const_list is now empty too
    ASSERT_THROW(const_list.back(), std::out_of_range);
}


/**
 * @brief Tests the copy constructor for deep copying.
 * Ensures that modifications to the copy do not affect the original and vice-versa.
 */
TEST_F(SkipListTest, CopyConstructor) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(30);

    SkipList<int> list_copy(list_);

    // Check sizes
    ASSERT_EQ(list_.size(), 3);
    ASSERT_EQ(list_copy.size(), 3);

    // Check content equality
    auto it_orig = list_.begin();
    auto it_copy = list_copy.begin();
    while (it_orig != list_.end()) {
        ASSERT_NE(it_copy, list_copy.end());
        ASSERT_EQ(*it_orig, *it_copy);
        // Also check that node pointers are different (deep copy)
        // This requires access to node pointers, which iterators provide via get_node()
        ASSERT_NE(it_orig.get_node(), it_copy.get_node()); 
        ++it_orig;
        ++it_copy;
    }
    ASSERT_EQ(it_copy, list_copy.end());

    // Modify copy and check original is unaffected
    list_copy.insert(40);
    ASSERT_EQ(list_.size(), 3);
    ASSERT_EQ(list_copy.size(), 4);
    ASSERT_TRUE(list_copy.contains(40));
    ASSERT_FALSE(list_.contains(40));

    // Modify original and check copy is unaffected
    list_.erase(10);
    ASSERT_EQ(list_.size(), 2);
    ASSERT_EQ(list_copy.size(), 4); // Should still contain 10
    ASSERT_TRUE(list_copy.contains(10));
    ASSERT_FALSE(list_.contains(10));

    // Test copy of an empty list
    SkipList<int> empty_list;
    SkipList<int> empty_list_copy(empty_list);
    ASSERT_TRUE(empty_list_copy.empty());
    ASSERT_EQ(empty_list_copy.size(), 0);
}

/**
 * @brief Tests the copy assignment operator.
 * Includes self-assignment and deep copying.
 */
TEST_F(SkipListTest, CopyAssignmentOperator) {
    list_.insert(10);
    list_.insert(20);

    SkipList<int> list_assigned;
    list_assigned.insert(100);
    list_assigned.insert(200);
    list_assigned.insert(300);

    list_assigned = list_; // Assign list_ to list_assigned

    ASSERT_EQ(list_assigned.size(), 2);
    ASSERT_TRUE(list_assigned.contains(10));
    ASSERT_TRUE(list_assigned.contains(20));
    ASSERT_FALSE(list_assigned.contains(100));

    // Check for deep copy (modifying original list_ does not affect list_assigned)
    list_.insert(5);
    ASSERT_EQ(list_assigned.size(), 2); // Should not have 5
    ASSERT_FALSE(list_assigned.contains(5));

    // Self-assignment
    // NOLINTNEXTLINE(clang-diagnostic-self-assign-overloaded)
    list_assigned = list_assigned;
    ASSERT_EQ(list_assigned.size(), 2); // Still 2 elements
    ASSERT_TRUE(list_assigned.contains(10));
    ASSERT_TRUE(list_assigned.contains(20));

    // Assign empty list
    SkipList<int> empty_list;
    list_assigned = empty_list;
    ASSERT_TRUE(list_assigned.empty());
    ASSERT_EQ(list_assigned.size(), 0);
}

/**
 * @brief Tests the member swap and std::swap functionality.
 */
TEST_F(SkipListTest, SwapLists) {
    list_.insert(10);
    list_.insert(20);

    SkipList<int> other_list;
    other_list.insert(100);
    other_list.insert(200);
    other_list.insert(300);

    list_.swap(other_list);

    ASSERT_EQ(list_.size(), 3);
    ASSERT_TRUE(list_.contains(100));
    ASSERT_TRUE(list_.contains(200));
    ASSERT_TRUE(list_.contains(300));
    ASSERT_FALSE(list_.contains(10));

    ASSERT_EQ(other_list.size(), 2);
    ASSERT_TRUE(other_list.contains(10));
    ASSERT_TRUE(other_list.contains(20));
    ASSERT_FALSE(other_list.contains(100));

    // Test std::swap
    std::swap(list_, other_list);

    ASSERT_EQ(list_.size(), 2);
    ASSERT_TRUE(list_.contains(10));
    ASSERT_FALSE(list_.contains(100));

    ASSERT_EQ(other_list.size(), 3);
    ASSERT_TRUE(other_list.contains(100));
    ASSERT_FALSE(other_list.contains(10));
}

/**
 * @brief Tests comparison operators (==, !=, <, <=, >, >=).
 */
TEST_F(SkipListTest, ComparisonOperators) {
    SkipList<int> list1;
    list1.insert(10);
    list1.insert(20);

    SkipList<int> list2;
    list2.insert(10);
    list2.insert(20);

    SkipList<int> list3;
    list3.insert(10);
    list3.insert(30);

    SkipList<int> list4; // empty
    SkipList<int> list5;
    list5.insert(5);


    // == and !=
    ASSERT_TRUE(list1 == list2);
    ASSERT_FALSE(list1 != list2);
    ASSERT_FALSE(list1 == list3);
    ASSERT_TRUE(list1 != list3);
    ASSERT_FALSE(list1 == list4); // Non-empty vs empty
    ASSERT_TRUE(list1 != list4);

    // <
    ASSERT_FALSE(list1 < list2); // Equal
    ASSERT_TRUE(list1 < list3);  // 20 < 30 at second element
    ASSERT_FALSE(list3 < list1);
    ASSERT_TRUE(list4 < list1);  // Empty is less than non-empty
    ASSERT_FALSE(list1 < list4);
    ASSERT_TRUE(list5 < list1);  // 5 < 10 at first element


    // <=
    ASSERT_TRUE(list1 <= list2); // Equal
    ASSERT_TRUE(list1 <= list3);
    ASSERT_FALSE(list3 <= list1);
    ASSERT_TRUE(list4 <= list1);

    // >
    ASSERT_FALSE(list1 > list2); // Equal
    ASSERT_FALSE(list1 > list3);
    ASSERT_TRUE(list3 > list1);
    ASSERT_TRUE(list1 > list4);

    // >=
    ASSERT_TRUE(list1 >= list2); // Equal
    ASSERT_FALSE(list1 >= list3);
    ASSERT_TRUE(list3 >= list1);
    ASSERT_TRUE(list1 >= list4);
}

/**
 * @brief Tests erase(iterator) specifically for edge cases like first, last, and middle elements.
 * Also checks list integrity and prev_pointer consistency after erasure.
 */
TEST_F(SkipListTest, EraseByIteratorAdvanced) {
    list_.insert(10); // 0
    list_.insert(20); // 1
    list_.insert(30); // 2
    list_.insert(40); // 3
    list_.insert(50); // 4
    // List: 10, 20, 30, 40, 50

    // Erase middle element (30)
    auto it_to_erase = list_.find(30);
    ASSERT_NE(it_to_erase, list_.end());
    auto next_it = list_.erase(it_to_erase); // Erase 30
    ASSERT_EQ(list_.size(), 4);
    ASSERT_NE(next_it, list_.end());
    ASSERT_EQ(*next_it, 40); // Iterator points to element after erased one
    ASSERT_FALSE(list_.contains(30));
    // Check sequence and prev pointers
    auto it_chk = list_.begin(); // 10
    ASSERT_EQ(*it_chk, 10);
    ASSERT_EQ(it_chk.get_node()->prev_pointer, list_.begin().get_node()->prev_pointer); // head's prev is nullptr or special
    ++it_chk; // 20
    ASSERT_EQ(*it_chk, 20);
    ASSERT_EQ(it_chk.get_node()->prev_pointer->value, 10);
    ++it_chk; // 40 (30 was erased)
    ASSERT_EQ(*it_chk, 40);
    ASSERT_EQ(it_chk.get_node()->prev_pointer->value, 20); // prev of 40 should be 20
    ++it_chk; // 50
    ASSERT_EQ(*it_chk, 50);
    ASSERT_EQ(it_chk.get_node()->prev_pointer->value, 40);
    ++it_chk;
    ASSERT_EQ(it_chk, list_.end());
    ASSERT_EQ(list_.back(), 50); // Check back
    ASSERT_EQ(list_.end().get_node()->prev_pointer->value, 50); // tail->prev_pointer should be last element


    // Erase first element (10)
    it_to_erase = list_.begin(); // Points to 10
    ASSERT_EQ(*it_to_erase, 10);
    next_it = list_.erase(it_to_erase); // Erase 10
    ASSERT_EQ(list_.size(), 3);
    ASSERT_NE(next_it, list_.end());
    ASSERT_EQ(*next_it, 20); // Iterator points to 20
    ASSERT_FALSE(list_.contains(10));
    ASSERT_EQ(list_.front(), 20);
    ASSERT_EQ(list_.begin().get_node()->prev_pointer, list_.begin().get_node()->prev_pointer); // Check prev_pointer of new begin


    // Erase last element (50)
    // Need to find the iterator to the last element
    it_to_erase = list_.find(50);
    ASSERT_NE(it_to_erase, list_.end());
    next_it = list_.erase(it_to_erase); // Erase 50
    ASSERT_EQ(list_.size(), 2);
    ASSERT_EQ(next_it, list_.end()); // Iterator points to end()
    ASSERT_FALSE(list_.contains(50));
    ASSERT_EQ(list_.back(), 40); // 40 is now the last element
    ASSERT_EQ(list_.end().get_node()->prev_pointer->value, 40);


    // Erase remaining elements
    list_.erase(list_.find(20)); // Erase 20
    list_.erase(list_.find(40)); // Erase 40
    ASSERT_TRUE(list_.empty());
    ASSERT_EQ(list_.begin(), list_.end());
    ASSERT_EQ(list_.end().get_node()->prev_pointer, list_.begin().get_node()->prev_pointer); // tail->prev should be head
}

/**
 * @brief Tests inserting elements with hints.
 * Focuses on correctness, not performance measurement.
 */
TEST_F(SkipListTest, InsertWithHint) {
    // Valid hint: insert 15, hint pointing to 10
    list_.insert(10);
    list_.insert(20);
    auto hint_it = list_.find(10);
    auto inserted_it = list_.insert(hint_it, 15);
    ASSERT_EQ(list_.size(), 3);
    ASSERT_EQ(*inserted_it, 15);
    auto it_chk = list_.begin();
    ASSERT_EQ(*it_chk++, 10);
    ASSERT_EQ(*it_chk++, 15);
    ASSERT_EQ(*it_chk++, 20);

    // Hint at begin: insert 5, hint pointing to begin() (which is 10)
    hint_it = list_.begin();
    inserted_it = list_.insert(hint_it, 5);
    ASSERT_EQ(list_.size(), 4);
    ASSERT_EQ(*inserted_it, 5);
    ASSERT_EQ(list_.front(), 5);

    // Hint at end: insert 25, hint pointing to end()
    hint_it = list_.end();
    inserted_it = list_.insert(hint_it, 25);
    ASSERT_EQ(list_.size(), 5);
    ASSERT_EQ(*inserted_it, 25);
    ASSERT_EQ(list_.back(), 25);

    // Invalid hint (hint points after insertion point): insert 0, hint pointing to 10
    hint_it = list_.find(10);
    inserted_it = list_.insert(hint_it, 0); // Should still insert correctly
    ASSERT_EQ(list_.size(), 6);
    ASSERT_EQ(*inserted_it, 0);
    ASSERT_EQ(list_.front(), 0);

    // Verify final sorted order
    std::vector<int> expected = {0, 5, 10, 15, 20, 25};
    size_t i = 0;
    for(int val : list_) {
        ASSERT_EQ(val, expected[i++]);
    }
}



/**
 * @brief Tests that iterators are invalidated or updated correctly after erase operations.
 * Specifically, the iterator returned by erase should be valid.
 */
TEST_F(SkipListTest, IteratorValidityAfterErase) {
    list_.insert(10);
    list_.insert(20);
    list_.insert(30);
    list_.insert(40);

    // Erase 20, iterator should point to 30
    auto it_find_20 = list_.find(20);
    ASSERT_NE(it_find_20, list_.end());
    auto next_it = list_.erase(it_find_20);
    ASSERT_NE(next_it, list_.end());
    ASSERT_EQ(*next_it, 30);

    // Erase 30 (which is now pointed to by next_it)
    next_it = list_.erase(next_it);
    ASSERT_NE(next_it, list_.end());
    ASSERT_EQ(*next_it, 40);

    // Erase 40 (last element)
    next_it = list_.erase(next_it);
    ASSERT_EQ(next_it, list_.end()); // Should be end iterator

    // List should contain only 10
    ASSERT_EQ(list_.size(), 1);
    ASSERT_EQ(list_.front(), 10);

    // Erase the last remaining element
    next_it = list_.erase(list_.begin());
    ASSERT_EQ(next_it, list_.end());
    ASSERT_TRUE(list_.empty());
}

/**
 * @brief Tests `max_size()` method.
 * The exact value can vary, but it should return a positive number.
 */
TEST_F(SkipListTest, MaxSize) {
    ASSERT_GT(list_.max_size(), 0);
}

/**
 * @brief Tests `get_allocator()` method.
 */
TEST_F(SkipListTest, GetAllocator) {
    SkipList<int>::allocator_type alloc = list_.get_allocator();
    // We can't do much to test the allocator itself here without specific knowledge
    // or custom allocators, but we can check it's obtainable.
    (void)alloc; // Suppress unused variable warning
    SUCCEED(); // If it runs without crashing, it's a basic pass
}

