# ECE391 MP3 Team L3C Working Space

## CHECKPOINT 1

| Task     | Assignee  |
| -------- | --------- |
| Load the GDT   | dsc  |
| Initialize the IDT   | dsc  |
| Initialize the PIC   | lkn  |
| Initialize the keyboard  | lyz  |
| Initialize the RTC   | not assigned  |
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
### Initialize Paging
***