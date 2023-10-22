# ECE391 MP3 Team L3C Working Space

## CHECKPOINT 1

| Task     | Assignee  |
| -------- | --------- |
| Load the GDT   | dsc  |
| Initialize the IDT   | dsc  |
| Initialize the PIC   | lkn  |
| Initialize the keyboard  | lyz  |
| Initialize the RTC   | lkn  |
| Initialize Paging   | ljy  |


### Load the GDT 
***
### Initialize the IDT
***
### Initialize the PIC @lkn
***
**command port and data port**    (Reference: [8259 PIC](https://wiki.osdev.org/8259_PIC))
| Chip - Purpose    | I/O port  |
| -------- | --------- |
| Master PIC - Command   | 0x0020  |
| Master PIC - Data   | 0x0021  |
| Slave PIC - Command   | 0x00A0 |
| Slave PIC - Data  | 0x00A1  |

**control words**
| Control Words    | port  | info |
| -------- | --------- | --------- |
| ICW1 - 0x11   | Master/Slave PIC - Command  | start init, edge-triggered inputs, cascade mode, 4 ICWs |
| ICW2_MASTER -0x20  | Master PIC - Data  | 8259A-1, IR0-7 mapped to 0x20-0x21 |
| ICW2_SLAVE - 0x28   | Slave PIC - Data | 8259A-2, IR0-7 mapped to 0x28-0x2f |
| ICW3_MASTER  | Master PIC - Data  | 8259A-1 (the primary) has a secondary on IRQ2 |
| ICW3_SLAVE  | Slave PIC - Data  | 8259A-2 is a secondary on primary’s IRQ2 |
| ICW4  | Master/Slave PIC - Data  | ISA=x86, normal EOI |

``i8259_init``: This is a function to initialize master and slave PICs.  
>    1. mask master and slave  
>    2. send control words to master  
>    3. send control words to slave  
>    4. enable slave   

*Q: Do we need to set critical sections?*  

``enable_irq``: This is used to Enable (unmask) the specified IRQ.  
>   1. Find the corresponding PIC and IRQ line. 
>   - If IRQ < 8, choose master, IRQ line = IRQ;
>   - If IRQ >= 8, choose slave, IRQ line = IRQ - 8;  
>   2. Set the IRQ line to be unmasked (to 0).

``enable_irq``: This is used to Disable (mask) the specified IRQ.  
>   It should be almost the same as ``enable_irq``, the only difference is it set IRQ line to 1.  

``send_eoi``: This is used to send end-of-interrupt signal for the specified IRQ.  
>   1. Find the corresponding PIC and IRQ line.   
>   - If IRQ < 8, choose master  
>   - If IRQ >= 8, choose slave  
>   2. Send EOI to PIC.
>   - If it's an interrupt on slave, both slave and master should receive EOI
>   - If it's an interrupt on master, only master should receive EOI 
>   - Don't forget to OR EOI with correct IRQ line before sending!  
***
### Initialize the keyboard
***
### Initialize the RTC  
The 2 I/O ports used for the RTC and CMOS are 0x70 and 0x71.  
>   - Port 0x70 is used to specify an index or "register number", and to disable NMI. 
>   - Port 0x71 is used to read or write from/to that byte of CMOS configuration space. 
>   - Only three bytes of CMOS RAM are used to control the RTC periodic interrupt function. 
>   - They are called RTC Status Register A, B, and C. They are at offset 0xA, 0xB, and 0xC in the CMOS RAM.  

``rtc_init``: This is used to initialize the RTC and enable it on PIC.  
>   1. Turn on IRQ 8
>   2. Set the interrupt rate
>   3. Initialize all variables
>   4. Enable IRQ 8 on PIC  

``rtc_handler``: This is used to handle the interrupt handler, and disable irq 8.
>   1. Set interrupt flag to 0, indicating the interrupt happend.
>   2. Read Register C so that the interrupt can happen again.  

***
### Initialize Paging @ljy
***
Reference web link1: https://wiki.osdev.org/CPU_Registers_x86#CR0
Reference web link1: https://wiki.osdev.org/Paging

PDE (Page Directory Entry) structure
|31 ... 12|11 ... 8| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---------|--------|---|---|---|---|---|---|---|---|
| Address |   AVL  | PS| D | A |PCD|PWT|U/S|R/W| P |

>- (0) P : present. set 1 when the page is present. set 0 indicates being ignored.
>- (1) R/W : set 1 when page is writable. set 0 when page is read only.
>- (2) U/S : set 1 when user can visit. set 0 when user is forbidden and only supervisor(kernel) is allowed.
>- (3) PWT : set 1 when using write through. set 0 when using write back.
>- (4) PCD : set 1 when disable page cache. set 0 when enable page cache.
>- (5) A : set 1 when the page is being accessed once. set 0 by the system.
>- (6) D : set 1 when the page is written before. set 0 if the page is not changed.
>- (7) PS : set 1 when PDE is mapped into 4MB page. set 0 when PDE is mapped into 4 KB page table.
>- (11 ... 8) AVL : is ignored.
>- (31 ... 12) Address : For PDE, it's the address of PT;For PTE, it's physical address of 4kb memory space (memory frame)
>- PDE and PTE has small difference in PAT (bit 7)

``Page_Initialize``: Initialize page table(PT) and page directory(PD) when kernel is loaded. (called in kernel.c)
>1. create global array for PT and PD;
>2. Initialize and set flags for PTE and PDE.
>3. load kernel memory (start at 4MB and end at 8MB)and set flag 
>4. load video memory (start at 0xb8000 and end at 0xb9000 )and set flag
>5. change cr(s) registers (flush tlb and open paging mode and protection mode)



## CHECKPOINT 2

| Task     | Assignee  |
| -------- | --------- |
| Create a Terminal Driver   | lyz & lkn  |
| Parse the Read-only File System   | dsc & ljy  |
| The Real-Time Clock Driver   | lkn  |

### The Real-Time Clock Driver  
**virtualizing the RTC**:
>   - "By virtualizing the RTC you basically give the illusion to the process  
that the RTC is set to their frequency when in reality the RTC isn’t.   
Instead you set the RTC to the highest possible frequency and wait until   
you receive x interrupts to return back to the process such that those x interrupts   
at the highest frequency is equivalent to 1 interrupt at the frequency the process wants."
***  
``rtc_open``: Reset the frequency to 2Hz.  
>   - "set up any data necessary to handle RTC"

``rtc_read``:  
>   - "You might want to use some sort of flag here (you will not need spinlocks. Why?)"  
>   - "should always return 0."  
>   - "but only after an interrupt has occurred (set a flag and wait until the interrupt handler clears it, then return 0)"

``rtc_write``: This function is used to set the interrupt rate.  
>   - "should always accept only a 4-byte integer specifying the interrupt rate in Hz."  
>   - "and should set the rate of periodic interrupts accordingly."
>   - "The RTC device itself can only generate interrupts at a rate that is a power of 2 (do a parameter check)"
>   - "writing to the RTC should block interrupts to interact with the device."  
>   - "must get its input parameter through a buffer and not read the value directly." 
>   - "The call returns the number of bytes written, or -1 on failure." 

``rtc_close``: successful closes should return 0.

