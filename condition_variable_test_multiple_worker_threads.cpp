//
// Created by Vova on 12.10.2020.
//

#include "condition_variable_test_multiple_worker_threads.h"

namespace single_run_many_threads_condition_variable
{
	std::mutex print_mutex;

	void safe_print(const std::string& str) {
		std::lock_guard<std::mutex> g(print_mutex);
		std::cout << str << std::endl;
	}

	std::mutex cv_protector;
	bool threads_should_ge_run = false;
	std::condition_variable cv;

	void thread_function(size_t thread_id) {
		std::unique_lock<std::mutex> lck(cv_protector);
		cv.wait(lck, [](){ return threads_should_ge_run; });

		// Run:
		std::cout << "thread " << thread_id << " acts!" << std::endl;
	}

	void launch_all_threads() {
		std::lock_guard<std::mutex> lck(cv_protector);
		threads_should_ge_run = true;
		cv.notify_all();
	}

	void main_thread ()
	{
		size_t thread_number = 10;
		std::vector<std::thread> threads; threads.reserve(thread_number);
		for (size_t thread_index = 0; thread_index < thread_number; ++thread_index) {
			threads.emplace_back(thread_function, thread_index);
		}

		getchar();
		launch_all_threads();

		for (auto& thread : threads) thread.join();
	}
}
