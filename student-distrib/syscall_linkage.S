#define ASM     1
#include "syscall_linkage.h"

.globl syscall_linkage
syscall_linkage:
    addl $-4, %esp      # push dummy
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx

    #check whether call number is valid
    cmpl $1, %eax
    jl invalid_call
    cmpl $10, %eax
    jg invalid_call

    #call valid system call
    movl %eax, %esi
    addl $-1, %esi      #jumptable starts from index 0
    #push parameters
    pushl %edx
    pushl %ecx
    pushl %ebx
    call *syscall_jumptable( ,%esi, 4);     #long is 4 bytes
    addl $12, %esp      #pop 3 parameters
    jmp done   

invalid_call:
    movl $-1, %eax      #return -1

done:
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    addl $4, %esp
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp           # pop dummy
    iret


syscall_jumptable:
    .long   halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

