#pragma once
#include <array>
#include <atomic>
#include <iostream>
#include <optional>
namespace ConcurrentDataStructures {
	template<typename Value>
	class CircularBuffer {
	private:
		std::atomic< size_t> head{ 0 };
		std::atomic < size_t> tail{ 0 };
		std::size_t capacity = 5;
		std::array<Value, 5> buffer; // std arrays don't need to be delted bc they maintain their own memory
	public:
		void push(Value value) {
			size_t old = tail.load();
			size_t nnew = (old + 1) % capacity;
			if (nnew == head.load()) {
				return;
			}
			while (!tail.compare_exchange_strong(old, nnew, std::memory_order_release, std::memory_order_relaxed)) {
				nnew = (old + 1) % capacity;
				if (nnew == head.load()) {
					return;
				}
			};
			buffer[old] = value;
			std::atomic_thread_fence(std::memory_order_release);

		};
		std::optional<Value> pop() {
			size_t old = head.load();
			if (old == tail.load()) {
				return std::nullopt;
			}
			size_t nnew = (old + 1) % capacity;
			while (!head.compare_exchange_strong(old, nnew, std::memory_order_acquire, std::memory_order_relaxed)) {
				nnew = (old + 1) % capacity;
				if (old == tail.load()) {
					return std::nullopt;
				}
				return buffer[old];
			};
		};
		std::optional<Value> peek() {
			size_t old_h = head.load(std::memory_order_acquire);
			size_t old_t = tail.load(std::memory_order_acquire);
			return old_h == old_t ? std::nullopt : buffer[old_h]; //best guess
		};
		size_t size() {
			if (tail.load() >= head.load()) {
				return tail.load() - head.load();
			}
			return capacity - (head.load() - tail.load());
		};
	};
}