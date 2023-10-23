#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "filesys.h"
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


/* ------------------------ Checkpoint 2 tests --------------------------------*/
void wait_for_b(){
	char str[128];
	printt("please enter b to go back\n");
	readt(str);
	while (strncmp(str, "b", 1)!=0){
		printt("command not found, try again!\n");
		readt(str);
	}
}

/* RTC Test
 * 
 * Test and visualize if the rtc_write can correctly set the frequency
 * Inputs: None
 * Outputs: PASS
 * Side Effects: Print the visualization of rtc frequency
 * Files: rtc.c/.h
*/
int rtc_write_test(){
	int freq, rate, count;
	//printt("rtc test start!\n");
	for (freq = 2, rate = 0; freq <= 1024; freq <<= 1, rate++) {
		rtc_write(0, &freq, 4);
		for (count = 0; count < freq; count++) {
			rtc_read(0, NULL, 0);
			printt("*");
		}
		printt("\n");
	}
	return PASS;
}

/* Terminal Test
 * 
 * Try to repeatedly read from and write to terminal
 * Inputs: none
 * Output: PASS
 * Side Effects: None
 * Files: terminal.c/h, keyboard.c/h
*/
int terminal_test(){
	TEST_HEADER;

	int32_t t_read, t_write;
	int32_t read_nbytes, write_nbytes;
	uint8_t buf[128];
	char str[128];
	read_nbytes = 128;
	write_nbytes = 128;
	t_read = terminal_read(0, buf, read_nbytes);
	t_write = terminal_write(0, buf, write_nbytes);
	printt("please enter b to go back\n");
	readt(str);
	while (strncmp(str, "b", 1)!=0){
		printt("command not found, try again!\n");
		readt(str);
	}
	return PASS;
}

/* File System Test -- read small file
*  
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_Read_Small(){
	TEST_HEADER;
	dentry_t test;
	char filename[] = "frame0.txt";
	printf("READ SMALL FILE TEST");
	printf(" \n");
	if(read_dentry_by_name((uint8_t*)filename,&test) == -1){
		printf("A non-existent file!");
		return FAIL;
	}else{
		printf("The file name is %s.\n",test.filename);
	}

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILENAME_LEN)!=0){
		return FAIL;
	}
	
	return PASS;
	
}

/* File System Test -- read executable file
*  
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_Read_Exe(){
	TEST_HEADER;
	dentry_t test;
	char filename[] = "grep";
	printf("READ EXECUTABLE FILE TEST");
	printf(" \n");
	if(read_dentry_by_name((uint8_t*)filename,&test) == -1){
		printf("A non-existent file!");
		return FAIL;
	}else{
		printf("The file name is %s.\n",test.filename);
	}

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILENAME_LEN)!=0){
		return FAIL;
	}
	
	return PASS;
}

/* File System Test -- read large file
*  
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_Read_Large(){
	TEST_HEADER;
	dentry_t test;
	char filename[] = "verylargetextwithverylongname.txt";
	printf("READ LARGE FILE TEST");
	printf(" \n");
	if(read_dentry_by_name((uint8_t*)filename,&test) == -1){
		printf("A non-existent file!");
		return FAIL;
	}else{
		printf("The file name is %s.\n",test.filename);
	}

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILENAME_LEN)!=0){
		return FAIL;
	}
	
	return PASS;
}

/* File System Test -- read file data
*  
*  Inputs: uint8_t* filename -- the file to be read
*  Outputs: None
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_Read_Data(uint8_t* filename){
	TEST_HEADER;
	dentry_t test;
	uint8_t buf[1000];
	int i;
	int32_t bytes_read;
	read_dentry_by_name(filename,&test);
	printf(test.filename);
	printf("\n");
	bytes_read = read_data(test.inode_num,0,buf,1000);
	printf("bytes_read:%d\n",bytes_read);
	for(i=0; i<bytes_read; i++)
		putc(buf[i]);
	return PASS;
}


/* File System Test -- Open
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_Open(){
	TEST_HEADER;

	char file_Nonexistent[] = "lalala";	
	if (open_file((const uint8_t*)file_Nonexistent) == -1 && open_dir((const uint8_t*)file_Nonexistent) == 0)return PASS;
	return FAIL;
}

/* File System Test -- close
*  try to close fd and return PASS if fd != 0 or 1 
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_close(){
	TEST_HEADER;

	int32_t fd_default = 0;
	int32_t fd_normal = 3;
	if (close_file(fd_default) == -1 && close_file(fd_normal) == 0 && close_file(fd_normal) == -1 && close_dir(fd_normal) == 0)return PASS;
	return FAIL;
}

/* File System Test -- write
*  try to write into file or directory and return PASS if write_file is denied
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_Write(){
	TEST_HEADER;

	int32_t buf[20] = {1};
	if (write_file(0,buf,32) == -1 && write_dir(0,buf,32) == -1)return PASS;
	return FAIL;
}

/* File System Test -- read of directory
*  If read of directory is succeeded, it will display file_name on the screen
*  Inputs: None
*  Outputs: Showing directory name list
*  Side Effects: None
*  Files: filesystem_module.c 
*/
int File_System_Test_Dir_Read(){
	TEST_HEADER;
	int i;
	uint8_t* buf[FILENAME_LEN];

	for(i =0 ; i< FILES_NUM_MAX; i++){
		if (read_dir_index(0,buf,i) == -1)break;
		printf((int8_t*)buf);
		printf("\n");
	}
	return PASS;
}

