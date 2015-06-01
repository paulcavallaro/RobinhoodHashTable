#include "RobinhoodHashTable.h"
#include <string>


size_t probe_distance(size_t desired_pos, size_t cur_pos, size_t cap) {
  return desired_pos <= cur_pos ? (cur_pos - desired_pos)
    : ((cur_pos + cap) - desired_pos);
}

int main(int argc, char** argv) {
  RobinhoodHashTable<std::string, uint32_t> rht{4};
  std::vector<std::string> words = {
    "hello", "world", "how", "is", "it", "going",
    "some", "more", "words", "should", "work"
  };
  for (uint32_t i = 0; i < words.size(); i++) {
    printf("Inserting '%s'\n", words[i].c_str());
    rht.insert(words[i], i);
  }
  const auto cap = rht.cap();
  printf("(cap): %lu (size): %lu\n", cap, rht.size());
  std::hash<std::string> hasher;
  for (auto& word : words) {
    const auto desired_pos = (hasher(word) >> 1) % cap;
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
  return 0;
}
