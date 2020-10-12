//
// Created by Vova on 11.10.2020.
//

#include "conditional_variable_test_single_thread.h"
#include <condition_variable>


namespace minimal_conditional_variable
{
	std::string data;

	std::mutex protector;
	std::condition_variable cv;
	bool state_flag = false;

	std::mutex print_mutex;

	void thread_function() {
		{
			std::lock_guard<std::mutex> g(print_mutex);
			std::cout << "[worker thread]: In worker thread" << std::endl;
		}

		std::unique_lock<std::mutex> locker (protector);
		cv.wait(locker, [](){ return state_flag; });

		locker.unlock();

		std::cout << "[worker thread]: string is: " << data << std::endl;


	}

	void main_thread() {
		{
			std::lock_guard<std::mutex> g(print_mutex);
			std::cout << "[main thread]: Starting test!" << std::endl;
		}
		std::thread thread(thread_function);

		// Setting data:
		{
			std::lock_guard<std::mutex> locker(protector);

			data = "test string";
			{
				std::lock_guard<std::mutex> g(print_mutex);
				std::cout << "[main thread]: set data" << std::endl;
			}
			state_flag = true;
			cv.notify_one();
			{
				std::lock_guard<std::mutex> g(print_mutex);
				std::cout << "[main thread]: notified" << std::endl;
			}
		}

		thread.join();
		std::cout << "[main thread]: Finished" << std::endl;
	}

}


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

		std::this_thread::sleep_for(1s);

		{
			std::lock_guard<std::mutex> lck(cv_mutex);

			thread_input_data = { 1, 2, 3, 4, 5, 6, 7 };

			input_data_sent = true;

			cv.notify_one();

			std::cout << "[main]: Sent data" << std::endl;
		}


		{
			// Wait:
			std::unique_lock<std::mutex> lck(cv_mutex);
			cv.wait(lck, [] { return data_processed; });
		}
		worker.join();

		std::cout << "Again in main, data = " << thread_result << std::endl;

	}
}

namespace always_notifying_conditional_variable
{
	std::mutex print_mutex;

	void safe_print(const std::string& str) {
		std::lock_guard<std::mutex> g(print_mutex);
		std::cout << str << std::endl;
	}


	std::mutex ending_cv_protector;
	std::condition_variable ending_cv;
	bool ending_state_ready = false;
	bool end_or_not = false;

	std::mutex worker_thread_ready_cv_protector;
	std::condition_variable worker_thread_ready_cv;
	bool worker_thread_ready = false;

	size_t counted_runs = 0;

	bool get_ending_state_for_thread() {
		bool exit_or_not;
		{
			std::unique_lock<std::mutex> locker(ending_cv_protector);
			ending_cv.wait(locker, [] () { return ending_state_ready; });
			ending_state_ready = false;
			exit_or_not = end_or_not;
		}
		return exit_or_not;
	}


	void thread_function()
	{
		safe_print("[worker thread]: In worker thread");

		while (true) {
			safe_print("[worker thread]: getting ending state");
			bool exit_or_not = get_ending_state_for_thread();
			safe_print("[worker thread]: got ending state: "s + std::to_string(exit_or_not));
			if (exit_or_not) break;

			counted_runs++;
			std::this_thread::sleep_for(1s);

			// Signal being ready:
			{
				std::lock_guard<std::mutex> locker(worker_thread_ready_cv_protector);
				worker_thread_ready = true;
				worker_thread_ready_cv.notify_one();
			}
		}

		safe_print("[worker thread]: exiting");
	}


	void main_thread()
	{
		safe_print("[main thread]: Starting");
		std::thread worker_thread(thread_function);

		std::this_thread::sleep_for(0.2s);

		auto signal_ending_state = [&] (bool value) {
			std::lock_guard<std::mutex> locker(ending_cv_protector);
			end_or_not = value;
			ending_state_ready = true;
			ending_cv.notify_one();
		};

		size_t runs = 5;
		for (size_t run_index = 0; run_index < runs; ++run_index) {
			signal_ending_state(false);
			safe_print("_____________________________________\n[main thread]: Begin Cycle "s + std::to_string(run_index));

			// Wait worker thread to be ready:
			std::unique_lock<std::mutex> lck(worker_thread_ready_cv_protector);
			worker_thread_ready_cv.wait(lck, []() { return worker_thread_ready; });
			worker_thread_ready = false;
			lck.unlock();
		}
		signal_ending_state(true);

		worker_thread.join();
		safe_print("[main thread]: Finished");
		if (counted_runs == runs) {
			std::cout << console_colors::green << "GOOD COUNTED RUNS: " << counted_runs << console_colors::remove_all_colors << std::endl;
		}
		else {
			std::cout << console_colors::red << "BAD COUNTED RUNS: " << counted_runs << console_colors::remove_all_colors << std::endl;
		}
	}
}


