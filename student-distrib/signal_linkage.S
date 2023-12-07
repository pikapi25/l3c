#define ASM 1

.globl DO_SIG_RETURN
.globl DO_SIG_RETURN_END

DO_SIG_RETURN:
    movl    $10, %eax
    int     $0x80
DO_SIG_RETURN_END:

