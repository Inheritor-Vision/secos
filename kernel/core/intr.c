/* GPLv2 (c) Airbus */
#include <intr.h>
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <cr.h>

#include <mapping.h>
#include <segmentation.h>
#include <asm.h>


extern info_t *info;
extern void idt_trampoline();
static int_desc_t IDT[IDT_NR_DESC];

enum IRQ0_mode{IRQ0_KERNEL, IRQ0_USER1, IRQ0_USER2} irq0_current_mode;
uint32_t esp_user1, esp_user2;

void sys_counter(uint32_t *counter){
   debug("\n[INT]Int 0x80: cpt= 0x%x\n", *counter);
}

void intr_IRQ0(void){
   tss_t *ts = (tss_t*) SEGMENTATION_TSS_BASE_ADDRESS;
   uint8_t *sem = (uint8_t*)(SHARED_USERS_MEMORY + sizeof(uint32_t));
   switch(irq0_current_mode){
      case IRQ0_USER1:
         *sem = 1;
         irq0_current_mode = IRQ0_USER2;
         set_cr3(PAGING_PGD_USER2_BASE_ADDRESS);
         ts->s0.esp = USER2_KERNEL_STACK; 
         esp_user1 = USER1_KERNEL_STACK;
         set_esp(esp_user2);
         asm volatile("jmp resume_from_intr");
         break;
      case IRQ0_USER2:
         *sem = 1;
         irq0_current_mode = IRQ0_USER1;
         ts->s0.esp = USER1_KERNEL_STACK; 
         set_cr3(PAGING_PGD_USER1_BASE_ADDRESS);
         esp_user2 = USER2_KERNEL_STACK;
         set_esp(esp_user1);
         asm volatile("jmp resume_from_intr");
         break;
      default:
         irq0_current_mode = IRQ0_USER1;
         esp_user1 =  USER1_KERNEL_STACK;
         esp_user2 =  USER2_KERNEL_STACK;
         set_cr3(PAGING_PGD_USER1_BASE_ADDRESS);
         ts->s0.esp = USER1_KERNEL_STACK; 
         set_esp(esp_user1);
         asm volatile("jmp resume_from_intr");
   }

}

void intr_init()
{
   idt_reg_t idtr;
   offset_t  isr;
   size_t    i;

   isr = (offset_t)idt_trampoline;

   /* re-use default grub GDT code descriptor */
   for(i=0 ; i<IDT_NR_DESC ; i++, isr += IDT_ISR_ALGN)
      int_desc(&IDT[i], gdt_krn_seg_sel(1), isr);

   idtr.desc  = IDT;
   idtr.limit = sizeof(IDT) - 1;
   set_idtr(idtr);

   //Perso
   IDT[0x80].dpl = 3;
   irq0_current_mode = IRQ0_KERNEL;
}

void __regparm__(1) intr_hdlr(int_ctx_t *ctx)
{
   uint8_t vector = ctx->nr.blow;

   if(vector == 0x80){
      sys_counter((uint32_t*)(ctx->gpr.eax.raw));
   }else if(vector == 0x20){
      debug("\nHorloge!\n");
      intr_IRQ0();
   }else{
      debug("\nIDT event\n"
            " . int    #%d\n"
            " . error  0x%x\n"
            " . cs:eip 0x%x:0x%x\n"
            " . ss:esp 0x%x:0x%x\n"
            " . eflags 0x%x\n"
            "\n- GPR\n"
            "eax     : 0x%x\n"
            "ecx     : 0x%x\n"
            "edx     : 0x%x\n"
            "ebx     : 0x%x\n"
            "esp     : 0x%x\n"
            "ebp     : 0x%x\n"
            "esi     : 0x%x\n"
            "edi     : 0x%x\n"
            ,ctx->nr.raw, ctx->err.raw
            ,ctx->cs.raw, ctx->eip.raw
            ,ctx->ss.raw, ctx->esp.raw
            ,ctx->eflags.raw
            ,ctx->gpr.eax.raw
            ,ctx->gpr.ecx.raw
            ,ctx->gpr.edx.raw
            ,ctx->gpr.ebx.raw
            ,ctx->gpr.esp.raw
            ,ctx->gpr.ebp.raw
            ,ctx->gpr.esi.raw
            ,ctx->gpr.edi.raw);

      

      if(vector < NR_EXCP)
         excp_hdlr(ctx);
      else
         debug("ignore IRQ %d\n", vector);
   }
  
}
