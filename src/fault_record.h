#ifndef FAULT_RECORD_H
#define FAULT_RECORD_H

#include <stdint.h>

// The magic word tells a real record from power-on RAM garbage.
#define FAULT_RECORD_MAGIC 0x4641554Cu

typedef struct {
  uint32_t magic, id, pc, info;
} fault_record_t;

// Second half of the linker's 0x80-byte NOINIT region at 0x20007F80; the first
// half holds the BLE peer data. Placed by address because a second .noinit input
// section from another object file is not merged into the NOINIT output section.
#define FAULT_RECORD_ADDR 0x20007FC0u
#define g_fault_record (*(fault_record_t*) FAULT_RECORD_ADDR)

#endif
