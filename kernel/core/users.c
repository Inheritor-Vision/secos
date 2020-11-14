#include <users.h>
#include <mapping.h>
#include <debug.h>

uint32_t *shared_cpt = (uint32_t*)SHARED_USERS_MEMORY;

void __attribute__((section(".user1"))) user1(void){
    debug("\nuser1 reached!\n");
    while(1){
        (*shared_cpt)++;
    }


}

void __attribute__((section(".user2"))) user2(void){
    debug("\nuser2 reached!\n");
    while(1){
        asm volatile("int $0x80"::"a"(shared_cpt));
    }

    
}