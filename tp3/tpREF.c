/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

#include <string.h>
#include <segmem.h>
#include <cr.h>


extern info_t *info;

void userland(){
    while (1) ;
    
}

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


void tp()
{

    //Ancienne GDT
    gdt_reg_t gdt_r= { 0 };
    get_gdtr(gdt_r);
    seg_desc_t *gdt_s = gdt_r.desc;

    //Nvelle GDT
    gdt_s = (seg_desc_t *)0x100000;

    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->limit_1 = 0xfff0;

    gdt_s++;
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_CODE_XR;
    gdt_s->limit_1 = 0xffff;
    gdt_s->limit_2 = 0xf;    
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    gdt_s->g = 1;

    gdt_s++;
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_DATA_RW;
    gdt_s->limit_1 = 0xffff;
    gdt_s->limit_2 = 0xf;   
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    gdt_s->g = 1;
    

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
    gdt_s->avl = 1;
    printf("Addr gdt code user %x\n", ((uint32_t) gdt_s) - 0x100000);

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
    gdt_s->avl = 1;

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
    gdt_s->avl = 1;
    gdt_s->dpl = 3;
    printf("Addr gdt pile user %x\n", ((uint32_t) gdt_s) - 0x100000);
    

    seg_desc_t *gdt_next =  ++gdt_s;

    gdt_s = (seg_desc_t *)0x100000; 
    //print_gdt(gdt_s, 5);

    seg_sel_t data_userland;
    data_userland.rpl = 3;
    data_userland.ti = 0;
    data_userland.index = 4;


    printf("\nCP1 Reached!\n");
    
    /*Question 3.1: OK*/


    /*Question 3.2: NOK*/
    //set_ss(data_userland.raw);

    /*Question 3.2: NOK*/
    // set_cs(27);

    printf("\nCP2 Reached!\n");

    // fptr32_t jmp = {.segment=27, .offset= (uint32_t)userland};
    // farjump(jmp);

    printf("\nCP3 Reached!\n");
    
    // asm volatile("mov %esp, %eax");
    // asm volatile("push 0x23");
    // asm volatile("push %eax");
    // asm volatile("pushf");
    // asm volatile("push 0x1B");
    // asm volatile("push userland");
    // asm volatile("iret");

/*       asm volatile("  \
                   \
     mov %esp, %eax; \
     pushl $0x23; \
     pushl %eax; \
     pushf; \
     pushl $0x1B; \
     pushl $userland; \
     iret; \
   1: \
    call userland \
     ");*/



    tss_t *tss_seg = (tss_t*)0x400000;
    memset(tss_seg, 0, sizeof(tss_t));
    tss_seg->s0.ss = data_userland.raw;
    tss_seg->s0.esp = 0x300010;

    uint32_t base = ((uint32_t) tss_seg);
    uint32_t limit = sizeof(tss_t);

    gdt_s = gdt_next;
    gdt_next++;

    gdt_s->limit_1 = limit & 0xFFFF;
    gdt_s->limit_2 = (limit & 0xF0000) >>16;
    gdt_s->base_1 = base & 0xFFFF;
    gdt_s->base_2 = (base & 0xFF0000)>>16;
    gdt_s->base_3 = (base & 0xFF000000)>>24;
    gdt_s->type = SEG_DESC_SYS_TSS_AVL_32;
    gdt_s->p = 1;
    gdt_s->s = 0;
    gdt_s->dpl = 3;

    gdt_s = (seg_desc_t *)0x100000; 
    print_gdt(gdt_s, 7);

    gdt_r.limit = 0x37;
    gdt_r.desc = gdt_s;
    set_gdtr(gdt_r);
    seg_sel_t tr_userland;
    tr_userland.rpl = 0;
    tr_userland.ti = 0;
    tr_userland.index = 0x6;
    set_tr(tr_userland.raw);

    idt_reg_t idt_r;
    get_idtr(idt_r);
    debug("IDT limit 0x%x\n", idt_r.limit);
    debug("IDT addr 0x%x\n", idt_r.addr);

    int i;
    idt_reg_t idt_r_u;
    idt_r_u.limit = idt_r.limit;
    idt_r_u.addr = (offset_t) 0x600000;
    int_desc_t *idtd = (int_desc_t *)idt_r.addr;
    int_desc_t *idtd_u = (int_desc_t*)0x600000;
    for (i = 0; i < 256; i++){
        memcpy(idtd_u,idtd, sizeof(int_desc_t));
        idtd_u->dpl = 3;
        idtd++;
        idtd_u++;
    }

    printf("\nCP4 Reached! %p\n",userland);

    set_idtr(idt_r_u);

    asm volatile("  movw %%ss, %0 \n \
       movl %%esp, %1" : "=m" (tss_seg->s0.ss), "=m" (tss_seg->s0.esp) : );

    set_ds(data_userland.raw);
    set_es(data_userland.raw);
    set_gs(data_userland.raw);
    set_fs(data_userland.raw);
    
    

    asm volatile("  \
     cli \n           \
     push $0x2B \n \
     push $0x500000 \n \
     pushfl           \n \
     popl %%eax        \n \
     orl $0x200, %%eax \n \
     and $0xffffbfff, %%eax \n \
     push %%eax        \n \
     push $0x1B \n \
     push $0x304078 \n \
     movl $0x20000, %0 \n \
     iret \n \
     ": "=m" (tss_seg->s0.esp) : );



    


}
