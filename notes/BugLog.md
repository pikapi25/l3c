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