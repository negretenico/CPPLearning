// ThreadPool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "T_Pool.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace ThreadPool;
void print_thread_id(int num)
{
	std::cout << "Task " << num << " is running in thread ";
	std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

int main()
{
	T_Pool pool;
	pool.schedule([]() {print_thread_id(1);});
	pool.schedule([]() {print_thread_id(2);});
	pool.schedule([]() {print_thread_id(3);});
	pool.schedule([]() {print_thread_id(4);});
	pool.schedule([]() {print_thread_id(5);});
	pool.schedule([]() {print_thread_id(6);});
	pool.schedule([]() {print_thread_id(7);});
	pool.schedule([]() {print_thread_id(8);});
	std::cin.get();
}