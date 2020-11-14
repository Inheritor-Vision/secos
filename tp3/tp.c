/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

#include <string.h>
#include <segmem.h>

#define GDT_VOID 0
#define GDT_CODE_KERNEL 1
#define GDT_DATA_KERNEL 2
#define GDT_CODE_USER 3
#define GDT_DATA_USER 4
#define GDT_STACK_USER 5
#define GDT_TSS 6

extern info_t *info;

tss_t tss_seg ;

void userland(){
    printf("YO\n");
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
    

    seg_desc_t *gdt_next =  ++gdt_s;

    gdt_s = (seg_desc_t *)0x100000; 
    //print_gdt(gdt_s, 5);

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



    memset(&tss_seg, 0, sizeof(tss_t));
    tss_seg.s0.ss = gdt_krn_seg_sel(GDT_DATA_KERNEL);
    tss_seg.s0.esp = get_esp();

    raw32_t base     = {.raw = (offset_t)&tss_seg};  
    uint32_t limit = sizeof(tss_t);

    gdt_s = gdt_next;
    gdt_next++;

    gdt_s->limit_1 = limit & 0xFFFF;
    gdt_s->limit_2 = (limit & 0xF0000) >>16;
    gdt_s->base_1 = base.wlow;
    gdt_s->base_2 = base._whigh.blow;
    gdt_s->base_3 = base._whigh.bhigh;
    gdt_s->type = SEG_DESC_SYS_TSS_AVL_32;
    gdt_s->p = 1;

    gdt_s = (seg_desc_t *)0x100000; 
    print_gdt(gdt_s, 7);

    gdt_r.limit = 0x37;
    gdt_r.desc = gdt_s;
    set_gdtr(gdt_r);
    set_tr(gdt_krn_seg_sel(GDT_TSS));

    printf("\nCP4 Reached! %p\n",userland);

    asm volatile("  movw %%ss, %0 \n \
        movl %%esp, %1" : "=m" (tss_seg.s0.ss), "=m" (tss_seg.s0.esp) : );

    set_ds(gdt_usr_seg_sel(GDT_DATA_USER));
    set_es(gdt_usr_seg_sel(GDT_DATA_USER));
    set_gs(gdt_usr_seg_sel(GDT_DATA_USER));
    set_fs(gdt_usr_seg_sel(GDT_DATA_USER));
    
    printf("\nCP5 Reached!\n");

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
