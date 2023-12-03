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

## CHECKPOINT 3

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

## CHECKPOINT 4

--- problem 1 ---   
Problem Description:   
>   - pingpong stucks at second line.  
>   - After testing I found ``terminal_read`` and ``rtc_read`` can't execute at the same time.  
>   - After ``terminal_read`` is called, ``rtc_read`` will stuck.

Solution:   
>   - It seems that it is  ``rtc_handler``'s virtualization has something wrong. changed ``if (rtc_ticks  == rtc_virtual_rate)`` to  
>   - ``if (rtc_ticks  >= rtc_virtual_rate)`` and it works.

Time: 3h  

--- problem 2 ---   
Problem Description:   
>   - fish and cat doesn't work.  
>   - with gdb I found it is ``file_read``'s return value doesn't match ``cat``

Solution:   
>   - When ``read_data`` returns negative value, ``file_read`` should return 0  
>   - Otherwise it should return exactly what ``read_data`` returns.

Time: 30min  

--- problem 3 ---  
Problem Description:  
>   - Page fault occurs when memcpy function is called in execute function.  

Solution:   
>   - The size for ages in pcb should be 128+1 instead of 128.

Time: 30min  


## CHECKPOINT 5

--- problem 1 ---   
Problem Description:   
>   - Exception occurs when starting the vm.

Solution:   
>   - Set dpl for pit in idt.c.

Time: 3h

--- problem 2 ---   
Problem Description:   
>   - terminal_init(), scheduler_initialize() wasn't added in kernel.c 
    which contributes to error in cur_terminal_id and myScheduler.task.

Solution:   
>   - Add initialization functions in kernel.c.

Time: 1h

--- problem 3 ---   
Problem Description:   
>   - What we type occurs in terminals randomly.

Solution:   
>   - Add video remapping in clear and user_terminal_putc functions.

Time: 1h

--- problem 4 ---   
Problem Description:   
>   - When we switch to other terminal, the terminals become empty.

Solution:   
>   - Let pit be initialized after all initialization finish.

Time: 30min

--- problem 5 ---   
Problem Description:   
>   - We can't clear terminal.

Solution:   
>   - Add clear() in terminal_init().

Time: 15min

--- problem 6 ---   
Problem Description:   
>   - Starting sentences can't be printed in the first terminal when we
    start the system. However, the position of the cursor was correct. T
    This was because the contents that should be printed were in the backup
    buffer.

Solution:   
>   - Add a if condition in scheduler:
    If current_terminal_id is equal to next_terminal_id, map to video memory.
    Otherwise, map to background buffer. 

Time: 1h30min