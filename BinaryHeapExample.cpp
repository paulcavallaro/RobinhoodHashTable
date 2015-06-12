#include "BinaryHeap.h"

#include <cstdio>
#include <string>

int main(int argc, char** argv) {
  BinaryHeap<std::string> binaryHeap(10);
  binaryHeap.insert("ten", 10);
  binaryHeap.insert("fifteen", 15);
  binaryHeap.insert("three", 3);
  binaryHeap.insert("seven", 7);
  binaryHeap.insert("twenty", 20);
  while (!binaryHeap.isEmpty()) {
    printf("%s\n", binaryHeap.findMin().c_str());
    binaryHeap.popMin();
  }
}

