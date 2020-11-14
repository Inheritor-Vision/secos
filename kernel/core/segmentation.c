#include <segmentation.h>

#include <debug.h>
#include <string.h>
#include <mapping.h>


void print_gdt(seg_desc_t * base_addr, int nbr_desc){
    printf("\n[GDT]Print GDT\n");
    for (int i = 0; i < nbr_desc; i++){
        debug("\t[GDT]Print GDT desc %d with addr %p\n",i, base_addr);
        int a = base_addr->base_1 + (base_addr->base_2 << 16) + (base_addr->base_3 << 24);
        int b = base_addr->limit_1 + (base_addr->limit_2 << 16);
        debug("\t[GDT]Base:0x%x Limit:0x%x G:0x%x D/B:0x%x L:0x%x AVL:0x%x P:0x%x DPL:0x%x S:0x%x Type:0x%x\n\n", a, b, base_addr->g, base_addr->d, base_addr->l, base_addr->avl, base_addr->p,base_addr->dpl, base_addr->s, base_addr->type);
        base_addr++;
    }
}

void print_gdtr(void){
    gdt_reg_t gdt_r= { 0 };
    get_gdtr(gdt_r);
    debug("[GDT]Print GDTR\n", gdt_r.addr);
    debug("\t[GDT]Address 0x%x\n", gdt_r.addr);
    debug("\t[GDT]Limit 0x%x\n", gdt_r.limit);
}

void create_gdt(void){
    seg_desc_t *gdt_s = (seg_desc_t*)SEGMENTATION_GDT_BASE_ADDRESS;

    memset(gdt_s, 0,sizeof(seg_desc_t));

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


    tss_t *tss_seg = (tss_t*) SEGMENTATION_TSS_BASE_ADDRESS;
    memset(tss_seg, 0, sizeof(tss_t));
    tss_seg->s0.ss = gdt_krn_seg_sel(GDT_DESC_KERNEL_DATA);

    raw32_t base     = {.raw = (offset_t)tss_seg};  
    uint32_t limit = sizeof(tss_t);

    gdt_s++;
    gdt_s->limit_1 = limit & 0xFFFF;
    gdt_s->limit_2 = (limit & 0xF0000) >>16;
    gdt_s->base_1 = base.wlow;
    gdt_s->base_2 = base._whigh.blow;
    gdt_s->base_3 = base._whigh.bhigh;
    gdt_s->type = SEG_DESC_SYS_TSS_AVL_32;
    gdt_s->p = 1;

    gdt_s++;
    gdt_reg_t gdtr;
    gdtr.desc = (seg_desc_t*) SEGMENTATION_GDT_BASE_ADDRESS;
    gdtr.limit = (((uint32_t)gdt_s) - SEGMENTATION_GDT_BASE_ADDRESS) - 1;
    set_gdtr(gdtr);
}

void set_registers(void){
    set_cs(gdt_krn_seg_sel(GDT_DESC_KERNEL_CODE));
	set_ss(gdt_krn_seg_sel(GDT_DESC_KERNEL_DATA));
	set_ds(gdt_krn_seg_sel(GDT_DESC_KERNEL_DATA));
	set_es(gdt_krn_seg_sel(GDT_DESC_KERNEL_DATA));
	set_fs(gdt_krn_seg_sel(GDT_DESC_KERNEL_DATA));
	set_gs(gdt_krn_seg_sel(GDT_DESC_KERNEL_DATA));
}

void init_gdt(void){
    create_gdt();
    set_registers();
    print_gdtr();
    print_gdt((seg_desc_t*)SEGMENTATION_GDT_BASE_ADDRESS,GDT_SIZE);
}