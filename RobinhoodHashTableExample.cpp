#include "RobinhoodHashTable.h"
#include <string>

int main(int argc, char** argv) {
  RobinhoodHashTable<std::string, uint32_t> rht{4};
  rht.insert("hello", 1);
  rht.insert("world", 2);
  printf("%lu\n", rht.lookup_index("hello"));
  printf("%u\n", rht.m_vals[rht.lookup_index("hello")]);
  printf("%lu\n", rht.lookup_index("world"));
  printf("%u\n", rht.m_vals[rht.lookup_index("world")]);
  return 0;
}
