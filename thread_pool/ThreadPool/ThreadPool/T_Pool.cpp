#include "T_Pool.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>
#define LOG(x) std::cout << x << std::endl;
namespace ThreadPool {
	static unsigned int current_index = 1;
	T_Pool::T_Pool() {
		LOG("Thread Pool created");
		working[0] = std::move(std::thread(&T_Pool::work, this));
		working[0].detach(); // Detach the initial worker thread
	}
	T_Pool::~T_Pool() {
		// Join all working threads before destruction
		LOG("Destroying Thread Pool, joining all working threads");
		for (std::thread& t : working) {
			if (t.joinable()) {
				t.join();
			}
		}
	}
	void T_Pool::drain() {
		LOG("Draining Thread Pool, waiting for all tasks to complete");
		// Wait for all working threads to finish
		for (std::thread& t : working) {
			if (t.joinable()) {
				t.join();
			}
		}
	}
	void T_Pool::work() {
		while (true) {
			std::cout << "Worker thread waiting for tasks..." << std::endl;
			Callable task;
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				work_signal.wait(lock, [this]() { return !waiting.empty(); });
				task = std::move(waiting.front());
				waiting.pop();
			}
			LOG("Worker thread picked up a task from waiting queue");
			task(); // Execute the task
			LOG("Task completed by worker thread");
			// After completing the task, decrease the current index to free up a thread slot
			current_index--;
		}
	}

	int T_Pool::numWaitingTasks() {
		std::lock_guard<std::mutex> lock(queue_mutex);
		return waiting.size();
	}

	int T_Pool::numWorkingTasks() {
		return current_index;
	}


	void T_Pool::schedule(Callable task) {
		LOG("Scheduling task to working queue");
		// Check if we have any available threads if not push to waiting queue
		if (current_index >= SIZE) {
			std::lock_guard<std::mutex> lock(queue_mutex);
			waiting.push(task);
			work_signal.notify_one();
			LOG("All threads are busy, task added to waiting queue");
			return;
		}
		// Assign task to the next available thread
		working[current_index] = std::move(std::thread(task));
		working[current_index].detach(); // Detach the thread to allow it to run independently
		current_index++;
	}
};