void ckpt2_print_message(){
	clear_redraw();
	printt("CHECKPOINT 2 TEST!!!\n");
	printt("please select one test:\n");
	printt("0: rtc_driver_test\n");
	printt("q: quit\n");
}

void ckpt2_test(){
	char str[128];
	int test_result;
	// printt("Press enter to start testing.");
	// readt(str);	
	ckpt2_print_message();
	readt(str);	
	while (strncmp(str, "q", 1)!=0){
		if (strncmp(str, "0", 1)==0){
			printt("\n");
			clear_redraw();
			test_result = rtc_write_test();
			wait_for_b();
			//test_result = terminal_test();
		}else{
			printt("no such command!\n");
		}
		ckpt2_print_message();
		readt(str);	
	}
	
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

void sleep(uint32_t ticks) {
	int32_t i;
	for (i = 0; i < ticks; i++) {
		rtc_read(0, NULL, 0);
	}
}

/* Test suite entry point */
void launch_tests(){
	/*CHECKPOINT1 TESTS*/
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("de_test", de_test());
	// TEST_OUTPUT("paging test1", paging_test1());
	// TEST_OUTPUT("paging test2", paging_test2());
	// TEST_OUTPUT("paging test3", paging_test3());
	// TEST_OUTPUT("paging test4", paging_test4());
	// TEST_OUTPUT("rtc_driver_test", rtc_write_test());

	/*CHECKPOINT2 TESTS*/
	// clear_redraw();
	// TEST_OUTPUT("terminal_test", terminal_test());
	//clear_redraw();
	// TEST_OUTPUT("File_System_Test_Read_Small", File_System_Test_Read_Small());
	// sleep(5);
	// clear_redraw();
	// TEST_OUTPUT("File_System_Test_Read_Large", File_System_Test_Read_Large());
	// sleep(5);
	// clear_redraw();
	// TEST_OUTPUT("File_System_Test_Read_Exe", File_System_Test_Read_Exe());
	// sleep(5);
	// clear_redraw();
	// TEST_OUTPUT("File_System_Test_Read_Data", File_System_Test_Read_Data((uint8_t*)"frame0.txt"));
	ckpt2_test();
	//rtc_write_test();
	printt("Checkpoint 2 Test Finished! Well Done! \n");
}
