#ifndef __SEGMENTATION_H__
#define __SEGMENTATION_H__

#include <segmem.h>

#define GDT_DESC_EMPTY 0
#define GDT_DESC_KERNEL_CODE 1
#define GDT_DESC_KERNEL_DATA 2
#define GDT_DESC_USER_CODE 3
#define GDT_DESC_USER_DATA 4
#define GDT_DESC_TSS 5

#define GDT_SIZE 6

void print_gdt(seg_desc_t * base_addr, int nbr_desc);
void print_gdtr(void);
void init_gdt(void);
void set_registers_kernel(void);
void set_registers_user(void);
#endif