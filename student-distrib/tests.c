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

/**
 * sleep
 * 
 * Description: Helper function for tests. Pauses the screen for some time.
 * Inputs:  ticks -- The number of ticks to sleep.
 * Return value: none
 * Side effects: none
 */
void sleep(uint32_t ticks) {
	int32_t i;
	for (i = 0; i < ticks; i++) {
		rtc_read(0, NULL, 0);
	}
}

/**
 * my_test_output
 * 
 * Description: Helper function for tests. Print the results of tests.
 * Inputs: func -- The test function
 * Return value: none
 * Side effects: none
 */
void my_test_output(int func){
	if(func){
		printt("\n[TEST] Result = PASS\n");
	}else{
		printt("\n[TEST] Result = FAIL\n");
	}
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
	printt("[TEST: RTC_WRITE_TEST]\n");
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
	int32_t t_read, t_write;
	int32_t read_nbytes, write_nbytes;
	uint8_t buf[128];
	memset(buf, 0, 128);
	read_nbytes = 128;
	write_nbytes = 128;
	printt("type here!\n");
	t_read = terminal_read(0, buf, read_nbytes);
	t_write = terminal_write(0, buf, write_nbytes);
	printf("terminal read returns: %d\n", t_read);
	printf("terminal write returns: %d\n", t_write);
	return PASS;
}


/* File System Test -- helper function for printing data of the file
*  
*  Inputs: uint8_t* filename -- the file to be read
*		   uint32_t offset -- The starting offset within the file to begin reading
*		   uint32_t length -- The number of bytes to read from the file
*  Outputs: None
*  Side Effects: None
*  Files: filesys.c 
*/
void Filesys_Test_Read_Data(uint8_t* filename, uint32_t offset, uint32_t length){
	dentry_t test;
	uint8_t buf[10000];
	int i;
	int32_t bytes_read;
	read_dentry_by_name(filename,&test);
	//printf(" \n");
	bytes_read = read_data(test.inode_num,offset,buf,length);
	//printf(" Bytes read:%d\n",bytes_read);
	for(i=0; i<bytes_read; i++)
		user_terminal_putc(buf[i],1);
	return;
}


/* File System Test -- read small file
*  
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesys.c 
*/
int Filesys_Test_Read_Small(){
	//TEST_HEADER;
	printt("[TEST: Filesys_Test_Read_Small]\n");
	dentry_t test;
	int i=0;
	char filename[] = "frame0.txt";
	if(read_dentry_by_name((uint8_t*)filename,&test) == -1){
		printt("A non-existent file!\n");
		return FAIL;
	}

	/*print and test file name*/
	printt("[TEST] Reading file: ");
	for(i=0; i<FILENAME_LEN; i++){
		user_terminal_putc((uint8_t)test.filename[i],1);
	}
	printt(" \n");
	printt(" \n");

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILENAME_LEN)!=0){
		return FAIL;
	}

	/*print file data*/
	Filesys_Test_Read_Data((uint8_t*)filename,0,1000);
	printt(" \n");
	
	return PASS;
	
}

/* File System Test -- read executable file
*  
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesys.c 
*/
int Filesys_Test_Read_Exe(){
	//TEST_HEADER;
	printt("[TEST: Filesys_Test_Read_Exe]\n");
	dentry_t test;
	int i;
	char filename[] = "grep";
	if(read_dentry_by_name((uint8_t*)filename,&test) == -1){
		printt("A non-existent file!\n");
		return FAIL;
	}

	/*print and test file name*/
	printt("[TEST] Reading file: ");
	for(i=0; i<FILENAME_LEN; i++){
		user_terminal_putc((uint8_t)test.filename[i],1);
	}
	printt(" \n");
	printt(" \n");

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILENAME_LEN)!=0){
		return FAIL;
	}

	/*print file data*/
	printt("[TEST] Reading the beginning:\n");
	Filesys_Test_Read_Data((uint8_t*)filename,0,1000);
	printt("\n\n[TEST] Reading the end:\n");
	Filesys_Test_Read_Data((uint8_t*)filename,1900,5000);
	// Filesys_Test_Read_Data((uint8_t*)filename,0,10000);

	printt(" \n");
	
	return PASS;
}


