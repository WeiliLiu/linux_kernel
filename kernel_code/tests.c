#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "idt.h"
#include "keyboard_wrapper.h"
#include "rtc_wrapper.h"
#include "terminal.h"
#include "file.h"

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

/* test_right_address
 * Test the right addresses
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: None
 * Files: x86_desc.h/S
 */

int test_right_address() {
	TEST_HEADER;
	// Random correct values
	int result = FAIL;
	/*testing kernel addresses */
	int * ptr = (int*) 0x400000;
	int number = * ptr;
	ptr = (int*) 0x800000 - 4;
	number = *ptr;
	ptr = (int*) 0x666666;
	number = *ptr;
	ptr = (int*) 0x555555;
	number = *ptr;

	/* testing video addresses */
	ptr = (int*) 0xB8000;
	number = * ptr;
	ptr = (int*) 0xB8044;
	number = * ptr;
	ptr = (int*) 0xB8279;
	number = * ptr;

	result = PASS;

	return result;
}

/* test_wrong_address
 * Test the address
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: None
 * Files: x86_desc.h/S
 */

void test_wrong_address() {
	TEST_HEADER;
	/* testing  */
	// Random wrong values
	int * ptr = (int*) 0x400000;
	int number = * ptr;
	ptr = (int*) 0x00022;
	number = *ptr;
	ptr = (int*) 0xB9001;
	number = *ptr;
	ptr = (int*) 0x8000F9;
	number = *ptr;
}

/* test_null_pointer
 * Test page fault
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Create page fault
 * Files: None
 */

void test_null_pointer(){
	TEST_HEADER;
	int *a;
	a = NULL;
	int c = *a;
	printf("%x\n", c);
}

/* test_paging_structures
 * Test for paging structure
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Paging input appear
 * Files: idt.c/h
 */

void test_paging_structures() {
	clear();
	TEST_HEADER;
	int i = 0;
	for (i = 0; i < 10; i++) {
		printf("Page Directory %d is: %x\n",i,page_directory[i]);
	}//The interrupt modes
	printf("First Page Table %d is: %x\n",0xB8,first_page_table[0xB8]);
}

/* test_idt_values
 * Test IDT values
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Operate idt
 * Files: idt.c/h
 */

void test_idt_values(){
	TEST_HEADER;
	printf("IDT[0x20]: 0x%x%x\n", idt[0x21].offset_31_16, idt[0x21].offset_15_00);
	printf("IDT[0x28]: 0x%x%x\n", idt[0x28].offset_31_16, idt[0x28].offset_15_00);
	printf("keyboard_wrapper function is located at: 0x%x\n", &keyboard_wrapper);
	printf("rtc_wrapper function is located at: 0x%x\n", &rtc_wrapper);
}




/* test_garbage_values
 * Check garbage values
 * Inputs: None
 * Outputs: None
 * Side Effects: Kernel freezes
 * Coverage: Operate idt,loaddirectory function
 */
void test_garbage_values(){
	//check input for init_idt and loaddirectory functions
	TEST_HEADER;
	if(idt==NULL || page_directory==NULL){
		printf("This is garbage!\n");
		while(1){}
	}
}


/* test_divide_zero_exception
 * Test error
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Operate rtc
 * Files: None
 */
/*
void test_divide_zero_exception(){
	TEST_HEADER;
	int a = 1 / 0;
	printf("%x\n", a);
}*/



/* Checkpoint 2 tests */
/* test_terminal
 * Inputs: None
 * Outputs: None
 * Side Effects: Kernel gets page fault if buf size is too small
 * Coverage: terminal write and read
 */
void test_terminal(){
	int32_t cnt;
	uint8_t buf[1023];
	terminal_write(1, (uint8_t*)"Hi, what's your name? ", 22);
	cnt = terminal_read(0, buf, 1024-1);
	buf[cnt] = '\n';
	terminal_write(1, (uint8_t*)"Hello, ", 7);
	terminal_write(1, buf, cnt + 1);
}

/* test_dir
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: filesystem write
 */
