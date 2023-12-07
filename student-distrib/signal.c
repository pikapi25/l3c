#include "signal.h"
#include "syscall.h"
#include "lib.h"
#include "scheduler.h"
#include "terminal.h"
// #include "types.h"


/*
 * kill_task
 * default signal handler for signal number 0, 1 and 2
 */
void kill_task(){
    int i;
    pcb_t* cur_pcb = get_cur_pcb();
    for (i = 0; i < SIG_COUNT; i++){
        cur_pcb->sig_masked[i] = 0;
    }
    halt(0);
    return;
}

/*
 * ignore
 * default signal handler for signal number 3 and 4
 */
void ignore(){
    int i;
    pcb_t* cur_pcb = get_cur_pcb();
    for (i = 0; i < SIG_COUNT; i++){
        cur_pcb->sig_masked[i] = 0;
    }
    return;
}

/*
 * send_signal
 * send a signal to current program
 */
void send_signal(int sig_num){
    pcb_t* cur_pcb;
    cli();
    if(sig_num == INTERRUPT){
        cur_pcb = (pcb_t*)(EIGHT_MB-(myScheduler.tasks[curr_term_id]+1)*EIGHT_KB);
    }else{
        cur_pcb = get_cur_pcb();
    }
    cur_pcb->sig_pending[sig_num] = 1;
    sti();
}

/*
 * handle_signal
 * Check whether there is pending signal in current process, 
 * if so, build the stack in user stack
 * if not, do nothing
 */
void handle_signal(){
    pcb_t* cur_pcb = get_cur_pcb();
    uint32_t sig_num = -1;  

    /*mask all other signals*/
    int i, j;
    for(i = 0; i < SIG_COUNT; i++){
        if(cur_pcb->sig_pending[i] == 1){
            cur_pcb->sig_pending[i] = 0;
            for(j = 0; j < SIG_COUNT; j++){
                cur_pcb->sig_masked[j] = 1;
            }
            sig_num = i;
            break;
        }
    }
    //no pending signal
    if(sig_num == -1)
        return;

    /*handle pending signal*/
    void* handler;
    handler = cur_pcb->signal_handler[sig_num];
    //call default signal handler
    if (handler == kill_task || handler == ignore){
        ((void(*)())handler)();
        return;
    }

    //set up stack frame
    uint32_t user_esp;  
    uint32_t asm_size;
    uint32_t return_addr; 
    register uint32_t saved_ebp asm("ebp");
    hwcontext_t* context = (hwcontext_t*)(saved_ebp + 8);
    user_esp = context->esp;        //esp of user stack

    // ignore interrupts from the kernel space
    if(user_esp < USER_PROGRAM_IMG_START){return;}

    asm_size = DO_SIG_RETURN_END - DO_SIG_RETURN;
    return_addr = user_esp - asm_size;
    
    //build stack frame
    memcpy((void*)(user_esp - asm_size), DO_SIG_RETURN, asm_size);
    memcpy((void*)(user_esp - asm_size - sizeof(hwcontext_t)), context, sizeof(hwcontext_t));
    memcpy((void*)(user_esp - asm_size - sizeof(hwcontext_t) - 4), (void*)(&sig_num), 4);
    memcpy((void*)(user_esp - asm_size - sizeof(hwcontext_t) - 8), (void*)(&return_addr), 4);

    // set the new esp in hwcontext for iret
    context->return_address = (uint32_t)handler;
    user_esp = user_esp - asm_size - sizeof(hwcontext_t) - 8;
    context->esp = user_esp;
}