/* File System Test -- read large file
*  
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesys.c 
*/
int Filesys_Test_Read_Large(){
	//TEST_HEADER;
	printt("[TEST: Filesys_Test_Read_Large]\n");
	dentry_t test;
	int i;
	char filename[] = "verylargetextwithverylongname.txt";
	if(read_dentry_by_name((uint8_t*)filename,&test) == -1){
		printt("A non-existent file!\n");
		return FAIL;
	}

	/*print and test file name*/
	printt("[TEST] Reading file: ");
	for(i=0; i<FILENAME_LEN; i++){
		user_terminal_putc((uint8_t)test.filename[i], 1);
	}
	printt(" \n");

	if(strncmp((int8_t*)test.filename,(int8_t*)filename,FILENAME_LEN)!=0){
		return FAIL;
	}

	/*read some parts of the file*/
	printt("(Reading some parts of file because the file is too large to be shown on screen)\n\n");
	Filesys_Test_Read_Data((uint8_t*)filename,0,1250);

	/*read the whole file, but the screen doesn't look good*/
	// Filesys_Test_Read_Data((uint8_t*)filename,0,15000);

	printt(" \n");
	
	return PASS;
}


/* File System Test -- Open
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesys.c 
*/
int File_System_Test_Open(){

	int32_t result1, result2;
	printt("[TEST: File_System_Test_Open]\n");
	printt("try to open nonexistent file/directory lalala\n");
	char file_Nonexistent[] = "lalala";
	result1 = open_file((const uint8_t*)file_Nonexistent);
	result2 =  open_dir((const uint8_t*)file_Nonexistent);
	printt("open_file with file_Nonexistent should return -1\n");
	if(result1 == -1){
		printt("actually returns -1.\n");
	}else{
		printt("Error!\n");
		return FAIL;
	}

	// (as piazza says do nothing and return 0)
	printt("open_dir with dir_Nonexistent should return 0\n");
	if(result2 == 0){
		printt("actually returns 0.\n");
	}else{
		printt("Error!\n");
		return FAIL;
	}
	
	return PASS;
}

/* File System Test -- close
*  try to close fd and return PASS if fd != 0 or 1 
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesys.c 
*/
int File_System_Test_close(){

	int32_t fd_default = 0;
	int32_t fd_normal = 3;
	int32_t result1, result2;
	printt("[TEST: File_System_Test_Close]\n");
	result1 = close_file(fd_default);
	result2 = close_file(fd_normal);
	printt("close_file function try to close fd = 0 should return -1;\n");
	if (result1 == -1){
		printt("actually returns -1\n");
	}else{
		printt("Error!\n");
		return FAIL;
	}
	printt("close_file function try to close fd = 3 should return 0;\n");
	if (result2 == 0){
		printt("actually returns 0\n");
	}else{
		printt("Error!\n");
		return FAIL;
	}
	return PASS;
}

/* File System Test -- write
*  try to write into file or directory and return PASS if write_file is denied
*  Inputs: None
*  Outputs: PASS on success and FAIL on Failure
*  Side Effects: None
*  Files: filesys.c 
*/
int File_System_Test_Write(){

	int32_t buf[20] = {1};
	int32_t result1, result2;
	printt("[TEST: File_System_Test_Write]\n");
	result1 = write_file(0,buf,32);
	result2 = write_dir(0,buf,32);
	printt("since it's read-only, both write functions shall return -1 \n");
	if (result1 == -1){
		printt("write file returns -1\n");
	}
	else{
		printt("write file Error!\n");
		return FAIL;
	}
	if (result2 == -1){
		printt("write directory returns -1\n");
	}
	else{
		printt("write directory Error!\n");
		return FAIL;
	}
	return PASS;
}

