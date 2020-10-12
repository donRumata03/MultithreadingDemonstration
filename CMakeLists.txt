cmake_minimum_required(VERSION 3.16)
project(MultithreadingDemo)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
message(${CMAKE_MODULE_PATH})
find_package(Json)
set(Pythonic_DIR D:/pythonic)
find_package(Pythonic)


add_executable(MultithreadingDemo
        main.cpp
        mutex_test.cpp
        mutex_test.h
        thread_test.h

        thread_test.cpp conditional_variable_test_single_thread.cpp conditional_variable_test_single_thread.h condition_variable_test_multiple_worker_threads.cpp condition_variable_test_multiple_worker_threads.h)

include_directories(${pythonic_include_directories})
target_link_libraries(MultithreadingDemo ${pythonic_static_library})

