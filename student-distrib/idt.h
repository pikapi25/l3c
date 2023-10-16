#ifndef _IDT_H
#define _IDT_H


//vectors for system call
#define SYSTEM_CALL_VEC         0x80

//vectors for interrupts
#define KEYBOARD_VEC             0x21
#define RTC_VEC                  0x28

//number of exceptions
#define NUM_EXCP            20

//local functions
<<<<<<< HEAD
void idt_init();
=======
extern void idt_init();
>>>>>>> 15acf65b3aebf1db565b9dc89d2c009b097e3880
void exception_handler_de();
void exception_handler_db();
void exception_handler_nmi();
void exception_handler_bp();
void exception_handler_of();
void exception_handler_br();
void exception_handler_ud();
void exception_handler_nm();
void exception_handler_df();
void exception_handler_cso();
void exception_handler_ts();
void exception_handler_np();
void exception_handler_ss();
void exception_handler_gp();
void exception_handler_pf();
void exception_handler_reserved();
void exception_handler_mf();
void exception_handler_ac();
void exception_handler_mc();
void exception_handler_xm();
void system_call_handler();

#endif

