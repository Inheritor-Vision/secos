/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>


extern info_t *info;
extern void resume_from_intr();
extern void bp_pre_handler();


void  __regparm__(1) bp_handler(int_ctx_t *ctx){
    // asm volatile (".align  16\n");
    // asm volatile ("pusha \n");
    asm volatile("pusha\n");
    debug("\nBp handler reached!\n");
    debug("Ctx: eax 0x%x\n", ctx->gpr.eax);
    asm volatile("popa; leave; iret");
    /*asm volatile("jmp     resume_from_intr\n");
    asm volatile("push %%eax \n" : "=a" (ctx->gpr.eax));
    asm volatile("push %%eax \n" : "=a" (ctx->gpr.ecx));
    asm volatile("push %%eax \n" : "=a" (ctx->gpr.edx));
    asm volatile("push %%eax \n" : "=a" (ctx->gpr.ebx));
    asm volatile("push %%eax \n" : "=a" (ctx->gpr.ebp));
    asm volatile("push %%eax \n" : "=a" (ctx->gpr.esi));
    asm volatile("push %%eax \n" : "=a" (ctx->gpr.edi));
    return ;
    asm volatile("jmp     resume_from_intr\n");*/
}

void bp_trigger(){
    asm volatile("int3 \n" ::: "memory"); 
}

void print_idt(int_desc_t * base_addr, int nbr_desc){
    printf("DEBUT\n");
    for (int i = 0; i < nbr_desc; i++){
        debug("INT desc %d with addr %p\n",i, base_addr);
        int a = base_addr->offset_1 + (base_addr->offset_2 << 16);
        debug("offset:0x%x selector:0x%x ist:0x%x type:0x%x zero1:0x%x zero2:0x%x DPL:0x%x P:0x%x \n\n", a, base_addr->selector, base_addr->ist, base_addr->type, base_addr->zero_1, base_addr->zero_2, base_addr->p,base_addr->dpl, base_addr->p);
        base_addr++;
    }
    printf("FIN\n");
}


void tp()
{   

    idt_reg_t idt_r;
    get_idtr(idt_r);
    debug("IDT limit 0x%x\n", idt_r.limit);
    debug("IDT addr 0x%x\n", idt_r.addr);

    int_desc_t *idtd = (int_desc_t *)idt_r.desc;
    print_idt(idtd, 10);

    raw32_t addr = {.raw = (uint32_t) &bp_handler};
    idtd[3].offset_1 = addr.wlow;
    idtd[3].offset_2 = addr.whigh;

    //idtd[3].offset_1 = 0xffff & ((int ) &bp_trigger);
    //idtd[3].offset_2 = 0xffff & (((int ) &bp_trigger) >> 16);

    print_idt(idtd, 4);
    debug("%p\n", bp_handler);

    bp_trigger();
    debug("LOLOLOL\nLOLOLOL\nLOLOLOL\nLOLOLOL\n");
    while (1){}
    


}
