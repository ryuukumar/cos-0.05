#ifndef MPMNG_H
#define MPMNG_H

typedef struct {
	uint32_t base;
	uint32_t len;
	bool occupied;
} m_block;

/*extern uint32_t* malloc(int);
extern bool is_alloc(uint32_t);
extern bool is_range_alloc(uint32_t, int);
extern bool free(uint32_t);*/

extern void init_pm();

extern void* alloc_block_p();
extern void* alloc_blocks_p(size_t);
extern void free_block_p(uint32_t);
extern void free_blocks_p(uint32_t, int);

#endif
