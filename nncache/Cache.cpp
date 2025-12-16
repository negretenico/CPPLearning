#include "Cache.h"
#include <chrono>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
namespace ThreadSafe {
	class TestCache {
	public:
		static void runTests() {
			Cache<int, std::string> cache("log.txt");
			cache.put(1, "one");
			cache.put(2, "two");
			cache.put(3, "three");
			auto val1 = cache.get(1);
			if (val1) {
				std::cout << "Key 1: " << *val1 << std::endl;
			}
			else {
				std::cout << "Key 1 not found" << std::endl;
			}
			auto val2 = cache.get(4);
			if (val2) {
				std::cout << "Key 4: " << *val2 << std::endl;
			}
			else {
				std::cout << "Key 4 not found" << std::endl;
			}
			// Wait for eviction to occur
			std::this_thread::sleep_for(std::chrono::seconds(15));
			auto val3 = cache.get(1);
			if (val3) {
				std::cout << "Key 1: " << *val3 << std::endl;
			}
			else {
				std::cout << "Key 1 not found (likely evicted)" << std::endl;
			}
		}
	};
}
void main() {
	std::cout << "Running Cache Tests..." << std::endl;
	ThreadSafe::TestCache::runTests();
	std::cout << "Cache Tests Completed." << std::endl;
	std::cin.get();
}