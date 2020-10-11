//
// Created by Vova on 11.10.2020.
//

#include "conritional_variable_test.h"
#include <condition_variable>

namespace single_worker_job
{

	std::vector<double> thread_input_data;
	std::vector<double> thread_result;

	std::mutex cv_mutex;
	std::condition_variable cv;

	bool input_data_sent = false;
	bool data_processed = false;

	static void thread_worker ()
	{
		// Aquire CV:
		std::unique_lock<std::mutex> lck(cv_mutex);
		std::cout << "[worker]: Went through mutex" << std::endl;
		cv.wait(lck, [] () { return input_data_sent; });

		// Do work:
		thread_result = thread_input_data;
		for (size_t i = 1; i < thread_input_data.size(); ++i) {
			thread_result[i] = thread_input_data[i] + thread_result[i - 1];
		}
		data_processed = true;

		// Release CV:
		lck.unlock();
		cv.notify_one();

		std::cout << "[Worker]: killing self" << std::endl;
	}

	void test_conditional_variable ()
	{
		std::thread worker(thread_worker);

		thread_input_data = { 1, 2, 3, 4, 5, 6, 7 };
		{
			std::lock_guard<std::mutex> lck(cv_mutex);
			input_data_sent = true;
			std::cout << "[main]: Sent data" << std::endl;
		}

		cv.notify_one();

		{
			// Wait:
			std::unique_lock<std::mutex> lck(cv_mutex);
			cv.wait(lck, [] { return data_processed; });
		}
		worker.join();

		std::cout << "Again in main, data = " << thread_result << std::endl;

	}
}