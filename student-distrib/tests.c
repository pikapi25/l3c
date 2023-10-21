#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}


/* de_test
 * Test if div 0 exception can be raised
 * Inputs: None
 * return: FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: idt.c/h
 */
int de_test(){
	TEST_HEADER;

	int a = 0;
	int b = 1 / a;
	b++;	

	return FAIL;
}


/* Paging Test
 * 
 * Dereferencing different address ranges with paging turned on
 * And Values contained in your paging structures like set of P bit
 * Here we test accessible address like video memory and kernel memory
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Dereference address
 * Files: paging.c/.h
 */
int paging_test1(){
	TEST_HEADER;
	// here we define the virtual memory address
	// 0xb8000 is where video_memory starts
	// 0xb8FFF is where video_memory ends
	// 0x400000 is where kernel_memory starts
	// 0x7FFFFF is where kernel_memory ends
	char* video_memory = (char*)0xb8000;
	char* video_memory_end = (char*)0xb8FFF;
	char* kernel_memory = (char*)0x400000;
	char* kernel_memory_end = (char*)0x7FFFFF;
	char read_result;

	// here we try to read one byte by dereferencing virtual memory address into physical memory address
	// if nothing wrong with dereferencing, no error would raise and pass is returned.
	read_result = *(video_memory);
	read_result = *(video_memory_end);
	read_result = *(kernel_memory);
	read_result = *(kernel_memory_end);

	printf("pass!\n");
	return PASS;
	
}


/* Paging Test
 * 
 * Dereferencing different address ranges with paging turned on
 * here we test address in page not present (out of range)
 * it test memory outside kernel memory and user memory
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: Page Fault
 * Coverage: Dereference address
 * Files: paging.c/.h
 */
int paging_test2(){
	TEST_HEADER;
	char* pointer = (char*)0x800001;
	char read_result;
	read_result = *(pointer);

	// if page fault is raised, it would not return FAIL, means the test is PASSED
	return FAIL;
}

/* Paging Test
 * 
 * Dereferencing different address ranges with paging turned on
 * here we test address in page not present (out of range)
 * it test the gap in user memory but video memory
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: Page Fault
 * Coverage: Dereference address
 * Files: paging.c/.h
 */
int paging_test3(){
	TEST_HEADER;
	char* pointer = (char*)0xb9001;
	char read_result;
	read_result = *(pointer);

	// if page fault is raised, it would not return FAIL, means the test is PASSED
	return FAIL;
}


/* Paging Test
 * 
 * Dereferencing different address ranges with paging turned on
 * here we test invalid address NULL
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: Page Fault
 * Coverage: Dereference address
 * Files: paging.c/.h
 */
int paging_test4(){
	TEST_HEADER;
	char* pointer = NULL;
	char read_result;
	read_result = *(pointer);

	// if page fault is raised, it would not return FAIL, means the test is PASSED
	return FAIL;
}


/* Checkpoint 2 tests */

/* RTC Test
 * 
 * Test and visualize if the rtc_write can correctly set the frequency
 * Inputs: None
 * Outputs: PASS
 * Side Effects: Print the visualization of rtc frequency
 * Files: rtc.c/.h
*/
int rtc_write_test(){
	TEST_HEADER;
	int freq, rate, count;
	for (freq = 2, rate = 0; freq <= 1024; freq <<= 1, rate++) {
		rtc_write(0, &freq, 4);
		for (count = 0; count < freq; count++) {
			rtc_read(0, NULL, 0);
			printf("%d", rate);
		}
		printf("\n");
	}

	return PASS;
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("de_test", de_test());
	// TEST_OUTPUT("paging test1", paging_test1());
	// TEST_OUTPUT("paging test2", paging_test2());
	// TEST_OUTPUT("paging test3", paging_test3());
	// TEST_OUTPUT("paging test4", paging_test4());
	TEST_OUTPUT("rtc_driver_test", rtc_write_test());
}

