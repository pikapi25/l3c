#include "idt.h"

static char* exception_names[] = {
    "Division Error", "Debug", "Non-maskable Interrupt", "Breakpoint",
    "Overflow", "Bound Rnage Exceeded", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
    "Stack-Segment Fault", "General Protection Fault", "Page Fault", "Reserved",
    "x87 Floating-Point Exception", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception"
};

void idt_init()
{
    /*set up IDT struct*/
    //initialize idt struct for exceptions
    int i;
    for(i=0; i<NUM_VEC; i++){    
        idt[i].seg_selector = KERNEL_CS;         
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;       //need to change for interrupts
        idt[i].reserved2 = 1; 
        idt[i].reserved1 = 1;
        idt[i].size = 1;          //32 bits
        idt[i].reserved0 = 0;   
        idt[i].dpl = 0;         //need to change for system calls
        if(i<NUM_EXCP){
            idt[i].present = 1;
        }
    }

    //initialize idt struct for interrupts
    idt[KEYBOARD_VEC].reserved3=1;
    idt[KEYBOARD_VEC].present=1;
    idt[RTC_VEC].reserved3=1;
    idt[RTC_VEC].present=1;

    //initialize idt struct for system call
    idt[SYSTEM_CALL_VEC].dpl=3;
    idt[SYSTEM_CALL_VEC].present=1;

    /*set IDT entries*/
    //set IDT entries for exceptions from 0x0 to 0x13
    SET_IDT_ENTRY(idt[0x0], exception_handler(0x0));
    SET_IDT_ENTRY(idt[0x1], exception_handler(0x1));
    SET_IDT_ENTRY(idt[0x2], exception_handler(0x2));
    SET_IDT_ENTRY(idt[0x3], exception_handler(0x3));
    SET_IDT_ENTRY(idt[0x4], exception_handler(0x4));
    SET_IDT_ENTRY(idt[0x5], exception_handler(0x5));
    SET_IDT_ENTRY(idt[0x6], exception_handler(0x6));
    SET_IDT_ENTRY(idt[0x7], exception_handler(0x7));
    SET_IDT_ENTRY(idt[0x8], exception_handler(0x8));
    SET_IDT_ENTRY(idt[0x9], exception_handler(0x9));
    SET_IDT_ENTRY(idt[0xA], exception_handler(0xA));
    SET_IDT_ENTRY(idt[0xB], exception_handler(0xB));
    SET_IDT_ENTRY(idt[0xC], exception_handler(0xC));
    SET_IDT_ENTRY(idt[0xD], exception_handler(0xD));
    SET_IDT_ENTRY(idt[0xE], exception_handler(0xE));
    SET_IDT_ENTRY(idt[0xF], exception_handler(0xF));
    SET_IDT_ENTRY(idt[0x10], exception_handler(0x10));
    SET_IDT_ENTRY(idt[0x11], exception_handler(0x11));
    SET_IDT_ENTRY(idt[0x12], exception_handler(0x12));
    SET_IDT_ENTRY(idt[0x13], exception_handler(0x13));

    //set IDT entries for interrupts
    SET_IDT_ENTRY(idt[KEYBOARD_VEC], keyboard_handler_linkage);
    SET_IDT_ENTRY(idt[RTC_VEC], rtc_handler_linkage);

    //set IDT entries for system call
    SET_IDT_ENTRY(idt[SYSTEM_CALL_VEC], system_call_handler);

    /*load IDT*/
    lidt(idt_desc_ptr);
}


void exception_handler(uint32_t vec_nr)
{
    printf("An exception occurs! IDT vector number %x: %s\n", vec_nr, exception_names[vec_nr]);
    while(1){}
}

void system_call_handler()
{
    printf("A system call was called.");
}