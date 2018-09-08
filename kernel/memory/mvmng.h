#ifndef MVMNG_H
#define MVMNG_H

typedef enum page_dir_entry_m {
	dpresent = 1,
	dwritable = 2,
	duser = 4,
	dpwd = 8,
	dpcd = 0x10,
	daccessed = 0x20,
	ddirty = 0x40,
	d4mb = 0x80,
	dcpu_global = 0x100,
	dlv4_global = 0x200,
	dframe = 0x7ffff000
};
typedef uint32_t page_dir_entry_t;

typedef enum {
	tpresent = 1,
	twritable = 2,
	tuser = 4,
	twritethrough = 8,
	tnot_cacheable = 0x10,
	taccessed = 0x20,
	tdirty = 0x40,
	tpat = 0x80,
	tcpu_global = 0x100,
	tlv4_global = 0x200,
	tframe = 0x7ffff000
} page_table_entry_m;
typedef uint32_t page_table_entry_t;

extern void set_bit_pdir (uint32_t, page_dir_entry_t*);
extern void clr_bit_pdir (uint32_t, page_dir_entry_t*);
extern uint32_t get_bit_pdir (uint32_t, page_dir_entry_t*);
extern void set_addr_pdir (uint32_t, page_dir_entry_t*);
extern uint32_t get_addr_pdir (page_dir_entry_t*);

extern void set_bit_ptb (uint32_t, page_table_entry_t*);
extern void clr_bit_ptb (uint32_t, page_table_entry_t*);
extern uint32_t get_bit_ptb (uint32_t, page_table_entry_t*);
extern void set_addr_ptb (uint32_t, page_table_entry_t*);
extern uint32_t get_addr_ptb (page_table_entry_t*);

typedef struct {
	page_dir_entry_t entry [1024];
} pd;
typedef struct {
	page_table_entry_t entry [1024];
} pt;

extern void init_vm();

#endif
