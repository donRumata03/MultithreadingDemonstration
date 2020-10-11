#include <iostream>

#include "mutex_test.h"
#include "thread_test.h"

int main ()
{
	SetConsoleOutputCP(CP_UTF8);
	system("chcp 65001");

	// test_std_thread();
	test_vector_filling_with_mutex();

	return 0;
}
