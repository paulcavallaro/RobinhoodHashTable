#include "RobinhoodHashTable.h"
#include "Benchmark.h"
#include <string>
#include <unordered_map>

size_t probe_distance(size_t desired_pos, size_t cur_pos, size_t cap) {
  return desired_pos <= cur_pos ? (cur_pos - desired_pos)
    : ((cur_pos + cap) - desired_pos);
}

int main(int argc, char** argv) {
  RobinhoodHashTable<std::string, uint32_t> rht{4};
  std::vector<std::string> words = {
    "hello", "world", "how", "is", "it", "going",
    "some", "more", "words", "should", "work",
    "hello",
  };
  for (uint32_t i = 0; i < words.size(); i++) {
    auto ret = rht.insert(words[i], i);
    printf("Inserting '%s' ... %d\n", words[i].c_str(), ret);
  }
  const auto cap = rht.cap();
  printf("(cap): %lu (size): %lu\n", cap, rht.size());
  std::hash<std::string> hasher;
  for (auto& word : words) {
    const auto desired_pos = (hasher(word) & 0x7FFFFFFFFFFFFFFF) % cap;
    const auto idx = rht.lookup_index(word);
    printf("(idx): %lu (hash %% cap): %lu (dist): %lu (val): %u\n", idx,
           desired_pos, probe_distance(desired_pos, idx, cap),
           *rht.lookup(word));
  }
  for (auto& word : words) {
    printf("Removing '%s'\n", word.c_str());
    auto flag = rht.remove(word);
  }
  printf("(cap): %lu (size): %lu\n", rht.cap(), rht.size());

  const size_t numIters = 10;
  const size_t initSize = 80000;
  const size_t rangeMax = 1000000;
  printf("==std::unordered_map==\n");
  timeFunc(numIters, [] {
      std::unordered_map<size_t, size_t> map(initSize);
      for (size_t i = 0; i < rangeMax; i++) {
        map.emplace(i, i * 2);
      }
      for (size_t i = 0; i < rangeMax; i++) {
        if (i % 4 == 0) {
          map.erase(i);
        }
      }
      for (size_t i = 0; i < rangeMax; i++) {
        map.find(i);
      }
    });
  printf("==RobinhoodHashTable==\n");
  timeFunc(numIters, [] {
      RobinhoodHashTable<size_t, size_t> rht(initSize);
      for (size_t i = 0; i < rangeMax; i++) {
        rht.insert(i, i * 2);
      }
      for (size_t i = 0; i < rangeMax; i++) {
        if (i % 4 == 0) {
          rht.remove(i);
        }
      }
      for (size_t i = 0; i < rangeMax; i++) {
        rht.lookup(i);
      }
    });
  return 0;
}
