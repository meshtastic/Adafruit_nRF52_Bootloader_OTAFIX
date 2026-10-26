#ifndef FAULT_RECORD_H
#define FAULT_RECORD_H

#include <stdint.h>

// The magic word tells a real record from power-on RAM garbage.
#define FAULT_RECORD_MAGIC 0x4641554Cu

typedef struct {
  uint32_t magic, id, pc, info;
} fault_record_t;

// Lives in the linker's 16-byte FAULT_RECORD region at the start of bootloader
// RAM: above the SoftDevice RAM, outside .data and .bss, so reset leaves it alone.
extern fault_record_t g_fault_record;

#endif
