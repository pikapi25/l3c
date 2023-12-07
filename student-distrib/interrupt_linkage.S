#define ASM     1
#include "interrupt_linkage.h"

#define INTR_LINK(name, func)   \
    .globl name                ;\
    name:                      ;\
        addl  $-4, %esp        ;\
        pushl %fs              ;\
        pushl %es              ;\
        pushl %ds              ;\
        pushl %eax             ;\
        pushl %ebp             ;\
        pushl %edi             ;\
        pushl %esi             ;\
        pushl %edx             ;\
        pushl %ecx             ;\
        pushl %ebx             ;\
        call func              ;\
        call handle_signal     ;\
        popl %ebx              ;\
        popl %ecx              ;\
        popl %edx              ;\
        popl %esi              ;\
        popl %edi              ;\
        popl %ebp              ;\
        popl %eax              ;\
        popl %ds               ;\
        popl %es               ;\
        popl %fs               ;\
        addl $4, %esp          ;\
        iret 

#assembly linkage for interrupts
INTR_LINK(keyboard_handler_linkage, keyboard_handler);   
INTR_LINK(rtc_handler_linkage, rtc_handler);         
INTR_LINK(pit_handler_linkage, pit_handler);  

#create the assembly linkage for mouse interrupt
INTR_LINK(mouse_handler_linkage, mouse_handler); 
#assembly linkage for exceptions without error code
INTR_LINK(exception_handler_de_linkage, exception_handler_de);  
INTR_LINK(exception_handler_db_linkage, exception_handler_db);  
INTR_LINK(exception_handler_nmi_linkage, exception_handler_nmi);  
INTR_LINK(exception_handler_bp_linkage, exception_handler_bp);  
INTR_LINK(exception_handler_of_linkage, exception_handler_of);  
INTR_LINK(exception_handler_br_linkage, exception_handler_br);  
INTR_LINK(exception_handler_ud_linkage, exception_handler_ud);  
INTR_LINK(exception_handler_nm_linkage, exception_handler_nm);   
INTR_LINK(exception_handler_cso_linkage, exception_handler_cso);     
INTR_LINK(exception_handler_reserved_linkage, exception_handler_reserved);  
INTR_LINK(exception_handler_mf_linkage, exception_handler_mf);   
INTR_LINK(exception_handler_mc_linkage, exception_handler_mc);  
INTR_LINK(exception_handler_xm_linkage, exception_handler_xm);  


#for exceptions with error code
#define INTR_LINK_ERROR(name, func)   \
    .globl name                ;\
    name:                      ;\
        pushl %fs              ;\
        pushl %es              ;\
        pushl %ds              ;\
        pushl %eax             ;\
        pushl %ebp             ;\
        pushl %edi             ;\
        pushl %esi             ;\
        pushl %edx             ;\
        pushl %ecx             ;\
        pushl %ebx             ;\
        call func              ;\
        call handle_signal     ;\
        popl %ebx              ;\
        popl %ecx              ;\
        popl %edx              ;\
        popl %esi              ;\
        popl %edi              ;\
        popl %ebp              ;\
        popl %eax              ;\
        popl %ds               ;\
        popl %es               ;\
        popl %fs               ;\
        addl $4, %esp          ;\
        iret 

INTR_LINK_ERROR(exception_handler_df_linkage, exception_handler_df); 
INTR_LINK_ERROR(exception_handler_ts_linkage, exception_handler_ts); 
INTR_LINK_ERROR(exception_handler_np_linkage, exception_handler_np);  
INTR_LINK_ERROR(exception_handler_ss_linkage, exception_handler_ss);  
INTR_LINK_ERROR(exception_handler_gp_linkage, exception_handler_gp);  
INTR_LINK_ERROR(exception_handler_pf_linkage, exception_handler_pf);
INTR_LINK_ERROR(exception_handler_ac_linkage, exception_handler_ac); 

