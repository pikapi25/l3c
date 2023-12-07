#ifndef _INTERRUPT_LINKAGE_H
#define _INTERRUPT_LINKAGE_H
#ifndef ASM


extern void keyboard_handler_linkage();
extern void rtc_handler_linkage();
extern void pit_handler_linkage();


extern void exception_handler_de_linkage();
extern void exception_handler_db_linkage();  
extern void exception_handler_nmi_linkage(); 
extern void exception_handler_bp_linkage();
extern void exception_handler_of_linkage(); 
extern void exception_handler_br_linkage();
extern void exception_handler_ud_linkage(); 
extern void exception_handler_nm_linkage(); 
extern void exception_handler_cso_linkage();  
extern void exception_handler_reserved_linkage();
extern void exception_handler_mf_linkage(); 
extern void exception_handler_mc_linkage();  
extern void exception_handler_xm_linkage();

extern void exception_handler_df_linkage();
extern void exception_handler_ts_linkage();
extern void exception_handler_np_linkage();
extern void exception_handler_ss_linkage();
extern void exception_handler_gp_linkage();
extern void exception_handler_pf_linkage();
extern void exception_handler_ac_linkage();


#endif
#endif
