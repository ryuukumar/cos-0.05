#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/sys.h>
#include <sys/multiboot.h>
#include "../kernel/memory/mmain.h"

extern void gdt_flush();
extern void get_gdt();

struct gdt_entry gdt[3];
struct gdt_ptr gp;
struct gdt_ptr mainGdtP;

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (uint32_t) &gdt;

    /* Our NULL descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);
    /* The Code Segment */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    /* The Data Segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Flush out the old GDT and install the new changes! */
    gdt_flush();
}

struct gdt_ptr get_gdt_ptr() {
	return mainGdtP;
}

/*----------------*/

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load();
extern void* memset(void*, char, size_t);

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {

	/* The interrupt routine's base address */
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

/* Installs the IDT */
void idt_install() {

    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = &idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    /* Add any new ISRs to the IDT here using idt_set_gate */

    /* Points the processor's internal register to the new IDT */
    idt_load();
}

/*----------------*/

inline void i86_pic_send_command (uint8_t cmd, uint8_t picNum) {

	if (picNum > 1)
		return;

	uint8_t	reg = (picNum==1) ? 0xA0 : 0x20;
	outb (reg, cmd);
}


//! send data to PICs
inline void i86_pic_send_data (uint8_t data, uint8_t picNum) {

	if (picNum > 1)
		return;

	uint8_t	reg = (picNum==1) ? 0xA1 : 0x21;
	outb (reg, data);
}

//! Initialize pic
void pic_install () {

	uint8_t base0 = (idtp.base & 0xF); uint8_t base1 = ((idtp.base & 0xF0) >> 4);
	uint8_t	icw = 0;

	//! disable hardware interrupts
	//disable ();

	//! Begin initialization of PIC

	icw = (icw & ~0x10) | 0x10;
	icw = (icw & ~0x1) | 1;

	i86_pic_send_command (icw, 0);
	i86_pic_send_command (icw, 1);

	//! Send initialization control word 2. This is the base addresses of the irq's

	i86_pic_send_data (base0, 0);
	i86_pic_send_data (base1, 1);

	//! Send initialization control word 3. This is the connection between master and slave.
	//! ICW3 for master PIC is the IR that connects to secondary pic in binary format
	//! ICW3 for secondary PIC is the IR that connects to master pic in decimal format

	i86_pic_send_data (0x04, 0);
	i86_pic_send_data (0x02, 1);

	//! Send Initialization control word 4. Enables i86 mode

	icw = (icw & ~0x1) | 1;

	i86_pic_send_data (icw, 0);
	i86_pic_send_data (icw, 1);
}
