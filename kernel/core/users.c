#include <users.h>
#include <mapping.h>
#include <debug.h>

uint32_t *shared_cpt = (uint32_t*)SHARED_USERS_MEMORY;
uint8_t *sem = (uint8_t*)(SHARED_USERS_MEMORY + sizeof(uint32_t));

void __attribute__((section(".user1"))) user1(void){
    while(1){
        if(*sem == 1){
            (*shared_cpt)++;
            debug("\n[USR]Valeur incrémentée!\n");
            *sem = 0;
        } 
    }
}

void __attribute__((section(".user2"))) user2(void){
    while(1){
        if(*sem == 1){
            asm volatile("int $0x80"::"a"(shared_cpt));
            *sem = 0;
        }
    }

    
}