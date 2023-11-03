# Bug Log for MP3

## CHECKPOINT 1

--- PROBLEM 1 ---

Problem Description: (For paging) Cannot pass the tests of dereferencing locations that shouldn't be accessible.

Solution: Adding $ before numbers in the assembly code in paging.c.

Time to fix: two hours

--- PROBLEM 2 ---

Problem Description: Screen is automatically reseted in 5 secs .

Solution: Change the assembly code in paging.c used to change register cr0,cr4，cr3. Adjust the order of updating crs (from old cr3->cr0->cr4 to new cr3->cr4->cr0)

Time to fix: two hours

--- Problem 3 ---

Problem Description: RTC test_interrupts doesn't work. It seems like the rtc handler isn't working.

Solution: 
>   - First, we checked the PIC, and found that in ``send_eoi``, the EOI is sent to the **data port** instead of **command port**.
>   - However after correcting the PIC, the RTC still can't work. We guess it's because the ``rtc_init``   
is affected by other interrupts, so we add a critical section in ``rtc_init``. And now it work!

Time to fix: one and a half hours

## CHECKPOINT 2

--- problem 1 ---

Problem Description: When keyboard reads more than 128 characters(buffer overflow), must enter 3 'ENTER's to continue typing.

Solution:  
>   - It turns out to be, the buffer is only cleaned in terminal_read, so when no body is reading, the buffer can't be cleaned automatically.  
>   - Therefore, we add an ``if`` in keyboard dealing with '\n'. If no one is reading, clean the buffer when read '\n'; If terminal is reading, just put '\n' into buffer and wait for terminal to clean the buffer.

--- problem 2 ---  
Problem Description: Always print a smile emoji when trying to print very long filename

Cause:   
>   - The null character is not in the right place. Seems like the buffer holding the string has a size of 33, but position 32 is not set to null.  

Solution: 
>   - Using ``putc`` to print out buffer content instead of ``printf``    

Time: 1 h

--- problem 3 ---  
Problem Description:   
>   - Cursor appears below the characters.  

Solution:   
>   - The arguments in ``enable_cursor`` are wrong.    

Time: 1h 20min.  

--- problem 4 ---  
Problem Description:   
>   - Test for reading name of "verylargetextwithverylongname.txt" returned FAIL. 

Solution:   
>   - Changing the sanity check for read_dentry_by_name.   

Time: 30 min.  

--- problem 5 ---  
Problem Description:   
>   - Tried to print the data in the files by calling read_data, but nothing was printed.

Solution:   
>   - filesys_init didn't find the correct address for data blocks. Corrected that.

Time: 1 h.  

## CHECKPOINT 2

--- problem 1 ---
Problem Description:   
>   - Failed to call read_data function in execute system call. Showing memory at address 0x8fffffc cannot be accessed. 

Solution:   
>   - It seems that there is lacking steps changing page table used to be set but not used into present (present bit = 1). 

Time: 2h

--- problem 2 ---
Problem Description:   
>   - Nothing is showing up after calling execute(shell) and page fault is raised.

Solution:   
>   - Forget to set stdin and stdout. These two functions are written in terminal.c but not used. 

Time: 2h

--- problem 3 ---
Problem Description:   
>   - execute system call is failed with page fault. It seems that "IET" is not correctly functioning.

Solution:   
>   - Not resolved yet.

Time: 4h
