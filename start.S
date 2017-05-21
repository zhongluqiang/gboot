.text
.global _start
_start:
	b reset
	ldr pc, _undefined_instruction  @�˴�ldr������һ��αָ�����һ��װ��ָ����ڴ洦װ��ֵ
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

	
_undefined_instruction: .word undefined_instruction @���ڴ��ַ������Ŷ�Ӧ����ŵĵ�ַ��������һ����
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used: .word not_used
_irq: .word irq
_fiq: .word fiq


undefined_instruction:
	nop
	
software_interrupt:
	nop

prefetch_abort:
	nop

data_abort:
	nop
	
not_used:
	nop

irq:
	nop

fiq:
	nop
	
reset:
	bl set_svc
	bl disable_watchdog
	bl disable_interrupt
	bl disable_mmu
	
	
set_svc:
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr,r0
	mov pc, lr 

#define WTCON 0x7E004000	
disable_watchdog:
	ldr r0, =WTCON
	mov r1, #0x0
	str r1, [r0]
	mov pc, lr
	
disable_interrupt:
	mvn r1, #0x0
	ldr r0, =0x71200014
	str r1, [r0]
	ldr r0, =0x71300014
	str r1, [r0]
	mov pc, lr
	
disable_mmu:
	mcr p15, 0, r0, c7, c7, 0  @��I/D Cache��ʧЧ
	mrc p15, 0, r0, c1, c0, 0  @��ȡControl�Ĵ�����ֵ��r0
	bic r0, r0, #0x7           @�������λ���㣬�ر�MMU��DCache
	mcr p15, 0, r0, c1, c0, 0  @��r0��ֵд��Control�Ĵ���
	mov pc, lr