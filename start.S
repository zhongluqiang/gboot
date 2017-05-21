.text
.global _start
_start:
	b reset
	ldr pc, _undefined_instruction  @此处ldr不再是一条伪指令，而是一条装载指令，从内存处装载值
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

	
_undefined_instruction: .word undefined_instruction @该内存地址处存放着对应语句标号的地址，长度是一个字
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
	nop