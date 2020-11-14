#include <paging.h>
#include <mapping.h>

#include <pagemem.h>
#include <cr.h>
#include <debug.h>

#include <string.h>

void init_kernel(void){
    pde32_t *pde = (pde32_t *) PAGING_PGD_KERNEL_BASE_ADDRESS;
    memset(pde, 0, sizeof(pde32_t)*1024);
    pte32_t *pte = (pte32_t *) PAGING_PTB_KERNEL_BASE_ADDRESS;
    memset(pte, 0, sizeof(pte32_t)*1024);

    pg_set_entry(pde,PG_KRN|PG_RW,page_nr(PAGING_PTB_KERNEL_BASE_ADDRESS));

    for(unsigned long i = 0; i < PTE32_PER_PT; i++){
        pg_set_entry(&pte[i], PG_KRN|PG_RW, (i + (0x0>>10)));
    }
}

void init_user1(void){
    pde32_t *pde = (pde32_t *) PAGING_PGD_USER1_BASE_ADDRESS;
    memset(pde, 0, sizeof(pde32_t)*1024);
    pte32_t *pte_kernel = (pte32_t *) PAGING_PGD_USER1_KERNEL_BASE_ADDRESS;
    memset(pte_kernel, 0, sizeof(pte32_t)*1024);
    pte32_t *pte_user = (pte32_t *) PAGING_PTB_USER1_USER_BASE_ADDRESS;
    memset(pte_user, 0, sizeof(pte32_t)*1024);

    pg_set_entry(pde,PG_USR|PG_RW,page_nr(PAGING_PGD_USER1_KERNEL_BASE_ADDRESS));
    pg_set_entry(&pde[1],PG_USR|PG_RW,page_nr(PAGING_PTB_USER1_USER_BASE_ADDRESS));

    //A raffiner si l'envie me prends
    for(unsigned long i = 0; i < PTE32_PER_PT; i++){
        pg_set_entry(&pte_kernel[i], PG_KRN|PG_RW, (i + (0x0<<10)));
    }

    for(unsigned long i = 0; i < PTE32_PER_PT; i++){
        if(((i << 12) + (1<<22)) == USER1_VIRTUAL_SHARED_MEMORY){
            debug("Reached ! 0x%x %d \n", i,i);
            pg_set_entry(&pte_user[i], PG_USR|PG_RW, SHARED_USERS_MEMORY >> 12);
        }else{
            pg_set_entry(&pte_user[i], PG_USR|PG_RW, (i + (0x1<<10)));
        }
        
    }
    
}

void init_user2(void){
    pde32_t *pde = (pde32_t *) PAGING_PGD_USER2_BASE_ADDRESS;
    memset(pde, 0, sizeof(pde32_t)*1024);
    pte32_t *pte_kernel = (pte32_t *) PAGING_PGD_USER2_KERNEL_BASE_ADDRESS;
    memset(pte_kernel, 0, sizeof(pte32_t)*1024);
    pte32_t *pte_user = (pte32_t *) PAGING_PTB_USER2_USER_BASE_ADDRESS;
    memset(pte_user, 0, sizeof(pte32_t)*1024);

    pg_set_entry(pde,PG_USR|PG_RW,page_nr(PAGING_PGD_USER2_KERNEL_BASE_ADDRESS));
    pg_set_entry(&pde[1],PG_USR|PG_RW,page_nr(PAGING_PTB_USER2_USER_BASE_ADDRESS));

    //A raffiner si l'envie me prends
    for(unsigned long i = 0; i < PTE32_PER_PT; i++){
        pg_set_entry(&pte_kernel[i], PG_KRN|PG_RW, (i + (0x0<<10)));
    }

    for(unsigned long i = 0; i < PTE32_PER_PT; i++){
        if(((i << 12) + (1<<22)) == USER2_VIRTUAL_SHARED_MEMORY){
            debug("Reached ! 0x%x %d \n", i,i);
            pg_set_entry(&pte_user[i], PG_USR|PG_RW, SHARED_USERS_MEMORY >> 12);
        }else{
            pg_set_entry(&pte_user[i], PG_USR|PG_RW, (i + (0x1<<10)));
        }
    }
    
}

void init_paging(void){
    init_kernel();
    init_user1();
    init_user2();

    cr3_reg_t cr3 = { 0 };
    cr3.addr = ((uint32_t)PAGING_PGD_KERNEL_BASE_ADDRESS) >>12;
    set_cr3(cr3);
    set_cr0(get_cr0()|CR0_PG);

    
}