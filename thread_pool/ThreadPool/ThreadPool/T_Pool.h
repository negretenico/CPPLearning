#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
namespace ThreadPool {
	typedef std::function<void()> Callable;
	constexpr int SIZE = 5;
	class T_Pool
	{
	public:
		T_Pool();
		~T_Pool();
		void schedule(Callable task);
		void work();
		void drain();
		int numWaitingTasks();
		int numWorkingTasks();

	private:
		std::queue<Callable> waiting;
		std::mutex queue_mutex;
		std::condition_variable work_signal;
		std::thread working[SIZE] = { std::thread(),std::thread(),std::thread(),std::thread(),std::thread() };
	};
}