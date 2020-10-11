//
// Created by Vova on 11.10.2020.
//

#include "mutex_test.h"

namespace mutex_with_vector
{

	std::mutex vector_protector;
	std::mutex print_mutex;

	static void
	vector_thread_worker (std::vector<std::string> &to_append, const std::string &string_to_append, size_t index)
	{
		{
			std::lock_guard<std::mutex> lck(vector_protector);
			for (size_t i = 0; i < 100; ++i) {
				to_append.push_back(string_to_append);
			}

			{
				std::lock_guard<std::mutex> lck_print(print_mutex);
				std::cout << "Thread #" << index << " (" << std::this_thread::get_id() << ") finished!" << std::endl;
			}
		}
	}

	void test_vector_filling_with_mutex ()
	{
		std::vector<std::string> test_vector;
		std::string to_fill = "Hello, world!";

		std::vector<std::thread> threads;
		for (size_t thread_index = 0; thread_index < 12; ++thread_index) {
			{
				std::lock_guard<std::mutex> lck_print(print_mutex);
				std::cout << "Launching thread " << thread_index << std::endl;
			}
			threads.emplace_back(vector_thread_worker, std::ref(test_vector), std::cref(to_fill), thread_index);
		}
		{
			std::lock_guard<std::mutex> lck_print(print_mutex);
			std::cout << "Thread work distributed" << std::endl;
		}

		for (auto &thread : threads) thread.join();

		{
			std::lock_guard<std::mutex> lck_print(print_mutex);
			std::cout << "Finished!" << std::endl;
		}

		if ((test_vector.size() == 12 * 100)) {
			std::cout << console_colors::green << "GOOD SIZE" << console_colors::remove_all_colors << std::endl;
			if (std::all_of(test_vector.begin(), test_vector.end(), [&] (auto &val) { return val == to_fill; })) {
				std::cout << console_colors::green << "GOOD CONTENTS" << console_colors::remove_all_colors << std::endl;
			} else {
				std::cout << console_colors::red << "BAD CONTENTS" << console_colors::remove_all_colors << std::endl;
			}
		} else {
			std::cout << console_colors::red << "BAD SIZE (" << test_vector.size() << ")"
			          << console_colors::remove_all_colors << std::endl;
		}
	}
}