/* File System Test -- read of directory
*  If read of directory is succeeded, it will display file_name on the screen
*  Inputs: None
*  Outputs: Showing directory name list
*  Side Effects: None
*  Files: filesys.c 
*/
int File_System_Test_Dir_Read(){
	dentry_t dentry;
	char buf[32] = {"\0"};

	int x,i;
	printt("[TEST: File_System_Test_Dir_Read]\n");
	for (i=0;i<FILES_NUM_MAX;i++){
		if (read_dentry_by_index(i,&dentry)==-1){break;}
		// printf("filename:%s  \n",dentry.filename);
		memset(buf, 0, 32);
		strcpy(buf,dentry.filename);
		for (x=0;x<32;x++){
			user_terminal_putc(buf[x], 1);
		}
		printt("\n");
	}
	return PASS;
}


void ckpt2_print_message(){
	clear_redraw();
	printt("CHECKPOINT 2 TEST!!!\n");
	printt("please select one test:\n");
	printt("0: Terminal Test\n");
	printt("1: Filesys Test Read Small\n");
	printt("2: Filesys Test Read Large\n");
	printt("3: Filesys Test Read Exe\n");
	printt("4: Filesys Test Open\n");
	printt("5: Filesys Test Close\n");
	printt("6: Filesys Test Write\n");
	printt("7: Filesys Test Read Directory\n");
	printt("q: quit\n");
}

void ckpt2_test(){
	char str[128];
	int test_result;
	// printt("Press enter to start testing.");
	// readt(str);	
	ckpt2_print_message();
	printt("Test: ");
	readt(str);	
	while (strncmp(str, "q", 1)!=0){
		if (strncmp(str, "0", 1)==0){
			printt("\n");
			clear_redraw();
			test_result = terminal_test();
			wait_for_b();
			//test_result = terminal_test();
		}else if(strncmp(str, "1", 1)==0){
			clear_redraw();
			my_test_output(Filesys_Test_Read_Small());
			wait_for_b();
		}else if(strncmp(str, "2", 1)==0){
			clear_redraw();
			my_test_output(Filesys_Test_Read_Large());
			wait_for_b();
		}else if(strncmp(str, "3", 1)==0){
			clear_redraw();
			my_test_output(Filesys_Test_Read_Exe());
			wait_for_b();
		}else if(strncmp(str, "4", 1)==0){
			clear_redraw();
			my_test_output(File_System_Test_Open());
			wait_for_b();
		}else if(strncmp(str, "5", 1)==0){
			clear_redraw();
			my_test_output(File_System_Test_close());
			wait_for_b();
		}else if(strncmp(str, "6", 1)==0){
			clear_redraw();
			my_test_output(File_System_Test_Write());
			wait_for_b();
		}else if(strncmp(str, "7", 1)==0){
			clear_redraw();
			my_test_output(File_System_Test_Dir_Read());
			wait_for_b();
		}
		ckpt2_print_message();
		printt("Test: ");
		readt(str);	
	}
	
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */



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
	// TEST_OUTPUT("File_System_Test_Open",File_System_Test_Open());
	// TEST_OUTPUT("File_System_Test_Write",File_System_Test_Write());
	// TEST_OUTPUT("File_System_Test_close",File_System_Test_close());
	// clear_redraw();
	// TEST_OUTPUT("File_System_Test_Dir_Read",File_System_Test_Dir_Read());
	// clear_redraw();
	// my_test_output(Filesys_Test_Read_Small());
	// sleep(6);
	// clear_redraw();
	// my_test_output(Filesys_Test_Read_Large());
	// sleep(6);
	// clear_redraw();
	// my_test_output(Filesys_Test_Read_Exe());
	// sleep(6);
	// clear_redraw();
	clear_redraw();
	rtc_write_test();
	ckpt2_test();
	printt("Checkpoint 2 Test Finished! Well Done! \n");

}
