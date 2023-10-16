# Bug Log for MP3

## CHECKPOINT 1

--- PROBLEM 1 ---

Problem Description: Multiple definition error in lib.o during compiling.

Solution:

--- PROBLEM 2 ---

Problem Description: Screen is automatically reseted in 5 secs .

Solution: Change the assembly code in paging.c used to change register cr0,cr4，cr3. Adjust the order of updating crs (from old cr3->cr0->cr4 to new cr3->cr4->cr0)


