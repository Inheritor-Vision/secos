/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

#include <pagemem.h>
#include <cr.h>
#include <string.h>

extern info_t *info;

void tp()
{
    cr0_reg_t cr0 = {.raw =get_cr0()};
    printf("\n\nCeci n'est pas un exercice: cr0=%p\n\n", cr0);

    //Question3: Triple fault car la table est vide: Donc pas de maping entre adresse phy et virt, donc pas moyen de faire quoi que ce soit à part planter 
    
    //Question4:
    pde32_t *pde = (pde32_t *) 0x600000;
    memset(pde, 0, sizeof(pde32_t)*1024);
    pte32_t *pte = (pte32_t *)0x601000;
    memset(pte, 0, sizeof(pte32_t)*1024);
    cr3_reg_t cr3 = { 0 };

    cr3.addr = ((uint32_t)pde) >>12;
    set_cr3(cr3);


    printf("\n\npde1=%p\n", pde);
    printf("pte1=%p\n", pte);
    
    pde->addr = ((uint32_t)pte) >>12;
    pde->p = 1;
    pde->rw = 1;

    
    pde++;
    pte = (pte32_t *)0x602000;
    memset(pte, 0, sizeof(pte32_t)*1024);
    printf("pde2=%p\n", pde);
    printf("pte2=%p\n", pte);
    pde->addr = ((uint32_t)pte) >>12;
    pde->p = 1;
    pde->rw = 1;



    pte = (pte32_t*) ((0x300000 >>12)*sizeof(pte32_t) + 0x601000);
    printf("pte 0x300000=%p\n", pte);
    pte->addr = 0x300000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x301000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x302000 >>12;
    pte->p = 1;

    pte++;
    pte->addr = 0x303000 >>12;
    pte->p = 1;

    pte++;
    pte->addr = 0x304000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x305000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x306000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x307000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x308000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x309000 >>12;
    pte->p = 1;
    pte->rw = 1;


    pte = (pte32_t*) ((0x67ef0 >>12)*sizeof(pte32_t) + 0x601000);
    printf("pte 0x67ef0=%p\n", pte);
    pte->addr = 0x67ef0 >> 12;
    pte->p = 1;
    pte->rw = 1;


    pte = (pte32_t*) ((0x0 >>12)*sizeof(pte32_t) + 0x601000);
    printf("pte 0x0=%p\n", pte);
    pte->addr = 0x0;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x1000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x2000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x3000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x4000 >>12;
    pte->p = 1;
    pte->rw = 1;
    
    pte++;
    pte->addr = 0x5000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x6000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x7000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x8000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x9000 >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0xa000 >>12;
    pte->p = 1;
    pte->rw = 1;





    
    pte = (pte32_t*) (((0x600000 % (1<<22))  >>12)*sizeof(pte32_t) + 0x602000);
    printf("pte 0x600000=%p\n", pte);
    pte->addr = 0x600000  >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x601000   >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x602000  >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x603000   >>12;
    pte->p = 1;
    pte->rw = 1;

    pte++;
    pte->addr = 0x604000   >>12;
    pte->p = 1;
    pte->rw = 1;



    printf("\nCP1 Reached!\n\n");
    
    cr0.pg = 1;
    set_cr0(cr0);

    cr0.pg = 0;
    set_cr0(cr0);

    printf("\nCP2 Reached!\n\n");

    cr0.pg = 1;
    set_cr0(cr0);

    pde = (pde32_t*) 0x600000;
    printf("PDE: %p Present: %x Address:%x RW: %x\n",pde, pde->p,  pde->addr, pde->rw);
    
    cr0.pg = 0;
    set_cr0(cr0);

    // pte = (pte32_t*) ((0x0 >>12)*sizeof(pte32_t) + 0x601000);
    // pte->addr = 0x0;
    // pte->p = 1;
    // pte->rw = 1;

    printf("\nCP3 Reached!\n\n");

    pde = (pde32_t*) 0x600000;
    pde += 0xc0000000 / (1<<22) ;
    pte = (pte32_t*)0x603000;
    memset(pte, 0, sizeof(pte32_t)*1024);
    printf("pde3=%p\n", pde);
    printf("pte3=%p\n", pte);
    pde->addr = ((uint32_t)pte) >>12;
    pde->p = 1;
    pde->rw = 1;

    pte = (pte32_t*) (((0xc0000000 % (1<<22)) >>12)*sizeof(pte32_t) + 0x603000);
    printf("pte 0xc0000000=%p\n", pte);
    pte->addr = 0x600000 >>12;
    pte->p = 1;
    pte->rw = 1;

    // cr0.pg = 1;
    // set_cr0(cr0);
    // void * ptr = (void*) 0xc0000000;
    // memset(ptr, 0, sizeof(pde32_t) * 1024);

    printf("\nCP4 Reached!\n\n");


    pte = (pte32_t*) (((0x700000 % (1<<22))  >>12)*sizeof(pte32_t) + 0x602000);
    printf("pte 0x700000=%p\n", pte);
    pte->addr = (0x2000 >> 12);
    pte->p = 1;
    pte->rw = 1;

    pte = (pte32_t*) (((0x7ff000 % (1<<22))  >>12)*sizeof(pte32_t) + 0x602000);
    printf("pte 0x7ff000=%p\n", pte);
    pte->addr = (0x2000 >> 12);
    pte->p = 1;
    pte->rw = 1;

    cr0.pg = 1;
    set_cr0(cr0);

    char *str = (char*) 0x700000;
    printf("String at 0x700000:%s\n", str);
    str = (char*) 0x7ff000;
    printf("String at 0x7ff000:%s\n", str);


    printf("\nCP5 Reached!\n\n");

    // pde = (pde32_t *) 0x600000;
    // printf("PDE: %p Present: %x Address:%x RW: %x\n",pde, pde->p,  pde->addr, pde->rw);
    // memset(pde,0,sizeof(pde32_t));
    // printf("PDE: %p Present: %x Address:%x RW: %x\n",pde, pde->p,  pde->addr, pde->rw);

    printf("\nCP6 Reached!\n\n");




}
