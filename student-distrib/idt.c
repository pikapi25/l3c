#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "interrupt_linkage.h"
#include "keyboard.h"
#include "rtc.h"

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
    SET_IDT_ENTRY(idt[0x0], exception_handler_de);
    SET_IDT_ENTRY(idt[0x1], exception_handler_db);
    SET_IDT_ENTRY(idt[0x2], exception_handler_nmi);
    SET_IDT_ENTRY(idt[0x3], exception_handler_bp);
    SET_IDT_ENTRY(idt[0x4], exception_handler_of);
    SET_IDT_ENTRY(idt[0x5], exception_handler_br);
    SET_IDT_ENTRY(idt[0x6], exception_handler_ud);
    SET_IDT_ENTRY(idt[0x7], exception_handler_nm);
    SET_IDT_ENTRY(idt[0x8], exception_handler_df);
    SET_IDT_ENTRY(idt[0x9], exception_handler_cso);
    SET_IDT_ENTRY(idt[0xA], exception_handler_ts);
    SET_IDT_ENTRY(idt[0xB], exception_handler_np);
    SET_IDT_ENTRY(idt[0xC], exception_handler_ss);
    SET_IDT_ENTRY(idt[0xD], exception_handler_gp);
    SET_IDT_ENTRY(idt[0xE], exception_handler_pf);
    SET_IDT_ENTRY(idt[0xF], exception_handler_reserved);
    SET_IDT_ENTRY(idt[0x10], exception_handler_mf);
    SET_IDT_ENTRY(idt[0x11], exception_handler_ac);
    SET_IDT_ENTRY(idt[0x12], exception_handler_mc);
    SET_IDT_ENTRY(idt[0x13], exception_handler_xm);

    //set IDT entries for interrupts
    SET_IDT_ENTRY(idt[KEYBOARD_VEC], keyboard_handler_linkage);
    SET_IDT_ENTRY(idt[RTC_VEC], rtc_handler_linkage);

    //set IDT entries for system call
    SET_IDT_ENTRY(idt[SYSTEM_CALL_VEC], system_call_handler);

    /*load IDT*/
    lidt(idt_desc_ptr);
}

void exception_handler_de()
{
    printf("An exception occurs! IDT vector number 0x0: %s\n",exception_names[0x0]);
    while(1){}
}

void exception_handler_db()
{
    printf("An exception occurs! IDT vector number 0x1: %s\n",exception_names[0x1]);
    while(1){}
}

void exception_handler_nmi()
{
    printf("An exception occurs! IDT vector number 0x2: %s\n",exception_names[0x2]);
    while(1){}
}

void exception_handler_bp()
{
    printf("An exception occurs! IDT vector number 0x3: %s\n",exception_names[0x3]);
    while(1){}
}

void exception_handler_of()
{
    printf("An exception occurs! IDT vector number 0x4: %s\n",exception_names[0x4]);
    while(1){}
}

void exception_handler_br()
{
    printf("An exception occurs! IDT vector number 0x5: %s\n",exception_names[0x5]);
    while(1){}
}

void exception_handler_ud()
{
    printf("An exception occurs! IDT vector number 0x6: %s\n",exception_names[0x6]);
    while(1){}
}

void exception_handler_nm()
{
    printf("An exception occurs! IDT vector number 0x7: %s\n",exception_names[0x7]);
    while(1){}
}

void exception_handler_df()
{
    printf("An exception occurs! IDT vector number 0x8: %s\n",exception_names[0x8]);
    while(1){}
}

void exception_handler_cso()
{
    printf("An exception occurs! IDT vector number 0x9: %s\n",exception_names[0x9]);
    while(1){}
}

void exception_handler_ts()
{
    printf("An exception occurs! IDT vector number 0xA: %s\n",exception_names[0xA]);
    while(1){}
}

void exception_handler_np()
{
    printf("An exception occurs! IDT vector number 0xB: %s\n",exception_names[0xB]);
    while(1){}
}

void exception_handler_ss()
{
    printf("An exception occurs! IDT vector number 0xC: %s\n",exception_names[0xC]);
    while(1){}
}

void exception_handler_gp()
{
    printf("An exception occurs! IDT vector number 0xD: %s\n",exception_names[0xD]);
    while(1){}
}

void exception_handler_pf()
{
    printf("An exception occurs! IDT vector number 0xE: %s\n",exception_names[0xE]);
    while(1){}
}

void exception_handler_reserved()
{
    printf("An exception occurs! IDT vector number 0xF: %s\n",exception_names[0xF]);
    while(1){}
}

void exception_handler_mf()
{
    printf("An exception occurs! IDT vector number 0x10: %s\n",exception_names[0x10]);
    while(1){ }
}

void exception_handler_ac()
{
    printf("An exception occurs! IDT vector number 0x11: %s\n",exception_names[0x11]);
    while(1){}
}

void exception_handler_mc()
{
    printf("An exception occurs! IDT vector number 0x12: %s\n",exception_names[0x12]);
    while(1){}
}

void exception_handler_xm()
{
    printf("An exception occurs! IDT vector number 0x13: %s\n",exception_names[0x13]);
    while(1){}
}

void system_call_handler()
{
    printf("A system call was called.");
}
