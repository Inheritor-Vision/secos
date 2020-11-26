/* GPLv2 (c) Airbus */
#include <string.h>

#include <debug.h>
#include <info.h>
#include <asm.h>
#include <intr.h>

#include <paging.h>
#include <mapping.h>
#include <users.h>
#include <segmentation.h>

#include <pagemem.h>


extern info_t *info;

//Faire à la mano la première fois 
void init_user1_kernel_stack(void){
    memset((void *)USER1_KERNEL_STACK, 0, sizeof(int_ctx_t));
    int_ctx_t *stack = (int_ctx_t*) USER1_KERNEL_STACK;

    stack->ss.raw = gdt_usr_seg_sel(GDT_DESC_USER_DATA);
    stack->esp.raw = (uint32_t) USER1_USER_STACK;
    stack->eflags.raw = EFLAGS_IF;
    stack->cs.raw = gdt_usr_seg_sel(GDT_DESC_USER_CODE);
    stack->eip.raw = (uint32_t) user1;

  




}

void init_user2_kernel_stack(void){

    memset((void *)USER2_KERNEL_STACK, 0, sizeof(int_ctx_t));
    int_ctx_t *stack = (int_ctx_t*) USER2_KERNEL_STACK;

    stack->ss.raw = gdt_usr_seg_sel(GDT_DESC_USER_DATA);
    stack->esp.raw = (uint32_t) USER2_USER_STACK;
    stack->eflags.raw = EFLAGS_IF;
    stack->cs.raw = gdt_usr_seg_sel(GDT_DESC_USER_CODE);
    stack->eip.raw = (uint32_t) user2;

    
}



void tp()
{   
    
    init_gdt();
    init_paging();
    debug("\n[TP] Starting init_user1_kernel_stack\n");
    init_user1_kernel_stack();
    debug("[TP] End of init_user1_kernel_stack!\n");
    debug("[TP]\n");
    debug("[TP] Init init_user2_kernel_stack\n");
    init_user2_kernel_stack();
    debug("[TP] End of init_user2_kernel_stack!\n");


    pte32_t *pte_user = (pte32_t *) (PAGING_PTB_USER1_KERNEL_BASE_ADDRESS + 0x305 * sizeof(pte32_t));
    debug("\naddr 0x%x p 0x%x wr 0x%x us 0x%x\n", pte_user->addr << 12, pte_user->p, pte_user->rw, pte_user->lvl);

    set_registers_user();


    force_interrupts_on();
    while(1);
    
}
