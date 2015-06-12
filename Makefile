all:
	clang++ -std=c++11 -O3 RobinhoodHashTableExample.cpp -o RobinhoodHashTableExample
	clang++ -std=c++11 -O3 BinaryHeapExample.cpp -o BinaryHeapExample

debug:
	clang++ -std=c++11 -g RobinhoodHashTableExample.cpp -o RobinhoodHashTableExample
	clang++ -std=c++11 -g BinaryHeapExample.cpp -o BinaryHeapExample
