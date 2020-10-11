//
// Created by Vova on 11.10.2020.
//

#include "thread_test.h"


static void threadFunction(int &a)
{
	a++;
}

void test_std_thread ()
{
	auto worker = [](int& value){
		value++;
	};

	int v1 = 0;
	// Doesn't compile:
//	std::thread thread1 (threadFunction, v1);
//	thread1.join();
	std::cout << v1 << std::endl;

	int v2 = 0;
	std::thread thread2 (threadFunction, std::ref(v2));
	thread2.join();
	std::cout << v2 << std::endl;

}