namespace always_working_conditional_variable
{
	std::vector<double> thread_input_data;
	std::vector<double> thread_result;


	std::mutex data_transmission_cv_protector;
	std::condition_variable data_transmission_cv;

	bool input_data_sent = false;
	bool data_processed = false;

	std::mutex ending_cv_protector;
	std::condition_variable ending_cv;
	bool ending_state_ready = false;
	bool end_or_not = false;


	std::mutex print_mutex;

	void safe_print(const std::string& str) {
		// std::lock_guard<std::mutex> g(print_mutex);
		// std::cout << str << std::endl;
	}

	bool get_ending_state_for_thread() {
		bool exit_or_not;
		{
			std::unique_lock<std::mutex> locker(ending_cv_protector);
			ending_cv.wait(locker, [] () { return ending_state_ready; });
			ending_state_ready = false;
			exit_or_not = end_or_not;
		}
		return exit_or_not;
	}

	void thread_do_work () {
		thread_result = thread_input_data;
		for (size_t i = 1; i < thread_input_data.size(); ++i) {
			thread_result[i] = thread_input_data[i] + thread_result[i - 1];
		}
		// std::this_thread::sleep_for(2s);
	}

	void thread_function() {
		safe_print("[worker thread]: In worker thread");

		while (true)
		{
			safe_print("[worker thread]: getting ending state");
			bool exit_or_not = get_ending_state_for_thread();
			safe_print("[worker thread]: got ending state: "s + std::to_string(exit_or_not));
			if (exit_or_not) break;

			/// Wait for data to be sent:
			std::unique_lock<std::mutex> wait_locker(data_transmission_cv_protector);
			data_transmission_cv.wait(wait_locker, [] () { return input_data_sent; });
			input_data_sent = false;

			wait_locker.unlock();

			/// Process data:
			safe_print("[worker thread]: processing data");
			thread_do_work();
			safe_print("[worker thread]: processed data");

			/// Notify, that data is processed:
			{
				std::lock_guard<std::mutex> locker(data_transmission_cv_protector);
				data_processed = true;
				data_transmission_cv.notify_one();
			}
		}

		safe_print("[worker thread]: exiting");
	}

	void main_thread() {
		safe_print("[main thread]: Starting");
		std::thread worker_thread(thread_function);

		std::this_thread::sleep_for(0.2s);

		auto signal_ending_state = [&](bool value){
			std::lock_guard<std::mutex> locker(ending_cv_protector);
			end_or_not = value;
			ending_state_ready = true;
			ending_cv.notify_one();
		};

		size_t runs = 5;
		std::vector<std::vector<double>> res;
		for (size_t i = 0; i < runs; ++i) {
			signal_ending_state (false);
			safe_print("_____________________________________\n[main thread]: Begin Cycle "s + std::to_string(i));

			/// Send data:
			{
				std::lock_guard<std::mutex> locker(data_transmission_cv_protector);

				thread_input_data = { double(randint(3, 10)), 2, 3, 4, 5, 6, 7, 8, 9, 10 };

				input_data_sent = true;
				data_transmission_cv.notify_one();
			}

			/// Wait until the data is processed:
			safe_print("[main thread]: waiting for data to be ready");
			std::unique_lock<std::mutex> locker(data_transmission_cv_protector);
			data_transmission_cv.wait(locker, [] () { return data_processed; });
			data_processed = false;

			locker.unlock();

			safe_print("[main thread]: result is ready!");
			res.push_back(thread_result);
		}
		signal_ending_state(true);


		worker_thread.join();
		safe_print("[main thread]: Finished");
		std::cout << "Results are: " << std::endl;
		std::cout << res << std::endl;
		if (res.size() == runs) {
			std::cout << console_colors::green << "GOOD SIZE" << console_colors::remove_all_colors << std::endl;
		}
		else {
			std::cout << console_colors::red << "BAD SIZE" << console_colors::remove_all_colors << std::endl;
		}
	}

}



