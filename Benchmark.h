#include <chrono>

template<class T>
void timeFunc(size_t numIters, T thunk) {
  using namespace std::chrono;
  auto start = high_resolution_clock::now();
  for (size_t i = 0; i < numIters; i++) {
    thunk();
  }
  auto end = high_resolution_clock::now();
  auto diff = end - start;
  auto ns = duration_cast<nanoseconds>(end - start).count();
  auto us = duration_cast<microseconds>(end - start).count();
  auto ms = duration_cast<milliseconds>(end - start).count();
  printf("num iters: %lu\ntotal: %lld ms, %lld us\n", numIters, ms, us);
  printf("per iter: %.2f ms, %.2f us, %.2f ns\n", (double) ms / numIters,
         (double) us / numIters, (double) ns / numIters);
}

