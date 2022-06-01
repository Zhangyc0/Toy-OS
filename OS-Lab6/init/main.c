#include "put.h"
#include "sched.h"
#include "slub.h"
int start_kernel()
{
	puts("hello-riscv");
	// test_page_fault();
	// test_section_mod();
	slub_init();
	task_init();
	dead_loop();

	return 0;
}
