#include <stddef.h>
#include <stdint.h>

#include "../include/sys/sys.h"

struct gdt_entry gdt[3];
struct gdt_ptr gp;

void gdt_flush(struct gdt_ptr x) {
	__asm__ ("lgdt [%eax]\n\t"
		"mov $0x10, %bx\n\t"
		"mov %bx, %ds\n\t"
		"mov %bx, %es\n\t"
		"mov %bx, %fs\n\t"
		"mov %bx, %gs\n\t"
		"mov %bx, %ss\n\t");
}

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
    gdt_flush(gp);
}

void initSys() {
	
}
