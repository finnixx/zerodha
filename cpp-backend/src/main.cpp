#include <iostream>
#include "stable_vector.h"

int main() {
    // Create stable_vectors for asks and bids with chunk size of 4
    stable_vector<int, 4> asks;
    stable_vector<int, 4> bids;

    // Add 5 entries to asks and bids (as a simple example)
    asks.push_back(100);
    asks.push_back(200);
    asks.push_back(300);
    asks.push_back(400);
    asks.push_back(500);

    bids.push_back(50);
    bids.push_back(40);
    bids.push_back(30);
    bids.push_back(20);
    bids.push_back(10);

    // Print the contents of asks
    std::cout << "Asks: ";
    for (size_t i = 0; i < asks.size(); ++i) {
        std::cout << asks[i] << " ";
    }
    std::cout << std::endl;

    // Print the contents of bids
    std::cout << "Bids: ";
    for (size_t i = 0; i < bids.size(); ++i) {
        std::cout << bids[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
