#ifndef __BINARY_HEAP_H__
#define __BINARY_HEAP_H__

#include <cassert>
#include <cstdlib>
#include <new>

template <class Elem, class Key = uint64_t>
struct BinaryHeap {

  static constexpr size_t rootIdx = 0;

  struct Node {
    Elem elem;
    Key key;
  };

  BinaryHeap(size_t cap) :
    m_cap(cap),
    m_size(0)
  {
    m_array = static_cast<Node*>(std::calloc(m_cap, sizeof(Node)));
    if (m_array == nullptr) {
      m_cap = 0;
      throw std::bad_alloc();
    }
  }

  ~BinaryHeap() {
    for (size_t i = 0; i < m_size; i++) {
      m_array[i].~Node();
    }
    free(m_array);
  }

  void insert(Elem&& elem, Key&& key) {
    ++m_size;
    Node n;
    n.elem = elem;
    n.key = key;
    m_array[lastIdx()] = n;
    swimUp(lastIdx());
  }

  void popMin() {
    assert(m_size > 0);
    std::swap(m_array[lastIdx()], m_array[rootIdx]);
    m_array[lastIdx()].~Node();
    --m_size;
    sink(rootIdx);
  }

  bool isEmpty() {
    return m_size == 0;
  }

  const Elem& findMin() {
    assert(!isEmpty());
    return m_array[0].elem;
  }

  void merge(BinaryHeap&& heap) {
    // TODO(ptc)
    return;
  }

 private:

  void sink(size_t curIdx) {
    while (true) {
      const auto leftIdx = leftChildIdx(curIdx);
      const auto rightIdx = rightChildIdx(curIdx);
      if (leftIdx < m_size && rightIdx < m_size) {
        // Two children
        const auto minIdx = keyAt(leftIdx) < keyAt(rightIdx) ? leftIdx
                                                             : rightIdx;
        if (keyAt(minIdx) < keyAt(curIdx)) {
          std::swap(m_array[curIdx], m_array[minIdx]);
          curIdx = minIdx;
        } else {
          return;
        }
      } else if (leftIdx < m_size) {
        // Left only child
        if (keyAt(leftIdx) < keyAt(curIdx)) {
          std::swap(m_array[curIdx], m_array[leftIdx]);
          curIdx = leftIdx;
        } else {
          return;
        }
      } else if (rightIdx < m_size) {
        // Right only child
        if (keyAt(rightIdx) < keyAt(curIdx)) {
          std::swap(m_array[curIdx], m_array[rightIdx]);
          curIdx = rightIdx;
        } else {
          return;
        }
      } else {
        // No children
        return;
      }
    }
  }

  void swimUp(size_t cur) {
    auto parent = parentIdx(cur);
    while (cur != rootIdx && keyAt(parent) > keyAt(cur)) {
      std::swap(m_array[cur], m_array[parent]);
      cur = parent;
    }
  }

  Key keyAt(size_t idx) {
    return m_array[idx].key;
  }

  void grow() {
    auto new_array = static_cast<Elem*>(realloc(m_array, sizeof(Node) * m_cap));
    if (new_array == nullptr) {
      throw std::bad_alloc();
    }
    m_cap = m_cap << 1;
    m_array = new_array;
  }

  size_t parentIdx(size_t childIdx) {
    return (childIdx - 1) >> 1;
  }

  size_t leftChildIdx(size_t parentIdx) {
    return (parentIdx << 1) + 1;
  }

  size_t rightChildIdx(size_t parentIdx) {
    return (parentIdx << 1) + 2;
  }

  size_t lastIdx() {
    return m_size - 1;
  }

  size_t m_cap;
  size_t m_size;
  Node* m_array;

};

#endif
