#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

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

// Some Exception Tests
/* de_test
 * Test if div 0 exception can be triggered
 * Inputs: None
 * return: FAIL
 * Side Effects: None
 * Files: idt.c/h
 */
int de_test(){
	TEST_HEADER;

	int a = 0;
	int b = 1 / a;
	b++;	

	return FAIL;
}

/* ss_test
 * Test if stack-segmentf fault exception can be triggered
 * Inputs: None
 * return: FAIL
 * Side Effects: None
 * Files: idt.c/h
 */
int ss_test(){
	TEST_HEADER;

	int a[1];
	a[3]++;

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

int rtc_init_test( void )
{
	TEST_HEADER;
	int i;
	for (i = 0; i < (1000000000); i++) {}

	rtc_init();

	for (i = 0; i < 1000000000; i++) {}

	disable_irq(8);

	clear();

	return PASS;
}

/* ENABLE_IRQ INVALID TEST */
/* Tests whether passing in a negative IRQ number and an IRQ   */
/* number greater than 15 when trying to enable that specific  */
/* IRQ number will cause the PIC to break   		           */
/* Inputs: None. 											   */
/* Outputs: None. 											   */
/* Side Effects: If the test fails, nothing should show on 	   */
/*				 on the screen when typing					   */
/* Coverage: Enable IRQ for the PIC							   */
int enable_irq_inval_test( void )
{
	TEST_HEADER;

	enable_irq(-1);
	enable_irq(100);

	return PASS;
}

/* DISABLE_IRQ INVALID TEST */
/* Tests whether passing in a negative IRQ number and an IRQ   */
/* number greater than 15 when trying to disable that specific */
/* IRQ number will cause the PIC to break   		           */
/* Inputs: None. 											   */
/* Outputs: None. 											   */
/* Side Effects: If the test fails, nothing should show on 	   */
/*				 on the screen when typing					   */
/* Coverage: Disable IRQ for the PIC					       */
int disable_irq_inval_test( void )
{
	TEST_HEADER;

	disable_irq(-1);
	disable_irq(100);

	return PASS;
}

/* SEND_EOI INVALID TEST */
/* Tests whether passing in a negative IRQ number and an IRQ   */
/* number greater than 15 when trying to send an 			   */
/* end-of-interrupt signal for that specific IRQ number will   */
/* cause the PIC to break   		          				   */ 
/* Inputs: None. 											   */
/* Outputs: None. 											   */
/* Side Effects: If the test fails, nothing should show on 	   */
/*				 on the screen when typing					   */
/* Coverage: Send EOI for the PIC						       */
int send_eoi_inval_test( void )
{
	TEST_HEADER;

	send_eoi(-1);
	send_eoi(100);

	return PASS;
}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("de_test", de_test());
	// TEST_OUTPUT("ss_test", ss_test());
	// TEST_OUTPUT("paging test1", paging_test1());
	// TEST_OUTPUT("paging test2", paging_test2());
	// TEST_OUTPUT("paging test3", paging_test3());
	// TEST_OUTPUT("paging test4", paging_test4());
	//TEST_OUTPUT("rtc_init_test", rtc_init_test( ));
	// TEST_OUTPUT("enable_irq_inval_test", enable_irq_inval_test( ));
	// printf("\n");
	// TEST_OUTPUT("disable_irq_inval_test", disable_irq_inval_test( ));
	// printf("\n");
	// TEST_OUTPUT("send_eoi_inval_test", send_eoi_inval_test( ));
	// printf("\n");
}

