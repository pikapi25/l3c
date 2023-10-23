/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */

/*
 * i8259_init
 * INPUT: none
 * OUTPUT: none
 * Effect: set PIC, mask all interrupts
*/
/* Initialize the 8259 PIC */
void i8259_init(void) {
    /* first mask all interrupts */
    cli();


    outb(0xff, MASTER_8259_DATA);
    outb(0xff, SLAVE_8259_DATA);
    /* send control words to master */
    outb(ICW1, MASTER_8259_PORT); // start init, edge-triggered inputs, cascade mode, 4 ICWs
    outb(ICW2_MASTER, MASTER_8259_DATA); // 8259A-1, IR0-7 mapped to 0x20-0x21
    outb(ICW3_MASTER, MASTER_8259_DATA); // 8259A-1 (the primary) has a secondary on IRQ2
    outb(ICW4, MASTER_8259_DATA); // ISA=x86, normal EOI

    /* send contro words to slave */
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);
    sti();
    /* enable slave (port 2 on master) */
    enable_irq(ICW3_SLAVE);
}

/*
 * enable_irq
 * INPUT: irq number
 * OUTPUT: none
 * Effect: set the IRQ line to be unmasked
*/
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    /* if irq_num is between 0-7, it should be on master */
    if (irq_num >= 0 && irq_num <=7){
        /* first set bit-irq_num to 1, all others to 0 by shifting, 
         * then use NOT to set all others to 1 and only bit-irq_num to 0 */ 
        master_mask &= ~(1 << irq_num); 
        outb(master_mask, MASTER_8259_DATA);
    }else if (irq_num >= 8 && irq_num <=15){
        /* if irq_num is between 8-15, it should be on slave */
        /* and the actual IRQ line should be irq_num - 8 */
        slave_mask &= ~(1 << (irq_num-8)); 
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/*
 * disable_irq
 * INPUT: irq number
 * OUTPUT: none
 * Effect: set the IRQ line to be masked
*/
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    /* if irq_num is between 0-7, it should be on master */
    if (irq_num >= 0 && irq_num <=7){
        /* Set bit-irq_num to 1, all others to 0 by shifting */
        master_mask |= (1 << irq_num); 
        outb(master_mask, MASTER_8259_DATA);
    }else if (irq_num >= 8 && irq_num <=15){
        /* if irq_num is between 8-15, it should be on slave */
        /* and the actual IRQ line should be irq_num - 8 */
        slave_mask |= (1 << (irq_num-8)); 
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/*
 * send_eoi
 * INPUT: irq number
 * OUTPUT: none
 * Effect: Send end-of-interrupt signa
*/
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
     /* if irq_num is between 0-7, it should be on master */
     if (irq_num >= 0 && irq_num <=7){
        /* send EOI to master */
        outb(EOI | irq_num, MASTER_8259_PORT); // tell the master which line 
    }else if (irq_num >= 8 && irq_num <=15){
        /* if irq_num is between 8-15, it should be on slave */
        /* and we should send EOI to both master and slave */
        outb(EOI | 2, MASTER_8259_PORT); // tell the master the slave is sending EOI (from irq line 2)
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT); // tell the slave which line 
    }
}
