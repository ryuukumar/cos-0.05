#include <stddef.h>
#include <stdint.h>

#include <mvmng.h>
#include <mpmng.h>
#include <mmain.h>

#include <stdio.h>
#include <sys/drivers/timer.h>

extern "C" {
	extern void write_cr3(uint32_t);
	extern uint32_t read_cr3(void);
}

void set_bit_pdir (uint32_t bit, page_dir_entry_t* p_entry) {*p_entry |= bit;}
void clr_bit_pdir (uint32_t bit, page_dir_entry_t* p_entry) {*p_entry &= ~bit;}
uint32_t get_bit_pdir (uint32_t bit, page_dir_entry_t* p_entry) {return *p_entry & bit;}
// note: above will return 0 if clear, else won't return 1
void set_addr_pdir (uint32_t addr, page_dir_entry_t* p_entry) {*p_entry = (*p_entry & ~dframe) | addr;}
uint32_t get_addr_pdir (page_dir_entry_t* p_entry) {return *p_entry & dframe;}

void set_bit_ptb (uint32_t bit, page_table_entry_t* p_entry) {*p_entry |= bit;}
void clr_bit_ptb (uint32_t bit, page_table_entry_t* p_entry) {*p_entry &= ~bit;}
uint32_t get_bit_ptb (uint32_t bit, page_table_entry_t* p_entry) {return *p_entry & bit;}
// note: above will return 0 if clear, else won't return 1
void set_addr_ptb (uint32_t addr, page_table_entry_t* p_entry) {*p_entry = (*p_entry & ~dframe) | addr;}
uint32_t get_addr_ptb (page_table_entry_t* p_entry) {return *p_entry & tframe;}

pd* gpdir = NULL;

bool alloc_page_v (page_table_entry_t* e) {
	void* p = alloc_block_p ();
	if (p==0) return false;
	
	set_addr_ptb ((uint32_t)p, e);
	set_bit_ptb (tpresent, e); 
	return true;
}

void free_page_v (page_table_entry_t* e) { 
	void* p = (void*)get_addr_ptb (e);
	if (p) free_block_p ((uint32_t)p); 
	clr_bit_ptb (tpresent, e);
}

page_table_entry_t* find_pt_entry_v (pt* p, uint32_t addr) { 
	if (p) return &p->entry[(((addr) >> 12) & 0x3ff)];
	return 0;
}

page_dir_entry_t* find_pd_entry_v (pd* p, uint32_t addr) { 
	if (p) return &p->entry[(((addr) >> 12) & 0x3ff)];
	return 0;
}

bool set_pdir (pd* pdir) {
	if (!pdir) return false;
	gpdir = pdir;
	write_cr3((uint32_t)pdir);
	if (read_cr3() == (uint32_t)pdir) return true;
	return false;
}

pd* get_pdir () {return gpdir;}

void map_page_v (void* phys, void* virt) {
	pd* pdir = gpdir;
	page_dir_entry_t* e = &pdir->entry [((((uint32_t)virt) >> 22) & 0x3ff)];
	
	if ((*e & tpresent) != tpresent) {
		//! page table not present, allocate it
		pt* table = (pt*) alloc_block_p ();
		if (!table) return;

		//! clear page table
		memset (table, 0, sizeof(pt));

		//! create a new entry
		page_dir_entry_t* entry =
		&pdir->entry [((((uint32_t)virt) >> 22) & 0x3ff)];

		//! map in the table (Can also just do *entry |= 3) to enable these bits
		set_bit_pdir (dpresent, entry);
		set_bit_pdir (dwritable, entry);
		set_addr_pdir ((uint32_t)table, entry);
	}
	
	//! get table
	pt* table = (pt*) (*e & ~0xfff);

	//! get page
	page_table_entry_t* page = &table->entry [((((uint32_t)virt) >> 12) & 0x3ff)];

	//! map it in (Can also do (*page |= 3 to enable..)
	set_addr_ptb ((uint32_t) phys, page);
	set_bit_ptb (tpresent, page);
}

void init_vm() {
	//! allocate default page table
	pt* table = (pt*) alloc_block_p ();
	if (!table) return;
 
	//! allocates 3gb page table
	pt* table2 = (pt*) alloc_block_p ();
	if (!table2) return;

	//! clear page table
	memset((void*) table, 0, sizeof(pt));
	
	//! 1st 4mb are idenitity mapped
	for (int i=0, frame=0x0, virt=0x00000000; i<1024; i++, frame+=4096, virt+=4096) {
 		//! create a new page
		page_table_entry_t page=0;
		set_bit_ptb (tpresent, &page);
 		set_addr_ptb (frame, &page);

		//! ...and add it to the page table
		table2->entry [(((virt) >> 12) & 0x3ff)] = page;
	}
	
	//! map 1mb to 3gb (where we are at)
	for (int i=0, frame=0x100000, virt=0xc0000000; i<1024; i++, frame+=4096, virt+=4096) {

		//! create a new page
		page_table_entry_t page=0;
		set_bit_ptb (tpresent, &page);
 		set_addr_ptb (frame, &page);

		//! ...and add it to the page table
		table->entry [(((virt) >> 12) & 0x3ff)] = page;
	}
	
	//! create default directory table
	pd* dir = (pd*) alloc_blocks_p (3);
	if (!dir) return;
 
	//! clear directory table and set it as current
	memset (dir, 0, sizeof (pd));
	
	page_dir_entry_t* entry = &dir->entry [((0xC0000000) >> 22) & 0x3ff];
	set_bit_pdir (dpresent, entry);
	set_bit_pdir (dwritable, entry);
	set_addr_pdir ((uint32_t)table, entry);

	page_dir_entry_t* entry2 = &dir->entry [((0x00000000) >> 22) & 0x3ff];
	set_bit_pdir (dpresent, entry2);
	set_bit_pdir (dwritable, entry2);
	set_addr_pdir ((uint32_t)table2, entry2);
 
	//! switch to our page directory
	set_pdir(dir);
 
	//! enable paging	
	__asm__("mov %cr4, %eax\n\t"
			"or $0x80000001, %eax\n\t"
			"mov %eax, %cr4\n\t");
}
