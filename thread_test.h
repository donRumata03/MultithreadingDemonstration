//
// Created by Vova on 11.10.2020.
//

#pragma once

#include <pythonic.h>

inline void threadFunction(int &a)
{
	a++;
}

inline void test_threading() {
//	auto worker = [](int& value){
//		value++;
//	};

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
