/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

#include <string.h>
#include <segmem.h>
#include <intr.h>

#define GDT_VOID 0
#define GDT_CODE_KERNEL 1
#define GDT_DATA_KERNEL 2
#define GDT_CODE_USER 3
#define GDT_DATA_USER 4
#define GDT_STACK_USER 5
#define GDT_TSS 6

extern info_t *info;

tss_t tss_seg ;

char * texteB = "I'm afraid I can't explain myself, sir ... Because I am not myself, you see?\n";

void print_gdt(seg_desc_t * base_addr, int nbr_desc){
    printf("\n");
    for (int i = 0; i < nbr_desc; i++){
        debug("GDT desc %d with addr %p\n",i, base_addr);
        int a = base_addr->base_1 + (base_addr->base_2 << 16) + (base_addr->base_3 << 24);
        int b = base_addr->limit_1 + (base_addr->limit_2 << 16);
        debug("base:0x%x limit:0x%x G:0x%x D/B:0x%x L:0x%x AVL:0x%x P:0x%x DPL:0x%x S:0x%x Type:0x%x\n\n", a, b, base_addr->g, base_addr->d, base_addr->l, base_addr->avl, base_addr->p,base_addr->dpl, base_addr->s, base_addr->type);
        base_addr++;
    }
}

void print_idt(int_desc_t * base_addr, int nbr_desc){
    printf("\n");
    for (int i = 0; i < nbr_desc; i++){
        debug("IDT desc %d with addr %p\n",i, base_addr);
        int a = base_addr->offset_1 + (base_addr->offset_2 << 16);
        int b = base_addr->zero_1 + (base_addr->zero_2 >>5);
        debug("offset:0x%x seg_sel:0x%x dpl:0x%x ist:0x%x p:0x%x type:0x%x zeros:0x%x \n\n", a, base_addr->selector, base_addr->dpl, base_addr->ist, base_addr->p, base_addr->type,b);
        base_addr++;
    }
}

void userland(){
    char * texteA = "War is peace.\nFreedom is slavery.\nIgnorance is strength.\n";
    asm volatile("int $0x30\n" ::"S" (texteA), "a" (0x48));

    asm volatile("int $0x30\n" ::"S" (texteB), "a" (texteB));
    printf("Userland while 1 reached!\n");
    while (1) ;
    
}

void syscall_isr()
{
   asm volatile (
      "leave ; pusha        \n"
      "mov %esp, %eax      \n"
      "call syscall_handler \n"
      "popa ; iret"
      );
}

void __regparm__(1) syscall_handler(int_ctx_t *ctx)
{  
   debug("\nSYSCALL eax = %p\n", ctx->gpr.eax);
   debug("\n%s\n", ctx->gpr.esi);
}

void tp()
{   
    //Temp zone 


    //Set up de int 48
    idt_reg_t temp_reg;
    get_idtr(temp_reg);
    int_desc_t *tmp = temp_reg.desc;
    tmp[48].dpl = 3;
    print_idt(&tmp[48],1);

    //Set up syscall
    raw32_t addr = {.raw = (uint32_t) syscall_isr};
    tmp[48].offset_1 = addr.wlow;
    tmp[48].offset_2 = addr.whigh;




    //Ancienne GDT
    gdt_reg_t gdt_r= { 0 };
    get_gdtr(gdt_r);
    seg_desc_t *gdt_s = gdt_r.desc;

    //Nvelle GDT
    gdt_s = (seg_desc_t *)0x100000;

    //GDT[0]:GDT_VOID
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->limit_1 = 0xfff0;

    //GDT[1]:GDT_CODE_KERNEL
    gdt_s++;
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_CODE_XR;
    gdt_s->limit_1 = 0xffff;
    gdt_s->limit_2 = 0xf;    
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    gdt_s->g = 1;

    //GDT[2]:GDT_DATA_KERNEL
    gdt_s++;
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_DATA_RW;
    gdt_s->limit_1 = 0xffff;
    gdt_s->limit_2 = 0xf;   
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    gdt_s->g = 1;
    
    //GDT[3]:GDT_CODE_USER
    gdt_s++;
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_CODE_XR;
    gdt_s->limit_1 = 0xffff;
    gdt_s->limit_2 = 0xf;
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    gdt_s->g = 1;
    gdt_s->dpl = 3;

    //GDT[4]:GDT_DATA_USER
    gdt_s++;
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_DATA_RW;
    gdt_s->limit_1 = 0xffff;
    gdt_s->limit_2 = 0xf;
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    gdt_s->g = 1;
    gdt_s->dpl = 3;

    //GDT[5]:GDT_STACK_USER
    gdt_s++;
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_DATA_ERW;
    gdt_s->base_1 = 0x500 & 0xFFFF;
    gdt_s->base_2 = (0x500 & 0xFF0000)>>16;
    gdt_s->base_3 = (0x500 & 0xFF000000)>>24;
    gdt_s->limit_1 = 0x20;
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    gdt_s->g = 1;
    gdt_s->dpl = 3;

    //GDT[6]:GDT_TSS
    gdt_s++;
    raw32_t base     = {.raw = (offset_t)&tss_seg};  
    uint32_t limit = sizeof(tss_t);
    gdt_s->limit_1 = limit & 0xFFFF;
    gdt_s->limit_2 = (limit & 0xF0000) >>16;
    gdt_s->base_1 = base.wlow;
    gdt_s->base_2 = base._whigh.blow;
    gdt_s->base_3 = base._whigh.bhigh;
    gdt_s->type = SEG_DESC_SYS_TSS_AVL_32;
    gdt_s->p = 1;

    //Set GDTr
    gdt_s = (seg_desc_t *)0x100000; 
    print_gdt(gdt_s, 7);
    gdt_r.limit = 0x37;
    gdt_r.desc = gdt_s;
    set_gdtr(gdt_r);

    //Set TSSr
    memset(&tss_seg, 0, sizeof(tss_t));
    tss_seg.s0.ss = gdt_krn_seg_sel(GDT_DATA_KERNEL);
    tss_seg.s0.esp = get_esp();
    set_tr(gdt_krn_seg_sel(GDT_TSS));
    asm volatile("  movw %%ss, %0 \n \
        movl %%esp, %1" : "=m" (tss_seg.s0.ss), "=m" (tss_seg.s0.esp) : );

    //Set Generic Selectors
    set_ds(gdt_usr_seg_sel(GDT_DATA_USER));
    set_es(gdt_usr_seg_sel(GDT_DATA_USER));
    set_gs(gdt_usr_seg_sel(GDT_DATA_USER));
    set_fs(gdt_usr_seg_sel(GDT_DATA_USER));

    //Iret to r0
    asm volatile("  \
     cli \n           \
     push %0 \n \
     push %%esp \n \
     pushf         \n \
     push %1 \n \
     push %2 \n \
     iret \n \
     ":: "i"(gdt_usr_seg_sel(GDT_DATA_USER)), //Why no stack user ? 
		"i"(gdt_usr_seg_sel(GDT_CODE_USER)),"i"(userland));



}
