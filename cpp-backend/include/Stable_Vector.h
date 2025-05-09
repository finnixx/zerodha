#ifndef STABLE_VECTOR_H
#define STABLE_VECTOR_H

#include <boost/container/static_vector.hpp>
#include <memory>
#include <stdexcept>
#include <vector>

// Static assertion to ensure ChunkSize is a multiple of 2
template <class T, size_t ChunkSize> struct stable_vector {
  static_assert(ChunkSize % 2 == 0, "ChunkSize needs to be a multiple of 2");

  using Chunk = boost::container::static_vector<T, ChunkSize>;
  std::vector<std::unique_ptr<Chunk>> mChunks;

  stable_vector() = default;

  // Add an element to the stable_vector
  void push_back(const T &value) {
    // If the last chunk is full or there are no chunks, create a new chunk
    if (mChunks.empty() || mChunks.back()->size() == ChunkSize) {
      mChunks.push_back(std::make_unique<Chunk>());
    }
    // Add the element to the last chunk
    mChunks.back()->push_back(value);
  }

  // Access an element by index
  T &operator[](size_t i) {
    size_t chunkIndex = i / ChunkSize; // Find which chunk the index belongs to
    size_t indexInChunk = i % ChunkSize; // Find the position within that chunk
    if (chunkIndex >= mChunks.size()) {  // Check if index is out of bounds
      throw std::out_of_range("Index out of range");
    }
    return (*mChunks[chunkIndex])[indexInChunk]; // Return the element from the
                                                 // chunk
  }

  // Size of the stable_vector
  size_t size() const {
    size_t totalSize = 0;
    for (const auto &chunk : mChunks) {
      totalSize += chunk->size();
    }
    return totalSize;
  }
};

#endif // STABLE_VECTOR_H
