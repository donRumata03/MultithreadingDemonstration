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

	std::atomic<size_t> total_runs = 0;

	void thread_function(size_t thread_id) {
		std::unique_lock<std::mutex> lck(cv_protector);
		cv.wait(lck, [](){ return threads_should_ge_run; });

		total_runs++;

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

		std::cout << total_runs << std::endl;
		if(total_runs == thread_number) {
			std::cout << console_colors::green << "GOOD NUMBER OF RUNS" << console_colors::remove_all_colors << std::endl;
		}
		else {
			std::cout << console_colors::red << "BAD NUMBER OF RUNS" << console_colors::remove_all_colors << std::endl;
		}

	}
}

namespace multiple_runs_many_threads_condition_variable
{
	std::mutex print_mutex;

	void safe_print(const std::string& str) {
		std::lock_guard<std::mutex> g(print_mutex);
		std::cout << str << std::endl;
	}

	signed long long get_thread_id() {
		std::stringstream thread_acceptor;
		thread_acceptor << std::this_thread::get_id();

		signed long long res;
		thread_acceptor >> res;

		return res;
	}

	std::mutex running_cv_protector;
	bool threads_should_be_run = false;
	std::condition_variable running_cv;

	std::vector<bool> threads_ready; // Updated by threads after ending,
							// read by main thread to set true to all_threads_ready,
							// is reset by main thread after ending
	std::atomic<lint> max_all_threads_ready_run = -1; // All the threads have finished work for all the indexes <= max_all_threads_ready_run
							// Set by main thread,
							// All the threads are waiting for all the others to be ready.


	void thread_function(size_t my_index, size_t run_number) {
		std::string thread_prefix = "[worker thread "s + std::to_string(my_index) + " (" + std::to_string(get_thread_id()) + ")]: ";
		safe_print(thread_prefix + "Starting");

		for (size_t run_index = 0; run_index < run_number; run_index++) {
			/// Passively Wait to start (don`t consume CPU cycles for checking):
			safe_print(thread_prefix + "Waiting for job...");
			std::unique_lock<std::mutex> lck(running_cv_protector);
			running_cv.wait(lck, [](){ return threads_should_be_run; });

			lck.unlock();

			safe_print(thread_prefix + "I'm released");

			/// Do work:
			safe_print(thread_prefix + "working (" + std::to_string(run_index) + ")...");
			std::this_thread::sleep_for(1s);
			safe_print(thread_prefix + "work done (" + std::to_string(run_index) + ")");


			// Say that I`m ready:
			threads_ready[my_index] = true;
			safe_print(thread_prefix + "max_all_threads_ready_run is " + std::to_string(max_all_threads_ready_run) + ", run_index is " + std::to_string(run_index) + ", " + std::to_string(max_all_threads_ready_run > run_index));
			while (max_all_threads_ready_run < lint(run_index)) {
				/// Wait all threads
			}
		}

		safe_print(thread_prefix + "Finishing");
	}


	void main_thread() {
		safe_print("[main thread]: Starting");

		size_t thread_number = 3;
		size_t runs_planned = 5;
		std::vector<std::thread> threads; threads.reserve(thread_number);
		for (size_t thread_index = 0; thread_index < thread_number; ++thread_index) {
			threads.emplace_back(thread_function, thread_index, runs_planned);
		}

		threads_ready.assign(thread_number, false);

//		auto signal_ending_state = [&](bool value){
//			std::lock_guard<std::mutex> locker(ending_cv_protector);
//			end_or_not = value;
//			ending_state_ready = true;
//			ending_cv.notify_one();
//		};

		for (size_t run_index = 0; run_index < runs_planned; ++run_index) {
			safe_print("_____________________________________\n[main thread]: Begin Cycle "s + std::to_string(run_index));
			safe_print("[main thread]: working...");
			std::this_thread::sleep_for(0.5s);
			safe_print("[main thread]: done work");

			/// Notifying:
			{
				std::lock_guard<std::mutex> locker(running_cv_protector);
				threads_should_be_run = true;
				running_cv.notify_all();
			}

			/// ACTIVE Waiting for all threads to complete:
			safe_print("[main thread]: launched other threads, waiting for them");
			while(std::any_of(threads_ready.begin(), threads_ready.end(), [](bool val){ return !val; })) { // TODO!
				// Wait
			}

			/// All threads ready => reset everything to the initial state:
			safe_print("[main thread]: all threads ready => resetting variables");
			std::fill(threads_ready.begin(), threads_ready.end(), false); // Clear thread ready
			threads_should_be_run = false;

			/// Release other threads
			max_all_threads_ready_run++;
			safe_print("[main thread]: all threads released");
		}

		for (auto& thread : threads) thread.join();

		safe_print("[main thread]: Finished");
	}

}