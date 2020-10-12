//
// Created by Vova on 11.10.2020.
//

#pragma once

#include <pythonic.h>

namespace single_worker_job
{
	void test_conditional_variable ();
}

namespace minimal_conditional_variable {
	void main_thread();
}

namespace always_working_conditional_variable {
	void main_thread();
}

namespace always_notifying_conditional_variable
{
	void main_thread();
}

