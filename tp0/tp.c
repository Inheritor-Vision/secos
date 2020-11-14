/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

void tp() {
   debug("kernel mem [0x%x - 0x%x]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);


   debug("MBI mmap_addr 0x%x\n", info->mbi->mmap_addr);
   debug("MBI mmap_length 0x%x\n", info->mbi->mmap_length);

   unsigned int taille2 = 0;
   multiboot_memory_map_t * test2 = (multiboot_memory_map_t *)info->mbi->mmap_addr;
   int ram_num = 0;

   while(taille2 < info->mbi->mmap_length){
      debug("\nMM%d(0x%x):",ram_num,taille2);
      debug("MM%d size 0x%lx addr 0x%08llx length 0x%08llx type 0x%lx\n",ram_num, test2->size,test2->addr,test2->len, test2->type);
      ram_num++;
      taille2 += test2->size + 4;
      test2 = (multiboot_memory_map_t *)(info->mbi->mmap_addr + taille2);

   }

   char * test = (char *) 0x100000000000;
   *test = 'l';
   printf("%c \n", *test);
}
