#include <iostream>

#include "mutex_test.h"
#include "thread_test.h"
#include "conritional_variable_test.h"

int main ()
{
	SetConsoleOutputCP(CP_UTF8);
	system("chcp 65001");

	// test_std_thread();
	// mutex_with_vector::test_vector_filling_with_mutex();

	// single_worker_job::test_conditional_variable ();
	// minimal_conditional_variable::main_thread();
	// always_working_conditional_variable::main_thread();
	always_notifying_conditional_variable::main_thread();

	return 0;
}
