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
	if (read_cr3() == pdir) return true;
	return false;
}


