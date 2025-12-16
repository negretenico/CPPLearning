#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include<iostream>
#include<map>
#include<mutex>
#include<optional>
#include <ostream>
#include <string>

#include <sstream>
#include<thread>
namespace ThreadSafe {
	template <typename Key, typename Value>
	class Cache
	{
	private:
		std::map<Key, Value> cache;
		std::thread eviction_thread;
		std::string file_name;
		std::mutex cache_mutex;
		std::atomic_flag is_running{  };
		std::condition_variable eviction_signal;
		void log() {
			std::ofstream ofs(file_name);
			if (!ofs) {
				std::cerr << "Error opening file for writing." << std::endl;
				return;
			}

			for (const auto& pair : cache) {
				ofs << pair.first << ":" << pair.second << "\n";
			}

			ofs.close();
			std::cout << "Map serialized to " << file_name << std::endl;
		};
		void read() {
			std::ifstream ifs(file_name);
			if (!ifs) {
				std::cerr << "Error opening file for reading." << std::endl;
				return;
			}
			std::string line;
			while (std::getline(ifs, line)) {
				size_t delimiter_pos = line.find(':');
				if (delimiter_pos != std::string::npos) {
					std::stringstream ss_key(line.substr(0, delimiter_pos));
					std::stringstream ss_val(line.substr(delimiter_pos + 1));
					Key key;
					Value value;
					ss_key >> key;
					ss_val >> value;
					cache[key] = value;
				}
			}
			ifs.close();
			std::cout << "Map deserialized from " << file_name << std::endl;
		}
		//Eviciton uses the "smallest" element for now later iteration will provide custom. 
		// "smallest" is defined by the users
		void eviction() {
			while (is_running.test()) {
				std::unique_lock<std::mutex> lock(cache_mutex);
				eviction_signal.wait_for(lock, std::chrono::seconds(10));
				auto it = cache.begin();
				//need to guard incase the map is empty
				if (it == cache.end()) {
					std::cout << "Empty Map" << std::endl;
					continue;
				}
				cache.erase(it->first);
			}
		};

	public:
		Cache(std::string file_name) : file_name(file_name) {
			read();
			// initaliing with {true} is not standard so we use test and set in the consturctor
			is_running.test_and_set();
			eviction_thread = std::thread(&Cache::eviction, this);
		};
		~Cache() {
			// we need to say not to run anymore and notify the thread runnings the evictions
			// that its time to stop 
			is_running.clear();
			eviction_signal.notify_one();
			if (!eviction_thread.joinable()) { return; }
			eviction_thread.join();
			log();

		};
		std::optional<Value> get(const Key& key) {
			std::lock_guard<std::mutex> lock(cache_mutex);
			auto it = cache.find(key);
			return (it != cache.end()) ? std::optional<Value>(it->second) : std::nullopt;
		};
		void put(const Key& key, const Value& value) {
			std::lock_guard<std::mutex> lock(cache_mutex);
			cache.insert_or_assign(key, value);
		};

	};
}