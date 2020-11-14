/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <string.h>

#include <segmem.h>

extern info_t *info;

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
    gdt_reg_t gdt_r= { 0 };
    get_gdtr(gdt_r);
    debug("GDTR limit 0x%x\n", gdt_r.limit);
    debug("GDTR addr 0x%x\n", gdt_r.addr);

    //Question 1
    seg_desc_t *gdt_s = gdt_r.desc;
    print_gdt(gdt_r.desc, 5);


    //Question2
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

    gdt_s = (seg_desc_t *)0x100000; 
    gdt_r.limit = 0x17;
    gdt_r.desc = gdt_s;
    set_gdtr(gdt_r); 
    set_cs(8);
    set_ds(16);
    set_ss(16);
    set_es(16);
    set_gs(16);
    set_fs(16);



    //Question 3
    char  src[64];
    char *dst = 0;

    memset(src, 0xff, 64);

    
    gdt_s += 3;
    
    memset(gdt_s, 0,sizeof(seg_desc_t));
    gdt_s->type = SEG_DESC_DATA_RW;
    gdt_s->limit_1 = 0x20;  
    gdt_s->base_2 = 0x60;
    gdt_s->d = 1;
    gdt_s->p = 1;
    gdt_s->s = 1;
    
    gdt_s = (seg_desc_t *)0x100000;
    print_gdt(gdt_s, 4);
    gdt_r.limit = 0x1F;
    gdt_r.desc = gdt_s;
    set_gdtr(gdt_r); 
    set_cs(8);
    set_ds(16);
    set_ss(16);
    set_es(24);
    set_gs(16);
    set_fs(16);

    
    debug("new ES 0x%x\n", get_es());
    gdt_reg_t gdt_r2= { 0 };
    get_gdtr(gdt_r2);
    debug("new GDTR limit 0x%x\n", gdt_r2.limit);
    debug("new GDTR addr 0x%x\n", gdt_r2.addr);
    _memcpy8(dst, src, 32);
    debug("passed 32\n");
    _memcpy8(dst, src, 64);
    debug("passed 64\n");
    
}
