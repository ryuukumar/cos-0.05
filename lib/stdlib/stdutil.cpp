#include <stddef.h>
#include <stdint.h>

#include "../../include/stdutil.h"

uint8_t skim32to8 (uint32_t var) {
	return (var & 0xFF);
}