int test_dir(){
	int32_t fd, cnt;
	uint8_t buf[33];

	fd = 0;

	while (0 != (cnt = dir_read (fd, buf, 33-1))) {
			if (-1 == cnt) {
				terminal_write (1, (uint8_t*)"directory entry read failed\n", 29);
				return 3;
		}
		buf[cnt] = '\n';
		fd ++;
		if (-1 == terminal_write(1, buf, cnt + 1))
				return 3;
	}

	return 0;
}

/* test_terminal_write_bad_input
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: terminal write
 */
int test_terminal_write_bad_input(){
	TEST_HEADER;
	int32_t cnt;
	int result = PASS;

	cnt = terminal_write(1, NULL, 11);

	if(cnt != -1){
		assertion_failure();
		result = FAIL;
	}

	return result;
}

/* test_terminal_read_bad_input
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: terminal read
 */
int test_rtc_read_bad_input(){
	TEST_HEADER;
	int32_t cnt;
	int result = PASS;

	cnt = rtc_read(0,NULL,1);

	if(cnt != 0){
		assertion_failure();
		result = FAIL;
	}

	return result;
}

/* test_rtc_write_bad_input
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: rtc write
 */
int test_rtc_write_bad_input(){
	TEST_HEADER;
	int32_t cnt;
	int result = PASS;

	cnt = rtc_write(1,NULL,1);

	if(cnt != -1){
		assertion_failure();
		result = FAIL;
	}

	return result;
}

/* test_file
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: file system
 */
void test_file(){
	int32_t fd, cnt;
	uint8_t buf[20000];
	fd = 10;
	cnt = file_read(fd, buf, 20000);
	terminal_write(1, buf, cnt);

	//terminal_write (1, (uint8_t*)"directory entry read failed\n", 29);

	cnt = file_read(fd, buf, 25);
	terminal_write(1, buf, cnt);

	//terminal_write (1, (uint8_t*)"directory entry read failed\n", 29);

	cnt = file_read(fd, buf, 25);
	terminal_write(1, buf, cnt);

	cnt = file_read(fd, buf, 20000);
	terminal_write(1, buf, cnt);

	cnt = file_read(fd, buf, 40);
	terminal_write(1, buf, cnt);
}

/* test_largr_file
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: file system
 */
void test_large_file(){
	int32_t fd, cnt;
	uint8_t buf[20000];
	fd = 11;
	cnt = file_read(fd, buf, 20000);
	terminal_write(1, buf, cnt);
}

void test_non_txt_file(){
	int32_t fd, cnt;
	uint8_t buf[20000];
	fd = 2;
	cnt = file_read(fd, buf, 20000);
	terminal_write(1, buf, cnt);
}


/* test_null_file
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: file system
 */
int test_null_file(){
	TEST_HEADER;
	int result = PASS;
	int32_t fd, cnt;
	uint8_t buf[20000];
	fd = 20;
	cnt = file_read(fd, buf, 20000);

	if(cnt != -1){
		assertion_failure();
		result = FAIL;
	}

	return result;
}


/* test_rtc
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 * Coverage: rtc read and write
 */
void test_rtc(){
  rtc_init();
	printf("Testing freq\n");
	//int freq = 16;
  int freq = 128;//adjust freq for printing
	//int freq = 256;
	//int freq = 512;
  rtc_write(1,&freq,1);
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* launch_test()
 * function that launches all the test cases
 * Inputs: None
 * Outputs: None
 * Side Effects: execute all test cases
 */
/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//Checkpoint 1 tests
	//TEST_OUTPUT("test_right_address", test_right_address());
	//test_wrong_address();
	//test_null_pointer();
	//test_paging_structures();
	//test_idt_values();
	//test_divide_zero_exception();
	//test_garbage_values();
	//Checkpoint 2 tests
	//test_terminal();
	//test_dir();
	//test_file();
	//test_large_file();
	//test_non_txt_file();
	//TEST_OUTPUT("test", test_null_file());
	//TEST_OUTPUT("test", test_terminal_write_bad_input());
	//TEST_OUTPUT("test", test_rtc_write_bad_input());
	//TEST_OUTPUT("test", test_rtc_read_bad_input());
	//test_rtc();
}
