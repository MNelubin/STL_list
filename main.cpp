
#include <iostream>
#include <list>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include "include/skip_list.h" 

int main() {
    // Original SkipList example
    SkipList<int> mySkipList;

    // Insert elements into the list
    mySkipList.insert(10);
    mySkipList.insert(30);
    mySkipList.insert(20);
    mySkipList.insert(5);
    mySkipList.insert(25);

    // Print the elements of the SkipList using an iterator
    std::cout << "Elements in SkipList: ";
    for (const auto& element : mySkipList) {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    // Example of erasing an element
    mySkipList.erase(20);

    // Print the elements again after erasing
    std::cout << "Elements after erasing 20: ";
    for (const auto& element : mySkipList) {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    std::cout << "\n--- Performance Test ---\n";

    const int num_elements = 10000; // Number of elements for testing
    std::vector<int> test_data(num_elements);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, num_elements * 10);

    // Generate random data
    for (int i = 0; i < num_elements; ++i) {
        test_data[i] = distrib(gen);
    }

    // Test SkipList insertion
    SkipList<int> skipListTest;
    auto start_skip = std::chrono::high_resolution_clock::now();
    for (int val : test_data) {
        skipListTest.insert(val);
    }
    auto end_skip = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_skip = end_skip - start_skip;
    std::cout << "SkipList insertion time for " << num_elements << " elements: " << elapsed_skip.count() << " seconds\n";

    // Test std::list insertion (sorted insertion for fair comparison)
    std::list<int> stdListTest;
    auto start_std = std::chrono::high_resolution_clock::now();
    for (int val : test_data) {
        // Find the correct position to insert to maintain sorted order
        auto it = stdListTest.begin();
        while (it != stdListTest.end() && *it < val) {
            ++it;
        }
        stdListTest.insert(it, val);
    }
    auto end_std = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_std = end_std - start_std;
    std::cout << "std::list sorted insertion time for " << num_elements << " elements: " << elapsed_std.count() << " seconds\n";

    // Optional: Test search performance
    std::cout << "\n--- Search Test ---\n";
    const int num_searches = 1000;
    std::vector<int> search_data(num_searches);
    for (int i = 0; i < num_searches; ++i) {
        search_data[i] = distrib(gen);
    }

    // Test SkipList search
    int found_count_skip = 0;
    auto start_search_skip = std::chrono::high_resolution_clock::now();
    for (int val : search_data) {
        if (skipListTest.find(val) != skipListTest.end()) {
            found_count_skip++;
        }
    }
    auto end_search_skip = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_search_skip = end_search_skip - start_search_skip;
    std::cout << "SkipList search time for " << num_searches << " elements: " << elapsed_search_skip.count() << " seconds (found " << found_count_skip << ")\n";

    // Test std::list search
    int found_count_std = 0;
    auto start_search_std = std::chrono::high_resolution_clock::now();
    for (int val : search_data) {
        auto it = stdListTest.begin();
        while (it != stdListTest.end() && *it != val) {
            ++it;
        }
        if (it != stdListTest.end()) {
            found_count_std++;
        }
    }
    auto end_search_std = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_search_std = end_search_std - start_search_std;
    std::cout << "std::list search time for " << num_searches << " elements: " << elapsed_search_std.count() << " seconds (found " << found_count_std << ")\n";

    // Test SkipList deletion by value
    std::cout << "\n--- Deletion Test ---\n";
    SkipList<int> skipListDeleteTest = skipListTest; // Copy the list after insertion
    auto start_delete_skip_value = std::chrono::high_resolution_clock::now();
    for (int val : search_data) { // Use search_data for values to delete
        skipListDeleteTest.erase(val);
    }
    auto end_delete_skip_value = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_delete_skip_value = end_delete_skip_value - start_delete_skip_value;
    std::cout << "SkipList deletion by value time for " << num_searches << " elements: " << elapsed_delete_skip_value.count() << " seconds\n";

    // Test std::list deletion by value
    std::list<int> stdListDeleteTest = stdListTest; // Copy the list after insertion
    auto start_delete_std_value = std::chrono::high_resolution_clock::now();
    for (int val : search_data) { // Use search_data for values to delete
        stdListDeleteTest.remove(val); // std::list::remove removes all occurrences of value
    }
    auto end_delete_std_value = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_delete_std_value = end_delete_std_value - start_delete_std_value;
    std::cout << "std::list deletion by value time for " << num_searches << " elements: " << elapsed_delete_std_value.count() << " seconds\n";

    
    return 0;
}