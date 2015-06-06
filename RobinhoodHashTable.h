#ifndef __ROBINHOOD_HASH_TABLE_H__
#define __ROBINHOOD_HASH_TABLE_H__

#include <cassert>
#include <cstddef>
#include <functional>
#include <vector>

/*
 * Robinhood HashTable, see:
 * http://sebastiansylvan.com/2013/05/08/robin-hood-hashing-should-be-your-default-hash-table-implementation/
 *
 */

template<class Key, class Val, class Hash = std::hash<Key>,
         class KeyEqual = std::equal_to<Key>>
struct RobinhoodHashTable {

  RobinhoodHashTable(size_t size) :
    m_hasher(),
    m_keyeq(),
    m_size(0),
    m_cap(size)
  {
    assert(size % 2 == 0);
    assert(size > 2);
    m_keys = static_cast<Key*>(calloc(size, sizeof(Key)));
    m_vals = static_cast<Val*>(calloc(size, sizeof(Val)));
    m_hashes = static_cast<size_t*>(calloc(size, sizeof(size_t)));
  }

  ~RobinhoodHashTable() {
    for (size_t i = 0; i < m_cap; i++) {
      const auto pos_hash = elem_hash(i);
      if (!is_deleted(pos_hash)) {
        m_keys[i].~Key();
        m_vals[i].~Val();
      }
    }
    free(m_keys);
    free(m_vals);
    free(m_hashes);
  }

  bool insert(Key key, Val val) {
    if (m_size >= (7 * m_cap / 8)) {
      grow();
    }
    auto hash = hash_of(key);
    if (insert_helper(key, val, hash)) {
      ++m_size;
      return true;
    } else {
      return false;
    }
  }

  bool remove(const Key& key) {
    const auto idx = lookup_index(key);
    if (idx == -1) {
      return false;
    }
    m_keys[idx].~Key();
    m_vals[idx].~Val();
    m_hashes[idx] |= 0x8000000000000000;
    --m_size;
    return true;
  }

  const Val* lookup(const Key& key) const {
    const auto idx = lookup_index(key);
    if (idx == -1) {
      return nullptr;
    } else {
      return m_vals + idx;
    }
  }

  ssize_t lookup_index(const Key& key) const {
    const auto hash = hash_of(key);
    auto pos = desired_pos(hash);
    size_t dist = 0;
    for (;;) {
      const auto pos_hash = elem_hash(pos);
      if (pos_hash == 0) {
        // Uninitialized slot during probe means does not exist
        return -1;
      } else if (dist > probe_distance(pos_hash, pos)) {
        // If we've found an element who is less than our current probe distance
        // then it means the key does not exist, because we keep the invariant
        // that every key on its insertion probe evicts any key that has a
        // shorter probe distance.
        return -1;
      } else if (pos_hash == hash && m_keyeq(m_keys[pos], key)) {
        return pos;
      }
      pos = (pos + 1) % m_cap;
      ++dist;
    }
  }

  size_t size() const {
    return m_size;
  }

  size_t cap() const {
    return m_cap;
  }

 private:

  bool insert_helper(Key key, Val val, size_t hash) {
    auto pos = desired_pos(hash);
    size_t dist = 0;
    for (;;) {
      const auto pos_hash = elem_hash(pos);
      if (pos_hash == 0) {
        // Empty Slot
        construct(pos, hash, std::move(key), std::move(val));
        return true;
      }
      size_t existing_dist = probe_distance(pos_hash, pos);
      if (existing_dist < dist) {
        if (is_deleted(pos_hash)) {
          // Replace tombstone
          construct(pos, hash, std::move(key), std::move(val));
          return true;
        }
        // Current Insertion has traveled farther, Robinhood time
        std::swap(hash, m_hashes[pos]);
        std::swap(key, m_keys[pos]);
        std::swap(val, m_vals[pos]);
        dist = existing_dist;
      } else if (existing_dist == dist) {
        if (is_deleted(pos_hash)) {
          construct(pos, hash, std::move(key), std::move(val));
          return true;
        }
        if (pos_hash == hash && m_keyeq(m_keys[pos], key)) {
          // Conflict
          return false;
        }
      }
      pos = (pos + 1) % m_cap;
      ++dist;
    }
  }

  void grow() {
    const auto old_cap = m_cap;
    m_cap = m_cap * 2;
    Key* tmp_keys = m_keys;
    Val* tmp_vals = m_vals;
    size_t* tmp_hashes = m_hashes;
    m_keys = static_cast<Key*>(calloc(m_cap, sizeof(Key)));
    m_vals = static_cast<Val*>(calloc(m_cap, sizeof(Val)));
    m_hashes = static_cast<size_t*>(calloc(m_cap, sizeof(size_t)));
    for (size_t i = 0; i < old_cap; i++) {
      const auto hash = tmp_hashes[i];
      if (!is_empty(hash) && !is_deleted(hash)) {
        insert_helper(tmp_keys[i], tmp_vals[i], hash);
      }
    }
    free(tmp_keys);
    free(tmp_vals);
    free(tmp_hashes);
  }

  bool is_empty(size_t hash) const {
    return hash == 0;
  }

  bool is_deleted(size_t hash) const {
    return hash & 0x8000000000000000;
  }

  size_t probe_distance(size_t elem_hash, size_t cur_pos) const {
    const size_t desired_pos = elem_hash % m_cap;
    return ((cur_pos + m_cap) - desired_pos) % m_cap;
  }

  size_t desired_pos(const size_t hash) const {
    return hash % m_cap;
  }

  size_t elem_hash(const size_t pos) const {
    return m_hashes[pos];
  }

  size_t hash_of(const Key& key) const {
    // Reserve top-bit for tombstone marker, always set at least one bit
    return (m_hasher(key) & 0x7FFFFFFFFFFFFFFF) | 0x2;
  }

  void construct(size_t pos, size_t hash, const Key&& key, const Val&& val) {
    m_hashes[pos] = hash;
    m_keys[pos] = key;
    m_vals[pos] = val;
  }

  Hash m_hasher;
  KeyEqual m_keyeq;
  size_t m_size;
  size_t m_cap;
  Val* m_vals;
  Key* m_keys;
  size_t* m_hashes;
};

#endif
