#ifndef STABLE_VECTOR_H
#define STABLE_VECTOR_H

#include <boost/container/static_vector.hpp>
#include <memory>
#include <stdexcept>
#include <vector>
#include <atomic>
#include <thread>
#include <cstring> // for memcpy
#include <type_traits>



template <class T>
class SeqLock {
  static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

  std::atomic<uint64_t> version;
  alignas(64) T data;

public:
  SeqLock() : version(0), data() {}

  void store(const T& newData) {
    version.fetch_add(1, std::memory_order_acquire); 
    std::memcpy(&data, &newData, sizeof(T));
    version.fetch_add(1, std::memory_order_release); 
  }

  T load() const {
    T result;
    uint64_t v1, v2;
    do {
      v1 = version.load(std::memory_order_acquire);
      while (v1 & 1) { // writer in progress
        std::this_thread::yield();
        v1 = version.load(std::memory_order_acquire);
      }
      std::memcpy(&result, &data, sizeof(T));
      v2 = version.load(std::memory_order_acquire);
    } while (v1 != v2);
    return result;
  }
};

// ------------------------
// Stable Vector
// ------------------------

template <class T, size_t ChunkSize>
struct stable_vector {
  static_assert(ChunkSize % 2 == 0, "ChunkSize must be even");
  static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

  using Chunk = boost::container::static_vector<SeqLock<T>, ChunkSize>;

  struct ChunkWithLocks {
    std::unique_ptr<Chunk> data;
    size_t count;

    ChunkWithLocks() : data(std::make_unique<Chunk>()), count(0) {}
  };

  std::vector<ChunkWithLocks> mChunks;

  stable_vector() = default;

  void push_back(const T& value) {
    if (mChunks.empty() || mChunks.back().count == ChunkSize) {
      mChunks.emplace_back();
    }
    auto& chunk = mChunks.back();
    chunk.data->emplace_back(); 
    (*chunk.data)[chunk.count].store(value);
    ++chunk.count;
  }

  T operator[](size_t i) const {
    size_t chunkIndex = i / ChunkSize;
    size_t indexInChunk = i % ChunkSize;
    if (chunkIndex >= mChunks.size()) {
      throw std::out_of_range("Index out of range");
    }
    return (*mChunks[chunkIndex].data)[indexInChunk].load();
  }

  size_t size() const {
    size_t total = 0;
    for (const auto& chunk : mChunks) {
      total += chunk.count;
    }
    return total;
  }
};

#endif // STABLE_VECTOR_H
