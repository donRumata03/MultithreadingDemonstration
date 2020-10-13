#include <iostream>

#include "mutex_test.h"
#include "thread_test.h"

#include "conditional_variable_test_single_thread.h"
#include "condition_variable_test_multiple_worker_threads.h"

int main ()
{
	SetConsoleOutputCP(CP_UTF8);
	system("chcp 65001");

	// test_std_thread();
	// mutex_with_vector::test_vector_filling_with_mutex();

	// single_worker_job::test_conditional_variable ();
	// minimal_conditional_variable::main_thread();
	// always_working_conditional_variable::main_thread();
	// always_notifying_conditional_variable::main_thread();

	// single_run_many_threads_condition_variable::main_thread();
	multiple_runs_many_threads_condition_variable::main_thread();


	return 0;
}
