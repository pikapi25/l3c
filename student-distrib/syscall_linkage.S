#define ASM     1
#include "syscall_linkage.h"

.globl syscall_linkage
syscall_linkage:
    #callee-saved regs
    pushl %ebx
    pushl %esi
    pushl %edi
    pushfl

    #check whether call number is valid
    cmpl $1, %eax
    jl invalid_call
    cmpl $10, %eax
    jg invalid_call

    #call valid system call
    movl %eax, %esi
    addl $-1, %esi      #jumptable starts from index 0
    #push parameters
    pushl %ebx
    pushl %ecx
    pushl %edx
    call *syscall_jumptable( , 4, %esi)     #long is 4 bytes
    addl $12, %esp      #pop 3 parameters
    jmp done   

invalid_call:
    movl $-1, %eax      #return -1

done:
    #restore regs
    popfl
    popl %edi
    popl %esi
    popl %ebx

    iret


syscall_jumptable:
    .long   halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
