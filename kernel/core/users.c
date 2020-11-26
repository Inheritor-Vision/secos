#include <users.h>
#include <mapping.h>
#include <debug.h>





void __attribute__((section(".user1"))) user1(void){
    uint8_t *sem = (uint8_t*)(SHARED_USERS_MEMORY + sizeof(uint32_t));
    uint32_t *shared_cpt1 = (uint32_t*)USER1_VIRTUAL_SHARED_MEMORY;
    while(1){
        if(*sem == 1){
            (*shared_cpt1)++;
            //debug("\n[USR]Valeur incrémentée! %p\n",sem);
            *sem = 0;
        } 
    }
}

void __attribute__((section(".user2"))) user2(void){
    uint8_t *sem = (uint8_t*)(SHARED_USERS_MEMORY + sizeof(uint32_t));
    uint32_t *shared_cpt2 = (uint32_t*)USER2_VIRTUAL_SHARED_MEMORY;
    while(1){
        if(*sem == 1){
            asm volatile("int $0x80"::"a"(shared_cpt2));
            *sem = 0;
        }
    }

    
}