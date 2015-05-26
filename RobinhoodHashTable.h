#include <functional>
#include <vector>
#include <cstddef>

template<class Key, class Val, class Hash = std::hash<Key>,
         class KeyEqual = std::equal_to<Key>>
struct RobinhoodHashTable {

  RobinhoodHashTable(size_t size) :
    m_hasher(),
    m_size(size),
    m_keys(size),
    m_vals(size),
    m_hashes(size)
  {}

  void insert(Key key, Val val) {
    auto hash = m_hasher(key);
    auto pos = desired_pos(hash);
    size_t dist = 0;
    for (;;) {
      if (m_hashes[pos] == 0) {
        // Empty Slot
        construct(pos, hash, std::move(key), std::move(val));
        return;
      }
      size_t existing_dist = probe_distance(elem_hash(pos), pos);
      if (existing_dist < dist) {
        // Current Insertion has traveled farther, Robinhood time
        if (is_deleted(elem_hash(pos))) {
          // Replace tombstone
          construct(pos, hash, std::move(key), std::move(val));
          return;
        }
        std::swap(hash, m_hashes[pos]);
        std::swap(key, m_keys[pos]);
        std::swap(val, m_vals[pos]);
        dist = existing_dist;
      }
      pos = (pos + 1) % m_size;
      ++dist;
    }
  }

  size_t lookup_index(const Key& key) const {
    const auto hash = m_hasher(key);
    auto pos = desired_pos(hash);
    size_t dist = 0;
    for (;;) {
      if (elem_hash(pos) == 0) {
        // Uninitialized slot during probe means does not exist
        return -1;
      } else if (dist > probe_distance(elem_hash(pos), pos)) {
        // If we've found an element who is less than our current probe distance
        // then it means the key does not exist, because we keep the invariant
        // that every key on its insertion probe evicts any key that has a
        // shorter probe distance.
        return -1;
      } else if (elem_hash(pos) == hash && m_keys[pos] == key) {
        return pos;
      }

      pos = (pos + 1) % m_size;
      ++dist;
    }
  }

  bool is_deleted(size_t hash) const {
    // The low bit is for the tombstone marker
    return hash & 0x1;
  }

  size_t probe_distance(size_t desired_pos, size_t cur_pos) const {
    return ((desired_pos + m_size) - cur_pos) % m_size;
  }

  size_t desired_pos(const size_t hash) const {
    return hash % m_size;
  }

  size_t elem_hash(const size_t pos) const {
    return m_hashes[pos];
  }

  void construct(size_t pos, size_t hash, const Key&& key, const Val&& val) {
    m_hashes[pos] = hash;
    m_keys[pos] = key;
    m_vals[pos] = val;
  }

  Hash m_hasher;
  size_t m_size;
  std::vector<Val> m_vals;
  std::vector<Key> m_keys;
  std::vector<size_t> m_hashes;
};
