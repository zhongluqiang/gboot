.text
.global mem_init
mem_init:
	@set data pin
	ldr r0, =0x7e00f120
	mov r1, #0x0
	str r1, [r0]

	@Enter config state
	ldr r0, =0x7e001004
	mov r1, #0x4
	str r1, [r0]
	
	@Time parameter, chip configuration, id configuration
	ldr r0, =0x7e001004  @allow configuration
	mov r1, #0x4
	str r1, [r0]
	
	ldr r0, =0x7e001010  @refresh period
	ldr r1, =( ( 7800 / ( 1000000000/133000000 ) + 1 ) )
	str r1, [r0]
	
	ldr r0, =0x7e001014  @CAS latency
	mov r1, #(3 << 1)
	str r1, [r0]
	
	ldr r0, =0x7e001018  @t_DQSS
    mov r1, #0x1
    str r1, [r0]
	
	ldr r0, =0x7e00101c  @t_MRD
    mov r1, #0x2
    str r1, [r0]
	
	ldr r0, =0x7e001020   @t_RAS
    ldr r1, =( ( 45 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
    ldr r0, =0x7e001024   @t_RC
    ldr r1, =( ( 68 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
    ldr r0, =0x7e001028   @t_RCD
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
    ldr r0, =0x7e00102c   @t_RFC
    ldr r1, =( ( 80 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
	ldr r0, =0x7e001030   @t_RP
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
	ldr r0, =0x7e001034   @t_RRD
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
    ldr r0, =0x7e001038   @t_WR
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
	ldr r0, =0x7e00103c   @t_WTR
    mov r1, #0x07
    str r1, [r0]
	
	ldr r0, =0x7e001040   @t_XP
    mov r1, #0x02
    str r1, [r0]
	
    ldr r0, =0x7e001044   @t_XSR
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
	ldr r0, =0x7e001048   @t_ESR
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
    ldr r0, =0x7e00100c   @内存控制配置寄存器
    ldr r1, =0x00010012   @配置控制器
    str r1, [r0]

    ldr r0, =0x7e00104c   @32位DRAM配置控制寄存器
    ldr r1, =0x0b45
    str r1, [r0]

    ldr r0, =0x7e001200   @片选寄存器
    ldr r1, =0x150f8
    str r1, [r0]

    ldr r0, =0x7e001304   @用户配置寄存器
    mov r1, #0x0
    str r1, [r0]

	@Excute memory initialization sequence
	ldr r0, =0x7e001008  @NOP
	ldr r1, =0xc0000
	str r1, [r0]
	
	ldr r0, =0x7e001008  @prechargeall
	ldr r1, =0x0        
	str r1, [r0]
	
	ldr r0, =0x7e001008  @autorefresh
	ldr r1, =0x4        
	str r1, [r0]
	
	ldr r0, =0x7e001008  @autorefresh
	ldr r1, =0x4        
	str r1, [r0]
	
	ldr r0, =0x7e001008  @MRS-EMRS muse be set
	ldr r1, =0xa0000        
	str r1, [r0]
	
	ldr r0, =0x7e001008  @MRS-MRS muse be set
	ldr r1, =0x80032        
	str r1, [r0]
	
	@Enter Ready state
	ldr r0, =0x7e001004
	mov r1, #0x0
	str r1, [r0]
	
	@Check until memory state is ready
check_ready:
	ldr r0, =0x7e001000
	ldr r1, [r0]
	mov r2, #0x3
	and r1, r1, r2
	cmp r1, #0x1
	bne check_ready

	mov pc, lr