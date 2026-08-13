
pub/otfl:     file format elf64-littleaarch64


Disassembly of section .text:

0000000004022e00 <_start>:
 4022e00:	14000002 	b	4022e08 <reset>
 4022e04:	d503201f 	nop

0000000004022e08 <reset>:
 4022e08:	58000e40 	ldr	x0, 4022fd0 <get_current_core+0x1c>
 4022e0c:	58000e21 	ldr	x1, 4022fd0 <get_current_core+0x1c>
 4022e10:	58000e02 	ldr	x2, 4022fd0 <get_current_core+0x1c>
 4022e14:	58000de3 	ldr	x3, 4022fd0 <get_current_core+0x1c>
 4022e18:	58000dc4 	ldr	x4, 4022fd0 <get_current_core+0x1c>
 4022e1c:	58000da5 	ldr	x5, 4022fd0 <get_current_core+0x1c>
 4022e20:	58000d86 	ldr	x6, 4022fd0 <get_current_core+0x1c>
 4022e24:	58000d67 	ldr	x7, 4022fd0 <get_current_core+0x1c>
 4022e28:	58000d48 	ldr	x8, 4022fd0 <get_current_core+0x1c>
 4022e2c:	58000d29 	ldr	x9, 4022fd0 <get_current_core+0x1c>
 4022e30:	58000d0a 	ldr	x10, 4022fd0 <get_current_core+0x1c>
 4022e34:	58000ceb 	ldr	x11, 4022fd0 <get_current_core+0x1c>
 4022e38:	58000ccc 	ldr	x12, 4022fd0 <get_current_core+0x1c>
 4022e3c:	58000cad 	ldr	x13, 4022fd0 <get_current_core+0x1c>
 4022e40:	58000c8e 	ldr	x14, 4022fd0 <get_current_core+0x1c>
 4022e44:	58000c6f 	ldr	x15, 4022fd0 <get_current_core+0x1c>
 4022e48:	58000c50 	ldr	x16, 4022fd0 <get_current_core+0x1c>
 4022e4c:	58000c31 	ldr	x17, 4022fd0 <get_current_core+0x1c>
 4022e50:	58000c12 	ldr	x18, 4022fd0 <get_current_core+0x1c>
 4022e54:	58000bf3 	ldr	x19, 4022fd0 <get_current_core+0x1c>
 4022e58:	58000bd4 	ldr	x20, 4022fd0 <get_current_core+0x1c>
 4022e5c:	58000bb5 	ldr	x21, 4022fd0 <get_current_core+0x1c>
 4022e60:	58000b96 	ldr	x22, 4022fd0 <get_current_core+0x1c>
 4022e64:	58000b77 	ldr	x23, 4022fd0 <get_current_core+0x1c>
 4022e68:	58000b58 	ldr	x24, 4022fd0 <get_current_core+0x1c>
 4022e6c:	58000b39 	ldr	x25, 4022fd0 <get_current_core+0x1c>
 4022e70:	58000b1a 	ldr	x26, 4022fd0 <get_current_core+0x1c>
 4022e74:	58000afb 	ldr	x27, 4022fd0 <get_current_core+0x1c>
 4022e78:	58000adc 	ldr	x28, 4022fd0 <get_current_core+0x1c>
 4022e7c:	58000abd 	ldr	x29, 4022fd0 <get_current_core+0x1c>
 4022e80:	58000a9e 	ldr	x30, 4022fd0 <get_current_core+0x1c>
 4022e84:	10070be0 	adr	x0, 4031000 <sync_exception_sp_el0>
 4022e88:	d51ec000 	msr	vbar_el3, x0
 4022e8c:	d53e1100 	mrs	x0, scr_el3
 4022e90:	b2400c00 	orr	x0, x0, #0xf
 4022e94:	d51e1100 	msr	scr_el3, x0
 4022e98:	d51e115f 	msr	cptr_el3, xzr
 4022e9c:	94000046 	bl	4022fb4 <get_current_core>
 4022ea0:	b5000200 	cbnz	x0, 4022ee0 <minor_core_boot>
 4022ea4:	580009a0 	ldr	x0, 4022fd8 <get_current_core+0x24>
 4022ea8:	d2800001 	mov	x1, #0x0                   	// #0
 4022eac:	580009a2 	ldr	x2, 4022fe0 <get_current_core+0x2c>
 4022eb0:	eb000042 	subs	x2, x2, x0
 4022eb4:	94003994 	bl	4031504 <clear_data>
 4022eb8:	58000940 	ldr	x0, 4022fe0 <get_current_core+0x2c>
 4022ebc:	927cec1f 	and	sp, x0, #0xfffffffffffffff0
 4022ec0:	58000940 	ldr	x0, 4022fe8 <get_current_core+0x34>
 4022ec4:	d2800001 	mov	x1, #0x0                   	// #0
 4022ec8:	58000942 	ldr	x2, 4022ff0 <get_current_core+0x3c>
 4022ecc:	eb000042 	subs	x2, x2, x0
 4022ed0:	9400398d 	bl	4031504 <clear_data>
 4022ed4:	94000c7a 	bl	40260bc <stack_chk_guard_setup>
 4022ed8:	94000ce9 	bl	402627c <sys_watchdog_enable>
 4022edc:	14000ced 	b	4026290 <main_entry>

0000000004022ee0 <minor_core_boot>:
 4022ee0:	58000401 	ldr	x1, 4022f60 <_jump_addr>
 4022ee4:	f9400037 	ldr	x23, [x1]
 4022ee8:	aa1703e0 	mov	x0, x23
 4022eec:	14000028 	b	4022f8c <switch_from_el3_to_el1>

0000000004022ef0 <non_tee_copy_begin>:
 4022ef0:	aa1503e0 	mov	x0, x21
 4022ef4:	aa1603e1 	mov	x1, x22
 4022ef8:	52800002 	mov	w2, #0x0                   	// #0
 4022efc:	d2800083 	mov	x3, #0x4                   	// #4

0000000004022f00 <do_code_clear_loop>:
 4022f00:	b9000002 	str	w2, [x0]
 4022f04:	8b030000 	add	x0, x0, x3
 4022f08:	eb01001f 	cmp	x0, x1
 4022f0c:	54ffffab 	b.lt	4022f00 <do_code_clear_loop>  // b.tstop
 4022f10:	14000002 	b	4022f18 <set_scs_finish>
 4022f14:	d503201f 	nop

0000000004022f18 <set_scs_finish>:
 4022f18:	58000203 	ldr	x3, 4022f58 <__REG_BASE_CA_MISC>
 4022f1c:	b9440061 	ldr	w1, [x3, #1024]
 4022f20:	121c6c21 	and	w1, w1, #0xfffffff0
 4022f24:	528000a2 	mov	w2, #0x5                   	// #5
 4022f28:	2a020021 	orr	w1, w1, w2
 4022f2c:	b9040061 	str	w1, [x3, #1024]

0000000004022f30 <excute_bootloader>:
 4022f30:	aa1703fe 	mov	x30, x23
 4022f34:	d65f03c0 	ret
 4022f38:	d503201f 	nop
 4022f3c:	d503201f 	nop
 4022f40:	d503201f 	nop
 4022f44:	d503201f 	nop
 4022f48:	d503201f 	nop
 4022f4c:	d503201f 	nop
 4022f50:	d503201f 	nop
 4022f54:	d503201f 	nop

0000000004022f58 <__REG_BASE_CA_MISC>:
 4022f58:	101e8000 	adr	x0, 405ff58 <_end+0x2bb58>
 4022f5c:	00000000 	udf	#0

0000000004022f60 <_jump_addr>:
 4022f60:	04021ef8 	.inst	0x04021ef8 ; undefined
 4022f64:	00000000 	udf	#0

0000000004022f68 <copy_code_to_sram>:
 4022f68:	8b020002 	add	x2, x0, x2

0000000004022f6c <copy_code_to_sram_loop>:
 4022f6c:	a8c12c0a 	ldp	x10, x11, [x0], #16
 4022f70:	a8812c2a 	stp	x10, x11, [x1], #16
 4022f74:	eb02001f 	cmp	x0, x2
 4022f78:	54ffffad 	b.le	4022f6c <copy_code_to_sram_loop>
 4022f7c:	d65f03c0 	ret

0000000004022f80 <_do_error>:
 4022f80:	580003c1 	ldr	x1, 4022ff8 <get_current_core+0x44>
 4022f84:	18000220 	ldr	w0, 4022fc8 <get_current_core+0x14>
 4022f88:	b9000420 	str	w0, [x1, #4]

0000000004022f8c <switch_from_el3_to_el1>:
 4022f8c:	580003a1 	ldr	x1, 4023000 <get_current_core+0x4c>
 4022f90:	d5181001 	msr	sctlr_el1, x1
 4022f94:	580003a1 	ldr	x1, 4023008 <get_current_core+0x54>
 4022f98:	d51c1101 	msr	hcr_el2, x1
 4022f9c:	580003a1 	ldr	x1, 4023010 <get_current_core+0x5c>
 4022fa0:	d51e1101 	msr	scr_el3, x1
 4022fa4:	580003a1 	ldr	x1, 4023018 <get_current_core+0x64>
 4022fa8:	d51e4001 	msr	spsr_el3, x1
 4022fac:	d51e4020 	msr	elr_el3, x0
 4022fb0:	d69f03e0 	eret

0000000004022fb4 <get_current_core>:
 4022fb4:	d53800a0 	mrs	x0, mpidr_el1
 4022fb8:	92401c01 	and	x1, x0, #0xff
 4022fbc:	92781c00 	and	x0, x0, #0xff00
 4022fc0:	8b402020 	add	x0, x1, x0, lsr #8
 4022fc4:	d65f03c0 	ret
 4022fc8:	00000001 	udf	#1
	...
 4022fd8:	04020000 	.inst	0x04020000 ; undefined
 4022fdc:	00000000 	udf	#0
 4022fe0:	04021000 	.inst	0x04021000 ; undefined
 4022fe4:	00000000 	udf	#0
 4022fe8:	04021a80 	.inst	0x04021a80 ; undefined
 4022fec:	00000000 	udf	#0
 4022ff0:	04022050 	.inst	0x04022050 ; undefined
 4022ff4:	00000000 	udf	#0
 4022ff8:	11020000 	add	w0, w0, #0x80
 4022ffc:	00000000 	udf	#0
 4023000:	30d00800 	adr	x0, 3fc3101 <gs_misc_record-0x5e97f>
 4023004:	00000000 	udf	#0
 4023008:	80000000 	.inst	0x80000000 ; undefined
 402300c:	00000000 	udf	#0
 4023010:	00000401 	udf	#1025
 4023014:	00000000 	udf	#0
 4023018:	000003c5 	udf	#965
 402301c:	00000000 	udf	#0

0000000004023020 <clean_boot_img_addr_size>:
 4023020:	d2800280 	mov	x0, #0x14                  	// #20
 4023024:	f2a22240 	movk	x0, #0x1112, lsl #16
 4023028:	b900001f 	str	wzr, [x0]
 402302c:	b900041f 	str	wzr, [x0, #4]
 4023030:	d65f03c0 	ret

0000000004023034 <add_boot_img_size>:
 4023034:	d2800302 	mov	x2, #0x18                  	// #24
 4023038:	f2a22242 	movk	x2, #0x1112, lsl #16
 402303c:	b9400041 	ldr	w1, [x2]
 4023040:	0b000021 	add	w1, w1, w0
 4023044:	b9000041 	str	w1, [x2]
 4023048:	d65f03c0 	ret

000000000402304c <set_verify_backup_img_reg>:
 402304c:	d2800382 	mov	x2, #0x1c                  	// #28
 4023050:	f2a22242 	movk	x2, #0x1112, lsl #16
 4023054:	b9400041 	ldr	w1, [x2]
 4023058:	33180401 	bfi	w1, w0, #8, #2
 402305c:	b9000041 	str	w1, [x2]
 4023060:	d65f03c0 	ret

0000000004023064 <get_verify_backup_img_reg>:
 4023064:	d2800380 	mov	x0, #0x1c                  	// #28
 4023068:	f2a22240 	movk	x0, #0x1112, lsl #16
 402306c:	b9400000 	ldr	w0, [x0]
 4023070:	d3482400 	ubfx	x0, x0, #8, #2
 4023074:	d65f03c0 	ret

0000000004023078 <build_board_type_frame>:
 4023078:	f100003f 	cmp	x1, #0x0
 402307c:	7a4a1840 	ccmp	w2, #0xa, #0x0, ne  // ne = any
 4023080:	54000469 	b.ls	402310c <build_board_type_frame+0x94>  // b.plast
 4023084:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4023088:	aa0103f3 	mov	x19, x1
 402308c:	12800621 	mov	w1, #0xffffffce            	// #-50
 4023090:	f9000bfe 	str	x30, [sp, #16]
 4023094:	d2800014 	mov	x20, #0x0                   	// #0
 4023098:	39000261 	strb	w1, [x19]
 402309c:	52800021 	mov	w1, #0x1                   	// #1
 40230a0:	39000661 	strb	w1, [x19, #1]
 40230a4:	52800081 	mov	w1, #0x4                   	// #4
 40230a8:	39000e61 	strb	w1, [x19, #3]
 40230ac:	53187c01 	lsr	w1, w0, #24
 40230b0:	39001261 	strb	w1, [x19, #4]
 40230b4:	53107c01 	lsr	w1, w0, #16
 40230b8:	39001661 	strb	w1, [x19, #5]
 40230bc:	53087c01 	lsr	w1, w0, #8
 40230c0:	39000a7f 	strb	wzr, [x19, #2]
 40230c4:	39001a61 	strb	w1, [x19, #6]
 40230c8:	52800001 	mov	w1, #0x0                   	// #0
 40230cc:	39001e60 	strb	w0, [x19, #7]
 40230d0:	38746a60 	ldrb	w0, [x19, x20]
 40230d4:	91000694 	add	x20, x20, #0x1
 40230d8:	94000f62 	bl	4026e60 <cal_crc_perbyte>
 40230dc:	12003c01 	and	w1, w0, #0xffff
 40230e0:	f100229f 	cmp	x20, #0x8
 40230e4:	54ffff61 	b.ne	40230d0 <build_board_type_frame+0x58>  // b.any
 40230e8:	53087c20 	lsr	w0, w1, #8
 40230ec:	39002260 	strb	w0, [x19, #8]
 40230f0:	12800aa0 	mov	w0, #0xffffffaa            	// #-86
 40230f4:	39002661 	strb	w1, [x19, #9]
 40230f8:	39002a60 	strb	w0, [x19, #10]
 40230fc:	528014a0 	mov	w0, #0xa5                  	// #165
 4023100:	f9400bfe 	ldr	x30, [sp, #16]
 4023104:	a8c253f3 	ldp	x19, x20, [sp], #32
 4023108:	d65f03c0 	ret
 402310c:	52800b40 	mov	w0, #0x5a                  	// #90
 4023110:	d65f03c0 	ret

0000000004023114 <get_board_param_index>:
 4023114:	d2800300 	mov	x0, #0x18                  	// #24
 4023118:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402311c:	f2a22040 	movk	x0, #0x1102, lsl #16
 4023120:	b9400013 	ldr	w19, [x0]
 4023124:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023128:	912a5000 	add	x0, x0, #0xa94
 402312c:	940016cb 	bl	4028c58 <log_serial_puts>
 4023130:	d35b7673 	ubfx	x19, x19, #27, #3
 4023134:	2a1303e0 	mov	w0, w19
 4023138:	940014bd 	bl	402842c <serial_put_hex>
 402313c:	2a1303e0 	mov	w0, w19
 4023140:	a8c17bf3 	ldp	x19, x30, [sp], #16
 4023144:	d65f03c0 	ret

0000000004023148 <get_data_channel_type>:
 4023148:	d2806380 	mov	x0, #0x31c                 	// #796
 402314c:	52968f01 	mov	w1, #0xb478                	// #46200
 4023150:	f2a22040 	movk	x0, #0x1102, lsl #16
 4023154:	72ad34a1 	movk	w1, #0x69a5, lsl #16
 4023158:	b9400000 	ldr	w0, [x0]
 402315c:	6b01001f 	cmp	w0, w1
 4023160:	540002e0 	b.eq	40231bc <get_data_channel_type+0x74>  // b.none
 4023164:	529690e1 	mov	w1, #0xb487                	// #46215
 4023168:	540001e8 	b.hi	40231a4 <get_data_channel_type+0x5c>  // b.pmore
 402316c:	72ad2b41 	movk	w1, #0x695a, lsl #16
 4023170:	6b01001f 	cmp	w0, w1
 4023174:	54000240 	b.eq	40231bc <get_data_channel_type+0x74>  // b.none
 4023178:	528970e1 	mov	w1, #0x4b87                	// #19335
 402317c:	72ad34a1 	movk	w1, #0x69a5, lsl #16
 4023180:	6b01001f 	cmp	w0, w1
 4023184:	540001c0 	b.eq	40231bc <get_data_channel_type+0x74>  // b.none
 4023188:	f81f0ffe 	str	x30, [sp, #-16]!
 402318c:	528007e0 	mov	w0, #0x3f                  	// #63
 4023190:	94000bde 	bl	4026108 <err_print>
 4023194:	52896f00 	mov	w0, #0x4b78                	// #19320
 4023198:	72b2cb40 	movk	w0, #0x965a, lsl #16
 402319c:	f84107fe 	ldr	x30, [sp], #16
 40231a0:	d65f03c0 	ret
 40231a4:	72b2cb41 	movk	w1, #0x965a, lsl #16
 40231a8:	6b01001f 	cmp	w0, w1
 40231ac:	54000080 	b.eq	40231bc <get_data_channel_type+0x74>  // b.none
 40231b0:	52896f01 	mov	w1, #0x4b78                	// #19320
 40231b4:	72b2d4a1 	movk	w1, #0x96a5, lsl #16
 40231b8:	17fffff2 	b	4023180 <get_data_channel_type+0x38>
 40231bc:	d65f03c0 	ret

00000000040231c0 <boot_control_get_boot_slot_num>:
 40231c0:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 40231c4:	d0fffff3 	adrp	x19, 4021000 <gs_misc_record-0xa80>
 40231c8:	912a0273 	add	x19, x19, #0xa80
 40231cc:	a9015bf5 	stp	x21, x22, [sp, #16]
 40231d0:	d0000076 	adrp	x22, 4031000 <sync_exception_sp_el0>
 40231d4:	2a0003f5 	mov	w21, w0
 40231d8:	912ac6d6 	add	x22, x22, #0xab1
 40231dc:	a90263f7 	stp	x23, x24, [sp, #32]
 40231e0:	52886857 	mov	w23, #0x4342                	// #17218
 40231e4:	52860618 	mov	w24, #0x3030                	// #12336
 40231e8:	72a84837 	movk	w23, #0x4241, lsl #16
 40231ec:	72a60638 	movk	w24, #0x3031, lsl #16
 40231f0:	a9037bf9 	stp	x25, x30, [sp, #48]
 40231f4:	52800019 	mov	w25, #0x0                   	// #0
 40231f8:	aa1303e3 	mov	x3, x19
 40231fc:	12000322 	and	w2, w25, #0x1
 4023200:	d3412321 	ubfx	x1, x25, #1, #8
 4023204:	2a1503e0 	mov	w0, w21
 4023208:	9400072b 	bl	4024eb4 <get_misc_data_from_flash>
 402320c:	2a0003f4 	mov	w20, w0
 4023210:	7102941f 	cmp	w0, #0xa5
 4023214:	540001c0 	b.eq	402324c <boot_control_get_boot_slot_num+0x8c>  // b.none
 4023218:	aa1603e0 	mov	x0, x22
 402321c:	9400168f 	bl	4028c58 <log_serial_puts>
 4023220:	11000739 	add	w25, w25, #0x1
 4023224:	12001f39 	and	w25, w25, #0xff
 4023228:	7100133f 	cmp	w25, #0x4
 402322c:	54fffe61 	b.ne	40231f8 <boot_control_get_boot_slot_num+0x38>  // b.any
 4023230:	52800b54 	mov	w20, #0x5a                  	// #90
 4023234:	2a1403e0 	mov	w0, w20
 4023238:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402323c:	a94263f7 	ldp	x23, x24, [sp, #32]
 4023240:	a9437bf9 	ldp	x25, x30, [sp, #48]
 4023244:	a8c453f3 	ldp	x19, x20, [sp], #64
 4023248:	d65f03c0 	ret
 402324c:	52800381 	mov	w1, #0x1c                  	// #28
 4023250:	aa1303e0 	mov	x0, x19
 4023254:	94000f11 	bl	4026e98 <crc32>
 4023258:	b9401e61 	ldr	w1, [x19, #28]
 402325c:	6b01001f 	cmp	w0, w1
 4023260:	54fffe01 	b.ne	4023220 <boot_control_get_boot_slot_num+0x60>  // b.any
 4023264:	b9400260 	ldr	w0, [x19]
 4023268:	6b17001f 	cmp	w0, w23
 402326c:	54fffda1 	b.ne	4023220 <boot_control_get_boot_slot_num+0x60>  // b.any
 4023270:	b9400660 	ldr	w0, [x19, #4]
 4023274:	6b17001f 	cmp	w0, w23
 4023278:	54fffd41 	b.ne	4023220 <boot_control_get_boot_slot_num+0x60>  // b.any
 402327c:	b9400a60 	ldr	w0, [x19, #8]
 4023280:	6b18001f 	cmp	w0, w24
 4023284:	54fffce1 	b.ne	4023220 <boot_control_get_boot_slot_num+0x60>  // b.any
 4023288:	39403260 	ldrb	w0, [x19, #12]
 402328c:	71003c1f 	cmp	w0, #0xf
 4023290:	540004c1 	b.ne	4023328 <boot_control_get_boot_slot_num+0x168>  // b.any
 4023294:	39403a60 	ldrb	w0, [x19, #14]
 4023298:	34000720 	cbz	w0, 402337c <boot_control_get_boot_slot_num+0x1bc>
 402329c:	39404260 	ldrb	w0, [x19, #16]
 40232a0:	7100381f 	cmp	w0, #0xe
 40232a4:	540006c1 	b.ne	402337c <boot_control_get_boot_slot_num+0x1bc>  // b.any
 40232a8:	39404a60 	ldrb	w0, [x19, #18]
 40232ac:	34000680 	cbz	w0, 402337c <boot_control_get_boot_slot_num+0x1bc>
 40232b0:	39404660 	ldrb	w0, [x19, #17]
 40232b4:	34000640 	cbz	w0, 402337c <boot_control_get_boot_slot_num+0x1bc>
 40232b8:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40232bc:	393ce01f 	strb	wzr, [x0, #3896]
 40232c0:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40232c4:	397ce000 	ldrb	w0, [x0, #3896]
 40232c8:	8b000a60 	add	x0, x19, x0, lsl #2
 40232cc:	39403401 	ldrb	w1, [x0, #13]
 40232d0:	350006e1 	cbnz	w1, 40233ac <boot_control_get_boot_slot_num+0x1ec>
 40232d4:	39403801 	ldrb	w1, [x0, #14]
 40232d8:	51000421 	sub	w1, w1, #0x1
 40232dc:	39003801 	strb	w1, [x0, #14]
 40232e0:	aa1303e0 	mov	x0, x19
 40232e4:	52800381 	mov	w1, #0x1c                  	// #28
 40232e8:	94000eec 	bl	4026e98 <crc32>
 40232ec:	b9001e60 	str	w0, [x19, #28]
 40232f0:	7100073f 	cmp	w25, #0x1
 40232f4:	54000608 	b.hi	40233b4 <boot_control_get_boot_slot_num+0x1f4>  // b.pmore
 40232f8:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40232fc:	912b2800 	add	x0, x0, #0xaca
 4023300:	94001656 	bl	4028c58 <log_serial_puts>
 4023304:	aa1303e2 	mov	x2, x19
 4023308:	2a1503e0 	mov	w0, w21
 402330c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4023310:	52800043 	mov	w3, #0x2                   	// #2
 4023314:	a94263f7 	ldp	x23, x24, [sp, #32]
 4023318:	52800001 	mov	w1, #0x0                   	// #0
 402331c:	a9437bf9 	ldp	x25, x30, [sp, #48]
 4023320:	a8c453f3 	ldp	x19, x20, [sp], #64
 4023324:	14000729 	b	4024fc8 <write_misc_data_to_flash>
 4023328:	7100381f 	cmp	w0, #0xe
 402332c:	54000281 	b.ne	402337c <boot_control_get_boot_slot_num+0x1bc>  // b.any
 4023330:	39403a60 	ldrb	w0, [x19, #14]
 4023334:	34000240 	cbz	w0, 402337c <boot_control_get_boot_slot_num+0x1bc>
 4023338:	39403660 	ldrb	w0, [x19, #13]
 402333c:	34000140 	cbz	w0, 4023364 <boot_control_get_boot_slot_num+0x1a4>
 4023340:	39404260 	ldrb	w0, [x19, #16]
 4023344:	71003c1f 	cmp	w0, #0xf
 4023348:	540000e1 	b.ne	4023364 <boot_control_get_boot_slot_num+0x1a4>  // b.any
 402334c:	39404a60 	ldrb	w0, [x19, #18]
 4023350:	340000a0 	cbz	w0, 4023364 <boot_control_get_boot_slot_num+0x1a4>
 4023354:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023358:	52800021 	mov	w1, #0x1                   	// #1
 402335c:	393ce001 	strb	w1, [x0, #3896]
 4023360:	17ffffd8 	b	40232c0 <boot_control_get_boot_slot_num+0x100>
 4023364:	39403660 	ldrb	w0, [x19, #13]
 4023368:	340000a0 	cbz	w0, 402337c <boot_control_get_boot_slot_num+0x1bc>
 402336c:	b9401260 	ldr	w0, [x19, #16]
 4023370:	12005c00 	and	w0, w0, #0xffffff
 4023374:	71003c1f 	cmp	w0, #0xf
 4023378:	54fffa00 	b.eq	40232b8 <boot_control_get_boot_slot_num+0xf8>  // b.none
 402337c:	b9400e60 	ldr	w0, [x19, #12]
 4023380:	12005c00 	and	w0, w0, #0xffffff
 4023384:	71003c1f 	cmp	w0, #0xf
 4023388:	54fff4c1 	b.ne	4023220 <boot_control_get_boot_slot_num+0x60>  // b.any
 402338c:	39404260 	ldrb	w0, [x19, #16]
 4023390:	7100381f 	cmp	w0, #0xe
 4023394:	54fff461 	b.ne	4023220 <boot_control_get_boot_slot_num+0x60>  // b.any
 4023398:	39404a60 	ldrb	w0, [x19, #18]
 402339c:	34fff420 	cbz	w0, 4023220 <boot_control_get_boot_slot_num+0x60>
 40233a0:	39404660 	ldrb	w0, [x19, #17]
 40233a4:	34fff3e0 	cbz	w0, 4023220 <boot_control_get_boot_slot_num+0x60>
 40233a8:	17ffffeb 	b	4023354 <boot_control_get_boot_slot_num+0x194>
 40233ac:	7100073f 	cmp	w25, #0x1
 40233b0:	54fff429 	b.ls	4023234 <boot_control_get_boot_slot_num+0x74>  // b.plast
 40233b4:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40233b8:	912b9000 	add	x0, x0, #0xae4
 40233bc:	94001627 	bl	4028c58 <log_serial_puts>
 40233c0:	aa1303e2 	mov	x2, x19
 40233c4:	2a1503e0 	mov	w0, w21
 40233c8:	52800043 	mov	w3, #0x2                   	// #2
 40233cc:	52800021 	mov	w1, #0x1                   	// #1
 40233d0:	940006fe 	bl	4024fc8 <write_misc_data_to_flash>
 40233d4:	2a0003f4 	mov	w20, w0
 40233d8:	7102941f 	cmp	w0, #0xa5
 40233dc:	54fff2c1 	b.ne	4023234 <boot_control_get_boot_slot_num+0x74>  // b.any
 40233e0:	17ffffc6 	b	40232f8 <boot_control_get_boot_slot_num+0x138>

00000000040233e4 <boot_control_get_active_slot>:
 40233e4:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40233e8:	397ce000 	ldrb	w0, [x0, #3896]
 40233ec:	d65f03c0 	ret

00000000040233f0 <boot_control_get_reset_state>:
 40233f0:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40233f4:	397ce001 	ldrb	w1, [x0, #3896]
 40233f8:	7103fc3f 	cmp	w1, #0xff
 40233fc:	540001a0 	b.eq	4023430 <boot_control_get_reset_state+0x40>  // b.none
 4023400:	d0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4023404:	912a0000 	add	x0, x0, #0xa80
 4023408:	39403802 	ldrb	w2, [x0, #14]
 402340c:	39404803 	ldrb	w3, [x0, #18]
 4023410:	2a030042 	orr	w2, w2, w3
 4023414:	72001c5f 	tst	w2, #0xff
 4023418:	540000c0 	b.eq	4023430 <boot_control_get_reset_state+0x40>  // b.none
 402341c:	8b21c800 	add	x0, x0, w1, sxtw #2
 4023420:	39403400 	ldrb	w0, [x0, #13]
 4023424:	7100001f 	cmp	w0, #0x0
 4023428:	1a9f07e0 	cset	w0, ne  // ne = any
 402342c:	d65f03c0 	ret
 4023430:	52800020 	mov	w0, #0x1                   	// #1
 4023434:	17fffffe 	b	402342c <boot_control_get_reset_state+0x3c>

0000000004023438 <check_msid_ext>:
 4023438:	d100c3ff 	sub	sp, sp, #0x30
 402343c:	d2800002 	mov	x2, #0x0                   	// #0
 4023440:	a90153f3 	stp	x19, x20, [sp, #16]
 4023444:	2a0003f3 	mov	w19, w0
 4023448:	2a0103f4 	mov	w20, w1
 402344c:	f90013fe 	str	x30, [sp, #32]
 4023450:	d0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4023454:	f9477001 	ldr	x1, [x0, #3808]
 4023458:	f90007e1 	str	x1, [sp, #8]
 402345c:	d2800001 	mov	x1, #0x0                   	// #0
 4023460:	52800600 	mov	w0, #0x30                  	// #48
 4023464:	910013e1 	add	x1, sp, #0x4
 4023468:	9400159c 	bl	4028ad8 <uapi_drv_otp_read_word>
 402346c:	7102941f 	cmp	w0, #0xa5
 4023470:	54000160 	b.eq	402349c <check_msid_ext+0x64>  // b.none
 4023474:	52800bc0 	mov	w0, #0x5e                  	// #94
 4023478:	94000b24 	bl	4026108 <err_print>
 402347c:	52800b40 	mov	w0, #0x5a                  	// #90
 4023480:	d0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4023484:	f94007e2 	ldr	x2, [sp, #8]
 4023488:	f9477023 	ldr	x3, [x1, #3808]
 402348c:	eb030042 	subs	x2, x2, x3
 4023490:	d2800003 	mov	x3, #0x0                   	// #0
 4023494:	54000100 	b.eq	40234b4 <check_msid_ext+0x7c>  // b.none
 4023498:	94000b59 	bl	40261fc <__stack_chk_fail>
 402349c:	b94007e1 	ldr	w1, [sp, #4]
 40234a0:	4a010273 	eor	w19, w19, w1
 40234a4:	6a14027f 	tst	w19, w20
 40234a8:	54fffec0 	b.eq	4023480 <check_msid_ext+0x48>  // b.none
 40234ac:	52800c40 	mov	w0, #0x62                  	// #98
 40234b0:	17fffff2 	b	4023478 <check_msid_ext+0x40>
 40234b4:	a94153f3 	ldp	x19, x20, [sp, #16]
 40234b8:	f94013fe 	ldr	x30, [sp, #32]
 40234bc:	9100c3ff 	add	sp, sp, #0x30
 40234c0:	d65f03c0 	ret

00000000040234c4 <die_id_compare>:
 40234c4:	d10103ff 	sub	sp, sp, #0x40
 40234c8:	d2800203 	mov	x3, #0x10                  	// #16
 40234cc:	d2800004 	mov	x4, #0x0                   	// #0
 40234d0:	52800002 	mov	w2, #0x0                   	// #0
 40234d4:	a90253f3 	stp	x19, x20, [sp, #32]
 40234d8:	aa0003f4 	mov	x20, x0
 40234dc:	d0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40234e0:	f9001bfe 	str	x30, [sp, #48]
 40234e4:	52800b53 	mov	w19, #0x5a                  	// #90
 40234e8:	f9477001 	ldr	x1, [x0, #3808]
 40234ec:	f9000fe1 	str	x1, [sp, #24]
 40234f0:	d2800001 	mov	x1, #0x0                   	// #0
 40234f4:	910023e0 	add	x0, sp, #0x8
 40234f8:	aa0303e1 	mov	x1, x3
 40234fc:	b90007f3 	str	w19, [sp, #4]
 4023500:	9400160a 	bl	4028d28 <memset_ss>
 4023504:	b90007e0 	str	w0, [sp, #4]
 4023508:	b94007e0 	ldr	w0, [sp, #4]
 402350c:	7102941f 	cmp	w0, #0xa5
 4023510:	54000160 	b.eq	402353c <die_id_compare+0x78>  // b.none
 4023514:	52800a20 	mov	w0, #0x51                  	// #81
 4023518:	94000afc 	bl	4026108 <err_print>
 402351c:	2a1303e0 	mov	w0, w19
 4023520:	d0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4023524:	f9400fe2 	ldr	x2, [sp, #24]
 4023528:	f9477023 	ldr	x3, [x1, #3808]
 402352c:	eb030042 	subs	x2, x2, x3
 4023530:	d2800003 	mov	x3, #0x0                   	// #0
 4023534:	540002c0 	b.eq	402358c <die_id_compare+0xc8>  // b.none
 4023538:	94000b31 	bl	40261fc <__stack_chk_fail>
 402353c:	910023e0 	add	x0, sp, #0x8
 4023540:	d2800002 	mov	x2, #0x0                   	// #0
 4023544:	52800201 	mov	w1, #0x10                  	// #16
 4023548:	9400156c 	bl	4028af8 <uapi_drv_otp_get_die_id>
 402354c:	b94007e0 	ldr	w0, [sp, #4]
 4023550:	7102941f 	cmp	w0, #0xa5
 4023554:	54000060 	b.eq	4023560 <die_id_compare+0x9c>  // b.none
 4023558:	52800c60 	mov	w0, #0x63                  	// #99
 402355c:	17ffffef 	b	4023518 <die_id_compare+0x54>
 4023560:	910023e1 	add	x1, sp, #0x8
 4023564:	aa1403e0 	mov	x0, x20
 4023568:	d2800003 	mov	x3, #0x0                   	// #0
 402356c:	d2800202 	mov	x2, #0x10                  	// #16
 4023570:	940015e6 	bl	4028d08 <memcmp_ss>
 4023574:	b90007e0 	str	w0, [sp, #4]
 4023578:	b94007e0 	ldr	w0, [sp, #4]
 402357c:	7102941f 	cmp	w0, #0xa5
 4023580:	54fffd00 	b.eq	4023520 <die_id_compare+0x5c>  // b.none
 4023584:	52800c80 	mov	w0, #0x64                  	// #100
 4023588:	17ffffe4 	b	4023518 <die_id_compare+0x54>
 402358c:	a94253f3 	ldp	x19, x20, [sp, #32]
 4023590:	f9401bfe 	ldr	x30, [sp, #48]
 4023594:	910103ff 	add	sp, sp, #0x40
 4023598:	d65f03c0 	ret

000000000402359c <count_bit_set>:
 402359c:	1e270000 	fmov	s0, w0
 40235a0:	0e205800 	cnt	v0.8b, v0.8b
 40235a4:	0e31b800 	addv	b0, v0.8b
 40235a8:	1e260000 	fmov	w0, s0
 40235ac:	d65f03c0 	ret

00000000040235b0 <check_version.constprop.0>:
 40235b0:	d10083ff 	sub	sp, sp, #0x20
 40235b4:	d2800002 	mov	x2, #0x0                   	// #0
 40235b8:	a9017bf3 	stp	x19, x30, [sp, #16]
 40235bc:	2a0003f3 	mov	w19, w0
 40235c0:	d0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40235c4:	f9477001 	ldr	x1, [x0, #3808]
 40235c8:	f90007e1 	str	x1, [sp, #8]
 40235cc:	d2800001 	mov	x1, #0x0                   	// #0
 40235d0:	52803a00 	mov	w0, #0x1d0                 	// #464
 40235d4:	910003e1 	mov	x1, sp
 40235d8:	29007fff 	stp	wzr, wzr, [sp]
 40235dc:	9400153f 	bl	4028ad8 <uapi_drv_otp_read_word>
 40235e0:	7102941f 	cmp	w0, #0xa5
 40235e4:	54000160 	b.eq	4023610 <check_version.constprop.0+0x60>  // b.none
 40235e8:	52800bc0 	mov	w0, #0x5e                  	// #94
 40235ec:	94000ac7 	bl	4026108 <err_print>
 40235f0:	52800b41 	mov	w1, #0x5a                  	// #90
 40235f4:	d0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40235f8:	f94007e2 	ldr	x2, [sp, #8]
 40235fc:	f9477003 	ldr	x3, [x0, #3808]
 4023600:	eb030042 	subs	x2, x2, x3
 4023604:	d2800003 	mov	x3, #0x0                   	// #0
 4023608:	54000260 	b.eq	4023654 <check_version.constprop.0+0xa4>  // b.none
 402360c:	94000afc 	bl	40261fc <__stack_chk_fail>
 4023610:	910013e1 	add	x1, sp, #0x4
 4023614:	d2800002 	mov	x2, #0x0                   	// #0
 4023618:	52803980 	mov	w0, #0x1cc                 	// #460
 402361c:	9400152f 	bl	4028ad8 <uapi_drv_otp_read_word>
 4023620:	2a0003e1 	mov	w1, w0
 4023624:	7102941f 	cmp	w0, #0xa5
 4023628:	54fffe01 	b.ne	40235e8 <check_version.constprop.0+0x38>  // b.any
 402362c:	b94007e0 	ldr	w0, [sp, #4]
 4023630:	97ffffdb 	bl	402359c <count_bit_set>
 4023634:	2a0003e2 	mov	w2, w0
 4023638:	b94003e0 	ldr	w0, [sp]
 402363c:	97ffffd8 	bl	402359c <count_bit_set>
 4023640:	0b000042 	add	w2, w2, w0
 4023644:	6b13005f 	cmp	w2, w19
 4023648:	54fffd69 	b.ls	40235f4 <check_version.constprop.0+0x44>  // b.plast
 402364c:	52800be0 	mov	w0, #0x5f                  	// #95
 4023650:	17ffffe7 	b	40235ec <check_version.constprop.0+0x3c>
 4023654:	2a0103e0 	mov	w0, w1
 4023658:	a9417bf3 	ldp	x19, x30, [sp, #16]
 402365c:	910083ff 	add	sp, sp, #0x20
 4023660:	d65f03c0 	ret

0000000004023664 <get_otfl_code_info_addr>:
 4023664:	52858000 	mov	w0, #0x2c00                	// #11264
 4023668:	72a08040 	movk	w0, #0x402, lsl #16
 402366c:	d65f03c0 	ret

0000000004023670 <get_otfl_code_addr>:
 4023670:	5285c000 	mov	w0, #0x2e00                	// #11776
 4023674:	72a08040 	movk	w0, #0x402, lsl #16
 4023678:	d65f03c0 	ret

000000000402367c <get_otfl_code_area_len>:
 402367c:	d2858000 	mov	x0, #0x2c00                	// #11264
 4023680:	f2a08040 	movk	x0, #0x402, lsl #16
 4023684:	b9402400 	ldr	w0, [x0, #36]
 4023688:	d65f03c0 	ret

000000000402368c <get_partition_area_addr>:
 402368c:	d2858000 	mov	x0, #0x2c00                	// #11264
 4023690:	f2a08040 	movk	x0, #0x402, lsl #16
 4023694:	b940a401 	ldr	w1, [x0, #164]
 4023698:	11080000 	add	w0, w0, #0x200
 402369c:	0b000020 	add	w0, w1, w0
 40236a0:	d65f03c0 	ret

00000000040236a4 <get_partition_area_len>:
 40236a4:	d2858000 	mov	x0, #0x2c00                	// #11264
 40236a8:	f2a08040 	movk	x0, #0x402, lsl #16
 40236ac:	b940a800 	ldr	w0, [x0, #168]
 40236b0:	d65f03c0 	ret

00000000040236b4 <get_otfl_heap_addr>:
 40236b4:	d2858000 	mov	x0, #0x2c00                	// #11264
 40236b8:	f2a08040 	movk	x0, #0x402, lsl #16
 40236bc:	b9402401 	ldr	w1, [x0, #36]
 40236c0:	11080000 	add	w0, w0, #0x200
 40236c4:	0b000020 	add	w0, w1, w0
 40236c8:	f240241f 	tst	x0, #0x3ff
 40236cc:	540000a0 	b.eq	40236e0 <get_otfl_heap_addr+0x2c>  // b.none
 40236d0:	52864000 	mov	w0, #0x3200                	// #12800
 40236d4:	72a08040 	movk	w0, #0x402, lsl #16
 40236d8:	0b000020 	add	w0, w1, w0
 40236dc:	12165400 	and	w0, w0, #0xfffffc00
 40236e0:	d65f03c0 	ret

00000000040236e4 <get_otfl_heap_len>:
 40236e4:	52900000 	mov	w0, #0x8000                	// #32768
 40236e8:	d65f03c0 	ret

00000000040236ec <get_ree_key_area_addr>:
 40236ec:	f81f0ffe 	str	x30, [sp, #-16]!
 40236f0:	97fffff1 	bl	40236b4 <get_otfl_heap_addr>
 40236f4:	11402000 	add	w0, w0, #0x8, lsl #12
 40236f8:	f84107fe 	ldr	x30, [sp], #16
 40236fc:	d65f03c0 	ret

0000000004023700 <get_ddr_param_info_addr>:
 4023700:	f81f0ffe 	str	x30, [sp, #-16]!
 4023704:	97fffffa 	bl	40236ec <get_ree_key_area_addr>
 4023708:	11040000 	add	w0, w0, #0x100
 402370c:	f84107fe 	ldr	x30, [sp], #16
 4023710:	d65f03c0 	ret

0000000004023714 <get_ddr_param_data_addr>:
 4023714:	f81f0ffe 	str	x30, [sp, #-16]!
 4023718:	97fffffa 	bl	4023700 <get_ddr_param_info_addr>
 402371c:	110c0000 	add	w0, w0, #0x300
 4023720:	f84107fe 	ldr	x30, [sp], #16
 4023724:	d65f03c0 	ret

0000000004023728 <get_uboot_info_download_ddr_addr>:
 4023728:	d2a82000 	mov	x0, #0x41000000            	// #1090519040
 402372c:	d65f03c0 	ret

0000000004023730 <get_uboot_code_ddr_addr>:
 4023730:	d2804000 	mov	x0, #0x200                 	// #512
 4023734:	f2a82000 	movk	x0, #0x4100, lsl #16
 4023738:	d65f03c0 	ret

000000000402373c <get_uboot_code_size>:
 402373c:	d2a82000 	mov	x0, #0x41000000            	// #1090519040
 4023740:	b9402400 	ldr	w0, [x0, #36]
 4023744:	d65f03c0 	ret

0000000004023748 <get_uboot_entrypoint_ddr_addr>:
 4023748:	d2a82000 	mov	x0, #0x41000000            	// #1090519040
 402374c:	b9408800 	ldr	w0, [x0, #136]
 4023750:	d65f03c0 	ret

0000000004023754 <hash_compare>:
 4023754:	d10183ff 	sub	sp, sp, #0x60
 4023758:	d2800403 	mov	x3, #0x20                  	// #32
 402375c:	d2800004 	mov	x4, #0x0                   	// #0
 4023760:	a90353f3 	stp	x19, x20, [sp, #48]
 4023764:	52800b53 	mov	w19, #0x5a                  	// #90
 4023768:	aa0203f4 	mov	x20, x2
 402376c:	a9045bf5 	stp	x21, x22, [sp, #64]
 4023770:	aa0003f5 	mov	x21, x0
 4023774:	2a0103f6 	mov	w22, w1
 4023778:	f9002bfe 	str	x30, [sp, #80]
 402377c:	d0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4023780:	2a1303e2 	mov	w2, w19
 4023784:	f9477001 	ldr	x1, [x0, #3808]
 4023788:	f90017e1 	str	x1, [sp, #40]
 402378c:	d2800001 	mov	x1, #0x0                   	// #0
 4023790:	910023e0 	add	x0, sp, #0x8
 4023794:	aa0303e1 	mov	x1, x3
 4023798:	b90007f3 	str	w19, [sp, #4]
 402379c:	94001563 	bl	4028d28 <memset_ss>
 40237a0:	b90007e0 	str	w0, [sp, #4]
 40237a4:	b94007e0 	ldr	w0, [sp, #4]
 40237a8:	7102941f 	cmp	w0, #0xa5
 40237ac:	540001c1 	b.ne	40237e4 <hash_compare+0x90>  // b.any
 40237b0:	910023e2 	add	x2, sp, #0x8
 40237b4:	2a1603e1 	mov	w1, w22
 40237b8:	2a1503e0 	mov	w0, w21
 40237bc:	52800004 	mov	w4, #0x0                   	// #0
 40237c0:	52800403 	mov	w3, #0x20                  	// #32
 40237c4:	b90007f3 	str	w19, [sp, #4]
 40237c8:	940004c8 	bl	4024ae8 <calc_hash>
 40237cc:	b90007e0 	str	w0, [sp, #4]
 40237d0:	b94007e0 	ldr	w0, [sp, #4]
 40237d4:	7102941f 	cmp	w0, #0xa5
 40237d8:	54000140 	b.eq	4023800 <hash_compare+0xac>  // b.none
 40237dc:	52800a40 	mov	w0, #0x52                  	// #82
 40237e0:	94000a4a 	bl	4026108 <err_print>
 40237e4:	d0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 40237e8:	f94017e0 	ldr	x0, [sp, #40]
 40237ec:	f9477022 	ldr	x2, [x1, #3808]
 40237f0:	eb020000 	subs	x0, x0, x2
 40237f4:	d2800002 	mov	x2, #0x0                   	// #0
 40237f8:	54000200 	b.eq	4023838 <hash_compare+0xe4>  // b.none
 40237fc:	94000a80 	bl	40261fc <__stack_chk_fail>
 4023800:	aa1403e1 	mov	x1, x20
 4023804:	910023e0 	add	x0, sp, #0x8
 4023808:	d2800003 	mov	x3, #0x0                   	// #0
 402380c:	d2800402 	mov	x2, #0x20                  	// #32
 4023810:	b90007f3 	str	w19, [sp, #4]
 4023814:	9400153d 	bl	4028d08 <memcmp_ss>
 4023818:	b90007e0 	str	w0, [sp, #4]
 402381c:	b94007e0 	ldr	w0, [sp, #4]
 4023820:	7102941f 	cmp	w0, #0xa5
 4023824:	54000060 	b.eq	4023830 <hash_compare+0xdc>  // b.none
 4023828:	52800aa0 	mov	w0, #0x55                  	// #85
 402382c:	17ffffed 	b	40237e0 <hash_compare+0x8c>
 4023830:	b94007f3 	ldr	w19, [sp, #4]
 4023834:	17ffffec 	b	40237e4 <hash_compare+0x90>
 4023838:	2a1303e0 	mov	w0, w19
 402383c:	a94353f3 	ldp	x19, x20, [sp, #48]
 4023840:	a9445bf5 	ldp	x21, x22, [sp, #64]
 4023844:	f9402bfe 	ldr	x30, [sp, #80]
 4023848:	910183ff 	add	sp, sp, #0x60
 402384c:	d65f03c0 	ret

0000000004023850 <secure_verify_area>:
 4023850:	d102c3ff 	sub	sp, sp, #0xb0
 4023854:	a90953f3 	stp	x19, x20, [sp, #144]
 4023858:	aa0003f4 	mov	x20, x0
 402385c:	d0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4023860:	a90a7bf5 	stp	x21, x30, [sp, #160]
 4023864:	52800b53 	mov	w19, #0x5a                  	// #90
 4023868:	52800415 	mov	w21, #0x20                  	// #32
 402386c:	f9477004 	ldr	x4, [x0, #3808]
 4023870:	f90047e4 	str	x4, [sp, #136]
 4023874:	d2800004 	mov	x4, #0x0                   	// #0
 4023878:	528f0b40 	mov	w0, #0x785a                	// #30810
 402387c:	b9000ff3 	str	w19, [sp, #12]
 4023880:	72b2d860 	movk	w0, #0x96c3, lsl #16
 4023884:	b9000ff3 	str	w19, [sp, #12]
 4023888:	b90013f3 	str	w19, [sp, #16]
 402388c:	b90017e0 	str	w0, [sp, #20]
 4023890:	91008060 	add	x0, x3, #0x20
 4023894:	b9001be2 	str	w2, [sp, #24]
 4023898:	f90013e1 	str	x1, [sp, #32]
 402389c:	f9002be3 	str	x3, [sp, #80]
 40238a0:	f9002fe0 	str	x0, [sp, #88]
 40238a4:	b90063f5 	str	w21, [sp, #96]
 40238a8:	b40000f4 	cbz	x20, 40238c4 <secure_verify_area+0x74>
 40238ac:	b40000c3 	cbz	x3, 40238c4 <secure_verify_area+0x74>
 40238b0:	94001496 	bl	4028b08 <uapi_drv_pke_init>
 40238b4:	7102941f 	cmp	w0, #0xa5
 40238b8:	54000240 	b.eq	4023900 <secure_verify_area+0xb0>  // b.none
 40238bc:	52800b20 	mov	w0, #0x59                  	// #89
 40238c0:	94000a12 	bl	4026108 <err_print>
 40238c4:	b9000ff3 	str	w19, [sp, #12]
 40238c8:	b9400fe0 	ldr	w0, [sp, #12]
 40238cc:	7102941f 	cmp	w0, #0xa5
 40238d0:	54000761 	b.ne	40239bc <secure_verify_area+0x16c>  // b.any
 40238d4:	b9400fe0 	ldr	w0, [sp, #12]
 40238d8:	7102941f 	cmp	w0, #0xa5
 40238dc:	54000701 	b.ne	40239bc <secure_verify_area+0x16c>  // b.any
 40238e0:	b9400fe0 	ldr	w0, [sp, #12]
 40238e4:	d0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 40238e8:	f94047e2 	ldr	x2, [sp, #136]
 40238ec:	f9477023 	ldr	x3, [x1, #3808]
 40238f0:	eb030042 	subs	x2, x2, x3
 40238f4:	d2800003 	mov	x3, #0x0                   	// #0
 40238f8:	54000660 	b.eq	40239c4 <secure_verify_area+0x174>  // b.none
 40238fc:	94000a40 	bl	40261fc <__stack_chk_fail>
 4023900:	9101a3f3 	add	x19, sp, #0x68
 4023904:	a90353f3 	stp	x19, x20, [sp, #48]
 4023908:	91008294 	add	x20, x20, #0x20
 402390c:	f90023f4 	str	x20, [sp, #64]
 4023910:	528f0bb4 	mov	w20, #0x785d                	// #30813
 4023914:	9100a3e3 	add	x3, sp, #0x28
 4023918:	910143e2 	add	x2, sp, #0x50
 402391c:	9100e3e1 	add	x1, sp, #0x38
 4023920:	910063e0 	add	x0, sp, #0x18
 4023924:	52800004 	mov	w4, #0x0                   	// #0
 4023928:	72b2d874 	movk	w20, #0x96c3, lsl #16
 402392c:	b9002bf5 	str	w21, [sp, #40]
 4023930:	b9004bf5 	str	w21, [sp, #72]
 4023934:	940004c8 	bl	4024c54 <verify_signature>
 4023938:	b90013e0 	str	w0, [sp, #16]
 402393c:	b94013e0 	ldr	w0, [sp, #16]
 4023940:	7102941f 	cmp	w0, #0xa5
 4023944:	54000261 	b.ne	4023990 <secure_verify_area+0x140>  // b.any
 4023948:	f9402be0 	ldr	x0, [sp, #80]
 402394c:	aa1303e1 	mov	x1, x19
 4023950:	d2800003 	mov	x3, #0x0                   	// #0
 4023954:	d2800402 	mov	x2, #0x20                  	// #32
 4023958:	940014ec 	bl	4028d08 <memcmp_ss>
 402395c:	b90013e0 	str	w0, [sp, #16]
 4023960:	b94013e0 	ldr	w0, [sp, #16]
 4023964:	7102941f 	cmp	w0, #0xa5
 4023968:	54000180 	b.eq	4023998 <secure_verify_area+0x148>  // b.none
 402396c:	52800a80 	mov	w0, #0x54                  	// #84
 4023970:	940009e6 	bl	4026108 <err_print>
 4023974:	94001469 	bl	4028b18 <uapi_drv_pke_deinit>
 4023978:	7102941f 	cmp	w0, #0xa5
 402397c:	540001c0 	b.eq	40239b4 <secure_verify_area+0x164>  // b.none
 4023980:	52800b60 	mov	w0, #0x5b                  	// #91
 4023984:	52800b53 	mov	w19, #0x5a                  	// #90
 4023988:	940009e0 	bl	4026108 <err_print>
 402398c:	17ffffce 	b	40238c4 <secure_verify_area+0x74>
 4023990:	52800b40 	mov	w0, #0x5a                  	// #90
 4023994:	17fffff7 	b	4023970 <secure_verify_area+0x120>
 4023998:	b94017e0 	ldr	w0, [sp, #20]
 402399c:	11000400 	add	w0, w0, #0x1
 40239a0:	b90017e0 	str	w0, [sp, #20]
 40239a4:	b94017e0 	ldr	w0, [sp, #20]
 40239a8:	6b14001f 	cmp	w0, w20
 40239ac:	54fffc89 	b.ls	402393c <secure_verify_area+0xec>  // b.plast
 40239b0:	17fffff1 	b	4023974 <secure_verify_area+0x124>
 40239b4:	b94013f3 	ldr	w19, [sp, #16]
 40239b8:	17ffffc3 	b	40238c4 <secure_verify_area+0x74>
 40239bc:	52800b40 	mov	w0, #0x5a                  	// #90
 40239c0:	17ffffc9 	b	40238e4 <secure_verify_area+0x94>
 40239c4:	a94953f3 	ldp	x19, x20, [sp, #144]
 40239c8:	a94a7bf5 	ldp	x21, x30, [sp, #160]
 40239cc:	9102c3ff 	add	sp, sp, #0xb0
 40239d0:	d65f03c0 	ret

00000000040239d4 <lpds_hash_cmp>:
 40239d4:	d10143ff 	sub	sp, sp, #0x50
 40239d8:	a90353f3 	stp	x19, x20, [sp, #48]
 40239dc:	aa0103f3 	mov	x19, x1
 40239e0:	2a0203f4 	mov	w20, w2
 40239e4:	f90023fe 	str	x30, [sp, #64]
 40239e8:	d0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 40239ec:	f9477022 	ldr	x2, [x1, #3808]
 40239f0:	f90017e2 	str	x2, [sp, #40]
 40239f4:	d2800002 	mov	x2, #0x0                   	// #0
 40239f8:	2a0003e1 	mov	w1, w0
 40239fc:	52800402 	mov	w2, #0x20                  	// #32
 4023a00:	910023e0 	add	x0, sp, #0x8
 4023a04:	9400092f 	bl	4025ec0 <lpds_hash_get>
 4023a08:	7102941f 	cmp	w0, #0xa5
 4023a0c:	54000141 	b.ne	4023a34 <lpds_hash_cmp+0x60>  // b.any
 4023a10:	910023e2 	add	x2, sp, #0x8
 4023a14:	2a1403e1 	mov	w1, w20
 4023a18:	aa1303e0 	mov	x0, x19
 4023a1c:	97ffff4e 	bl	4023754 <hash_compare>
 4023a20:	7102941f 	cmp	w0, #0xa5
 4023a24:	540000c0 	b.eq	4023a3c <lpds_hash_cmp+0x68>  // b.none
 4023a28:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023a2c:	912bf800 	add	x0, x0, #0xafe
 4023a30:	9400148a 	bl	4028c58 <log_serial_puts>
 4023a34:	52800b40 	mov	w0, #0x5a                  	// #90
 4023a38:	14000007 	b	4023a54 <lpds_hash_cmp+0x80>
 4023a3c:	910023e2 	add	x2, sp, #0x8
 4023a40:	2a1403e1 	mov	w1, w20
 4023a44:	aa1303e0 	mov	x0, x19
 4023a48:	97ffff43 	bl	4023754 <hash_compare>
 4023a4c:	7102941f 	cmp	w0, #0xa5
 4023a50:	54fffec1 	b.ne	4023a28 <lpds_hash_cmp+0x54>  // b.any
 4023a54:	d0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4023a58:	f94017e2 	ldr	x2, [sp, #40]
 4023a5c:	f9477023 	ldr	x3, [x1, #3808]
 4023a60:	eb030042 	subs	x2, x2, x3
 4023a64:	d2800003 	mov	x3, #0x0                   	// #0
 4023a68:	54000040 	b.eq	4023a70 <lpds_hash_cmp+0x9c>  // b.none
 4023a6c:	940009e4 	bl	40261fc <__stack_chk_fail>
 4023a70:	a94353f3 	ldp	x19, x20, [sp, #48]
 4023a74:	f94023fe 	ldr	x30, [sp, #64]
 4023a78:	910143ff 	add	sp, sp, #0x50
 4023a7c:	d65f03c0 	ret

0000000004023a80 <get_ree_verify_enable_flag>:
 4023a80:	d2902a80 	mov	x0, #0x8154                	// #33108
 4023a84:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4023a88:	b9400000 	ldr	w0, [x0]
 4023a8c:	d3483c00 	ubfx	x0, x0, #8, #8
 4023a90:	d65f03c0 	ret

0000000004023a94 <get_tee_verify_enable_flag>:
 4023a94:	d2902a80 	mov	x0, #0x8154                	// #33108
 4023a98:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4023a9c:	b9400000 	ldr	w0, [x0]
 4023aa0:	d65f03c0 	ret

0000000004023aa4 <get_tp_verify_enable_flag>:
 4023aa4:	d2902a80 	mov	x0, #0x8154                	// #33108
 4023aa8:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4023aac:	b9400000 	ldr	w0, [x0]
 4023ab0:	d3505c00 	ubfx	x0, x0, #16, #8
 4023ab4:	d65f03c0 	ret

0000000004023ab8 <handle_ree_key_area>:
 4023ab8:	d2902a81 	mov	x1, #0x8154                	// #33108
 4023abc:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4023ac0:	f2a203c1 	movk	x1, #0x101e, lsl #16
 4023ac4:	b9400020 	ldr	w0, [x1]
 4023ac8:	f9000bfe 	str	x30, [sp, #16]
 4023acc:	d3483c00 	ubfx	x0, x0, #8, #8
 4023ad0:	7101081f 	cmp	w0, #0x42
 4023ad4:	540000e1 	b.ne	4023af0 <handle_ree_key_area+0x38>  // b.any
 4023ad8:	b9400020 	ldr	w0, [x1]
 4023adc:	d3483c00 	ubfx	x0, x0, #8, #8
 4023ae0:	7101081f 	cmp	w0, #0x42
 4023ae4:	54000a00 	b.eq	4023c24 <handle_ree_key_area+0x16c>  // b.none
 4023ae8:	52800b53 	mov	w19, #0x5a                  	// #90
 4023aec:	1400002d 	b	4023ba0 <handle_ree_key_area+0xe8>
 4023af0:	97fffeff 	bl	40236ec <get_ree_key_area_addr>
 4023af4:	2a0003f4 	mov	w20, w0
 4023af8:	528783c0 	mov	w0, #0x3c1e                	// #15390
 4023afc:	72a963c0 	movk	w0, #0x4b1e, lsl #16
 4023b00:	b9400281 	ldr	w1, [x20]
 4023b04:	6b00003f 	cmp	w1, w0
 4023b08:	54000301 	b.ne	4023b68 <handle_ree_key_area+0xb0>  // b.any
 4023b0c:	940008d6 	bl	4025e64 <get_low_power_mode>
 4023b10:	7101681f 	cmp	w0, #0x5a
 4023b14:	54000300 	b.eq	4023b74 <handle_ree_key_area+0xbc>  // b.none
 4023b18:	d2902a01 	mov	x1, #0x8150                	// #33104
 4023b1c:	f2a203c1 	movk	x1, #0x101e, lsl #16
 4023b20:	b9400020 	ldr	w0, [x1]
 4023b24:	12000c00 	and	w0, w0, #0xf
 4023b28:	7100141f 	cmp	w0, #0x5
 4023b2c:	54000421 	b.ne	4023bb0 <handle_ree_key_area+0xf8>  // b.any
 4023b30:	b9400020 	ldr	w0, [x1]
 4023b34:	12000c00 	and	w0, w0, #0xf
 4023b38:	7100141f 	cmp	w0, #0x5
 4023b3c:	540003a1 	b.ne	4023bb0 <handle_ree_key_area+0xf8>  // b.any
 4023b40:	d2834800 	mov	x0, #0x1a40                	// #6720
 4023b44:	91030283 	add	x3, x20, #0xc0
 4023b48:	aa1403e1 	mov	x1, x20
 4023b4c:	52801802 	mov	w2, #0xc0                  	// #192
 4023b50:	f2a08040 	movk	x0, #0x402, lsl #16
 4023b54:	97ffff3f 	bl	4023850 <secure_verify_area>
 4023b58:	7102941f 	cmp	w0, #0xa5
 4023b5c:	540002e0 	b.eq	4023bb8 <handle_ree_key_area+0x100>  // b.none
 4023b60:	52800ba0 	mov	w0, #0x5d                  	// #93
 4023b64:	14000002 	b	4023b6c <handle_ree_key_area+0xb4>
 4023b68:	528008a0 	mov	w0, #0x45                  	// #69
 4023b6c:	94000967 	bl	4026108 <err_print>
 4023b70:	17ffffde 	b	4023ae8 <handle_ree_key_area+0x30>
 4023b74:	52801600 	mov	w0, #0xb0                  	// #176
 4023b78:	aa1403e1 	mov	x1, x20
 4023b7c:	52801802 	mov	w2, #0xc0                  	// #192
 4023b80:	72a22240 	movk	w0, #0x1112, lsl #16
 4023b84:	97ffff94 	bl	40239d4 <lpds_hash_cmp>
 4023b88:	2a0003f3 	mov	w19, w0
 4023b8c:	7102941f 	cmp	w0, #0xa5
 4023b90:	54fffac1 	b.ne	4023ae8 <handle_ree_key_area+0x30>  // b.any
 4023b94:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023b98:	912c3800 	add	x0, x0, #0xb0e
 4023b9c:	9400142f 	bl	4028c58 <log_serial_puts>
 4023ba0:	2a1303e0 	mov	w0, w19
 4023ba4:	f9400bfe 	ldr	x30, [sp, #16]
 4023ba8:	a8c253f3 	ldp	x19, x20, [sp], #32
 4023bac:	d65f03c0 	ret
 4023bb0:	52800ac0 	mov	w0, #0x56                  	// #86
 4023bb4:	17ffffee 	b	4023b6c <handle_ree_key_area+0xb4>
 4023bb8:	b9402680 	ldr	w0, [x20, #36]
 4023bbc:	97fffe7d 	bl	40235b0 <check_version.constprop.0>
 4023bc0:	2a0003f3 	mov	w19, w0
 4023bc4:	7102941f 	cmp	w0, #0xa5
 4023bc8:	54000201 	b.ne	4023c08 <handle_ree_key_area+0x150>  // b.any
 4023bcc:	29458680 	ldp	w0, w1, [x20, #44]
 4023bd0:	97fffe1a 	bl	4023438 <check_msid_ext>
 4023bd4:	7102941f 	cmp	w0, #0xa5
 4023bd8:	54fff881 	b.ne	4023ae8 <handle_ree_key_area+0x30>  // b.any
 4023bdc:	b9403681 	ldr	w1, [x20, #52]
 4023be0:	5292dc20 	mov	w0, #0x96e1                	// #38625
 4023be4:	72a78f00 	movk	w0, #0x3c78, lsl #16
 4023be8:	6b00003f 	cmp	w1, w0
 4023bec:	54000120 	b.eq	4023c10 <handle_ree_key_area+0x158>  // b.none
 4023bf0:	d2801600 	mov	x0, #0xb0                  	// #176
 4023bf4:	f2a22240 	movk	x0, #0x1112, lsl #16
 4023bf8:	940003fc 	bl	4024be8 <store_hash_to_lpds>
 4023bfc:	52802000 	mov	w0, #0x100                 	// #256
 4023c00:	97fffd0d 	bl	4023034 <add_boot_img_size>
 4023c04:	17ffffe7 	b	4023ba0 <handle_ree_key_area+0xe8>
 4023c08:	52800c20 	mov	w0, #0x61                  	// #97
 4023c0c:	17ffffd8 	b	4023b6c <handle_ree_key_area+0xb4>
 4023c10:	9100e280 	add	x0, x20, #0x38
 4023c14:	97fffe2c 	bl	40234c4 <die_id_compare>
 4023c18:	7102941f 	cmp	w0, #0xa5
 4023c1c:	54fff661 	b.ne	4023ae8 <handle_ree_key_area+0x30>  // b.any
 4023c20:	17fffff4 	b	4023bf0 <handle_ree_key_area+0x138>
 4023c24:	528014b3 	mov	w19, #0xa5                  	// #165
 4023c28:	17ffffde 	b	4023ba0 <handle_ree_key_area+0xe8>

0000000004023c2c <get_ddr_param_len>:
 4023c2c:	f81f0ffe 	str	x30, [sp, #-16]!
 4023c30:	97fffeb4 	bl	4023700 <get_ddr_param_info_addr>
 4023c34:	2a0003e0 	mov	w0, w0
 4023c38:	f84107fe 	ldr	x30, [sp], #16
 4023c3c:	b9402400 	ldr	w0, [x0, #36]
 4023c40:	d65f03c0 	ret

0000000004023c44 <get_ddr_param_data_end_addr>:
 4023c44:	f81f0ffe 	str	x30, [sp, #-16]!
 4023c48:	97fffeb3 	bl	4023714 <get_ddr_param_data_addr>
 4023c4c:	2a0003e2 	mov	w2, w0
 4023c50:	97fffff7 	bl	4023c2c <get_ddr_param_len>
 4023c54:	0b000040 	add	w0, w2, w0
 4023c58:	f240241f 	tst	x0, #0x3ff
 4023c5c:	54000060 	b.eq	4023c68 <get_ddr_param_data_end_addr+0x24>  // b.none
 4023c60:	11100000 	add	w0, w0, #0x400
 4023c64:	12165400 	and	w0, w0, #0xfffffc00
 4023c68:	f84107fe 	ldr	x30, [sp], #16
 4023c6c:	d65f03c0 	ret

0000000004023c70 <get_ddr_param_cnt>:
 4023c70:	f81f0ffe 	str	x30, [sp, #-16]!
 4023c74:	97fffea3 	bl	4023700 <get_ddr_param_info_addr>
 4023c78:	2a0003e0 	mov	w0, w0
 4023c7c:	f84107fe 	ldr	x30, [sp], #16
 4023c80:	b9402800 	ldr	w0, [x0, #40]
 4023c84:	d65f03c0 	ret

0000000004023c88 <handle_ddr_param_info>:
 4023c88:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4023c8c:	2a0003f3 	mov	w19, w0
 4023c90:	a9017bf5 	stp	x21, x30, [sp, #16]
 4023c94:	97fffe9b 	bl	4023700 <get_ddr_param_info_addr>
 4023c98:	2a0003f4 	mov	w20, w0
 4023c9c:	97fffff5 	bl	4023c70 <get_ddr_param_cnt>
 4023ca0:	7100201f 	cmp	w0, #0x8
 4023ca4:	54000129 	b.ls	4023cc8 <handle_ddr_param_info+0x40>  // b.plast
 4023ca8:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023cac:	912c9000 	add	x0, x0, #0xb24
 4023cb0:	940013ea 	bl	4028c58 <log_serial_puts>
 4023cb4:	52800100 	mov	w0, #0x8                   	// #8
 4023cb8:	940011dd 	bl	402842c <serial_put_hex>
 4023cbc:	52800ca0 	mov	w0, #0x65                  	// #101
 4023cc0:	94000912 	bl	4026108 <err_print>
 4023cc4:	1400000b 	b	4023cf0 <handle_ddr_param_info+0x68>
 4023cc8:	8b334293 	add	x19, x20, w19, uxtw
 4023ccc:	2a0003f5 	mov	w21, w0
 4023cd0:	3944b260 	ldrb	w0, [x19, #300]
 4023cd4:	7103fc1f 	cmp	w0, #0xff
 4023cd8:	54000101 	b.ne	4023cf8 <handle_ddr_param_info+0x70>  // b.any
 4023cdc:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023ce0:	912cd800 	add	x0, x0, #0xb36
 4023ce4:	940013dd 	bl	4028c58 <log_serial_puts>
 4023ce8:	3944b260 	ldrb	w0, [x19, #300]
 4023cec:	940011d0 	bl	402842c <serial_put_hex>
 4023cf0:	52800b53 	mov	w19, #0x5a                  	// #90
 4023cf4:	14000036 	b	4023dcc <handle_ddr_param_info+0x144>
 4023cf8:	6b0002bf 	cmp	w21, w0
 4023cfc:	540000c8 	b.hi	4023d14 <handle_ddr_param_info+0x8c>  // b.pmore
 4023d00:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023d04:	912d8400 	add	x0, x0, #0xb61
 4023d08:	940013d4 	bl	4028c58 <log_serial_puts>
 4023d0c:	2a1503e0 	mov	w0, w21
 4023d10:	17ffffea 	b	4023cb8 <handle_ddr_param_info+0x30>
 4023d14:	52878013 	mov	w19, #0x3c00                	// #15360
 4023d18:	97ffffc5 	bl	4023c2c <get_ddr_param_len>
 4023d1c:	6b13001f 	cmp	w0, w19
 4023d20:	54000109 	b.ls	4023d40 <handle_ddr_param_info+0xb8>  // b.plast
 4023d24:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023d28:	912dcc00 	add	x0, x0, #0xb73
 4023d2c:	940013cb 	bl	4028c58 <log_serial_puts>
 4023d30:	2a1303e0 	mov	w0, w19
 4023d34:	940011be 	bl	402842c <serial_put_hex>
 4023d38:	52800cc0 	mov	w0, #0x66                  	// #102
 4023d3c:	17ffffe1 	b	4023cc0 <handle_ddr_param_info+0x38>
 4023d40:	34ffffc0 	cbz	w0, 4023d38 <handle_ddr_param_info+0xb0>
 4023d44:	b9402280 	ldr	w0, [x20, #32]
 4023d48:	7104001f 	cmp	w0, #0x100
 4023d4c:	540000a0 	b.eq	4023d60 <handle_ddr_param_info+0xd8>  // b.none
 4023d50:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023d54:	912e1800 	add	x0, x0, #0xb86
 4023d58:	940013c0 	bl	4028c58 <log_serial_puts>
 4023d5c:	17ffffe5 	b	4023cf0 <handle_ddr_param_info+0x68>
 4023d60:	b9400281 	ldr	w1, [x20]
 4023d64:	5294a5a0 	mov	w0, #0xa52d                	// #42285
 4023d68:	72a970e0 	movk	w0, #0x4b87, lsl #16
 4023d6c:	6b00003f 	cmp	w1, w0
 4023d70:	54000361 	b.ne	4023ddc <handle_ddr_param_info+0x154>  // b.any
 4023d74:	d2902a95 	mov	x21, #0x8154                	// #33108
 4023d78:	f2a203d5 	movk	x21, #0x101e, lsl #16
 4023d7c:	b94002a0 	ldr	w0, [x21]
 4023d80:	d3483c00 	ubfx	x0, x0, #8, #8
 4023d84:	7101081f 	cmp	w0, #0x42
 4023d88:	540002e0 	b.eq	4023de4 <handle_ddr_param_info+0x15c>  // b.none
 4023d8c:	97fffe58 	bl	40236ec <get_ree_key_area_addr>
 4023d90:	2a0003f3 	mov	w19, w0
 4023d94:	94000834 	bl	4025e64 <get_low_power_mode>
 4023d98:	7101681f 	cmp	w0, #0x5a
 4023d9c:	54000301 	b.ne	4023dfc <handle_ddr_param_info+0x174>  // b.any
 4023da0:	52801a00 	mov	w0, #0xd0                  	// #208
 4023da4:	aa1403e1 	mov	x1, x20
 4023da8:	52803002 	mov	w2, #0x180                 	// #384
 4023dac:	72a22240 	movk	w0, #0x1112, lsl #16
 4023db0:	97ffff09 	bl	40239d4 <lpds_hash_cmp>
 4023db4:	2a0003f3 	mov	w19, w0
 4023db8:	7102941f 	cmp	w0, #0xa5
 4023dbc:	54fff9a1 	b.ne	4023cf0 <handle_ddr_param_info+0x68>  // b.any
 4023dc0:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023dc4:	912e8000 	add	x0, x0, #0xba0
 4023dc8:	940013a4 	bl	4028c58 <log_serial_puts>
 4023dcc:	2a1303e0 	mov	w0, w19
 4023dd0:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4023dd4:	a8c253f3 	ldp	x19, x20, [sp], #32
 4023dd8:	d65f03c0 	ret
 4023ddc:	528008a0 	mov	w0, #0x45                  	// #69
 4023de0:	17ffffb8 	b	4023cc0 <handle_ddr_param_info+0x38>
 4023de4:	b94002a0 	ldr	w0, [x21]
 4023de8:	d3483c00 	ubfx	x0, x0, #8, #8
 4023dec:	7101081f 	cmp	w0, #0x42
 4023df0:	54fff801 	b.ne	4023cf0 <handle_ddr_param_info+0x68>  // b.any
 4023df4:	528014b3 	mov	w19, #0xa5                  	// #165
 4023df8:	17fffff5 	b	4023dcc <handle_ddr_param_info+0x144>
 4023dfc:	b94002a0 	ldr	w0, [x21]
 4023e00:	d3505c00 	ubfx	x0, x0, #16, #8
 4023e04:	7101081f 	cmp	w0, #0x42
 4023e08:	54000160 	b.eq	4023e34 <handle_ddr_param_info+0x1ac>  // b.none
 4023e0c:	d2857000 	mov	x0, #0x2b80                	// #11136
 4023e10:	91070283 	add	x3, x20, #0x1c0
 4023e14:	aa1403e1 	mov	x1, x20
 4023e18:	52803802 	mov	w2, #0x1c0                 	// #448
 4023e1c:	f2a08040 	movk	x0, #0x402, lsl #16
 4023e20:	97fffe8c 	bl	4023850 <secure_verify_area>
 4023e24:	7102941f 	cmp	w0, #0xa5
 4023e28:	54000060 	b.eq	4023e34 <handle_ddr_param_info+0x1ac>  // b.none
 4023e2c:	52800b80 	mov	w0, #0x5c                  	// #92
 4023e30:	17ffffa4 	b	4023cc0 <handle_ddr_param_info+0x38>
 4023e34:	2a1303e0 	mov	w0, w19
 4023e38:	91060283 	add	x3, x20, #0x180
 4023e3c:	aa1403e1 	mov	x1, x20
 4023e40:	91020000 	add	x0, x0, #0x80
 4023e44:	52803002 	mov	w2, #0x180                 	// #384
 4023e48:	97fffe82 	bl	4023850 <secure_verify_area>
 4023e4c:	7102941f 	cmp	w0, #0xa5
 4023e50:	54000221 	b.ne	4023e94 <handle_ddr_param_info+0x20c>  // b.any
 4023e54:	b9401280 	ldr	w0, [x20, #16]
 4023e58:	97fffdd6 	bl	40235b0 <check_version.constprop.0>
 4023e5c:	2a0003f3 	mov	w19, w0
 4023e60:	7102941f 	cmp	w0, #0xa5
 4023e64:	540001c1 	b.ne	4023e9c <handle_ddr_param_info+0x214>  // b.any
 4023e68:	29430680 	ldp	w0, w1, [x20, #24]
 4023e6c:	97fffd73 	bl	4023438 <check_msid_ext>
 4023e70:	7102941f 	cmp	w0, #0xa5
 4023e74:	54fff3e1 	b.ne	4023cf0 <handle_ddr_param_info+0x68>  // b.any
 4023e78:	d2801a00 	mov	x0, #0xd0                  	// #208
 4023e7c:	f2a22240 	movk	x0, #0x1112, lsl #16
 4023e80:	9400035a 	bl	4024be8 <store_hash_to_lpds>
 4023e84:	b9402280 	ldr	w0, [x20, #32]
 4023e88:	11080000 	add	w0, w0, #0x200
 4023e8c:	97fffc6a 	bl	4023034 <add_boot_img_size>
 4023e90:	17ffffcf 	b	4023dcc <handle_ddr_param_info+0x144>
 4023e94:	52800ba0 	mov	w0, #0x5d                  	// #93
 4023e98:	17ffff8a 	b	4023cc0 <handle_ddr_param_info+0x38>
 4023e9c:	52800c20 	mov	w0, #0x61                  	// #97
 4023ea0:	17ffff88 	b	4023cc0 <handle_ddr_param_info+0x38>

0000000004023ea4 <handle_ddr_param>:
 4023ea4:	d2902a81 	mov	x1, #0x8154                	// #33108
 4023ea8:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4023eac:	f2a203c1 	movk	x1, #0x101e, lsl #16
 4023eb0:	2a0003f3 	mov	w19, w0
 4023eb4:	a9017bf5 	stp	x21, x30, [sp, #16]
 4023eb8:	b9400020 	ldr	w0, [x1]
 4023ebc:	d3483c00 	ubfx	x0, x0, #8, #8
 4023ec0:	7101081f 	cmp	w0, #0x42
 4023ec4:	540000e1 	b.ne	4023ee0 <handle_ddr_param+0x3c>  // b.any
 4023ec8:	b9400020 	ldr	w0, [x1]
 4023ecc:	d3483c00 	ubfx	x0, x0, #8, #8
 4023ed0:	7101081f 	cmp	w0, #0x42
 4023ed4:	540004a0 	b.eq	4023f68 <handle_ddr_param+0xc4>  // b.none
 4023ed8:	52800b53 	mov	w19, #0x5a                  	// #90
 4023edc:	1400001c 	b	4023f4c <handle_ddr_param+0xa8>
 4023ee0:	97fffe08 	bl	4023700 <get_ddr_param_info_addr>
 4023ee4:	2a0003f4 	mov	w20, w0
 4023ee8:	8b204260 	add	x0, x19, w0, uxtw
 4023eec:	b9402a81 	ldr	w1, [x20, #40]
 4023ef0:	3944b000 	ldrb	w0, [x0, #300]
 4023ef4:	6b00003f 	cmp	w1, w0
 4023ef8:	54000329 	b.ls	4023f5c <handle_ddr_param+0xb8>  // b.plast
 4023efc:	97fffe06 	bl	4023714 <get_ddr_param_data_addr>
 4023f00:	8b131693 	add	x19, x20, x19, lsl #5
 4023f04:	b9402681 	ldr	w1, [x20, #36]
 4023f08:	2a0003f5 	mov	w21, w0
 4023f0c:	9100b273 	add	x19, x19, #0x2c
 4023f10:	aa1503e0 	mov	x0, x21
 4023f14:	aa1303e2 	mov	x2, x19
 4023f18:	97fffe0f 	bl	4023754 <hash_compare>
 4023f1c:	7102941f 	cmp	w0, #0xa5
 4023f20:	54fffdc1 	b.ne	4023ed8 <handle_ddr_param+0x34>  // b.any
 4023f24:	b9402681 	ldr	w1, [x20, #36]
 4023f28:	aa1303e2 	mov	x2, x19
 4023f2c:	aa1503e0 	mov	x0, x21
 4023f30:	97fffe09 	bl	4023754 <hash_compare>
 4023f34:	2a0003f3 	mov	w19, w0
 4023f38:	7102941f 	cmp	w0, #0xa5
 4023f3c:	54fffce1 	b.ne	4023ed8 <handle_ddr_param+0x34>  // b.any
 4023f40:	29448680 	ldp	w0, w1, [x20, #36]
 4023f44:	1b007c20 	mul	w0, w1, w0
 4023f48:	97fffc3b 	bl	4023034 <add_boot_img_size>
 4023f4c:	2a1303e0 	mov	w0, w19
 4023f50:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4023f54:	a8c253f3 	ldp	x19, x20, [sp], #32
 4023f58:	d65f03c0 	ret
 4023f5c:	52800ca0 	mov	w0, #0x65                  	// #101
 4023f60:	9400086a 	bl	4026108 <err_print>
 4023f64:	17ffffdd 	b	4023ed8 <handle_ddr_param+0x34>
 4023f68:	528014b3 	mov	w19, #0xa5                  	// #165
 4023f6c:	17fffff8 	b	4023f4c <handle_ddr_param+0xa8>

0000000004023f70 <handle_uboot_info>:
 4023f70:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4023f74:	d2a82014 	mov	x20, #0x41000000            	// #1090519040
 4023f78:	5283c5a0 	mov	w0, #0x1e2d                	// #7725
 4023f7c:	b9400281 	ldr	w1, [x20]
 4023f80:	72a97e00 	movk	w0, #0x4bf0, lsl #16
 4023f84:	a9017bf5 	stp	x21, x30, [sp, #16]
 4023f88:	6b00003f 	cmp	w1, w0
 4023f8c:	54000361 	b.ne	4023ff8 <handle_uboot_info+0x88>  // b.any
 4023f90:	d2902a95 	mov	x21, #0x8154                	// #33108
 4023f94:	f2a203d5 	movk	x21, #0x101e, lsl #16
 4023f98:	b94002a0 	ldr	w0, [x21]
 4023f9c:	d3483c00 	ubfx	x0, x0, #8, #8
 4023fa0:	7101081f 	cmp	w0, #0x42
 4023fa4:	54000300 	b.eq	4024004 <handle_uboot_info+0x94>  // b.none
 4023fa8:	97fffdd1 	bl	40236ec <get_ree_key_area_addr>
 4023fac:	2a0003f3 	mov	w19, w0
 4023fb0:	940007ad 	bl	4025e64 <get_low_power_mode>
 4023fb4:	7101681f 	cmp	w0, #0x5a
 4023fb8:	54000321 	b.ne	402401c <handle_uboot_info+0xac>  // b.any
 4023fbc:	52801e00 	mov	w0, #0xf0                  	// #240
 4023fc0:	aa1403e1 	mov	x1, x20
 4023fc4:	52803002 	mov	w2, #0x180                 	// #384
 4023fc8:	72a22240 	movk	w0, #0x1112, lsl #16
 4023fcc:	97fffe82 	bl	40239d4 <lpds_hash_cmp>
 4023fd0:	2a0003f3 	mov	w19, w0
 4023fd4:	7102941f 	cmp	w0, #0xa5
 4023fd8:	540001e1 	b.ne	4024014 <handle_uboot_info+0xa4>  // b.any
 4023fdc:	d0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4023fe0:	912ed000 	add	x0, x0, #0xbb4
 4023fe4:	9400131d 	bl	4028c58 <log_serial_puts>
 4023fe8:	2a1303e0 	mov	w0, w19
 4023fec:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4023ff0:	a8c253f3 	ldp	x19, x20, [sp], #32
 4023ff4:	d65f03c0 	ret
 4023ff8:	528008a0 	mov	w0, #0x45                  	// #69
 4023ffc:	94000843 	bl	4026108 <err_print>
 4024000:	14000005 	b	4024014 <handle_uboot_info+0xa4>
 4024004:	b94002a0 	ldr	w0, [x21]
 4024008:	d3483c00 	ubfx	x0, x0, #8, #8
 402400c:	7101081f 	cmp	w0, #0x42
 4024010:	540005e0 	b.eq	40240cc <handle_uboot_info+0x15c>  // b.none
 4024014:	52800b53 	mov	w19, #0x5a                  	// #90
 4024018:	17fffff4 	b	4023fe8 <handle_uboot_info+0x78>
 402401c:	b94002a0 	ldr	w0, [x21]
 4024020:	d3505c00 	ubfx	x0, x0, #16, #8
 4024024:	7101081f 	cmp	w0, #0x42
 4024028:	54000180 	b.eq	4024058 <handle_uboot_info+0xe8>  // b.none
 402402c:	d2803803 	mov	x3, #0x1c0                 	// #448
 4024030:	d2857000 	mov	x0, #0x2b80                	// #11136
 4024034:	aa1403e1 	mov	x1, x20
 4024038:	f2a82003 	movk	x3, #0x4100, lsl #16
 402403c:	52803802 	mov	w2, #0x1c0                 	// #448
 4024040:	f2a08040 	movk	x0, #0x402, lsl #16
 4024044:	97fffe03 	bl	4023850 <secure_verify_area>
 4024048:	7102941f 	cmp	w0, #0xa5
 402404c:	54000060 	b.eq	4024058 <handle_uboot_info+0xe8>  // b.none
 4024050:	52800b80 	mov	w0, #0x5c                  	// #92
 4024054:	17ffffea 	b	4023ffc <handle_uboot_info+0x8c>
 4024058:	2a1303e0 	mov	w0, w19
 402405c:	d2803003 	mov	x3, #0x180                 	// #384
 4024060:	91020000 	add	x0, x0, #0x80
 4024064:	f2a82003 	movk	x3, #0x4100, lsl #16
 4024068:	52803002 	mov	w2, #0x180                 	// #384
 402406c:	d2a82001 	mov	x1, #0x41000000            	// #1090519040
 4024070:	97fffdf8 	bl	4023850 <secure_verify_area>
 4024074:	7102941f 	cmp	w0, #0xa5
 4024078:	54000221 	b.ne	40240bc <handle_uboot_info+0x14c>  // b.any
 402407c:	d2a82014 	mov	x20, #0x41000000            	// #1090519040
 4024080:	b9401280 	ldr	w0, [x20, #16]
 4024084:	97fffd4b 	bl	40235b0 <check_version.constprop.0>
 4024088:	2a0003f3 	mov	w19, w0
 402408c:	7102941f 	cmp	w0, #0xa5
 4024090:	540001a1 	b.ne	40240c4 <handle_uboot_info+0x154>  // b.any
 4024094:	29430680 	ldp	w0, w1, [x20, #24]
 4024098:	97fffce8 	bl	4023438 <check_msid_ext>
 402409c:	7102941f 	cmp	w0, #0xa5
 40240a0:	54fffba1 	b.ne	4024014 <handle_uboot_info+0xa4>  // b.any
 40240a4:	d2801e00 	mov	x0, #0xf0                  	// #240
 40240a8:	f2a22240 	movk	x0, #0x1112, lsl #16
 40240ac:	940002cf 	bl	4024be8 <store_hash_to_lpds>
 40240b0:	52804000 	mov	w0, #0x200                 	// #512
 40240b4:	97fffbe0 	bl	4023034 <add_boot_img_size>
 40240b8:	17ffffcc 	b	4023fe8 <handle_uboot_info+0x78>
 40240bc:	52800ba0 	mov	w0, #0x5d                  	// #93
 40240c0:	17ffffcf 	b	4023ffc <handle_uboot_info+0x8c>
 40240c4:	52800c20 	mov	w0, #0x61                  	// #97
 40240c8:	17ffffcd 	b	4023ffc <handle_uboot_info+0x8c>
 40240cc:	528014b3 	mov	w19, #0xa5                  	// #165
 40240d0:	17ffffc6 	b	4023fe8 <handle_uboot_info+0x78>

00000000040240d4 <handle_uboot_code>:
 40240d4:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 40240d8:	a9017bf5 	stp	x21, x30, [sp, #16]
 40240dc:	d2a82015 	mov	x21, #0x41000000            	// #1090519040
 40240e0:	b94026b4 	ldr	w20, [x21, #36]
 40240e4:	b9408ab3 	ldr	w19, [x21, #136]
 40240e8:	94000cc4 	bl	40273f8 <is_ree_boot_dec_en_enable>
 40240ec:	528d34a1 	mov	w1, #0x69a5                	// #27045
 40240f0:	72b86b41 	movk	w1, #0xc35a, lsl #16
 40240f4:	6b01001f 	cmp	w0, w1
 40240f8:	540000c1 	b.ne	4024110 <handle_uboot_code+0x3c>  // b.any
 40240fc:	b9404aa1 	ldr	w1, [x21, #72]
 4024100:	5292dc20 	mov	w0, #0x96e1                	// #38625
 4024104:	72a78f00 	movk	w0, #0x3c78, lsl #16
 4024108:	6b00003f 	cmp	w1, w0
 402410c:	54000240 	b.eq	4024154 <handle_uboot_code+0x80>  // b.none
 4024110:	d2800901 	mov	x1, #0x48                  	// #72
 4024114:	52800080 	mov	w0, #0x4                   	// #4
 4024118:	2a1403e4 	mov	w4, w20
 402411c:	aa1303e3 	mov	x3, x19
 4024120:	aa1303e2 	mov	x2, x19
 4024124:	f2a82001 	movk	x1, #0x4100, lsl #16
 4024128:	72a06000 	movk	w0, #0x300, lsl #16
 402412c:	9400018e 	bl	4024764 <decrypt_data>
 4024130:	7102941f 	cmp	w0, #0xa5
 4024134:	54000100 	b.eq	4024154 <handle_uboot_code+0x80>  // b.none
 4024138:	52800a00 	mov	w0, #0x50                  	// #80
 402413c:	940007f3 	bl	4026108 <err_print>
 4024140:	52800b53 	mov	w19, #0x5a                  	// #90
 4024144:	2a1303e0 	mov	w0, w19
 4024148:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402414c:	a8c253f3 	ldp	x19, x20, [sp], #32
 4024150:	d65f03c0 	ret
 4024154:	d2902a81 	mov	x1, #0x8154                	// #33108
 4024158:	f2a203c1 	movk	x1, #0x101e, lsl #16
 402415c:	b9400020 	ldr	w0, [x1]
 4024160:	d3483c00 	ubfx	x0, x0, #8, #8
 4024164:	7101081f 	cmp	w0, #0x42
 4024168:	54000280 	b.eq	40241b8 <handle_uboot_code+0xe4>  // b.none
 402416c:	d2800502 	mov	x2, #0x28                  	// #40
 4024170:	2a1403e1 	mov	w1, w20
 4024174:	aa1303e0 	mov	x0, x19
 4024178:	f2a82002 	movk	x2, #0x4100, lsl #16
 402417c:	97fffd76 	bl	4023754 <hash_compare>
 4024180:	7102941f 	cmp	w0, #0xa5
 4024184:	54fffde1 	b.ne	4024140 <handle_uboot_code+0x6c>  // b.any
 4024188:	d2800502 	mov	x2, #0x28                  	// #40
 402418c:	aa1303e0 	mov	x0, x19
 4024190:	2a1403e1 	mov	w1, w20
 4024194:	f2a82002 	movk	x2, #0x4100, lsl #16
 4024198:	97fffd6f 	bl	4023754 <hash_compare>
 402419c:	2a0003f3 	mov	w19, w0
 40241a0:	7102941f 	cmp	w0, #0xa5
 40241a4:	54fffce1 	b.ne	4024140 <handle_uboot_code+0x6c>  // b.any
 40241a8:	d2a82000 	mov	x0, #0x41000000            	// #1090519040
 40241ac:	b9402400 	ldr	w0, [x0, #36]
 40241b0:	97fffba1 	bl	4023034 <add_boot_img_size>
 40241b4:	17ffffe4 	b	4024144 <handle_uboot_code+0x70>
 40241b8:	b9400020 	ldr	w0, [x1]
 40241bc:	d3483c00 	ubfx	x0, x0, #8, #8
 40241c0:	7101081f 	cmp	w0, #0x42
 40241c4:	54fffbe1 	b.ne	4024140 <handle_uboot_code+0x6c>  // b.any
 40241c8:	528014b3 	mov	w19, #0xa5                  	// #165
 40241cc:	17ffffde 	b	4024144 <handle_uboot_code+0x70>

00000000040241d0 <handle_tee_key>:
 40241d0:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 40241d4:	528d34b4 	mov	w20, #0x69a5                	// #27045
 40241d8:	72b86b54 	movk	w20, #0xc35a, lsl #16
 40241dc:	aa0003f3 	mov	x19, x0
 40241e0:	f9000bfe 	str	x30, [sp, #16]
 40241e4:	94000c63 	bl	4027370 <is_soc_tee_enable>
 40241e8:	6b14001f 	cmp	w0, w20
 40241ec:	54000200 	b.eq	402422c <handle_tee_key+0x5c>  // b.none
 40241f0:	d2902a80 	mov	x0, #0x8154                	// #33108
 40241f4:	f2a203c0 	movk	x0, #0x101e, lsl #16
 40241f8:	b9400000 	ldr	w0, [x0]
 40241fc:	12001c00 	and	w0, w0, #0xff
 4024200:	7101081f 	cmp	w0, #0x42
 4024204:	54000140 	b.eq	402422c <handle_tee_key+0x5c>  // b.none
 4024208:	b9400261 	ldr	w1, [x19]
 402420c:	5281e3c0 	mov	w0, #0xf1e                 	// #3870
 4024210:	72a97c20 	movk	w0, #0x4be1, lsl #16
 4024214:	6b00003f 	cmp	w1, w0
 4024218:	54000260 	b.eq	4024264 <handle_tee_key+0x94>  // b.none
 402421c:	528008a0 	mov	w0, #0x45                  	// #69
 4024220:	940007ba 	bl	4026108 <err_print>
 4024224:	52800b40 	mov	w0, #0x5a                  	// #90
 4024228:	14000005 	b	402423c <handle_tee_key+0x6c>
 402422c:	94000c51 	bl	4027370 <is_soc_tee_enable>
 4024230:	6b14001f 	cmp	w0, w20
 4024234:	540000a1 	b.ne	4024248 <handle_tee_key+0x78>  // b.any
 4024238:	528014a0 	mov	w0, #0xa5                  	// #165
 402423c:	f9400bfe 	ldr	x30, [sp, #16]
 4024240:	a8c253f3 	ldp	x19, x20, [sp], #32
 4024244:	d65f03c0 	ret
 4024248:	d2902a80 	mov	x0, #0x8154                	// #33108
 402424c:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4024250:	b9400000 	ldr	w0, [x0]
 4024254:	12001c00 	and	w0, w0, #0xff
 4024258:	7101081f 	cmp	w0, #0x42
 402425c:	54fffd61 	b.ne	4024208 <handle_tee_key+0x38>  // b.any
 4024260:	17fffff6 	b	4024238 <handle_tee_key+0x68>
 4024264:	d2855000 	mov	x0, #0x2a80                	// #10880
 4024268:	91030263 	add	x3, x19, #0xc0
 402426c:	aa1303e1 	mov	x1, x19
 4024270:	52801802 	mov	w2, #0xc0                  	// #192
 4024274:	f2a08040 	movk	x0, #0x402, lsl #16
 4024278:	97fffd76 	bl	4023850 <secure_verify_area>
 402427c:	7102941f 	cmp	w0, #0xa5
 4024280:	54000261 	b.ne	40242cc <handle_tee_key+0xfc>  // b.any
 4024284:	b9402660 	ldr	w0, [x19, #36]
 4024288:	97fffcca 	bl	40235b0 <check_version.constprop.0>
 402428c:	7102941f 	cmp	w0, #0xa5
 4024290:	54000221 	b.ne	40242d4 <handle_tee_key+0x104>  // b.any
 4024294:	29458660 	ldp	w0, w1, [x19, #44]
 4024298:	97fffc68 	bl	4023438 <check_msid_ext>
 402429c:	7102941f 	cmp	w0, #0xa5
 40242a0:	54fffc21 	b.ne	4024224 <handle_tee_key+0x54>  // b.any
 40242a4:	b9403661 	ldr	w1, [x19, #52]
 40242a8:	5292dc20 	mov	w0, #0x96e1                	// #38625
 40242ac:	72a78f00 	movk	w0, #0x3c78, lsl #16
 40242b0:	6b00003f 	cmp	w1, w0
 40242b4:	54fffc21 	b.ne	4024238 <handle_tee_key+0x68>  // b.any
 40242b8:	9100e260 	add	x0, x19, #0x38
 40242bc:	97fffc82 	bl	40234c4 <die_id_compare>
 40242c0:	7102941f 	cmp	w0, #0xa5
 40242c4:	54fffb01 	b.ne	4024224 <handle_tee_key+0x54>  // b.any
 40242c8:	17ffffdc 	b	4024238 <handle_tee_key+0x68>
 40242cc:	52800ba0 	mov	w0, #0x5d                  	// #93
 40242d0:	17ffffd4 	b	4024220 <handle_tee_key+0x50>
 40242d4:	52800c20 	mov	w0, #0x61                  	// #97
 40242d8:	17ffffd2 	b	4024220 <handle_tee_key+0x50>

00000000040242dc <handle_tee_info>:
 40242dc:	d100c3ff 	sub	sp, sp, #0x30
 40242e0:	a90153f3 	stp	x19, x20, [sp, #16]
 40242e4:	aa0103f3 	mov	x19, x1
 40242e8:	aa0003f4 	mov	x20, x0
 40242ec:	a9027bf5 	stp	x21, x30, [sp, #32]
 40242f0:	b0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40242f4:	f9477001 	ldr	x1, [x0, #3808]
 40242f8:	f90007e1 	str	x1, [sp, #8]
 40242fc:	d2800001 	mov	x1, #0x0                   	// #0
 4024300:	5281e5a0 	mov	w0, #0xf2d                 	// #3885
 4024304:	b9400261 	ldr	w1, [x19]
 4024308:	72a97c20 	movk	w0, #0x4be1, lsl #16
 402430c:	6b00003f 	cmp	w1, w0
 4024310:	54000221 	b.ne	4024354 <handle_tee_info+0x78>  // b.any
 4024314:	528d34b5 	mov	w21, #0x69a5                	// #27045
 4024318:	94000c16 	bl	4027370 <is_soc_tee_enable>
 402431c:	72b86b55 	movk	w21, #0xc35a, lsl #16
 4024320:	6b15001f 	cmp	w0, w21
 4024324:	540001e1 	b.ne	4024360 <handle_tee_info+0x84>  // b.any
 4024328:	94000c12 	bl	4027370 <is_soc_tee_enable>
 402432c:	6b15001f 	cmp	w0, w21
 4024330:	54000581 	b.ne	40243e0 <handle_tee_info+0x104>  // b.any
 4024334:	528014a0 	mov	w0, #0xa5                  	// #165
 4024338:	b0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402433c:	f94007e2 	ldr	x2, [sp, #8]
 4024340:	f9477023 	ldr	x3, [x1, #3808]
 4024344:	eb030042 	subs	x2, x2, x3
 4024348:	d2800003 	mov	x3, #0x0                   	// #0
 402434c:	54000a20 	b.eq	4024490 <handle_tee_info+0x1b4>  // b.none
 4024350:	940007ab 	bl	40261fc <__stack_chk_fail>
 4024354:	528008a0 	mov	w0, #0x45                  	// #69
 4024358:	9400076c 	bl	4026108 <err_print>
 402435c:	1400004b 	b	4024488 <handle_tee_info+0x1ac>
 4024360:	d2902a80 	mov	x0, #0x8154                	// #33108
 4024364:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4024368:	b9400000 	ldr	w0, [x0]
 402436c:	12001c00 	and	w0, w0, #0xff
 4024370:	7101081f 	cmp	w0, #0x42
 4024374:	54fffda0 	b.eq	4024328 <handle_tee_info+0x4c>  // b.none
 4024378:	d2902a80 	mov	x0, #0x8154                	// #33108
 402437c:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4024380:	b9400000 	ldr	w0, [x0]
 4024384:	d3505c00 	ubfx	x0, x0, #16, #8
 4024388:	7101081f 	cmp	w0, #0x42
 402438c:	54000381 	b.ne	40243fc <handle_tee_info+0x120>  // b.any
 4024390:	91060263 	add	x3, x19, #0x180
 4024394:	aa1303e1 	mov	x1, x19
 4024398:	91020280 	add	x0, x20, #0x80
 402439c:	52803002 	mov	w2, #0x180                 	// #384
 40243a0:	97fffd2c 	bl	4023850 <secure_verify_area>
 40243a4:	7102941f 	cmp	w0, #0xa5
 40243a8:	540003e1 	b.ne	4024424 <handle_tee_info+0x148>  // b.any
 40243ac:	b9402a80 	ldr	w0, [x20, #40]
 40243b0:	910013e1 	add	x1, sp, #0x4
 40243b4:	b9401275 	ldr	w21, [x19, #16]
 40243b8:	d2800002 	mov	x2, #0x0                   	// #0
 40243bc:	2a2003f4 	mvn	w20, w0
 40243c0:	52803700 	mov	w0, #0x1b8                 	// #440
 40243c4:	940011c5 	bl	4028ad8 <uapi_drv_otp_read_word>
 40243c8:	7102941f 	cmp	w0, #0xa5
 40243cc:	54000300 	b.eq	402442c <handle_tee_info+0x150>  // b.none
 40243d0:	52800bc0 	mov	w0, #0x5e                  	// #94
 40243d4:	9400074d 	bl	4026108 <err_print>
 40243d8:	52800c20 	mov	w0, #0x61                  	// #97
 40243dc:	17ffffdf 	b	4024358 <handle_tee_info+0x7c>
 40243e0:	d2902a80 	mov	x0, #0x8154                	// #33108
 40243e4:	f2a203c0 	movk	x0, #0x101e, lsl #16
 40243e8:	b9400000 	ldr	w0, [x0]
 40243ec:	12001c00 	and	w0, w0, #0xff
 40243f0:	7101081f 	cmp	w0, #0x42
 40243f4:	54fffc21 	b.ne	4024378 <handle_tee_info+0x9c>  // b.any
 40243f8:	17ffffcf 	b	4024334 <handle_tee_info+0x58>
 40243fc:	d2857000 	mov	x0, #0x2b80                	// #11136
 4024400:	91070263 	add	x3, x19, #0x1c0
 4024404:	aa1303e1 	mov	x1, x19
 4024408:	52803802 	mov	w2, #0x1c0                 	// #448
 402440c:	f2a08040 	movk	x0, #0x402, lsl #16
 4024410:	97fffd10 	bl	4023850 <secure_verify_area>
 4024414:	7102941f 	cmp	w0, #0xa5
 4024418:	54fffbc0 	b.eq	4024390 <handle_tee_info+0xb4>  // b.none
 402441c:	52800b80 	mov	w0, #0x5c                  	// #92
 4024420:	17ffffce 	b	4024358 <handle_tee_info+0x7c>
 4024424:	52800ba0 	mov	w0, #0x5d                  	// #93
 4024428:	17ffffcc 	b	4024358 <handle_tee_info+0x7c>
 402442c:	b94007e0 	ldr	w0, [sp, #4]
 4024430:	910013e1 	add	x1, sp, #0x4
 4024434:	d2800002 	mov	x2, #0x0                   	// #0
 4024438:	0a000280 	and	w0, w20, w0
 402443c:	b90007e0 	str	w0, [sp, #4]
 4024440:	97fffc57 	bl	402359c <count_bit_set>
 4024444:	2a0003f4 	mov	w20, w0
 4024448:	52803780 	mov	w0, #0x1bc                 	// #444
 402444c:	940011a3 	bl	4028ad8 <uapi_drv_otp_read_word>
 4024450:	7102941f 	cmp	w0, #0xa5
 4024454:	54fffbe1 	b.ne	40243d0 <handle_tee_info+0xf4>  // b.any
 4024458:	b94007e0 	ldr	w0, [sp, #4]
 402445c:	97fffc50 	bl	402359c <count_bit_set>
 4024460:	0b000294 	add	w20, w20, w0
 4024464:	710102bf 	cmp	w21, #0x40
 4024468:	7a5492a0 	ccmp	w21, w20, #0x0, ls  // ls = plast
 402446c:	54000062 	b.cs	4024478 <handle_tee_info+0x19c>  // b.hs, b.nlast
 4024470:	52800c00 	mov	w0, #0x60                  	// #96
 4024474:	17ffffd8 	b	40243d4 <handle_tee_info+0xf8>
 4024478:	29430660 	ldp	w0, w1, [x19, #24]
 402447c:	97fffbef 	bl	4023438 <check_msid_ext>
 4024480:	7102941f 	cmp	w0, #0xa5
 4024484:	54fff580 	b.eq	4024334 <handle_tee_info+0x58>  // b.none
 4024488:	52800b40 	mov	w0, #0x5a                  	// #90
 402448c:	17ffffab 	b	4024338 <handle_tee_info+0x5c>
 4024490:	a94153f3 	ldp	x19, x20, [sp, #16]
 4024494:	a9427bf5 	ldp	x21, x30, [sp, #32]
 4024498:	9100c3ff 	add	sp, sp, #0x30
 402449c:	d65f03c0 	ret

00000000040244a0 <handle_atf_code>:
 40244a0:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 40244a4:	aa0103f4 	mov	x20, x1
 40244a8:	a9015bf5 	stp	x21, x22, [sp, #16]
 40244ac:	aa0203f5 	mov	x21, x2
 40244b0:	b940b836 	ldr	w22, [x1, #184]
 40244b4:	a90263f7 	stp	x23, x24, [sp, #32]
 40244b8:	12bfffc1 	mov	w1, #0x1ffff               	// #131071
 40244bc:	510006c2 	sub	w2, w22, #0x1
 40244c0:	f9001bfe 	str	x30, [sp, #48]
 40244c4:	6b01005f 	cmp	w2, w1
 40244c8:	54000109 	b.ls	40244e8 <handle_atf_code+0x48>  // b.plast
 40244cc:	52800b53 	mov	w19, #0x5a                  	// #90
 40244d0:	2a1303e0 	mov	w0, w19
 40244d4:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40244d8:	a94263f7 	ldp	x23, x24, [sp, #32]
 40244dc:	f9401bfe 	ldr	x30, [sp, #48]
 40244e0:	a8c453f3 	ldp	x19, x20, [sp], #64
 40244e4:	d65f03c0 	ret
 40244e8:	910c0013 	add	x19, x0, #0x300
 40244ec:	94000bbb 	bl	40273d8 <is_tee_dec_en_enable>
 40244f0:	2a0003f7 	mov	w23, w0
 40244f4:	528d34a0 	mov	w0, #0x69a5                	// #27045
 40244f8:	72b86b40 	movk	w0, #0xc35a, lsl #16
 40244fc:	6b0002ff 	cmp	w23, w0
 4024500:	54000441 	b.ne	4024588 <handle_atf_code+0xe8>  // b.any
 4024504:	b9404a98 	ldr	w24, [x20, #72]
 4024508:	5292dc20 	mov	w0, #0x96e1                	// #38625
 402450c:	72a78f00 	movk	w0, #0x3c78, lsl #16
 4024510:	6b00031f 	cmp	w24, w0
 4024514:	540003a1 	b.ne	4024588 <handle_atf_code+0xe8>  // b.any
 4024518:	94000bb0 	bl	40273d8 <is_tee_dec_en_enable>
 402451c:	6b17001f 	cmp	w0, w23
 4024520:	54000341 	b.ne	4024588 <handle_atf_code+0xe8>  // b.any
 4024524:	b9404a80 	ldr	w0, [x20, #72]
 4024528:	6b18001f 	cmp	w0, w24
 402452c:	540002e1 	b.ne	4024588 <handle_atf_code+0xe8>  // b.any
 4024530:	aa1303e2 	mov	x2, x19
 4024534:	2a1603e1 	mov	w1, w22
 4024538:	d2a80020 	mov	x0, #0x40010000            	// #1073807360
 402453c:	94000244 	bl	4024e4c <dma_copy>
 4024540:	2a0003f3 	mov	w19, w0
 4024544:	7102941f 	cmp	w0, #0xa5
 4024548:	54000301 	b.ne	40245a8 <handle_atf_code+0x108>  // b.any
 402454c:	528d34b6 	mov	w22, #0x69a5                	// #27045
 4024550:	94000b88 	bl	4027370 <is_soc_tee_enable>
 4024554:	72b86b56 	movk	w22, #0xc35a, lsl #16
 4024558:	6b16001f 	cmp	w0, w22
 402455c:	540002c1 	b.ne	40245b4 <handle_atf_code+0x114>  // b.any
 4024560:	94000b84 	bl	4027370 <is_soc_tee_enable>
 4024564:	6b16001f 	cmp	w0, w22
 4024568:	54fffb40 	b.eq	40244d0 <handle_atf_code+0x30>  // b.none
 402456c:	d2902a80 	mov	x0, #0x8154                	// #33108
 4024570:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4024574:	b9400000 	ldr	w0, [x0]
 4024578:	12001c00 	and	w0, w0, #0xff
 402457c:	7101081f 	cmp	w0, #0x42
 4024580:	54000261 	b.ne	40245cc <handle_atf_code+0x12c>  // b.any
 4024584:	17ffffd3 	b	40244d0 <handle_atf_code+0x30>
 4024588:	52800020 	mov	w0, #0x1                   	// #1
 402458c:	2a1603e4 	mov	w4, w22
 4024590:	aa1303e3 	mov	x3, x19
 4024594:	91012281 	add	x1, x20, #0x48
 4024598:	d2a80022 	mov	x2, #0x40010000            	// #1073807360
 402459c:	72a06000 	movk	w0, #0x300, lsl #16
 40245a0:	94000071 	bl	4024764 <decrypt_data>
 40245a4:	17ffffe7 	b	4024540 <handle_atf_code+0xa0>
 40245a8:	52800a00 	mov	w0, #0x50                  	// #80
 40245ac:	940006d7 	bl	4026108 <err_print>
 40245b0:	17ffffc7 	b	40244cc <handle_atf_code+0x2c>
 40245b4:	d2902a80 	mov	x0, #0x8154                	// #33108
 40245b8:	f2a203c0 	movk	x0, #0x101e, lsl #16
 40245bc:	b9400000 	ldr	w0, [x0]
 40245c0:	12001c00 	and	w0, w0, #0xff
 40245c4:	7101081f 	cmp	w0, #0x42
 40245c8:	54fffcc0 	b.eq	4024560 <handle_atf_code+0xc0>  // b.none
 40245cc:	b940ba81 	ldr	w1, [x20, #184]
 40245d0:	91025296 	add	x22, x20, #0x94
 40245d4:	aa1603e2 	mov	x2, x22
 40245d8:	aa1503e0 	mov	x0, x21
 40245dc:	97fffc5e 	bl	4023754 <hash_compare>
 40245e0:	7102941f 	cmp	w0, #0xa5
 40245e4:	54fff741 	b.ne	40244cc <handle_atf_code+0x2c>  // b.any
 40245e8:	b940ba81 	ldr	w1, [x20, #184]
 40245ec:	aa1603e2 	mov	x2, x22
 40245f0:	aa1503e0 	mov	x0, x21
 40245f4:	97fffc58 	bl	4023754 <hash_compare>
 40245f8:	7102941f 	cmp	w0, #0xa5
 40245fc:	54fff6a0 	b.eq	40244d0 <handle_atf_code+0x30>  // b.none
 4024600:	17ffffb3 	b	40244cc <handle_atf_code+0x2c>

0000000004024604 <handle_tee_code>:
 4024604:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 4024608:	aa0103f4 	mov	x20, x1
 402460c:	a9015bf5 	stp	x21, x22, [sp, #16]
 4024610:	aa0203f5 	mov	x21, x2
 4024614:	b9402436 	ldr	w22, [x1, #36]
 4024618:	a90263f7 	stp	x23, x24, [sp, #32]
 402461c:	12bfe001 	mov	w1, #0xffffff              	// #16777215
 4024620:	510006c2 	sub	w2, w22, #0x1
 4024624:	f9001bfe 	str	x30, [sp, #48]
 4024628:	6b01005f 	cmp	w2, w1
 402462c:	54000069 	b.ls	4024638 <handle_tee_code+0x34>  // b.plast
 4024630:	52800b53 	mov	w19, #0x5a                  	// #90
 4024634:	14000025 	b	40246c8 <handle_tee_code+0xc4>
 4024638:	910c0000 	add	x0, x0, #0x300
 402463c:	b9409293 	ldr	w19, [x20, #144]
 4024640:	8b000273 	add	x19, x19, x0
 4024644:	94000b65 	bl	40273d8 <is_tee_dec_en_enable>
 4024648:	2a0003f7 	mov	w23, w0
 402464c:	528d34a0 	mov	w0, #0x69a5                	// #27045
 4024650:	72b86b40 	movk	w0, #0xc35a, lsl #16
 4024654:	6b0002ff 	cmp	w23, w0
 4024658:	54000441 	b.ne	40246e0 <handle_tee_code+0xdc>  // b.any
 402465c:	b9404a98 	ldr	w24, [x20, #72]
 4024660:	5292dc20 	mov	w0, #0x96e1                	// #38625
 4024664:	72a78f00 	movk	w0, #0x3c78, lsl #16
 4024668:	6b00031f 	cmp	w24, w0
 402466c:	540003a1 	b.ne	40246e0 <handle_tee_code+0xdc>  // b.any
 4024670:	94000b5a 	bl	40273d8 <is_tee_dec_en_enable>
 4024674:	6b17001f 	cmp	w0, w23
 4024678:	54000341 	b.ne	40246e0 <handle_tee_code+0xdc>  // b.any
 402467c:	b9404a80 	ldr	w0, [x20, #72]
 4024680:	6b18001f 	cmp	w0, w24
 4024684:	540002e1 	b.ne	40246e0 <handle_tee_code+0xdc>  // b.any
 4024688:	d29ffc80 	mov	x0, #0xffe4                	// #65508
 402468c:	aa1303e2 	mov	x2, x19
 4024690:	2a1603e1 	mov	w1, w22
 4024694:	f2a80040 	movk	x0, #0x4002, lsl #16
 4024698:	940001ed 	bl	4024e4c <dma_copy>
 402469c:	2a0003f3 	mov	w19, w0
 40246a0:	7102941f 	cmp	w0, #0xa5
 40246a4:	54000301 	b.ne	4024704 <handle_tee_code+0x100>  // b.any
 40246a8:	528d34b6 	mov	w22, #0x69a5                	// #27045
 40246ac:	94000b31 	bl	4027370 <is_soc_tee_enable>
 40246b0:	72b86b56 	movk	w22, #0xc35a, lsl #16
 40246b4:	6b16001f 	cmp	w0, w22
 40246b8:	540002c1 	b.ne	4024710 <handle_tee_code+0x10c>  // b.any
 40246bc:	94000b2d 	bl	4027370 <is_soc_tee_enable>
 40246c0:	6b16001f 	cmp	w0, w22
 40246c4:	54000481 	b.ne	4024754 <handle_tee_code+0x150>  // b.any
 40246c8:	2a1303e0 	mov	w0, w19
 40246cc:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40246d0:	a94263f7 	ldp	x23, x24, [sp, #32]
 40246d4:	f9401bfe 	ldr	x30, [sp, #48]
 40246d8:	a8c453f3 	ldp	x19, x20, [sp], #64
 40246dc:	d65f03c0 	ret
 40246e0:	d29ffc82 	mov	x2, #0xffe4                	// #65508
 40246e4:	52800020 	mov	w0, #0x1                   	// #1
 40246e8:	2a1603e4 	mov	w4, w22
 40246ec:	aa1303e3 	mov	x3, x19
 40246f0:	91012281 	add	x1, x20, #0x48
 40246f4:	f2a80042 	movk	x2, #0x4002, lsl #16
 40246f8:	72a06000 	movk	w0, #0x300, lsl #16
 40246fc:	9400001a 	bl	4024764 <decrypt_data>
 4024700:	17ffffe7 	b	402469c <handle_tee_code+0x98>
 4024704:	52800a00 	mov	w0, #0x50                  	// #80
 4024708:	94000680 	bl	4026108 <err_print>
 402470c:	17ffffc9 	b	4024630 <handle_tee_code+0x2c>
 4024710:	94000b0f 	bl	402734c <is_tee_verify_enable>
 4024714:	6b16001f 	cmp	w0, w22
 4024718:	54fffd20 	b.eq	40246bc <handle_tee_code+0xb8>  // b.none
 402471c:	b9402681 	ldr	w1, [x20, #36]
 4024720:	9100a296 	add	x22, x20, #0x28
 4024724:	aa1603e2 	mov	x2, x22
 4024728:	aa1503e0 	mov	x0, x21
 402472c:	97fffc0a 	bl	4023754 <hash_compare>
 4024730:	7102941f 	cmp	w0, #0xa5
 4024734:	54fff7e1 	b.ne	4024630 <handle_tee_code+0x2c>  // b.any
 4024738:	b9402681 	ldr	w1, [x20, #36]
 402473c:	aa1603e2 	mov	x2, x22
 4024740:	aa1503e0 	mov	x0, x21
 4024744:	97fffc04 	bl	4023754 <hash_compare>
 4024748:	7102941f 	cmp	w0, #0xa5
 402474c:	54fffbe0 	b.eq	40246c8 <handle_tee_code+0xc4>  // b.none
 4024750:	17ffffb8 	b	4024630 <handle_tee_code+0x2c>
 4024754:	94000afe 	bl	402734c <is_tee_verify_enable>
 4024758:	6b16001f 	cmp	w0, w22
 402475c:	54fffe01 	b.ne	402471c <handle_tee_code+0x118>  // b.any
 4024760:	17ffffda 	b	40246c8 <handle_tee_code+0xc4>

0000000004024764 <decrypt_data>:
 4024764:	d103c3ff 	sub	sp, sp, #0xf0
 4024768:	a90b53f3 	stp	x19, x20, [sp, #176]
 402476c:	aa0103f3 	mov	x19, x1
 4024770:	aa0303f4 	mov	x20, x3
 4024774:	a90c5bf5 	stp	x21, x22, [sp, #192]
 4024778:	2a0403f6 	mov	w22, w4
 402477c:	a90d63f7 	stp	x23, x24, [sp, #208]
 4024780:	aa0203f7 	mov	x23, x2
 4024784:	d2800002 	mov	x2, #0x0                   	// #0
 4024788:	a90e7bf9 	stp	x25, x30, [sp, #224]
 402478c:	2a0003f9 	mov	w25, w0
 4024790:	b0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4024794:	f9477001 	ldr	x1, [x0, #3808]
 4024798:	f90057e1 	str	x1, [sp, #168]
 402479c:	d2800001 	mov	x1, #0x0                   	// #0
 40247a0:	910023e0 	add	x0, sp, #0x8
 40247a4:	910063e1 	add	x1, sp, #0x18
 40247a8:	29037fff 	stp	wzr, wzr, [sp, #24]
 40247ac:	94001067 	bl	4028948 <uapi_drv_cipher_create>
 40247b0:	7102941f 	cmp	w0, #0xa5
 40247b4:	54000160 	b.eq	40247e0 <decrypt_data+0x7c>  // b.none
 40247b8:	528008c0 	mov	w0, #0x46                  	// #70
 40247bc:	52800b53 	mov	w19, #0x5a                  	// #90
 40247c0:	94000652 	bl	4026108 <err_print>
 40247c4:	b0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40247c8:	f94057e1 	ldr	x1, [sp, #168]
 40247cc:	f9477002 	ldr	x2, [x0, #3808]
 40247d0:	eb020021 	subs	x1, x1, x2
 40247d4:	d2800002 	mov	x2, #0x0                   	// #0
 40247d8:	540017a0 	b.eq	4024acc <decrypt_data+0x368>  // b.none
 40247dc:	94000688 	bl	40261fc <__stack_chk_fail>
 40247e0:	9400108a 	bl	4028a08 <uapi_drv_keyslot_init>
 40247e4:	7102941f 	cmp	w0, #0xa5
 40247e8:	540000a0 	b.eq	40247fc <decrypt_data+0x98>  // b.none
 40247ec:	f94007e0 	ldr	x0, [sp, #8]
 40247f0:	9400105a 	bl	4028958 <uapi_drv_cipher_destroy>
 40247f4:	528008e0 	mov	w0, #0x47                  	// #71
 40247f8:	17fffff1 	b	40247bc <decrypt_data+0x58>
 40247fc:	910043e0 	add	x0, sp, #0x10
 4024800:	52800001 	mov	w1, #0x0                   	// #0
 4024804:	94001089 	bl	4028a28 <uapi_drv_keyslot_create>
 4024808:	7102941f 	cmp	w0, #0xa5
 402480c:	540000c0 	b.eq	4024824 <decrypt_data+0xc0>  // b.none
 4024810:	94001082 	bl	4028a18 <uapi_drv_keyslot_deinit>
 4024814:	f94007e0 	ldr	x0, [sp, #8]
 4024818:	94001050 	bl	4028958 <uapi_drv_cipher_destroy>
 402481c:	52800900 	mov	w0, #0x48                  	// #72
 4024820:	17ffffe7 	b	40247bc <decrypt_data+0x58>
 4024824:	a94087e0 	ldp	x0, x1, [sp, #8]
 4024828:	d2800002 	mov	x2, #0x0                   	// #0
 402482c:	94001057 	bl	4028988 <uapi_drv_cipher_attach>
 4024830:	7102941f 	cmp	w0, #0xa5
 4024834:	54000100 	b.eq	4024854 <decrypt_data+0xf0>  // b.none
 4024838:	94001078 	bl	4028a18 <uapi_drv_keyslot_deinit>
 402483c:	f94007e0 	ldr	x0, [sp, #8]
 4024840:	94001046 	bl	4028958 <uapi_drv_cipher_destroy>
 4024844:	f9400be0 	ldr	x0, [sp, #16]
 4024848:	9400107c 	bl	4028a38 <uapi_drv_keyslot_destroy>
 402484c:	52800920 	mov	w0, #0x49                  	// #73
 4024850:	17ffffdb 	b	40247bc <decrypt_data+0x58>
 4024854:	52800b40 	mov	w0, #0x5a                  	// #90
 4024858:	b90003e0 	str	w0, [sp]
 402485c:	d2a203c0 	mov	x0, #0x101e0000            	// #270401536
 4024860:	91009262 	add	x2, x19, #0x24
 4024864:	52800021 	mov	w1, #0x1                   	// #1
 4024868:	b9400000 	ldr	w0, [x0]
 402486c:	f94007f8 	ldr	x24, [sp, #8]
 4024870:	360003a0 	tbz	w0, #0, 40248e4 <decrypt_data+0x180>
 4024874:	52800040 	mov	w0, #0x2                   	// #2
 4024878:	b9005be0 	str	w0, [sp, #88]
 402487c:	9101c3e0 	add	x0, sp, #0x70
 4024880:	f90037e0 	str	x0, [sp, #104]
 4024884:	9101e3e0 	add	x0, sp, #0x78
 4024888:	290e07ff 	stp	wzr, w1, [sp, #112]
 402488c:	d2800203 	mov	x3, #0x10                  	// #16
 4024890:	d2800004 	mov	x4, #0x0                   	// #0
 4024894:	aa0303e1 	mov	x1, x3
 4024898:	94001120 	bl	4028d18 <memcpy_ss>
 402489c:	b90003e0 	str	w0, [sp]
 40248a0:	b94003e0 	ldr	w0, [sp]
 40248a4:	7102941f 	cmp	w0, #0xa5
 40248a8:	540002e0 	b.eq	4024904 <decrypt_data+0x1a0>  // b.none
 40248ac:	52800960 	mov	w0, #0x4b                  	// #75
 40248b0:	52800b53 	mov	w19, #0x5a                  	// #90
 40248b4:	94000615 	bl	4026108 <err_print>
 40248b8:	528009a0 	mov	w0, #0x4d                  	// #77
 40248bc:	94000613 	bl	4026108 <err_print>
 40248c0:	a94087e0 	ldp	x0, x1, [sp, #8]
 40248c4:	d2800002 	mov	x2, #0x0                   	// #0
 40248c8:	94001034 	bl	4028998 <uapi_drv_cipher_detach>
 40248cc:	f9400be0 	ldr	x0, [sp, #16]
 40248d0:	9400105a 	bl	4028a38 <uapi_drv_keyslot_destroy>
 40248d4:	f94007e0 	ldr	x0, [sp, #8]
 40248d8:	94001020 	bl	4028958 <uapi_drv_cipher_destroy>
 40248dc:	9400104f 	bl	4028a18 <uapi_drv_keyslot_deinit>
 40248e0:	17ffffb9 	b	40247c4 <decrypt_data+0x60>
 40248e4:	52800060 	mov	w0, #0x3                   	// #3
 40248e8:	291207e0 	stp	w0, w1, [sp, #144]
 40248ec:	910223e0 	add	x0, sp, #0x88
 40248f0:	b9005be1 	str	w1, [sp, #88]
 40248f4:	f90037e0 	str	x0, [sp, #104]
 40248f8:	910263e0 	add	x0, sp, #0x98
 40248fc:	29117fe1 	stp	w1, wzr, [sp, #136]
 4024900:	17ffffe3 	b	402488c <decrypt_data+0x128>
 4024904:	528000a0 	mov	w0, #0x5                   	// #5
 4024908:	910163e1 	add	x1, sp, #0x58
 402490c:	52800035 	mov	w21, #0x1                   	// #1
 4024910:	d2800002 	mov	x2, #0x0                   	// #0
 4024914:	290b83f5 	stp	w21, w0, [sp, #92]
 4024918:	aa1803e0 	mov	x0, x24
 402491c:	94001017 	bl	4028978 <uapi_drv_cipher_set_config>
 4024920:	b90003e0 	str	w0, [sp]
 4024924:	b94003e0 	ldr	w0, [sp]
 4024928:	7102941f 	cmp	w0, #0xa5
 402492c:	54000060 	b.eq	4024938 <decrypt_data+0x1d4>  // b.none
 4024930:	52800980 	mov	w0, #0x4c                  	// #76
 4024934:	17ffffdf 	b	40248b0 <decrypt_data+0x14c>
 4024938:	52800b40 	mov	w0, #0x5a                  	// #90
 402493c:	b90007e0 	str	w0, [sp, #4]
 4024940:	d2a203c0 	mov	x0, #0x101e0000            	// #270401536
 4024944:	52800402 	mov	w2, #0x20                  	// #32
 4024948:	52800041 	mov	w1, #0x2                   	// #2
 402494c:	b9400000 	ldr	w0, [x0]
 4024950:	790063f5 	strh	w21, [sp, #48]
 4024954:	72000000 	ands	w0, w0, #0x1
 4024958:	b90037f5 	str	w21, [sp, #52]
 402495c:	1a951021 	csel	w1, w1, w21, ne  // ne = any
 4024960:	52800a00 	mov	w0, #0x50                  	// #80
 4024964:	1a821000 	csel	w0, w0, w2, ne  // ne = any
 4024968:	91001262 	add	x2, x19, #0x4
 402496c:	290503f9 	stp	w25, w0, [sp, #40]
 4024970:	3200c3e0 	mov	w0, #0x1010101             	// #16843009
 4024974:	b9003be0 	str	w0, [sp, #56]
 4024978:	52800200 	mov	w0, #0x10                  	// #16
 402497c:	91005273 	add	x19, x19, #0x14
 4024980:	3900f3f5 	strb	w21, [sp, #60]
 4024984:	b90043e1 	str	w1, [sp, #64]
 4024988:	b90047e0 	str	w0, [sp, #68]
 402498c:	f90027f3 	str	x19, [sp, #72]
 4024990:	b90053ff 	str	wzr, [sp, #80]
 4024994:	290b07ff 	stp	wzr, w1, [sp, #88]
 4024998:	b90063e0 	str	w0, [sp, #96]
 402499c:	910083e0 	add	x0, sp, #0x20
 40249a0:	f90037e2 	str	x2, [sp, #104]
 40249a4:	f9400bf8 	ldr	x24, [sp, #16]
 40249a8:	94001028 	bl	4028a48 <uapi_drv_klad_create>
 40249ac:	7102941f 	cmp	w0, #0xa5
 40249b0:	54000861 	b.ne	4024abc <decrypt_data+0x358>  // b.any
 40249b4:	f94013e0 	ldr	x0, [sp, #32]
 40249b8:	aa1803e2 	mov	x2, x24
 40249bc:	d2800003 	mov	x3, #0x0                   	// #0
 40249c0:	52800001 	mov	w1, #0x0                   	// #0
 40249c4:	94001029 	bl	4028a68 <uapi_drv_klad_attach>
 40249c8:	7102941f 	cmp	w0, #0xa5
 40249cc:	540003c1 	b.ne	4024a44 <decrypt_data+0x2e0>  // b.any
 40249d0:	f94013e0 	ldr	x0, [sp, #32]
 40249d4:	9100a3e1 	add	x1, sp, #0x28
 40249d8:	d2800002 	mov	x2, #0x0                   	// #0
 40249dc:	9400102b 	bl	4028a88 <uapi_drv_klad_set_attr>
 40249e0:	7102941f 	cmp	w0, #0xa5
 40249e4:	540001e1 	b.ne	4024a20 <decrypt_data+0x2bc>  // b.any
 40249e8:	f94013e0 	ldr	x0, [sp, #32]
 40249ec:	910163e1 	add	x1, sp, #0x58
 40249f0:	d2800002 	mov	x2, #0x0                   	// #0
 40249f4:	94001029 	bl	4028a98 <uapi_drv_klad_set_session_key>
 40249f8:	7102941f 	cmp	w0, #0xa5
 40249fc:	54000121 	b.ne	4024a20 <decrypt_data+0x2bc>  // b.any
 4024a00:	f94013e0 	ldr	x0, [sp, #32]
 4024a04:	910103e1 	add	x1, sp, #0x40
 4024a08:	d2800003 	mov	x3, #0x0                   	// #0
 4024a0c:	52800002 	mov	w2, #0x0                   	// #0
 4024a10:	94001026 	bl	4028aa8 <uapi_drv_klad_set_content_key>
 4024a14:	7102941f 	cmp	w0, #0xa5
 4024a18:	54000041 	b.ne	4024a20 <decrypt_data+0x2bc>  // b.any
 4024a1c:	b90007e0 	str	w0, [sp, #4]
 4024a20:	f94013e0 	ldr	x0, [sp, #32]
 4024a24:	aa1803e2 	mov	x2, x24
 4024a28:	d2800003 	mov	x3, #0x0                   	// #0
 4024a2c:	52800001 	mov	w1, #0x0                   	// #0
 4024a30:	94001012 	bl	4028a78 <uapi_drv_klad_detach>
 4024a34:	7102941f 	cmp	w0, #0xa5
 4024a38:	54000060 	b.eq	4024a44 <decrypt_data+0x2e0>  // b.none
 4024a3c:	52800b40 	mov	w0, #0x5a                  	// #90
 4024a40:	b90007e0 	str	w0, [sp, #4]
 4024a44:	f94013e0 	ldr	x0, [sp, #32]
 4024a48:	94001004 	bl	4028a58 <uapi_drv_klad_destroy>
 4024a4c:	7102941f 	cmp	w0, #0xa5
 4024a50:	54000361 	b.ne	4024abc <decrypt_data+0x358>  // b.any
 4024a54:	b94007f3 	ldr	w19, [sp, #4]
 4024a58:	7102967f 	cmp	w19, #0xa5
 4024a5c:	54000321 	b.ne	4024ac0 <decrypt_data+0x35c>  // b.any
 4024a60:	52800020 	mov	w0, #0x1                   	// #1
 4024a64:	b9004be0 	str	w0, [sp, #72]
 4024a68:	b90063e0 	str	w0, [sp, #96]
 4024a6c:	92407e94 	and	x20, x20, #0xffffffff
 4024a70:	f94007e0 	ldr	x0, [sp, #8]
 4024a74:	2a1603e3 	mov	w3, w22
 4024a78:	910163e2 	add	x2, sp, #0x58
 4024a7c:	910103e1 	add	x1, sp, #0x40
 4024a80:	52800b55 	mov	w21, #0x5a                  	// #90
 4024a84:	d2800005 	mov	x5, #0x0                   	// #0
 4024a88:	52800004 	mov	w4, #0x0                   	// #0
 4024a8c:	b9002bf5 	str	w21, [sp, #40]
 4024a90:	f90023f4 	str	x20, [sp, #64]
 4024a94:	f9002ff7 	str	x23, [sp, #88]
 4024a98:	94000fc4 	bl	40289a8 <uapi_drv_cipher_decrypt>
 4024a9c:	b9002be0 	str	w0, [sp, #40]
 4024aa0:	b9402bf3 	ldr	w19, [sp, #40]
 4024aa4:	7102967f 	cmp	w19, #0xa5
 4024aa8:	54fff0c0 	b.eq	40248c0 <decrypt_data+0x15c>  // b.none
 4024aac:	528009e0 	mov	w0, #0x4f                  	// #79
 4024ab0:	2a1503f3 	mov	w19, w21
 4024ab4:	94000595 	bl	4026108 <err_print>
 4024ab8:	17ffff82 	b	40248c0 <decrypt_data+0x15c>
 4024abc:	52800b53 	mov	w19, #0x5a                  	// #90
 4024ac0:	528009c0 	mov	w0, #0x4e                  	// #78
 4024ac4:	94000591 	bl	4026108 <err_print>
 4024ac8:	17ffff7e 	b	40248c0 <decrypt_data+0x15c>
 4024acc:	2a1303e0 	mov	w0, w19
 4024ad0:	a94b53f3 	ldp	x19, x20, [sp, #176]
 4024ad4:	a94c5bf5 	ldp	x21, x22, [sp, #192]
 4024ad8:	a94d63f7 	ldp	x23, x24, [sp, #208]
 4024adc:	a94e7bf9 	ldp	x25, x30, [sp, #224]
 4024ae0:	9103c3ff 	add	sp, sp, #0xf0
 4024ae4:	d65f03c0 	ret

0000000004024ae8 <calc_hash>:
 4024ae8:	d10183ff 	sub	sp, sp, #0x60
 4024aec:	2a0003e0 	mov	w0, w0
 4024af0:	a90453f3 	stp	x19, x20, [sp, #64]
 4024af4:	2a0103f3 	mov	w19, w1
 4024af8:	b0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4024afc:	a9057bf5 	stp	x21, x30, [sp, #80]
 4024b00:	aa0203f5 	mov	x21, x2
 4024b04:	52800b54 	mov	w20, #0x5a                  	// #90
 4024b08:	f9477022 	ldr	x2, [x1, #3808]
 4024b0c:	f9001fe2 	str	x2, [sp, #56]
 4024b10:	d2800002 	mov	x2, #0x0                   	// #0
 4024b14:	d2a203c1 	mov	x1, #0x101e0000            	// #270401536
 4024b18:	b90007f4 	str	w20, [sp, #4]
 4024b1c:	52800042 	mov	w2, #0x2                   	// #2
 4024b20:	b9400021 	ldr	w1, [x1]
 4024b24:	a90203ff 	stp	xzr, x0, [sp, #32]
 4024b28:	52800020 	mov	w0, #0x1                   	// #1
 4024b2c:	f240003f 	tst	x1, #0x1
 4024b30:	52800201 	mov	w1, #0x10                  	// #16
 4024b34:	1a821021 	csel	w1, w1, w2, ne  // ne = any
 4024b38:	b90033e0 	str	w0, [sp, #48]
 4024b3c:	52800400 	mov	w0, #0x20                  	// #32
 4024b40:	b9000bf4 	str	w20, [sp, #8]
 4024b44:	b9000fe0 	str	w0, [sp, #12]
 4024b48:	910043e0 	add	x0, sp, #0x10
 4024b4c:	b9001be1 	str	w1, [sp, #24]
 4024b50:	910063e1 	add	x1, sp, #0x18
 4024b54:	ca010002 	eor	x2, x0, x1
 4024b58:	b9000bf4 	str	w20, [sp, #8]
 4024b5c:	94000f97 	bl	40289b8 <uapi_drv_cipher_hash_init>
 4024b60:	b9000be0 	str	w0, [sp, #8]
 4024b64:	b9400be0 	ldr	w0, [sp, #8]
 4024b68:	7102941f 	cmp	w0, #0xa5
 4024b6c:	54000160 	b.eq	4024b98 <calc_hash+0xb0>  // b.none
 4024b70:	b9400be0 	ldr	w0, [sp, #8]
 4024b74:	b0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4024b78:	b90007e0 	str	w0, [sp, #4]
 4024b7c:	b94007e0 	ldr	w0, [sp, #4]
 4024b80:	f9401fe2 	ldr	x2, [sp, #56]
 4024b84:	f9477023 	ldr	x3, [x1, #3808]
 4024b88:	eb030042 	subs	x2, x2, x3
 4024b8c:	d2800003 	mov	x3, #0x0                   	// #0
 4024b90:	54000240 	b.eq	4024bd8 <calc_hash+0xf0>  // b.none
 4024b94:	9400059a 	bl	40261fc <__stack_chk_fail>
 4024b98:	f9400be0 	ldr	x0, [sp, #16]
 4024b9c:	9100a3e1 	add	x1, sp, #0x28
 4024ba0:	2a1303e2 	mov	w2, w19
 4024ba4:	52800003 	mov	w3, #0x0                   	// #0
 4024ba8:	ca000024 	eor	x4, x1, x0
 4024bac:	b9000bf4 	str	w20, [sp, #8]
 4024bb0:	ca020084 	eor	x4, x4, x2
 4024bb4:	2a1303e2 	mov	w2, w19
 4024bb8:	94000f84 	bl	40289c8 <uapi_drv_cipher_hash_update>
 4024bbc:	b9000be0 	str	w0, [sp, #8]
 4024bc0:	f9400be0 	ldr	x0, [sp, #16]
 4024bc4:	910033e2 	add	x2, sp, #0xc
 4024bc8:	aa1503e1 	mov	x1, x21
 4024bcc:	d2800003 	mov	x3, #0x0                   	// #0
 4024bd0:	94000f82 	bl	40289d8 <uapi_drv_cipher_hash_final>
 4024bd4:	17ffffe7 	b	4024b70 <calc_hash+0x88>
 4024bd8:	a94453f3 	ldp	x19, x20, [sp, #64]
 4024bdc:	a9457bf5 	ldp	x21, x30, [sp, #80]
 4024be0:	910183ff 	add	sp, sp, #0x60
 4024be4:	d65f03c0 	ret

0000000004024be8 <store_hash_to_lpds>:
 4024be8:	f81e0ffe 	str	x30, [sp, #-32]!
 4024bec:	52800b41 	mov	w1, #0x5a                  	// #90
 4024bf0:	d2800403 	mov	x3, #0x20                  	// #32
 4024bf4:	d2800004 	mov	x4, #0x0                   	// #0
 4024bf8:	b9001fe1 	str	w1, [sp, #28]
 4024bfc:	b0ffffe2 	adrp	x2, 4021000 <gs_misc_record-0xa80>
 4024c00:	912a8042 	add	x2, x2, #0xaa0
 4024c04:	b9001fe1 	str	w1, [sp, #28]
 4024c08:	aa0303e1 	mov	x1, x3
 4024c0c:	94001043 	bl	4028d18 <memcpy_ss>
 4024c10:	b9001fe0 	str	w0, [sp, #28]
 4024c14:	b9401fe0 	ldr	w0, [sp, #28]
 4024c18:	7102941f 	cmp	w0, #0xa5
 4024c1c:	54000180 	b.eq	4024c4c <store_hash_to_lpds+0x64>  // b.none
 4024c20:	52800ce0 	mov	w0, #0x67                  	// #103
 4024c24:	94000539 	bl	4026108 <err_print>
 4024c28:	d2800040 	mov	x0, #0x2                   	// #2
 4024c2c:	94001027 	bl	4028cc8 <mdelay>
 4024c30:	94001016 	bl	4028c88 <timer_deinit>
 4024c34:	d2800080 	mov	x0, #0x4                   	// #4
 4024c38:	52800021 	mov	w1, #0x1                   	// #1
 4024c3c:	f2a22040 	movk	x0, #0x1102, lsl #16
 4024c40:	b9000001 	str	w1, [x0]
 4024c44:	b9000001 	str	w1, [x0]
 4024c48:	b9000001 	str	w1, [x0]
 4024c4c:	f84207fe 	ldr	x30, [sp], #32
 4024c50:	d65f03c0 	ret

0000000004024c54 <verify_signature>:
 4024c54:	d10403ff 	sub	sp, sp, #0x100
 4024c58:	a90c53f3 	stp	x19, x20, [sp, #192]
 4024c5c:	aa0003f3 	mov	x19, x0
 4024c60:	b0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4024c64:	a90d5bf5 	stp	x21, x22, [sp, #208]
 4024c68:	aa0103f6 	mov	x22, x1
 4024c6c:	a90e63f7 	stp	x23, x24, [sp, #224]
 4024c70:	f9007bfe 	str	x30, [sp, #240]
 4024c74:	f9477001 	ldr	x1, [x0, #3808]
 4024c78:	f9005fe1 	str	x1, [sp, #184]
 4024c7c:	d2800001 	mov	x1, #0x0                   	// #0
 4024c80:	b5000123 	cbnz	x3, 4024ca4 <verify_signature+0x50>
 4024c84:	52800b53 	mov	w19, #0x5a                  	// #90
 4024c88:	b0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4024c8c:	f9405fe1 	ldr	x1, [sp, #184]
 4024c90:	f9477002 	ldr	x2, [x0, #3808]
 4024c94:	eb020021 	subs	x1, x1, x2
 4024c98:	d2800002 	mov	x2, #0x0                   	// #0
 4024c9c:	54000ca0 	b.eq	4024e30 <verify_signature+0x1dc>  // b.none
 4024ca0:	94000557 	bl	40261fc <__stack_chk_fail>
 4024ca4:	f9400460 	ldr	x0, [x3, #8]
 4024ca8:	aa0303f4 	mov	x20, x3
 4024cac:	b4fffec0 	cbz	x0, 4024c84 <verify_signature+0x30>
 4024cb0:	b9400075 	ldr	w21, [x3]
 4024cb4:	710082bf 	cmp	w21, #0x20
 4024cb8:	54fffe61 	b.ne	4024c84 <verify_signature+0x30>  // b.any
 4024cbc:	b4fffe53 	cbz	x19, 4024c84 <verify_signature+0x30>
 4024cc0:	9101e3f7 	add	x23, sp, #0x78
 4024cc4:	aa0203f8 	mov	x24, x2
 4024cc8:	d2800403 	mov	x3, #0x20                  	// #32
 4024ccc:	aa1703e0 	mov	x0, x23
 4024cd0:	aa0303e1 	mov	x1, x3
 4024cd4:	d2800004 	mov	x4, #0x0                   	// #0
 4024cd8:	52800b42 	mov	w2, #0x5a                  	// #90
 4024cdc:	94001013 	bl	4028d28 <memset_ss>
 4024ce0:	7102941f 	cmp	w0, #0xa5
 4024ce4:	54fffd01 	b.ne	4024c84 <verify_signature+0x30>  // b.any
 4024ce8:	b9400283 	ldr	w3, [x20]
 4024cec:	d2800004 	mov	x4, #0x0                   	// #0
 4024cf0:	f9400680 	ldr	x0, [x20, #8]
 4024cf4:	aa0303e1 	mov	x1, x3
 4024cf8:	12800b42 	mov	w2, #0xffffffa5            	// #-91
 4024cfc:	9400100b 	bl	4028d28 <memset_ss>
 4024d00:	7102941f 	cmp	w0, #0xa5
 4024d04:	54fffc01 	b.ne	4024c84 <verify_signature+0x30>  // b.any
 4024d08:	d2a203c0 	mov	x0, #0x101e0000            	// #270401536
 4024d0c:	b90003f5 	str	w21, [sp]
 4024d10:	b9400000 	ldr	w0, [x0]
 4024d14:	360004a0 	tbz	w0, #0, 4024da8 <verify_signature+0x154>
 4024d18:	910263e0 	add	x0, sp, #0x98
 4024d1c:	f90007e0 	str	x0, [sp, #8]
 4024d20:	b0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4024d24:	912f2000 	add	x0, x0, #0xbc8
 4024d28:	f9000fe0 	str	x0, [sp, #24]
 4024d2c:	52800200 	mov	w0, #0x10                  	// #16
 4024d30:	b90013e0 	str	w0, [sp, #16]
 4024d34:	52800080 	mov	w0, #0x4                   	// #4
 4024d38:	b9400261 	ldr	w1, [x19]
 4024d3c:	910003e3 	mov	x3, sp
 4024d40:	b90073e0 	str	w0, [sp, #112]
 4024d44:	52800020 	mov	w0, #0x1                   	// #1
 4024d48:	f9400662 	ldr	x2, [x19, #8]
 4024d4c:	d2800004 	mov	x4, #0x0                   	// #0
 4024d50:	b90023e1 	str	w1, [sp, #32]
 4024d54:	aa1603e1 	mov	x1, x22
 4024d58:	f90017e2 	str	x2, [sp, #40]
 4024d5c:	910083e2 	add	x2, sp, #0x20
 4024d60:	b90033e0 	str	w0, [sp, #48]
 4024d64:	910043e0 	add	x0, sp, #0x10
 4024d68:	94000f74 	bl	4028b38 <uapi_drv_pke_sm2_dsa_hash>
 4024d6c:	7102941f 	cmp	w0, #0xa5
 4024d70:	54fff8a1 	b.ne	4024c84 <verify_signature+0x30>  // b.any
 4024d74:	9100e3f5 	add	x21, sp, #0x38
 4024d78:	b9400261 	ldr	w1, [x19]
 4024d7c:	aa1703e2 	mov	x2, x23
 4024d80:	b9400a60 	ldr	w0, [x19, #8]
 4024d84:	52800004 	mov	w4, #0x0                   	// #0
 4024d88:	52800403 	mov	w3, #0x20                  	// #32
 4024d8c:	97ffff57 	bl	4024ae8 <calc_hash>
 4024d90:	2a0003f3 	mov	w19, w0
 4024d94:	7102941f 	cmp	w0, #0xa5
 4024d98:	54000100 	b.eq	4024db8 <verify_signature+0x164>  // b.none
 4024d9c:	52800a40 	mov	w0, #0x52                  	// #82
 4024da0:	940004da 	bl	4026108 <err_print>
 4024da4:	17ffffb9 	b	4024c88 <verify_signature+0x34>
 4024da8:	b0000075 	adrp	x21, 4031000 <sync_exception_sp_el0>
 4024dac:	911542b5 	add	x21, x21, #0x550
 4024db0:	f90007f7 	str	x23, [sp, #8]
 4024db4:	17fffff1 	b	4024d78 <verify_signature+0x124>
 4024db8:	b0fffff3 	adrp	x19, 4021000 <gs_misc_record-0xa80>
 4024dbc:	912a8273 	add	x19, x19, #0xaa0
 4024dc0:	d2800403 	mov	x3, #0x20                  	// #32
 4024dc4:	aa1303e0 	mov	x0, x19
 4024dc8:	aa0303e1 	mov	x1, x3
 4024dcc:	d2800004 	mov	x4, #0x0                   	// #0
 4024dd0:	52800002 	mov	w2, #0x0                   	// #0
 4024dd4:	94000fd5 	bl	4028d28 <memset_ss>
 4024dd8:	7102941f 	cmp	w0, #0xa5
 4024ddc:	54fff541 	b.ne	4024c84 <verify_signature+0x30>  // b.any
 4024de0:	d2800403 	mov	x3, #0x20                  	// #32
 4024de4:	aa1703e2 	mov	x2, x23
 4024de8:	aa0303e1 	mov	x1, x3
 4024dec:	aa1303e0 	mov	x0, x19
 4024df0:	d2800004 	mov	x4, #0x0                   	// #0
 4024df4:	94000fc9 	bl	4028d18 <memcpy_ss>
 4024df8:	7102941f 	cmp	w0, #0xa5
 4024dfc:	54fff441 	b.ne	4024c84 <verify_signature+0x30>  // b.any
 4024e00:	f9400684 	ldr	x4, [x20, #8]
 4024e04:	aa1803e3 	mov	x3, x24
 4024e08:	910003e2 	mov	x2, sp
 4024e0c:	aa1603e1 	mov	x1, x22
 4024e10:	aa1503e0 	mov	x0, x21
 4024e14:	d2800005 	mov	x5, #0x0                   	// #0
 4024e18:	94000f44 	bl	4028b28 <uapi_drv_pke_ecdsa_verify>
 4024e1c:	2a0003f3 	mov	w19, w0
 4024e20:	7102941f 	cmp	w0, #0xa5
 4024e24:	54fff320 	b.eq	4024c88 <verify_signature+0x34>  // b.none
 4024e28:	52800a60 	mov	w0, #0x53                  	// #83
 4024e2c:	17ffffdd 	b	4024da0 <verify_signature+0x14c>
 4024e30:	2a1303e0 	mov	w0, w19
 4024e34:	a94c53f3 	ldp	x19, x20, [sp, #192]
 4024e38:	a94d5bf5 	ldp	x21, x22, [sp, #208]
 4024e3c:	a94e63f7 	ldp	x23, x24, [sp, #224]
 4024e40:	f9407bfe 	ldr	x30, [sp, #240]
 4024e44:	910403ff 	add	sp, sp, #0x100
 4024e48:	d65f03c0 	ret

0000000004024e4c <dma_copy>:
 4024e4c:	d10103ff 	sub	sp, sp, #0x40
 4024e50:	b0ffffe3 	adrp	x3, 4021000 <gs_misc_record-0xa80>
 4024e54:	f9001bfe 	str	x30, [sp, #48]
 4024e58:	f9477064 	ldr	x4, [x3, #3808]
 4024e5c:	f90017e4 	str	x4, [sp, #40]
 4024e60:	d2800004 	mov	x4, #0x0                   	// #0
 4024e64:	f90007e0 	str	x0, [sp, #8]
 4024e68:	52800020 	mov	w0, #0x1                   	// #1
 4024e6c:	d2800003 	mov	x3, #0x0                   	// #0
 4024e70:	b90013e0 	str	w0, [sp, #16]
 4024e74:	f9000fe2 	str	x2, [sp, #24]
 4024e78:	2a0103e2 	mov	w2, w1
 4024e7c:	910063e1 	add	x1, sp, #0x18
 4024e80:	b90023e0 	str	w0, [sp, #32]
 4024e84:	910023e0 	add	x0, sp, #0x8
 4024e88:	94000eb8 	bl	4028968 <uapi_drv_cipher_dma_copy>
 4024e8c:	b0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4024e90:	f94017e2 	ldr	x2, [sp, #40]
 4024e94:	f9477023 	ldr	x3, [x1, #3808]
 4024e98:	eb030042 	subs	x2, x2, x3
 4024e9c:	d2800003 	mov	x3, #0x0                   	// #0
 4024ea0:	54000040 	b.eq	4024ea8 <dma_copy+0x5c>  // b.none
 4024ea4:	940004d6 	bl	40261fc <__stack_chk_fail>
 4024ea8:	f9401bfe 	ldr	x30, [sp, #48]
 4024eac:	910103ff 	add	sp, sp, #0x40
 4024eb0:	d65f03c0 	ret

0000000004024eb4 <get_misc_data_from_flash>:
 4024eb4:	d10103ff 	sub	sp, sp, #0x40
 4024eb8:	a90153f3 	stp	x19, x20, [sp, #16]
 4024ebc:	12001c34 	and	w20, w1, #0xff
 4024ec0:	aa0303f3 	mov	x19, x3
 4024ec4:	a9025bf5 	stp	x21, x22, [sp, #32]
 4024ec8:	2a0003f5 	mov	w21, w0
 4024ecc:	b0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4024ed0:	f9001bfe 	str	x30, [sp, #48]
 4024ed4:	12001c56 	and	w22, w2, #0xff
 4024ed8:	f9477001 	ldr	x1, [x0, #3808]
 4024edc:	f90007e1 	str	x1, [sp, #8]
 4024ee0:	d2800001 	mov	x1, #0x0                   	// #0
 4024ee4:	97fff9ea 	bl	402368c <get_partition_area_addr>
 4024ee8:	2a0003e0 	mov	w0, w0
 4024eec:	b0000062 	adrp	x2, 4031000 <sync_exception_sp_el0>
 4024ef0:	910003e1 	mov	x1, sp
 4024ef4:	912f6442 	add	x2, x2, #0xbd9
 4024ef8:	940008e8 	bl	4027298 <ot_partition_get_partition_info_by_name>
 4024efc:	7102941f 	cmp	w0, #0xa5
 4024f00:	540000c0 	b.eq	4024f18 <get_misc_data_from_flash+0x64>  // b.none
 4024f04:	b0000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4024f08:	912f7800 	add	x0, x0, #0xbde
 4024f0c:	94000f53 	bl	4028c58 <log_serial_puts>
 4024f10:	52800b40 	mov	w0, #0x5a                  	// #90
 4024f14:	14000015 	b	4024f68 <get_misc_data_from_flash+0xb4>
 4024f18:	f94003e0 	ldr	x0, [sp]
 4024f1c:	39410001 	ldrb	w1, [x0, #64]
 4024f20:	39410402 	ldrb	w2, [x0, #65]
 4024f24:	aa022022 	orr	x2, x1, x2, lsl #8
 4024f28:	39410801 	ldrb	w1, [x0, #66]
 4024f2c:	aa014042 	orr	x2, x2, x1, lsl #16
 4024f30:	39410c01 	ldrb	w1, [x0, #67]
 4024f34:	52896f00 	mov	w0, #0x4b78                	// #19320
 4024f38:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 4024f3c:	aa016041 	orr	x1, x2, x1, lsl #24
 4024f40:	0b163036 	add	w22, w1, w22, lsl #12
 4024f44:	0b144ad4 	add	w20, w22, w20, lsl #18
 4024f48:	6b0002bf 	cmp	w21, w0
 4024f4c:	54000221 	b.ne	4024f90 <get_misc_data_from_flash+0xdc>  // b.any
 4024f50:	94001288 	bl	4029970 <is_nand>
 4024f54:	2a1403e2 	mov	w2, w20
 4024f58:	52800401 	mov	w1, #0x20                  	// #32
 4024f5c:	34000140 	cbz	w0, 4024f84 <get_misc_data_from_flash+0xd0>
 4024f60:	aa1303e0 	mov	x0, x19
 4024f64:	94001238 	bl	4029844 <spi_nand_read>
 4024f68:	b0ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4024f6c:	f94007e2 	ldr	x2, [sp, #8]
 4024f70:	f9477023 	ldr	x3, [x1, #3808]
 4024f74:	eb030042 	subs	x2, x2, x3
 4024f78:	d2800003 	mov	x3, #0x0                   	// #0
 4024f7c:	540001c0 	b.eq	4024fb4 <get_misc_data_from_flash+0x100>  // b.none
 4024f80:	9400049f 	bl	40261fc <__stack_chk_fail>
 4024f84:	aa1303e0 	mov	x0, x19
 4024f88:	94001485 	bl	402a19c <spi_nor_read>
 4024f8c:	17fffff7 	b	4024f68 <get_misc_data_from_flash+0xb4>
 4024f90:	529690e0 	mov	w0, #0xb487                	// #46215
 4024f94:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4024f98:	6b0002bf 	cmp	w21, w0
 4024f9c:	54fffba1 	b.ne	4024f10 <get_misc_data_from_flash+0x5c>  // b.any
 4024fa0:	2a1403e1 	mov	w1, w20
 4024fa4:	aa1303e0 	mov	x0, x19
 4024fa8:	52800402 	mov	w2, #0x20                  	// #32
 4024fac:	940015a1 	bl	402a630 <emmc_read>
 4024fb0:	17ffffee 	b	4024f68 <get_misc_data_from_flash+0xb4>
 4024fb4:	a94153f3 	ldp	x19, x20, [sp, #16]
 4024fb8:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4024fbc:	f9401bfe 	ldr	x30, [sp, #48]
 4024fc0:	910103ff 	add	sp, sp, #0x40
 4024fc4:	d65f03c0 	ret

0000000004024fc8 <write_misc_data_to_flash>:
 4024fc8:	d10103ff 	sub	sp, sp, #0x40
 4024fcc:	a90153f3 	stp	x19, x20, [sp, #16]
 4024fd0:	12001c33 	and	w19, w1, #0xff
 4024fd4:	12001c74 	and	w20, w3, #0xff
 4024fd8:	a9025bf5 	stp	x21, x22, [sp, #32]
 4024fdc:	aa0203f6 	mov	x22, x2
 4024fe0:	a9037bf7 	stp	x23, x30, [sp, #48]
 4024fe4:	2a0003f7 	mov	w23, w0
 4024fe8:	b0ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4024fec:	f9477001 	ldr	x1, [x0, #3808]
 4024ff0:	f90007e1 	str	x1, [sp, #8]
 4024ff4:	d2800001 	mov	x1, #0x0                   	// #0
 4024ff8:	97fff9a5 	bl	402368c <get_partition_area_addr>
 4024ffc:	2a0003e0 	mov	w0, w0
 4025000:	90000062 	adrp	x2, 4031000 <sync_exception_sp_el0>
 4025004:	910003e1 	mov	x1, sp
 4025008:	912f6442 	add	x2, x2, #0xbd9
 402500c:	940008a3 	bl	4027298 <ot_partition_get_partition_info_by_name>
 4025010:	7102941f 	cmp	w0, #0xa5
 4025014:	540000c0 	b.eq	402502c <write_misc_data_to_flash+0x64>  // b.none
 4025018:	90000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402501c:	912f7800 	add	x0, x0, #0xbde
 4025020:	94000f0e 	bl	4028c58 <log_serial_puts>
 4025024:	52800b55 	mov	w21, #0x5a                  	// #90
 4025028:	14000019 	b	402508c <write_misc_data_to_flash+0xc4>
 402502c:	2a0003f5 	mov	w21, w0
 4025030:	f94003e0 	ldr	x0, [sp]
 4025034:	39410401 	ldrb	w1, [x0, #65]
 4025038:	39410002 	ldrb	w2, [x0, #64]
 402503c:	aa012042 	orr	x2, x2, x1, lsl #8
 4025040:	39410801 	ldrb	w1, [x0, #66]
 4025044:	39410c00 	ldrb	w0, [x0, #67]
 4025048:	aa014041 	orr	x1, x2, x1, lsl #16
 402504c:	aa006020 	orr	x0, x1, x0, lsl #24
 4025050:	0b134813 	add	w19, w0, w19, lsl #18
 4025054:	52896f00 	mov	w0, #0x4b78                	// #19320
 4025058:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 402505c:	6b0002ff 	cmp	w23, w0
 4025060:	54000481 	b.ne	40250f0 <write_misc_data_to_flash+0x128>  // b.any
 4025064:	53144e94 	lsl	w20, w20, #12
 4025068:	94001242 	bl	4029970 <is_nand>
 402506c:	340002a0 	cbz	w0, 40250c0 <write_misc_data_to_flash+0xf8>
 4025070:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025074:	0b130294 	add	w20, w20, w19
 4025078:	b94ac001 	ldr	w1, [x0, #2752]
 402507c:	2a1303e0 	mov	w0, w19
 4025080:	9400115e 	bl	40295f8 <spi_nand_erase>
 4025084:	6b14027f 	cmp	w19, w20
 4025088:	54000101 	b.ne	40250a8 <write_misc_data_to_flash+0xe0>  // b.any
 402508c:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025090:	f94007e1 	ldr	x1, [sp, #8]
 4025094:	f9477002 	ldr	x2, [x0, #3808]
 4025098:	eb020021 	subs	x1, x1, x2
 402509c:	d2800002 	mov	x2, #0x0                   	// #0
 40250a0:	54000420 	b.eq	4025124 <write_misc_data_to_flash+0x15c>  // b.none
 40250a4:	94000456 	bl	40261fc <__stack_chk_fail>
 40250a8:	2a1303e2 	mov	w2, w19
 40250ac:	aa1603e0 	mov	x0, x22
 40250b0:	52800401 	mov	w1, #0x20                  	// #32
 40250b4:	11400673 	add	w19, w19, #0x1, lsl #12
 40250b8:	9400118f 	bl	40296f4 <spi_nand_write>
 40250bc:	17fffff2 	b	4025084 <write_misc_data_to_flash+0xbc>
 40250c0:	0b130294 	add	w20, w20, w19
 40250c4:	2a1303e1 	mov	w1, w19
 40250c8:	52a00020 	mov	w0, #0x10000               	// #65536
 40250cc:	940013e9 	bl	402a070 <spi_nor_erase>
 40250d0:	6b14027f 	cmp	w19, w20
 40250d4:	54fffdc0 	b.eq	402508c <write_misc_data_to_flash+0xc4>  // b.none
 40250d8:	2a1303e2 	mov	w2, w19
 40250dc:	aa1603e0 	mov	x0, x22
 40250e0:	52800401 	mov	w1, #0x20                  	// #32
 40250e4:	11400673 	add	w19, w19, #0x1, lsl #12
 40250e8:	94001418 	bl	402a148 <spi_nor_write>
 40250ec:	17fffff9 	b	40250d0 <write_misc_data_to_flash+0x108>
 40250f0:	529690e0 	mov	w0, #0xb487                	// #46215
 40250f4:	72ad2b40 	movk	w0, #0x695a, lsl #16
 40250f8:	6b0002ff 	cmp	w23, w0
 40250fc:	54fff941 	b.ne	4025024 <write_misc_data_to_flash+0x5c>  // b.any
 4025100:	0b143274 	add	w20, w19, w20, lsl #12
 4025104:	6b14027f 	cmp	w19, w20
 4025108:	54fffc20 	b.eq	402508c <write_misc_data_to_flash+0xc4>  // b.none
 402510c:	2a1303e1 	mov	w1, w19
 4025110:	aa1603e0 	mov	x0, x22
 4025114:	52800402 	mov	w2, #0x20                  	// #32
 4025118:	11400673 	add	w19, w19, #0x1, lsl #12
 402511c:	94001518 	bl	402a57c <emmc_write>
 4025120:	17fffff9 	b	4025104 <write_misc_data_to_flash+0x13c>
 4025124:	2a1503e0 	mov	w0, w21
 4025128:	a94153f3 	ldp	x19, x20, [sp, #16]
 402512c:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025130:	a9437bf7 	ldp	x23, x30, [sp, #48]
 4025134:	910103ff 	add	sp, sp, #0x40
 4025138:	d65f03c0 	ret

000000000402513c <is_ab_boot_enable>:
 402513c:	d10083ff 	sub	sp, sp, #0x20
 4025140:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025144:	f9000bfe 	str	x30, [sp, #16]
 4025148:	f9477001 	ldr	x1, [x0, #3808]
 402514c:	f90007e1 	str	x1, [sp, #8]
 4025150:	d2800001 	mov	x1, #0x0                   	// #0
 4025154:	f90003ff 	str	xzr, [sp]
 4025158:	97fff94d 	bl	402368c <get_partition_area_addr>
 402515c:	2a0003e0 	mov	w0, w0
 4025160:	910003e1 	mov	x1, sp
 4025164:	90000062 	adrp	x2, 4031000 <sync_exception_sp_el0>
 4025168:	912f6442 	add	x2, x2, #0xbd9
 402516c:	9400084b 	bl	4027298 <ot_partition_get_partition_info_by_name>
 4025170:	f94003e0 	ldr	x0, [sp]
 4025174:	90ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4025178:	f100001f 	cmp	x0, #0x0
 402517c:	1a9f07e0 	cset	w0, ne  // ne = any
 4025180:	f94007e2 	ldr	x2, [sp, #8]
 4025184:	f9477023 	ldr	x3, [x1, #3808]
 4025188:	eb030042 	subs	x2, x2, x3
 402518c:	d2800003 	mov	x3, #0x0                   	// #0
 4025190:	54000040 	b.eq	4025198 <is_ab_boot_enable+0x5c>  // b.none
 4025194:	9400041a 	bl	40261fc <__stack_chk_fail>
 4025198:	f9400bfe 	ldr	x30, [sp, #16]
 402519c:	910083ff 	add	sp, sp, #0x20
 40251a0:	d65f03c0 	ret

00000000040251a4 <get_uboot_part_flash_offset>:
 40251a4:	d10283ff 	sub	sp, sp, #0xa0
 40251a8:	a90553f3 	stp	x19, x20, [sp, #80]
 40251ac:	90fffff4 	adrp	x20, 4021000 <gs_misc_record-0xa80>
 40251b0:	912b0294 	add	x20, x20, #0xac0
 40251b4:	a9065bf5 	stp	x21, x22, [sp, #96]
 40251b8:	aa0003f6 	mov	x22, x0
 40251bc:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40251c0:	a90763f7 	stp	x23, x24, [sp, #112]
 40251c4:	aa0103f5 	mov	x21, x1
 40251c8:	a9086bf9 	stp	x25, x26, [sp, #128]
 40251cc:	f9004bfe 	str	x30, [sp, #144]
 40251d0:	f9477001 	ldr	x1, [x0, #3808]
 40251d4:	f90027e1 	str	x1, [sp, #72]
 40251d8:	d2800001 	mov	x1, #0x0                   	// #0
 40251dc:	b9400680 	ldr	w0, [x20, #4]
 40251e0:	f90003ff 	str	xzr, [sp]
 40251e4:	35000f40 	cbnz	w0, 40253cc <get_uboot_part_flash_offset+0x228>
 40251e8:	97fff929 	bl	402368c <get_partition_area_addr>
 40251ec:	2a0003f7 	mov	w23, w0
 40251f0:	940011e0 	bl	4029970 <is_nand>
 40251f4:	35000380 	cbnz	w0, 4025264 <get_uboot_part_flash_offset+0xc0>
 40251f8:	97ffffd1 	bl	402513c <is_ab_boot_enable>
 40251fc:	72001c1f 	tst	w0, #0xff
 4025200:	54000b21 	b.ne	4025364 <get_uboot_part_flash_offset+0x1c0>  // b.any
 4025204:	90000073 	adrp	x19, 4031000 <sync_exception_sp_el0>
 4025208:	91301273 	add	x19, x19, #0xc04
 402520c:	aa1303e2 	mov	x2, x19
 4025210:	910003e1 	mov	x1, sp
 4025214:	aa1703e0 	mov	x0, x23
 4025218:	94000820 	bl	4027298 <ot_partition_get_partition_info_by_name>
 402521c:	7102941f 	cmp	w0, #0xa5
 4025220:	54000b40 	b.eq	4025388 <get_uboot_part_flash_offset+0x1e4>  // b.none
 4025224:	90000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4025228:	91313c00 	add	x0, x0, #0xc4f
 402522c:	94000e8b 	bl	4028c58 <log_serial_puts>
 4025230:	aa1303e0 	mov	x0, x19
 4025234:	94000e89 	bl	4028c58 <log_serial_puts>
 4025238:	90000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402523c:	912fc800 	add	x0, x0, #0xbf2
 4025240:	94000e86 	bl	4028c58 <log_serial_puts>
 4025244:	52800b40 	mov	w0, #0x5a                  	// #90
 4025248:	90ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402524c:	f94027e2 	ldr	x2, [sp, #72]
 4025250:	f9477023 	ldr	x3, [x1, #3808]
 4025254:	eb030042 	subs	x2, x2, x3
 4025258:	d2800003 	mov	x3, #0x0                   	// #0
 402525c:	54000c60 	b.eq	40253e8 <get_uboot_part_flash_offset+0x244>  // b.none
 4025260:	940003e7 	bl	40261fc <__stack_chk_fail>
 4025264:	5288ea59 	mov	w25, #0x4752                	// #18258
 4025268:	5290641a 	mov	w26, #0x8320                	// #33568
 402526c:	910023f8 	add	x24, sp, #0x8
 4025270:	52a00313 	mov	w19, #0x180000              	// #1572864
 4025274:	72aaaa19 	movk	w25, #0x5550, lsl #16
 4025278:	72bdb71a 	movk	w26, #0xedb8, lsl #16
 402527c:	2a1303e2 	mov	w2, w19
 4025280:	aa1803e0 	mov	x0, x24
 4025284:	52800801 	mov	w1, #0x40                  	// #64
 4025288:	9400116f 	bl	4029844 <spi_nand_read>
 402528c:	b9400be0 	ldr	w0, [sp, #8]
 4025290:	6b19001f 	cmp	w0, w25
 4025294:	54000601 	b.ne	4025354 <get_uboot_part_flash_offset+0x1b0>  // b.any
 4025298:	b9400fe0 	ldr	w0, [sp, #12]
 402529c:	7100041f 	cmp	w0, #0x1
 40252a0:	540005a1 	b.ne	4025354 <get_uboot_part_flash_offset+0x1b0>  // b.any
 40252a4:	b9400a80 	ldr	w0, [x20, #8]
 40252a8:	34000340 	cbz	w0, 4025310 <get_uboot_part_flash_offset+0x16c>
 40252ac:	91003282 	add	x2, x20, #0xc
 40252b0:	d2800001 	mov	x1, #0x0                   	// #0
 40252b4:	12800003 	mov	w3, #0xffffffff            	// #-1
 40252b8:	38616b00 	ldrb	w0, [x24, x1]
 40252bc:	91000421 	add	x1, x1, #0x1
 40252c0:	4a030000 	eor	w0, w0, w3
 40252c4:	92401c00 	and	x0, x0, #0xff
 40252c8:	b8607840 	ldr	w0, [x2, x0, lsl #2]
 40252cc:	4a432003 	eor	w3, w0, w3, lsr #8
 40252d0:	f100f03f 	cmp	x1, #0x3c
 40252d4:	54ffff21 	b.ne	40252b8 <get_uboot_part_flash_offset+0x114>  // b.any
 40252d8:	b94047e0 	ldr	w0, [sp, #68]
 40252dc:	2a2303e3 	mvn	w3, w3
 40252e0:	6b03001f 	cmp	w0, w3
 40252e4:	54000381 	b.ne	4025354 <get_uboot_part_flash_offset+0x1b0>  // b.any
 40252e8:	b94013e0 	ldr	w0, [sp, #16]
 40252ec:	51000400 	sub	w0, w0, #0x1
 40252f0:	7100041f 	cmp	w0, #0x1
 40252f4:	54000308 	b.hi	4025354 <get_uboot_part_flash_offset+0x1b0>  // b.pmore
 40252f8:	90000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40252fc:	90000073 	adrp	x19, 4031000 <sync_exception_sp_el0>
 4025300:	91306800 	add	x0, x0, #0xc1a
 4025304:	912ffe73 	add	x19, x19, #0xbff
 4025308:	94000e54 	bl	4028c58 <log_serial_puts>
 402530c:	17ffffc0 	b	402520c <get_uboot_part_flash_offset+0x68>
 4025310:	91003283 	add	x3, x20, #0xc
 4025314:	d2800001 	mov	x1, #0x0                   	// #0
 4025318:	2a0103e0 	mov	w0, w1
 402531c:	52800102 	mov	w2, #0x8                   	// #8
 4025320:	12000004 	and	w4, w0, #0x1
 4025324:	53017c00 	lsr	w0, w0, #1
 4025328:	34000044 	cbz	w4, 4025330 <get_uboot_part_flash_offset+0x18c>
 402532c:	4a1a0000 	eor	w0, w0, w26
 4025330:	71000442 	subs	w2, w2, #0x1
 4025334:	54ffff61 	b.ne	4025320 <get_uboot_part_flash_offset+0x17c>  // b.any
 4025338:	b8217860 	str	w0, [x3, x1, lsl #2]
 402533c:	91000421 	add	x1, x1, #0x1
 4025340:	f104003f 	cmp	x1, #0x100
 4025344:	54fffea1 	b.ne	4025318 <get_uboot_part_flash_offset+0x174>  // b.any
 4025348:	52800020 	mov	w0, #0x1                   	// #1
 402534c:	b9000a80 	str	w0, [x20, #8]
 4025350:	17ffffd7 	b	40252ac <get_uboot_part_flash_offset+0x108>
 4025354:	11408273 	add	w19, w19, #0x20, lsl #12
 4025358:	7148027f 	cmp	w19, #0x200, lsl #12
 402535c:	54fff901 	b.ne	402527c <get_uboot_part_flash_offset+0xd8>  // b.any
 4025360:	17ffffa6 	b	40251f8 <get_uboot_part_flash_offset+0x54>
 4025364:	97fff820 	bl	40233e4 <boot_control_get_active_slot>
 4025368:	12001c00 	and	w0, w0, #0xff
 402536c:	7100041f 	cmp	w0, #0x1
 4025370:	90000062 	adrp	x2, 4031000 <sync_exception_sp_el0>
 4025374:	90000073 	adrp	x19, 4031000 <sync_exception_sp_el0>
 4025378:	91304842 	add	x2, x2, #0xc12
 402537c:	91302a73 	add	x19, x19, #0xc0a
 4025380:	9a821273 	csel	x19, x19, x2, ne  // ne = any
 4025384:	17ffffa2 	b	402520c <get_uboot_part_flash_offset+0x68>
 4025388:	f94003e2 	ldr	x2, [sp]
 402538c:	39410040 	ldrb	w0, [x2, #64]
 4025390:	39410441 	ldrb	w1, [x2, #65]
 4025394:	aa012001 	orr	x1, x0, x1, lsl #8
 4025398:	39410840 	ldrb	w0, [x2, #66]
 402539c:	aa004021 	orr	x1, x1, x0, lsl #16
 40253a0:	39410c40 	ldrb	w0, [x2, #67]
 40253a4:	aa006020 	orr	x0, x1, x0, lsl #24
 40253a8:	b9000680 	str	w0, [x20, #4]
 40253ac:	39412040 	ldrb	w0, [x2, #72]
 40253b0:	39412441 	ldrb	w1, [x2, #73]
 40253b4:	aa012001 	orr	x1, x0, x1, lsl #8
 40253b8:	39412840 	ldrb	w0, [x2, #74]
 40253bc:	aa004021 	orr	x1, x1, x0, lsl #16
 40253c0:	39412c40 	ldrb	w0, [x2, #75]
 40253c4:	aa006020 	orr	x0, x1, x0, lsl #24
 40253c8:	b9040e80 	str	w0, [x20, #1036]
 40253cc:	b9400680 	ldr	w0, [x20, #4]
 40253d0:	b90002c0 	str	w0, [x22]
 40253d4:	b4000075 	cbz	x21, 40253e0 <get_uboot_part_flash_offset+0x23c>
 40253d8:	b9440e80 	ldr	w0, [x20, #1036]
 40253dc:	b90002a0 	str	w0, [x21]
 40253e0:	528014a0 	mov	w0, #0xa5                  	// #165
 40253e4:	17ffff99 	b	4025248 <get_uboot_part_flash_offset+0xa4>
 40253e8:	a94553f3 	ldp	x19, x20, [sp, #80]
 40253ec:	a9465bf5 	ldp	x21, x22, [sp, #96]
 40253f0:	a94763f7 	ldp	x23, x24, [sp, #112]
 40253f4:	a9486bf9 	ldp	x25, x26, [sp, #128]
 40253f8:	f9404bfe 	ldr	x30, [sp, #144]
 40253fc:	910283ff 	add	sp, sp, #0xa0
 4025400:	d65f03c0 	ret

0000000004025404 <flash_read>:
 4025404:	d10203ff 	sub	sp, sp, #0x80
 4025408:	a90253f3 	stp	x19, x20, [sp, #32]
 402540c:	a9035bf5 	stp	x21, x22, [sp, #48]
 4025410:	2a0203f6 	mov	w22, w2
 4025414:	a90463f7 	stp	x23, x24, [sp, #64]
 4025418:	a9056bf9 	stp	x25, x26, [sp, #80]
 402541c:	2a0103fa 	mov	w26, w1
 4025420:	a90673fb 	stp	x27, x28, [sp, #96]
 4025424:	2a0003fc 	mov	w28, w0
 4025428:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402542c:	f9003bfe 	str	x30, [sp, #112]
 4025430:	f9477001 	ldr	x1, [x0, #3808]
 4025434:	f9000fe1 	str	x1, [sp, #24]
 4025438:	d2800001 	mov	x1, #0x0                   	// #0
 402543c:	9400114d 	bl	4029970 <is_nand>
 4025440:	34001040 	cbz	w0, 4025648 <flash_read+0x244>
 4025444:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025448:	910053e1 	add	x1, sp, #0x14
 402544c:	912b0014 	add	x20, x0, #0xac0
 4025450:	29027fff 	stp	wzr, wzr, [sp, #16]
 4025454:	b94ac013 	ldr	w19, [x0, #2752]
 4025458:	910043e0 	add	x0, sp, #0x10
 402545c:	97ffff52 	bl	40251a4 <get_uboot_part_flash_offset>
 4025460:	2a0003fb 	mov	w27, w0
 4025464:	7102941f 	cmp	w0, #0xa5
 4025468:	54000220 	b.eq	40254ac <flash_read+0xa8>  // b.none
 402546c:	52800b5b 	mov	w27, #0x5a                  	// #90
 4025470:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025474:	f9400fe1 	ldr	x1, [sp, #24]
 4025478:	f9477002 	ldr	x2, [x0, #3808]
 402547c:	eb020021 	subs	x1, x1, x2
 4025480:	d2800002 	mov	x2, #0x0                   	// #0
 4025484:	54000ee1 	b.ne	4025660 <flash_read+0x25c>  // b.any
 4025488:	2a1b03e0 	mov	w0, w27
 402548c:	a94253f3 	ldp	x19, x20, [sp, #32]
 4025490:	a9435bf5 	ldp	x21, x22, [sp, #48]
 4025494:	a94463f7 	ldp	x23, x24, [sp, #64]
 4025498:	a9456bf9 	ldp	x25, x26, [sp, #80]
 402549c:	a94673fb 	ldp	x27, x28, [sp, #96]
 40254a0:	f9403bfe 	ldr	x30, [sp, #112]
 40254a4:	910203ff 	add	sp, sp, #0x80
 40254a8:	d65f03c0 	ret
 40254ac:	b94013f7 	ldr	w23, [sp, #16]
 40254b0:	51000679 	sub	w25, w19, #0x1
 40254b4:	6a17033f 	tst	w25, w23
 40254b8:	54fffda1 	b.ne	402546c <flash_read+0x68>  // b.any
 40254bc:	b94017f8 	ldr	w24, [sp, #20]
 40254c0:	6a18033f 	tst	w25, w24
 40254c4:	54fffd41 	b.ne	402546c <flash_read+0x68>  // b.any
 40254c8:	1ad30af7 	udiv	w23, w23, w19
 40254cc:	b9441280 	ldr	w0, [x20, #1040]
 40254d0:	1ad30b18 	udiv	w24, w24, w19
 40254d4:	0b170318 	add	w24, w24, w23
 40254d8:	7100041f 	cmp	w0, #0x1
 40254dc:	540002a0 	b.eq	4025530 <flash_read+0x12c>  // b.none
 40254e0:	1b177e61 	mul	w1, w19, w23
 40254e4:	2a1703f5 	mov	w21, w23
 40254e8:	b9041697 	str	w23, [x20, #1044]
 40254ec:	b9041a9f 	str	wzr, [x20, #1048]
 40254f0:	6b1502ff 	cmp	w23, w21
 40254f4:	7a559300 	ccmp	w24, w21, #0x0, ls  // ls = plast
 40254f8:	54fffba9 	b.ls	402546c <flash_read+0x68>  // b.plast
 40254fc:	2a0103e0 	mov	w0, w1
 4025500:	b90007e1 	str	w1, [sp, #4]
 4025504:	94001120 	bl	4029984 <is_bad_block>
 4025508:	b94007e1 	ldr	w1, [sp, #4]
 402550c:	0b130021 	add	w1, w1, w19
 4025510:	340000c0 	cbz	w0, 4025528 <flash_read+0x124>
 4025514:	b9441a80 	ldr	w0, [x20, #1048]
 4025518:	110006b5 	add	w21, w21, #0x1
 402551c:	11000400 	add	w0, w0, #0x1
 4025520:	b9041a80 	str	w0, [x20, #1048]
 4025524:	17fffff3 	b	40254f0 <flash_read+0xec>
 4025528:	52800020 	mov	w0, #0x1                   	// #1
 402552c:	b9041280 	str	w0, [x20, #1040]
 4025530:	1ad30ac1 	udiv	w1, w22, w19
 4025534:	b9441680 	ldr	w0, [x20, #1044]
 4025538:	6b00003f 	cmp	w1, w0
 402553c:	54fff983 	b.cc	402546c <flash_read+0x68>  // b.lo, b.ul, b.last
 4025540:	b9441a95 	ldr	w21, [x20, #1048]
 4025544:	0b150015 	add	w21, w0, w21
 4025548:	54000289 	b.ls	4025598 <flash_read+0x194>  // b.plast
 402554c:	1b134ea2 	madd	w2, w21, w19, w19
 4025550:	4b000021 	sub	w1, w1, w0
 4025554:	110006b5 	add	w21, w21, #0x1
 4025558:	b9000be1 	str	w1, [sp, #8]
 402555c:	6b1502ff 	cmp	w23, w21
 4025560:	7a559300 	ccmp	w24, w21, #0x0, ls  // ls = plast
 4025564:	54fff849 	b.ls	402546c <flash_read+0x68>  // b.plast
 4025568:	2a0203e0 	mov	w0, w2
 402556c:	b90007e2 	str	w2, [sp, #4]
 4025570:	94001105 	bl	4029984 <is_bad_block>
 4025574:	294087e2 	ldp	w2, w1, [sp, #4]
 4025578:	340000c0 	cbz	w0, 4025590 <flash_read+0x18c>
 402557c:	b9441a80 	ldr	w0, [x20, #1048]
 4025580:	11000400 	add	w0, w0, #0x1
 4025584:	b9041a80 	str	w0, [x20, #1048]
 4025588:	0b130042 	add	w2, w2, w19
 402558c:	17fffff2 	b	4025554 <flash_read+0x150>
 4025590:	71000421 	subs	w1, w1, #0x1
 4025594:	54ffffa1 	b.ne	4025588 <flash_read+0x184>  // b.any
 4025598:	0a1902c2 	and	w2, w22, w25
 402559c:	1b157e63 	mul	w3, w19, w21
 40255a0:	4b020279 	sub	w25, w19, w2
 40255a4:	2a1c03e0 	mov	w0, w28
 40255a8:	6b1a033f 	cmp	w25, w26
 40255ac:	0b030042 	add	w2, w2, w3
 40255b0:	1a9a9339 	csel	w25, w25, w26, ls  // ls = plast
 40255b4:	b90007e3 	str	w3, [sp, #4]
 40255b8:	2a1903e1 	mov	w1, w25
 40255bc:	940010a2 	bl	4029844 <spi_nand_read>
 40255c0:	b94007e3 	ldr	w3, [sp, #4]
 40255c4:	4b190344 	sub	w4, w26, w25
 40255c8:	0b030262 	add	w2, w19, w3
 40255cc:	350000a4 	cbnz	w4, 40255e0 <flash_read+0x1dc>
 40255d0:	0b160356 	add	w22, w26, w22
 40255d4:	1ad30ad3 	udiv	w19, w22, w19
 40255d8:	b9041693 	str	w19, [x20, #1044]
 40255dc:	17ffffa5 	b	4025470 <flash_read+0x6c>
 40255e0:	110006b5 	add	w21, w21, #0x1
 40255e4:	b9000be4 	str	w4, [sp, #8]
 40255e8:	6b1502ff 	cmp	w23, w21
 40255ec:	7a559300 	ccmp	w24, w21, #0x0, ls  // ls = plast
 40255f0:	54fff3e9 	b.ls	402546c <flash_read+0x68>  // b.plast
 40255f4:	2a0203e0 	mov	w0, w2
 40255f8:	b90007e2 	str	w2, [sp, #4]
 40255fc:	940010e2 	bl	4029984 <is_bad_block>
 4025600:	294093e2 	ldp	w2, w4, [sp, #4]
 4025604:	340000c0 	cbz	w0, 402561c <flash_read+0x218>
 4025608:	b9441a80 	ldr	w0, [x20, #1048]
 402560c:	11000400 	add	w0, w0, #0x1
 4025610:	b9041a80 	str	w0, [x20, #1048]
 4025614:	0b130042 	add	w2, w2, w19
 4025618:	17ffffed 	b	40255cc <flash_read+0x1c8>
 402561c:	6b04027f 	cmp	w19, w4
 4025620:	0b190380 	add	w0, w28, w25
 4025624:	1a849261 	csel	w1, w19, w4, ls  // ls = plast
 4025628:	29008be1 	stp	w1, w2, [sp, #4]
 402562c:	b9000fe4 	str	w4, [sp, #12]
 4025630:	94001085 	bl	4029844 <spi_nand_read>
 4025634:	29408be1 	ldp	w1, w2, [sp, #4]
 4025638:	b9400fe4 	ldr	w4, [sp, #12]
 402563c:	0b010339 	add	w25, w25, w1
 4025640:	4b010084 	sub	w4, w4, w1
 4025644:	17fffff4 	b	4025614 <flash_read+0x210>
 4025648:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402564c:	f9400fe1 	ldr	x1, [sp, #24]
 4025650:	f9477002 	ldr	x2, [x0, #3808]
 4025654:	eb020021 	subs	x1, x1, x2
 4025658:	d2800002 	mov	x2, #0x0                   	// #0
 402565c:	54000040 	b.eq	4025664 <flash_read+0x260>  // b.none
 4025660:	940002e7 	bl	40261fc <__stack_chk_fail>
 4025664:	2a1603e2 	mov	w2, w22
 4025668:	2a1a03e1 	mov	w1, w26
 402566c:	2a1c03e0 	mov	w0, w28
 4025670:	a94253f3 	ldp	x19, x20, [sp, #32]
 4025674:	a9435bf5 	ldp	x21, x22, [sp, #48]
 4025678:	a94463f7 	ldp	x23, x24, [sp, #64]
 402567c:	a9456bf9 	ldp	x25, x26, [sp, #80]
 4025680:	a94673fb 	ldp	x27, x28, [sp, #96]
 4025684:	f9403bfe 	ldr	x30, [sp, #112]
 4025688:	910203ff 	add	sp, sp, #0x80
 402568c:	140012c4 	b	402a19c <spi_nor_read>

0000000004025690 <get_ree_key_flash_offset>:
 4025690:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 4025694:	aa0003f4 	mov	x20, x0
 4025698:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402569c:	a9015bf5 	stp	x21, x22, [sp, #16]
 40256a0:	f90013fe 	str	x30, [sp, #32]
 40256a4:	d10843ff 	sub	sp, sp, #0x210
 40256a8:	f9477001 	ldr	x1, [x0, #3808]
 40256ac:	f90107e1 	str	x1, [sp, #520]
 40256b0:	d2800001 	mov	x1, #0x0                   	// #0
 40256b4:	97fff6a5 	bl	4023148 <get_data_channel_type>
 40256b8:	2a0003f5 	mov	w21, w0
 40256bc:	528970e0 	mov	w0, #0x4b87                	// #19335
 40256c0:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 40256c4:	6b0002bf 	cmp	w21, w0
 40256c8:	540001a1 	b.ne	40256fc <get_ree_key_flash_offset+0x6c>  // b.any
 40256cc:	97fff7ec 	bl	402367c <get_otfl_code_area_len>
 40256d0:	52828004 	mov	w4, #0x1400                	// #5120
 40256d4:	0b040000 	add	w0, w0, w4
 40256d8:	528014b3 	mov	w19, #0xa5                  	// #165
 40256dc:	b9000280 	str	w0, [x20]
 40256e0:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40256e4:	f94107e1 	ldr	x1, [sp, #520]
 40256e8:	f9477002 	ldr	x2, [x0, #3808]
 40256ec:	eb020021 	subs	x1, x1, x2
 40256f0:	d2800002 	mov	x2, #0x0                   	// #0
 40256f4:	54000580 	b.eq	40257a4 <get_ree_key_flash_offset+0x114>  // b.none
 40256f8:	940002c1 	bl	40261fc <__stack_chk_fail>
 40256fc:	90fffff6 	adrp	x22, 4021000 <gs_misc_record-0xa80>
 4025700:	912b02d6 	add	x22, x22, #0xac0
 4025704:	b9441ec0 	ldr	w0, [x22, #1052]
 4025708:	35fffe80 	cbnz	w0, 40256d8 <get_ree_key_flash_offset+0x48>
 402570c:	910013e0 	add	x0, sp, #0x4
 4025710:	d2800001 	mov	x1, #0x0                   	// #0
 4025714:	97fffea4 	bl	40251a4 <get_uboot_part_flash_offset>
 4025718:	2a0003f3 	mov	w19, w0
 402571c:	7102941f 	cmp	w0, #0xa5
 4025720:	54fffe01 	b.ne	40256e0 <get_ree_key_flash_offset+0x50>  // b.any
 4025724:	52896f00 	mov	w0, #0x4b78                	// #19320
 4025728:	b94007e1 	ldr	w1, [sp, #4]
 402572c:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 4025730:	6b0002bf 	cmp	w21, w0
 4025734:	54000241 	b.ne	402577c <get_ree_key_flash_offset+0xec>  // b.any
 4025738:	52824003 	mov	w3, #0x1200                	// #4608
 402573c:	110023e0 	add	w0, wsp, #0x8
 4025740:	0b030022 	add	w2, w1, w3
 4025744:	52804001 	mov	w1, #0x200                 	// #512
 4025748:	97ffff2f 	bl	4025404 <flash_read>
 402574c:	7102941f 	cmp	w0, #0xa5
 4025750:	54000121 	b.ne	4025774 <get_ree_key_flash_offset+0xe4>  // b.any
 4025754:	b94007e0 	ldr	w0, [sp, #4]
 4025758:	52828002 	mov	w2, #0x1400                	// #5120
 402575c:	b9402fe1 	ldr	w1, [sp, #44]
 4025760:	0b000021 	add	w1, w1, w0
 4025764:	0b020021 	add	w1, w1, w2
 4025768:	b9000281 	str	w1, [x20]
 402576c:	b9041ec1 	str	w1, [x22, #1052]
 4025770:	17ffffdc 	b	40256e0 <get_ree_key_flash_offset+0x50>
 4025774:	52800b53 	mov	w19, #0x5a                  	// #90
 4025778:	17ffffda 	b	40256e0 <get_ree_key_flash_offset+0x50>
 402577c:	529690e0 	mov	w0, #0xb487                	// #46215
 4025780:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4025784:	6b0002bf 	cmp	w21, w0
 4025788:	54fffe61 	b.ne	4025754 <get_ree_key_flash_offset+0xc4>  // b.any
 402578c:	52824000 	mov	w0, #0x1200                	// #4608
 4025790:	52804002 	mov	w2, #0x200                 	// #512
 4025794:	0b000021 	add	w1, w1, w0
 4025798:	910023e0 	add	x0, sp, #0x8
 402579c:	940013a5 	bl	402a630 <emmc_read>
 40257a0:	17ffffeb 	b	402574c <get_ree_key_flash_offset+0xbc>
 40257a4:	910843ff 	add	sp, sp, #0x210
 40257a8:	2a1303e0 	mov	w0, w19
 40257ac:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40257b0:	f94013fe 	ldr	x30, [sp, #32]
 40257b4:	a8c353f3 	ldp	x19, x20, [sp], #48
 40257b8:	d65f03c0 	ret

00000000040257bc <get_ree_key_and_paras_info_from_device>:
 40257bc:	d10103ff 	sub	sp, sp, #0x40
 40257c0:	a90153f3 	stp	x19, x20, [sp, #16]
 40257c4:	a9025bf5 	stp	x21, x22, [sp, #32]
 40257c8:	aa0003f6 	mov	x22, x0
 40257cc:	2a0103f5 	mov	w21, w1
 40257d0:	f9001bfe 	str	x30, [sp, #48]
 40257d4:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40257d8:	f9477001 	ldr	x1, [x0, #3808]
 40257dc:	f90007e1 	str	x1, [sp, #8]
 40257e0:	d2800001 	mov	x1, #0x0                   	// #0
 40257e4:	97fff7c2 	bl	40236ec <get_ree_key_area_addr>
 40257e8:	2a0003f4 	mov	w20, w0
 40257ec:	910013e0 	add	x0, sp, #0x4
 40257f0:	97ffffa8 	bl	4025690 <get_ree_key_flash_offset>
 40257f4:	2a0003f3 	mov	w19, w0
 40257f8:	7102941f 	cmp	w0, #0xa5
 40257fc:	540001a1 	b.ne	4025830 <get_ree_key_and_paras_info_from_device+0x74>  // b.any
 4025800:	52896f00 	mov	w0, #0x4b78                	// #19320
 4025804:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 4025808:	6b0002bf 	cmp	w21, w0
 402580c:	54000201 	b.ne	402584c <get_ree_key_and_paras_info_from_device+0x90>  // b.any
 4025810:	b94007e0 	ldr	w0, [sp, #4]
 4025814:	52808001 	mov	w1, #0x400                 	// #1024
 4025818:	b9400ac2 	ldr	w2, [x22, #8]
 402581c:	0b000042 	add	w2, w2, w0
 4025820:	2a1403e0 	mov	w0, w20
 4025824:	b90007e2 	str	w2, [sp, #4]
 4025828:	97fffef7 	bl	4025404 <flash_read>
 402582c:	2a0003f3 	mov	w19, w0
 4025830:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025834:	f94007e1 	ldr	x1, [sp, #8]
 4025838:	f9477002 	ldr	x2, [x0, #3808]
 402583c:	eb020021 	subs	x1, x1, x2
 4025840:	d2800002 	mov	x2, #0x0                   	// #0
 4025844:	540003e0 	b.eq	40258c0 <get_ree_key_and_paras_info_from_device+0x104>  // b.none
 4025848:	9400026d 	bl	40261fc <__stack_chk_fail>
 402584c:	529690e0 	mov	w0, #0xb487                	// #46215
 4025850:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4025854:	6b0002bf 	cmp	w21, w0
 4025858:	54000121 	b.ne	402587c <get_ree_key_and_paras_info_from_device+0xc0>  // b.any
 402585c:	b94007e0 	ldr	w0, [sp, #4]
 4025860:	52808002 	mov	w2, #0x400                 	// #1024
 4025864:	b9400ac1 	ldr	w1, [x22, #8]
 4025868:	0b000021 	add	w1, w1, w0
 402586c:	2a1403e0 	mov	w0, w20
 4025870:	b90007e1 	str	w1, [sp, #4]
 4025874:	9400136f 	bl	402a630 <emmc_read>
 4025878:	17ffffed 	b	402582c <get_ree_key_and_paras_info_from_device+0x70>
 402587c:	528970e0 	mov	w0, #0x4b87                	// #19335
 4025880:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4025884:	6b0002bf 	cmp	w21, w0
 4025888:	540000a1 	b.ne	402589c <get_ree_key_and_paras_info_from_device+0xe0>  // b.any
 402588c:	94000d37 	bl	4028d68 <self_sdio_check>
 4025890:	350000a0 	cbnz	w0, 40258a4 <get_ree_key_and_paras_info_from_device+0xe8>
 4025894:	52800800 	mov	w0, #0x40                  	// #64
 4025898:	9400021c 	bl	4026108 <err_print>
 402589c:	52800b53 	mov	w19, #0x5a                  	// #90
 40258a0:	17ffffe4 	b	4025830 <get_ree_key_and_paras_info_from_device+0x74>
 40258a4:	94000cc9 	bl	4028bc8 <enable_sdio_dma>
 40258a8:	b94007e0 	ldr	w0, [sp, #4]
 40258ac:	94000cc3 	bl	4028bb8 <set_sdio_pos>
 40258b0:	2a1403e0 	mov	w0, w20
 40258b4:	d2808001 	mov	x1, #0x400                 	// #1024
 40258b8:	94000cbc 	bl	4028ba8 <copy_from_sdio>
 40258bc:	17ffffdd 	b	4025830 <get_ree_key_and_paras_info_from_device+0x74>
 40258c0:	2a1303e0 	mov	w0, w19
 40258c4:	a94153f3 	ldp	x19, x20, [sp, #16]
 40258c8:	a9425bf5 	ldp	x21, x22, [sp, #32]
 40258cc:	f9401bfe 	ldr	x30, [sp, #48]
 40258d0:	910103ff 	add	sp, sp, #0x40
 40258d4:	d65f03c0 	ret

00000000040258d8 <get_first_param_data_flash_offset>:
 40258d8:	d10083ff 	sub	sp, sp, #0x20
 40258dc:	a9017bf3 	stp	x19, x30, [sp, #16]
 40258e0:	aa0003f3 	mov	x19, x0
 40258e4:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40258e8:	f9477001 	ldr	x1, [x0, #3808]
 40258ec:	f90007e1 	str	x1, [sp, #8]
 40258f0:	d2800001 	mov	x1, #0x0                   	// #0
 40258f4:	910013e0 	add	x0, sp, #0x4
 40258f8:	97ffff66 	bl	4025690 <get_ree_key_flash_offset>
 40258fc:	7102941f 	cmp	w0, #0xa5
 4025900:	54000081 	b.ne	4025910 <get_first_param_data_flash_offset+0x38>  // b.any
 4025904:	b94007e1 	ldr	w1, [sp, #4]
 4025908:	11100021 	add	w1, w1, #0x400
 402590c:	b9000261 	str	w1, [x19]
 4025910:	90ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4025914:	f94007e2 	ldr	x2, [sp, #8]
 4025918:	f9477023 	ldr	x3, [x1, #3808]
 402591c:	eb030042 	subs	x2, x2, x3
 4025920:	d2800003 	mov	x3, #0x0                   	// #0
 4025924:	54000040 	b.eq	402592c <get_first_param_data_flash_offset+0x54>  // b.none
 4025928:	94000235 	bl	40261fc <__stack_chk_fail>
 402592c:	a9417bf3 	ldp	x19, x30, [sp, #16]
 4025930:	910083ff 	add	sp, sp, #0x20
 4025934:	d65f03c0 	ret

0000000004025938 <get_paras_data_from_flash>:
 4025938:	d10183ff 	sub	sp, sp, #0x60
 402593c:	a90153f3 	stp	x19, x20, [sp, #16]
 4025940:	a9025bf5 	stp	x21, x22, [sp, #32]
 4025944:	2a0203f6 	mov	w22, w2
 4025948:	a90363f7 	stp	x23, x24, [sp, #48]
 402594c:	2a0103f8 	mov	w24, w1
 4025950:	a9046bf9 	stp	x25, x26, [sp, #64]
 4025954:	aa0003fa 	mov	x26, x0
 4025958:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402595c:	a9057bfb 	stp	x27, x30, [sp, #80]
 4025960:	90fffff9 	adrp	x25, 4021000 <gs_misc_record-0xa80>
 4025964:	f9477001 	ldr	x1, [x0, #3808]
 4025968:	f90007e1 	str	x1, [sp, #8]
 402596c:	d2800001 	mov	x1, #0x0                   	// #0
 4025970:	97fff8af 	bl	4023c2c <get_ddr_param_len>
 4025974:	2a0003f4 	mov	w20, w0
 4025978:	97fff767 	bl	4023714 <get_ddr_param_data_addr>
 402597c:	2a0003f3 	mov	w19, w0
 4025980:	97fff8ab 	bl	4023c2c <get_ddr_param_len>
 4025984:	2a0003fb 	mov	w27, w0
 4025988:	97fff75e 	bl	4023700 <get_ddr_param_info_addr>
 402598c:	2a0003f7 	mov	w23, w0
 4025990:	910013e0 	add	x0, sp, #0x4
 4025994:	97ffffd1 	bl	40258d8 <get_first_param_data_flash_offset>
 4025998:	7102941f 	cmp	w0, #0xa5
 402599c:	54000861 	b.ne	4025aa8 <get_paras_data_from_flash+0x170>  // b.any
 40259a0:	8b374317 	add	x23, x24, w23, uxtw
 40259a4:	b94007e0 	ldr	w0, [sp, #4]
 40259a8:	3944b2f5 	ldrb	w21, [x23, #300]
 40259ac:	1b1b02b5 	madd	w21, w21, w27, w0
 40259b0:	52896f00 	mov	w0, #0x4b78                	// #19320
 40259b4:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 40259b8:	6b0002df 	cmp	w22, w0
 40259bc:	54000241 	b.ne	4025a04 <get_paras_data_from_flash+0xcc>  // b.any
 40259c0:	f94007e0 	ldr	x0, [sp, #8]
 40259c4:	f9477321 	ldr	x1, [x25, #3808]
 40259c8:	eb010000 	subs	x0, x0, x1
 40259cc:	d2800001 	mov	x1, #0x0                   	// #0
 40259d0:	54000040 	b.eq	40259d8 <get_paras_data_from_flash+0xa0>  // b.none
 40259d4:	9400020a 	bl	40261fc <__stack_chk_fail>
 40259d8:	b9400b42 	ldr	w2, [x26, #8]
 40259dc:	2a1403e1 	mov	w1, w20
 40259e0:	2a1303e0 	mov	w0, w19
 40259e4:	0b0202a2 	add	w2, w21, w2
 40259e8:	a94153f3 	ldp	x19, x20, [sp, #16]
 40259ec:	a9425bf5 	ldp	x21, x22, [sp, #32]
 40259f0:	a94363f7 	ldp	x23, x24, [sp, #48]
 40259f4:	a9446bf9 	ldp	x25, x26, [sp, #64]
 40259f8:	a9457bfb 	ldp	x27, x30, [sp, #80]
 40259fc:	910183ff 	add	sp, sp, #0x60
 4025a00:	17fffe81 	b	4025404 <flash_read>
 4025a04:	529690e0 	mov	w0, #0xb487                	// #46215
 4025a08:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4025a0c:	6b0002df 	cmp	w22, w0
 4025a10:	54000221 	b.ne	4025a54 <get_paras_data_from_flash+0x11c>  // b.any
 4025a14:	f94007e0 	ldr	x0, [sp, #8]
 4025a18:	f9477321 	ldr	x1, [x25, #3808]
 4025a1c:	eb010000 	subs	x0, x0, x1
 4025a20:	d2800001 	mov	x1, #0x0                   	// #0
 4025a24:	54fffd81 	b.ne	40259d4 <get_paras_data_from_flash+0x9c>  // b.any
 4025a28:	b9400b41 	ldr	w1, [x26, #8]
 4025a2c:	2a1403e2 	mov	w2, w20
 4025a30:	2a1303e0 	mov	w0, w19
 4025a34:	0b0102a1 	add	w1, w21, w1
 4025a38:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025a3c:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025a40:	a94363f7 	ldp	x23, x24, [sp, #48]
 4025a44:	a9446bf9 	ldp	x25, x26, [sp, #64]
 4025a48:	a9457bfb 	ldp	x27, x30, [sp, #80]
 4025a4c:	910183ff 	add	sp, sp, #0x60
 4025a50:	140012f8 	b	402a630 <emmc_read>
 4025a54:	528970e0 	mov	w0, #0x4b87                	// #19335
 4025a58:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4025a5c:	6b0002df 	cmp	w22, w0
 4025a60:	54000241 	b.ne	4025aa8 <get_paras_data_from_flash+0x170>  // b.any
 4025a64:	94000c59 	bl	4028bc8 <enable_sdio_dma>
 4025a68:	2a1503e0 	mov	w0, w21
 4025a6c:	94000c53 	bl	4028bb8 <set_sdio_pos>
 4025a70:	f94007e0 	ldr	x0, [sp, #8]
 4025a74:	f9477321 	ldr	x1, [x25, #3808]
 4025a78:	eb010000 	subs	x0, x0, x1
 4025a7c:	d2800001 	mov	x1, #0x0                   	// #0
 4025a80:	54fffaa1 	b.ne	40259d4 <get_paras_data_from_flash+0x9c>  // b.any
 4025a84:	2a1403e1 	mov	w1, w20
 4025a88:	2a1303e0 	mov	w0, w19
 4025a8c:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025a90:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025a94:	a94363f7 	ldp	x23, x24, [sp, #48]
 4025a98:	a9446bf9 	ldp	x25, x26, [sp, #64]
 4025a9c:	a9457bfb 	ldp	x27, x30, [sp, #80]
 4025aa0:	910183ff 	add	sp, sp, #0x60
 4025aa4:	14000c41 	b	4028ba8 <copy_from_sdio>
 4025aa8:	f94007e0 	ldr	x0, [sp, #8]
 4025aac:	f9477321 	ldr	x1, [x25, #3808]
 4025ab0:	eb010000 	subs	x0, x0, x1
 4025ab4:	d2800001 	mov	x1, #0x0                   	// #0
 4025ab8:	54fff8e1 	b.ne	40259d4 <get_paras_data_from_flash+0x9c>  // b.any
 4025abc:	52800b40 	mov	w0, #0x5a                  	// #90
 4025ac0:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025ac4:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025ac8:	a94363f7 	ldp	x23, x24, [sp, #48]
 4025acc:	a9446bf9 	ldp	x25, x26, [sp, #64]
 4025ad0:	a9457bfb 	ldp	x27, x30, [sp, #80]
 4025ad4:	910183ff 	add	sp, sp, #0x60
 4025ad8:	d65f03c0 	ret

0000000004025adc <get_uboot_info_flash_offset>:
 4025adc:	d100c3ff 	sub	sp, sp, #0x30
 4025ae0:	a90153f3 	stp	x19, x20, [sp, #16]
 4025ae4:	aa0003f4 	mov	x20, x0
 4025ae8:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025aec:	a9027bf5 	stp	x21, x30, [sp, #32]
 4025af0:	f9477001 	ldr	x1, [x0, #3808]
 4025af4:	f90007e1 	str	x1, [sp, #8]
 4025af8:	d2800001 	mov	x1, #0x0                   	// #0
 4025afc:	97fff84c 	bl	4023c2c <get_ddr_param_len>
 4025b00:	2a0003f3 	mov	w19, w0
 4025b04:	97fff85b 	bl	4023c70 <get_ddr_param_cnt>
 4025b08:	2a0003f5 	mov	w21, w0
 4025b0c:	910013e0 	add	x0, sp, #0x4
 4025b10:	97ffff72 	bl	40258d8 <get_first_param_data_flash_offset>
 4025b14:	7102941f 	cmp	w0, #0xa5
 4025b18:	54000081 	b.ne	4025b28 <get_uboot_info_flash_offset+0x4c>  // b.any
 4025b1c:	b94007e1 	ldr	w1, [sp, #4]
 4025b20:	1b150673 	madd	w19, w19, w21, w1
 4025b24:	b9000293 	str	w19, [x20]
 4025b28:	90ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4025b2c:	f94007e2 	ldr	x2, [sp, #8]
 4025b30:	f9477023 	ldr	x3, [x1, #3808]
 4025b34:	eb030042 	subs	x2, x2, x3
 4025b38:	d2800003 	mov	x3, #0x0                   	// #0
 4025b3c:	54000040 	b.eq	4025b44 <get_uboot_info_flash_offset+0x68>  // b.none
 4025b40:	940001af 	bl	40261fc <__stack_chk_fail>
 4025b44:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025b48:	a9427bf5 	ldp	x21, x30, [sp, #32]
 4025b4c:	9100c3ff 	add	sp, sp, #0x30
 4025b50:	d65f03c0 	ret

0000000004025b54 <get_uboot_code_from_flash>:
 4025b54:	d10143ff 	sub	sp, sp, #0x50
 4025b58:	a90153f3 	stp	x19, x20, [sp, #16]
 4025b5c:	2a0103f4 	mov	w20, w1
 4025b60:	a9025bf5 	stp	x21, x22, [sp, #32]
 4025b64:	aa0003f6 	mov	x22, x0
 4025b68:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025b6c:	a90363f7 	stp	x23, x24, [sp, #48]
 4025b70:	90fffff5 	adrp	x21, 4021000 <gs_misc_record-0xa80>
 4025b74:	f90023fe 	str	x30, [sp, #64]
 4025b78:	f9477001 	ldr	x1, [x0, #3808]
 4025b7c:	f90007e1 	str	x1, [sp, #8]
 4025b80:	d2800001 	mov	x1, #0x0                   	// #0
 4025b84:	97fff6e9 	bl	4023728 <get_uboot_info_download_ddr_addr>
 4025b88:	b9408818 	ldr	w24, [x0, #136]
 4025b8c:	aa0003f7 	mov	x23, x0
 4025b90:	910013e0 	add	x0, sp, #0x4
 4025b94:	97ffffd2 	bl	4025adc <get_uboot_info_flash_offset>
 4025b98:	7102941f 	cmp	w0, #0xa5
 4025b9c:	54000801 	b.ne	4025c9c <get_uboot_code_from_flash+0x148>  // b.any
 4025ba0:	b94007f3 	ldr	w19, [sp, #4]
 4025ba4:	52896f00 	mov	w0, #0x4b78                	// #19320
 4025ba8:	b94026f7 	ldr	w23, [x23, #36]
 4025bac:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 4025bb0:	11080273 	add	w19, w19, #0x200
 4025bb4:	6b00029f 	cmp	w20, w0
 4025bb8:	54000221 	b.ne	4025bfc <get_uboot_code_from_flash+0xa8>  // b.any
 4025bbc:	f94007e0 	ldr	x0, [sp, #8]
 4025bc0:	f94772a1 	ldr	x1, [x21, #3808]
 4025bc4:	eb010000 	subs	x0, x0, x1
 4025bc8:	d2800001 	mov	x1, #0x0                   	// #0
 4025bcc:	54000040 	b.eq	4025bd4 <get_uboot_code_from_flash+0x80>  // b.none
 4025bd0:	9400018b 	bl	40261fc <__stack_chk_fail>
 4025bd4:	b9400ac2 	ldr	w2, [x22, #8]
 4025bd8:	2a1703e1 	mov	w1, w23
 4025bdc:	2a1803e0 	mov	w0, w24
 4025be0:	0b020262 	add	w2, w19, w2
 4025be4:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025be8:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025bec:	a94363f7 	ldp	x23, x24, [sp, #48]
 4025bf0:	f94023fe 	ldr	x30, [sp, #64]
 4025bf4:	910143ff 	add	sp, sp, #0x50
 4025bf8:	17fffe03 	b	4025404 <flash_read>
 4025bfc:	529690e0 	mov	w0, #0xb487                	// #46215
 4025c00:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4025c04:	6b00029f 	cmp	w20, w0
 4025c08:	54000201 	b.ne	4025c48 <get_uboot_code_from_flash+0xf4>  // b.any
 4025c0c:	f94007e0 	ldr	x0, [sp, #8]
 4025c10:	f94772a1 	ldr	x1, [x21, #3808]
 4025c14:	eb010000 	subs	x0, x0, x1
 4025c18:	d2800001 	mov	x1, #0x0                   	// #0
 4025c1c:	54fffda1 	b.ne	4025bd0 <get_uboot_code_from_flash+0x7c>  // b.any
 4025c20:	b9400ac1 	ldr	w1, [x22, #8]
 4025c24:	2a1703e2 	mov	w2, w23
 4025c28:	2a1803e0 	mov	w0, w24
 4025c2c:	0b010261 	add	w1, w19, w1
 4025c30:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025c34:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025c38:	a94363f7 	ldp	x23, x24, [sp, #48]
 4025c3c:	f94023fe 	ldr	x30, [sp, #64]
 4025c40:	910143ff 	add	sp, sp, #0x50
 4025c44:	1400127b 	b	402a630 <emmc_read>
 4025c48:	528970e0 	mov	w0, #0x4b87                	// #19335
 4025c4c:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4025c50:	6b00029f 	cmp	w20, w0
 4025c54:	54000221 	b.ne	4025c98 <get_uboot_code_from_flash+0x144>  // b.any
 4025c58:	94000bdc 	bl	4028bc8 <enable_sdio_dma>
 4025c5c:	2a1303e0 	mov	w0, w19
 4025c60:	94000bd6 	bl	4028bb8 <set_sdio_pos>
 4025c64:	f94007e0 	ldr	x0, [sp, #8]
 4025c68:	f94772a1 	ldr	x1, [x21, #3808]
 4025c6c:	eb010000 	subs	x0, x0, x1
 4025c70:	d2800001 	mov	x1, #0x0                   	// #0
 4025c74:	54fffae1 	b.ne	4025bd0 <get_uboot_code_from_flash+0x7c>  // b.any
 4025c78:	2a1703e1 	mov	w1, w23
 4025c7c:	2a1803e0 	mov	w0, w24
 4025c80:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025c84:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025c88:	a94363f7 	ldp	x23, x24, [sp, #48]
 4025c8c:	f94023fe 	ldr	x30, [sp, #64]
 4025c90:	910143ff 	add	sp, sp, #0x50
 4025c94:	14000bc5 	b	4028ba8 <copy_from_sdio>
 4025c98:	52800b40 	mov	w0, #0x5a                  	// #90
 4025c9c:	f94007e1 	ldr	x1, [sp, #8]
 4025ca0:	f94772a2 	ldr	x2, [x21, #3808]
 4025ca4:	eb020021 	subs	x1, x1, x2
 4025ca8:	d2800002 	mov	x2, #0x0                   	// #0
 4025cac:	54fff921 	b.ne	4025bd0 <get_uboot_code_from_flash+0x7c>  // b.any
 4025cb0:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025cb4:	a9425bf5 	ldp	x21, x22, [sp, #32]
 4025cb8:	a94363f7 	ldp	x23, x24, [sp, #48]
 4025cbc:	f94023fe 	ldr	x30, [sp, #64]
 4025cc0:	910143ff 	add	sp, sp, #0x50
 4025cc4:	d65f03c0 	ret

0000000004025cc8 <get_uboot_info_from_flash>:
 4025cc8:	d100c3ff 	sub	sp, sp, #0x30
 4025ccc:	a90153f3 	stp	x19, x20, [sp, #16]
 4025cd0:	2a0103f4 	mov	w20, w1
 4025cd4:	a9027bf5 	stp	x21, x30, [sp, #32]
 4025cd8:	aa0003f5 	mov	x21, x0
 4025cdc:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025ce0:	f9477001 	ldr	x1, [x0, #3808]
 4025ce4:	f90007e1 	str	x1, [sp, #8]
 4025ce8:	d2800001 	mov	x1, #0x0                   	// #0
 4025cec:	97fff68f 	bl	4023728 <get_uboot_info_download_ddr_addr>
 4025cf0:	aa0003f3 	mov	x19, x0
 4025cf4:	910013e0 	add	x0, sp, #0x4
 4025cf8:	97ffff79 	bl	4025adc <get_uboot_info_flash_offset>
 4025cfc:	7102941f 	cmp	w0, #0xa5
 4025d00:	54000181 	b.ne	4025d30 <get_uboot_info_from_flash+0x68>  // b.any
 4025d04:	52896f00 	mov	w0, #0x4b78                	// #19320
 4025d08:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 4025d0c:	6b00029f 	cmp	w20, w0
 4025d10:	540001e1 	b.ne	4025d4c <get_uboot_info_from_flash+0x84>  // b.any
 4025d14:	b94007e0 	ldr	w0, [sp, #4]
 4025d18:	52804001 	mov	w1, #0x200                 	// #512
 4025d1c:	b9400aa2 	ldr	w2, [x21, #8]
 4025d20:	0b000042 	add	w2, w2, w0
 4025d24:	2a1303e0 	mov	w0, w19
 4025d28:	b90007e2 	str	w2, [sp, #4]
 4025d2c:	97fffdb6 	bl	4025404 <flash_read>
 4025d30:	90ffffe1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4025d34:	f94007e2 	ldr	x2, [sp, #8]
 4025d38:	f9477023 	ldr	x3, [x1, #3808]
 4025d3c:	eb030042 	subs	x2, x2, x3
 4025d40:	d2800003 	mov	x3, #0x0                   	// #0
 4025d44:	54000360 	b.eq	4025db0 <get_uboot_info_from_flash+0xe8>  // b.none
 4025d48:	9400012d 	bl	40261fc <__stack_chk_fail>
 4025d4c:	529690e0 	mov	w0, #0xb487                	// #46215
 4025d50:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4025d54:	6b00029f 	cmp	w20, w0
 4025d58:	54000121 	b.ne	4025d7c <get_uboot_info_from_flash+0xb4>  // b.any
 4025d5c:	b94007e0 	ldr	w0, [sp, #4]
 4025d60:	52804002 	mov	w2, #0x200                 	// #512
 4025d64:	b9400aa1 	ldr	w1, [x21, #8]
 4025d68:	0b000021 	add	w1, w1, w0
 4025d6c:	2a1303e0 	mov	w0, w19
 4025d70:	b90007e1 	str	w1, [sp, #4]
 4025d74:	9400122f 	bl	402a630 <emmc_read>
 4025d78:	17ffffee 	b	4025d30 <get_uboot_info_from_flash+0x68>
 4025d7c:	528970e0 	mov	w0, #0x4b87                	// #19335
 4025d80:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4025d84:	6b00029f 	cmp	w20, w0
 4025d88:	54000101 	b.ne	4025da8 <get_uboot_info_from_flash+0xe0>  // b.any
 4025d8c:	94000b8f 	bl	4028bc8 <enable_sdio_dma>
 4025d90:	b94007e0 	ldr	w0, [sp, #4]
 4025d94:	94000b89 	bl	4028bb8 <set_sdio_pos>
 4025d98:	2a1303e0 	mov	w0, w19
 4025d9c:	d2804001 	mov	x1, #0x200                 	// #512
 4025da0:	94000b82 	bl	4028ba8 <copy_from_sdio>
 4025da4:	17ffffe3 	b	4025d30 <get_uboot_info_from_flash+0x68>
 4025da8:	52800b40 	mov	w0, #0x5a                  	// #90
 4025dac:	17ffffe1 	b	4025d30 <get_uboot_info_from_flash+0x68>
 4025db0:	a94153f3 	ldp	x19, x20, [sp, #16]
 4025db4:	a9427bf5 	ldp	x21, x30, [sp, #32]
 4025db8:	9100c3ff 	add	sp, sp, #0x30
 4025dbc:	d65f03c0 	ret

0000000004025dc0 <flash_device_init>:
 4025dc0:	f81f0ffe 	str	x30, [sp, #-16]!
 4025dc4:	94000eeb 	bl	4029970 <is_nand>
 4025dc8:	35000060 	cbnz	w0, 4025dd4 <flash_device_init+0x14>
 4025dcc:	f84107fe 	ldr	x30, [sp], #16
 4025dd0:	14001105 	b	402a1e4 <spi_nor_init>
 4025dd4:	94000f0e 	bl	4029a0c <spi_nand_init>
 4025dd8:	90ffffe0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4025ddc:	912b0000 	add	x0, x0, #0xac0
 4025de0:	94000ec8 	bl	4029900 <spi_nand_block_size>
 4025de4:	528014a0 	mov	w0, #0xa5                  	// #165
 4025de8:	f84107fe 	ldr	x30, [sp], #16
 4025dec:	d65f03c0 	ret

0000000004025df0 <enable_lpds_lock>:
 4025df0:	d2800580 	mov	x0, #0x2c                  	// #44
 4025df4:	f2a22240 	movk	x0, #0x1112, lsl #16
 4025df8:	b9400001 	ldr	w1, [x0]
 4025dfc:	32000021 	orr	w1, w1, #0x1
 4025e00:	b9000001 	str	w1, [x0]
 4025e04:	b9400000 	ldr	w0, [x0]
 4025e08:	37000200 	tbnz	w0, #0, 4025e48 <enable_lpds_lock+0x58>
 4025e0c:	f81f0ffe 	str	x30, [sp, #-16]!
 4025e10:	90000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4025e14:	91318800 	add	x0, x0, #0xc62
 4025e18:	94000b90 	bl	4028c58 <log_serial_puts>
 4025e1c:	d2800040 	mov	x0, #0x2                   	// #2
 4025e20:	94000baa 	bl	4028cc8 <mdelay>
 4025e24:	94000b99 	bl	4028c88 <timer_deinit>
 4025e28:	d2800080 	mov	x0, #0x4                   	// #4
 4025e2c:	52800021 	mov	w1, #0x1                   	// #1
 4025e30:	f2a22040 	movk	x0, #0x1102, lsl #16
 4025e34:	f84107fe 	ldr	x30, [sp], #16
 4025e38:	b9000001 	str	w1, [x0]
 4025e3c:	b9000001 	str	w1, [x0]
 4025e40:	b9000001 	str	w1, [x0]
 4025e44:	d65f03c0 	ret
 4025e48:	d65f03c0 	ret

0000000004025e4c <set_low_power_mode>:
 4025e4c:	d2800382 	mov	x2, #0x1c                  	// #28
 4025e50:	f2a22242 	movk	x2, #0x1112, lsl #16
 4025e54:	b9400041 	ldr	w1, [x2]
 4025e58:	33001c01 	bfxil	w1, w0, #0, #8
 4025e5c:	b9000041 	str	w1, [x2]
 4025e60:	d65f03c0 	ret

0000000004025e64 <get_low_power_mode>:
 4025e64:	d2800380 	mov	x0, #0x1c                  	// #28
 4025e68:	f2a22240 	movk	x0, #0x1112, lsl #16
 4025e6c:	b9400000 	ldr	w0, [x0]
 4025e70:	12001c00 	and	w0, w0, #0xff
 4025e74:	d65f03c0 	ret

0000000004025e78 <clear_lpds>:
 4025e78:	d2800580 	mov	x0, #0x2c                  	// #44
 4025e7c:	f81f0ffe 	str	x30, [sp, #-16]!
 4025e80:	f2a22240 	movk	x0, #0x1112, lsl #16
 4025e84:	b9400000 	ldr	w0, [x0]
 4025e88:	37000100 	tbnz	w0, #0, 4025ea8 <clear_lpds+0x30>
 4025e8c:	d2800600 	mov	x0, #0x30                  	// #48
 4025e90:	d2802601 	mov	x1, #0x130                 	// #304
 4025e94:	f2a22240 	movk	x0, #0x1112, lsl #16
 4025e98:	f2a22241 	movk	x1, #0x1112, lsl #16
 4025e9c:	b800441f 	str	wzr, [x0], #4
 4025ea0:	eb01001f 	cmp	x0, x1
 4025ea4:	54ffffc1 	b.ne	4025e9c <clear_lpds+0x24>  // b.any
 4025ea8:	52800000 	mov	w0, #0x0                   	// #0
 4025eac:	97ffffe8 	bl	4025e4c <set_low_power_mode>
 4025eb0:	f84107fe 	ldr	x30, [sp], #16
 4025eb4:	90000060 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4025eb8:	9131ec00 	add	x0, x0, #0xc7b
 4025ebc:	14000b67 	b	4028c58 <log_serial_puts>

0000000004025ec0 <lpds_hash_get>:
 4025ec0:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4025ec4:	2a0203e3 	mov	w3, w2
 4025ec8:	2a0103f4 	mov	w20, w1
 4025ecc:	aa0003f3 	mov	x19, x0
 4025ed0:	d2800004 	mov	x4, #0x0                   	// #0
 4025ed4:	52800002 	mov	w2, #0x0                   	// #0
 4025ed8:	d2800401 	mov	x1, #0x20                  	// #32
 4025edc:	f9000bfe 	str	x30, [sp, #16]
 4025ee0:	94000b92 	bl	4028d28 <memset_ss>
 4025ee4:	7102941f 	cmp	w0, #0xa5
 4025ee8:	54000161 	b.ne	4025f14 <lpds_hash_get+0x54>  // b.any
 4025eec:	d2800002 	mov	x2, #0x0                   	// #0
 4025ef0:	0b020281 	add	w1, w20, w2
 4025ef4:	b9400021 	ldr	w1, [x1]
 4025ef8:	b8226a61 	str	w1, [x19, x2]
 4025efc:	91001042 	add	x2, x2, #0x4
 4025f00:	f100805f 	cmp	x2, #0x20
 4025f04:	54ffff61 	b.ne	4025ef0 <lpds_hash_get+0x30>  // b.any
 4025f08:	f9400bfe 	ldr	x30, [sp, #16]
 4025f0c:	a8c253f3 	ldp	x19, x20, [sp], #32
 4025f10:	d65f03c0 	ret
 4025f14:	52800b40 	mov	w0, #0x5a                  	// #90
 4025f18:	17fffffc 	b	4025f08 <lpds_hash_get+0x48>

0000000004025f1c <verify_all_lpds_data>:
 4025f1c:	d2800380 	mov	x0, #0x1c                  	// #28
 4025f20:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 4025f24:	f2a22240 	movk	x0, #0x1112, lsl #16
 4025f28:	b9400013 	ldr	w19, [x0]
 4025f2c:	12001e73 	and	w19, w19, #0xff
 4025f30:	71016a7f 	cmp	w19, #0x5a
 4025f34:	54000080 	b.eq	4025f44 <verify_all_lpds_data+0x28>  // b.none
 4025f38:	528014a0 	mov	w0, #0xa5                  	// #165
 4025f3c:	a8c17bf3 	ldp	x19, x30, [sp], #16
 4025f40:	d65f03c0 	ret
 4025f44:	d2800601 	mov	x1, #0x30                  	// #48
 4025f48:	52802200 	mov	w0, #0x110                 	// #272
 4025f4c:	52801c02 	mov	w2, #0xe0                  	// #224
 4025f50:	f2a22241 	movk	x1, #0x1112, lsl #16
 4025f54:	72a22240 	movk	w0, #0x1112, lsl #16
 4025f58:	97fff69f 	bl	40239d4 <lpds_hash_cmp>
 4025f5c:	7102941f 	cmp	w0, #0xa5
 4025f60:	54fffec0 	b.eq	4025f38 <verify_all_lpds_data+0x1c>  // b.none
 4025f64:	52800d60 	mov	w0, #0x6b                  	// #107
 4025f68:	94000068 	bl	4026108 <err_print>
 4025f6c:	2a1303e0 	mov	w0, w19
 4025f70:	17fffff3 	b	4025f3c <verify_all_lpds_data+0x20>

0000000004025f74 <calculate_all_lpds_data>:
 4025f74:	d2802202 	mov	x2, #0x110                 	// #272
 4025f78:	52800600 	mov	w0, #0x30                  	// #48
 4025f7c:	52800004 	mov	w4, #0x0                   	// #0
 4025f80:	52800403 	mov	w3, #0x20                  	// #32
 4025f84:	f2a22242 	movk	x2, #0x1112, lsl #16
 4025f88:	52801c01 	mov	w1, #0xe0                  	// #224
 4025f8c:	72a22240 	movk	w0, #0x1112, lsl #16
 4025f90:	17fffad6 	b	4024ae8 <calc_hash>

0000000004025f94 <get_random_num>:
 4025f94:	d29c2101 	mov	x1, #0xe108                	// #57608
 4025f98:	f2a203c1 	movk	x1, #0x101e, lsl #16
 4025f9c:	b9400020 	ldr	w0, [x1]
 4025fa0:	f240101f 	tst	x0, #0x1f
 4025fa4:	54ffffc0 	b.eq	4025f9c <get_random_num+0x8>  // b.none
 4025fa8:	d29c2000 	mov	x0, #0xe100                	// #57600
 4025fac:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4025fb0:	b9400000 	ldr	w0, [x0]
 4025fb4:	d65f03c0 	ret

0000000004025fb8 <failure_process>:
 4025fb8:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4025fbc:	2a0003f3 	mov	w19, w0
 4025fc0:	f9000bfe 	str	x30, [sp, #16]
 4025fc4:	97ffffa8 	bl	4025e64 <get_low_power_mode>
 4025fc8:	7101681f 	cmp	w0, #0x5a
 4025fcc:	54000081 	b.ne	4025fdc <failure_process+0x24>  // b.any
 4025fd0:	97ffffaa 	bl	4025e78 <clear_lpds>
 4025fd4:	52800000 	mov	w0, #0x0                   	// #0
 4025fd8:	1400000d 	b	402600c <failure_process+0x54>
 4025fdc:	d2800280 	mov	x0, #0x14                  	// #20
 4025fe0:	f2a22040 	movk	x0, #0x1102, lsl #16
 4025fe4:	b9400000 	ldr	w0, [x0]
 4025fe8:	72001c1f 	tst	w0, #0xff
 4025fec:	540003e1 	b.ne	4026068 <failure_process+0xb0>  // b.any
 4025ff0:	9400050a 	bl	4027418 <opt_get_boot_backup_enable>
 4025ff4:	2a0003f4 	mov	w20, w0
 4025ff8:	7100041f 	cmp	w0, #0x1
 4025ffc:	54000361 	b.ne	4026068 <failure_process+0xb0>  // b.any
 4026000:	97fff419 	bl	4023064 <get_verify_backup_img_reg>
 4026004:	35000320 	cbnz	w0, 4026068 <failure_process+0xb0>
 4026008:	2a1403e0 	mov	w0, w20
 402600c:	97fff410 	bl	402304c <set_verify_backup_img_reg>
 4026010:	97fff404 	bl	4023020 <clean_boot_img_addr_size>
 4026014:	f0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4026018:	91326c00 	add	x0, x0, #0xc9b
 402601c:	94000b0f 	bl	4028c58 <log_serial_puts>
 4026020:	940004dd 	bl	4027394 <is_sec_dbg_enable>
 4026024:	5292cb41 	mov	w1, #0x965a                	// #38490
 4026028:	72a794a1 	movk	w1, #0x3ca5, lsl #16
 402602c:	6b01001f 	cmp	w0, w1
 4026030:	54000041 	b.ne	4026038 <failure_process+0x80>  // b.any
 4026034:	14000000 	b	4026034 <failure_process+0x7c>
 4026038:	d2800040 	mov	x0, #0x2                   	// #2
 402603c:	94000b23 	bl	4028cc8 <mdelay>
 4026040:	94000b12 	bl	4028c88 <timer_deinit>
 4026044:	d2800080 	mov	x0, #0x4                   	// #4
 4026048:	52800021 	mov	w1, #0x1                   	// #1
 402604c:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026050:	f9400bfe 	ldr	x30, [sp, #16]
 4026054:	b9000001 	str	w1, [x0]
 4026058:	b9000001 	str	w1, [x0]
 402605c:	b9000001 	str	w1, [x0]
 4026060:	a8c253f3 	ldp	x19, x20, [sp], #32
 4026064:	d65f03c0 	ret
 4026068:	f0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402606c:	91322c00 	add	x0, x0, #0xc8b
 4026070:	94000afa 	bl	4028c58 <log_serial_puts>
 4026074:	d2902e00 	mov	x0, #0x8170                	// #33136
 4026078:	f2a203c0 	movk	x0, #0x101e, lsl #16
 402607c:	b9400014 	ldr	w20, [x0]
 4026080:	71002a7f 	cmp	w19, #0xa
 4026084:	54000100 	b.eq	40260a4 <failure_process+0xec>  // b.none
 4026088:	97fffc2d 	bl	402513c <is_ab_boot_enable>
 402608c:	72001c1f 	tst	w0, #0xff
 4026090:	540000a0 	b.eq	40260a4 <failure_process+0xec>  // b.none
 4026094:	97fff4d7 	bl	40233f0 <boot_control_get_reset_state>
 4026098:	12001c00 	and	w0, w0, #0xff
 402609c:	7100041f 	cmp	w0, #0x1
 40260a0:	1a9f0273 	csel	w19, w19, wzr, eq  // eq = none
 40260a4:	d2902e00 	mov	x0, #0x8170                	// #33136
 40260a8:	121c6e94 	and	w20, w20, #0xfffffff0
 40260ac:	f2a203c0 	movk	x0, #0x101e, lsl #16
 40260b0:	2a130293 	orr	w19, w20, w19
 40260b4:	b9000013 	str	w19, [x0]
 40260b8:	17ffffc7 	b	4025fd4 <failure_process+0x1c>

00000000040260bc <stack_chk_guard_setup>:
 40260bc:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
 40260c0:	910003fd 	mov	x29, sp
 40260c4:	9100b3e0 	add	x0, sp, #0x2c
 40260c8:	f9000bf3 	str	x19, [sp, #16]
 40260cc:	b9002fff 	str	wzr, [sp, #44]
 40260d0:	94000a46 	bl	40289e8 <uapi_drv_cipher_trng_get_random>
 40260d4:	b9402fe0 	ldr	w0, [sp, #44]
 40260d8:	f0ffffd3 	adrp	x19, 4021000 <gs_misc_record-0xa80>
 40260dc:	d3607c00 	lsl	x0, x0, #32
 40260e0:	f9077260 	str	x0, [x19, #3808]
 40260e4:	9100b3e0 	add	x0, sp, #0x2c
 40260e8:	94000a40 	bl	40289e8 <uapi_drv_cipher_trng_get_random>
 40260ec:	f9477261 	ldr	x1, [x19, #3808]
 40260f0:	b9402fe0 	ldr	w0, [sp, #44]
 40260f4:	aa010000 	orr	x0, x0, x1
 40260f8:	f9077260 	str	x0, [x19, #3808]
 40260fc:	f9400bf3 	ldr	x19, [sp, #16]
 4026100:	a8c37bfd 	ldp	x29, x30, [sp], #48
 4026104:	d65f03c0 	ret

0000000004026108 <err_print>:
 4026108:	d10083ff 	sub	sp, sp, #0x20
 402610c:	f0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4026110:	a9017bf3 	stp	x19, x30, [sp, #16]
 4026114:	f9477022 	ldr	x2, [x1, #3808]
 4026118:	f90007e2 	str	x2, [sp, #8]
 402611c:	d2800002 	mov	x2, #0x0                   	// #0
 4026120:	7219001f 	tst	w0, #0x80
 4026124:	54000481 	b.ne	40261b4 <err_print+0xac>  // b.any
 4026128:	12001c04 	and	w4, w0, #0xff
 402612c:	52800143 	mov	w3, #0xa                   	// #10
 4026130:	52800c82 	mov	w2, #0x64                  	// #100
 4026134:	39000fff 	strb	wzr, [sp, #3]
 4026138:	1ac30880 	udiv	w0, w4, w3
 402613c:	1ac20882 	udiv	w2, w4, w2
 4026140:	1ac30801 	udiv	w1, w0, w3
 4026144:	1100c042 	add	w2, w2, #0x30
 4026148:	12001c42 	and	w2, w2, #0xff
 402614c:	390003e2 	strb	w2, [sp]
 4026150:	1b038021 	msub	w1, w1, w3, w0
 4026154:	1b039000 	msub	w0, w0, w3, w4
 4026158:	1100c021 	add	w1, w1, #0x30
 402615c:	1100c000 	add	w0, w0, #0x30
 4026160:	12001c21 	and	w1, w1, #0xff
 4026164:	12001c00 	and	w0, w0, #0xff
 4026168:	390007e1 	strb	w1, [sp, #1]
 402616c:	39000be0 	strb	w0, [sp, #2]
 4026170:	7100c05f 	cmp	w2, #0x30
 4026174:	540002e1 	b.ne	40261d0 <err_print+0xc8>  // b.any
 4026178:	7100c03f 	cmp	w1, #0x30
 402617c:	540002e1 	b.ne	40261d8 <err_print+0xd0>  // b.any
 4026180:	7100c01f 	cmp	w0, #0x30
 4026184:	1a9f17f3 	cset	w19, eq  // eq = none
 4026188:	11000a73 	add	w19, w19, #0x2
 402618c:	52800141 	mov	w1, #0xa                   	// #10
 4026190:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4026194:	9400085b 	bl	4028300 <serial_putc>
 4026198:	71000e7f 	cmp	w19, #0x3
 402619c:	54000221 	b.ne	40261e0 <err_print+0xd8>  // b.any
 40261a0:	52800601 	mov	w1, #0x30                  	// #48
 40261a4:	52a22080 	mov	w0, #0x11040000            	// #285474816
 40261a8:	94000856 	bl	4028300 <serial_putc>
 40261ac:	d2800140 	mov	x0, #0xa                   	// #10
 40261b0:	94000ac6 	bl	4028cc8 <mdelay>
 40261b4:	f0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40261b8:	f94007e1 	ldr	x1, [sp, #8]
 40261bc:	f9477002 	ldr	x2, [x0, #3808]
 40261c0:	eb020021 	subs	x1, x1, x2
 40261c4:	d2800002 	mov	x2, #0x0                   	// #0
 40261c8:	54000140 	b.eq	40261f0 <err_print+0xe8>  // b.none
 40261cc:	9400000c 	bl	40261fc <__stack_chk_fail>
 40261d0:	52800013 	mov	w19, #0x0                   	// #0
 40261d4:	17ffffee 	b	402618c <err_print+0x84>
 40261d8:	52800033 	mov	w19, #0x1                   	// #1
 40261dc:	17ffffec 	b	402618c <err_print+0x84>
 40261e0:	92401e60 	and	x0, x19, #0xff
 40261e4:	8b2063e0 	add	x0, sp, x0
 40261e8:	94000a9c 	bl	4028c58 <log_serial_puts>
 40261ec:	17fffff0 	b	40261ac <err_print+0xa4>
 40261f0:	a9417bf3 	ldp	x19, x30, [sp, #16]
 40261f4:	910083ff 	add	sp, sp, #0x20
 40261f8:	d65f03c0 	ret

00000000040261fc <__stack_chk_fail>:
 40261fc:	a9bf7bfd 	stp	x29, x30, [sp, #-16]!
 4026200:	528007c0 	mov	w0, #0x3e                  	// #62
 4026204:	910003fd 	mov	x29, sp
 4026208:	97ffffc0 	bl	4026108 <err_print>
 402620c:	d2800040 	mov	x0, #0x2                   	// #2
 4026210:	94000aae 	bl	4028cc8 <mdelay>
 4026214:	94000a9d 	bl	4028c88 <timer_deinit>
 4026218:	d2800080 	mov	x0, #0x4                   	// #4
 402621c:	52800021 	mov	w1, #0x1                   	// #1
 4026220:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026224:	b9000001 	str	w1, [x0]
 4026228:	b9000001 	str	w1, [x0]
 402622c:	b9000001 	str	w1, [x0]
 4026230:	a8c17bfd 	ldp	x29, x30, [sp], #16
 4026234:	d65f03c0 	ret

0000000004026238 <first_recv_len_check>:
 4026238:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402623c:	aa0003f3 	mov	x19, x0
 4026240:	97fff530 	bl	4023700 <get_ddr_param_info_addr>
 4026244:	2a0003e2 	mov	w2, w0
 4026248:	29440041 	ldp	w1, w0, [x2, #32]
 402624c:	0b000021 	add	w1, w1, w0
 4026250:	110c0021 	add	w1, w1, #0x300
 4026254:	eb21c27f 	cmp	x19, w1, sxtw
 4026258:	540000c0 	b.eq	4026270 <first_recv_len_check+0x38>  // b.none
 402625c:	52800860 	mov	w0, #0x43                  	// #67
 4026260:	97ffffaa 	bl	4026108 <err_print>
 4026264:	52800b40 	mov	w0, #0x5a                  	// #90
 4026268:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402626c:	d65f03c0 	ret
 4026270:	528014a0 	mov	w0, #0xa5                  	// #165
 4026274:	17fffffd 	b	4026268 <first_recv_len_check+0x30>

0000000004026278 <sram_to_npu_info>:
 4026278:	d65f03c0 	ret

000000000402627c <sys_watchdog_enable>:
 402627c:	5298d802 	mov	w2, #0xc6c0                	// #50880
 4026280:	52807d01 	mov	w1, #0x3e8                 	// #1000
 4026284:	72a005a2 	movk	w2, #0x2d, lsl #16
 4026288:	52800000 	mov	w0, #0x0                   	// #0
 402628c:	14000a93 	b	4028cd8 <watchdog_enable>

0000000004026290 <main_entry>:
 4026290:	d10503ff 	sub	sp, sp, #0x140
 4026294:	f0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4026298:	a90f53f3 	stp	x19, x20, [sp, #240]
 402629c:	a9105bf5 	stp	x21, x22, [sp, #256]
 40262a0:	52896f15 	mov	w21, #0x4b78                	// #19320
 40262a4:	72b2d4b5 	movk	w21, #0x96a5, lsl #16
 40262a8:	a91163f7 	stp	x23, x24, [sp, #272]
 40262ac:	a9126bf9 	stp	x25, x26, [sp, #288]
 40262b0:	f9009bfe 	str	x30, [sp, #304]
 40262b4:	f9477001 	ldr	x1, [x0, #3808]
 40262b8:	f90077e1 	str	x1, [sp, #232]
 40262bc:	d2800001 	mov	x1, #0x0                   	// #0
 40262c0:	94000aa6 	bl	4028d58 <save_cur_point_syscnt>
 40262c4:	92a06001 	mov	x1, #0xfffffffffcffffff    	// #-50331649
 40262c8:	52a06000 	mov	w0, #0x3000000             	// #50331648
 40262cc:	940009ff 	bl	4028ac8 <uapi_drv_klad_rootkey_disable>
 40262d0:	97fff4f9 	bl	40236b4 <get_otfl_heap_addr>
 40262d4:	52900001 	mov	w1, #0x8000                	// #32768
 40262d8:	940002ff 	bl	4026ed4 <malloc_init>
 40262dc:	94000a67 	bl	4028c78 <timer_init>
 40262e0:	a9037fff 	stp	xzr, xzr, [sp, #48]
 40262e4:	90000000 	adrp	x0, 4026000 <failure_process+0x48>
 40262e8:	913f2000 	add	x0, x0, #0xfc8
 40262ec:	f9000fe0 	str	x0, [sp, #24]
 40262f0:	b0000000 	adrp	x0, 4027000 <malloc+0x38>
 40262f4:	91011000 	add	x0, x0, #0x44
 40262f8:	f90013e0 	str	x0, [sp, #32]
 40262fc:	d0000000 	adrp	x0, 4028000 <end_svb+0x30>
 4026300:	910c0000 	add	x0, x0, #0x300
 4026304:	f90017e0 	str	x0, [sp, #40]
 4026308:	910063e0 	add	x0, sp, #0x18
 402630c:	f90023ff 	str	xzr, [sp, #64]
 4026310:	94000a0e 	bl	4028b48 <uapi_drv_register_func>
 4026314:	94000985 	bl	4028928 <uapi_drv_cipher_init>
 4026318:	97fff37f 	bl	4023114 <get_board_param_index>
 402631c:	2a0003f4 	mov	w20, w0
 4026320:	97fff38a 	bl	4023148 <get_data_channel_type>
 4026324:	2a0003f3 	mov	w19, w0
 4026328:	52896f00 	mov	w0, #0x4b78                	// #19320
 402632c:	72b2cb40 	movk	w0, #0x965a, lsl #16
 4026330:	6b00027f 	cmp	w19, w0
 4026334:	540006e1 	b.ne	4026410 <main_entry+0x180>  // b.any
 4026338:	52800040 	mov	w0, #0x2                   	// #2
 402633c:	97ffff1f 	bl	4025fb8 <failure_process>
 4026340:	97fffb7f 	bl	402513c <is_ab_boot_enable>
 4026344:	6b15027f 	cmp	w19, w21
 4026348:	529690e1 	mov	w1, #0xb487                	// #46215
 402634c:	1a9f17f5 	cset	w21, eq  // eq = none
 4026350:	72ad2b41 	movk	w1, #0x695a, lsl #16
 4026354:	6b01027f 	cmp	w19, w1
 4026358:	1a9f17e1 	cset	w1, eq  // eq = none
 402635c:	2a0102b5 	orr	w21, w21, w1
 4026360:	72001c1f 	tst	w0, #0xff
 4026364:	54000160 	b.eq	4026390 <main_entry+0x100>  // b.none
 4026368:	34000155 	cbz	w21, 4026390 <main_entry+0x100>
 402636c:	2a1303e0 	mov	w0, w19
 4026370:	97fff394 	bl	40231c0 <boot_control_get_boot_slot_num>
 4026374:	7102941f 	cmp	w0, #0xa5
 4026378:	540000c0 	b.eq	4026390 <main_entry+0x100>  // b.none
 402637c:	f0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4026380:	9132a000 	add	x0, x0, #0xca8
 4026384:	94000a35 	bl	4028c58 <log_serial_puts>
 4026388:	52800140 	mov	w0, #0xa                   	// #10
 402638c:	97ffff0b 	bl	4025fb8 <failure_process>
 4026390:	52968f17 	mov	w23, #0xb478                	// #46200
 4026394:	529690e0 	mov	w0, #0xb487                	// #46215
 4026398:	72ad34b7 	movk	w23, #0x69a5, lsl #16
 402639c:	6b17027f 	cmp	w19, w23
 40263a0:	1a9f17f6 	cset	w22, eq  // eq = none
 40263a4:	72b2cb40 	movk	w0, #0x965a, lsl #16
 40263a8:	6b00027f 	cmp	w19, w0
 40263ac:	1a9f17e0 	cset	w0, eq  // eq = none
 40263b0:	2a0002d6 	orr	w22, w22, w0
 40263b4:	34002c76 	cbz	w22, 4026940 <main_entry+0x6b0>
 40263b8:	52800000 	mov	w0, #0x0                   	// #0
 40263bc:	94000a4b 	bl	4028ce8 <watchdog_disable>
 40263c0:	6b17027f 	cmp	w19, w23
 40263c4:	540002e1 	b.ne	4026420 <main_entry+0x190>  // b.any
 40263c8:	2a1403e0 	mov	w0, w20
 40263cc:	94000891 	bl	4028610 <send_board_type_to_uart>
 40263d0:	7102941f 	cmp	w0, #0xa5
 40263d4:	54000060 	b.eq	40263e0 <main_entry+0x150>  // b.none
 40263d8:	52800060 	mov	w0, #0x3                   	// #3
 40263dc:	97fffef7 	bl	4025fb8 <failure_process>
 40263e0:	97fff4c3 	bl	40236ec <get_ree_key_area_addr>
 40263e4:	2a0003e0 	mov	w0, w0
 40263e8:	910063e1 	add	x1, sp, #0x18
 40263ec:	52880002 	mov	w2, #0x4000                	// #16384
 40263f0:	94000a1e 	bl	4028c68 <copy_from_uart>
 40263f4:	7102941f 	cmp	w0, #0xa5
 40263f8:	54000a20 	b.eq	402653c <main_entry+0x2ac>  // b.none
 40263fc:	52800820 	mov	w0, #0x41                  	// #65
 4026400:	97ffff42 	bl	4026108 <err_print>
 4026404:	52800060 	mov	w0, #0x3                   	// #3
 4026408:	97fffeec 	bl	4025fb8 <failure_process>
 402640c:	14000050 	b	402654c <main_entry+0x2bc>
 4026410:	6b15027f 	cmp	w19, w21
 4026414:	54fff961 	b.ne	4026340 <main_entry+0xb0>  // b.any
 4026418:	97fffe6a 	bl	4025dc0 <flash_device_init>
 402641c:	17ffffc9 	b	4026340 <main_entry+0xb0>
 4026420:	52800b40 	mov	w0, #0x5a                  	// #90
 4026424:	d2800203 	mov	x3, #0x10                  	// #16
 4026428:	52800002 	mov	w2, #0x0                   	// #0
 402642c:	aa0303e1 	mov	x1, x3
 4026430:	b9000fe0 	str	w0, [sp, #12]
 4026434:	910063e0 	add	x0, sp, #0x18
 4026438:	f9000bff 	str	xzr, [sp, #16]
 402643c:	94000a43 	bl	4028d48 <memset_s>
 4026440:	35000780 	cbnz	w0, 4026530 <main_entry+0x2a0>
 4026444:	910363f9 	add	x25, sp, #0xd8
 4026448:	52a00038 	mov	w24, #0x10000               	// #65536
 402644c:	940009eb 	bl	4028bf8 <usb3_driver_init>
 4026450:	910043e1 	add	x1, sp, #0x10
 4026454:	aa1903e0 	mov	x0, x25
 4026458:	528001c2 	mov	w2, #0xe                   	// #14
 402645c:	940009e3 	bl	4028be8 <copy_from_usb>
 4026460:	b9000fe0 	str	w0, [sp, #12]
 4026464:	b9400fe0 	ldr	w0, [sp, #12]
 4026468:	7102941f 	cmp	w0, #0xa5
 402646c:	54000581 	b.ne	402651c <main_entry+0x28c>  // b.any
 4026470:	f9400be0 	ldr	x0, [sp, #16]
 4026474:	f100381f 	cmp	x0, #0xe
 4026478:	54000521 	b.ne	402651c <main_entry+0x28c>  // b.any
 402647c:	394363e0 	ldrb	w0, [sp, #216]
 4026480:	7103381f 	cmp	w0, #0xce
 4026484:	540004c1 	b.ne	402651c <main_entry+0x28c>  // b.any
 4026488:	39436be0 	ldrb	w0, [sp, #218]
 402648c:	394367e1 	ldrb	w1, [sp, #217]
 4026490:	2a2003e0 	mvn	w0, w0
 4026494:	6b20003f 	cmp	w1, w0, uxtb
 4026498:	54000421 	b.ne	402651c <main_entry+0x28c>  // b.any
 402649c:	7941cbf7 	ldrh	w23, [sp, #228]
 40264a0:	d280001a 	mov	x26, #0x0                   	// #0
 40264a4:	52800001 	mov	w1, #0x0                   	// #0
 40264a8:	5ac006f7 	rev16	w23, w23
 40264ac:	12003ef7 	and	w23, w23, #0xffff
 40264b0:	387a6b20 	ldrb	w0, [x25, x26]
 40264b4:	9100075a 	add	x26, x26, #0x1
 40264b8:	9400026a 	bl	4026e60 <cal_crc_perbyte>
 40264bc:	12003c01 	and	w1, w0, #0xffff
 40264c0:	f100335f 	cmp	x26, #0xc
 40264c4:	54ffff61 	b.ne	40264b0 <main_entry+0x220>  // b.any
 40264c8:	6b0102ff 	cmp	w23, w1
 40264cc:	54000281 	b.ne	402651c <main_entry+0x28c>  // b.any
 40264d0:	910323e1 	add	x1, sp, #0xc8
 40264d4:	52800162 	mov	w2, #0xb                   	// #11
 40264d8:	2a1403e0 	mov	w0, w20
 40264dc:	97fff2e7 	bl	4023078 <build_board_type_frame>
 40264e0:	d2800161 	mov	x1, #0xb                   	// #11
 40264e4:	910323e0 	add	x0, sp, #0xc8
 40264e8:	940009c8 	bl	4028c08 <send_to_usb>
 40264ec:	f0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40264f0:	91334400 	add	x0, x0, #0xcd1
 40264f4:	940009d9 	bl	4028c58 <log_serial_puts>
 40264f8:	97fff47d 	bl	40236ec <get_ree_key_area_addr>
 40264fc:	2a0003e0 	mov	w0, w0
 4026500:	910063e1 	add	x1, sp, #0x18
 4026504:	52880002 	mov	w2, #0x4000                	// #16384
 4026508:	940009b8 	bl	4028be8 <copy_from_usb>
 402650c:	7102941f 	cmp	w0, #0xa5
 4026510:	54000160 	b.eq	402653c <main_entry+0x2ac>  // b.none
 4026514:	52800840 	mov	w0, #0x42                  	// #66
 4026518:	17ffffba 	b	4026400 <main_entry+0x170>
 402651c:	71000718 	subs	w24, w24, #0x1
 4026520:	54fff981 	b.ne	4026450 <main_entry+0x1c0>  // b.any
 4026524:	f0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4026528:	91330000 	add	x0, x0, #0xcc0
 402652c:	940009cb 	bl	4028c58 <log_serial_puts>
 4026530:	52800060 	mov	w0, #0x3                   	// #3
 4026534:	97fffea1 	bl	4025fb8 <failure_process>
 4026538:	17fffff0 	b	40264f8 <main_entry+0x268>
 402653c:	f9400fe0 	ldr	x0, [sp, #24]
 4026540:	97ffff3e 	bl	4026238 <first_recv_len_check>
 4026544:	7102941f 	cmp	w0, #0xa5
 4026548:	54fff5e1 	b.ne	4026404 <main_entry+0x174>  // b.any
 402654c:	97ffff4c 	bl	402627c <sys_watchdog_enable>
 4026550:	97fff55a 	bl	4023ab8 <handle_ree_key_area>
 4026554:	7102941f 	cmp	w0, #0xa5
 4026558:	54000120 	b.eq	402657c <main_entry+0x2ec>  // b.none
 402655c:	52800d00 	mov	w0, #0x68                  	// #104
 4026560:	97fffeea 	bl	4026108 <err_print>
 4026564:	528000c0 	mov	w0, #0x6                   	// #6
 4026568:	97fffe94 	bl	4025fb8 <failure_process>
 402656c:	52800d00 	mov	w0, #0x68                  	// #104
 4026570:	97fffee6 	bl	4026108 <err_print>
 4026574:	528000c0 	mov	w0, #0x6                   	// #6
 4026578:	97fffe90 	bl	4025fb8 <failure_process>
 402657c:	2a1403e0 	mov	w0, w20
 4026580:	97fff5c2 	bl	4023c88 <handle_ddr_param_info>
 4026584:	7102941f 	cmp	w0, #0xa5
 4026588:	54000120 	b.eq	40265ac <main_entry+0x31c>  // b.none
 402658c:	52800d20 	mov	w0, #0x69                  	// #105
 4026590:	97fffede 	bl	4026108 <err_print>
 4026594:	528000e0 	mov	w0, #0x7                   	// #7
 4026598:	97fffe88 	bl	4025fb8 <failure_process>
 402659c:	52800d20 	mov	w0, #0x69                  	// #105
 40265a0:	97fffeda 	bl	4026108 <err_print>
 40265a4:	528000e0 	mov	w0, #0x7                   	// #7
 40265a8:	97fffe84 	bl	4025fb8 <failure_process>
 40265ac:	350000b5 	cbnz	w21, 40265c0 <main_entry+0x330>
 40265b0:	528970e0 	mov	w0, #0x4b87                	// #19335
 40265b4:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 40265b8:	6b00027f 	cmp	w19, w0
 40265bc:	54000161 	b.ne	40265e8 <main_entry+0x358>  // b.any
 40265c0:	f0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40265c4:	913b8000 	add	x0, x0, #0xee0
 40265c8:	2a1303e2 	mov	w2, w19
 40265cc:	2a1403e1 	mov	w1, w20
 40265d0:	91002000 	add	x0, x0, #0x8
 40265d4:	97fffcd9 	bl	4025938 <get_paras_data_from_flash>
 40265d8:	7102941f 	cmp	w0, #0xa5
 40265dc:	54000060 	b.eq	40265e8 <main_entry+0x358>  // b.none
 40265e0:	528000e0 	mov	w0, #0x7                   	// #7
 40265e4:	97fffe75 	bl	4025fb8 <failure_process>
 40265e8:	2a1403e0 	mov	w0, w20
 40265ec:	97fff62e 	bl	4023ea4 <handle_ddr_param>
 40265f0:	7102941f 	cmp	w0, #0xa5
 40265f4:	54000120 	b.eq	4026618 <main_entry+0x388>  // b.none
 40265f8:	52800d40 	mov	w0, #0x6a                  	// #106
 40265fc:	97fffec3 	bl	4026108 <err_print>
 4026600:	528000e0 	mov	w0, #0x7                   	// #7
 4026604:	97fffe6d 	bl	4025fb8 <failure_process>
 4026608:	52800d40 	mov	w0, #0x6a                  	// #106
 402660c:	97fffebf 	bl	4026108 <err_print>
 4026610:	528000e0 	mov	w0, #0x7                   	// #7
 4026614:	97fffe69 	bl	4025fb8 <failure_process>
 4026618:	2a1303e0 	mov	w0, w19
 402661c:	94000a34 	bl	4028eec <bsp_gpll_config>
 4026620:	97fff43d 	bl	4023714 <get_ddr_param_data_addr>
 4026624:	2a0003e1 	mov	w1, w0
 4026628:	f9400022 	ldr	x2, [x1]
 402662c:	d2918561 	mov	x1, #0x8c2b                	// #35883
 4026630:	f2a34dc1 	movk	x1, #0x1a6e, lsl #16
 4026634:	f2cdc341 	movk	x1, #0x6e1a, lsl #32
 4026638:	f2e57181 	movk	x1, #0x2b8c, lsl #48
 402663c:	eb01005f 	cmp	x2, x1
 4026640:	54000041 	b.ne	4026648 <main_entry+0x3b8>  // b.any
 4026644:	1102e000 	add	w0, w0, #0xb8
 4026648:	52800001 	mov	w1, #0x0                   	// #0
 402664c:	940006aa 	bl	40280f4 <init_registers>
 4026650:	d2884400 	mov	x0, #0x4220                	// #16928
 4026654:	52800141 	mov	w1, #0xa                   	// #10
 4026658:	f2a22040 	movk	x0, #0x1102, lsl #16
 402665c:	52800034 	mov	w20, #0x1                   	// #1
 4026660:	b9000001 	str	w1, [x0]
 4026664:	b900041f 	str	wzr, [x0, #4]
 4026668:	b9000414 	str	w20, [x0, #4]
 402666c:	9400043b 	bl	4027758 <start_svb>
 4026670:	2a1403e2 	mov	w2, w20
 4026674:	12800001 	mov	w1, #0xffffffff            	// #-1
 4026678:	2a0103e0 	mov	w0, w1
 402667c:	940006e7 	bl	4028218 <bsp_ddrtrn_training_item>
 4026680:	940006e5 	bl	4028214 <bsp_ddrtrn_retrain_enable>
 4026684:	940006e3 	bl	4028210 <bsp_ddrtrn_dmc_auto_power_down_cfg>
 4026688:	d2802d00 	mov	x0, #0x168                 	// #360
 402668c:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026690:	b9400000 	ldr	w0, [x0]
 4026694:	12003c00 	and	w0, w0, #0xffff
 4026698:	7104001f 	cmp	w0, #0x100
 402669c:	54000049 	b.ls	40266a4 <main_entry+0x414>  // b.plast
 40266a0:	94000704 	bl	40282b0 <bsp_ddrtrn_capat_adapt>
 40266a4:	94000a0d 	bl	4028ed8 <get_ddr_scrub_and_gpll_switch>
 40266a8:	12001c00 	and	w0, w0, #0xff
 40266ac:	7100041f 	cmp	w0, #0x1
 40266b0:	54001b20 	b.eq	4026a14 <main_entry+0x784>  // b.none
 40266b4:	f0ffffd4 	adrp	x20, 4021000 <gs_misc_record-0xa80>
 40266b8:	94000646 	bl	4027fd0 <end_svb>
 40266bc:	913b8294 	add	x20, x20, #0xee0
 40266c0:	340025f6 	cbz	w22, 4026b7c <main_entry+0x8ec>
 40266c4:	52800000 	mov	w0, #0x0                   	// #0
 40266c8:	94000988 	bl	4028ce8 <watchdog_disable>
 40266cc:	52968f00 	mov	w0, #0xb478                	// #46200
 40266d0:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 40266d4:	6b00027f 	cmp	w19, w0
 40266d8:	54002301 	b.ne	4026b38 <main_entry+0x8a8>  // b.any
 40266dc:	f9000fff 	str	xzr, [sp, #24]
 40266e0:	97fff412 	bl	4023728 <get_uboot_info_download_ddr_addr>
 40266e4:	910063e1 	add	x1, sp, #0x18
 40266e8:	2a0003e0 	mov	w0, w0
 40266ec:	52a00202 	mov	w2, #0x100000              	// #1048576
 40266f0:	9400095e 	bl	4028c68 <copy_from_uart>
 40266f4:	7102941f 	cmp	w0, #0xa5
 40266f8:	54002320 	b.eq	4026b5c <main_entry+0x8cc>  // b.none
 40266fc:	52800de0 	mov	w0, #0x6f                  	// #111
 4026700:	97fffe82 	bl	4026108 <err_print>
 4026704:	52800100 	mov	w0, #0x8                   	// #8
 4026708:	97fffe2c 	bl	4025fb8 <failure_process>
 402670c:	97fffedc 	bl	402627c <sys_watchdog_enable>
 4026710:	97fff618 	bl	4023f70 <handle_uboot_info>
 4026714:	7102941f 	cmp	w0, #0xa5
 4026718:	54000120 	b.eq	402673c <main_entry+0x4ac>  // b.none
 402671c:	52800e20 	mov	w0, #0x71                  	// #113
 4026720:	97fffe7a 	bl	4026108 <err_print>
 4026724:	52800100 	mov	w0, #0x8                   	// #8
 4026728:	97fffe24 	bl	4025fb8 <failure_process>
 402672c:	52800e20 	mov	w0, #0x71                  	// #113
 4026730:	97fffe76 	bl	4026108 <err_print>
 4026734:	52800100 	mov	w0, #0x8                   	// #8
 4026738:	97fffe20 	bl	4025fb8 <failure_process>
 402673c:	34000176 	cbz	w22, 4026768 <main_entry+0x4d8>
 4026740:	97fff3fa 	bl	4023728 <get_uboot_info_download_ddr_addr>
 4026744:	92407c03 	and	x3, x0, #0xffffffff
 4026748:	11080002 	add	w2, w0, #0x200
 402674c:	b9402461 	ldr	w1, [x3, #36]
 4026750:	b9408860 	ldr	w0, [x3, #136]
 4026754:	97fff9be 	bl	4024e4c <dma_copy>
 4026758:	7102941f 	cmp	w0, #0xa5
 402675c:	54000060 	b.eq	4026768 <main_entry+0x4d8>  // b.none
 4026760:	52800100 	mov	w0, #0x8                   	// #8
 4026764:	97fffe15 	bl	4025fb8 <failure_process>
 4026768:	350000b5 	cbnz	w21, 402677c <main_entry+0x4ec>
 402676c:	528970e0 	mov	w0, #0x4b87                	// #19335
 4026770:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4026774:	6b00027f 	cmp	w19, w0
 4026778:	54000101 	b.ne	4026798 <main_entry+0x508>  // b.any
 402677c:	2a1303e1 	mov	w1, w19
 4026780:	91002280 	add	x0, x20, #0x8
 4026784:	97fffcf4 	bl	4025b54 <get_uboot_code_from_flash>
 4026788:	7102941f 	cmp	w0, #0xa5
 402678c:	54000060 	b.eq	4026798 <main_entry+0x508>  // b.none
 4026790:	528000e0 	mov	w0, #0x7                   	// #7
 4026794:	97fffe09 	bl	4025fb8 <failure_process>
 4026798:	97fff64f 	bl	40240d4 <handle_uboot_code>
 402679c:	7102941f 	cmp	w0, #0xa5
 40267a0:	54000120 	b.eq	40267c4 <main_entry+0x534>  // b.none
 40267a4:	52800e40 	mov	w0, #0x72                  	// #114
 40267a8:	97fffe58 	bl	4026108 <err_print>
 40267ac:	52800100 	mov	w0, #0x8                   	// #8
 40267b0:	97fffe02 	bl	4025fb8 <failure_process>
 40267b4:	52800e40 	mov	w0, #0x72                  	// #114
 40267b8:	97fffe54 	bl	4026108 <err_print>
 40267bc:	52800100 	mov	w0, #0x8                   	// #8
 40267c0:	97fffdfe 	bl	4025fb8 <failure_process>
 40267c4:	97fff3d9 	bl	4023728 <get_uboot_info_download_ddr_addr>
 40267c8:	2a0003f3 	mov	w19, w0
 40267cc:	d2804003 	mov	x3, #0x200                 	// #512
 40267d0:	aa1303e2 	mov	x2, x19
 40267d4:	aa0303e1 	mov	x1, x3
 40267d8:	b9408a60 	ldr	w0, [x19, #136]
 40267dc:	51080000 	sub	w0, w0, #0x200
 40267e0:	94000956 	bl	4028d38 <memcpy_s>
 40267e4:	35000220 	cbnz	w0, 4026828 <main_entry+0x598>
 40267e8:	b9408a75 	ldr	w21, [x19, #136]
 40267ec:	97fff3c0 	bl	40236ec <get_ree_key_area_addr>
 40267f0:	d2802003 	mov	x3, #0x100                 	// #256
 40267f4:	2a0003e2 	mov	w2, w0
 40267f8:	510c02b5 	sub	w21, w21, #0x300
 40267fc:	aa0303e1 	mov	x1, x3
 4026800:	aa1503e0 	mov	x0, x21
 4026804:	9400094d 	bl	4028d38 <memcpy_s>
 4026808:	35000100 	cbnz	w0, 4026828 <main_entry+0x598>
 402680c:	b9408a60 	ldr	w0, [x19, #136]
 4026810:	d2856002 	mov	x2, #0x2b00                	// #11008
 4026814:	d2802003 	mov	x3, #0x100                 	// #256
 4026818:	f2a08042 	movk	x2, #0x402, lsl #16
 402681c:	aa0303e1 	mov	x1, x3
 4026820:	51100000 	sub	w0, w0, #0x400
 4026824:	94000945 	bl	4028d38 <memcpy_s>
 4026828:	d28a0501 	mov	x1, #0x5028                	// #20520
 402682c:	f2a22041 	movk	x1, #0x1102, lsl #16
 4026830:	b9400020 	ldr	w0, [x1]
 4026834:	12185c00 	and	w0, w0, #0xffffff00
 4026838:	32001800 	orr	w0, w0, #0x7f
 402683c:	b9000020 	str	w0, [x1]
 4026840:	b9400020 	ldr	w0, [x1]
 4026844:	12001c00 	and	w0, w0, #0xff
 4026848:	7101fc1f 	cmp	w0, #0x7f
 402684c:	540001a0 	b.eq	4026880 <main_entry+0x5f0>  // b.none
 4026850:	f0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4026854:	9133b000 	add	x0, x0, #0xcec
 4026858:	94000900 	bl	4028c58 <log_serial_puts>
 402685c:	d2800040 	mov	x0, #0x2                   	// #2
 4026860:	9400091a 	bl	4028cc8 <mdelay>
 4026864:	94000909 	bl	4028c88 <timer_deinit>
 4026868:	d2800080 	mov	x0, #0x4                   	// #4
 402686c:	52800021 	mov	w1, #0x1                   	// #1
 4026870:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026874:	b9000001 	str	w1, [x0]
 4026878:	b9000001 	str	w1, [x0]
 402687c:	b9000001 	str	w1, [x0]
 4026880:	d28a0600 	mov	x0, #0x5030                	// #20528
 4026884:	52800021 	mov	w1, #0x1                   	// #1
 4026888:	f2a22040 	movk	x0, #0x1102, lsl #16
 402688c:	528d34b5 	mov	w21, #0x69a5                	// #27045
 4026890:	f0000053 	adrp	x19, 4031000 <sync_exception_sp_el0>
 4026894:	91340e73 	add	x19, x19, #0xd03
 4026898:	b9000001 	str	w1, [x0]
 402689c:	72b86b55 	movk	w21, #0xc35a, lsl #16
 40268a0:	97fffdb5 	bl	4025f74 <calculate_all_lpds_data>
 40268a4:	97fffd53 	bl	4025df0 <enable_lpds_lock>
 40268a8:	97fff3a0 	bl	4023728 <get_uboot_info_download_ddr_addr>
 40268ac:	b9408800 	ldr	w0, [x0, #136]
 40268b0:	d2802801 	mov	x1, #0x140                 	// #320
 40268b4:	f2a22041 	movk	x1, #0x1102, lsl #16
 40268b8:	2a0003e0 	mov	w0, w0
 40268bc:	b900003f 	str	wzr, [x1]
 40268c0:	f9000e80 	str	x0, [x20, #24]
 40268c4:	52800000 	mov	w0, #0x0                   	// #0
 40268c8:	94000908 	bl	4028ce8 <watchdog_disable>
 40268cc:	94000923 	bl	4028d58 <save_cur_point_syscnt>
 40268d0:	97fff36f 	bl	402368c <get_partition_area_addr>
 40268d4:	d2a80001 	mov	x1, #0x40000000            	// #1073741824
 40268d8:	b9000020 	str	w0, [x1]
 40268dc:	97fff2c2 	bl	40233e4 <boot_control_get_active_slot>
 40268e0:	12001c00 	and	w0, w0, #0xff
 40268e4:	d2800081 	mov	x1, #0x4                   	// #4
 40268e8:	f2a80001 	movk	x1, #0x4000, lsl #16
 40268ec:	b9000020 	str	w0, [x1]
 40268f0:	d2884080 	mov	x0, #0x4204                	// #16900
 40268f4:	f2a22040 	movk	x0, #0x1102, lsl #16
 40268f8:	52917001 	mov	w1, #0x8b80                	// #35712
 40268fc:	72a02001 	movk	w1, #0x100, lsl #16
 4026900:	b9000001 	str	w1, [x0]
 4026904:	d2840981 	mov	x1, #0x204c                	// #8268
 4026908:	f2a22021 	movk	x1, #0x1101, lsl #16
 402690c:	b9400020 	ldr	w0, [x1]
 4026910:	121e7400 	and	w0, w0, #0xfffffffc
 4026914:	321c0000 	orr	w0, w0, #0x10
 4026918:	b9000020 	str	w0, [x1]
 402691c:	910123e1 	add	x1, sp, #0x48
 4026920:	52800022 	mov	w2, #0x1                   	// #1
 4026924:	52800000 	mov	w0, #0x0                   	// #0
 4026928:	94000935 	bl	4028dfc <ipc_recv_msg>
 402692c:	7100001f 	cmp	w0, #0x0
 4026930:	5400154c 	b.gt	4026bd8 <main_entry+0x948>
 4026934:	aa1303e0 	mov	x0, x19
 4026938:	940008c8 	bl	4028c58 <log_serial_puts>
 402693c:	17fffff8 	b	402691c <main_entry+0x68c>
 4026940:	350000b5 	cbnz	w21, 4026954 <main_entry+0x6c4>
 4026944:	528970e0 	mov	w0, #0x4b87                	// #19335
 4026948:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 402694c:	6b00027f 	cmp	w19, w0
 4026950:	540004e1 	b.ne	40269ec <main_entry+0x75c>  // b.any
 4026954:	97fffd72 	bl	4025f1c <verify_all_lpds_data>
 4026958:	7102941f 	cmp	w0, #0xa5
 402695c:	54000060 	b.eq	4026968 <main_entry+0x6d8>  // b.none
 4026960:	528000e0 	mov	w0, #0x7                   	// #7
 4026964:	97fffd95 	bl	4025fb8 <failure_process>
 4026968:	d2800183 	mov	x3, #0xc                   	// #12
 402696c:	f0ffffd7 	adrp	x23, 4021000 <gs_misc_record-0xa80>
 4026970:	aa0303e1 	mov	x1, x3
 4026974:	913b82f7 	add	x23, x23, #0xee0
 4026978:	52800002 	mov	w2, #0x0                   	// #0
 402697c:	910022f9 	add	x25, x23, #0x8
 4026980:	aa1903e0 	mov	x0, x25
 4026984:	940008f1 	bl	4028d48 <memset_s>
 4026988:	97fff1b7 	bl	4023064 <get_verify_backup_img_reg>
 402698c:	2a0003f8 	mov	w24, w0
 4026990:	d2800280 	mov	x0, #0x14                  	// #20
 4026994:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026998:	b9400000 	ldr	w0, [x0]
 402699c:	72001c1f 	tst	w0, #0xff
 40269a0:	54000161 	b.ne	40269cc <main_entry+0x73c>  // b.any
 40269a4:	9400029d 	bl	4027418 <opt_get_boot_backup_enable>
 40269a8:	7100041f 	cmp	w0, #0x1
 40269ac:	54000101 	b.ne	40269cc <main_entry+0x73c>  // b.any
 40269b0:	340000f8 	cbz	w24, 40269cc <main_entry+0x73c>
 40269b4:	d2800281 	mov	x1, #0x14                  	// #20
 40269b8:	f2a22241 	movk	x1, #0x1112, lsl #16
 40269bc:	b9400021 	ldr	w1, [x1]
 40269c0:	34000061 	cbz	w1, 40269cc <main_entry+0x73c>
 40269c4:	b9000ae0 	str	w0, [x23, #8]
 40269c8:	b90012ff 	str	wzr, [x23, #16]
 40269cc:	2a1303e1 	mov	w1, w19
 40269d0:	910022e0 	add	x0, x23, #0x8
 40269d4:	97fffb7a 	bl	40257bc <get_ree_key_and_paras_info_from_device>
 40269d8:	7102941f 	cmp	w0, #0xa5
 40269dc:	54ffdba0 	b.eq	4026550 <main_entry+0x2c0>  // b.none
 40269e0:	528000e0 	mov	w0, #0x7                   	// #7
 40269e4:	97fffd75 	bl	4025fb8 <failure_process>
 40269e8:	17fffeda 	b	4026550 <main_entry+0x2c0>
 40269ec:	d2800040 	mov	x0, #0x2                   	// #2
 40269f0:	940008b6 	bl	4028cc8 <mdelay>
 40269f4:	940008a5 	bl	4028c88 <timer_deinit>
 40269f8:	d2800080 	mov	x0, #0x4                   	// #4
 40269fc:	52800021 	mov	w1, #0x1                   	// #1
 4026a00:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026a04:	b9000001 	str	w1, [x0]
 4026a08:	b9000001 	str	w1, [x0]
 4026a0c:	b9000001 	str	w1, [x0]
 4026a10:	17fffed0 	b	4026550 <main_entry+0x2c0>
 4026a14:	d2900a01 	mov	x1, #0x8050                	// #32848
 4026a18:	f2a22281 	movk	x1, #0x1114, lsl #16
 4026a1c:	b9400023 	ldr	w3, [x1]
 4026a20:	b9500022 	ldr	w2, [x1, #4096]
 4026a24:	72000c63 	ands	w3, w3, #0xf
 4026a28:	12000c42 	and	w2, w2, #0xf
 4026a2c:	54000080 	b.eq	4026a3c <main_entry+0x7ac>  // b.none
 4026a30:	d2900001 	mov	x1, #0x8000                	// #32768
 4026a34:	f2a22281 	movk	x1, #0x1114, lsl #16
 4026a38:	b9000020 	str	w0, [x1]
 4026a3c:	340000a2 	cbz	w2, 4026a50 <main_entry+0x7c0>
 4026a40:	d2920000 	mov	x0, #0x9000                	// #36864
 4026a44:	52800021 	mov	w1, #0x1                   	// #1
 4026a48:	f2a22280 	movk	x0, #0x1114, lsl #16
 4026a4c:	b9000001 	str	w1, [x0]
 4026a50:	d2925284 	mov	x4, #0x9294                	// #37524
 4026a54:	d2905285 	mov	x5, #0x8294                	// #33428
 4026a58:	f2a22284 	movk	x4, #0x1114, lsl #16
 4026a5c:	f2a22285 	movk	x5, #0x1114, lsl #16
 4026a60:	34001fa3 	cbz	w3, 4026e54 <main_entry+0xbc4>
 4026a64:	b94000a1 	ldr	w1, [x5]
 4026a68:	12000021 	and	w1, w1, #0x1
 4026a6c:	340005e2 	cbz	w2, 4026b28 <main_entry+0x898>
 4026a70:	b9400080 	ldr	w0, [x4]
 4026a74:	12000000 	and	w0, w0, #0x1
 4026a78:	6a00003f 	tst	w1, w0
 4026a7c:	54ffff20 	b.eq	4026a60 <main_entry+0x7d0>  // b.none
 4026a80:	97fffd45 	bl	4025f94 <get_random_num>
 4026a84:	d2884301 	mov	x1, #0x4218                	// #16920
 4026a88:	f2a22041 	movk	x1, #0x1102, lsl #16
 4026a8c:	b9000020 	str	w0, [x1]
 4026a90:	97fffd41 	bl	4025f94 <get_random_num>
 4026a94:	d2884381 	mov	x1, #0x421c                	// #16924
 4026a98:	f2a22041 	movk	x1, #0x1102, lsl #16
 4026a9c:	b9000020 	str	w0, [x1]
 4026aa0:	d2884400 	mov	x0, #0x4220                	// #16928
 4026aa4:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026aa8:	528000a1 	mov	w1, #0x5                   	// #5
 4026aac:	b9000001 	str	w1, [x0]
 4026ab0:	d2884481 	mov	x1, #0x4224                	// #16932
 4026ab4:	f2a22041 	movk	x1, #0x1102, lsl #16
 4026ab8:	52800020 	mov	w0, #0x1                   	// #1
 4026abc:	b900003f 	str	wzr, [x1]
 4026ac0:	b9000020 	str	w0, [x1]
 4026ac4:	b9000420 	str	w0, [x1, #4]
 4026ac8:	97fffd33 	bl	4025f94 <get_random_num>
 4026acc:	340000a3 	cbz	w3, 4026ae0 <main_entry+0x850>
 4026ad0:	d2900000 	mov	x0, #0x8000                	// #32768
 4026ad4:	52800041 	mov	w1, #0x2                   	// #2
 4026ad8:	f2a22280 	movk	x0, #0x1114, lsl #16
 4026adc:	b9000001 	str	w1, [x0]
 4026ae0:	340000a2 	cbz	w2, 4026af4 <main_entry+0x864>
 4026ae4:	d2920000 	mov	x0, #0x9000                	// #36864
 4026ae8:	52800041 	mov	w1, #0x2                   	// #2
 4026aec:	f2a22280 	movk	x0, #0x1114, lsl #16
 4026af0:	b9000001 	str	w1, [x0]
 4026af4:	d2925284 	mov	x4, #0x9294                	// #37524
 4026af8:	d2905285 	mov	x5, #0x8294                	// #33428
 4026afc:	f2a22284 	movk	x4, #0x1114, lsl #16
 4026b00:	f2a22285 	movk	x5, #0x1114, lsl #16
 4026b04:	34001a23 	cbz	w3, 4026e48 <main_entry+0xbb8>
 4026b08:	b94000a0 	ldr	w0, [x5]
 4026b0c:	12000001 	and	w1, w0, #0x1
 4026b10:	34000102 	cbz	w2, 4026b30 <main_entry+0x8a0>
 4026b14:	b9400080 	ldr	w0, [x4]
 4026b18:	12000000 	and	w0, w0, #0x1
 4026b1c:	2a010000 	orr	w0, w0, w1
 4026b20:	35ffff20 	cbnz	w0, 4026b04 <main_entry+0x874>
 4026b24:	17fffee4 	b	40266b4 <main_entry+0x424>
 4026b28:	52800020 	mov	w0, #0x1                   	// #1
 4026b2c:	17ffffd3 	b	4026a78 <main_entry+0x7e8>
 4026b30:	52800000 	mov	w0, #0x0                   	// #0
 4026b34:	17fffffa 	b	4026b1c <main_entry+0x88c>
 4026b38:	97fff2fc 	bl	4023728 <get_uboot_info_download_ddr_addr>
 4026b3c:	2a0003e0 	mov	w0, w0
 4026b40:	910063e1 	add	x1, sp, #0x18
 4026b44:	52a00202 	mov	w2, #0x100000              	// #1048576
 4026b48:	94000828 	bl	4028be8 <copy_from_usb>
 4026b4c:	7102941f 	cmp	w0, #0xa5
 4026b50:	54000060 	b.eq	4026b5c <main_entry+0x8cc>  // b.none
 4026b54:	52800e00 	mov	w0, #0x70                  	// #112
 4026b58:	17fffeea 	b	4026700 <main_entry+0x470>
 4026b5c:	f9400ff7 	ldr	x23, [sp, #24]
 4026b60:	97fff2f2 	bl	4023728 <get_uboot_info_download_ddr_addr>
 4026b64:	b9402400 	ldr	w0, [x0, #36]
 4026b68:	11080000 	add	w0, w0, #0x200
 4026b6c:	eb20c2ff 	cmp	x23, w0, sxtw
 4026b70:	54ffdce0 	b.eq	402670c <main_entry+0x47c>  // b.none
 4026b74:	52800880 	mov	w0, #0x44                  	// #68
 4026b78:	17fffee2 	b	4026700 <main_entry+0x470>
 4026b7c:	350000b5 	cbnz	w21, 4026b90 <main_entry+0x900>
 4026b80:	528970e0 	mov	w0, #0x4b87                	// #19335
 4026b84:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4026b88:	6b00027f 	cmp	w19, w0
 4026b8c:	54000121 	b.ne	4026bb0 <main_entry+0x920>  // b.any
 4026b90:	2a1303e1 	mov	w1, w19
 4026b94:	91002280 	add	x0, x20, #0x8
 4026b98:	97fffc4c 	bl	4025cc8 <get_uboot_info_from_flash>
 4026b9c:	7102941f 	cmp	w0, #0xa5
 4026ba0:	54ffdb80 	b.eq	4026710 <main_entry+0x480>  // b.none
 4026ba4:	52800100 	mov	w0, #0x8                   	// #8
 4026ba8:	97fffd04 	bl	4025fb8 <failure_process>
 4026bac:	17fffed9 	b	4026710 <main_entry+0x480>
 4026bb0:	d2800040 	mov	x0, #0x2                   	// #2
 4026bb4:	94000845 	bl	4028cc8 <mdelay>
 4026bb8:	94000834 	bl	4028c88 <timer_deinit>
 4026bbc:	d2800080 	mov	x0, #0x4                   	// #4
 4026bc0:	52800021 	mov	w1, #0x1                   	// #1
 4026bc4:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026bc8:	b9000001 	str	w1, [x0]
 4026bcc:	b9000001 	str	w1, [x0]
 4026bd0:	b9000001 	str	w1, [x0]
 4026bd4:	17fffecf 	b	4026710 <main_entry+0x480>
 4026bd8:	940001e6 	bl	4027370 <is_soc_tee_enable>
 4026bdc:	6b15001f 	cmp	w0, w21
 4026be0:	b9404be0 	ldr	w0, [sp, #72]
 4026be4:	54000121 	b.ne	4026c08 <main_entry+0x978>  // b.any
 4026be8:	7102841f 	cmp	w0, #0xa1
 4026bec:	54000160 	b.eq	4026c18 <main_entry+0x988>  // b.none
 4026bf0:	52801620 	mov	w0, #0xb1                  	// #177
 4026bf4:	910223e1 	add	x1, sp, #0x88
 4026bf8:	29117fe0 	stp	w0, wzr, [sp, #136]
 4026bfc:	52800020 	mov	w0, #0x1                   	// #1
 4026c00:	9400085d 	bl	4028d74 <ipc_send_msg>
 4026c04:	17ffff46 	b	402691c <main_entry+0x68c>
 4026c08:	7102881f 	cmp	w0, #0xa2
 4026c0c:	54000060 	b.eq	4026c18 <main_entry+0x988>  // b.none
 4026c10:	52801640 	mov	w0, #0xb2                  	// #178
 4026c14:	17fffff8 	b	4026bf4 <main_entry+0x964>
 4026c18:	294a07e0 	ldp	w0, w1, [sp, #80]
 4026c1c:	f0ffffd5 	adrp	x21, 4021000 <gs_misc_record-0xa80>
 4026c20:	aa008020 	orr	x0, x1, x0, lsl #32
 4026c24:	910223e1 	add	x1, sp, #0x88
 4026c28:	f9001280 	str	x0, [x20, #32]
 4026c2c:	52801400 	mov	w0, #0xa0                  	// #160
 4026c30:	29117fe0 	stp	w0, wzr, [sp, #136]
 4026c34:	52800020 	mov	w0, #0x1                   	// #1
 4026c38:	9400084f 	bl	4028d74 <ipc_send_msg>
 4026c3c:	940001cd 	bl	4027370 <is_soc_tee_enable>
 4026c40:	528d34a1 	mov	w1, #0x69a5                	// #27045
 4026c44:	72b86b41 	movk	w1, #0xc35a, lsl #16
 4026c48:	6b01001f 	cmp	w0, w1
 4026c4c:	540004a1 	b.ne	4026ce0 <main_entry+0xa50>  // b.any
 4026c50:	f9401282 	ldr	x2, [x20, #32]
 4026c54:	d2a80020 	mov	x0, #0x40010000            	// #1073807360
 4026c58:	eb00005f 	cmp	x2, x0
 4026c5c:	54000220 	b.eq	4026ca0 <main_entry+0xa10>  // b.none
 4026c60:	52a00041 	mov	w1, #0x20000               	// #131072
 4026c64:	97fff87a 	bl	4024e4c <dma_copy>
 4026c68:	7102941f 	cmp	w0, #0xa5
 4026c6c:	540001a0 	b.eq	4026ca0 <main_entry+0xa10>  // b.none
 4026c70:	f0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4026c74:	91346000 	add	x0, x0, #0xd18
 4026c78:	940007f8 	bl	4028c58 <log_serial_puts>
 4026c7c:	d2800040 	mov	x0, #0x2                   	// #2
 4026c80:	94000812 	bl	4028cc8 <mdelay>
 4026c84:	94000801 	bl	4028c88 <timer_deinit>
 4026c88:	d2800080 	mov	x0, #0x4                   	// #4
 4026c8c:	52800021 	mov	w1, #0x1                   	// #1
 4026c90:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026c94:	b9000001 	str	w1, [x0]
 4026c98:	b9000001 	str	w1, [x0]
 4026c9c:	b9000001 	str	w1, [x0]
 4026ca0:	f94077e0 	ldr	x0, [sp, #232]
 4026ca4:	f94772a1 	ldr	x1, [x21, #3808]
 4026ca8:	eb010000 	subs	x0, x0, x1
 4026cac:	d2800001 	mov	x1, #0x0                   	// #0
 4026cb0:	54000040 	b.eq	4026cb8 <main_entry+0xa28>  // b.none
 4026cb4:	97fffd52 	bl	40261fc <__stack_chk_fail>
 4026cb8:	d2800000 	mov	x0, #0x0                   	// #0
 4026cbc:	a94f53f3 	ldp	x19, x20, [sp, #240]
 4026cc0:	d2a80021 	mov	x1, #0x40010000            	// #1073807360
 4026cc4:	a9505bf5 	ldp	x21, x22, [sp, #256]
 4026cc8:	aa0103f0 	mov	x16, x1
 4026ccc:	a95163f7 	ldp	x23, x24, [sp, #272]
 4026cd0:	a9526bf9 	ldp	x25, x26, [sp, #288]
 4026cd4:	f9409bfe 	ldr	x30, [sp, #304]
 4026cd8:	910503ff 	add	sp, sp, #0x140
 4026cdc:	d61f0200 	br	x16
 4026ce0:	52800201 	mov	w1, #0x10                  	// #16
 4026ce4:	d2c00043 	mov	x3, #0x200000000           	// #8589934592
 4026ce8:	d2a80002 	mov	x2, #0x40000000            	// #1073741824
 4026cec:	72aaaaa1 	movk	w1, #0x5555, lsl #16
 4026cf0:	52800020 	mov	w0, #0x1                   	// #1
 4026cf4:	f9401293 	ldr	x19, [x20, #32]
 4026cf8:	94000700 	bl	40288f8 <config_tzasc>
 4026cfc:	d2840003 	mov	x3, #0x2000                	// #8192
 4026d00:	d29e0002 	mov	x2, #0xf000                	// #61440
 4026d04:	52800201 	mov	w1, #0x10                  	// #16
 4026d08:	f2a80002 	movk	x2, #0x4000, lsl #16
 4026d0c:	72aaa001 	movk	w1, #0x5500, lsl #16
 4026d10:	52800040 	mov	w0, #0x2                   	// #2
 4026d14:	f2a02043 	movk	x3, #0x102, lsl #16
 4026d18:	940006f8 	bl	40288f8 <config_tzasc>
 4026d1c:	d2840002 	mov	x2, #0x2000                	// #8192
 4026d20:	d29e0001 	mov	x1, #0xf000                	// #61440
 4026d24:	f2a02042 	movk	x2, #0x102, lsl #16
 4026d28:	f2a80001 	movk	x1, #0x4000, lsl #16
 4026d2c:	52800040 	mov	w0, #0x2                   	// #2
 4026d30:	9400069d 	bl	40287a4 <tzasc_sec_config_read_back>
 4026d34:	7102941f 	cmp	w0, #0xa5
 4026d38:	54000201 	b.ne	4026d78 <main_entry+0xae8>  // b.any
 4026d3c:	940006b9 	bl	4028820 <tzasc_bypass_disable>
 4026d40:	7102941f 	cmp	w0, #0xa5
 4026d44:	540001a1 	b.ne	4026d78 <main_entry+0xae8>  // b.any
 4026d48:	aa1303e2 	mov	x2, x19
 4026d4c:	52806001 	mov	w1, #0x300                 	// #768
 4026d50:	d2a82060 	mov	x0, #0x41030000            	// #1090715648
 4026d54:	97fff83e 	bl	4024e4c <dma_copy>
 4026d58:	7102941f 	cmp	w0, #0xa5
 4026d5c:	54000121 	b.ne	4026d80 <main_entry+0xaf0>  // b.any
 4026d60:	d2a82060 	mov	x0, #0x41030000            	// #1090715648
 4026d64:	97fff51b 	bl	40241d0 <handle_tee_key>
 4026d68:	7102941f 	cmp	w0, #0xa5
 4026d6c:	54000380 	b.eq	4026ddc <main_entry+0xb4c>  // b.none
 4026d70:	52800e60 	mov	w0, #0x73                  	// #115
 4026d74:	14000002 	b	4026d7c <main_entry+0xaec>
 4026d78:	52800dc0 	mov	w0, #0x6e                  	// #110
 4026d7c:	97fffce3 	bl	4026108 <err_print>
 4026d80:	d2902e01 	mov	x1, #0x8170                	// #33136
 4026d84:	52800122 	mov	w2, #0x9                   	// #9
 4026d88:	f2a203c1 	movk	x1, #0x101e, lsl #16
 4026d8c:	b9400020 	ldr	w0, [x1]
 4026d90:	121c6c00 	and	w0, w0, #0xfffffff0
 4026d94:	2a020000 	orr	w0, w0, w2
 4026d98:	b9000020 	str	w0, [x1]
 4026d9c:	d2800040 	mov	x0, #0x2                   	// #2
 4026da0:	940007ca 	bl	4028cc8 <mdelay>
 4026da4:	940007b9 	bl	4028c88 <timer_deinit>
 4026da8:	d2800080 	mov	x0, #0x4                   	// #4
 4026dac:	52800021 	mov	w1, #0x1                   	// #1
 4026db0:	f2a22040 	movk	x0, #0x1102, lsl #16
 4026db4:	b9000001 	str	w1, [x0]
 4026db8:	b9000001 	str	w1, [x0]
 4026dbc:	b9000001 	str	w1, [x0]
 4026dc0:	f94077e0 	ldr	x0, [sp, #232]
 4026dc4:	f94772a1 	ldr	x1, [x21, #3808]
 4026dc8:	eb010000 	subs	x0, x0, x1
 4026dcc:	d2800001 	mov	x1, #0x0                   	// #0
 4026dd0:	54fff721 	b.ne	4026cb4 <main_entry+0xa24>  // b.any
 4026dd4:	d2a80060 	mov	x0, #0x40030000            	// #1073938432
 4026dd8:	17ffffb9 	b	4026cbc <main_entry+0xa2c>
 4026ddc:	d2802001 	mov	x1, #0x100                 	// #256
 4026de0:	d2a82060 	mov	x0, #0x41030000            	// #1090715648
 4026de4:	f2a82061 	movk	x1, #0x4103, lsl #16
 4026de8:	97fff53d 	bl	40242dc <handle_tee_info>
 4026dec:	7102941f 	cmp	w0, #0xa5
 4026df0:	54000060 	b.eq	4026dfc <main_entry+0xb6c>  // b.none
 4026df4:	52800e80 	mov	w0, #0x74                  	// #116
 4026df8:	17ffffe1 	b	4026d7c <main_entry+0xaec>
 4026dfc:	d2802001 	mov	x1, #0x100                 	// #256
 4026e00:	aa1303e0 	mov	x0, x19
 4026e04:	d2a80022 	mov	x2, #0x40010000            	// #1073807360
 4026e08:	f2a82061 	movk	x1, #0x4103, lsl #16
 4026e0c:	97fff5a5 	bl	40244a0 <handle_atf_code>
 4026e10:	7102941f 	cmp	w0, #0xa5
 4026e14:	54000060 	b.eq	4026e20 <main_entry+0xb90>  // b.none
 4026e18:	52800ea0 	mov	w0, #0x75                  	// #117
 4026e1c:	17ffffd8 	b	4026d7c <main_entry+0xaec>
 4026e20:	d29ffc82 	mov	x2, #0xffe4                	// #65508
 4026e24:	d2802001 	mov	x1, #0x100                 	// #256
 4026e28:	aa1303e0 	mov	x0, x19
 4026e2c:	f2a80042 	movk	x2, #0x4002, lsl #16
 4026e30:	f2a82061 	movk	x1, #0x4103, lsl #16
 4026e34:	97fff5f4 	bl	4024604 <handle_tee_code>
 4026e38:	7102941f 	cmp	w0, #0xa5
 4026e3c:	54fffc20 	b.eq	4026dc0 <main_entry+0xb30>  // b.none
 4026e40:	52800ec0 	mov	w0, #0x76                  	// #118
 4026e44:	17ffffce 	b	4026d7c <main_entry+0xaec>
 4026e48:	34ffc362 	cbz	w2, 40266b4 <main_entry+0x424>
 4026e4c:	52800001 	mov	w1, #0x0                   	// #0
 4026e50:	17ffff31 	b	4026b14 <main_entry+0x884>
 4026e54:	34ffe162 	cbz	w2, 4026a80 <main_entry+0x7f0>
 4026e58:	52800021 	mov	w1, #0x1                   	// #1
 4026e5c:	17ffff05 	b	4026a70 <main_entry+0x7e0>

0000000004026e60 <cal_crc_perbyte>:
 4026e60:	531c2c23 	ubfiz	w3, w1, #4, #12
 4026e64:	53041c04 	ubfx	w4, w0, #4, #4
 4026e68:	530c3c21 	ubfx	w1, w1, #12, #4
 4026e6c:	f0000042 	adrp	x2, 4031000 <sync_exception_sp_el0>
 4026e70:	4a040021 	eor	w1, w1, w4
 4026e74:	91194042 	add	x2, x2, #0x650
 4026e78:	12000c00 	and	w0, w0, #0xf
 4026e7c:	7861d841 	ldrh	w1, [x2, w1, sxtw #1]
 4026e80:	4a030021 	eor	w1, w1, w3
 4026e84:	531c2c23 	ubfiz	w3, w1, #4, #12
 4026e88:	4a413001 	eor	w1, w0, w1, lsr #12
 4026e8c:	7861d840 	ldrh	w0, [x2, w1, sxtw #1]
 4026e90:	4a030000 	eor	w0, w0, w3
 4026e94:	d65f03c0 	ret

0000000004026e98 <crc32>:
 4026e98:	f0000045 	adrp	x5, 4031000 <sync_exception_sp_el0>
 4026e9c:	9119c0a5 	add	x5, x5, #0x670
 4026ea0:	d2800004 	mov	x4, #0x0                   	// #0
 4026ea4:	12800003 	mov	w3, #0xffffffff            	// #-1
 4026ea8:	6b04003f 	cmp	w1, w4
 4026eac:	54000068 	b.hi	4026eb8 <crc32+0x20>  // b.pmore
 4026eb0:	2a2303e0 	mvn	w0, w3
 4026eb4:	d65f03c0 	ret
 4026eb8:	38646802 	ldrb	w2, [x0, x4]
 4026ebc:	91000484 	add	x4, x4, #0x1
 4026ec0:	4a030042 	eor	w2, w2, w3
 4026ec4:	92401c42 	and	x2, x2, #0xff
 4026ec8:	b86278a2 	ldr	w2, [x5, x2, lsl #2]
 4026ecc:	4a432043 	eor	w3, w2, w3, lsr #8
 4026ed0:	17fffff6 	b	4026ea8 <crc32+0x10>

0000000004026ed4 <malloc_init>:
 4026ed4:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4026ed8:	f0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4026edc:	913c4033 	add	x19, x1, #0xf10
 4026ee0:	2a0003f4 	mov	w20, w0
 4026ee4:	d2802403 	mov	x3, #0x120                 	// #288
 4026ee8:	aa1303e0 	mov	x0, x19
 4026eec:	aa0303e1 	mov	x1, x3
 4026ef0:	52800002 	mov	w2, #0x0                   	// #0
 4026ef4:	f9000bfe 	str	x30, [sp, #16]
 4026ef8:	94000794 	bl	4028d48 <memset_s>
 4026efc:	35000620 	cbnz	w0, 4026fc0 <malloc_init+0xec>
 4026f00:	f0000042 	adrp	x2, 4031000 <sync_exception_sp_el0>
 4026f04:	aa1303e1 	mov	x1, x19
 4026f08:	9129c042 	add	x2, x2, #0xa70
 4026f0c:	79400440 	ldrh	w0, [x2, #2]
 4026f10:	35000120 	cbnz	w0, 4026f34 <malloc_init+0x60>
 4026f14:	d2a04000 	mov	x0, #0x2000000             	// #33554432
 4026f18:	f9000020 	str	x0, [x1]
 4026f1c:	aa1303e1 	mov	x1, x19
 4026f20:	b9400820 	ldr	w0, [x1, #8]
 4026f24:	350001a0 	cbnz	w0, 4026f58 <malloc_init+0x84>
 4026f28:	f9400bfe 	ldr	x30, [sp, #16]
 4026f2c:	a8c253f3 	ldp	x19, x20, [sp], #32
 4026f30:	d65f03c0 	ret
 4026f34:	78404443 	ldrh	w3, [x2], #4
 4026f38:	91008021 	add	x1, x1, #0x20
 4026f3c:	b81e8020 	stur	w0, [x1, #-24]
 4026f40:	92403c64 	and	x4, x3, #0xffff
 4026f44:	f81e0024 	stur	x4, [x1, #-32]
 4026f48:	f81f0034 	stur	x20, [x1, #-16]
 4026f4c:	d503201f 	nop
 4026f50:	9ba05074 	umaddl	x20, w3, w0, x20
 4026f54:	17ffffee 	b	4026f0c <malloc_init+0x38>
 4026f58:	91008022 	add	x2, x1, #0x20
 4026f5c:	aa0203e3 	mov	x3, x2
 4026f60:	b9400864 	ldr	w4, [x3, #8]
 4026f64:	35000224 	cbnz	w4, 4026fa8 <malloc_init+0xd4>
 4026f68:	d1008043 	sub	x3, x2, #0x20
 4026f6c:	eb03003f 	cmp	x1, x3
 4026f70:	54000180 	b.eq	4026fa0 <malloc_init+0xcc>  // b.none
 4026f74:	f9400825 	ldr	x5, [x1, #16]
 4026f78:	f85f0044 	ldur	x4, [x2, #-16]
 4026f7c:	f81f0045 	stur	x5, [x2, #-16]
 4026f80:	b9400825 	ldr	w5, [x1, #8]
 4026f84:	b81e8045 	stur	w5, [x2, #-24]
 4026f88:	f9400025 	ldr	x5, [x1]
 4026f8c:	f85e0043 	ldur	x3, [x2, #-32]
 4026f90:	f81e0045 	stur	x5, [x2, #-32]
 4026f94:	f9000023 	str	x3, [x1]
 4026f98:	b9000820 	str	w0, [x1, #8]
 4026f9c:	f9000824 	str	x4, [x1, #16]
 4026fa0:	aa0203e1 	mov	x1, x2
 4026fa4:	17ffffdf 	b	4026f20 <malloc_init+0x4c>
 4026fa8:	f9400024 	ldr	x4, [x1]
 4026fac:	f9400065 	ldr	x5, [x3]
 4026fb0:	eb0400bf 	cmp	x5, x4
 4026fb4:	9a832021 	csel	x1, x1, x3, cs  // cs = hs, nlast
 4026fb8:	91008063 	add	x3, x3, #0x20
 4026fbc:	17ffffe9 	b	4026f60 <malloc_init+0x8c>
 4026fc0:	12800000 	mov	w0, #0xffffffff            	// #-1
 4026fc4:	17ffffd9 	b	4026f28 <malloc_init+0x54>

0000000004026fc8 <malloc>:
 4026fc8:	f0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4026fcc:	913c4021 	add	x1, x1, #0xf10
 4026fd0:	f9400022 	ldr	x2, [x1]
 4026fd4:	eb00005f 	cmp	x2, x0
 4026fd8:	54000163 	b.cc	4027004 <malloc+0x3c>  // b.lo, b.ul, b.last
 4026fdc:	f9400c25 	ldr	x5, [x1, #24]
 4026fe0:	52800003 	mov	w3, #0x0                   	// #0
 4026fe4:	b9400824 	ldr	w4, [x1, #8]
 4026fe8:	aa0503e0 	mov	x0, x5
 4026fec:	6b03009f 	cmp	w4, w3
 4026ff0:	540000e1 	b.ne	402700c <malloc+0x44>  // b.any
 4026ff4:	91008021 	add	x1, x1, #0x20
 4026ff8:	35ffff24 	cbnz	w4, 4026fdc <malloc+0x14>
 4026ffc:	d2800000 	mov	x0, #0x0                   	// #0
 4027000:	1400000d 	b	4027034 <malloc+0x6c>
 4027004:	91008021 	add	x1, x1, #0x20
 4027008:	17fffff2 	b	4026fd0 <malloc+0x8>
 402700c:	37000160 	tbnz	w0, #0, 4027038 <malloc+0x70>
 4027010:	f9400024 	ldr	x4, [x1]
 4027014:	2a0303e0 	mov	w0, w3
 4027018:	f9400822 	ldr	x2, [x1, #16]
 402701c:	9b040800 	madd	x0, x0, x4, x2
 4027020:	52800022 	mov	w2, #0x1                   	// #1
 4027024:	1ac32042 	lsl	w2, w2, w3
 4027028:	93407c42 	sxtw	x2, w2
 402702c:	aa050042 	orr	x2, x2, x5
 4027030:	f9000c22 	str	x2, [x1, #24]
 4027034:	d65f03c0 	ret
 4027038:	d341fc00 	lsr	x0, x0, #1
 402703c:	11000463 	add	w3, w3, #0x1
 4027040:	17ffffeb 	b	4026fec <malloc+0x24>

0000000004027044 <free>:
 4027044:	d0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4027048:	913c4021 	add	x1, x1, #0xf10
 402704c:	b9800822 	ldrsw	x2, [x1, #8]
 4027050:	f9400024 	ldr	x4, [x1]
 4027054:	f9400823 	ldr	x3, [x1, #16]
 4027058:	9b047c42 	mul	x2, x2, x4
 402705c:	cb030003 	sub	x3, x0, x3
 4027060:	eb02007f 	cmp	x3, x2
 4027064:	540001c2 	b.cs	402709c <free+0x58>  // b.hs, b.nlast
 4027068:	52800002 	mov	w2, #0x0                   	// #0
 402706c:	2a0203e0 	mov	w0, w2
 4027070:	9b047c00 	mul	x0, x0, x4
 4027074:	eb00007f 	cmp	x3, x0
 4027078:	54000168 	b.hi	40270a4 <free+0x60>  // b.pmore
 402707c:	52800020 	mov	w0, #0x1                   	// #1
 4027080:	1ac22000 	lsl	w0, w0, w2
 4027084:	2a2003e0 	mvn	w0, w0
 4027088:	f9400c22 	ldr	x2, [x1, #24]
 402708c:	93407c00 	sxtw	x0, w0
 4027090:	8a000040 	and	x0, x2, x0
 4027094:	f9000c20 	str	x0, [x1, #24]
 4027098:	d65f03c0 	ret
 402709c:	91008021 	add	x1, x1, #0x20
 40270a0:	17ffffeb 	b	402704c <free+0x8>
 40270a4:	11000442 	add	w2, w2, #0x1
 40270a8:	17fffff1 	b	402706c <free+0x28>

00000000040270ac <ot_strcmp>:
 40270ac:	d2800003 	mov	x3, #0x0                   	// #0
 40270b0:	38636804 	ldrb	w4, [x0, x3]
 40270b4:	38636822 	ldrb	w2, [x1, x3]
 40270b8:	6b020082 	subs	w2, w4, w2
 40270bc:	350000c2 	cbnz	w2, 40270d4 <ot_strcmp+0x28>
 40270c0:	91000463 	add	x3, x3, #0x1
 40270c4:	35ffff64 	cbnz	w4, 40270b0 <ot_strcmp+0x4>
 40270c8:	7100005f 	cmp	w2, #0x0
 40270cc:	1a9f07e0 	cset	w0, ne  // ne = any
 40270d0:	d65f03c0 	ret
 40270d4:	54ffffa5 	b.pl	40270c8 <ot_strcmp+0x1c>  // b.nfrst
 40270d8:	12800000 	mov	w0, #0xffffffff            	// #-1
 40270dc:	17fffffd 	b	40270d0 <ot_strcmp+0x24>

00000000040270e0 <timer_get_divider>:
 40270e0:	d2817700 	mov	x0, #0xbb8                 	// #3000
 40270e4:	d65f03c0 	ret

00000000040270e8 <do_unpack>:
 40270e8:	b5000060 	cbnz	x0, 40270f4 <do_unpack+0xc>
 40270ec:	52800b40 	mov	w0, #0x5a                  	// #90
 40270f0:	d65f03c0 	ret
 40270f4:	a9b853f3 	stp	x19, x20, [sp, #-128]!
 40270f8:	91005014 	add	x20, x0, #0x14
 40270fc:	a9015bf5 	stp	x21, x22, [sp, #16]
 4027100:	52800015 	mov	w21, #0x0                   	// #0
 4027104:	39400016 	ldrb	w22, [x0]
 4027108:	a90263f7 	stp	x23, x24, [sp, #32]
 402710c:	aa0103f7 	mov	x23, x1
 4027110:	39400401 	ldrb	w1, [x0, #1]
 4027114:	aa0303f8 	mov	x24, x3
 4027118:	a9036bf9 	stp	x25, x26, [sp, #48]
 402711c:	aa0403f9 	mov	x25, x4
 4027120:	aa0122c1 	orr	x1, x22, x1, lsl #8
 4027124:	39400816 	ldrb	w22, [x0, #2]
 4027128:	a90473fb 	stp	x27, x28, [sp, #64]
 402712c:	2a0203fc 	mov	w28, w2
 4027130:	5280001b 	mov	w27, #0x0                   	// #0
 4027134:	aa164021 	orr	x1, x1, x22, lsl #16
 4027138:	39400c16 	ldrb	w22, [x0, #3]
 402713c:	52890062 	mov	w2, #0x4803                	// #18435
 4027140:	f9002bfe 	str	x30, [sp, #80]
 4027144:	aa166036 	orr	x22, x1, x22, lsl #24
 4027148:	510052d6 	sub	w22, w22, #0x14
 402714c:	6b1602bf 	cmp	w21, w22
 4027150:	7a4232a2 	ccmp	w21, w2, #0x2, cc  // cc = lo, ul, last
 4027154:	54000129 	b.ls	4027178 <do_unpack+0x90>  // b.plast
 4027158:	52800b40 	mov	w0, #0x5a                  	// #90
 402715c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4027160:	a94263f7 	ldp	x23, x24, [sp, #32]
 4027164:	a9436bf9 	ldp	x25, x26, [sp, #48]
 4027168:	a94473fb 	ldp	x27, x28, [sp, #64]
 402716c:	f9402bfe 	ldr	x30, [sp, #80]
 4027170:	a8c853f3 	ldp	x19, x20, [sp], #128
 4027174:	d65f03c0 	ret
 4027178:	8b354293 	add	x19, x20, w21, uxtw
 402717c:	38754a80 	ldrb	w0, [x20, w21, uxtw]
 4027180:	2a1503e3 	mov	w3, w21
 4027184:	39400661 	ldrb	w1, [x19, #1]
 4027188:	aa012001 	orr	x1, x0, x1, lsl #8
 402718c:	39400a60 	ldrb	w0, [x19, #2]
 4027190:	aa004021 	orr	x1, x1, x0, lsl #16
 4027194:	39400e60 	ldrb	w0, [x19, #3]
 4027198:	aa006020 	orr	x0, x1, x0, lsl #24
 402719c:	f100041f 	cmp	x0, #0x1
 40271a0:	540002a1 	b.ne	40271f4 <do_unpack+0x10c>  // b.any
 40271a4:	39401260 	ldrb	w0, [x19, #4]
 40271a8:	39401661 	ldrb	w1, [x19, #5]
 40271ac:	aa012001 	orr	x1, x0, x1, lsl #8
 40271b0:	39401a60 	ldrb	w0, [x19, #6]
 40271b4:	aa004021 	orr	x1, x1, x0, lsl #16
 40271b8:	39401e60 	ldrb	w0, [x19, #7]
 40271bc:	aa006020 	orr	x0, x1, x0, lsl #24
 40271c0:	0b0002b5 	add	w21, w21, w0
 40271c4:	b40003d7 	cbz	x23, 402723c <do_unpack+0x154>
 40271c8:	91002264 	add	x4, x19, #0x8
 40271cc:	5280001a 	mov	w26, #0x0                   	// #0
 40271d0:	39401260 	ldrb	w0, [x19, #4]
 40271d4:	39401661 	ldrb	w1, [x19, #5]
 40271d8:	aa012001 	orr	x1, x0, x1, lsl #8
 40271dc:	39401a60 	ldrb	w0, [x19, #6]
 40271e0:	aa004021 	orr	x1, x1, x0, lsl #16
 40271e4:	39401e60 	ldrb	w0, [x19, #7]
 40271e8:	aa006020 	orr	x0, x1, x0, lsl #24
 40271ec:	6b00035f 	cmp	w26, w0
 40271f0:	540002a3 	b.cc	4027244 <do_unpack+0x15c>  // b.lo, b.ul, b.last
 40271f4:	38636a80 	ldrb	w0, [x20, x3]
 40271f8:	39400661 	ldrb	w1, [x19, #1]
 40271fc:	aa012001 	orr	x1, x0, x1, lsl #8
 4027200:	39400a60 	ldrb	w0, [x19, #2]
 4027204:	aa004021 	orr	x1, x1, x0, lsl #16
 4027208:	39400e60 	ldrb	w0, [x19, #3]
 402720c:	aa006020 	orr	x0, x1, x0, lsl #24
 4027210:	f100081f 	cmp	x0, #0x2
 4027214:	54000141 	b.ne	402723c <do_unpack+0x154>  // b.any
 4027218:	b50003b9 	cbnz	x25, 402728c <do_unpack+0x1a4>
 402721c:	39401260 	ldrb	w0, [x19, #4]
 4027220:	39401661 	ldrb	w1, [x19, #5]
 4027224:	aa012001 	orr	x1, x0, x1, lsl #8
 4027228:	39401a60 	ldrb	w0, [x19, #6]
 402722c:	aa004021 	orr	x1, x1, x0, lsl #16
 4027230:	39401e60 	ldrb	w0, [x19, #7]
 4027234:	aa006020 	orr	x0, x1, x0, lsl #24
 4027238:	0b0002b5 	add	w21, w21, w0
 402723c:	110022b5 	add	w21, w21, #0x8
 4027240:	17ffffc3 	b	402714c <do_unpack+0x64>
 4027244:	8b3a4081 	add	x1, x4, w26, uxtw
 4027248:	6b1c037f 	cmp	w27, w28
 402724c:	54000081 	b.ne	402725c <do_unpack+0x174>  // b.any
 4027250:	f90002e1 	str	x1, [x23]
 4027254:	528014a0 	mov	w0, #0xa5                  	// #165
 4027258:	17ffffc1 	b	402715c <do_unpack+0x74>
 402725c:	b4000138 	cbz	x24, 4027280 <do_unpack+0x198>
 4027260:	aa1803e0 	mov	x0, x24
 4027264:	a9068fe1 	stp	x1, x3, [sp, #104]
 4027268:	f9003fe4 	str	x4, [sp, #120]
 402726c:	97ffff90 	bl	40270ac <ot_strcmp>
 4027270:	52890062 	mov	w2, #0x4803                	// #18435
 4027274:	a9468fe1 	ldp	x1, x3, [sp, #104]
 4027278:	f9403fe4 	ldr	x4, [sp, #120]
 402727c:	34fffea0 	cbz	w0, 4027250 <do_unpack+0x168>
 4027280:	1100077b 	add	w27, w27, #0x1
 4027284:	1102035a 	add	w26, w26, #0x80
 4027288:	17ffffd2 	b	40271d0 <do_unpack+0xe8>
 402728c:	91002273 	add	x19, x19, #0x8
 4027290:	f9000333 	str	x19, [x25]
 4027294:	17fffff0 	b	4027254 <do_unpack+0x16c>

0000000004027298 <ot_partition_get_partition_info_by_name>:
 4027298:	f100001f 	cmp	x0, #0x0
 402729c:	aa0203e3 	mov	x3, x2
 40272a0:	fa401824 	ccmp	x1, #0x0, #0x4, ne  // ne = any
 40272a4:	54000080 	b.eq	40272b4 <ot_partition_get_partition_info_by_name+0x1c>  // b.none
 40272a8:	d2800004 	mov	x4, #0x0                   	// #0
 40272ac:	12800002 	mov	w2, #0xffffffff            	// #-1
 40272b0:	17ffff8e 	b	40270e8 <do_unpack>
 40272b4:	52800b40 	mov	w0, #0x5a                  	// #90
 40272b8:	d65f03c0 	ret

00000000040272bc <ot_partition_get_partition_info_by_index>:
 40272bc:	f100001f 	cmp	x0, #0x0
 40272c0:	fa401824 	ccmp	x1, #0x0, #0x4, ne  // ne = any
 40272c4:	54000080 	b.eq	40272d4 <ot_partition_get_partition_info_by_index+0x18>  // b.none
 40272c8:	d2800004 	mov	x4, #0x0                   	// #0
 40272cc:	d2800003 	mov	x3, #0x0                   	// #0
 40272d0:	17ffff86 	b	40270e8 <do_unpack>
 40272d4:	52800b40 	mov	w0, #0x5a                  	// #90
 40272d8:	d65f03c0 	ret

00000000040272dc <ot_partition_get_env_info>:
 40272dc:	f100001f 	cmp	x0, #0x0
 40272e0:	aa0103e4 	mov	x4, x1
 40272e4:	fa401824 	ccmp	x1, #0x0, #0x4, ne  // ne = any
 40272e8:	540000a0 	b.eq	40272fc <ot_partition_get_env_info+0x20>  // b.none
 40272ec:	d2800003 	mov	x3, #0x0                   	// #0
 40272f0:	52800002 	mov	w2, #0x0                   	// #0
 40272f4:	d2800001 	mov	x1, #0x0                   	// #0
 40272f8:	17ffff7c 	b	40270e8 <do_unpack>
 40272fc:	52800b40 	mov	w0, #0x5a                  	// #90
 4027300:	d65f03c0 	ret

0000000004027304 <is_ree_verify_enable>:
 4027304:	d2800280 	mov	x0, #0x14                  	// #20
 4027308:	f2a203c0 	movk	x0, #0x101e, lsl #16
 402730c:	b9400001 	ldr	w1, [x0]
 4027310:	528d34a0 	mov	w0, #0x69a5                	// #27045
 4027314:	72b86b40 	movk	w0, #0xc35a, lsl #16
 4027318:	d3505c21 	ubfx	x1, x1, #16, #8
 402731c:	7101083f 	cmp	w1, #0x42
 4027320:	5a800000 	cinv	w0, w0, ne  // ne = any
 4027324:	d65f03c0 	ret

0000000004027328 <is_tp_verify_enable>:
 4027328:	d2800280 	mov	x0, #0x14                  	// #20
 402732c:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4027330:	b9400001 	ldr	w1, [x0]
 4027334:	528d34a0 	mov	w0, #0x69a5                	// #27045
 4027338:	72b86b40 	movk	w0, #0xc35a, lsl #16
 402733c:	d3483c21 	ubfx	x1, x1, #8, #8
 4027340:	7101083f 	cmp	w1, #0x42
 4027344:	5a800000 	cinv	w0, w0, ne  // ne = any
 4027348:	d65f03c0 	ret

000000000402734c <is_tee_verify_enable>:
 402734c:	d2800280 	mov	x0, #0x14                  	// #20
 4027350:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4027354:	b9400001 	ldr	w1, [x0]
 4027358:	528d34a0 	mov	w0, #0x69a5                	// #27045
 402735c:	72b86b40 	movk	w0, #0xc35a, lsl #16
 4027360:	53187c21 	lsr	w1, w1, #24
 4027364:	7101083f 	cmp	w1, #0x42
 4027368:	5a800000 	cinv	w0, w0, ne  // ne = any
 402736c:	d65f03c0 	ret

0000000004027370 <is_soc_tee_enable>:
 4027370:	d2800200 	mov	x0, #0x10                  	// #16
 4027374:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4027378:	b9400001 	ldr	w1, [x0]
 402737c:	528d34a0 	mov	w0, #0x69a5                	// #27045
 4027380:	72b86b40 	movk	w0, #0xc35a, lsl #16
 4027384:	d3505c21 	ubfx	x1, x1, #16, #8
 4027388:	7101083f 	cmp	w1, #0x42
 402738c:	5a800000 	cinv	w0, w0, ne  // ne = any
 4027390:	d65f03c0 	ret

0000000004027394 <is_sec_dbg_enable>:
 4027394:	d2803c00 	mov	x0, #0x1e0                 	// #480
 4027398:	f2a203c0 	movk	x0, #0x101e, lsl #16
 402739c:	b9400001 	ldr	w1, [x0]
 40273a0:	5292cb40 	mov	w0, #0x965a                	// #38490
 40273a4:	72a794a0 	movk	w0, #0x3ca5, lsl #16
 40273a8:	d3505c21 	ubfx	x1, x1, #16, #8
 40273ac:	7101083f 	cmp	w1, #0x42
 40273b0:	5a800000 	cinv	w0, w0, ne  // ne = any
 40273b4:	d65f03c0 	ret

00000000040273b8 <is_sec_dbg_lv_enable>:
 40273b8:	d2803c00 	mov	x0, #0x1e0                 	// #480
 40273bc:	f2a203c0 	movk	x0, #0x101e, lsl #16
 40273c0:	b9400001 	ldr	w1, [x0]
 40273c4:	5292cb40 	mov	w0, #0x965a                	// #38490
 40273c8:	72a794a0 	movk	w0, #0x3ca5, lsl #16
 40273cc:	7218043f 	tst	w1, #0x300
 40273d0:	5a800000 	cinv	w0, w0, ne  // ne = any
 40273d4:	d65f03c0 	ret

00000000040273d8 <is_tee_dec_en_enable>:
 40273d8:	d2800080 	mov	x0, #0x4                   	// #4
 40273dc:	f2a203c0 	movk	x0, #0x101e, lsl #16
 40273e0:	b9400001 	ldr	w1, [x0]
 40273e4:	5292cb40 	mov	w0, #0x965a                	// #38490
 40273e8:	72a794a0 	movk	w0, #0x3ca5, lsl #16
 40273ec:	72100c3f 	tst	w1, #0xf0000
 40273f0:	5a801000 	cinv	w0, w0, eq  // eq = none
 40273f4:	d65f03c0 	ret

00000000040273f8 <is_ree_boot_dec_en_enable>:
 40273f8:	d2800080 	mov	x0, #0x4                   	// #4
 40273fc:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4027400:	b9400001 	ldr	w1, [x0]
 4027404:	5292cb40 	mov	w0, #0x965a                	// #38490
 4027408:	72a794a0 	movk	w0, #0x3ca5, lsl #16
 402740c:	720c0c3f 	tst	w1, #0xf00000
 4027410:	5a801000 	cinv	w0, w0, eq  // eq = none
 4027414:	d65f03c0 	ret

0000000004027418 <opt_get_boot_backup_enable>:
 4027418:	d2a203c0 	mov	x0, #0x101e0000            	// #270401536
 402741c:	b9400000 	ldr	w0, [x0]
 4027420:	d3471c00 	ubfx	x0, x0, #7, #1
 4027424:	d65f03c0 	ret

0000000004027428 <check_volt_val>:
 4027428:	b9400003 	ldr	w3, [x0]
 402742c:	6b01007f 	cmp	w3, w1
 4027430:	54000069 	b.ls	402743c <check_volt_val+0x14>  // b.plast
 4027434:	b9000001 	str	w1, [x0]
 4027438:	d65f03c0 	ret
 402743c:	6b02007f 	cmp	w3, w2
 4027440:	54ffffc2 	b.cs	4027438 <check_volt_val+0x10>  // b.hs, b.nlast
 4027444:	b9000002 	str	w2, [x0]
 4027448:	17fffffc 	b	4027438 <check_volt_val+0x10>

000000000402744c <svb_pwm_cfg>:
 402744c:	2a0103e1 	mov	w1, w1
 4027450:	53102400 	ubfiz	w0, w0, #16, #10
 4027454:	52833ea2 	mov	w2, #0x19f5                	// #6645
 4027458:	2a020000 	orr	w0, w0, w2
 402745c:	b9000020 	str	w0, [x1]
 4027460:	d65f03c0 	ret

0000000004027464 <svb_error>:
 4027464:	f81f0ffe 	str	x30, [sp, #-16]!
 4027468:	d0000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402746c:	9134b400 	add	x0, x0, #0xd2d
 4027470:	940005fa 	bl	4028c58 <log_serial_puts>
 4027474:	d2800040 	mov	x0, #0x2                   	// #2
 4027478:	94000614 	bl	4028cc8 <mdelay>
 402747c:	94000603 	bl	4028c88 <timer_deinit>
 4027480:	d2800080 	mov	x0, #0x4                   	// #4
 4027484:	52800021 	mov	w1, #0x1                   	// #1
 4027488:	f2a22040 	movk	x0, #0x1102, lsl #16
 402748c:	f84107fe 	ldr	x30, [sp], #16
 4027490:	b9000001 	str	w1, [x0]
 4027494:	b9000001 	str	w1, [x0]
 4027498:	b9000001 	str	w1, [x0]
 402749c:	d65f03c0 	ret

00000000040274a0 <hpm_value_avg.constprop.0>:
 40274a0:	d10043ff 	sub	sp, sp, #0x10
 40274a4:	d2800001 	mov	x1, #0x0                   	// #0
 40274a8:	b9000fff 	str	wzr, [sp, #12]
 40274ac:	b8617803 	ldr	w3, [x0, x1, lsl #2]
 40274b0:	91000421 	add	x1, x1, #0x1
 40274b4:	b9400fe2 	ldr	w2, [sp, #12]
 40274b8:	0b030042 	add	w2, w2, w3
 40274bc:	b9000fe2 	str	w2, [sp, #12]
 40274c0:	f100103f 	cmp	x1, #0x4
 40274c4:	54ffff41 	b.ne	40274ac <hpm_value_avg.constprop.0+0xc>  // b.any
 40274c8:	b9400fe0 	ldr	w0, [sp, #12]
 40274cc:	910043ff 	add	sp, sp, #0x10
 40274d0:	53057c00 	lsr	w0, w0, #5
 40274d4:	d65f03c0 	ret

00000000040274d8 <calc_volt_regval.constprop.0>:
 40274d8:	d10043ff 	sub	sp, sp, #0x10
 40274dc:	6b01001f 	cmp	w0, w1
 40274e0:	540001c3 	b.cc	4027518 <calc_volt_regval.constprop.0+0x40>  // b.lo, b.ul, b.last
 40274e4:	51000420 	sub	w0, w1, #0x1
 40274e8:	510a2022 	sub	w2, w1, #0x288
 40274ec:	4b000020 	sub	w0, w1, w0
 40274f0:	52803403 	mov	w3, #0x1a0                 	// #416
 40274f4:	510a2421 	sub	w1, w1, #0x289
 40274f8:	53017c42 	lsr	w2, w2, #1
 40274fc:	1b030800 	madd	w0, w0, w3, w2
 4027500:	1ac10800 	udiv	w0, w0, w1
 4027504:	51000400 	sub	w0, w0, #0x1
 4027508:	b9000fe0 	str	w0, [sp, #12]
 402750c:	b9400fe0 	ldr	w0, [sp, #12]
 4027510:	910043ff 	add	sp, sp, #0x10
 4027514:	d65f03c0 	ret
 4027518:	710a281f 	cmp	w0, #0x28a
 402751c:	52805142 	mov	w2, #0x28a                 	// #650
 4027520:	1a822000 	csel	w0, w0, w2, cs  // cs = hs, nlast
 4027524:	17fffff1 	b	40274e8 <calc_volt_regval.constprop.0+0x10>

0000000004027528 <set_volt>:
 4027528:	d100c3ff 	sub	sp, sp, #0x30
 402752c:	2a0103e7 	mov	w7, w1
 4027530:	d0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4027534:	2a0203e6 	mov	w6, w2
 4027538:	aa0303e4 	mov	x4, x3
 402753c:	52800023 	mov	w3, #0x1                   	// #1
 4027540:	f90013fe 	str	x30, [sp, #32]
 4027544:	5290d408 	mov	w8, #0x86a0                	// #34464
 4027548:	72a00028 	movk	w8, #0x1, lsl #16
 402754c:	f9477022 	ldr	x2, [x1, #3808]
 4027550:	f9000fe2 	str	x2, [sp, #24]
 4027554:	d2800002 	mov	x2, #0x0                   	// #0
 4027558:	d2802181 	mov	x1, #0x10c                 	// #268
 402755c:	f2a203c1 	movk	x1, #0x101e, lsl #16
 4027560:	b9400021 	ldr	w1, [x1]
 4027564:	b9000be1 	str	w1, [sp, #8]
 4027568:	b9400be1 	ldr	w1, [sp, #8]
 402756c:	b9400be2 	ldr	w2, [sp, #8]
 4027570:	93401821 	sbfx	x1, x1, #0, #7
 4027574:	f279005f 	tst	x2, #0x80
 4027578:	5a83046a 	cneg	w10, w3, ne  // ne = any
 402757c:	1b017d4a 	mul	w10, w10, w1
 4027580:	d2802481 	mov	x1, #0x124                 	// #292
 4027584:	f2a203c1 	movk	x1, #0x101e, lsl #16
 4027588:	b9400021 	ldr	w1, [x1]
 402758c:	b90013e1 	str	w1, [sp, #16]
 4027590:	b94013e1 	ldr	w1, [sp, #16]
 4027594:	b94013e2 	ldr	w2, [sp, #16]
 4027598:	b94013e5 	ldr	w5, [sp, #16]
 402759c:	93505821 	sbfx	x1, x1, #16, #7
 40275a0:	f269005f 	tst	x2, #0x800000
 40275a4:	b9400082 	ldr	w2, [x4]
 40275a8:	5a830469 	cneg	w9, w3, ne  // ne = any
 40275ac:	935878a5 	sbfx	x5, x5, #24, #7
 40275b0:	1b017d29 	mul	w9, w9, w1
 40275b4:	b94013e1 	ldr	w1, [sp, #16]
 40275b8:	7100003f 	cmp	w1, #0x0
 40275bc:	b9400481 	ldr	w1, [x4, #4]
 40275c0:	5a83a463 	cneg	w3, w3, lt  // lt = tstop
 40275c4:	7104341f 	cmp	w0, #0x10d
 40275c8:	1b028402 	msub	w2, w0, w2, w1
 40275cc:	b9400c81 	ldr	w1, [x4, #12]
 40275d0:	1b057c65 	mul	w5, w3, w5
 40275d4:	1ac80842 	udiv	w2, w2, w8
 40275d8:	1a818040 	csel	w0, w2, w1, hi  // hi = pmore
 40275dc:	b9400882 	ldr	w2, [x4, #8]
 40275e0:	b90017e0 	str	w0, [sp, #20]
 40275e4:	910053e0 	add	x0, sp, #0x14
 40275e8:	97ffff90 	bl	4027428 <check_volt_val>
 40275ec:	b94017e1 	ldr	w1, [sp, #20]
 40275f0:	52806722 	mov	w2, #0x339                 	// #825
 40275f4:	0b0a0021 	add	w1, w1, w10
 40275f8:	b90017e1 	str	w1, [sp, #20]
 40275fc:	52807e41 	mov	w1, #0x3f2                 	// #1010
 4027600:	97ffff8a 	bl	4027428 <check_volt_val>
 4027604:	79402be1 	ldrh	w1, [sp, #20]
 4027608:	52800000 	mov	w0, #0x0                   	// #0
 402760c:	33003c20 	bfxil	w0, w1, #0, #16
 4027610:	33103c20 	bfi	w0, w1, #16, #16
 4027614:	d2806881 	mov	x1, #0x344                 	// #836
 4027618:	f2a22041 	movk	x1, #0x1102, lsl #16
 402761c:	b9000020 	str	w0, [x1]
 4027620:	528069a1 	mov	w1, #0x34d                 	// #845
 4027624:	b94017e0 	ldr	w0, [sp, #20]
 4027628:	5100a000 	sub	w0, w0, #0x28
 402762c:	710d301f 	cmp	w0, #0x34c
 4027630:	1a818000 	csel	w0, w0, w1, hi  // hi = pmore
 4027634:	52808361 	mov	w1, #0x41b                 	// #1051
 4027638:	b90017e0 	str	w0, [sp, #20]
 402763c:	97ffffa7 	bl	40274d8 <calc_volt_regval.constprop.0>
 4027640:	52920001 	mov	w1, #0x9000                	// #36864
 4027644:	72a22041 	movk	w1, #0x1102, lsl #16
 4027648:	97ffff81 	bl	402744c <svb_pwm_cfg>
 402764c:	29428881 	ldp	w1, w2, [x4, #20]
 4027650:	910053e0 	add	x0, sp, #0x14
 4027654:	1b0188e7 	msub	w7, w7, w1, w2
 4027658:	29438482 	ldp	w2, w1, [x4, #28]
 402765c:	1ac808e7 	udiv	w7, w7, w8
 4027660:	b90017e7 	str	w7, [sp, #20]
 4027664:	97ffff71 	bl	4027428 <check_volt_val>
 4027668:	b94017e1 	ldr	w1, [sp, #20]
 402766c:	52806542 	mov	w2, #0x32a                 	// #810
 4027670:	0b090021 	add	w1, w1, w9
 4027674:	b90017e1 	str	w1, [sp, #20]
 4027678:	52808321 	mov	w1, #0x419                 	// #1049
 402767c:	97ffff6b 	bl	4027428 <check_volt_val>
 4027680:	79402be2 	ldrh	w2, [sp, #20]
 4027684:	52800000 	mov	w0, #0x0                   	// #0
 4027688:	33003c40 	bfxil	w0, w2, #0, #16
 402768c:	33103c40 	bfi	w0, w2, #16, #16
 4027690:	d2806902 	mov	x2, #0x348                 	// #840
 4027694:	f2a22042 	movk	x2, #0x1102, lsl #16
 4027698:	b9000040 	str	w0, [x2]
 402769c:	b94017e0 	ldr	w0, [sp, #20]
 40276a0:	97ffff8e 	bl	40274d8 <calc_volt_regval.constprop.0>
 40276a4:	52920081 	mov	w1, #0x9004                	// #36868
 40276a8:	72a22041 	movk	w1, #0x1102, lsl #16
 40276ac:	97ffff68 	bl	402744c <svb_pwm_cfg>
 40276b0:	b9403881 	ldr	w1, [x4, #56]
 40276b4:	35000461 	cbnz	w1, 4027740 <set_volt+0x218>
 40276b8:	29450482 	ldp	w2, w1, [x4, #40]
 40276bc:	910053e0 	add	x0, sp, #0x14
 40276c0:	1b0284c6 	msub	w6, w6, w2, w1
 40276c4:	29460482 	ldp	w2, w1, [x4, #48]
 40276c8:	1ac808c6 	udiv	w6, w6, w8
 40276cc:	b90017e6 	str	w6, [sp, #20]
 40276d0:	97ffff56 	bl	4027428 <check_volt_val>
 40276d4:	b94017e3 	ldr	w3, [sp, #20]
 40276d8:	52808321 	mov	w1, #0x419                 	// #1049
 40276dc:	52806542 	mov	w2, #0x32a                 	// #810
 40276e0:	910053e0 	add	x0, sp, #0x14
 40276e4:	0b050065 	add	w5, w3, w5
 40276e8:	b90017e5 	str	w5, [sp, #20]
 40276ec:	97ffff4f 	bl	4027428 <check_volt_val>
 40276f0:	79402be1 	ldrh	w1, [sp, #20]
 40276f4:	52800000 	mov	w0, #0x0                   	// #0
 40276f8:	33003c20 	bfxil	w0, w1, #0, #16
 40276fc:	33103c20 	bfi	w0, w1, #16, #16
 4027700:	d2806981 	mov	x1, #0x34c                 	// #844
 4027704:	f2a22041 	movk	x1, #0x1102, lsl #16
 4027708:	b9000020 	str	w0, [x1]
 402770c:	52808361 	mov	w1, #0x41b                 	// #1051
 4027710:	b94017e0 	ldr	w0, [sp, #20]
 4027714:	97ffff71 	bl	40274d8 <calc_volt_regval.constprop.0>
 4027718:	52920101 	mov	w1, #0x9008                	// #36872
 402771c:	72a22041 	movk	w1, #0x1102, lsl #16
 4027720:	97ffff4b 	bl	402744c <svb_pwm_cfg>
 4027724:	d0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4027728:	f9400fe1 	ldr	x1, [sp, #24]
 402772c:	f9477002 	ldr	x2, [x0, #3808]
 4027730:	eb020021 	subs	x1, x1, x2
 4027734:	d2800002 	mov	x2, #0x0                   	// #0
 4027738:	54000080 	b.eq	4027748 <set_volt+0x220>  // b.none
 402773c:	97fffab0 	bl	40261fc <__stack_chk_fail>
 4027740:	b90017e1 	str	w1, [sp, #20]
 4027744:	17ffffe4 	b	40276d4 <set_volt+0x1ac>
 4027748:	f94013fe 	ldr	x30, [sp, #32]
 402774c:	d2800280 	mov	x0, #0x14                  	// #20
 4027750:	9100c3ff 	add	sp, sp, #0x30
 4027754:	14000559 	b	4028cb8 <udelay>

0000000004027758 <start_svb>:
 4027758:	d10283ff 	sub	sp, sp, #0xa0
 402775c:	d0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4027760:	52800062 	mov	w2, #0x3                   	// #3
 4027764:	d2963203 	mov	x3, #0xb190                	// #45456
 4027768:	72ac0102 	movk	w2, #0x6008, lsl #16
 402776c:	f2a22043 	movk	x3, #0x1102, lsl #16
 4027770:	a90553f3 	stp	x19, x20, [sp, #80]
 4027774:	a9065bf5 	stp	x21, x22, [sp, #96]
 4027778:	a90763f7 	stp	x23, x24, [sp, #112]
 402777c:	a9086bf9 	stp	x25, x26, [sp, #128]
 4027780:	f9004bfe 	str	x30, [sp, #144]
 4027784:	f9477001 	ldr	x1, [x0, #3808]
 4027788:	f90027e1 	str	x1, [sp, #72]
 402778c:	d2800001 	mov	x1, #0x0                   	// #0
 4027790:	d2802d01 	mov	x1, #0x168                 	// #360
 4027794:	f2a22041 	movk	x1, #0x1102, lsl #16
 4027798:	b9400020 	ldr	w0, [x1]
 402779c:	72a02000 	movk	w0, #0x100, lsl #16
 40277a0:	b9000020 	str	w0, [x1]
 40277a4:	d2940000 	mov	x0, #0xa000                	// #40960
 40277a8:	529f8001 	mov	w1, #0xfc00                	// #64512
 40277ac:	f2a22040 	movk	x0, #0x1102, lsl #16
 40277b0:	72b811e1 	movk	w1, #0xc08f, lsl #16
 40277b4:	b9000001 	str	w1, [x0]
 40277b8:	d2892f81 	mov	x1, #0x497c                	// #18812
 40277bc:	f2a22021 	movk	x1, #0x1101, lsl #16
 40277c0:	b900041f 	str	wzr, [x0, #4]
 40277c4:	52800200 	mov	w0, #0x10                  	// #16
 40277c8:	b9000020 	str	w0, [x1]
 40277cc:	b9000420 	str	w0, [x1, #4]
 40277d0:	b9000820 	str	w0, [x1, #8]
 40277d4:	d2962000 	mov	x0, #0xb100                	// #45312
 40277d8:	f2a22040 	movk	x0, #0x1102, lsl #16
 40277dc:	52807fe1 	mov	w1, #0x3ff                 	// #1023
 40277e0:	b9000002 	str	w2, [x0]
 40277e4:	b9000401 	str	w1, [x0, #4]
 40277e8:	b8100002 	stur	w2, [x0, #-256]
 40277ec:	b8104001 	stur	w1, [x0, #-252]
 40277f0:	91004000 	add	x0, x0, #0x10
 40277f4:	b902f002 	str	w2, [x0, #752]
 40277f8:	b902f401 	str	w1, [x0, #756]
 40277fc:	eb03001f 	cmp	x0, x3
 4027800:	54ffff01 	b.ne	40277e0 <start_svb+0x88>  // b.any
 4027804:	52808361 	mov	w1, #0x41b                 	// #1051
 4027808:	52807bc0 	mov	w0, #0x3de                 	// #990
 402780c:	97ffff33 	bl	40274d8 <calc_volt_regval.constprop.0>
 4027810:	52802239 	mov	w25, #0x111                 	// #273
 4027814:	52920001 	mov	w1, #0x9000                	// #36864
 4027818:	d288e093 	mov	x19, #0x4704                	// #18180
 402781c:	72a22041 	movk	w1, #0x1102, lsl #16
 4027820:	97ffff0b 	bl	402744c <svb_pwm_cfg>
 4027824:	52808321 	mov	w1, #0x419                 	// #1049
 4027828:	52807bc0 	mov	w0, #0x3de                 	// #990
 402782c:	97ffff2b 	bl	40274d8 <calc_volt_regval.constprop.0>
 4027830:	f2a22053 	movk	x19, #0x1102, lsl #16
 4027834:	52920081 	mov	w1, #0x9004                	// #36868
 4027838:	52802218 	mov	w24, #0x110                 	// #272
 402783c:	72a22041 	movk	w1, #0x1102, lsl #16
 4027840:	97ffff03 	bl	402744c <svb_pwm_cfg>
 4027844:	52808361 	mov	w1, #0x41b                 	// #1051
 4027848:	52807bc0 	mov	w0, #0x3de                 	// #990
 402784c:	97ffff23 	bl	40274d8 <calc_volt_regval.constprop.0>
 4027850:	52802357 	mov	w23, #0x11a                 	// #282
 4027854:	52920101 	mov	w1, #0x9008                	// #36872
 4027858:	52806636 	mov	w22, #0x331                 	// #817
 402785c:	72a22041 	movk	w1, #0x1102, lsl #16
 4027860:	97fffefb 	bl	402744c <svb_pwm_cfg>
 4027864:	d2893200 	mov	x0, #0x4990                	// #18832
 4027868:	52802755 	mov	w21, #0x13a                 	// #314
 402786c:	f2a22020 	movk	x0, #0x1101, lsl #16
 4027870:	72a00035 	movk	w21, #0x1, lsl #16
 4027874:	52802754 	mov	w20, #0x13a                 	// #314
 4027878:	b9000019 	str	w25, [x0]
 402787c:	72a02034 	movk	w20, #0x101, lsl #16
 4027880:	b9000018 	str	w24, [x0]
 4027884:	d288e100 	mov	x0, #0x4708                	// #18184
 4027888:	f2a22040 	movk	x0, #0x1102, lsl #16
 402788c:	b9000277 	str	w23, [x19]
 4027890:	b9000016 	str	w22, [x0]
 4027894:	d2800140 	mov	x0, #0xa                   	// #10
 4027898:	b9000275 	str	w21, [x19]
 402789c:	94000507 	bl	4028cb8 <udelay>
 40278a0:	b9000274 	str	w20, [x19]
 40278a4:	d2800140 	mov	x0, #0xa                   	// #10
 40278a8:	94000504 	bl	4028cb8 <udelay>
 40278ac:	d2893280 	mov	x0, #0x4994                	// #18836
 40278b0:	f2a22020 	movk	x0, #0x1101, lsl #16
 40278b4:	b9000019 	str	w25, [x0]
 40278b8:	b9000018 	str	w24, [x0]
 40278bc:	d288e300 	mov	x0, #0x4718                	// #18200
 40278c0:	f2a22040 	movk	x0, #0x1102, lsl #16
 40278c4:	b9001277 	str	w23, [x19, #16]
 40278c8:	b9000016 	str	w22, [x0]
 40278cc:	d2800140 	mov	x0, #0xa                   	// #10
 40278d0:	b9001275 	str	w21, [x19, #16]
 40278d4:	940004f9 	bl	4028cb8 <udelay>
 40278d8:	b9001274 	str	w20, [x19, #16]
 40278dc:	d2800140 	mov	x0, #0xa                   	// #10
 40278e0:	940004fa 	bl	4028cc8 <mdelay>
 40278e4:	29037fff 	stp	wzr, wzr, [sp, #24]
 40278e8:	d2962508 	mov	x8, #0xb128                	// #45352
 40278ec:	d2962587 	mov	x7, #0xb12c                	// #45356
 40278f0:	d2960906 	mov	x6, #0xb048                	// #45128
 40278f4:	d2960985 	mov	x5, #0xb04c                	// #45132
 40278f8:	d2968b04 	mov	x4, #0xb458                	// #46168
 40278fc:	d2968b83 	mov	x3, #0xb45c                	// #46172
 4027900:	52800101 	mov	w1, #0x8                   	// #8
 4027904:	f2a22048 	movk	x8, #0x1102, lsl #16
 4027908:	f2a22047 	movk	x7, #0x1102, lsl #16
 402790c:	f2a22046 	movk	x6, #0x1102, lsl #16
 4027910:	f2a22045 	movk	x5, #0x1102, lsl #16
 4027914:	f2a22044 	movk	x4, #0x1102, lsl #16
 4027918:	f2a22043 	movk	x3, #0x1102, lsl #16
 402791c:	29047fff 	stp	wzr, wzr, [sp, #32]
 4027920:	29057fff 	stp	wzr, wzr, [sp, #40]
 4027924:	29067fff 	stp	wzr, wzr, [sp, #48]
 4027928:	29077fff 	stp	wzr, wzr, [sp, #56]
 402792c:	29087fff 	stp	wzr, wzr, [sp, #64]
 4027930:	b9400100 	ldr	w0, [x8]
 4027934:	71000421 	subs	w1, w1, #0x1
 4027938:	b9401fe2 	ldr	w2, [sp, #28]
 402793c:	d3506409 	ubfx	x9, x0, #16, #10
 4027940:	12002400 	and	w0, w0, #0x3ff
 4027944:	0b090042 	add	w2, w2, w9
 4027948:	b9001fe2 	str	w2, [sp, #28]
 402794c:	b9401be2 	ldr	w2, [sp, #24]
 4027950:	0b000040 	add	w0, w2, w0
 4027954:	b9001be0 	str	w0, [sp, #24]
 4027958:	b94027e2 	ldr	w2, [sp, #36]
 402795c:	b94000e0 	ldr	w0, [x7]
 4027960:	d3506409 	ubfx	x9, x0, #16, #10
 4027964:	12002400 	and	w0, w0, #0x3ff
 4027968:	0b090042 	add	w2, w2, w9
 402796c:	b90027e2 	str	w2, [sp, #36]
 4027970:	b94023e2 	ldr	w2, [sp, #32]
 4027974:	0b000040 	add	w0, w2, w0
 4027978:	b90023e0 	str	w0, [sp, #32]
 402797c:	b9402fe2 	ldr	w2, [sp, #44]
 4027980:	b94000c0 	ldr	w0, [x6]
 4027984:	d3506409 	ubfx	x9, x0, #16, #10
 4027988:	12002400 	and	w0, w0, #0x3ff
 402798c:	0b090042 	add	w2, w2, w9
 4027990:	b9002fe2 	str	w2, [sp, #44]
 4027994:	b9402be2 	ldr	w2, [sp, #40]
 4027998:	0b000040 	add	w0, w2, w0
 402799c:	b9002be0 	str	w0, [sp, #40]
 40279a0:	b94037e2 	ldr	w2, [sp, #52]
 40279a4:	b94000a0 	ldr	w0, [x5]
 40279a8:	d3506409 	ubfx	x9, x0, #16, #10
 40279ac:	12002400 	and	w0, w0, #0x3ff
 40279b0:	0b090042 	add	w2, w2, w9
 40279b4:	b90037e2 	str	w2, [sp, #52]
 40279b8:	b94033e2 	ldr	w2, [sp, #48]
 40279bc:	0b000040 	add	w0, w2, w0
 40279c0:	b90033e0 	str	w0, [sp, #48]
 40279c4:	b9403fe2 	ldr	w2, [sp, #60]
 40279c8:	b9400080 	ldr	w0, [x4]
 40279cc:	d3506409 	ubfx	x9, x0, #16, #10
 40279d0:	12002400 	and	w0, w0, #0x3ff
 40279d4:	0b090042 	add	w2, w2, w9
 40279d8:	b9003fe2 	str	w2, [sp, #60]
 40279dc:	b9403be2 	ldr	w2, [sp, #56]
 40279e0:	0b000040 	add	w0, w2, w0
 40279e4:	b9003be0 	str	w0, [sp, #56]
 40279e8:	b94047e2 	ldr	w2, [sp, #68]
 40279ec:	b9400060 	ldr	w0, [x3]
 40279f0:	d3506409 	ubfx	x9, x0, #16, #10
 40279f4:	12002400 	and	w0, w0, #0x3ff
 40279f8:	0b090042 	add	w2, w2, w9
 40279fc:	b90047e2 	str	w2, [sp, #68]
 4027a00:	b94043e2 	ldr	w2, [sp, #64]
 4027a04:	0b000040 	add	w0, w2, w0
 4027a08:	b90043e0 	str	w0, [sp, #64]
 4027a0c:	54fff921 	b.ne	4027930 <start_svb+0x1d8>  // b.any
 4027a10:	910063e0 	add	x0, sp, #0x18
 4027a14:	97fffea3 	bl	40274a0 <hpm_value_avg.constprop.0>
 4027a18:	2a0003e5 	mov	w5, w0
 4027a1c:	9100a3e0 	add	x0, sp, #0x28
 4027a20:	97fffea0 	bl	40274a0 <hpm_value_avg.constprop.0>
 4027a24:	2a0003e4 	mov	w4, w0
 4027a28:	9100e3e0 	add	x0, sp, #0x38
 4027a2c:	97fffe9d 	bl	40274a0 <hpm_value_avg.constprop.0>
 4027a30:	2a0003e3 	mov	w3, w0
 4027a34:	d288e180 	mov	x0, #0x470c                	// #18188
 4027a38:	f2a22040 	movk	x0, #0x1102, lsl #16
 4027a3c:	b9400014 	ldr	w20, [x0]
 4027a40:	b9401015 	ldr	w21, [x0, #16]
 4027a44:	d2940100 	mov	x0, #0xa008                	// #40968
 4027a48:	f2a22040 	movk	x0, #0x1102, lsl #16
 4027a4c:	b90017ff 	str	wzr, [sp, #20]
 4027a50:	12002298 	and	w24, w20, #0x1ff
 4027a54:	120022b9 	and	w25, w21, #0x1ff
 4027a58:	b9400000 	ldr	w0, [x0]
 4027a5c:	d3506294 	ubfx	x20, x20, #16, #9
 4027a60:	b90017e0 	str	w0, [sp, #20]
 4027a64:	d35062b5 	ubfx	x21, x21, #16, #9
 4027a68:	4b140316 	sub	w22, w24, w20
 4027a6c:	4b150337 	sub	w23, w25, w21
 4027a70:	b94017e0 	ldr	w0, [sp, #20]
 4027a74:	12002400 	and	w0, w0, #0x3ff
 4027a78:	b90017e0 	str	w0, [sp, #20]
 4027a7c:	528014a0 	mov	w0, #0xa5                  	// #165
 4027a80:	b94017f3 	ldr	w19, [sp, #20]
 4027a84:	51021273 	sub	w19, w19, #0x84
 4027a88:	1b007e73 	mul	w19, w19, w0
 4027a8c:	52806500 	mov	w0, #0x328                 	// #808
 4027a90:	1ac00e73 	sdiv	w19, w19, w0
 4027a94:	d2802500 	mov	x0, #0x128                 	// #296
 4027a98:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4027a9c:	b9400002 	ldr	w2, [x0]
 4027aa0:	5100a27a 	sub	w26, w19, #0x28
 4027aa4:	72002440 	ands	w0, w2, #0x3ff
 4027aa8:	d34a4c41 	ubfx	x1, x2, #10, #10
 4027aac:	7a401824 	ccmp	w1, #0x0, #0x4, ne  // ne = any
 4027ab0:	54000060 	b.eq	4027abc <start_svb+0x364>  // b.none
 4027ab4:	d3547442 	ubfx	x2, x2, #20, #10
 4027ab8:	35001562 	cbnz	w2, 4027d64 <start_svb+0x60c>
 4027abc:	71040cbf 	cmp	w5, #0x103
 4027ac0:	540008ed 	b.le	4027bdc <start_svb+0x484>
 4027ac4:	7101b75f 	cmp	w26, #0x6d
 4027ac8:	5400018d 	b.le	4027af8 <start_svb+0x3a0>
 4027acc:	528a3481 	mov	w1, #0x51a4                	// #20900
 4027ad0:	5280ae60 	mov	w0, #0x573                 	// #1395
 4027ad4:	72bfff61 	movk	w1, #0xfffb, lsl #16
 4027ad8:	1b0004a0 	madd	w0, w5, w0, w1
 4027adc:	5284e201 	mov	w1, #0x2710                	// #10000
 4027ae0:	1ac10800 	udiv	w0, w0, w1
 4027ae4:	7104209f 	cmp	w4, #0x108
 4027ae8:	540008ec 	b.gt	4027c04 <start_svb+0x4ac>
 4027aec:	7103f87f 	cmp	w3, #0xfe
 4027af0:	54000ccc 	b.gt	4027c88 <start_svb+0x530>
 4027af4:	1400003f 	b	4027bf0 <start_svb+0x498>
 4027af8:	7101675f 	cmp	w26, #0x59
 4027afc:	540002ad 	b.le	4027b50 <start_svb+0x3f8>
 4027b00:	529ec381 	mov	w1, #0xf61c                	// #63004
 4027b04:	528072e0 	mov	w0, #0x397                 	// #919
 4027b08:	72bfff81 	movk	w1, #0xfffc, lsl #16
 4027b0c:	1b0004a0 	madd	w0, w5, w0, w1
 4027b10:	5284e201 	mov	w1, #0x2710                	// #10000
 4027b14:	1ac10800 	udiv	w0, w0, w1
 4027b18:	7104209f 	cmp	w4, #0x108
 4027b1c:	540024ad 	b.le	4027fb0 <start_svb+0x858>
 4027b20:	52823242 	mov	w2, #0x1192                	// #4498
 4027b24:	52806661 	mov	w1, #0x333                 	// #819
 4027b28:	72bfffa2 	movk	w2, #0xfffd, lsl #16
 4027b2c:	1b010881 	madd	w1, w4, w1, w2
 4027b30:	5284e202 	mov	w2, #0x2710                	// #10000
 4027b34:	1ac20821 	udiv	w1, w1, w2
 4027b38:	7103f87f 	cmp	w3, #0xfe
 4027b3c:	5400074d 	b.le	4027c24 <start_svb+0x4cc>
 4027b40:	52859846 	mov	w6, #0x2cc2                	// #11458
 4027b44:	52806a62 	mov	w2, #0x353                 	// #851
 4027b48:	72bfffa6 	movk	w6, #0xfffd, lsl #16
 4027b4c:	14000053 	b	4027c98 <start_svb+0x540>
 4027b50:	7101175f 	cmp	w26, #0x45
 4027b54:	540002ad 	b.le	4027ba8 <start_svb+0x450>
 4027b58:	52853101 	mov	w1, #0x2988                	// #10632
 4027b5c:	528044c0 	mov	w0, #0x226                 	// #550
 4027b60:	72bfffc1 	movk	w1, #0xfffe, lsl #16
 4027b64:	1b0004a0 	madd	w0, w5, w0, w1
 4027b68:	5284e201 	mov	w1, #0x2710                	// #10000
 4027b6c:	1ac10800 	udiv	w0, w0, w1
 4027b70:	7104209f 	cmp	w4, #0x108
 4027b74:	5400226d 	b.le	4027fc0 <start_svb+0x868>
 4027b78:	528f66a2 	mov	w2, #0x7b35                	// #31541
 4027b7c:	528034c1 	mov	w1, #0x1a6                 	// #422
 4027b80:	72bfffc2 	movk	w2, #0xfffe, lsl #16
 4027b84:	1b010881 	madd	w1, w4, w1, w2
 4027b88:	5284e202 	mov	w2, #0x2710                	// #10000
 4027b8c:	1ac20821 	udiv	w1, w1, w2
 4027b90:	7103f87f 	cmp	w3, #0xfe
 4027b94:	5400048d 	b.le	4027c24 <start_svb+0x4cc>
 4027b98:	52893846 	mov	w6, #0x49c2                	// #18882
 4027b9c:	528040a2 	mov	w2, #0x205                 	// #517
 4027ba0:	72bfffc6 	movk	w6, #0xfffe, lsl #16
 4027ba4:	1400003d 	b	4027c98 <start_svb+0x540>
 4027ba8:	36f81f3a 	tbz	w26, #31, 4027f8c <start_svb+0x834>
 4027bac:	528f0861 	mov	w1, #0x7843                	// #30787
 4027bb0:	12803ba0 	mov	w0, #0xfffffe22            	// #-478
 4027bb4:	72a00021 	movk	w1, #0x1, lsl #16
 4027bb8:	1b0004a0 	madd	w0, w5, w0, w1
 4027bbc:	5284e201 	mov	w1, #0x2710                	// #10000
 4027bc0:	1ac10c00 	sdiv	w0, w0, w1
 4027bc4:	7104209f 	cmp	w4, #0x108
 4027bc8:	540003cc 	b.gt	4027c40 <start_svb+0x4e8>
 4027bcc:	7103f87f 	cmp	w3, #0xfe
 4027bd0:	5400010d 	b.le	4027bf0 <start_svb+0x498>
 4027bd4:	52800001 	mov	w1, #0x0                   	// #0
 4027bd8:	14000022 	b	4027c60 <start_svb+0x508>
 4027bdc:	7104209f 	cmp	w4, #0x108
 4027be0:	540000cc 	b.gt	4027bf8 <start_svb+0x4a0>
 4027be4:	7103f87f 	cmp	w3, #0xfe
 4027be8:	540004ac 	b.gt	4027c7c <start_svb+0x524>
 4027bec:	52800000 	mov	w0, #0x0                   	// #0
 4027bf0:	52800001 	mov	w1, #0x0                   	// #0
 4027bf4:	1400000c 	b	4027c24 <start_svb+0x4cc>
 4027bf8:	52800000 	mov	w0, #0x0                   	// #0
 4027bfc:	7101b75f 	cmp	w26, #0x6d
 4027c00:	5400016d 	b.le	4027c2c <start_svb+0x4d4>
 4027c04:	529a2d42 	mov	w2, #0xd16a                	// #53610
 4027c08:	52809601 	mov	w1, #0x4b0                 	// #1200
 4027c0c:	72bfff62 	movk	w2, #0xfffb, lsl #16
 4027c10:	1b010881 	madd	w1, w4, w1, w2
 4027c14:	5284e202 	mov	w2, #0x2710                	// #10000
 4027c18:	1ac20821 	udiv	w1, w1, w2
 4027c1c:	7103f87f 	cmp	w3, #0xfe
 4027c20:	5400036c 	b.gt	4027c8c <start_svb+0x534>
 4027c24:	52800002 	mov	w2, #0x0                   	// #0
 4027c28:	1400001f 	b	4027ca4 <start_svb+0x54c>
 4027c2c:	7101675f 	cmp	w26, #0x59
 4027c30:	54fff78c 	b.gt	4027b20 <start_svb+0x3c8>
 4027c34:	7101175f 	cmp	w26, #0x45
 4027c38:	54fffa0c 	b.gt	4027b78 <start_svb+0x420>
 4027c3c:	36fffdba 	tbz	w26, #31, 4027bf0 <start_svb+0x498>
 4027c40:	5297c1c2 	mov	w2, #0xbe0e                	// #48654
 4027c44:	12803781 	mov	w1, #0xfffffe43            	// #-445
 4027c48:	72a00022 	movk	w2, #0x1, lsl #16
 4027c4c:	1b010881 	madd	w1, w4, w1, w2
 4027c50:	5284e202 	mov	w2, #0x2710                	// #10000
 4027c54:	1ac20c21 	sdiv	w1, w1, w2
 4027c58:	7103f87f 	cmp	w3, #0xfe
 4027c5c:	54fffe4d 	b.le	4027c24 <start_svb+0x4cc>
 4027c60:	5292ce46 	mov	w6, #0x9672                	// #38514
 4027c64:	12803e22 	mov	w2, #0xfffffe0e            	// #-498
 4027c68:	72a00026 	movk	w6, #0x1, lsl #16
 4027c6c:	1b021862 	madd	w2, w3, w2, w6
 4027c70:	5284e206 	mov	w6, #0x2710                	// #10000
 4027c74:	1ac60c42 	sdiv	w2, w2, w6
 4027c78:	1400000b 	b	4027ca4 <start_svb+0x54c>
 4027c7c:	7101b75f 	cmp	w26, #0x6d
 4027c80:	540018ed 	b.le	4027f9c <start_svb+0x844>
 4027c84:	52800000 	mov	w0, #0x0                   	// #0
 4027c88:	52800001 	mov	w1, #0x0                   	// #0
 4027c8c:	52882bc6 	mov	w6, #0x415e                	// #16734
 4027c90:	52808f82 	mov	w2, #0x47c                 	// #1148
 4027c94:	72bfff86 	movk	w6, #0xfffc, lsl #16
 4027c98:	1b021862 	madd	w2, w3, w2, w6
 4027c9c:	5284e206 	mov	w6, #0x2710                	// #10000
 4027ca0:	1ac60842 	udiv	w2, w2, w6
 4027ca4:	0b050000 	add	w0, w0, w5
 4027ca8:	0b020062 	add	w2, w3, w2
 4027cac:	0b010081 	add	w1, w4, w1
 4027cb0:	52800024 	mov	w4, #0x1                   	// #1
 4027cb4:	12002405 	and	w5, w0, #0x3ff
 4027cb8:	53162423 	ubfiz	w3, w1, #10, #10
 4027cbc:	2a050063 	orr	w3, w3, w5
 4027cc0:	530c2445 	ubfiz	w5, w2, #20, #10
 4027cc4:	2a050063 	orr	w3, w3, w5
 4027cc8:	2a047863 	orr	w3, w3, w4, lsl #30
 4027ccc:	d2806804 	mov	x4, #0x340                 	// #832
 4027cd0:	f2a22044 	movk	x4, #0x1102, lsl #16
 4027cd4:	b9000083 	str	w3, [x4]
 4027cd8:	d2802403 	mov	x3, #0x120                 	// #288
 4027cdc:	f2a203c3 	movk	x3, #0x101e, lsl #16
 4027ce0:	b90013ff 	str	wzr, [sp, #16]
 4027ce4:	b9400063 	ldr	w3, [x3]
 4027ce8:	b90013e3 	str	w3, [sp, #16]
 4027cec:	d2802683 	mov	x3, #0x134                 	// #308
 4027cf0:	f2a22043 	movk	x3, #0x1102, lsl #16
 4027cf4:	b9400063 	ldr	w3, [x3]
 4027cf8:	b9000be3 	str	w3, [sp, #8]
 4027cfc:	b9400be3 	ldr	w3, [sp, #8]
 4027d00:	12000463 	and	w3, w3, #0x3
 4027d04:	7100047f 	cmp	w3, #0x1
 4027d08:	b9400be3 	ldr	w3, [sp, #8]
 4027d0c:	54000a61 	b.ne	4027e58 <start_svb+0x700>  // b.any
 4027d10:	d3421463 	ubfx	x3, x3, #2, #4
 4027d14:	7100047f 	cmp	w3, #0x1
 4027d18:	540002a1 	b.ne	4027d6c <start_svb+0x614>  // b.any
 4027d1c:	b94013e4 	ldr	w4, [sp, #16]
 4027d20:	52802023 	mov	w3, #0x101                 	// #257
 4027d24:	72a00443 	movk	w3, #0x22, lsl #16
 4027d28:	6b03009f 	cmp	w4, w3
 4027d2c:	54000201 	b.ne	4027d6c <start_svb+0x614>  // b.any
 4027d30:	d0000043 	adrp	x3, 4031000 <sync_exception_sp_el0>
 4027d34:	913cf063 	add	x3, x3, #0xf3c
 4027d38:	97fffdfc 	bl	4027528 <set_volt>
 4027d3c:	14000027 	b	4027dd8 <start_svb+0x680>
 4027d40:	7101175f 	cmp	w26, #0x45
 4027d44:	5400008d 	b.le	4027d54 <start_svb+0x5fc>
 4027d48:	52800001 	mov	w1, #0x0                   	// #0
 4027d4c:	52800000 	mov	w0, #0x0                   	// #0
 4027d50:	17ffff92 	b	4027b98 <start_svb+0x440>
 4027d54:	36fff4da 	tbz	w26, #31, 4027bec <start_svb+0x494>
 4027d58:	52800001 	mov	w1, #0x0                   	// #0
 4027d5c:	52800000 	mov	w0, #0x0                   	// #0
 4027d60:	17ffffc0 	b	4027c60 <start_svb+0x508>
 4027d64:	52800004 	mov	w4, #0x0                   	// #0
 4027d68:	17ffffd3 	b	4027cb4 <start_svb+0x55c>
 4027d6c:	b9400be3 	ldr	w3, [sp, #8]
 4027d70:	d3421463 	ubfx	x3, x3, #2, #4
 4027d74:	7100087f 	cmp	w3, #0x2
 4027d78:	54000141 	b.ne	4027da0 <start_svb+0x648>  // b.any
 4027d7c:	b94013e4 	ldr	w4, [sp, #16]
 4027d80:	52802023 	mov	w3, #0x101                 	// #257
 4027d84:	72a00443 	movk	w3, #0x22, lsl #16
 4027d88:	6b03009f 	cmp	w4, w3
 4027d8c:	540000a1 	b.ne	4027da0 <start_svb+0x648>  // b.any
 4027d90:	d0000043 	adrp	x3, 4031000 <sync_exception_sp_el0>
 4027d94:	913cf063 	add	x3, x3, #0xf3c
 4027d98:	9100f063 	add	x3, x3, #0x3c
 4027d9c:	17ffffe7 	b	4027d38 <start_svb+0x5e0>
 4027da0:	b9400be3 	ldr	w3, [sp, #8]
 4027da4:	d3421463 	ubfx	x3, x3, #2, #4
 4027da8:	71000c7f 	cmp	w3, #0x3
 4027dac:	54000141 	b.ne	4027dd4 <start_svb+0x67c>  // b.any
 4027db0:	b94013e4 	ldr	w4, [sp, #16]
 4027db4:	52802043 	mov	w3, #0x102                 	// #258
 4027db8:	72a00443 	movk	w3, #0x22, lsl #16
 4027dbc:	6b03009f 	cmp	w4, w3
 4027dc0:	540000a1 	b.ne	4027dd4 <start_svb+0x67c>  // b.any
 4027dc4:	d0000043 	adrp	x3, 4031000 <sync_exception_sp_el0>
 4027dc8:	913cf063 	add	x3, x3, #0xf3c
 4027dcc:	9101e063 	add	x3, x3, #0x78
 4027dd0:	17ffffda 	b	4027d38 <start_svb+0x5e0>
 4027dd4:	97fffda4 	bl	4027464 <svb_error>
 4027dd8:	d2806a80 	mov	x0, #0x354                 	// #852
 4027ddc:	f2a22040 	movk	x0, #0x1102, lsl #16
 4027de0:	b9400002 	ldr	w2, [x0]
 4027de4:	37f808da 	tbnz	w26, #31, 4027efc <start_svb+0x7a4>
 4027de8:	12001f41 	and	w1, w26, #0xff
 4027dec:	52800000 	mov	w0, #0x0                   	// #0
 4027df0:	33001c20 	bfxil	w0, w1, #0, #8
 4027df4:	33101c22 	bfi	w2, w1, #16, #8
 4027df8:	d2806a81 	mov	x1, #0x354                 	// #852
 4027dfc:	f2a22041 	movk	x1, #0x1102, lsl #16
 4027e00:	b9000022 	str	w2, [x1]
 4027e04:	3100535f 	cmn	w26, #0x14
 4027e08:	540008ea 	b.ge	4027f24 <start_svb+0x7cc>  // b.tcont
 4027e0c:	12106c00 	and	w0, w0, #0xffff0fff
 4027e10:	36f80076 	tbz	w22, #31, 4027e1c <start_svb+0x6c4>
 4027e14:	32090000 	orr	w0, w0, #0x800000
 4027e18:	4b180296 	sub	w22, w20, w24
 4027e1c:	33101ac0 	bfi	w0, w22, #16, #7
 4027e20:	36f80077 	tbz	w23, #31, 4027e2c <start_svb+0x6d4>
 4027e24:	32010000 	orr	w0, w0, #0x80000000
 4027e28:	4b1902b7 	sub	w23, w21, w25
 4027e2c:	d2806a01 	mov	x1, #0x350                 	// #848
 4027e30:	33081ae0 	bfi	w0, w23, #24, #7
 4027e34:	f2a22041 	movk	x1, #0x1102, lsl #16
 4027e38:	b9000020 	str	w0, [x1]
 4027e3c:	d0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4027e40:	f94027e1 	ldr	x1, [sp, #72]
 4027e44:	f9477002 	ldr	x2, [x0, #3808]
 4027e48:	eb020021 	subs	x1, x1, x2
 4027e4c:	d2800002 	mov	x2, #0x0                   	// #0
 4027e50:	540008e0 	b.eq	4027f6c <start_svb+0x814>  // b.none
 4027e54:	97fff8ea 	bl	40261fc <__stack_chk_fail>
 4027e58:	f240047f 	tst	x3, #0x3
 4027e5c:	54fffbc1 	b.ne	4027dd4 <start_svb+0x67c>  // b.any
 4027e60:	b9400be3 	ldr	w3, [sp, #8]
 4027e64:	d3421463 	ubfx	x3, x3, #2, #4
 4027e68:	7100047f 	cmp	w3, #0x1
 4027e6c:	54000141 	b.ne	4027e94 <start_svb+0x73c>  // b.any
 4027e70:	b94013e4 	ldr	w4, [sp, #16]
 4027e74:	52802023 	mov	w3, #0x101                 	// #257
 4027e78:	72a00443 	movk	w3, #0x22, lsl #16
 4027e7c:	6b03009f 	cmp	w4, w3
 4027e80:	540000a1 	b.ne	4027e94 <start_svb+0x73c>  // b.any
 4027e84:	d0000043 	adrp	x3, 4031000 <sync_exception_sp_el0>
 4027e88:	913cf063 	add	x3, x3, #0xf3c
 4027e8c:	9102d063 	add	x3, x3, #0xb4
 4027e90:	17ffffaa 	b	4027d38 <start_svb+0x5e0>
 4027e94:	b9400be3 	ldr	w3, [sp, #8]
 4027e98:	d3421463 	ubfx	x3, x3, #2, #4
 4027e9c:	7100087f 	cmp	w3, #0x2
 4027ea0:	54000141 	b.ne	4027ec8 <start_svb+0x770>  // b.any
 4027ea4:	b94013e4 	ldr	w4, [sp, #16]
 4027ea8:	52802023 	mov	w3, #0x101                 	// #257
 4027eac:	72a00443 	movk	w3, #0x22, lsl #16
 4027eb0:	6b03009f 	cmp	w4, w3
 4027eb4:	540000a1 	b.ne	4027ec8 <start_svb+0x770>  // b.any
 4027eb8:	d0000043 	adrp	x3, 4031000 <sync_exception_sp_el0>
 4027ebc:	913cf063 	add	x3, x3, #0xf3c
 4027ec0:	9103c063 	add	x3, x3, #0xf0
 4027ec4:	17ffff9d 	b	4027d38 <start_svb+0x5e0>
 4027ec8:	b9400be3 	ldr	w3, [sp, #8]
 4027ecc:	d3421463 	ubfx	x3, x3, #2, #4
 4027ed0:	71000c7f 	cmp	w3, #0x3
 4027ed4:	54fff801 	b.ne	4027dd4 <start_svb+0x67c>  // b.any
 4027ed8:	b94013e4 	ldr	w4, [sp, #16]
 4027edc:	52802043 	mov	w3, #0x102                 	// #258
 4027ee0:	72a00443 	movk	w3, #0x22, lsl #16
 4027ee4:	6b03009f 	cmp	w4, w3
 4027ee8:	54fff761 	b.ne	4027dd4 <start_svb+0x67c>  // b.any
 4027eec:	d0000043 	adrp	x3, 4031000 <sync_exception_sp_el0>
 4027ef0:	913cf063 	add	x3, x3, #0xf3c
 4027ef4:	9104b063 	add	x3, x3, #0x12c
 4027ef8:	17ffff90 	b	4027d38 <start_svb+0x5e0>
 4027efc:	52800021 	mov	w1, #0x1                   	// #1
 4027f00:	52800000 	mov	w0, #0x0                   	// #0
 4027f04:	33180c20 	bfi	w0, w1, #8, #4
 4027f08:	52800501 	mov	w1, #0x28                  	// #40
 4027f0c:	4b130033 	sub	w19, w1, w19
 4027f10:	32080042 	orr	w2, w2, #0x1000000
 4027f14:	12001e73 	and	w19, w19, #0xff
 4027f18:	33001e60 	bfxil	w0, w19, #0, #8
 4027f1c:	33101e62 	bfi	w2, w19, #16, #8
 4027f20:	17ffffb6 	b	4027df8 <start_svb+0x6a0>
 4027f24:	36f8009a 	tbz	w26, #31, 4027f34 <start_svb+0x7dc>
 4027f28:	52800021 	mov	w1, #0x1                   	// #1
 4027f2c:	33140c20 	bfi	w0, w1, #12, #4
 4027f30:	17ffffb8 	b	4027e10 <start_svb+0x6b8>
 4027f34:	71008b5f 	cmp	w26, #0x22
 4027f38:	5400006c 	b.gt	4027f44 <start_svb+0x7ec>
 4027f3c:	52800041 	mov	w1, #0x2                   	// #2
 4027f40:	17fffffb 	b	4027f2c <start_svb+0x7d4>
 4027f44:	71012b5f 	cmp	w26, #0x4a
 4027f48:	5400006c 	b.gt	4027f54 <start_svb+0x7fc>
 4027f4c:	52800061 	mov	w1, #0x3                   	// #3
 4027f50:	17fffff7 	b	4027f2c <start_svb+0x7d4>
 4027f54:	7101cb5f 	cmp	w26, #0x72
 4027f58:	5400006c 	b.gt	4027f64 <start_svb+0x80c>
 4027f5c:	52800081 	mov	w1, #0x4                   	// #4
 4027f60:	17fffff3 	b	4027f2c <start_svb+0x7d4>
 4027f64:	528000a1 	mov	w1, #0x5                   	// #5
 4027f68:	17fffff1 	b	4027f2c <start_svb+0x7d4>
 4027f6c:	a94553f3 	ldp	x19, x20, [sp, #80]
 4027f70:	d28000c0 	mov	x0, #0x6                   	// #6
 4027f74:	a9465bf5 	ldp	x21, x22, [sp, #96]
 4027f78:	a94763f7 	ldp	x23, x24, [sp, #112]
 4027f7c:	a9486bf9 	ldp	x25, x26, [sp, #128]
 4027f80:	f9404bfe 	ldr	x30, [sp, #144]
 4027f84:	910283ff 	add	sp, sp, #0xa0
 4027f88:	14000350 	b	4028cc8 <mdelay>
 4027f8c:	7104209f 	cmp	w4, #0x108
 4027f90:	54ffe2ec 	b.gt	4027bec <start_svb+0x494>
 4027f94:	7103f87f 	cmp	w3, #0xfe
 4027f98:	54ffe2ad 	b.le	4027bec <start_svb+0x494>
 4027f9c:	7101675f 	cmp	w26, #0x59
 4027fa0:	54ffed0d 	b.le	4027d40 <start_svb+0x5e8>
 4027fa4:	52800001 	mov	w1, #0x0                   	// #0
 4027fa8:	52800000 	mov	w0, #0x0                   	// #0
 4027fac:	17fffee5 	b	4027b40 <start_svb+0x3e8>
 4027fb0:	7103f87f 	cmp	w3, #0xfe
 4027fb4:	54ffe1ed 	b.le	4027bf0 <start_svb+0x498>
 4027fb8:	52800001 	mov	w1, #0x0                   	// #0
 4027fbc:	17fffee1 	b	4027b40 <start_svb+0x3e8>
 4027fc0:	7103f87f 	cmp	w3, #0xfe
 4027fc4:	54ffe16d 	b.le	4027bf0 <start_svb+0x498>
 4027fc8:	52800001 	mov	w1, #0x0                   	// #0
 4027fcc:	17fffef3 	b	4027b98 <start_svb+0x440>

0000000004027fd0 <end_svb>:
 4027fd0:	f81e0ffe 	str	x30, [sp, #-32]!
 4027fd4:	d2806881 	mov	x1, #0x344                 	// #836
 4027fd8:	f2a22041 	movk	x1, #0x1102, lsl #16
 4027fdc:	52807582 	mov	w2, #0x3ac                 	// #940
 4027fe0:	b9001bff 	str	wzr, [sp, #24]
 4027fe4:	b9001fff 	str	wzr, [sp, #28]
 4027fe8:	b9400020 	ldr	w0, [x1]
 4027fec:	12003c00 	and	w0, w0, #0xffff
 4027ff0:	b9001be0 	str	w0, [sp, #24]
 4027ff4:	b9401be0 	ldr	w0, [sp, #24]
 4027ff8:	710eb01f 	cmp	w0, #0x3ac
 4027ffc:	1a822000 	csel	w0, w0, w2, cs  // cs = hs, nlast
 4028000:	b9001be0 	str	w0, [sp, #24]
 4028004:	b9400020 	ldr	w0, [x1]
 4028008:	12103c00 	and	w0, w0, #0xffff0000
 402800c:	b9001fe0 	str	w0, [sp, #28]
 4028010:	b9401fe0 	ldr	w0, [sp, #28]
 4028014:	b9401be2 	ldr	w2, [sp, #24]
 4028018:	2a020000 	orr	w0, w0, w2
 402801c:	b9001fe0 	str	w0, [sp, #28]
 4028020:	b9401fe0 	ldr	w0, [sp, #28]
 4028024:	b9000020 	str	w0, [x1]
 4028028:	52808361 	mov	w1, #0x41b                 	// #1051
 402802c:	b9401be0 	ldr	w0, [sp, #24]
 4028030:	97fffd2a 	bl	40274d8 <calc_volt_regval.constprop.0>
 4028034:	b9001be0 	str	w0, [sp, #24]
 4028038:	52920001 	mov	w1, #0x9000                	// #36864
 402803c:	b9401be0 	ldr	w0, [sp, #24]
 4028040:	72a22041 	movk	w1, #0x1102, lsl #16
 4028044:	97fffd02 	bl	402744c <svb_pwm_cfg>
 4028048:	f84207fe 	ldr	x30, [sp], #32
 402804c:	d28000c0 	mov	x0, #0x6                   	// #6
 4028050:	1400031e 	b	4028cc8 <mdelay>

0000000004028054 <reg_read>:
 4028054:	b9400002 	ldr	w2, [x0]
 4028058:	b9400c04 	ldr	w4, [x0, #12]
 402805c:	b9400042 	ldr	w2, [x2]
 4028060:	d3535c85 	ubfx	x5, x4, #19, #5
 4028064:	110004a6 	add	w6, w5, #0x1
 4028068:	71007cbf 	cmp	w5, #0x1f
 402806c:	540000e0 	b.eq	4028088 <reg_read+0x34>  // b.none
 4028070:	531b7c84 	lsr	w4, w4, #27
 4028074:	52800023 	mov	w3, #0x1                   	// #1
 4028078:	1ac62063 	lsl	w3, w3, w6
 402807c:	51000463 	sub	w3, w3, #0x1
 4028080:	1ac42442 	lsr	w2, w2, w4
 4028084:	0a020062 	and	w2, w3, w2
 4028088:	b9400400 	ldr	w0, [x0, #4]
 402808c:	6b02001f 	cmp	w0, w2
 4028090:	1a9f07e0 	cset	w0, ne  // ne = any
 4028094:	b9000020 	str	w0, [x1]
 4028098:	d65f03c0 	ret

000000000402809c <reg_write>:
 402809c:	29400801 	ldp	w1, w2, [x0]
 40280a0:	29410c04 	ldp	w4, w3, [x0, #8]
 40280a4:	b9400026 	ldr	w6, [x1]
 40280a8:	d3431c65 	ubfx	x5, x3, #3, #5
 40280ac:	110004a7 	add	w7, w5, #0x1
 40280b0:	71007cbf 	cmp	w5, #0x1f
 40280b4:	54000120 	b.eq	40280d8 <reg_write+0x3c>  // b.none
 40280b8:	52800021 	mov	w1, #0x1                   	// #1
 40280bc:	d34b3c63 	ubfx	x3, x3, #11, #5
 40280c0:	1ac72021 	lsl	w1, w1, w7
 40280c4:	51000421 	sub	w1, w1, #0x1
 40280c8:	1ac32042 	lsl	w2, w2, w3
 40280cc:	1ac32021 	lsl	w1, w1, w3
 40280d0:	0a2100c1 	bic	w1, w6, w1
 40280d4:	2a010042 	orr	w2, w2, w1
 40280d8:	b9400000 	ldr	w0, [x0]
 40280dc:	b9000002 	str	w2, [x0]
 40280e0:	d503201f 	nop
 40280e4:	2a0403e0 	mov	w0, w4
 40280e8:	51000484 	sub	w4, w4, #0x1
 40280ec:	35ffffa0 	cbnz	w0, 40280e0 <reg_write+0x44>
 40280f0:	d65f03c0 	ret

00000000040280f4 <init_registers>:
 40280f4:	d10083ff 	sub	sp, sp, #0x20
 40280f8:	2a0103eb 	mov	w11, w1
 40280fc:	2a0003e9 	mov	w9, w0
 4028100:	b0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4028104:	5280000a 	mov	w10, #0x0                   	// #0
 4028108:	f9000bfe 	str	x30, [sp, #16]
 402810c:	f9477022 	ldr	x2, [x1, #3808]
 4028110:	f90007e2 	str	x2, [sp, #8]
 4028114:	d2800002 	mov	x2, #0x0                   	// #0
 4028118:	d37c7d40 	ubfiz	x0, x10, #4, #32
 402811c:	8b000128 	add	x8, x9, x0
 4028120:	b8606920 	ldr	w0, [x9, x0]
 4028124:	29410907 	ldp	w7, w2, [x8, #8]
 4028128:	350001a0 	cbnz	w0, 402815c <init_registers+0x68>
 402812c:	b9400500 	ldr	w0, [x8, #4]
 4028130:	2a070043 	orr	w3, w2, w7
 4028134:	2a000063 	orr	w3, w3, w0
 4028138:	35000123 	cbnz	w3, 402815c <init_registers+0x68>
 402813c:	d503201f 	nop
 4028140:	b0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4028144:	f94007e1 	ldr	x1, [sp, #8]
 4028148:	f9477002 	ldr	x2, [x0, #3808]
 402814c:	eb020021 	subs	x1, x1, x2
 4028150:	d2800002 	mov	x2, #0x0                   	// #0
 4028154:	54000580 	b.eq	4028204 <init_registers+0x110>  // b.none
 4028158:	97fff829 	bl	40261fc <__stack_chk_fail>
 402815c:	b90007ff 	str	wzr, [sp, #4]
 4028160:	340002eb 	cbz	w11, 40281bc <init_registers+0xc8>
 4028164:	36080082 	tbz	w2, #1, 4028174 <init_registers+0x80>
 4028168:	aa0803e0 	mov	x0, x8
 402816c:	97ffffcc 	bl	402809c <reg_write>
 4028170:	1400000c 	b	40281a0 <init_registers+0xac>
 4028174:	368801a2 	tbz	w2, #17, 40281a8 <init_registers+0xb4>
 4028178:	910013e1 	add	x1, sp, #0x4
 402817c:	aa0803e0 	mov	x0, x8
 4028180:	97ffffb5 	bl	4028054 <reg_read>
 4028184:	d503201f 	nop
 4028188:	b94007e0 	ldr	w0, [sp, #4]
 402818c:	35ffff60 	cbnz	w0, 4028178 <init_registers+0x84>
 4028190:	d503201f 	nop
 4028194:	2a0703e0 	mov	w0, w7
 4028198:	510004e7 	sub	w7, w7, #0x1
 402819c:	35ffffa0 	cbnz	w0, 4028190 <init_registers+0x9c>
 40281a0:	1100054a 	add	w10, w10, #0x1
 40281a4:	17ffffdd 	b	4028118 <init_registers+0x24>
 40281a8:	d503201f 	nop
 40281ac:	2a0703e0 	mov	w0, w7
 40281b0:	510004e7 	sub	w7, w7, #0x1
 40281b4:	35ffffa0 	cbnz	w0, 40281a8 <init_registers+0xb4>
 40281b8:	17fffffa 	b	40281a0 <init_registers+0xac>
 40281bc:	3717fd62 	tbnz	w2, #2, 4028168 <init_registers+0x74>
 40281c0:	36900182 	tbz	w2, #18, 40281f0 <init_registers+0xfc>
 40281c4:	910013e1 	add	x1, sp, #0x4
 40281c8:	aa0803e0 	mov	x0, x8
 40281cc:	97ffffa2 	bl	4028054 <reg_read>
 40281d0:	d503201f 	nop
 40281d4:	b94007e0 	ldr	w0, [sp, #4]
 40281d8:	35ffff60 	cbnz	w0, 40281c4 <init_registers+0xd0>
 40281dc:	d503201f 	nop
 40281e0:	2a0703e0 	mov	w0, w7
 40281e4:	510004e7 	sub	w7, w7, #0x1
 40281e8:	35ffffa0 	cbnz	w0, 40281dc <init_registers+0xe8>
 40281ec:	17ffffed 	b	40281a0 <init_registers+0xac>
 40281f0:	d503201f 	nop
 40281f4:	2a0703e0 	mov	w0, w7
 40281f8:	510004e7 	sub	w7, w7, #0x1
 40281fc:	35ffffa0 	cbnz	w0, 40281f0 <init_registers+0xfc>
 4028200:	17ffffe8 	b	40281a0 <init_registers+0xac>
 4028204:	f9400bfe 	ldr	x30, [sp, #16]
 4028208:	910083ff 	add	sp, sp, #0x20
 402820c:	d65f03c0 	ret

0000000004028210 <bsp_ddrtrn_dmc_auto_power_down_cfg>:
 4028210:	140012a6 	b	402cca8 <ddrtrn_dmc_auto_power_down_cfg>

0000000004028214 <bsp_ddrtrn_retrain_enable>:
 4028214:	14001883 	b	402e420 <ddrtrn_retrain_enable>

0000000004028218 <bsp_ddrtrn_training_item>:
 4028218:	d10283ff 	sub	sp, sp, #0xa0
 402821c:	a90853f3 	stp	x19, x20, [sp, #128]
 4028220:	2a0003f4 	mov	w20, w0
 4028224:	2a0103f3 	mov	w19, w1
 4028228:	a9097bf5 	stp	x21, x30, [sp, #144]
 402822c:	b0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4028230:	2a0203f5 	mov	w21, w2
 4028234:	f9477001 	ldr	x1, [x0, #3808]
 4028238:	f9003fe1 	str	x1, [sp, #120]
 402823c:	d2800001 	mov	x1, #0x0                   	// #0
 4028240:	910003e0 	mov	x0, sp
 4028244:	9100e3e1 	add	x1, sp, #0x38
 4028248:	94000a4a 	bl	402ab70 <ddrtrn_hal_set_cfg_addr>
 402824c:	2a1503e1 	mov	w1, w21
 4028250:	2a1403e0 	mov	w0, w20
 4028254:	94000c42 	bl	402b35c <ddrtrn_hw_training_init>
 4028258:	3100041f 	cmn	w0, #0x1
 402825c:	54000121 	b.ne	4028280 <bsp_ddrtrn_training_item+0x68>  // b.any
 4028260:	12800000 	mov	w0, #0xffffffff            	// #-1
 4028264:	b0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4028268:	f9403fe2 	ldr	x2, [sp, #120]
 402826c:	f9477023 	ldr	x3, [x1, #3808]
 4028270:	eb030042 	subs	x2, x2, x3
 4028274:	d2800003 	mov	x3, #0x0                   	// #0
 4028278:	54000140 	b.eq	40282a0 <bsp_ddrtrn_training_item+0x88>  // b.none
 402827c:	97fff7e0 	bl	40261fc <__stack_chk_fail>
 4028280:	2a1303e0 	mov	w0, w19
 4028284:	94000a28 	bl	402ab24 <ddrtrn_sw_training_if>
 4028288:	3100041f 	cmn	w0, #0x1
 402828c:	54fffea0 	b.eq	4028260 <bsp_ddrtrn_training_item+0x48>  // b.none
 4028290:	940019d5 	bl	402e9e4 <bsp_ddrtrn_suspend>
 4028294:	3100041f 	cmn	w0, #0x1
 4028298:	5a9f13e0 	csetm	w0, eq  // eq = none
 402829c:	17fffff2 	b	4028264 <bsp_ddrtrn_training_item+0x4c>
 40282a0:	a94853f3 	ldp	x19, x20, [sp, #128]
 40282a4:	a9497bf5 	ldp	x21, x30, [sp, #144]
 40282a8:	910283ff 	add	sp, sp, #0xa0
 40282ac:	d65f03c0 	ret

00000000040282b0 <bsp_ddrtrn_capat_adapt>:
 40282b0:	d10243ff 	sub	sp, sp, #0x90
 40282b4:	b0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 40282b8:	f90043fe 	str	x30, [sp, #128]
 40282bc:	f9477001 	ldr	x1, [x0, #3808]
 40282c0:	f9003fe1 	str	x1, [sp, #120]
 40282c4:	d2800001 	mov	x1, #0x0                   	// #0
 40282c8:	910003e0 	mov	x0, sp
 40282cc:	9100e3e1 	add	x1, sp, #0x38
 40282d0:	94000a28 	bl	402ab70 <ddrtrn_hal_set_cfg_addr>
 40282d4:	94000af5 	bl	402aea8 <ddrtrn_capat_adapt_func>
 40282d8:	b0ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 40282dc:	f9403fe2 	ldr	x2, [sp, #120]
 40282e0:	f9477023 	ldr	x3, [x1, #3808]
 40282e4:	eb030042 	subs	x2, x2, x3
 40282e8:	d2800003 	mov	x3, #0x0                   	// #0
 40282ec:	54000040 	b.eq	40282f4 <bsp_ddrtrn_capat_adapt+0x44>  // b.none
 40282f0:	97fff7c3 	bl	40261fc <__stack_chk_fail>
 40282f4:	f94043fe 	ldr	x30, [sp, #128]
 40282f8:	910243ff 	add	sp, sp, #0x90
 40282fc:	d65f03c0 	ret

0000000004028300 <serial_putc>:
 4028300:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4028304:	13001c34 	sxtb	w20, w1
 4028308:	2a0003f3 	mov	w19, w0
 402830c:	f9000bfe 	str	x30, [sp, #16]
 4028310:	97fffc2a 	bl	40273b8 <is_sec_dbg_lv_enable>
 4028314:	5292cb41 	mov	w1, #0x965a                	// #38490
 4028318:	72a794a1 	movk	w1, #0x3ca5, lsl #16
 402831c:	6b01001f 	cmp	w0, w1
 4028320:	54000201 	b.ne	4028360 <serial_putc+0x60>  // b.any
 4028324:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4028328:	6b00027f 	cmp	w19, w0
 402832c:	540001a1 	b.ne	4028360 <serial_putc+0x60>  // b.any
 4028330:	71002a9f 	cmp	w20, #0xa
 4028334:	54000081 	b.ne	4028344 <serial_putc+0x44>  // b.any
 4028338:	2a1303e0 	mov	w0, w19
 402833c:	528001a1 	mov	w1, #0xd                   	// #13
 4028340:	94000236 	bl	4028c18 <pl011_putc>
 4028344:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4028348:	2a1403e1 	mov	w1, w20
 402834c:	94000233 	bl	4028c18 <pl011_putc>
 4028350:	528014a0 	mov	w0, #0xa5                  	// #165
 4028354:	f9400bfe 	ldr	x30, [sp, #16]
 4028358:	a8c253f3 	ldp	x19, x20, [sp], #32
 402835c:	d65f03c0 	ret
 4028360:	52800b40 	mov	w0, #0x5a                  	// #90
 4028364:	17fffffc 	b	4028354 <serial_putc+0x54>

0000000004028368 <serial_puts>:
 4028368:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402836c:	aa0103f3 	mov	x19, x1
 4028370:	2a0003f4 	mov	w20, w0
 4028374:	f9000bfe 	str	x30, [sp, #16]
 4028378:	97fffc10 	bl	40273b8 <is_sec_dbg_lv_enable>
 402837c:	5292cb41 	mov	w1, #0x965a                	// #38490
 4028380:	72a794a1 	movk	w1, #0x3ca5, lsl #16
 4028384:	6b01001f 	cmp	w0, w1
 4028388:	540000a1 	b.ne	402839c <serial_puts+0x34>  // b.any
 402838c:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4028390:	6b00029f 	cmp	w20, w0
 4028394:	54000041 	b.ne	402839c <serial_puts+0x34>  // b.any
 4028398:	b50000d3 	cbnz	x19, 40283b0 <serial_puts+0x48>
 402839c:	52800b40 	mov	w0, #0x5a                  	// #90
 40283a0:	14000007 	b	40283bc <serial_puts+0x54>
 40283a4:	91000673 	add	x19, x19, #0x1
 40283a8:	52a22080 	mov	w0, #0x11040000            	// #285474816
 40283ac:	97ffffd5 	bl	4028300 <serial_putc>
 40283b0:	39c00261 	ldrsb	w1, [x19]
 40283b4:	35ffff81 	cbnz	w1, 40283a4 <serial_puts+0x3c>
 40283b8:	528014a0 	mov	w0, #0xa5                  	// #165
 40283bc:	f9400bfe 	ldr	x30, [sp, #16]
 40283c0:	a8c253f3 	ldp	x19, x20, [sp], #32
 40283c4:	d65f03c0 	ret

00000000040283c8 <serial_tstc>:
 40283c8:	52a22081 	mov	w1, #0x11040000            	// #285474816
 40283cc:	6b01001f 	cmp	w0, w1
 40283d0:	54000041 	b.ne	40283d8 <serial_tstc+0x10>  // b.any
 40283d4:	14000219 	b	4028c38 <pl011_tstc>
 40283d8:	12800000 	mov	w0, #0xffffffff            	// #-1
 40283dc:	d65f03c0 	ret

00000000040283e0 <serial_getc>:
 40283e0:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 40283e4:	aa0103f3 	mov	x19, x1
 40283e8:	52a22081 	mov	w1, #0x11040000            	// #285474816
 40283ec:	f9000bfe 	str	x30, [sp, #16]
 40283f0:	6b01001f 	cmp	w0, w1
 40283f4:	540000a0 	b.eq	4028408 <serial_getc+0x28>  // b.none
 40283f8:	52800b40 	mov	w0, #0x5a                  	// #90
 40283fc:	f9400bfe 	ldr	x30, [sp, #16]
 4028400:	a8c253f3 	ldp	x19, x20, [sp], #32
 4028404:	d65f03c0 	ret
 4028408:	b4ffff93 	cbz	x19, 40283f8 <serial_getc+0x18>
 402840c:	2a0003f4 	mov	w20, w0
 4028410:	9400020a 	bl	4028c38 <pl011_tstc>
 4028414:	34ffff20 	cbz	w0, 40283f8 <serial_getc+0x18>
 4028418:	2a1403e0 	mov	w0, w20
 402841c:	94000203 	bl	4028c28 <pl011_getc>
 4028420:	39000260 	strb	w0, [x19]
 4028424:	528014a0 	mov	w0, #0xa5                  	// #165
 4028428:	17fffff5 	b	40283fc <serial_getc+0x1c>

000000000402842c <serial_put_hex>:
 402842c:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4028430:	2a0003f4 	mov	w20, w0
 4028434:	f9000bfe 	str	x30, [sp, #16]
 4028438:	97fffbe0 	bl	40273b8 <is_sec_dbg_lv_enable>
 402843c:	5292cb41 	mov	w1, #0x965a                	// #38490
 4028440:	72a794a1 	movk	w1, #0x3ca5, lsl #16
 4028444:	6b01001f 	cmp	w0, w1
 4028448:	540001a1 	b.ne	402847c <serial_put_hex+0x50>  // b.any
 402844c:	52800393 	mov	w19, #0x1c                  	// #28
 4028450:	1ad32682 	lsr	w2, w20, w19
 4028454:	12000c42 	and	w2, w2, #0xf
 4028458:	7100245f 	cmp	w2, #0x9
 402845c:	1100c040 	add	w0, w2, #0x30
 4028460:	1100dc41 	add	w1, w2, #0x37
 4028464:	51001273 	sub	w19, w19, #0x4
 4028468:	1a808021 	csel	w1, w1, w0, hi  // hi = pmore
 402846c:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4028470:	97ffffa4 	bl	4028300 <serial_putc>
 4028474:	3100127f 	cmn	w19, #0x4
 4028478:	54fffec1 	b.ne	4028450 <serial_put_hex+0x24>  // b.any
 402847c:	f9400bfe 	ldr	x30, [sp, #16]
 4028480:	a8c253f3 	ldp	x19, x20, [sp], #32
 4028484:	d65f03c0 	ret

0000000004028488 <serial_put_dec>:
 4028488:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402848c:	2a0003f3 	mov	w19, w0
 4028490:	a9015bf5 	stp	x21, x22, [sp, #16]
 4028494:	f90013fe 	str	x30, [sp, #32]
 4028498:	97fffbc8 	bl	40273b8 <is_sec_dbg_lv_enable>
 402849c:	5292cb41 	mov	w1, #0x965a                	// #38490
 40284a0:	72a794a1 	movk	w1, #0x3ca5, lsl #16
 40284a4:	6b01001f 	cmp	w0, w1
 40284a8:	54000201 	b.ne	40284e8 <serial_put_dec+0x60>  // b.any
 40284ac:	36f80273 	tbz	w19, #31, 40284f8 <serial_put_dec+0x70>
 40284b0:	4b1303f5 	neg	w21, w19
 40284b4:	528005a1 	mov	w1, #0x2d                  	// #45
 40284b8:	52a22080 	mov	w0, #0x11040000            	// #285474816
 40284bc:	97ffff91 	bl	4028300 <serial_putc>
 40284c0:	52800034 	mov	w20, #0x1                   	// #1
 40284c4:	52800156 	mov	w22, #0xa                   	// #10
 40284c8:	1ad40aa1 	udiv	w1, w21, w20
 40284cc:	52a22080 	mov	w0, #0x11040000            	// #285474816
 40284d0:	1b14d421 	msub	w1, w1, w20, w21
 40284d4:	1b167e94 	mul	w20, w20, w22
 40284d8:	1100c021 	add	w1, w1, #0x30
 40284dc:	97ffff89 	bl	4028300 <serial_putc>
 40284e0:	6b13029f 	cmp	w20, w19
 40284e4:	54ffff23 	b.cc	40284c8 <serial_put_dec+0x40>  // b.lo, b.ul, b.last
 40284e8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40284ec:	f94013fe 	ldr	x30, [sp, #32]
 40284f0:	a8c353f3 	ldp	x19, x20, [sp], #48
 40284f4:	d65f03c0 	ret
 40284f8:	2a1303f5 	mov	w21, w19
 40284fc:	17fffff1 	b	40284c0 <serial_put_dec+0x38>

0000000004028500 <debug_puts>:
 4028500:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4028504:	2a0103f4 	mov	w20, w1
 4028508:	aa0003f3 	mov	x19, x0
 402850c:	a9017bf5 	stp	x21, x30, [sp, #16]
 4028510:	97fffbaa 	bl	40273b8 <is_sec_dbg_lv_enable>
 4028514:	5292cb41 	mov	w1, #0x965a                	// #38490
 4028518:	72a794a1 	movk	w1, #0x3ca5, lsl #16
 402851c:	6b01001f 	cmp	w0, w1
 4028520:	540005a1 	b.ne	40285d4 <debug_puts+0xd4>  // b.any
 4028524:	b0000055 	adrp	x21, 4031000 <sync_exception_sp_el0>
 4028528:	912e0eb5 	add	x21, x21, #0xb83
 402852c:	39400261 	ldrb	w1, [x19]
 4028530:	35000121 	cbnz	w1, 4028554 <debug_puts+0x54>
 4028534:	528001a1 	mov	w1, #0xd                   	// #13
 4028538:	52800000 	mov	w0, #0x0                   	// #0
 402853c:	97ffff71 	bl	4028300 <serial_putc>
 4028540:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4028544:	52800141 	mov	w1, #0xa                   	// #10
 4028548:	a8c253f3 	ldp	x19, x20, [sp], #32
 402854c:	52800000 	mov	w0, #0x0                   	// #0
 4028550:	17ffff6c 	b	4028300 <serial_putc>
 4028554:	7100943f 	cmp	w1, #0x25
 4028558:	540000a0 	b.eq	402856c <debug_puts+0x6c>  // b.none
 402855c:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4028560:	97ffff68 	bl	4028300 <serial_putc>
 4028564:	91000673 	add	x19, x19, #0x1
 4028568:	17fffff1 	b	402852c <debug_puts+0x2c>
 402856c:	39400660 	ldrb	w0, [x19, #1]
 4028570:	71008c1f 	cmp	w0, #0x23
 4028574:	540001c0 	b.eq	40285ac <debug_puts+0xac>  // b.none
 4028578:	91000673 	add	x19, x19, #0x1
 402857c:	39400260 	ldrb	w0, [x19]
 4028580:	7101901f 	cmp	w0, #0x64
 4028584:	54000220 	b.eq	40285c8 <debug_puts+0xc8>  // b.none
 4028588:	540001c8 	b.hi	40285c0 <debug_puts+0xc0>  // b.pmore
 402858c:	7101101f 	cmp	w0, #0x44
 4028590:	540001c0 	b.eq	40285c8 <debug_puts+0xc8>  // b.none
 4028594:	7101601f 	cmp	w0, #0x58
 4028598:	54fffe61 	b.ne	4028564 <debug_puts+0x64>  // b.any
 402859c:	91000673 	add	x19, x19, #0x1
 40285a0:	2a1403e0 	mov	w0, w20
 40285a4:	97ffffa2 	bl	402842c <serial_put_hex>
 40285a8:	17ffffef 	b	4028564 <debug_puts+0x64>
 40285ac:	aa1503e1 	mov	x1, x21
 40285b0:	52a22080 	mov	w0, #0x11040000            	// #285474816
 40285b4:	91000a73 	add	x19, x19, #0x2
 40285b8:	97ffff6c 	bl	4028368 <serial_puts>
 40285bc:	17fffff0 	b	402857c <debug_puts+0x7c>
 40285c0:	7101e01f 	cmp	w0, #0x78
 40285c4:	17fffff5 	b	4028598 <debug_puts+0x98>
 40285c8:	2a1403e0 	mov	w0, w20
 40285cc:	97ffffaf 	bl	4028488 <serial_put_dec>
 40285d0:	17ffffe5 	b	4028564 <debug_puts+0x64>
 40285d4:	a9417bf5 	ldp	x21, x30, [sp, #16]
 40285d8:	a8c253f3 	ldp	x19, x20, [sp], #32
 40285dc:	d65f03c0 	ret

00000000040285e0 <wait_uart_tx_busy_state>:
 40285e0:	52909000 	mov	w0, #0x8480                	// #33920
 40285e4:	d2800301 	mov	x1, #0x18                  	// #24
 40285e8:	72a003c0 	movk	w0, #0x1e, lsl #16
 40285ec:	f2a22081 	movk	x1, #0x1104, lsl #16
 40285f0:	b9400022 	ldr	w2, [x1]
 40285f4:	361800a2 	tbz	w2, #3, 4028608 <wait_uart_tx_busy_state+0x28>
 40285f8:	71000400 	subs	w0, w0, #0x1
 40285fc:	54ffffa1 	b.ne	40285f0 <wait_uart_tx_busy_state+0x10>  // b.any
 4028600:	52800b40 	mov	w0, #0x5a                  	// #90
 4028604:	d65f03c0 	ret
 4028608:	528014a0 	mov	w0, #0xa5                  	// #165
 402860c:	17fffffe 	b	4028604 <wait_uart_tx_busy_state+0x24>

0000000004028610 <send_board_type_to_uart>:
 4028610:	d10183ff 	sub	sp, sp, #0x60
 4028614:	d2800203 	mov	x3, #0x10                  	// #16
 4028618:	52800002 	mov	w2, #0x0                   	// #0
 402861c:	a90353f3 	stp	x19, x20, [sp, #48]
 4028620:	a9045bf5 	stp	x21, x22, [sp, #64]
 4028624:	2a0003f5 	mov	w21, w0
 4028628:	b0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402862c:	a9057bf7 	stp	x23, x30, [sp, #80]
 4028630:	f9477001 	ldr	x1, [x0, #3808]
 4028634:	f90017e1 	str	x1, [sp, #40]
 4028638:	d2800001 	mov	x1, #0x0                   	// #0
 402863c:	910023e0 	add	x0, sp, #0x8
 4028640:	aa0303e1 	mov	x1, x3
 4028644:	940001c1 	bl	4028d48 <memset_s>
 4028648:	350005e0 	cbnz	w0, 4028704 <send_board_type_to_uart+0xf4>
 402864c:	52800013 	mov	w19, #0x0                   	// #0
 4028650:	52800156 	mov	w22, #0xa                   	// #10
 4028654:	528001b7 	mov	w23, #0xd                   	// #13
 4028658:	97fffaa2 	bl	40270e0 <timer_get_divider>
 402865c:	1b007ed4 	mul	w20, w22, w0
 4028660:	9400018e 	bl	4028c98 <timer_start>
 4028664:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4028668:	97ffff58 	bl	40283c8 <serial_tstc>
 402866c:	34000460 	cbz	w0, 40286f8 <send_board_type_to_uart+0xe8>
 4028670:	91001fe1 	add	x1, sp, #0x7
 4028674:	52a22080 	mov	w0, #0x11040000            	// #285474816
 4028678:	97ffff5a 	bl	40283e0 <serial_getc>
 402867c:	39401fe0 	ldrb	w0, [sp, #7]
 4028680:	34000513 	cbz	w19, 4028720 <send_board_type_to_uart+0x110>
 4028684:	7100067f 	cmp	w19, #0x1
 4028688:	54000540 	b.eq	4028730 <send_board_type_to_uart+0x120>  // b.none
 402868c:	71000a7f 	cmp	w19, #0x2
 4028690:	54000560 	b.eq	402873c <send_board_type_to_uart+0x12c>  // b.none
 4028694:	35000053 	cbnz	w19, 402869c <send_board_type_to_uart+0x8c>
 4028698:	79001bf7 	strh	w23, [sp, #12]
 402869c:	79401be2 	ldrh	w2, [sp, #12]
 40286a0:	51000841 	sub	w1, w2, #0x2
 40286a4:	6b01027f 	cmp	w19, w1
 40286a8:	54000548 	b.hi	4028750 <send_board_type_to_uart+0x140>  // b.pmore
 40286ac:	794017e1 	ldrh	w1, [sp, #10]
 40286b0:	97fff9ec 	bl	4026e60 <cal_crc_perbyte>
 40286b4:	790017e0 	strh	w0, [sp, #10]
 40286b8:	79401be0 	ldrh	w0, [sp, #12]
 40286bc:	6b13001f 	cmp	w0, w19
 40286c0:	54000621 	b.ne	4028784 <send_board_type_to_uart+0x174>  // b.any
 40286c4:	d2800013 	mov	x19, #0x0                   	// #0
 40286c8:	910063f4 	add	x20, sp, #0x18
 40286cc:	aa1403e1 	mov	x1, x20
 40286d0:	2a1503e0 	mov	w0, w21
 40286d4:	52800162 	mov	w2, #0xb                   	// #11
 40286d8:	97ffea68 	bl	4023078 <build_board_type_frame>
 40286dc:	38736a81 	ldrb	w1, [x20, x19]
 40286e0:	52a22080 	mov	w0, #0x11040000            	// #285474816
 40286e4:	91000673 	add	x19, x19, #0x1
 40286e8:	97ffff06 	bl	4028300 <serial_putc>
 40286ec:	f1002e7f 	cmp	x19, #0xb
 40286f0:	54ffff61 	b.ne	40286dc <send_board_type_to_uart+0xcc>  // b.any
 40286f4:	14000004 	b	4028704 <send_board_type_to_uart+0xf4>
 40286f8:	9400016c 	bl	4028ca8 <timer_get_val>
 40286fc:	eb14001f 	cmp	x0, x20
 4028700:	54fffb23 	b.cc	4028664 <send_board_type_to_uart+0x54>  // b.lo, b.ul, b.last
 4028704:	b0ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4028708:	f94017e1 	ldr	x1, [sp, #40]
 402870c:	f9477002 	ldr	x2, [x0, #3808]
 4028710:	eb020021 	subs	x1, x1, x2
 4028714:	d2800002 	mov	x2, #0x0                   	// #0
 4028718:	540003a0 	b.eq	402878c <send_board_type_to_uart+0x17c>  // b.none
 402871c:	97fff6b8 	bl	40261fc <__stack_chk_fail>
 4028720:	7103381f 	cmp	w0, #0xce
 4028724:	54fffba0 	b.eq	4028698 <send_board_type_to_uart+0x88>  // b.none
 4028728:	52800013 	mov	w19, #0x0                   	// #0
 402872c:	17ffffcb 	b	4028658 <send_board_type_to_uart+0x48>
 4028730:	39003be0 	strb	w0, [sp, #14]
 4028734:	34fffb40 	cbz	w0, 402869c <send_board_type_to_uart+0x8c>
 4028738:	17fffffc 	b	4028728 <send_board_type_to_uart+0x118>
 402873c:	39403be1 	ldrb	w1, [sp, #14]
 4028740:	2a2103e1 	mvn	w1, w1
 4028744:	6b21001f 	cmp	w0, w1, uxtb
 4028748:	54fffaa0 	b.eq	402869c <send_board_type_to_uart+0x8c>  // b.none
 402874c:	17fffff7 	b	4028728 <send_board_type_to_uart+0x118>
 4028750:	4b130041 	sub	w1, w2, w19
 4028754:	531d7021 	lsl	w1, w1, #3
 4028758:	1ac12001 	lsl	w1, w0, w1
 402875c:	794013e0 	ldrh	w0, [sp, #8]
 4028760:	2a000021 	orr	w1, w1, w0
 4028764:	12003c21 	and	w1, w1, #0xffff
 4028768:	790013e1 	strh	w1, [sp, #8]
 402876c:	6b02027f 	cmp	w19, w2
 4028770:	540000a1 	b.ne	4028784 <send_board_type_to_uart+0x174>  // b.any
 4028774:	794017e0 	ldrh	w0, [sp, #10]
 4028778:	6b01001f 	cmp	w0, w1
 402877c:	54fffa40 	b.eq	40286c4 <send_board_type_to_uart+0xb4>  // b.none
 4028780:	17ffffea 	b	4028728 <send_board_type_to_uart+0x118>
 4028784:	11000673 	add	w19, w19, #0x1
 4028788:	17ffffb4 	b	4028658 <send_board_type_to_uart+0x48>
 402878c:	528014a0 	mov	w0, #0xa5                  	// #165
 4028790:	a94353f3 	ldp	x19, x20, [sp, #48]
 4028794:	a9445bf5 	ldp	x21, x22, [sp, #64]
 4028798:	a9457bf7 	ldp	x23, x30, [sp, #80]
 402879c:	910183ff 	add	sp, sp, #0x60
 40287a0:	d65f03c0 	ret

00000000040287a4 <tzasc_sec_config_read_back>:
 40287a4:	12001c00 	and	w0, w0, #0xff
 40287a8:	71003c1f 	cmp	w0, #0xf
 40287ac:	54000328 	b.hi	4028810 <tzasc_sec_config_read_back+0x6c>  // b.pmore
 40287b0:	52882203 	mov	w3, #0x4110                	// #16656
 40287b4:	72a02223 	movk	w3, #0x111, lsl #16
 40287b8:	0b030000 	add	w0, w0, w3
 40287bc:	531c6c03 	lsl	w3, w0, #4
 40287c0:	d37c6400 	ubfiz	x0, x0, #4, #26
 40287c4:	11040063 	add	w3, w3, #0x100
 40287c8:	93407c63 	sxtw	x3, w3
 40287cc:	b9400004 	ldr	w4, [x0]
 40287d0:	b9400060 	ldr	w0, [x3]
 40287d4:	92405483 	and	x3, x4, #0x3fffff
 40287d8:	d3405c00 	ubfx	x0, x0, #0, #24
 40287dc:	eb41307f 	cmp	x3, x1, lsr #12
 40287e0:	54000081 	b.ne	40287f0 <tzasc_sec_config_read_back+0x4c>  // b.any
 40287e4:	2a0003e0 	mov	w0, w0
 40287e8:	eb42301f 	cmp	x0, x2, lsr #12
 40287ec:	540000e0 	b.eq	4028808 <tzasc_sec_config_read_back+0x64>  // b.none
 40287f0:	f81f0ffe 	str	x30, [sp, #-16]!
 40287f4:	52800d80 	mov	w0, #0x6c                  	// #108
 40287f8:	97fff644 	bl	4026108 <err_print>
 40287fc:	52800b40 	mov	w0, #0x5a                  	// #90
 4028800:	f84107fe 	ldr	x30, [sp], #16
 4028804:	d65f03c0 	ret
 4028808:	528014a0 	mov	w0, #0xa5                  	// #165
 402880c:	d65f03c0 	ret
 4028810:	d2800000 	mov	x0, #0x0                   	// #0
 4028814:	b9400001 	ldr	w1, [x0]
 4028818:	b9400000 	ldr	w0, [x0]
 402881c:	d4207d00 	brk	#0x3e8

0000000004028820 <tzasc_bypass_disable>:
 4028820:	d2820080 	mov	x0, #0x1004                	// #4100
 4028824:	f2a22280 	movk	x0, #0x1114, lsl #16
 4028828:	b900001f 	str	wzr, [x0]
 402882c:	b9400000 	ldr	w0, [x0]
 4028830:	340000e0 	cbz	w0, 402884c <tzasc_bypass_disable+0x2c>
 4028834:	f81f0ffe 	str	x30, [sp, #-16]!
 4028838:	52800da0 	mov	w0, #0x6d                  	// #109
 402883c:	97fff633 	bl	4026108 <err_print>
 4028840:	52800b40 	mov	w0, #0x5a                  	// #90
 4028844:	f84107fe 	ldr	x30, [sp], #16
 4028848:	d65f03c0 	ret
 402884c:	528014a0 	mov	w0, #0xa5                  	// #165
 4028850:	d65f03c0 	ret

0000000004028854 <tzasc_rgn_enable>:
 4028854:	12001c00 	and	w0, w0, #0xff
 4028858:	71003c1f 	cmp	w0, #0xf
 402885c:	54000108 	b.hi	402887c <tzasc_rgn_enable+0x28>  // b.pmore
 4028860:	52882201 	mov	w1, #0x4110                	// #16656
 4028864:	72a02221 	movk	w1, #0x111, lsl #16
 4028868:	0b010000 	add	w0, w0, w1
 402886c:	d37c6400 	ubfiz	x0, x0, #4, #26
 4028870:	b9400001 	ldr	w1, [x0]
 4028874:	32010021 	orr	w1, w1, #0x80000000
 4028878:	b9000001 	str	w1, [x0]
 402887c:	d65f03c0 	ret

0000000004028880 <tzasc_set_rgn_map>:
 4028880:	12001c00 	and	w0, w0, #0xff
 4028884:	71003c1f 	cmp	w0, #0xf
 4028888:	54000208 	b.hi	40288c8 <tzasc_set_rgn_map+0x48>  // b.pmore
 402888c:	52882203 	mov	w3, #0x4110                	// #16656
 4028890:	d34c8421 	ubfx	x1, x1, #12, #22
 4028894:	72a02223 	movk	w3, #0x111, lsl #16
 4028898:	0b030000 	add	w0, w0, w3
 402889c:	d34c8c42 	ubfx	x2, x2, #12, #24
 40288a0:	531c6c03 	lsl	w3, w0, #4
 40288a4:	d37c6400 	ubfiz	x0, x0, #4, #26
 40288a8:	11040063 	add	w3, w3, #0x100
 40288ac:	b9400004 	ldr	w4, [x0]
 40288b0:	93407c63 	sxtw	x3, w3
 40288b4:	33005424 	bfxil	w4, w1, #0, #22
 40288b8:	b9000004 	str	w4, [x0]
 40288bc:	b9400060 	ldr	w0, [x3]
 40288c0:	33005c40 	bfxil	w0, w2, #0, #24
 40288c4:	b9000060 	str	w0, [x3]
 40288c8:	d65f03c0 	ret

00000000040288cc <tzasc_set_rgn_attr>:
 40288cc:	12001c00 	and	w0, w0, #0xff
 40288d0:	71003c1f 	cmp	w0, #0xf
 40288d4:	54000108 	b.hi	40288f4 <tzasc_set_rgn_attr+0x28>  // b.pmore
 40288d8:	52882002 	mov	w2, #0x4100                	// #16640
 40288dc:	72a02222 	movk	w2, #0x111, lsl #16
 40288e0:	0b020000 	add	w0, w0, w2
 40288e4:	531c6c00 	lsl	w0, w0, #4
 40288e8:	11041000 	add	w0, w0, #0x104
 40288ec:	93407c00 	sxtw	x0, w0
 40288f0:	b9000001 	str	w1, [x0]
 40288f4:	d65f03c0 	ret

00000000040288f8 <config_tzasc>:
 40288f8:	aa0203e4 	mov	x4, x2
 40288fc:	f81f0ffe 	str	x30, [sp, #-16]!
 4028900:	12001c05 	and	w5, w0, #0xff
 4028904:	2a0503e0 	mov	w0, w5
 4028908:	97fffff1 	bl	40288cc <tzasc_set_rgn_attr>
 402890c:	2a0503e0 	mov	w0, w5
 4028910:	aa0303e2 	mov	x2, x3
 4028914:	aa0403e1 	mov	x1, x4
 4028918:	97ffffda 	bl	4028880 <tzasc_set_rgn_map>
 402891c:	f84107fe 	ldr	x30, [sp], #16
 4028920:	2a0503e0 	mov	w0, w5
 4028924:	17ffffcc 	b	4028854 <tzasc_rgn_enable>

0000000004028928 <uapi_drv_cipher_init>:
 4028928:	d28a0000 	mov	x0, #0x5000                	// #20480
 402892c:	b9400000 	ldr	w0, [x0]
 4028930:	aa0003f0 	mov	x16, x0
 4028934:	d61f0200 	br	x16

0000000004028938 <uapi_drv_cipher_deinit>:
 4028938:	d28a0080 	mov	x0, #0x5004                	// #20484
 402893c:	b9400000 	ldr	w0, [x0]
 4028940:	aa0003f0 	mov	x16, x0
 4028944:	d61f0200 	br	x16

0000000004028948 <uapi_drv_cipher_create>:
 4028948:	d28a0103 	mov	x3, #0x5008                	// #20488
 402894c:	b9400063 	ldr	w3, [x3]
 4028950:	aa0303f0 	mov	x16, x3
 4028954:	d61f0200 	br	x16

0000000004028958 <uapi_drv_cipher_destroy>:
 4028958:	d28a0181 	mov	x1, #0x500c                	// #20492
 402895c:	b9400021 	ldr	w1, [x1]
 4028960:	aa0103f0 	mov	x16, x1
 4028964:	d61f0200 	br	x16

0000000004028968 <uapi_drv_cipher_dma_copy>:
 4028968:	d28a0204 	mov	x4, #0x5010                	// #20496
 402896c:	b9400084 	ldr	w4, [x4]
 4028970:	aa0403f0 	mov	x16, x4
 4028974:	d61f0200 	br	x16

0000000004028978 <uapi_drv_cipher_set_config>:
 4028978:	d28a0283 	mov	x3, #0x5014                	// #20500
 402897c:	b9400063 	ldr	w3, [x3]
 4028980:	aa0303f0 	mov	x16, x3
 4028984:	d61f0200 	br	x16

0000000004028988 <uapi_drv_cipher_attach>:
 4028988:	d28a0303 	mov	x3, #0x5018                	// #20504
 402898c:	b9400063 	ldr	w3, [x3]
 4028990:	aa0303f0 	mov	x16, x3
 4028994:	d61f0200 	br	x16

0000000004028998 <uapi_drv_cipher_detach>:
 4028998:	d28a0383 	mov	x3, #0x501c                	// #20508
 402899c:	b9400063 	ldr	w3, [x3]
 40289a0:	aa0303f0 	mov	x16, x3
 40289a4:	d61f0200 	br	x16

00000000040289a8 <uapi_drv_cipher_decrypt>:
 40289a8:	d28a0406 	mov	x6, #0x5020                	// #20512
 40289ac:	b94000c6 	ldr	w6, [x6]
 40289b0:	aa0603f0 	mov	x16, x6
 40289b4:	d61f0200 	br	x16

00000000040289b8 <uapi_drv_cipher_hash_init>:
 40289b8:	d28a0483 	mov	x3, #0x5024                	// #20516
 40289bc:	b9400063 	ldr	w3, [x3]
 40289c0:	aa0303f0 	mov	x16, x3
 40289c4:	d61f0200 	br	x16

00000000040289c8 <uapi_drv_cipher_hash_update>:
 40289c8:	d28a0505 	mov	x5, #0x5028                	// #20520
 40289cc:	b94000a5 	ldr	w5, [x5]
 40289d0:	aa0503f0 	mov	x16, x5
 40289d4:	d61f0200 	br	x16

00000000040289d8 <uapi_drv_cipher_hash_final>:
 40289d8:	d28a0584 	mov	x4, #0x502c                	// #20524
 40289dc:	b9400084 	ldr	w4, [x4]
 40289e0:	aa0403f0 	mov	x16, x4
 40289e4:	d61f0200 	br	x16

00000000040289e8 <uapi_drv_cipher_trng_get_random>:
 40289e8:	d28a0601 	mov	x1, #0x5030                	// #20528
 40289ec:	b9400021 	ldr	w1, [x1]
 40289f0:	aa0103f0 	mov	x16, x1
 40289f4:	d61f0200 	br	x16

00000000040289f8 <uapi_drv_cipher_register_cipher_wait_func>:
 40289f8:	d28a0684 	mov	x4, #0x5034                	// #20532
 40289fc:	b9400084 	ldr	w4, [x4]
 4028a00:	aa0403f0 	mov	x16, x4
 4028a04:	d61f0200 	br	x16

0000000004028a08 <uapi_drv_keyslot_init>:
 4028a08:	d28a0700 	mov	x0, #0x5038                	// #20536
 4028a0c:	b9400000 	ldr	w0, [x0]
 4028a10:	aa0003f0 	mov	x16, x0
 4028a14:	d61f0200 	br	x16

0000000004028a18 <uapi_drv_keyslot_deinit>:
 4028a18:	d28a0780 	mov	x0, #0x503c                	// #20540
 4028a1c:	b9400000 	ldr	w0, [x0]
 4028a20:	aa0003f0 	mov	x16, x0
 4028a24:	d61f0200 	br	x16

0000000004028a28 <uapi_drv_keyslot_create>:
 4028a28:	d28a0802 	mov	x2, #0x5040                	// #20544
 4028a2c:	b9400042 	ldr	w2, [x2]
 4028a30:	aa0203f0 	mov	x16, x2
 4028a34:	d61f0200 	br	x16

0000000004028a38 <uapi_drv_keyslot_destroy>:
 4028a38:	d28a0881 	mov	x1, #0x5044                	// #20548
 4028a3c:	b9400021 	ldr	w1, [x1]
 4028a40:	aa0103f0 	mov	x16, x1
 4028a44:	d61f0200 	br	x16

0000000004028a48 <uapi_drv_klad_create>:
 4028a48:	d28a0901 	mov	x1, #0x5048                	// #20552
 4028a4c:	b9400021 	ldr	w1, [x1]
 4028a50:	aa0103f0 	mov	x16, x1
 4028a54:	d61f0200 	br	x16

0000000004028a58 <uapi_drv_klad_destroy>:
 4028a58:	d28a0981 	mov	x1, #0x504c                	// #20556
 4028a5c:	b9400021 	ldr	w1, [x1]
 4028a60:	aa0103f0 	mov	x16, x1
 4028a64:	d61f0200 	br	x16

0000000004028a68 <uapi_drv_klad_attach>:
 4028a68:	d28a0a04 	mov	x4, #0x5050                	// #20560
 4028a6c:	b9400084 	ldr	w4, [x4]
 4028a70:	aa0403f0 	mov	x16, x4
 4028a74:	d61f0200 	br	x16

0000000004028a78 <uapi_drv_klad_detach>:
 4028a78:	d28a0a84 	mov	x4, #0x5054                	// #20564
 4028a7c:	b9400084 	ldr	w4, [x4]
 4028a80:	aa0403f0 	mov	x16, x4
 4028a84:	d61f0200 	br	x16

0000000004028a88 <uapi_drv_klad_set_attr>:
 4028a88:	d28a0b03 	mov	x3, #0x5058                	// #20568
 4028a8c:	b9400063 	ldr	w3, [x3]
 4028a90:	aa0303f0 	mov	x16, x3
 4028a94:	d61f0200 	br	x16

0000000004028a98 <uapi_drv_klad_set_session_key>:
 4028a98:	d28a0b83 	mov	x3, #0x505c                	// #20572
 4028a9c:	b9400063 	ldr	w3, [x3]
 4028aa0:	aa0303f0 	mov	x16, x3
 4028aa4:	d61f0200 	br	x16

0000000004028aa8 <uapi_drv_klad_set_content_key>:
 4028aa8:	d28a0c04 	mov	x4, #0x5060                	// #20576
 4028aac:	b9400084 	ldr	w4, [x4]
 4028ab0:	aa0403f0 	mov	x16, x4
 4028ab4:	d61f0200 	br	x16

0000000004028ab8 <uapi_drv_klad_set_clear_key>:
 4028ab8:	d28a2183 	mov	x3, #0x510c                	// #20748
 4028abc:	b9400063 	ldr	w3, [x3]
 4028ac0:	aa0303f0 	mov	x16, x3
 4028ac4:	d61f0200 	br	x16

0000000004028ac8 <uapi_drv_klad_rootkey_disable>:
 4028ac8:	d28a0c82 	mov	x2, #0x5064                	// #20580
 4028acc:	b9400042 	ldr	w2, [x2]
 4028ad0:	aa0203f0 	mov	x16, x2
 4028ad4:	d61f0200 	br	x16

0000000004028ad8 <uapi_drv_otp_read_word>:
 4028ad8:	d28a0d03 	mov	x3, #0x5068                	// #20584
 4028adc:	b9400063 	ldr	w3, [x3]
 4028ae0:	aa0303f0 	mov	x16, x3
 4028ae4:	d61f0200 	br	x16

0000000004028ae8 <uapi_drv_otp_read_byte>:
 4028ae8:	d28a0d83 	mov	x3, #0x506c                	// #20588
 4028aec:	b9400063 	ldr	w3, [x3]
 4028af0:	aa0303f0 	mov	x16, x3
 4028af4:	d61f0200 	br	x16

0000000004028af8 <uapi_drv_otp_get_die_id>:
 4028af8:	d28a0e03 	mov	x3, #0x5070                	// #20592
 4028afc:	b9400063 	ldr	w3, [x3]
 4028b00:	aa0303f0 	mov	x16, x3
 4028b04:	d61f0200 	br	x16

0000000004028b08 <uapi_drv_pke_init>:
 4028b08:	d28a0e80 	mov	x0, #0x5074                	// #20596
 4028b0c:	b9400000 	ldr	w0, [x0]
 4028b10:	aa0003f0 	mov	x16, x0
 4028b14:	d61f0200 	br	x16

0000000004028b18 <uapi_drv_pke_deinit>:
 4028b18:	d28a0f00 	mov	x0, #0x5078                	// #20600
 4028b1c:	b9400000 	ldr	w0, [x0]
 4028b20:	aa0003f0 	mov	x16, x0
 4028b24:	d61f0200 	br	x16

0000000004028b28 <uapi_drv_pke_ecdsa_verify>:
 4028b28:	d28a0f86 	mov	x6, #0x507c                	// #20604
 4028b2c:	b94000c6 	ldr	w6, [x6]
 4028b30:	aa0603f0 	mov	x16, x6
 4028b34:	d61f0200 	br	x16

0000000004028b38 <uapi_drv_pke_sm2_dsa_hash>:
 4028b38:	d28a1005 	mov	x5, #0x5080                	// #20608
 4028b3c:	b94000a5 	ldr	w5, [x5]
 4028b40:	aa0503f0 	mov	x16, x5
 4028b44:	d61f0200 	br	x16

0000000004028b48 <uapi_drv_register_func>:
 4028b48:	d28a1081 	mov	x1, #0x5084                	// #20612
 4028b4c:	b9400021 	ldr	w1, [x1]
 4028b50:	aa0103f0 	mov	x16, x1
 4028b54:	d61f0200 	br	x16

0000000004028b58 <mmc_init>:
 4028b58:	d28a1100 	mov	x0, #0x5088                	// #20616
 4028b5c:	b9400000 	ldr	w0, [x0]
 4028b60:	aa0003f0 	mov	x16, x0
 4028b64:	d61f0200 	br	x16

0000000004028b68 <_mmc_read>:
 4028b68:	d28a1184 	mov	x4, #0x508c                	// #20620
 4028b6c:	b9400084 	ldr	w4, [x4]
 4028b70:	aa0403f0 	mov	x16, x4
 4028b74:	d61f0200 	br	x16

0000000004028b78 <is_bootmode>:
 4028b78:	d28a1200 	mov	x0, #0x5090                	// #20624
 4028b7c:	b9400000 	ldr	w0, [x0]
 4028b80:	aa0003f0 	mov	x16, x0
 4028b84:	d61f0200 	br	x16

0000000004028b88 <mmc_get_cur_mode>:
 4028b88:	d28a1280 	mov	x0, #0x5094                	// #20628
 4028b8c:	b9400000 	ldr	w0, [x0]
 4028b90:	aa0003f0 	mov	x16, x0
 4028b94:	d61f0200 	br	x16

0000000004028b98 <mmc_set_bootup_mode>:
 4028b98:	d28a1301 	mov	x1, #0x5098                	// #20632
 4028b9c:	b9400021 	ldr	w1, [x1]
 4028ba0:	aa0103f0 	mov	x16, x1
 4028ba4:	d61f0200 	br	x16

0000000004028ba8 <copy_from_sdio>:
 4028ba8:	d28a1382 	mov	x2, #0x509c                	// #20636
 4028bac:	b9400042 	ldr	w2, [x2]
 4028bb0:	aa0203f0 	mov	x16, x2
 4028bb4:	d61f0200 	br	x16

0000000004028bb8 <set_sdio_pos>:
 4028bb8:	d28a1401 	mov	x1, #0x50a0                	// #20640
 4028bbc:	b9400021 	ldr	w1, [x1]
 4028bc0:	aa0103f0 	mov	x16, x1
 4028bc4:	d61f0200 	br	x16

0000000004028bc8 <enable_sdio_dma>:
 4028bc8:	d28a1480 	mov	x0, #0x50a4                	// #20644
 4028bcc:	b9400000 	ldr	w0, [x0]
 4028bd0:	aa0003f0 	mov	x16, x0
 4028bd4:	d61f0200 	br	x16

0000000004028bd8 <self_usb_check>:
 4028bd8:	d28a1500 	mov	x0, #0x50a8                	// #20648
 4028bdc:	b9400000 	ldr	w0, [x0]
 4028be0:	aa0003f0 	mov	x16, x0
 4028be4:	d61f0200 	br	x16

0000000004028be8 <copy_from_usb>:
 4028be8:	d28a1583 	mov	x3, #0x50ac                	// #20652
 4028bec:	b9400063 	ldr	w3, [x3]
 4028bf0:	aa0303f0 	mov	x16, x3
 4028bf4:	d61f0200 	br	x16

0000000004028bf8 <usb3_driver_init>:
 4028bf8:	d28a1600 	mov	x0, #0x50b0                	// #20656
 4028bfc:	b9400000 	ldr	w0, [x0]
 4028c00:	aa0003f0 	mov	x16, x0
 4028c04:	d61f0200 	br	x16

0000000004028c08 <send_to_usb>:
 4028c08:	d28a1682 	mov	x2, #0x50b4                	// #20660
 4028c0c:	b9400042 	ldr	w2, [x2]
 4028c10:	aa0203f0 	mov	x16, x2
 4028c14:	d61f0200 	br	x16

0000000004028c18 <pl011_putc>:
 4028c18:	d28a1702 	mov	x2, #0x50b8                	// #20664
 4028c1c:	b9400042 	ldr	w2, [x2]
 4028c20:	aa0203f0 	mov	x16, x2
 4028c24:	d61f0200 	br	x16

0000000004028c28 <pl011_getc>:
 4028c28:	d28a1781 	mov	x1, #0x50bc                	// #20668
 4028c2c:	b9400021 	ldr	w1, [x1]
 4028c30:	aa0103f0 	mov	x16, x1
 4028c34:	d61f0200 	br	x16

0000000004028c38 <pl011_tstc>:
 4028c38:	d28a1801 	mov	x1, #0x50c0                	// #20672
 4028c3c:	b9400021 	ldr	w1, [x1]
 4028c40:	aa0103f0 	mov	x16, x1
 4028c44:	d61f0200 	br	x16

0000000004028c48 <serial_init>:
 4028c48:	d28a1882 	mov	x2, #0x50c4                	// #20676
 4028c4c:	b9400042 	ldr	w2, [x2]
 4028c50:	aa0203f0 	mov	x16, x2
 4028c54:	d61f0200 	br	x16

0000000004028c58 <log_serial_puts>:
 4028c58:	d28a1901 	mov	x1, #0x50c8                	// #20680
 4028c5c:	b9400021 	ldr	w1, [x1]
 4028c60:	aa0103f0 	mov	x16, x1
 4028c64:	d61f0200 	br	x16

0000000004028c68 <copy_from_uart>:
 4028c68:	d28a1983 	mov	x3, #0x50cc                	// #20684
 4028c6c:	b9400063 	ldr	w3, [x3]
 4028c70:	aa0303f0 	mov	x16, x3
 4028c74:	d61f0200 	br	x16

0000000004028c78 <timer_init>:
 4028c78:	d28a1a00 	mov	x0, #0x50d0                	// #20688
 4028c7c:	b9400000 	ldr	w0, [x0]
 4028c80:	aa0003f0 	mov	x16, x0
 4028c84:	d61f0200 	br	x16

0000000004028c88 <timer_deinit>:
 4028c88:	d28a1a80 	mov	x0, #0x50d4                	// #20692
 4028c8c:	b9400000 	ldr	w0, [x0]
 4028c90:	aa0003f0 	mov	x16, x0
 4028c94:	d61f0200 	br	x16

0000000004028c98 <timer_start>:
 4028c98:	d28a1b00 	mov	x0, #0x50d8                	// #20696
 4028c9c:	b9400000 	ldr	w0, [x0]
 4028ca0:	aa0003f0 	mov	x16, x0
 4028ca4:	d61f0200 	br	x16

0000000004028ca8 <timer_get_val>:
 4028ca8:	d28a1b80 	mov	x0, #0x50dc                	// #20700
 4028cac:	b9400000 	ldr	w0, [x0]
 4028cb0:	aa0003f0 	mov	x16, x0
 4028cb4:	d61f0200 	br	x16

0000000004028cb8 <udelay>:
 4028cb8:	d28a1c01 	mov	x1, #0x50e0                	// #20704
 4028cbc:	b9400021 	ldr	w1, [x1]
 4028cc0:	aa0103f0 	mov	x16, x1
 4028cc4:	d61f0200 	br	x16

0000000004028cc8 <mdelay>:
 4028cc8:	d28a1c81 	mov	x1, #0x50e4                	// #20708
 4028ccc:	b9400021 	ldr	w1, [x1]
 4028cd0:	aa0103f0 	mov	x16, x1
 4028cd4:	d61f0200 	br	x16

0000000004028cd8 <watchdog_enable>:
 4028cd8:	d28a1d03 	mov	x3, #0x50e8                	// #20712
 4028cdc:	b9400063 	ldr	w3, [x3]
 4028ce0:	aa0303f0 	mov	x16, x3
 4028ce4:	d61f0200 	br	x16

0000000004028ce8 <watchdog_disable>:
 4028ce8:	d28a1d81 	mov	x1, #0x50ec                	// #20716
 4028cec:	b9400021 	ldr	w1, [x1]
 4028cf0:	aa0103f0 	mov	x16, x1
 4028cf4:	d61f0200 	br	x16

0000000004028cf8 <watchdog_feed>:
 4028cf8:	d28a1e02 	mov	x2, #0x50f0                	// #20720
 4028cfc:	b9400042 	ldr	w2, [x2]
 4028d00:	aa0203f0 	mov	x16, x2
 4028d04:	d61f0200 	br	x16

0000000004028d08 <memcmp_ss>:
 4028d08:	d28a1e84 	mov	x4, #0x50f4                	// #20724
 4028d0c:	b9400084 	ldr	w4, [x4]
 4028d10:	aa0403f0 	mov	x16, x4
 4028d14:	d61f0200 	br	x16

0000000004028d18 <memcpy_ss>:
 4028d18:	d28a1f05 	mov	x5, #0x50f8                	// #20728
 4028d1c:	b94000a5 	ldr	w5, [x5]
 4028d20:	aa0503f0 	mov	x16, x5
 4028d24:	d61f0200 	br	x16

0000000004028d28 <memset_ss>:
 4028d28:	d28a1f85 	mov	x5, #0x50fc                	// #20732
 4028d2c:	b94000a5 	ldr	w5, [x5]
 4028d30:	aa0503f0 	mov	x16, x5
 4028d34:	d61f0200 	br	x16

0000000004028d38 <memcpy_s>:
 4028d38:	d28a2004 	mov	x4, #0x5100                	// #20736
 4028d3c:	b9400084 	ldr	w4, [x4]
 4028d40:	aa0403f0 	mov	x16, x4
 4028d44:	d61f0200 	br	x16

0000000004028d48 <memset_s>:
 4028d48:	d28a2084 	mov	x4, #0x5104                	// #20740
 4028d4c:	b9400084 	ldr	w4, [x4]
 4028d50:	aa0403f0 	mov	x16, x4
 4028d54:	d61f0200 	br	x16

0000000004028d58 <save_cur_point_syscnt>:
 4028d58:	d28a2100 	mov	x0, #0x5108                	// #20744
 4028d5c:	b9400000 	ldr	w0, [x0]
 4028d60:	aa0003f0 	mov	x16, x0
 4028d64:	d61f0200 	br	x16

0000000004028d68 <self_sdio_check>:
 4028d68:	d2914d00 	mov	x0, #0x8a68                	// #35432
 4028d6c:	aa0003f0 	mov	x16, x0
 4028d70:	d61f0200 	br	x16

0000000004028d74 <ipc_send_msg>:
 4028d74:	7100201f 	cmp	w0, #0x8
 4028d78:	540003e8 	b.hi	4028df4 <ipc_send_msg+0x80>  // b.pmore
 4028d7c:	b40003c1 	cbz	x1, 4028df4 <ipc_send_msg+0x80>
 4028d80:	b9400422 	ldr	w2, [x1, #4]
 4028d84:	7100345f 	cmp	w2, #0xd
 4028d88:	54000368 	b.hi	4028df4 <ipc_send_msg+0x80>  // b.pmore
 4028d8c:	d29c0405 	mov	x5, #0xe020                	// #57376
 4028d90:	b9400022 	ldr	w2, [x1]
 4028d94:	f2a22065 	movk	x5, #0x1103, lsl #16
 4028d98:	d29c0484 	mov	x4, #0xe024                	// #57380
 4028d9c:	f2a22064 	movk	x4, #0x1103, lsl #16
 4028da0:	b90000a2 	str	w2, [x5]
 4028da4:	b9400423 	ldr	w3, [x1, #4]
 4028da8:	b9000083 	str	w3, [x4]
 4028dac:	4a030042 	eor	w2, w2, w3
 4028db0:	d2800023 	mov	x3, #0x1                   	// #1
 4028db4:	91000463 	add	x3, x3, #0x1
 4028db8:	b9400426 	ldr	w6, [x1, #4]
 4028dbc:	51000864 	sub	w4, w3, #0x2
 4028dc0:	6b0400df 	cmp	w6, w4
 4028dc4:	540000e8 	b.hi	4028de0 <ipc_send_msg+0x6c>  // b.pmore
 4028dc8:	d29c0b81 	mov	x1, #0xe05c                	// #57436
 4028dcc:	f2a22061 	movk	x1, #0x1103, lsl #16
 4028dd0:	b9000022 	str	w2, [x1]
 4028dd4:	b81a4020 	stur	w0, [x1, #-92]
 4028dd8:	52800000 	mov	w0, #0x0                   	// #0
 4028ddc:	d65f03c0 	ret
 4028de0:	d37ef464 	lsl	x4, x3, #2
 4028de4:	b8637826 	ldr	w6, [x1, x3, lsl #2]
 4028de8:	4a060042 	eor	w2, w2, w6
 4028dec:	b82468a6 	str	w6, [x5, x4]
 4028df0:	17fffff1 	b	4028db4 <ipc_send_msg+0x40>
 4028df4:	12800000 	mov	w0, #0xffffffff            	// #-1
 4028df8:	17fffff9 	b	4028ddc <ipc_send_msg+0x68>

0000000004028dfc <ipc_recv_msg>:
 4028dfc:	b4000481 	cbz	x1, 4028e8c <ipc_recv_msg+0x90>
 4028e00:	7100201f 	cmp	w0, #0x8
 4028e04:	54000448 	b.hi	4028e8c <ipc_recv_msg+0x90>  // b.pmore
 4028e08:	34000102 	cbz	w2, 4028e28 <ipc_recv_msg+0x2c>
 4028e0c:	52800022 	mov	w2, #0x1                   	// #1
 4028e10:	d29c0104 	mov	x4, #0xe008                	// #57352
 4028e14:	1ac02042 	lsl	w2, w2, w0
 4028e18:	f2a22064 	movk	x4, #0x1103, lsl #16
 4028e1c:	b9400083 	ldr	w3, [x4]
 4028e20:	6a02007f 	tst	w3, w2
 4028e24:	54ffffc0 	b.eq	4028e1c <ipc_recv_msg+0x20>  // b.none
 4028e28:	d29c0405 	mov	x5, #0xe020                	// #57376
 4028e2c:	d29c0483 	mov	x3, #0xe024                	// #57380
 4028e30:	f2a22065 	movk	x5, #0x1103, lsl #16
 4028e34:	f2a22063 	movk	x3, #0x1103, lsl #16
 4028e38:	b94000a2 	ldr	w2, [x5]
 4028e3c:	b9000022 	str	w2, [x1]
 4028e40:	b9400064 	ldr	w4, [x3]
 4028e44:	b9000424 	str	w4, [x1, #4]
 4028e48:	4a040042 	eor	w2, w2, w4
 4028e4c:	7100349f 	cmp	w4, #0xd
 4028e50:	54000308 	b.hi	4028eb0 <ipc_recv_msg+0xb4>  // b.pmore
 4028e54:	d2800023 	mov	x3, #0x1                   	// #1
 4028e58:	91000463 	add	x3, x3, #0x1
 4028e5c:	51000866 	sub	w6, w3, #0x2
 4028e60:	6b0400df 	cmp	w6, w4
 4028e64:	54000183 	b.cc	4028e94 <ipc_recv_msg+0x98>  // b.lo, b.ul, b.last
 4028e68:	d29c0b83 	mov	x3, #0xe05c                	// #57436
 4028e6c:	f2a22063 	movk	x3, #0x1103, lsl #16
 4028e70:	b9400063 	ldr	w3, [x3]
 4028e74:	b9003c23 	str	w3, [x1, #60]
 4028e78:	6b03005f 	cmp	w2, w3
 4028e7c:	d29c0082 	mov	x2, #0xe004                	// #57348
 4028e80:	f2a22062 	movk	x2, #0x1103, lsl #16
 4028e84:	b9000040 	str	w0, [x2]
 4028e88:	54000100 	b.eq	4028ea8 <ipc_recv_msg+0xac>  // b.none
 4028e8c:	12800000 	mov	w0, #0xffffffff            	// #-1
 4028e90:	14000007 	b	4028eac <ipc_recv_msg+0xb0>
 4028e94:	d37ef466 	lsl	x6, x3, #2
 4028e98:	b86668a6 	ldr	w6, [x5, x6]
 4028e9c:	b8237826 	str	w6, [x1, x3, lsl #2]
 4028ea0:	4a060042 	eor	w2, w2, w6
 4028ea4:	17ffffed 	b	4028e58 <ipc_recv_msg+0x5c>
 4028ea8:	b9400420 	ldr	w0, [x1, #4]
 4028eac:	d65f03c0 	ret
 4028eb0:	d29c0081 	mov	x1, #0xe004                	// #57348
 4028eb4:	f2a22061 	movk	x1, #0x1103, lsl #16
 4028eb8:	b9000020 	str	w0, [x1]
 4028ebc:	17fffff4 	b	4028e8c <ipc_recv_msg+0x90>

0000000004028ec0 <delay_cl>:
 4028ec0:	53017c00 	lsr	w0, w0, #1
 4028ec4:	35000040 	cbnz	w0, 4028ecc <delay_cl+0xc>
 4028ec8:	d65f03c0 	ret
 4028ecc:	d503201f 	nop
 4028ed0:	51000400 	sub	w0, w0, #0x1
 4028ed4:	17fffffc 	b	4028ec4 <delay_cl+0x4>

0000000004028ed8 <get_ddr_scrub_and_gpll_switch>:
 4028ed8:	d2802480 	mov	x0, #0x124                 	// #292
 4028edc:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4028ee0:	b9400000 	ldr	w0, [x0]
 4028ee4:	d3451400 	ubfx	x0, x0, #5, #1
 4028ee8:	d65f03c0 	ret

0000000004028eec <bsp_gpll_config>:
 4028eec:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 4028ef0:	2a0003f3 	mov	w19, w0
 4028ef4:	97fffff9 	bl	4028ed8 <get_ddr_scrub_and_gpll_switch>
 4028ef8:	12001c00 	and	w0, w0, #0xff
 4028efc:	7100041f 	cmp	w0, #0x1
 4028f00:	54001341 	b.ne	4029168 <bsp_gpll_config+0x27c>  // b.any
 4028f04:	d2800380 	mov	x0, #0x1c                  	// #28
 4028f08:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4028f0c:	b9400001 	ldr	w1, [x0]
 4028f10:	d2840800 	mov	x0, #0x2040                	// #8256
 4028f14:	f2a22020 	movk	x0, #0x1101, lsl #16
 4028f18:	37c00261 	tbnz	w1, #24, 4028f64 <bsp_gpll_config+0x78>
 4028f1c:	b9400001 	ldr	w1, [x0]
 4028f20:	12126421 	and	w1, w1, #0xffffc0ff
 4028f24:	b9000001 	str	w1, [x0]
 4028f28:	d2840001 	mov	x1, #0x2000                	// #8192
 4028f2c:	f2a22021 	movk	x1, #0x1101, lsl #16
 4028f30:	b9400020 	ldr	w0, [x1]
 4028f34:	12117000 	and	w0, w0, #0xffff8fff
 4028f38:	32130400 	orr	w0, w0, #0x6000
 4028f3c:	b9000020 	str	w0, [x1]
 4028f40:	52896f00 	mov	w0, #0x4b78                	// #19320
 4028f44:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 4028f48:	6b00027f 	cmp	w19, w0
 4028f4c:	54000141 	b.ne	4028f74 <bsp_gpll_config+0x88>  // b.any
 4028f50:	d287e801 	mov	x1, #0x3f40                	// #16192
 4028f54:	f2a22021 	movk	x1, #0x1101, lsl #16
 4028f58:	b9400020 	ldr	w0, [x1]
 4028f5c:	121b7800 	and	w0, w0, #0xffffffef
 4028f60:	14000057 	b	40290bc <bsp_gpll_config+0x1d0>
 4028f64:	b9400001 	ldr	w1, [x0]
 4028f68:	12126421 	and	w1, w1, #0xffffc0ff
 4028f6c:	b9000001 	str	w1, [x0]
 4028f70:	17fffff4 	b	4028f40 <bsp_gpll_config+0x54>
 4028f74:	529690e0 	mov	w0, #0xb487                	// #46215
 4028f78:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4028f7c:	6b00027f 	cmp	w19, w0
 4028f80:	54000960 	b.eq	40290ac <bsp_gpll_config+0x1c0>  // b.none
 4028f84:	52968f00 	mov	w0, #0xb478                	// #46200
 4028f88:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4028f8c:	6b00027f 	cmp	w19, w0
 4028f90:	540009a0 	b.eq	40290c4 <bsp_gpll_config+0x1d8>  // b.none
 4028f94:	d2806081 	mov	x1, #0x304                 	// #772
 4028f98:	d2806004 	mov	x4, #0x300                 	// #768
 4028f9c:	d2806705 	mov	x5, #0x338                 	// #824
 4028fa0:	52800062 	mov	w2, #0x3                   	// #3
 4028fa4:	f2a22021 	movk	x1, #0x1101, lsl #16
 4028fa8:	f2a22024 	movk	x4, #0x1101, lsl #16
 4028fac:	52a25006 	mov	w6, #0x12800000            	// #310378496
 4028fb0:	f2a22025 	movk	x5, #0x1101, lsl #16
 4028fb4:	52800280 	mov	w0, #0x14                  	// #20
 4028fb8:	97ffffc2 	bl	4028ec0 <delay_cl>
 4028fbc:	b9400020 	ldr	w0, [x1]
 4028fc0:	52800203 	mov	w3, #0x10                  	// #16
 4028fc4:	320c0000 	orr	w0, w0, #0x100000
 4028fc8:	b9000020 	str	w0, [x1]
 4028fcc:	b9000086 	str	w6, [x4]
 4028fd0:	52800020 	mov	w0, #0x1                   	// #1
 4028fd4:	97ffffbb 	bl	4028ec0 <delay_cl>
 4028fd8:	b9400020 	ldr	w0, [x1]
 4028fdc:	120b7800 	and	w0, w0, #0xffefffff
 4028fe0:	b9000020 	str	w0, [x1]
 4028fe4:	52800280 	mov	w0, #0x14                  	// #20
 4028fe8:	97ffffb6 	bl	4028ec0 <delay_cl>
 4028fec:	b94000a0 	ldr	w0, [x5]
 4028ff0:	372002c0 	tbnz	w0, #4, 4029048 <bsp_gpll_config+0x15c>
 4028ff4:	52800020 	mov	w0, #0x1                   	// #1
 4028ff8:	97ffffb2 	bl	4028ec0 <delay_cl>
 4028ffc:	71000463 	subs	w3, w3, #0x1
 4029000:	54ffff61 	b.ne	4028fec <bsp_gpll_config+0x100>  // b.any
 4029004:	71000442 	subs	w2, w2, #0x1
 4029008:	54fffda1 	b.ne	4028fbc <bsp_gpll_config+0xd0>  // b.any
 402900c:	52968f00 	mov	w0, #0xb478                	// #46200
 4029010:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4029014:	6b00027f 	cmp	w19, w0
 4029018:	54000060 	b.eq	4029024 <bsp_gpll_config+0x138>  // b.none
 402901c:	52800b00 	mov	w0, #0x58                  	// #88
 4029020:	97fff43a 	bl	4026108 <err_print>
 4029024:	d2800040 	mov	x0, #0x2                   	// #2
 4029028:	97ffff28 	bl	4028cc8 <mdelay>
 402902c:	97ffff17 	bl	4028c88 <timer_deinit>
 4029030:	d2800080 	mov	x0, #0x4                   	// #4
 4029034:	52800021 	mov	w1, #0x1                   	// #1
 4029038:	f2a22040 	movk	x0, #0x1102, lsl #16
 402903c:	b9000001 	str	w1, [x0]
 4029040:	b9000001 	str	w1, [x0]
 4029044:	b9000001 	str	w1, [x0]
 4029048:	d2800380 	mov	x0, #0x1c                  	// #28
 402904c:	d2840801 	mov	x1, #0x2040                	// #8256
 4029050:	f2a203c0 	movk	x0, #0x101e, lsl #16
 4029054:	f2a22021 	movk	x1, #0x1101, lsl #16
 4029058:	b9400000 	ldr	w0, [x0]
 402905c:	37c00540 	tbnz	w0, #24, 4029104 <bsp_gpll_config+0x218>
 4029060:	b9400020 	ldr	w0, [x1]
 4029064:	52822002 	mov	w2, #0x1100                	// #4352
 4029068:	12126400 	and	w0, w0, #0xffffc0ff
 402906c:	2a020000 	orr	w0, w0, w2
 4029070:	b9000020 	str	w0, [x1]
 4029074:	528a0002 	mov	w2, #0x5000                	// #20480
 4029078:	b85c0020 	ldur	w0, [x1, #-64]
 402907c:	12117000 	and	w0, w0, #0xffff8fff
 4029080:	2a020000 	orr	w0, w0, w2
 4029084:	b81c0020 	stur	w0, [x1, #-64]
 4029088:	52896f00 	mov	w0, #0x4b78                	// #19320
 402908c:	72b2d4a0 	movk	w0, #0x96a5, lsl #16
 4029090:	6b00027f 	cmp	w19, w0
 4029094:	54000441 	b.ne	402911c <bsp_gpll_config+0x230>  // b.any
 4029098:	d287e801 	mov	x1, #0x3f40                	// #16192
 402909c:	f2a22021 	movk	x1, #0x1101, lsl #16
 40290a0:	b9400020 	ldr	w0, [x1]
 40290a4:	321c0000 	orr	w0, w0, #0x10
 40290a8:	1400002c 	b	4029158 <bsp_gpll_config+0x26c>
 40290ac:	d2869801 	mov	x1, #0x34c0                	// #13504
 40290b0:	f2a22021 	movk	x1, #0x1101, lsl #16
 40290b4:	b9400020 	ldr	w0, [x1]
 40290b8:	121f7800 	and	w0, w0, #0xfffffffe
 40290bc:	b9000020 	str	w0, [x1]
 40290c0:	17ffffb5 	b	4028f94 <bsp_gpll_config+0xa8>
 40290c4:	97fffd47 	bl	40285e0 <wait_uart_tx_busy_state>
 40290c8:	7102941f 	cmp	w0, #0xa5
 40290cc:	54000180 	b.eq	40290fc <bsp_gpll_config+0x210>  // b.none
 40290d0:	52800ae0 	mov	w0, #0x57                  	// #87
 40290d4:	97fff40d 	bl	4026108 <err_print>
 40290d8:	d2800040 	mov	x0, #0x2                   	// #2
 40290dc:	97fffefb 	bl	4028cc8 <mdelay>
 40290e0:	97fffeea 	bl	4028c88 <timer_deinit>
 40290e4:	d2800080 	mov	x0, #0x4                   	// #4
 40290e8:	52800021 	mov	w1, #0x1                   	// #1
 40290ec:	f2a22040 	movk	x0, #0x1102, lsl #16
 40290f0:	b9000001 	str	w1, [x0]
 40290f4:	b9000001 	str	w1, [x0]
 40290f8:	b9000001 	str	w1, [x0]
 40290fc:	d2883001 	mov	x1, #0x4180                	// #16768
 4029100:	17ffff95 	b	4028f54 <bsp_gpll_config+0x68>
 4029104:	b9400020 	ldr	w0, [x1]
 4029108:	52828002 	mov	w2, #0x1400                	// #5120
 402910c:	12126400 	and	w0, w0, #0xffffc0ff
 4029110:	2a020000 	orr	w0, w0, w2
 4029114:	b9000020 	str	w0, [x1]
 4029118:	17ffffdc 	b	4029088 <bsp_gpll_config+0x19c>
 402911c:	529690e0 	mov	w0, #0xb487                	// #46215
 4029120:	72ad2b40 	movk	w0, #0x695a, lsl #16
 4029124:	6b00027f 	cmp	w19, w0
 4029128:	54000100 	b.eq	4029148 <bsp_gpll_config+0x25c>  // b.none
 402912c:	52968f00 	mov	w0, #0xb478                	// #46200
 4029130:	72ad34a0 	movk	w0, #0x69a5, lsl #16
 4029134:	6b00027f 	cmp	w19, w0
 4029138:	54000140 	b.eq	4029160 <bsp_gpll_config+0x274>  // b.none
 402913c:	a8c17bf3 	ldp	x19, x30, [sp], #16
 4029140:	52800280 	mov	w0, #0x14                  	// #20
 4029144:	17ffff5f 	b	4028ec0 <delay_cl>
 4029148:	d2869801 	mov	x1, #0x34c0                	// #13504
 402914c:	f2a22021 	movk	x1, #0x1101, lsl #16
 4029150:	b9400020 	ldr	w0, [x1]
 4029154:	32000000 	orr	w0, w0, #0x1
 4029158:	b9000020 	str	w0, [x1]
 402915c:	17fffff8 	b	402913c <bsp_gpll_config+0x250>
 4029160:	d2883001 	mov	x1, #0x4180                	// #16768
 4029164:	17ffffce 	b	402909c <bsp_gpll_config+0x1b0>
 4029168:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402916c:	d65f03c0 	ret

0000000004029170 <set_addr_reg>:
 4029170:	b0ffffc1 	adrp	x1, 4022000 <malloc_sizes+0xf0>
 4029174:	9100c022 	add	x2, x1, #0x30
 4029178:	d2800503 	mov	x3, #0x28                  	// #40
 402917c:	b9403021 	ldr	w1, [x1, #48]
 4029180:	f2a20003 	movk	x3, #0x1000, lsl #16
 4029184:	b9400442 	ldr	w2, [x2, #4]
 4029188:	4b020021 	sub	w1, w1, w2
 402918c:	1ac12402 	lsr	w2, w0, w1
 4029190:	1ac12041 	lsl	w1, w2, w1
 4029194:	4b010000 	sub	w0, w0, w1
 4029198:	d2800581 	mov	x1, #0x2c                  	// #44
 402919c:	d34a4444 	ubfx	x4, x2, #10, #8
 40291a0:	f2a20001 	movk	x1, #0x1000, lsl #16
 40291a4:	53101400 	ubfiz	w0, w0, #16, #6
 40291a8:	b9000064 	str	w4, [x3]
 40291ac:	2a025800 	orr	w0, w0, w2, lsl #22
 40291b0:	b9000020 	str	w0, [x1]
 40291b4:	d65f03c0 	ret

00000000040291b8 <set_dma_addr_reg>:
 40291b8:	d2800981 	mov	x1, #0x4c                  	// #76
 40291bc:	d3608402 	ubfx	x2, x0, #32, #2
 40291c0:	f2a20001 	movk	x1, #0x1000, lsl #16
 40291c4:	b9000020 	str	w0, [x1]
 40291c8:	b901b422 	str	w2, [x1, #436]
 40291cc:	b0ffffc1 	adrp	x1, 4022000 <malloc_sizes+0xf0>
 40291d0:	b9403821 	ldr	w1, [x1, #56]
 40291d4:	8b000020 	add	x0, x1, x0
 40291d8:	d2800b81 	mov	x1, #0x5c                  	// #92
 40291dc:	f2a20001 	movk	x1, #0x1000, lsl #16
 40291e0:	b9000020 	str	w0, [x1]
 40291e4:	d3608400 	ubfx	x0, x0, #32, #2
 40291e8:	b901b420 	str	w0, [x1, #436]
 40291ec:	d65f03c0 	ret

00000000040291f0 <spi_nand_feature_op>:
 40291f0:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 40291f4:	12001c21 	and	w1, w1, #0xff
 40291f8:	12001c14 	and	w20, w0, #0xff
 40291fc:	7103003f 	cmp	w1, #0xc0
 4029200:	aa0203f3 	mov	x19, x2
 4029204:	1a9f17e0 	cset	w0, eq  // eq = none
 4029208:	52000282 	eor	w2, w20, #0x1
 402920c:	a9015bf5 	stp	x21, x22, [sp, #16]
 4029210:	a9027bf7 	stp	x23, x30, [sp, #32]
 4029214:	6a00005f 	tst	w2, w0
 4029218:	540003e0 	b.eq	4029294 <spi_nand_feature_op+0xa4>  // b.none
 402921c:	d2800600 	mov	x0, #0x30                  	// #48
 4029220:	d2800795 	mov	x21, #0x3c                  	// #60
 4029224:	f2a20000 	movk	x0, #0x1000, lsl #16
 4029228:	f2a20015 	movk	x21, #0x1000, lsl #16
 402922c:	52840001 	mov	w1, #0x2000                	// #8192
 4029230:	5290d414 	mov	w20, #0x86a0                	// #34464
 4029234:	b9000001 	str	w1, [x0]
 4029238:	72a00034 	movk	w20, #0x1, lsl #16
 402923c:	52800060 	mov	w0, #0x3                   	// #3
 4029240:	b90002a0 	str	w0, [x21]
 4029244:	d2800140 	mov	x0, #0xa                   	// #10
 4029248:	97fffe9c 	bl	4028cb8 <udelay>
 402924c:	b94002a0 	ldr	w0, [x21]
 4029250:	51000694 	sub	w20, w20, #0x1
 4029254:	360000c0 	tbz	w0, #0, 402926c <spi_nand_feature_op+0x7c>
 4029258:	35ffff74 	cbnz	w20, 4029244 <spi_nand_feature_op+0x54>
 402925c:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029260:	91351800 	add	x0, x0, #0xd46
 4029264:	97fffe7d 	bl	4028c58 <log_serial_puts>
 4029268:	14000002 	b	4029270 <spi_nand_feature_op+0x80>
 402926c:	34ffff94 	cbz	w20, 402925c <spi_nand_feature_op+0x6c>
 4029270:	d2801580 	mov	x0, #0xac                  	// #172
 4029274:	f2a20000 	movk	x0, #0x1000, lsl #16
 4029278:	b9400000 	ldr	w0, [x0]
 402927c:	b9000260 	str	w0, [x19]
 4029280:	12800b40 	mov	w0, #0xffffffa5            	// #-91
 4029284:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4029288:	a9427bf7 	ldp	x23, x30, [sp, #32]
 402928c:	a8c353f3 	ldp	x19, x20, [sp], #48
 4029290:	d65f03c0 	ret
 4029294:	d2a20000 	mov	x0, #0x10000000            	// #268435456
 4029298:	528001e3 	mov	w3, #0xf                   	// #15
 402929c:	7100029f 	cmp	w20, #0x0
 40292a0:	b9400016 	ldr	w22, [x0]
 40292a4:	b9400002 	ldr	w2, [x0]
 40292a8:	12187042 	and	w2, w2, #0xffffff1f
 40292ac:	b9000002 	str	w2, [x0]
 40292b0:	528003e2 	mov	w2, #0x1f                  	// #31
 40292b4:	1a831042 	csel	w2, w2, w3, ne  // ne = any
 40292b8:	d2800483 	mov	x3, #0x24                  	// #36
 40292bc:	b9400260 	ldr	w0, [x19]
 40292c0:	f2a20003 	movk	x3, #0x1000, lsl #16
 40292c4:	b9000062 	str	w2, [x3]
 40292c8:	d2800582 	mov	x2, #0x2c                  	// #44
 40292cc:	f2a20002 	movk	x2, #0x1000, lsl #16
 40292d0:	b9000041 	str	w1, [x2]
 40292d4:	d2800601 	mov	x1, #0x30                  	// #48
 40292d8:	f2a20001 	movk	x1, #0x1000, lsl #16
 40292dc:	52840202 	mov	w2, #0x2010                	// #8208
 40292e0:	b9000022 	str	w2, [x1]
 40292e4:	52800022 	mov	w2, #0x1                   	// #1
 40292e8:	b9000822 	str	w2, [x1, #8]
 40292ec:	6b02029f 	cmp	w20, w2
 40292f0:	54000281 	b.ne	4029340 <spi_nand_feature_op+0x150>  // b.any
 40292f4:	d2a1e001 	mov	x1, #0xf000000             	// #251658240
 40292f8:	12001c00 	and	w0, w0, #0xff
 40292fc:	39000020 	strb	w0, [x1]
 4029300:	52801c20 	mov	w0, #0xe1                  	// #225
 4029304:	d2800797 	mov	x23, #0x3c                  	// #60
 4029308:	5290d415 	mov	w21, #0x86a0                	// #34464
 402930c:	f2a20017 	movk	x23, #0x1000, lsl #16
 4029310:	72a00035 	movk	w21, #0x1, lsl #16
 4029314:	b90002e0 	str	w0, [x23]
 4029318:	d2800140 	mov	x0, #0xa                   	// #10
 402931c:	97fffe67 	bl	4028cb8 <udelay>
 4029320:	b94002e0 	ldr	w0, [x23]
 4029324:	510006b5 	sub	w21, w21, #0x1
 4029328:	36000100 	tbz	w0, #0, 4029348 <spi_nand_feature_op+0x158>
 402932c:	35ffff75 	cbnz	w21, 4029318 <spi_nand_feature_op+0x128>
 4029330:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029334:	91351800 	add	x0, x0, #0xd46
 4029338:	97fffe48 	bl	4028c58 <log_serial_puts>
 402933c:	14000004 	b	402934c <spi_nand_feature_op+0x15c>
 4029340:	528018a0 	mov	w0, #0xc5                  	// #197
 4029344:	17fffff0 	b	4029304 <spi_nand_feature_op+0x114>
 4029348:	34ffff55 	cbz	w21, 4029330 <spi_nand_feature_op+0x140>
 402934c:	350000b4 	cbnz	w20, 4029360 <spi_nand_feature_op+0x170>
 4029350:	d2a1e000 	mov	x0, #0xf000000             	// #251658240
 4029354:	39400000 	ldrb	w0, [x0]
 4029358:	12001c00 	and	w0, w0, #0xff
 402935c:	b9000260 	str	w0, [x19]
 4029360:	d2a20000 	mov	x0, #0x10000000            	// #268435456
 4029364:	b9400001 	ldr	w1, [x0]
 4029368:	b9000016 	str	w22, [x0]
 402936c:	17ffffc5 	b	4029280 <spi_nand_feature_op+0x90>

0000000004029370 <spi_nand_write_enable>:
 4029370:	d100c3ff 	sub	sp, sp, #0x30
 4029374:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4029378:	910013e2 	add	x2, sp, #0x4
 402937c:	a90153f3 	stp	x19, x20, [sp, #16]
 4029380:	f90013fe 	str	x30, [sp, #32]
 4029384:	f9477001 	ldr	x1, [x0, #3808]
 4029388:	f90007e1 	str	x1, [sp, #8]
 402938c:	d2800001 	mov	x1, #0x0                   	// #0
 4029390:	52800000 	mov	w0, #0x0                   	// #0
 4029394:	128007e1 	mov	w1, #0xffffffc0            	// #-64
 4029398:	97ffff96 	bl	40291f0 <spi_nand_feature_op>
 402939c:	12001c00 	and	w0, w0, #0xff
 40293a0:	7102941f 	cmp	w0, #0xa5
 40293a4:	54000521 	b.ne	4029448 <spi_nand_write_enable+0xd8>  // b.any
 40293a8:	b94007e0 	ldr	w0, [sp, #4]
 40293ac:	36080120 	tbz	w0, #1, 40293d0 <spi_nand_write_enable+0x60>
 40293b0:	528014a0 	mov	w0, #0xa5                  	// #165
 40293b4:	90ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 40293b8:	f94007e2 	ldr	x2, [sp, #8]
 40293bc:	f9477023 	ldr	x3, [x1, #3808]
 40293c0:	eb030042 	subs	x2, x2, x3
 40293c4:	d2800003 	mov	x3, #0x0                   	// #0
 40293c8:	54000440 	b.eq	4029450 <spi_nand_write_enable+0xe0>  // b.none
 40293cc:	97fff38c 	bl	40261fc <__stack_chk_fail>
 40293d0:	d2800081 	mov	x1, #0x4                   	// #4
 40293d4:	f2a20001 	movk	x1, #0x1000, lsl #16
 40293d8:	b9400020 	ldr	w0, [x1]
 40293dc:	36300060 	tbz	w0, #6, 40293e8 <spi_nand_write_enable+0x78>
 40293e0:	12197800 	and	w0, w0, #0xffffffbf
 40293e4:	b9000020 	str	w0, [x1]
 40293e8:	d2800480 	mov	x0, #0x24                  	// #36
 40293ec:	d2800794 	mov	x20, #0x3c                  	// #60
 40293f0:	f2a20000 	movk	x0, #0x1000, lsl #16
 40293f4:	528000c1 	mov	w1, #0x6                   	// #6
 40293f8:	f2a20014 	movk	x20, #0x1000, lsl #16
 40293fc:	5290d413 	mov	w19, #0x86a0                	// #34464
 4029400:	b9000001 	str	w1, [x0]
 4029404:	52840001 	mov	w1, #0x2000                	// #8192
 4029408:	b9000c01 	str	w1, [x0, #12]
 402940c:	72a00033 	movk	w19, #0x1, lsl #16
 4029410:	52801020 	mov	w0, #0x81                  	// #129
 4029414:	b9000280 	str	w0, [x20]
 4029418:	d2800140 	mov	x0, #0xa                   	// #10
 402941c:	97fffe27 	bl	4028cb8 <udelay>
 4029420:	b9400280 	ldr	w0, [x20]
 4029424:	51000673 	sub	w19, w19, #0x1
 4029428:	360000c0 	tbz	w0, #0, 4029440 <spi_nand_write_enable+0xd0>
 402942c:	35ffff73 	cbnz	w19, 4029418 <spi_nand_write_enable+0xa8>
 4029430:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029434:	91351800 	add	x0, x0, #0xd46
 4029438:	97fffe08 	bl	4028c58 <log_serial_puts>
 402943c:	17ffffdd 	b	40293b0 <spi_nand_write_enable+0x40>
 4029440:	35fffb93 	cbnz	w19, 40293b0 <spi_nand_write_enable+0x40>
 4029444:	17fffffb 	b	4029430 <spi_nand_write_enable+0xc0>
 4029448:	52800b40 	mov	w0, #0x5a                  	// #90
 402944c:	17ffffda 	b	40293b4 <spi_nand_write_enable+0x44>
 4029450:	a94153f3 	ldp	x19, x20, [sp, #16]
 4029454:	f94013fe 	ldr	x30, [sp, #32]
 4029458:	9100c3ff 	add	sp, sp, #0x30
 402945c:	d65f03c0 	ret

0000000004029460 <spi_general_wait_ready>:
 4029460:	d100c3ff 	sub	sp, sp, #0x30
 4029464:	a90153f3 	stp	x19, x20, [sp, #16]
 4029468:	12001c14 	and	w20, w0, #0xff
 402946c:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4029470:	f90013fe 	str	x30, [sp, #32]
 4029474:	52820033 	mov	w19, #0x1001                	// #4097
 4029478:	f9477001 	ldr	x1, [x0, #3808]
 402947c:	f90007e1 	str	x1, [sp, #8]
 4029480:	d2800001 	mov	x1, #0x0                   	// #0
 4029484:	910013e2 	add	x2, sp, #0x4
 4029488:	128007e1 	mov	w1, #0xffffffc0            	// #-64
 402948c:	52800000 	mov	w0, #0x0                   	// #0
 4029490:	97ffff58 	bl	40291f0 <spi_nand_feature_op>
 4029494:	12001c00 	and	w0, w0, #0xff
 4029498:	7102941f 	cmp	w0, #0xa5
 402949c:	540000c0 	b.eq	40294b4 <spi_general_wait_ready+0x54>  // b.none
 40294a0:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40294a4:	9135b400 	add	x0, x0, #0xd6d
 40294a8:	97fffdec 	bl	4028c58 <log_serial_puts>
 40294ac:	52800b40 	mov	w0, #0x5a                  	// #90
 40294b0:	14000007 	b	40294cc <spi_general_wait_ready+0x6c>
 40294b4:	b94007e1 	ldr	w1, [sp, #4]
 40294b8:	37000181 	tbnz	w1, #0, 40294e8 <spi_general_wait_ready+0x88>
 40294bc:	7100429f 	cmp	w20, #0x10
 40294c0:	54000061 	b.ne	40294cc <spi_general_wait_ready+0x6c>  // b.any
 40294c4:	f27d003f 	tst	x1, #0x8
 40294c8:	1a801020 	csel	w0, w1, w0, ne  // ne = any
 40294cc:	90ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 40294d0:	f94007e2 	ldr	x2, [sp, #8]
 40294d4:	f9477023 	ldr	x3, [x1, #3808]
 40294d8:	eb030042 	subs	x2, x2, x3
 40294dc:	d2800003 	mov	x3, #0x0                   	// #0
 40294e0:	54000120 	b.eq	4029504 <spi_general_wait_ready+0xa4>  // b.none
 40294e4:	97fff346 	bl	40261fc <__stack_chk_fail>
 40294e8:	d2800020 	mov	x0, #0x1                   	// #1
 40294ec:	97fffdf3 	bl	4028cb8 <udelay>
 40294f0:	71000673 	subs	w19, w19, #0x1
 40294f4:	54fffc81 	b.ne	4029484 <spi_general_wait_ready+0x24>  // b.any
 40294f8:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40294fc:	91360c00 	add	x0, x0, #0xd83
 4029500:	17ffffea 	b	40294a8 <spi_general_wait_ready+0x48>
 4029504:	a94153f3 	ldp	x19, x20, [sp, #16]
 4029508:	f94013fe 	ldr	x30, [sp, #32]
 402950c:	9100c3ff 	add	sp, sp, #0x30
 4029510:	d65f03c0 	ret

0000000004029514 <spi_nand_page_read>:
 4029514:	b0ffffc6 	adrp	x6, 4022000 <malloc_sizes+0xf0>
 4029518:	9100c0c6 	add	x6, x6, #0x30
 402951c:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4029520:	7100003f 	cmp	w1, #0x0
 4029524:	d2800401 	mov	x1, #0x20                  	// #32
 4029528:	f2a20001 	movk	x1, #0x1000, lsl #16
 402952c:	b94004c3 	ldr	w3, [x6, #4]
 4029530:	f9000bfe 	str	x30, [sp, #16]
 4029534:	52801fe2 	mov	w2, #0xff                  	// #255
 4029538:	b9000022 	str	w2, [x1]
 402953c:	1a9f17e5 	cset	w5, eq  // eq = none
 4029540:	1ac32400 	lsr	w0, w0, w3
 4029544:	b9400cc1 	ldr	w1, [x6, #12]
 4029548:	531c6ca5 	lsl	w5, w5, #4
 402954c:	7100003f 	cmp	w1, #0x0
 4029550:	52800041 	mov	w1, #0x2                   	// #2
 4029554:	1a9f1422 	csinc	w2, w1, wzr, ne  // ne = any
 4029558:	52800081 	mov	w1, #0x4                   	// #4
 402955c:	1a9f1021 	csel	w1, w1, wzr, ne  // ne = any
 4029560:	2a011c41 	orr	w1, w2, w1, lsl #7
 4029564:	d2800602 	mov	x2, #0x30                  	// #48
 4029568:	f2a20002 	movk	x2, #0x1000, lsl #16
 402956c:	32130021 	orr	w1, w1, #0x2000
 4029570:	b9000041 	str	w1, [x2]
 4029574:	97fffeff 	bl	4029170 <set_addr_reg>
 4029578:	f94008c0 	ldr	x0, [x6, #16]
 402957c:	97ffff0f 	bl	40291b8 <set_dma_addr_reg>
 4029580:	b9400cc1 	ldr	w1, [x6, #12]
 4029584:	d2800d00 	mov	x0, #0x68                  	// #104
 4029588:	f2a20000 	movk	x0, #0x1000, lsl #16
 402958c:	34000281 	cbz	w1, 40295dc <spi_nand_page_read+0xc8>
 4029590:	52800021 	mov	w1, #0x1                   	// #1
 4029594:	72a01d61 	movk	w1, #0xeb, lsl #16
 4029598:	5290d413 	mov	w19, #0x86a0                	// #34464
 402959c:	d2800314 	mov	x20, #0x18                  	// #24
 40295a0:	2a0100a5 	orr	w5, w5, w1
 40295a4:	72a00033 	movk	w19, #0x1, lsl #16
 40295a8:	f2a20014 	movk	x20, #0x1000, lsl #16
 40295ac:	b9000005 	str	w5, [x0]
 40295b0:	d2800140 	mov	x0, #0xa                   	// #10
 40295b4:	97fffdc1 	bl	4028cb8 <udelay>
 40295b8:	b9400280 	ldr	w0, [x20]
 40295bc:	51000673 	sub	w19, w19, #0x1
 40295c0:	37000140 	tbnz	w0, #0, 40295e8 <spi_nand_page_read+0xd4>
 40295c4:	35ffff73 	cbnz	w19, 40295b0 <spi_nand_page_read+0x9c>
 40295c8:	f9400bfe 	ldr	x30, [sp, #16]
 40295cc:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40295d0:	a8c253f3 	ldp	x19, x20, [sp], #32
 40295d4:	91369000 	add	x0, x0, #0xda4
 40295d8:	17fffda0 	b	4028c58 <log_serial_puts>
 40295dc:	52800021 	mov	w1, #0x1                   	// #1
 40295e0:	72a00161 	movk	w1, #0xb, lsl #16
 40295e4:	17ffffed 	b	4029598 <spi_nand_page_read+0x84>
 40295e8:	34ffff13 	cbz	w19, 40295c8 <spi_nand_page_read+0xb4>
 40295ec:	f9400bfe 	ldr	x30, [sp, #16]
 40295f0:	a8c253f3 	ldp	x19, x20, [sp], #32
 40295f4:	d65f03c0 	ret

00000000040295f8 <spi_nand_erase>:
 40295f8:	a9bb53f3 	stp	x19, x20, [sp, #-80]!
 40295fc:	2a0103f3 	mov	w19, w1
 4029600:	2a0003f4 	mov	w20, w0
 4029604:	a9015bf5 	stp	x21, x22, [sp, #16]
 4029608:	b0ffffd5 	adrp	x21, 4022000 <malloc_sizes+0xf0>
 402960c:	d2800416 	mov	x22, #0x20                  	// #32
 4029610:	9100c2b5 	add	x21, x21, #0x30
 4029614:	a90263f7 	stp	x23, x24, [sp, #32]
 4029618:	d2800497 	mov	x23, #0x24                  	// #36
 402961c:	d2800598 	mov	x24, #0x2c                  	// #44
 4029620:	52800b40 	mov	w0, #0x5a                  	// #90
 4029624:	f2a20016 	movk	x22, #0x1000, lsl #16
 4029628:	f2a20017 	movk	x23, #0x1000, lsl #16
 402962c:	f2a20018 	movk	x24, #0x1000, lsl #16
 4029630:	a9036bf9 	stp	x25, x26, [sp, #48]
 4029634:	f90023fe 	str	x30, [sp, #64]
 4029638:	350000f3 	cbnz	w19, 4029654 <spi_nand_erase+0x5c>
 402963c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4029640:	a94263f7 	ldp	x23, x24, [sp, #32]
 4029644:	a9436bf9 	ldp	x25, x26, [sp, #48]
 4029648:	f94023fe 	ldr	x30, [sp, #64]
 402964c:	a8c553f3 	ldp	x19, x20, [sp], #80
 4029650:	d65f03c0 	ret
 4029654:	97ffff47 	bl	4029370 <spi_nand_write_enable>
 4029658:	7102941f 	cmp	w0, #0xa5
 402965c:	54ffff01 	b.ne	402963c <spi_nand_erase+0x44>  // b.any
 4029660:	b94006a0 	ldr	w0, [x21, #4]
 4029664:	52801fe1 	mov	w1, #0xff                  	// #255
 4029668:	b90002c1 	str	w1, [x22]
 402966c:	52801b01 	mov	w1, #0xd8                  	// #216
 4029670:	b90002e1 	str	w1, [x23]
 4029674:	d280079a 	mov	x26, #0x3c                  	// #60
 4029678:	1ac02680 	lsr	w0, w20, w0
 402967c:	121a6400 	and	w0, w0, #0xffffffc0
 4029680:	b9000300 	str	w0, [x24]
 4029684:	d2800600 	mov	x0, #0x30                  	// #48
 4029688:	f2a20000 	movk	x0, #0x1000, lsl #16
 402968c:	f2a2001a 	movk	x26, #0x1000, lsl #16
 4029690:	5290d419 	mov	w25, #0x86a0                	// #34464
 4029694:	52840601 	mov	w1, #0x2030                	// #8240
 4029698:	b9000001 	str	w1, [x0]
 402969c:	72a00039 	movk	w25, #0x1, lsl #16
 40296a0:	52801820 	mov	w0, #0xc1                  	// #193
 40296a4:	b9000340 	str	w0, [x26]
 40296a8:	d2800140 	mov	x0, #0xa                   	// #10
 40296ac:	97fffd83 	bl	4028cb8 <udelay>
 40296b0:	b9400340 	ldr	w0, [x26]
 40296b4:	51000739 	sub	w25, w25, #0x1
 40296b8:	360000c0 	tbz	w0, #0, 40296d0 <spi_nand_erase+0xd8>
 40296bc:	35ffff79 	cbnz	w25, 40296a8 <spi_nand_erase+0xb0>
 40296c0:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 40296c4:	91351800 	add	x0, x0, #0xd46
 40296c8:	97fffd64 	bl	4028c58 <log_serial_puts>
 40296cc:	14000002 	b	40296d4 <spi_nand_erase+0xdc>
 40296d0:	34ffff99 	cbz	w25, 40296c0 <spi_nand_erase+0xc8>
 40296d4:	52800c00 	mov	w0, #0x60                  	// #96
 40296d8:	97ffff62 	bl	4029460 <spi_general_wait_ready>
 40296dc:	7102941f 	cmp	w0, #0xa5
 40296e0:	54fffae1 	b.ne	402963c <spi_nand_erase+0x44>  // b.any
 40296e4:	b9401aa1 	ldr	w1, [x21, #24]
 40296e8:	4b010273 	sub	w19, w19, w1
 40296ec:	0b010294 	add	w20, w20, w1
 40296f0:	17ffffd2 	b	4029638 <spi_nand_erase+0x40>

00000000040296f4 <spi_nand_write>:
 40296f4:	a9b953f3 	stp	x19, x20, [sp, #-112]!
 40296f8:	b0ffffd4 	adrp	x20, 4022000 <malloc_sizes+0xf0>
 40296fc:	2a0103f3 	mov	w19, w1
 4029700:	9100c294 	add	x20, x20, #0x30
 4029704:	a90263f7 	stp	x23, x24, [sp, #32]
 4029708:	aa0003f7 	mov	x23, x0
 402970c:	2a0203f8 	mov	w24, w2
 4029710:	a9036bf9 	stp	x25, x26, [sp, #48]
 4029714:	d280041a 	mov	x26, #0x20                  	// #32
 4029718:	a90473fb 	stp	x27, x28, [sp, #64]
 402971c:	d280061b 	mov	x27, #0x30                  	// #48
 4029720:	d2800d1c 	mov	x28, #0x68                  	// #104
 4029724:	f2a2001a 	movk	x26, #0x1000, lsl #16
 4029728:	f2a2001b 	movk	x27, #0x1000, lsl #16
 402972c:	f2a2001c 	movk	x28, #0x1000, lsl #16
 4029730:	a9015bf5 	stp	x21, x22, [sp, #16]
 4029734:	52800b56 	mov	w22, #0x5a                  	// #90
 4029738:	f9002bfe 	str	x30, [sp, #80]
 402973c:	35000133 	cbnz	w19, 4029760 <spi_nand_write+0x6c>
 4029740:	2a1603e0 	mov	w0, w22
 4029744:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4029748:	a94263f7 	ldp	x23, x24, [sp, #32]
 402974c:	a9436bf9 	ldp	x25, x26, [sp, #48]
 4029750:	a94473fb 	ldp	x27, x28, [sp, #64]
 4029754:	f9402bfe 	ldr	x30, [sp, #80]
 4029758:	a8c753f3 	ldp	x19, x20, [sp], #112
 402975c:	d65f03c0 	ret
 4029760:	b9400a95 	ldr	w21, [x20, #8]
 4029764:	aa1703e2 	mov	x2, x23
 4029768:	f9400a80 	ldr	x0, [x20, #16]
 402976c:	6b1302bf 	cmp	w21, w19
 4029770:	1a9392b9 	csel	w25, w21, w19, ls  // ls = plast
 4029774:	1a9392b5 	csel	w21, w21, w19, ls  // ls = plast
 4029778:	aa1503e3 	mov	x3, x21
 402977c:	aa1503e1 	mov	x1, x21
 4029780:	97fffd6e 	bl	4028d38 <memcpy_s>
 4029784:	97fffefb 	bl	4029370 <spi_nand_write_enable>
 4029788:	2a0003f6 	mov	w22, w0
 402978c:	7102941f 	cmp	w0, #0xa5
 4029790:	54fffd81 	b.ne	4029740 <spi_nand_write+0x4c>  // b.any
 4029794:	b9400680 	ldr	w0, [x20, #4]
 4029798:	52801fe1 	mov	w1, #0xff                  	// #255
 402979c:	b9000341 	str	w1, [x26]
 40297a0:	b9400e81 	ldr	w1, [x20, #12]
 40297a4:	1ac02700 	lsr	w0, w24, w0
 40297a8:	7100003f 	cmp	w1, #0x0
 40297ac:	52800061 	mov	w1, #0x3                   	// #3
 40297b0:	1a9f1021 	csel	w1, w1, wzr, ne  // ne = any
 40297b4:	53196021 	lsl	w1, w1, #7
 40297b8:	32130021 	orr	w1, w1, #0x2000
 40297bc:	b9000361 	str	w1, [x27]
 40297c0:	97fffe6c 	bl	4029170 <set_addr_reg>
 40297c4:	f9400a80 	ldr	x0, [x20, #16]
 40297c8:	97fffe7c 	bl	40291b8 <set_dma_addr_reg>
 40297cc:	b9400e80 	ldr	w0, [x20, #12]
 40297d0:	52804061 	mov	w1, #0x203                 	// #515
 40297d4:	7100001f 	cmp	w0, #0x0
 40297d8:	52864060 	mov	w0, #0x3203                	// #12803
 40297dc:	1a811000 	csel	w0, w0, w1, ne  // ne = any
 40297e0:	5290d401 	mov	w1, #0x86a0                	// #34464
 40297e4:	72a00021 	movk	w1, #0x1, lsl #16
 40297e8:	b9000380 	str	w0, [x28]
 40297ec:	d2800140 	mov	x0, #0xa                   	// #10
 40297f0:	b9006fe1 	str	w1, [sp, #108]
 40297f4:	97fffd31 	bl	4028cb8 <udelay>
 40297f8:	d2800302 	mov	x2, #0x18                  	// #24
 40297fc:	b9406fe1 	ldr	w1, [sp, #108]
 4029800:	f2a20002 	movk	x2, #0x1000, lsl #16
 4029804:	51000421 	sub	w1, w1, #0x1
 4029808:	b9400040 	ldr	w0, [x2]
 402980c:	370000c0 	tbnz	w0, #0, 4029824 <spi_nand_write+0x130>
 4029810:	35fffee1 	cbnz	w1, 40297ec <spi_nand_write+0xf8>
 4029814:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029818:	91369000 	add	x0, x0, #0xda4
 402981c:	97fffd0f 	bl	4028c58 <log_serial_puts>
 4029820:	14000002 	b	4029828 <spi_nand_write+0x134>
 4029824:	34ffff81 	cbz	w1, 4029814 <spi_nand_write+0x120>
 4029828:	52800200 	mov	w0, #0x10                  	// #16
 402982c:	97ffff0d 	bl	4029460 <spi_general_wait_ready>
 4029830:	6b190273 	subs	w19, w19, w25
 4029834:	54fff860 	b.eq	4029740 <spi_nand_write+0x4c>  // b.none
 4029838:	0b190318 	add	w24, w24, w25
 402983c:	8b1502f7 	add	x23, x23, x21
 4029840:	17ffffbf 	b	402973c <spi_nand_write+0x48>

0000000004029844 <spi_nand_read>:
 4029844:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 4029848:	2a0103f4 	mov	w20, w1
 402984c:	a9015bf5 	stp	x21, x22, [sp, #16]
 4029850:	aa0003f6 	mov	x22, x0
 4029854:	2a0203f5 	mov	w21, w2
 4029858:	a90263f7 	stp	x23, x24, [sp, #32]
 402985c:	b0ffffd7 	adrp	x23, 4022000 <malloc_sizes+0xf0>
 4029860:	9100c2f7 	add	x23, x23, #0x30
 4029864:	f9001bfe 	str	x30, [sp, #48]
 4029868:	350000f4 	cbnz	w20, 4029884 <spi_nand_read+0x40>
 402986c:	528014a0 	mov	w0, #0xa5                  	// #165
 4029870:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4029874:	a94263f7 	ldp	x23, x24, [sp, #32]
 4029878:	f9401bfe 	ldr	x30, [sp, #48]
 402987c:	a8c453f3 	ldp	x19, x20, [sp], #64
 4029880:	d65f03c0 	ret
 4029884:	2a1503e0 	mov	w0, w21
 4029888:	52800021 	mov	w1, #0x1                   	// #1
 402988c:	97ffff22 	bl	4029514 <spi_nand_page_read>
 4029890:	b9400ae2 	ldr	w2, [x23, #8]
 4029894:	f9400ae0 	ldr	x0, [x23, #16]
 4029898:	1ac20ab3 	udiv	w19, w21, w2
 402989c:	1b02d673 	msub	w19, w19, w2, w21
 40298a0:	4b130053 	sub	w19, w2, w19
 40298a4:	51000442 	sub	w2, w2, #0x1
 40298a8:	6b14027f 	cmp	w19, w20
 40298ac:	0a150042 	and	w2, w2, w21
 40298b0:	1a949278 	csel	w24, w19, w20, ls  // ls = plast
 40298b4:	1a949273 	csel	w19, w19, w20, ls  // ls = plast
 40298b8:	8b000042 	add	x2, x2, x0
 40298bc:	aa1303e3 	mov	x3, x19
 40298c0:	aa1303e1 	mov	x1, x19
 40298c4:	aa1603e0 	mov	x0, x22
 40298c8:	97fffd1c 	bl	4028d38 <memcpy_s>
 40298cc:	6b180294 	subs	w20, w20, w24
 40298d0:	54fffce0 	b.eq	402986c <spi_nand_read+0x28>  // b.none
 40298d4:	8b1302d6 	add	x22, x22, x19
 40298d8:	0b1802b5 	add	w21, w21, w24
 40298dc:	17ffffe3 	b	4029868 <spi_nand_read+0x24>

00000000040298e0 <spi_nand_page_size>:
 40298e0:	d2a20001 	mov	x1, #0x10000000            	// #268435456
 40298e4:	52810002 	mov	w2, #0x800                 	// #2048
 40298e8:	b9400021 	ldr	w1, [x1]
 40298ec:	721d043f 	tst	w1, #0x18
 40298f0:	52820001 	mov	w1, #0x1000                	// #4096
 40298f4:	1a821021 	csel	w1, w1, w2, ne  // ne = any
 40298f8:	b9000001 	str	w1, [x0]
 40298fc:	d65f03c0 	ret

0000000004029900 <spi_nand_block_size>:
 4029900:	d10083ff 	sub	sp, sp, #0x20
 4029904:	aa0003e3 	mov	x3, x0
 4029908:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402990c:	f9000bfe 	str	x30, [sp, #16]
 4029910:	f9477001 	ldr	x1, [x0, #3808]
 4029914:	f90007e1 	str	x1, [sp, #8]
 4029918:	d2800001 	mov	x1, #0x0                   	// #0
 402991c:	910013e0 	add	x0, sp, #0x4
 4029920:	b90007ff 	str	wzr, [sp, #4]
 4029924:	97ffffef 	bl	40298e0 <spi_nand_page_size>
 4029928:	d2a20000 	mov	x0, #0x10000000            	// #268435456
 402992c:	b94007e1 	ldr	w1, [sp, #4]
 4029930:	b9400002 	ldr	w2, [x0]
 4029934:	53196020 	lsl	w0, w1, #7
 4029938:	7218045f 	tst	w2, #0x300
 402993c:	531a6421 	lsl	w1, w1, #6
 4029940:	1a800021 	csel	w1, w1, w0, eq  // eq = none
 4029944:	b9000061 	str	w1, [x3]
 4029948:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402994c:	f94007e1 	ldr	x1, [sp, #8]
 4029950:	f9477002 	ldr	x2, [x0, #3808]
 4029954:	eb020021 	subs	x1, x1, x2
 4029958:	d2800002 	mov	x2, #0x0                   	// #0
 402995c:	54000040 	b.eq	4029964 <spi_nand_block_size+0x64>  // b.none
 4029960:	97fff227 	bl	40261fc <__stack_chk_fail>
 4029964:	f9400bfe 	ldr	x30, [sp, #16]
 4029968:	910083ff 	add	sp, sp, #0x20
 402996c:	d65f03c0 	ret

0000000004029970 <is_nand>:
 4029970:	d2a20000 	mov	x0, #0x10000000            	// #268435456
 4029974:	b9400000 	ldr	w0, [x0]
 4029978:	721f041f 	tst	w0, #0x6
 402997c:	1a9f07e0 	cset	w0, ne  // ne = any
 4029980:	d65f03c0 	ret

0000000004029984 <is_bad_block>:
 4029984:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 4029988:	b0ffffd3 	adrp	x19, 4022000 <malloc_sizes+0xf0>
 402998c:	9100c273 	add	x19, x19, #0x30
 4029990:	a9015bf5 	stp	x21, x22, [sp, #16]
 4029994:	52800001 	mov	w1, #0x0                   	// #0
 4029998:	2a0003f6 	mov	w22, w0
 402999c:	b9400a75 	ldr	w21, [x19, #8]
 40299a0:	f9400a74 	ldr	x20, [x19, #16]
 40299a4:	a9027bf7 	stp	x23, x30, [sp, #32]
 40299a8:	97fffedb 	bl	4029514 <spi_nand_page_read>
 40299ac:	8b1402b7 	add	x23, x21, x20
 40299b0:	38746aa0 	ldrb	w0, [x21, x20]
 40299b4:	7103fc1f 	cmp	w0, #0xff
 40299b8:	540000c0 	b.eq	40299d0 <is_bad_block+0x4c>  // b.none
 40299bc:	52800020 	mov	w0, #0x1                   	// #1
 40299c0:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40299c4:	a9427bf7 	ldp	x23, x30, [sp, #32]
 40299c8:	a8c353f3 	ldp	x19, x20, [sp], #48
 40299cc:	d65f03c0 	ret
 40299d0:	394006e0 	ldrb	w0, [x23, #1]
 40299d4:	7103fc1f 	cmp	w0, #0xff
 40299d8:	54ffff21 	b.ne	40299bc <is_bad_block+0x38>  // b.any
 40299dc:	b9400a62 	ldr	w2, [x19, #8]
 40299e0:	528007e0 	mov	w0, #0x3f                  	// #63
 40299e4:	52800001 	mov	w1, #0x0                   	// #0
 40299e8:	1b005840 	madd	w0, w2, w0, w22
 40299ec:	97fffeca 	bl	4029514 <spi_nand_page_read>
 40299f0:	38746aa0 	ldrb	w0, [x21, x20]
 40299f4:	7103fc1f 	cmp	w0, #0xff
 40299f8:	54fffe21 	b.ne	40299bc <is_bad_block+0x38>  // b.any
 40299fc:	394006e0 	ldrb	w0, [x23, #1]
 4029a00:	7103fc1f 	cmp	w0, #0xff
 4029a04:	1a9f07e0 	cset	w0, ne  // ne = any
 4029a08:	17ffffee 	b	40299c0 <is_bad_block+0x3c>

0000000004029a0c <spi_nand_init>:
 4029a0c:	d100c3ff 	sub	sp, sp, #0x30
 4029a10:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4029a14:	a90153f3 	stp	x19, x20, [sp, #16]
 4029a18:	f90013fe 	str	x30, [sp, #32]
 4029a1c:	f9477001 	ldr	x1, [x0, #3808]
 4029a20:	f90007e1 	str	x1, [sp, #8]
 4029a24:	d2800001 	mov	x1, #0x0                   	// #0
 4029a28:	d2800081 	mov	x1, #0x4                   	// #4
 4029a2c:	f2a20001 	movk	x1, #0x1000, lsl #16
 4029a30:	b9400020 	ldr	w0, [x1]
 4029a34:	37300d80 	tbnz	w0, #6, 4029be4 <spi_nand_init+0x1d8>
 4029a38:	b90003e0 	str	w0, [sp]
 4029a3c:	b0ffffc3 	adrp	x3, 4022000 <malloc_sizes+0xf0>
 4029a40:	9100c073 	add	x19, x3, #0x30
 4029a44:	91002260 	add	x0, x19, #0x8
 4029a48:	97ffffa6 	bl	40298e0 <spi_nand_page_size>
 4029a4c:	b9400a60 	ldr	w0, [x19, #8]
 4029a50:	7140041f 	cmp	w0, #0x1, lsl #12
 4029a54:	54000d01 	b.ne	4029bf4 <spi_nand_init+0x1e8>  // b.any
 4029a58:	52800180 	mov	w0, #0xc                   	// #12
 4029a5c:	b9000660 	str	w0, [x19, #4]
 4029a60:	52800240 	mov	w0, #0x12                  	// #18
 4029a64:	b9003060 	str	w0, [x3, #48]
 4029a68:	91006260 	add	x0, x19, #0x18
 4029a6c:	97ffffa5 	bl	4029900 <spi_nand_block_size>
 4029a70:	d2800e80 	mov	x0, #0x74                  	// #116
 4029a74:	d29c0014 	mov	x20, #0xe000                	// #57344
 4029a78:	f2a20000 	movk	x0, #0x1000, lsl #16
 4029a7c:	f2a08074 	movk	x20, #0x403, lsl #16
 4029a80:	d2822403 	mov	x3, #0x1120                	// #4384
 4029a84:	52801fe2 	mov	w2, #0xff                  	// #255
 4029a88:	b9400000 	ldr	w0, [x0]
 4029a8c:	aa0303e1 	mov	x1, x3
 4029a90:	d3410400 	ubfx	x0, x0, #1, #1
 4029a94:	b9000e60 	str	w0, [x19, #12]
 4029a98:	aa1403e0 	mov	x0, x20
 4029a9c:	97fffcab 	bl	4028d48 <memset_s>
 4029aa0:	d2a20001 	mov	x1, #0x10000000            	// #268435456
 4029aa4:	b9400020 	ldr	w0, [x1]
 4029aa8:	f9000a74 	str	x20, [x19, #16]
 4029aac:	37000060 	tbnz	w0, #0, 4029ab8 <spi_nand_init+0xac>
 4029ab0:	32000000 	orr	w0, w0, #0x1
 4029ab4:	b9000020 	str	w0, [x1]
 4029ab8:	d2800100 	mov	x0, #0x8                   	// #8
 4029abc:	5280cde1 	mov	w1, #0x66f                 	// #1647
 4029ac0:	f2a20000 	movk	x0, #0x1000, lsl #16
 4029ac4:	b9000001 	str	w1, [x0]
 4029ac8:	d2800901 	mov	x1, #0x48                  	// #72
 4029acc:	f2a20001 	movk	x1, #0x1000, lsl #16
 4029ad0:	528000e0 	mov	w0, #0x7                   	// #7
 4029ad4:	b90003e0 	str	w0, [sp]
 4029ad8:	b9000020 	str	w0, [x1]
 4029adc:	b9400e73 	ldr	w19, [x19, #12]
 4029ae0:	34000493 	cbz	w19, 4029b70 <spi_nand_init+0x164>
 4029ae4:	910013e2 	add	x2, sp, #0x4
 4029ae8:	128009e1 	mov	w1, #0xffffffb0            	// #-80
 4029aec:	52800000 	mov	w0, #0x0                   	// #0
 4029af0:	97fffdc0 	bl	40291f0 <spi_nand_feature_op>
 4029af4:	12001c00 	and	w0, w0, #0xff
 4029af8:	7102941f 	cmp	w0, #0xa5
 4029afc:	540003a1 	b.ne	4029b70 <spi_nand_init+0x164>  // b.any
 4029b00:	b94007e0 	ldr	w0, [sp, #4]
 4029b04:	12000001 	and	w1, w0, #0x1
 4029b08:	6b13003f 	cmp	w1, w19
 4029b0c:	54000320 	b.eq	4029b70 <spi_nand_init+0x164>  // b.none
 4029b10:	7100067f 	cmp	w19, #0x1
 4029b14:	32000001 	orr	w1, w0, #0x1
 4029b18:	121f7800 	and	w0, w0, #0xfffffffe
 4029b1c:	910013e2 	add	x2, sp, #0x4
 4029b20:	1a811000 	csel	w0, w0, w1, ne  // ne = any
 4029b24:	128009e1 	mov	w1, #0xffffffb0            	// #-80
 4029b28:	b90007e0 	str	w0, [sp, #4]
 4029b2c:	52800020 	mov	w0, #0x1                   	// #1
 4029b30:	97fffdb0 	bl	40291f0 <spi_nand_feature_op>
 4029b34:	12001c00 	and	w0, w0, #0xff
 4029b38:	7102941f 	cmp	w0, #0xa5
 4029b3c:	540001a1 	b.ne	4029b70 <spi_nand_init+0x164>  // b.any
 4029b40:	52800000 	mov	w0, #0x0                   	// #0
 4029b44:	97fffe47 	bl	4029460 <spi_general_wait_ready>
 4029b48:	910013e2 	add	x2, sp, #0x4
 4029b4c:	128009e1 	mov	w1, #0xffffffb0            	// #-80
 4029b50:	52800000 	mov	w0, #0x0                   	// #0
 4029b54:	97fffda7 	bl	40291f0 <spi_nand_feature_op>
 4029b58:	12001c00 	and	w0, w0, #0xff
 4029b5c:	7102941f 	cmp	w0, #0xa5
 4029b60:	54000560 	b.eq	4029c0c <spi_nand_init+0x200>  // b.none
 4029b64:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029b68:	91372c00 	add	x0, x0, #0xdcb
 4029b6c:	97fffc3b 	bl	4028c58 <log_serial_puts>
 4029b70:	910003e2 	mov	x2, sp
 4029b74:	12800be1 	mov	w1, #0xffffffa0            	// #-96
 4029b78:	52800000 	mov	w0, #0x0                   	// #0
 4029b7c:	97fffd9d 	bl	40291f0 <spi_nand_feature_op>
 4029b80:	12001c13 	and	w19, w0, #0xff
 4029b84:	7102967f 	cmp	w19, #0xa5
 4029b88:	54000201 	b.ne	4029bc8 <spi_nand_init+0x1bc>  // b.any
 4029b8c:	b94003e0 	ldr	w0, [sp]
 4029b90:	721d0c1f 	tst	w0, #0x78
 4029b94:	54000740 	b.eq	4029c7c <spi_nand_init+0x270>  // b.none
 4029b98:	12196c00 	and	w0, w0, #0xffffff87
 4029b9c:	910003e2 	mov	x2, sp
 4029ba0:	12800be1 	mov	w1, #0xffffffa0            	// #-96
 4029ba4:	b90003e0 	str	w0, [sp]
 4029ba8:	52800020 	mov	w0, #0x1                   	// #1
 4029bac:	97fffd91 	bl	40291f0 <spi_nand_feature_op>
 4029bb0:	12001c13 	and	w19, w0, #0xff
 4029bb4:	7102967f 	cmp	w19, #0xa5
 4029bb8:	540003e0 	b.eq	4029c34 <spi_nand_init+0x228>  // b.none
 4029bbc:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029bc0:	91389c00 	add	x0, x0, #0xe27
 4029bc4:	97fffc25 	bl	4028c58 <log_serial_puts>
 4029bc8:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4029bcc:	f94007e1 	ldr	x1, [sp, #8]
 4029bd0:	f9477002 	ldr	x2, [x0, #3808]
 4029bd4:	eb020021 	subs	x1, x1, x2
 4029bd8:	d2800002 	mov	x2, #0x0                   	// #0
 4029bdc:	54000980 	b.eq	4029d0c <spi_nand_init+0x300>  // b.none
 4029be0:	97fff187 	bl	40261fc <__stack_chk_fail>
 4029be4:	12197800 	and	w0, w0, #0xffffffbf
 4029be8:	b90003e0 	str	w0, [sp]
 4029bec:	b9000020 	str	w0, [x1]
 4029bf0:	17ffff93 	b	4029a3c <spi_nand_init+0x30>
 4029bf4:	7120001f 	cmp	w0, #0x800
 4029bf8:	54fff381 	b.ne	4029a68 <spi_nand_init+0x5c>  // b.any
 4029bfc:	52800160 	mov	w0, #0xb                   	// #11
 4029c00:	b9000660 	str	w0, [x19, #4]
 4029c04:	52800220 	mov	w0, #0x11                  	// #17
 4029c08:	17ffff97 	b	4029a64 <spi_nand_init+0x58>
 4029c0c:	b94007e0 	ldr	w0, [sp, #4]
 4029c10:	12000000 	and	w0, w0, #0x1
 4029c14:	6b00027f 	cmp	w19, w0
 4029c18:	54000081 	b.ne	4029c28 <spi_nand_init+0x21c>  // b.any
 4029c1c:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029c20:	9137a800 	add	x0, x0, #0xdea
 4029c24:	17ffffd2 	b	4029b6c <spi_nand_init+0x160>
 4029c28:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029c2c:	91382400 	add	x0, x0, #0xe09
 4029c30:	17ffffcf 	b	4029b6c <spi_nand_init+0x160>
 4029c34:	52800000 	mov	w0, #0x0                   	// #0
 4029c38:	97fffe0a 	bl	4029460 <spi_general_wait_ready>
 4029c3c:	910003e2 	mov	x2, sp
 4029c40:	12800be1 	mov	w1, #0xffffffa0            	// #-96
 4029c44:	52800000 	mov	w0, #0x0                   	// #0
 4029c48:	97fffd6a 	bl	40291f0 <spi_nand_feature_op>
 4029c4c:	12001c13 	and	w19, w0, #0xff
 4029c50:	7102967f 	cmp	w19, #0xa5
 4029c54:	54000080 	b.eq	4029c64 <spi_nand_init+0x258>  // b.none
 4029c58:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029c5c:	91390000 	add	x0, x0, #0xe40
 4029c60:	17ffffd9 	b	4029bc4 <spi_nand_init+0x1b8>
 4029c64:	b94003e0 	ldr	w0, [sp]
 4029c68:	721d0c1f 	tst	w0, #0x78
 4029c6c:	54000080 	b.eq	4029c7c <spi_nand_init+0x270>  // b.none
 4029c70:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029c74:	91396400 	add	x0, x0, #0xe59
 4029c78:	97fffbf8 	bl	4028c58 <log_serial_puts>
 4029c7c:	910003e2 	mov	x2, sp
 4029c80:	128009e1 	mov	w1, #0xffffffb0            	// #-80
 4029c84:	52800000 	mov	w0, #0x0                   	// #0
 4029c88:	97fffd5a 	bl	40291f0 <spi_nand_feature_op>
 4029c8c:	12001c00 	and	w0, w0, #0xff
 4029c90:	7102941f 	cmp	w0, #0xa5
 4029c94:	54000381 	b.ne	4029d04 <spi_nand_init+0x2f8>  // b.any
 4029c98:	b94003e0 	ldr	w0, [sp]
 4029c9c:	37200060 	tbnz	w0, #4, 4029ca8 <spi_nand_init+0x29c>
 4029ca0:	528014b3 	mov	w19, #0xa5                  	// #165
 4029ca4:	17ffffc9 	b	4029bc8 <spi_nand_init+0x1bc>
 4029ca8:	121b7800 	and	w0, w0, #0xffffffef
 4029cac:	910003e2 	mov	x2, sp
 4029cb0:	128009e1 	mov	w1, #0xffffffb0            	// #-80
 4029cb4:	b90003e0 	str	w0, [sp]
 4029cb8:	52800020 	mov	w0, #0x1                   	// #1
 4029cbc:	97fffd4d 	bl	40291f0 <spi_nand_feature_op>
 4029cc0:	12001c13 	and	w19, w0, #0xff
 4029cc4:	7102967f 	cmp	w19, #0xa5
 4029cc8:	54fff801 	b.ne	4029bc8 <spi_nand_init+0x1bc>  // b.any
 4029ccc:	52800000 	mov	w0, #0x0                   	// #0
 4029cd0:	97fffde4 	bl	4029460 <spi_general_wait_ready>
 4029cd4:	910003e2 	mov	x2, sp
 4029cd8:	128009e1 	mov	w1, #0xffffffb0            	// #-80
 4029cdc:	52800000 	mov	w0, #0x0                   	// #0
 4029ce0:	97fffd44 	bl	40291f0 <spi_nand_feature_op>
 4029ce4:	12001c13 	and	w19, w0, #0xff
 4029ce8:	7102967f 	cmp	w19, #0xa5
 4029cec:	54fff6e1 	b.ne	4029bc8 <spi_nand_init+0x1bc>  // b.any
 4029cf0:	b94003e0 	ldr	w0, [sp]
 4029cf4:	3627fd60 	tbz	w0, #4, 4029ca0 <spi_nand_init+0x294>
 4029cf8:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029cfc:	91396400 	add	x0, x0, #0xe59
 4029d00:	17ffffb1 	b	4029bc4 <spi_nand_init+0x1b8>
 4029d04:	2a0003f3 	mov	w19, w0
 4029d08:	17ffffb0 	b	4029bc8 <spi_nand_init+0x1bc>
 4029d0c:	2a1303e0 	mov	w0, w19
 4029d10:	a94153f3 	ldp	x19, x20, [sp, #16]
 4029d14:	f94013fe 	ldr	x30, [sp, #32]
 4029d18:	9100c3ff 	add	sp, sp, #0x30
 4029d1c:	d65f03c0 	ret

0000000004029d20 <spi_nand_deinit>:
 4029d20:	d2a20001 	mov	x1, #0x10000000            	// #268435456
 4029d24:	b9400020 	ldr	w0, [x1]
 4029d28:	121f7800 	and	w0, w0, #0xfffffffe
 4029d2c:	b9000020 	str	w0, [x1]
 4029d30:	d65f03c0 	ret

0000000004029d34 <spi_general_get_flash_register.constprop.0>:
 4029d34:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4029d38:	aa0003f4 	mov	x20, x0
 4029d3c:	d2800600 	mov	x0, #0x30                  	// #48
 4029d40:	f2a20000 	movk	x0, #0x1000, lsl #16
 4029d44:	a9017bf5 	stp	x21, x30, [sp, #16]
 4029d48:	d2800795 	mov	x21, #0x3c                  	// #60
 4029d4c:	f2a20015 	movk	x21, #0x1000, lsl #16
 4029d50:	52840001 	mov	w1, #0x2000                	// #8192
 4029d54:	5290d413 	mov	w19, #0x86a0                	// #34464
 4029d58:	b9000001 	str	w1, [x0]
 4029d5c:	72a00033 	movk	w19, #0x1, lsl #16
 4029d60:	52800060 	mov	w0, #0x3                   	// #3
 4029d64:	b90002a0 	str	w0, [x21]
 4029d68:	d2800140 	mov	x0, #0xa                   	// #10
 4029d6c:	97fffbd3 	bl	4028cb8 <udelay>
 4029d70:	b94002a0 	ldr	w0, [x21]
 4029d74:	51000673 	sub	w19, w19, #0x1
 4029d78:	360000c0 	tbz	w0, #0, 4029d90 <spi_general_get_flash_register.constprop.0+0x5c>
 4029d7c:	35ffff73 	cbnz	w19, 4029d68 <spi_general_get_flash_register.constprop.0+0x34>
 4029d80:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029d84:	91351800 	add	x0, x0, #0xd46
 4029d88:	97fffbb4 	bl	4028c58 <log_serial_puts>
 4029d8c:	14000002 	b	4029d94 <spi_general_get_flash_register.constprop.0+0x60>
 4029d90:	34ffff93 	cbz	w19, 4029d80 <spi_general_get_flash_register.constprop.0+0x4c>
 4029d94:	d2801580 	mov	x0, #0xac                  	// #172
 4029d98:	f2a20000 	movk	x0, #0x1000, lsl #16
 4029d9c:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4029da0:	b9400000 	ldr	w0, [x0]
 4029da4:	39000280 	strb	w0, [x20]
 4029da8:	a8c253f3 	ldp	x19, x20, [sp], #32
 4029dac:	d65f03c0 	ret

0000000004029db0 <spi_general_wait_ready>:
 4029db0:	d10083ff 	sub	sp, sp, #0x20
 4029db4:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4029db8:	a9017bf3 	stp	x19, x30, [sp, #16]
 4029dbc:	52800033 	mov	w19, #0x1                   	// #1
 4029dc0:	72a00213 	movk	w19, #0x10, lsl #16
 4029dc4:	f9477001 	ldr	x1, [x0, #3808]
 4029dc8:	f90007e1 	str	x1, [sp, #8]
 4029dcc:	d2800001 	mov	x1, #0x0                   	// #0
 4029dd0:	91001fe0 	add	x0, sp, #0x7
 4029dd4:	97ffffd8 	bl	4029d34 <spi_general_get_flash_register.constprop.0>
 4029dd8:	39401fe0 	ldrb	w0, [sp, #7]
 4029ddc:	360001a0 	tbz	w0, #0, 4029e10 <spi_general_wait_ready+0x60>
 4029de0:	d2800020 	mov	x0, #0x1                   	// #1
 4029de4:	97fffbb5 	bl	4028cb8 <udelay>
 4029de8:	71000673 	subs	w19, w19, #0x1
 4029dec:	54ffff21 	b.ne	4029dd0 <spi_general_wait_ready+0x20>  // b.any
 4029df0:	52800b40 	mov	w0, #0x5a                  	// #90
 4029df4:	90ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4029df8:	f94007e2 	ldr	x2, [sp, #8]
 4029dfc:	f9477023 	ldr	x3, [x1, #3808]
 4029e00:	eb030042 	subs	x2, x2, x3
 4029e04:	d2800003 	mov	x3, #0x0                   	// #0
 4029e08:	54000080 	b.eq	4029e18 <spi_general_wait_ready+0x68>  // b.none
 4029e0c:	97fff0fc 	bl	40261fc <__stack_chk_fail>
 4029e10:	528014a0 	mov	w0, #0xa5                  	// #165
 4029e14:	17fffff8 	b	4029df4 <spi_general_wait_ready+0x44>
 4029e18:	a9417bf3 	ldp	x19, x30, [sp, #16]
 4029e1c:	910083ff 	add	sp, sp, #0x20
 4029e20:	d65f03c0 	ret

0000000004029e24 <spi_nor_write_en>:
 4029e24:	d100c3ff 	sub	sp, sp, #0x30
 4029e28:	90ffffc0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 4029e2c:	a90153f3 	stp	x19, x20, [sp, #16]
 4029e30:	f90013fe 	str	x30, [sp, #32]
 4029e34:	f9477001 	ldr	x1, [x0, #3808]
 4029e38:	f90007e1 	str	x1, [sp, #8]
 4029e3c:	d2800001 	mov	x1, #0x0                   	// #0
 4029e40:	91001fe0 	add	x0, sp, #0x7
 4029e44:	97ffffbc 	bl	4029d34 <spi_general_get_flash_register.constprop.0>
 4029e48:	39401fe0 	ldrb	w0, [sp, #7]
 4029e4c:	36080120 	tbz	w0, #1, 4029e70 <spi_nor_write_en+0x4c>
 4029e50:	528014a0 	mov	w0, #0xa5                  	// #165
 4029e54:	90ffffc1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 4029e58:	f94007e2 	ldr	x2, [sp, #8]
 4029e5c:	f9477023 	ldr	x3, [x1, #3808]
 4029e60:	eb030042 	subs	x2, x2, x3
 4029e64:	d2800003 	mov	x3, #0x0                   	// #0
 4029e68:	54000500 	b.eq	4029f08 <spi_nor_write_en+0xe4>  // b.none
 4029e6c:	97fff0e4 	bl	40261fc <__stack_chk_fail>
 4029e70:	d2800081 	mov	x1, #0x4                   	// #4
 4029e74:	f2a20001 	movk	x1, #0x1000, lsl #16
 4029e78:	b9400020 	ldr	w0, [x1]
 4029e7c:	36300060 	tbz	w0, #6, 4029e88 <spi_nor_write_en+0x64>
 4029e80:	12197800 	and	w0, w0, #0xffffffbf
 4029e84:	b9000020 	str	w0, [x1]
 4029e88:	d2800480 	mov	x0, #0x24                  	// #36
 4029e8c:	d2800794 	mov	x20, #0x3c                  	// #60
 4029e90:	f2a20000 	movk	x0, #0x1000, lsl #16
 4029e94:	528000c1 	mov	w1, #0x6                   	// #6
 4029e98:	f2a20014 	movk	x20, #0x1000, lsl #16
 4029e9c:	5290d413 	mov	w19, #0x86a0                	// #34464
 4029ea0:	b9000001 	str	w1, [x0]
 4029ea4:	52840001 	mov	w1, #0x2000                	// #8192
 4029ea8:	b9000c01 	str	w1, [x0, #12]
 4029eac:	72a00033 	movk	w19, #0x1, lsl #16
 4029eb0:	52801020 	mov	w0, #0x81                  	// #129
 4029eb4:	b9000280 	str	w0, [x20]
 4029eb8:	d2800140 	mov	x0, #0xa                   	// #10
 4029ebc:	97fffb7f 	bl	4028cb8 <udelay>
 4029ec0:	b9400280 	ldr	w0, [x20]
 4029ec4:	51000673 	sub	w19, w19, #0x1
 4029ec8:	360000c0 	tbz	w0, #0, 4029ee0 <spi_nor_write_en+0xbc>
 4029ecc:	35ffff73 	cbnz	w19, 4029eb8 <spi_nor_write_en+0x94>
 4029ed0:	90000040 	adrp	x0, 4031000 <sync_exception_sp_el0>
 4029ed4:	91351800 	add	x0, x0, #0xd46
 4029ed8:	97fffb60 	bl	4028c58 <log_serial_puts>
 4029edc:	14000002 	b	4029ee4 <spi_nor_write_en+0xc0>
 4029ee0:	34ffff93 	cbz	w19, 4029ed0 <spi_nor_write_en+0xac>
 4029ee4:	97ffffb3 	bl	4029db0 <spi_general_wait_ready>
 4029ee8:	7102941f 	cmp	w0, #0xa5
 4029eec:	54fffb41 	b.ne	4029e54 <spi_nor_write_en+0x30>  // b.any
 4029ef0:	91001fe0 	add	x0, sp, #0x7
 4029ef4:	97ffff90 	bl	4029d34 <spi_general_get_flash_register.constprop.0>
 4029ef8:	39401fe0 	ldrb	w0, [sp, #7]
 4029efc:	370ffaa0 	tbnz	w0, #1, 4029e50 <spi_nor_write_en+0x2c>
 4029f00:	52800b40 	mov	w0, #0x5a                  	// #90
 4029f04:	17ffffd4 	b	4029e54 <spi_nor_write_en+0x30>
 4029f08:	a94153f3 	ldp	x19, x20, [sp, #16]
 4029f0c:	f94013fe 	ldr	x30, [sp, #32]
 4029f10:	9100c3ff 	add	sp, sp, #0x30
 4029f14:	d65f03c0 	ret

0000000004029f18 <dma_cycle_op>:
 4029f18:	a9b953f3 	stp	x19, x20, [sp, #-112]!
 4029f1c:	2a0103f4 	mov	w20, w1
 4029f20:	aa0303f3 	mov	x19, x3
 4029f24:	a90263f7 	stp	x23, x24, [sp, #32]
 4029f28:	72001c17 	ands	w23, w0, #0xff
 4029f2c:	52820000 	mov	w0, #0x1000                	// #4096
 4029f30:	2a0203f8 	mov	w24, w2
 4029f34:	a9015bf5 	stp	x21, x22, [sp, #16]
 4029f38:	52a00415 	mov	w21, #0x200000              	// #2097152
 4029f3c:	1a8002b5 	csel	w21, w21, w0, eq  // eq = none
 4029f40:	531f7ae0 	lsl	w0, w23, #1
 4029f44:	a9036bf9 	stp	x25, x26, [sp, #48]
 4029f48:	d2800599 	mov	x25, #0x2c                  	// #44
 4029f4c:	d280061a 	mov	x26, #0x30                  	// #48
 4029f50:	a90473fb 	stp	x27, x28, [sp, #64]
 4029f54:	d280081b 	mov	x27, #0x40                  	// #64
 4029f58:	d280401c 	mov	x28, #0x200                 	// #512
 4029f5c:	32000000 	orr	w0, w0, #0x1
 4029f60:	f2a20019 	movk	x25, #0x1000, lsl #16
 4029f64:	f2a2001a 	movk	x26, #0x1000, lsl #16
 4029f68:	f2a2001b 	movk	x27, #0x1000, lsl #16
 4029f6c:	f2a2001c 	movk	x28, #0x1000, lsl #16
 4029f70:	f9002bfe 	str	x30, [sp, #80]
 4029f74:	b9006be0 	str	w0, [sp, #104]
 4029f78:	35000138 	cbnz	w24, 4029f9c <dma_cycle_op+0x84>
 4029f7c:	528014a0 	mov	w0, #0xa5                  	// #165
 4029f80:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4029f84:	a94263f7 	ldp	x23, x24, [sp, #32]
 4029f88:	a9436bf9 	ldp	x25, x26, [sp, #48]
 4029f8c:	a94473fb 	ldp	x27, x28, [sp, #64]
 4029f90:	f9402bfe 	ldr	x30, [sp, #80]
 4029f94:	a8c753f3 	ldp	x19, x20, [sp], #112
 4029f98:	d65f03c0 	ret
 4029f9c:	6b1802bf 	cmp	w21, w24
 4029fa0:	1a9892b6 	csel	w22, w21, w24, ls  // ls = plast
 4029fa4:	97ffff83 	bl	4029db0 <spi_general_wait_ready>
 4029fa8:	7102941f 	cmp	w0, #0xa5
 4029fac:	54fffea1 	b.ne	4029f80 <dma_cycle_op+0x68>  // b.any
 4029fb0:	d2800401 	mov	x1, #0x20                  	// #32
 4029fb4:	710006ff 	cmp	w23, #0x1
 4029fb8:	f2a20001 	movk	x1, #0x1000, lsl #16
 4029fbc:	52801fe0 	mov	w0, #0xff                  	// #255
 4029fc0:	1a9f07e3 	cset	w3, ne  // ne = any
 4029fc4:	52840604 	mov	w4, #0x2030                	// #8240
 4029fc8:	b9000020 	str	w0, [x1]
 4029fcc:	52800041 	mov	w1, #0x2                   	// #2
 4029fd0:	1a8112e1 	csel	w1, w23, w1, ne  // ne = any
 4029fd4:	52800160 	mov	w0, #0xb                   	// #11
 4029fd8:	1a9f1000 	csel	w0, w0, wzr, ne  // ne = any
 4029fdc:	2a040063 	orr	w3, w3, w4
 4029fe0:	53185c21 	lsl	w1, w1, #8
 4029fe4:	2a004020 	orr	w0, w1, w0, lsl #16
 4029fe8:	b9406be1 	ldr	w1, [sp, #104]
 4029fec:	b9000334 	str	w20, [x25]
 4029ff0:	b9000343 	str	w3, [x26]
 4029ff4:	d3608663 	ubfx	x3, x19, #32, #2
 4029ff8:	b9000376 	str	w22, [x27]
 4029ffc:	2a010000 	orr	w0, w0, w1
 402a000:	b9000383 	str	w3, [x28]
 402a004:	d2800983 	mov	x3, #0x4c                  	// #76
 402a008:	d2800d01 	mov	x1, #0x68                  	// #104
 402a00c:	f2a20003 	movk	x3, #0x1000, lsl #16
 402a010:	f2a20001 	movk	x1, #0x1000, lsl #16
 402a014:	b9000073 	str	w19, [x3]
 402a018:	b9000020 	str	w0, [x1]
 402a01c:	5290d401 	mov	w1, #0x86a0                	// #34464
 402a020:	72a00021 	movk	w1, #0x1, lsl #16
 402a024:	d2800140 	mov	x0, #0xa                   	// #10
 402a028:	b9006fe1 	str	w1, [sp, #108]
 402a02c:	97fffb23 	bl	4028cb8 <udelay>
 402a030:	d2800303 	mov	x3, #0x18                  	// #24
 402a034:	b9406fe1 	ldr	w1, [sp, #108]
 402a038:	f2a20003 	movk	x3, #0x1000, lsl #16
 402a03c:	51000421 	sub	w1, w1, #0x1
 402a040:	b9400060 	ldr	w0, [x3]
 402a044:	370000c0 	tbnz	w0, #0, 402a05c <dma_cycle_op+0x144>
 402a048:	35fffee1 	cbnz	w1, 402a024 <dma_cycle_op+0x10c>
 402a04c:	f0000020 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402a050:	91369000 	add	x0, x0, #0xda4
 402a054:	97fffb01 	bl	4028c58 <log_serial_puts>
 402a058:	14000002 	b	402a060 <dma_cycle_op+0x148>
 402a05c:	34ffff81 	cbz	w1, 402a04c <dma_cycle_op+0x134>
 402a060:	0b160294 	add	w20, w20, w22
 402a064:	8b364273 	add	x19, x19, w22, uxtw
 402a068:	4b160318 	sub	w24, w24, w22
 402a06c:	17ffffc3 	b	4029f78 <dma_cycle_op+0x60>

000000000402a070 <spi_nor_erase>:
 402a070:	72003c1f 	tst	w0, #0xffff
 402a074:	54000661 	b.ne	402a140 <spi_nor_erase+0xd0>  // b.any
 402a078:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402a07c:	2a0003f3 	mov	w19, w0
 402a080:	0b010014 	add	w20, w0, w1
 402a084:	a9015bf5 	stp	x21, x22, [sp, #16]
 402a088:	d2800796 	mov	x22, #0x3c                  	// #60
 402a08c:	f2a20016 	movk	x22, #0x1000, lsl #16
 402a090:	a90263f7 	stp	x23, x24, [sp, #32]
 402a094:	d2800497 	mov	x23, #0x24                  	// #36
 402a098:	d2800598 	mov	x24, #0x2c                  	// #44
 402a09c:	a9037bf9 	stp	x25, x30, [sp, #48]
 402a0a0:	d2800619 	mov	x25, #0x30                  	// #48
 402a0a4:	f2a20017 	movk	x23, #0x1000, lsl #16
 402a0a8:	f2a20018 	movk	x24, #0x1000, lsl #16
 402a0ac:	f2a20019 	movk	x25, #0x1000, lsl #16
 402a0b0:	4b130295 	sub	w21, w20, w19
 402a0b4:	350000f3 	cbnz	w19, 402a0d0 <spi_nor_erase+0x60>
 402a0b8:	528014a0 	mov	w0, #0xa5                  	// #165
 402a0bc:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402a0c0:	a94263f7 	ldp	x23, x24, [sp, #32]
 402a0c4:	a9437bf9 	ldp	x25, x30, [sp, #48]
 402a0c8:	a8c453f3 	ldp	x19, x20, [sp], #64
 402a0cc:	d65f03c0 	ret
 402a0d0:	97ffff38 	bl	4029db0 <spi_general_wait_ready>
 402a0d4:	7102941f 	cmp	w0, #0xa5
 402a0d8:	54ffff21 	b.ne	402a0bc <spi_nor_erase+0x4c>  // b.any
 402a0dc:	97ffff52 	bl	4029e24 <spi_nor_write_en>
 402a0e0:	7102941f 	cmp	w0, #0xa5
 402a0e4:	54fffec1 	b.ne	402a0bc <spi_nor_erase+0x4c>  // b.any
 402a0e8:	52801b00 	mov	w0, #0xd8                  	// #216
 402a0ec:	b90002e0 	str	w0, [x23]
 402a0f0:	b9000315 	str	w21, [x24]
 402a0f4:	52840600 	mov	w0, #0x2030                	// #8240
 402a0f8:	5290d415 	mov	w21, #0x86a0                	// #34464
 402a0fc:	b9000320 	str	w0, [x25]
 402a100:	72a00035 	movk	w21, #0x1, lsl #16
 402a104:	52801820 	mov	w0, #0xc1                  	// #193
 402a108:	b90002c0 	str	w0, [x22]
 402a10c:	d2800140 	mov	x0, #0xa                   	// #10
 402a110:	97fffaea 	bl	4028cb8 <udelay>
 402a114:	b94002c0 	ldr	w0, [x22]
 402a118:	510006b5 	sub	w21, w21, #0x1
 402a11c:	360000c0 	tbz	w0, #0, 402a134 <spi_nor_erase+0xc4>
 402a120:	35ffff75 	cbnz	w21, 402a10c <spi_nor_erase+0x9c>
 402a124:	f0000020 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402a128:	91351800 	add	x0, x0, #0xd46
 402a12c:	97fffacb 	bl	4028c58 <log_serial_puts>
 402a130:	14000002 	b	402a138 <spi_nor_erase+0xc8>
 402a134:	34ffff95 	cbz	w21, 402a124 <spi_nor_erase+0xb4>
 402a138:	51404273 	sub	w19, w19, #0x10, lsl #12
 402a13c:	17ffffdd 	b	402a0b0 <spi_nor_erase+0x40>
 402a140:	52800b40 	mov	w0, #0x5a                  	// #90
 402a144:	d65f03c0 	ret

000000000402a148 <spi_nor_write>:
 402a148:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402a14c:	2a0103f4 	mov	w20, w1
 402a150:	2a0203f3 	mov	w19, w2
 402a154:	a9017bf5 	stp	x21, x30, [sp, #16]
 402a158:	aa0003f5 	mov	x21, x0
 402a15c:	97ffff15 	bl	4029db0 <spi_general_wait_ready>
 402a160:	7102941f 	cmp	w0, #0xa5
 402a164:	54000161 	b.ne	402a190 <spi_nor_write+0x48>  // b.any
 402a168:	97ffff2f 	bl	4029e24 <spi_nor_write_en>
 402a16c:	7102941f 	cmp	w0, #0xa5
 402a170:	54000101 	b.ne	402a190 <spi_nor_write+0x48>  // b.any
 402a174:	aa1503e3 	mov	x3, x21
 402a178:	2a1403e2 	mov	w2, w20
 402a17c:	2a1303e1 	mov	w1, w19
 402a180:	52800020 	mov	w0, #0x1                   	// #1
 402a184:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402a188:	a8c253f3 	ldp	x19, x20, [sp], #32
 402a18c:	17ffff63 	b	4029f18 <dma_cycle_op>
 402a190:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402a194:	a8c253f3 	ldp	x19, x20, [sp], #32
 402a198:	d65f03c0 	ret

000000000402a19c <spi_nor_read>:
 402a19c:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402a1a0:	2a0103f4 	mov	w20, w1
 402a1a4:	2a0203f3 	mov	w19, w2
 402a1a8:	a9017bf5 	stp	x21, x30, [sp, #16]
 402a1ac:	aa0003f5 	mov	x21, x0
 402a1b0:	97ffff00 	bl	4029db0 <spi_general_wait_ready>
 402a1b4:	7102941f 	cmp	w0, #0xa5
 402a1b8:	54000101 	b.ne	402a1d8 <spi_nor_read+0x3c>  // b.any
 402a1bc:	aa1503e3 	mov	x3, x21
 402a1c0:	2a1403e2 	mov	w2, w20
 402a1c4:	2a1303e1 	mov	w1, w19
 402a1c8:	52800000 	mov	w0, #0x0                   	// #0
 402a1cc:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402a1d0:	a8c253f3 	ldp	x19, x20, [sp], #32
 402a1d4:	17ffff51 	b	4029f18 <dma_cycle_op>
 402a1d8:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402a1dc:	a8c253f3 	ldp	x19, x20, [sp], #32
 402a1e0:	d65f03c0 	ret

000000000402a1e4 <spi_nor_init>:
 402a1e4:	d2a20001 	mov	x1, #0x10000000            	// #268435456
 402a1e8:	b9400020 	ldr	w0, [x1]
 402a1ec:	37000060 	tbnz	w0, #0, 402a1f8 <spi_nor_init+0x14>
 402a1f0:	32000000 	orr	w0, w0, #0x1
 402a1f4:	b9000020 	str	w0, [x1]
 402a1f8:	d2800100 	mov	x0, #0x8                   	// #8
 402a1fc:	5280cde1 	mov	w1, #0x66f                 	// #1647
 402a200:	f2a20000 	movk	x0, #0x1000, lsl #16
 402a204:	b9000001 	str	w1, [x0]
 402a208:	528014a0 	mov	w0, #0xa5                  	// #165
 402a20c:	d65f03c0 	ret

000000000402a210 <spi_nor_deinit>:
 402a210:	d2a20001 	mov	x1, #0x10000000            	// #268435456
 402a214:	b9400020 	ldr	w0, [x1]
 402a218:	121f7800 	and	w0, w0, #0xfffffffe
 402a21c:	b9000020 	str	w0, [x1]
 402a220:	d65f03c0 	ret

000000000402a224 <sdhci_send_cmd.constprop.0>:
 402a224:	d101c3ff 	sub	sp, sp, #0x70
 402a228:	a90353f3 	stp	x19, x20, [sp, #48]
 402a22c:	aa0003f3 	mov	x19, x0
 402a230:	f0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402a234:	a9045bf5 	stp	x21, x22, [sp, #64]
 402a238:	a90563f7 	stp	x23, x24, [sp, #80]
 402a23c:	f90033fe 	str	x30, [sp, #96]
 402a240:	f9477001 	ldr	x1, [x0, #3808]
 402a244:	f90017e1 	str	x1, [sp, #40]
 402a248:	d2800001 	mov	x1, #0x0                   	// #0
 402a24c:	f9401260 	ldr	x0, [x19, #32]
 402a250:	f9400415 	ldr	x21, [x0, #8]
 402a254:	d2800300 	mov	x0, #0x18                  	// #24
 402a258:	f2a22040 	movk	x0, #0x1102, lsl #16
 402a25c:	2a1503f4 	mov	w20, w21
 402a260:	b9400001 	ldr	w1, [x0]
 402a264:	d2800500 	mov	x0, #0x28                  	// #40
 402a268:	f2a20040 	movk	x0, #0x1002, lsl #16
 402a26c:	36580ae1 	tbz	w1, #11, 402a3c8 <sdhci_send_cmd.constprop.0+0x1a4>
 402a270:	52800401 	mov	w1, #0x20                  	// #32
 402a274:	39000001 	strb	w1, [x0]
 402a278:	d28007c0 	mov	x0, #0x3e                  	// #62
 402a27c:	f2a20040 	movk	x0, #0x1002, lsl #16
 402a280:	7900001f 	strh	wzr, [x0]
 402a284:	f9401260 	ldr	x0, [x19, #32]
 402a288:	b4000640 	cbz	x0, 402a350 <sdhci_send_cmd.constprop.0+0x12c>
 402a28c:	79400001 	ldrh	w1, [x0]
 402a290:	d28000c0 	mov	x0, #0x6                   	// #6
 402a294:	f2a20040 	movk	x0, #0x1002, lsl #16
 402a298:	528002b6 	mov	w22, #0x15                  	// #21
 402a29c:	79000001 	strh	w1, [x0]
 402a2a0:	528e4001 	mov	w1, #0x7200                	// #29184
 402a2a4:	781fe001 	sturh	w1, [x0, #-2]
 402a2a8:	d2800200 	mov	x0, #0x10                  	// #16
 402a2ac:	f2c04000 	movk	x0, #0x200, lsl #32
 402a2b0:	f90003e0 	str	x0, [sp]
 402a2b4:	910003e0 	mov	x0, sp
 402a2b8:	390023f6 	strb	w22, [sp, #8]
 402a2bc:	390027ff 	strb	wzr, [sp, #9]
 402a2c0:	39002bff 	strb	wzr, [sp, #10]
 402a2c4:	39002fff 	strb	wzr, [sp, #11]
 402a2c8:	b9000fff 	str	wzr, [sp, #12]
 402a2cc:	f9000bff 	str	xzr, [sp, #16]
 402a2d0:	b9001bff 	str	wzr, [sp, #24]
 402a2d4:	f90013ff 	str	xzr, [sp, #32]
 402a2d8:	97ffffd3 	bl	402a224 <sdhci_send_cmd.constprop.0>
 402a2dc:	f9401260 	ldr	x0, [x19, #32]
 402a2e0:	528002e1 	mov	w1, #0x17                  	// #23
 402a2e4:	b9400000 	ldr	w0, [x0]
 402a2e8:	290003e1 	stp	w1, w0, [sp]
 402a2ec:	910003e0 	mov	x0, sp
 402a2f0:	390023f6 	strb	w22, [sp, #8]
 402a2f4:	390027ff 	strb	wzr, [sp, #9]
 402a2f8:	39002bff 	strb	wzr, [sp, #10]
 402a2fc:	39002fff 	strb	wzr, [sp, #11]
 402a300:	b9000fff 	str	wzr, [sp, #12]
 402a304:	f9000bff 	str	xzr, [sp, #16]
 402a308:	b9001bff 	str	wzr, [sp, #24]
 402a30c:	f90013ff 	str	xzr, [sp, #32]
 402a310:	97ffffc5 	bl	402a224 <sdhci_send_cmd.constprop.0>
 402a314:	f9401261 	ldr	x1, [x19, #32]
 402a318:	52800462 	mov	w2, #0x23                  	// #35
 402a31c:	b9400020 	ldr	w0, [x1]
 402a320:	39401821 	ldrb	w1, [x1, #6]
 402a324:	7100081f 	cmp	w0, #0x2
 402a328:	52800060 	mov	w0, #0x3                   	// #3
 402a32c:	1a823000 	csel	w0, w0, w2, cc  // cc = lo, ul, last
 402a330:	7100043f 	cmp	w1, #0x1
 402a334:	d2800181 	mov	x1, #0xc                   	// #12
 402a338:	f2a20041 	movk	x1, #0x1002, lsl #16
 402a33c:	54000041 	b.ne	402a344 <sdhci_send_cmd.constprop.0+0x120>  // b.any
 402a340:	321c0000 	orr	w0, w0, #0x10
 402a344:	79000020 	strh	w0, [x1]
 402a348:	d2a20040 	mov	x0, #0x10020000            	// #268566528
 402a34c:	b9000015 	str	w21, [x0]
 402a350:	d2800600 	mov	x0, #0x30                  	// #48
 402a354:	12800001 	mov	w1, #0xffffffff            	// #-1
 402a358:	f2a20040 	movk	x0, #0x1002, lsl #16
 402a35c:	b9000001 	str	w1, [x0]
 402a360:	b9400661 	ldr	w1, [x19, #4]
 402a364:	b81d8001 	stur	w1, [x0, #-40]
 402a368:	39402260 	ldrb	w0, [x19, #8]
 402a36c:	71001c1f 	cmp	w0, #0x7
 402a370:	540004e0 	b.eq	402a40c <sdhci_send_cmd.constprop.0+0x1e8>  // b.none
 402a374:	540002e8 	b.hi	402a3d0 <sdhci_send_cmd.constprop.0+0x1ac>  // b.pmore
 402a378:	340004e0 	cbz	w0, 402a414 <sdhci_send_cmd.constprop.0+0x1f0>
 402a37c:	52800041 	mov	w1, #0x2                   	// #2
 402a380:	7100041f 	cmp	w0, #0x1
 402a384:	54000300 	b.eq	402a3e4 <sdhci_send_cmd.constprop.0+0x1c0>  // b.none
 402a388:	d2800616 	mov	x22, #0x30                  	// #48
 402a38c:	52817715 	mov	w21, #0xbb8                 	// #3000
 402a390:	f2a20056 	movk	x22, #0x1002, lsl #16
 402a394:	97fff353 	bl	40270e0 <timer_get_divider>
 402a398:	1b007eb5 	mul	w21, w21, w0
 402a39c:	97fffa3f 	bl	4028c98 <timer_start>
 402a3a0:	b94002d7 	ldr	w23, [x22]
 402a3a4:	370004b7 	tbnz	w23, #0, 402a438 <sdhci_send_cmd.constprop.0+0x214>
 402a3a8:	97fffa40 	bl	4028ca8 <timer_get_val>
 402a3ac:	eb15001f 	cmp	x0, x21
 402a3b0:	540003a9 	b.ls	402a424 <sdhci_send_cmd.constprop.0+0x200>  // b.plast
 402a3b4:	f0000020 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402a3b8:	913a0800 	add	x0, x0, #0xe82
 402a3bc:	97fffa27 	bl	4028c58 <log_serial_puts>
 402a3c0:	52800b40 	mov	w0, #0x5a                  	// #90
 402a3c4:	1400003d 	b	402a4b8 <sdhci_send_cmd.constprop.0+0x294>
 402a3c8:	52800041 	mov	w1, #0x2                   	// #2
 402a3cc:	17ffffaa 	b	402a274 <sdhci_send_cmd.constprop.0+0x50>
 402a3d0:	7100541f 	cmp	w0, #0x15
 402a3d4:	54000240 	b.eq	402a41c <sdhci_send_cmd.constprop.0+0x1f8>  // b.none
 402a3d8:	7100741f 	cmp	w0, #0x1d
 402a3dc:	54fffd61 	b.ne	402a388 <sdhci_send_cmd.constprop.0+0x164>  // b.any
 402a3e0:	52800061 	mov	w1, #0x3                   	// #3
 402a3e4:	b9400260 	ldr	w0, [x19]
 402a3e8:	2a002020 	orr	w0, w1, w0, lsl #8
 402a3ec:	f9401261 	ldr	x1, [x19, #32]
 402a3f0:	12003c00 	and	w0, w0, #0xffff
 402a3f4:	b4000041 	cbz	x1, 402a3fc <sdhci_send_cmd.constprop.0+0x1d8>
 402a3f8:	321b0000 	orr	w0, w0, #0x20
 402a3fc:	d28001c1 	mov	x1, #0xe                   	// #14
 402a400:	f2a20041 	movk	x1, #0x1002, lsl #16
 402a404:	79000020 	strh	w0, [x1]
 402a408:	17ffffe0 	b	402a388 <sdhci_send_cmd.constprop.0+0x164>
 402a40c:	52800021 	mov	w1, #0x1                   	// #1
 402a410:	17fffff5 	b	402a3e4 <sdhci_send_cmd.constprop.0+0x1c0>
 402a414:	52800001 	mov	w1, #0x0                   	// #0
 402a418:	17fffff3 	b	402a3e4 <sdhci_send_cmd.constprop.0+0x1c0>
 402a41c:	52800041 	mov	w1, #0x2                   	// #2
 402a420:	17fffff1 	b	402a3e4 <sdhci_send_cmd.constprop.0+0x1c0>
 402a424:	721142ff 	tst	w23, #0xffff8000
 402a428:	54fffbc0 	b.eq	402a3a0 <sdhci_send_cmd.constprop.0+0x17c>  // b.none
 402a42c:	f0000020 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402a430:	913a6c00 	add	x0, x0, #0xe9b
 402a434:	17ffffe2 	b	402a3bc <sdhci_send_cmd.constprop.0+0x198>
 402a438:	52800020 	mov	w0, #0x1                   	// #1
 402a43c:	b90002c0 	str	w0, [x22]
 402a440:	39402260 	ldrb	w0, [x19, #8]
 402a444:	340000a0 	cbz	w0, 402a458 <sdhci_send_cmd.constprop.0+0x234>
 402a448:	d2800200 	mov	x0, #0x10                  	// #16
 402a44c:	f2a20040 	movk	x0, #0x1002, lsl #16
 402a450:	b9400000 	ldr	w0, [x0]
 402a454:	b9000e60 	str	w0, [x19, #12]
 402a458:	f9401260 	ldr	x0, [x19, #32]
 402a45c:	b40002c0 	cbz	x0, 402a4b4 <sdhci_send_cmd.constprop.0+0x290>
 402a460:	5290d433 	mov	w19, #0x86a1                	// #34465
 402a464:	d2800616 	mov	x22, #0x30                  	// #48
 402a468:	72a00033 	movk	w19, #0x1, lsl #16
 402a46c:	f2a20056 	movk	x22, #0x1002, lsl #16
 402a470:	52800117 	mov	w23, #0x8                   	// #8
 402a474:	d2a20058 	mov	x24, #0x10020000            	// #268566528
 402a478:	b94002d5 	ldr	w21, [x22]
 402a47c:	361800b5 	tbz	w21, #3, 402a490 <sdhci_send_cmd.constprop.0+0x26c>
 402a480:	120d3294 	and	w20, w20, #0xfff80000
 402a484:	b90002d7 	str	w23, [x22]
 402a488:	11420294 	add	w20, w20, #0x80, lsl #12
 402a48c:	b9000314 	str	w20, [x24]
 402a490:	71000673 	subs	w19, w19, #0x1
 402a494:	54fff960 	b.eq	402a3c0 <sdhci_send_cmd.constprop.0+0x19c>  // b.none
 402a498:	d2800140 	mov	x0, #0xa                   	// #10
 402a49c:	97fffa07 	bl	4028cb8 <udelay>
 402a4a0:	360ffed5 	tbz	w21, #1, 402a478 <sdhci_send_cmd.constprop.0+0x254>
 402a4a4:	d2800600 	mov	x0, #0x30                  	// #48
 402a4a8:	52800041 	mov	w1, #0x2                   	// #2
 402a4ac:	f2a20040 	movk	x0, #0x1002, lsl #16
 402a4b0:	b9000001 	str	w1, [x0]
 402a4b4:	528014a0 	mov	w0, #0xa5                  	// #165
 402a4b8:	f0ffffa1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402a4bc:	f94017e2 	ldr	x2, [sp, #40]
 402a4c0:	f9477023 	ldr	x3, [x1, #3808]
 402a4c4:	eb030042 	subs	x2, x2, x3
 402a4c8:	d2800003 	mov	x3, #0x0                   	// #0
 402a4cc:	54000040 	b.eq	402a4d4 <sdhci_send_cmd.constprop.0+0x2b0>  // b.none
 402a4d0:	97ffef4b 	bl	40261fc <__stack_chk_fail>
 402a4d4:	a94353f3 	ldp	x19, x20, [sp, #48]
 402a4d8:	a9445bf5 	ldp	x21, x22, [sp, #64]
 402a4dc:	a94563f7 	ldp	x23, x24, [sp, #80]
 402a4e0:	f94033fe 	ldr	x30, [sp, #96]
 402a4e4:	9101c3ff 	add	sp, sp, #0x70
 402a4e8:	d65f03c0 	ret

000000000402a4ec <mmc_write_block>:
 402a4ec:	d10143ff 	sub	sp, sp, #0x50
 402a4f0:	7100045f 	cmp	w2, #0x1
 402a4f4:	f0ffffa3 	adrp	x3, 4021000 <gs_misc_record-0xa80>
 402a4f8:	f90023fe 	str	x30, [sp, #64]
 402a4fc:	f9477064 	ldr	x4, [x3, #3808]
 402a500:	f9001fe4 	str	x4, [sp, #56]
 402a504:	d2800004 	mov	x4, #0x0                   	// #0
 402a508:	f90007e0 	str	x0, [sp, #8]
 402a50c:	1a9f97e0 	cset	w0, hi  // hi = pmore
 402a510:	52804003 	mov	w3, #0x200                 	// #512
 402a514:	11006000 	add	w0, w0, #0x18
 402a518:	b90013e0 	str	w0, [sp, #16]
 402a51c:	528002a0 	mov	w0, #0x15                  	// #21
 402a520:	390063e0 	strb	w0, [sp, #24]
 402a524:	910003e0 	mov	x0, sp
 402a528:	29000fe2 	stp	w2, w3, [sp]
 402a52c:	b90017e1 	str	w1, [sp, #20]
 402a530:	390067ff 	strb	wzr, [sp, #25]
 402a534:	39006bff 	strb	wzr, [sp, #26]
 402a538:	39006fff 	strb	wzr, [sp, #27]
 402a53c:	b9001fff 	str	wzr, [sp, #28]
 402a540:	f90013ff 	str	xzr, [sp, #32]
 402a544:	b9002bff 	str	wzr, [sp, #40]
 402a548:	f9001be0 	str	x0, [sp, #48]
 402a54c:	910043e0 	add	x0, sp, #0x10
 402a550:	97ffff35 	bl	402a224 <sdhci_send_cmd.constprop.0>
 402a554:	f0ffffa1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402a558:	f9401fe2 	ldr	x2, [sp, #56]
 402a55c:	f9477023 	ldr	x3, [x1, #3808]
 402a560:	eb030042 	subs	x2, x2, x3
 402a564:	d2800003 	mov	x3, #0x0                   	// #0
 402a568:	54000040 	b.eq	402a570 <mmc_write_block+0x84>  // b.none
 402a56c:	97ffef24 	bl	40261fc <__stack_chk_fail>
 402a570:	f94023fe 	ldr	x30, [sp, #64]
 402a574:	910143ff 	add	sp, sp, #0x50
 402a578:	d65f03c0 	ret

000000000402a57c <emmc_write>:
 402a57c:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402a580:	12175853 	and	w19, w2, #0xfffffe00
 402a584:	53097c34 	lsr	w20, w1, #9
 402a588:	a9015bf5 	stp	x21, x22, [sp, #16]
 402a58c:	aa0003f6 	mov	x22, x0
 402a590:	12002055 	and	w21, w2, #0x1ff
 402a594:	a9027bf7 	stp	x23, x30, [sp, #32]
 402a598:	7107fe7f 	cmp	w19, #0x1ff
 402a59c:	540001e9 	b.ls	402a5d8 <emmc_write+0x5c>  // b.plast
 402a5a0:	53097e77 	lsr	w23, w19, #9
 402a5a4:	2a1403e1 	mov	w1, w20
 402a5a8:	2a1703e2 	mov	w2, w23
 402a5ac:	97ffffd0 	bl	402a4ec <mmc_write_block>
 402a5b0:	7102941f 	cmp	w0, #0xa5
 402a5b4:	540000a1 	b.ne	402a5c8 <emmc_write+0x4c>  // b.any
 402a5b8:	0b170294 	add	w20, w20, w23
 402a5bc:	12175a73 	and	w19, w19, #0xfffffe00
 402a5c0:	35000115 	cbnz	w21, 402a5e0 <emmc_write+0x64>
 402a5c4:	528014a0 	mov	w0, #0xa5                  	// #165
 402a5c8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402a5cc:	a9427bf7 	ldp	x23, x30, [sp, #32]
 402a5d0:	a8c353f3 	ldp	x19, x20, [sp], #48
 402a5d4:	d65f03c0 	ret
 402a5d8:	34ffff75 	cbz	w21, 402a5c4 <emmc_write+0x48>
 402a5dc:	52800013 	mov	w19, #0x0                   	// #0
 402a5e0:	d29c0000 	mov	x0, #0xe000                	// #57344
 402a5e4:	d2804003 	mov	x3, #0x200                 	// #512
 402a5e8:	52801fe2 	mov	w2, #0xff                  	// #255
 402a5ec:	aa0303e1 	mov	x1, x3
 402a5f0:	f2a08060 	movk	x0, #0x403, lsl #16
 402a5f4:	97fff9d5 	bl	4028d48 <memset_s>
 402a5f8:	2a1503e3 	mov	w3, w21
 402a5fc:	d29c0000 	mov	x0, #0xe000                	// #57344
 402a600:	8b3342c2 	add	x2, x22, w19, uxtw
 402a604:	aa0303e1 	mov	x1, x3
 402a608:	f2a08060 	movk	x0, #0x403, lsl #16
 402a60c:	97fff9cb 	bl	4028d38 <memcpy_s>
 402a610:	2a1403e1 	mov	w1, w20
 402a614:	d29c0000 	mov	x0, #0xe000                	// #57344
 402a618:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402a61c:	52800022 	mov	w2, #0x1                   	// #1
 402a620:	a9427bf7 	ldp	x23, x30, [sp, #32]
 402a624:	f2a08060 	movk	x0, #0x403, lsl #16
 402a628:	a8c353f3 	ldp	x19, x20, [sp], #48
 402a62c:	17ffffb0 	b	402a4ec <mmc_write_block>

000000000402a630 <emmc_read>:
 402a630:	d102c3ff 	sub	sp, sp, #0xb0
 402a634:	a9065bf5 	stp	x21, x22, [sp, #96]
 402a638:	1107fc56 	add	w22, w2, #0x1ff
 402a63c:	2a0103f5 	mov	w21, w1
 402a640:	a90553f3 	stp	x19, x20, [sp, #80]
 402a644:	53097ed6 	lsr	w22, w22, #9
 402a648:	0b4126d6 	add	w22, w22, w1, lsr #9
 402a64c:	a90763f7 	stp	x23, x24, [sp, #112]
 402a650:	d2800038 	mov	x24, #0x1                   	// #1
 402a654:	f2c04018 	movk	x24, #0x200, lsl #32
 402a658:	a9086bf9 	stp	x25, x26, [sp, #128]
 402a65c:	d29c0017 	mov	x23, #0xe000                	// #57344
 402a660:	2a0203f4 	mov	w20, w2
 402a664:	a90973fb 	stp	x27, x28, [sp, #144]
 402a668:	aa0003fb 	mov	x27, x0
 402a66c:	53097c3c 	lsr	w28, w1, #9
 402a670:	f90053fe 	str	x30, [sp, #160]
 402a674:	f0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402a678:	f2e00038 	movk	x24, #0x1, lsl #48
 402a67c:	f2a08077 	movk	x23, #0x403, lsl #16
 402a680:	f9477001 	ldr	x1, [x0, #3808]
 402a684:	f90027e1 	str	x1, [sp, #72]
 402a688:	d2800001 	mov	x1, #0x0                   	// #0
 402a68c:	52800239 	mov	w25, #0x11                  	// #17
 402a690:	528002ba 	mov	w26, #0x15                  	// #21
 402a694:	6b16039f 	cmp	w28, w22
 402a698:	54000121 	b.ne	402a6bc <emmc_read+0x8c>  // b.any
 402a69c:	528014a0 	mov	w0, #0xa5                  	// #165
 402a6a0:	f0ffffa1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402a6a4:	f94027e2 	ldr	x2, [sp, #72]
 402a6a8:	f9477023 	ldr	x3, [x1, #3808]
 402a6ac:	eb030042 	subs	x2, x2, x3
 402a6b0:	d2800003 	mov	x3, #0x0                   	// #0
 402a6b4:	54000480 	b.eq	402a744 <emmc_read+0x114>  // b.none
 402a6b8:	97ffeed1 	bl	40261fc <__stack_chk_fail>
 402a6bc:	910043e0 	add	x0, sp, #0x10
 402a6c0:	a9015ff8 	stp	x24, x23, [sp, #16]
 402a6c4:	290473f9 	stp	w25, w28, [sp, #32]
 402a6c8:	1100079c 	add	w28, w28, #0x1
 402a6cc:	3900a3fa 	strb	w26, [sp, #40]
 402a6d0:	3900a7ff 	strb	wzr, [sp, #41]
 402a6d4:	3900abff 	strb	wzr, [sp, #42]
 402a6d8:	3900afff 	strb	wzr, [sp, #43]
 402a6dc:	b9002fff 	str	wzr, [sp, #44]
 402a6e0:	f9001bff 	str	xzr, [sp, #48]
 402a6e4:	b9003bff 	str	wzr, [sp, #56]
 402a6e8:	f90023e0 	str	x0, [sp, #64]
 402a6ec:	910083e0 	add	x0, sp, #0x20
 402a6f0:	97fffecd 	bl	402a224 <sdhci_send_cmd.constprop.0>
 402a6f4:	7102941f 	cmp	w0, #0xa5
 402a6f8:	54fffd41 	b.ne	402a6a0 <emmc_read+0x70>  // b.any
 402a6fc:	120022a2 	and	w2, w21, #0x1ff
 402a700:	52804013 	mov	w19, #0x200                 	// #512
 402a704:	4b020273 	sub	w19, w19, w2
 402a708:	aa1b03e0 	mov	x0, x27
 402a70c:	6b14027f 	cmp	w19, w20
 402a710:	d37e2042 	ubfiz	x2, x2, #2, #9
 402a714:	1a949264 	csel	w4, w19, w20, ls  // ls = plast
 402a718:	1a949273 	csel	w19, w19, w20, ls  // ls = plast
 402a71c:	aa1303e3 	mov	x3, x19
 402a720:	8b170042 	add	x2, x2, x23
 402a724:	aa1303e1 	mov	x1, x19
 402a728:	b9000fe4 	str	w4, [sp, #12]
 402a72c:	97fff983 	bl	4028d38 <memcpy_s>
 402a730:	8b13037b 	add	x27, x27, x19
 402a734:	b9400fe4 	ldr	w4, [sp, #12]
 402a738:	0b0402b5 	add	w21, w21, w4
 402a73c:	4b040294 	sub	w20, w20, w4
 402a740:	17ffffd5 	b	402a694 <emmc_read+0x64>
 402a744:	a94553f3 	ldp	x19, x20, [sp, #80]
 402a748:	a9465bf5 	ldp	x21, x22, [sp, #96]
 402a74c:	a94763f7 	ldp	x23, x24, [sp, #112]
 402a750:	a9486bf9 	ldp	x25, x26, [sp, #128]
 402a754:	a94973fb 	ldp	x27, x28, [sp, #144]
 402a758:	f94053fe 	ldr	x30, [sp, #160]
 402a75c:	9102c3ff 	add	sp, sp, #0xb0
 402a760:	d65f03c0 	ret

000000000402a764 <ddrtrn_training_boot_func>:
 402a764:	d10183ff 	sub	sp, sp, #0x60
 402a768:	f0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402a76c:	a9057bf3 	stp	x19, x30, [sp, #80]
 402a770:	f9477001 	ldr	x1, [x0, #3808]
 402a774:	f90027e1 	str	x1, [sp, #72]
 402a778:	d2800001 	mov	x1, #0x0                   	// #0
 402a77c:	940000f8 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a780:	b9400400 	ldr	w0, [x0, #4]
 402a784:	940008b8 	bl	402ca64 <ddrtrn_hal_get_gt_status>
 402a788:	340000e0 	cbz	w0, 402a7a4 <ddrtrn_training_boot_func+0x40>
 402a78c:	940000f4 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a790:	b9400401 	ldr	w1, [x0, #4]
 402a794:	12800003 	mov	w3, #0xffffffff            	// #-1
 402a798:	52800040 	mov	w0, #0x2                   	// #2
 402a79c:	2a0303e2 	mov	w2, w3
 402a7a0:	940007a2 	bl	402c628 <ddrtrn_hal_training_stat>
 402a7a4:	9400108d 	bl	402e9d8 <ddrtrn_wl_func>
 402a7a8:	2a0003f3 	mov	w19, w0
 402a7ac:	94000a7f 	bl	402d1a8 <ddrtrn_dataeye_training_func>
 402a7b0:	2b000273 	adds	w19, w19, w0
 402a7b4:	540001e0 	b.eq	402a7f0 <ddrtrn_training_boot_func+0x8c>  // b.none
 402a7b8:	52a00200 	mov	w0, #0x100000              	// #1048576
 402a7bc:	9400050f 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 402a7c0:	35000180 	cbnz	w0, 402a7f0 <ddrtrn_training_boot_func+0x8c>
 402a7c4:	910023e0 	add	x0, sp, #0x8
 402a7c8:	52a00201 	mov	w1, #0x100000              	// #1048576
 402a7cc:	94000338 	bl	402b4ac <ddrtrn_hal_save_reg>
 402a7d0:	94000df3 	bl	402df9c <ddrtrn_hw_dataeye_read>
 402a7d4:	2a0003f3 	mov	w19, w0
 402a7d8:	910023e0 	add	x0, sp, #0x8
 402a7dc:	94000378 	bl	402b5bc <ddrtrn_hal_restore_reg>
 402a7e0:	52800020 	mov	w0, #0x1                   	// #1
 402a7e4:	94000925 	bl	402cc78 <ddrtrn_hal_set_adjust>
 402a7e8:	94000a4a 	bl	402d110 <ddrtrn_dataeye_training>
 402a7ec:	0b000273 	add	w19, w19, w0
 402a7f0:	94000dde 	bl	402df68 <ddrtrn_gating_func>
 402a7f4:	0b000273 	add	w19, w19, w0
 402a7f8:	9400104b 	bl	402e924 <ddrtrn_vref_training_func>
 402a7fc:	0b000260 	add	w0, w19, w0
 402a800:	f0ffffa1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402a804:	f94027e2 	ldr	x2, [sp, #72]
 402a808:	f9477023 	ldr	x3, [x1, #3808]
 402a80c:	eb030042 	subs	x2, x2, x3
 402a810:	d2800003 	mov	x3, #0x0                   	// #0
 402a814:	54000040 	b.eq	402a81c <ddrtrn_training_boot_func+0xb8>  // b.none
 402a818:	97ffee79 	bl	40261fc <__stack_chk_fail>
 402a81c:	a9457bf3 	ldp	x19, x30, [sp, #80]
 402a820:	910183ff 	add	sp, sp, #0x60
 402a824:	d65f03c0 	ret

000000000402a828 <ddrtrn_training_by_dmc>:
 402a828:	f81f0ffe 	str	x30, [sp, #-16]!
 402a82c:	940000cc 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a830:	b9402c00 	ldr	w0, [x0, #44]
 402a834:	35000060 	cbnz	w0, 402a840 <ddrtrn_training_by_dmc+0x18>
 402a838:	f84107fe 	ldr	x30, [sp], #16
 402a83c:	17ffffca 	b	402a764 <ddrtrn_training_boot_func>
 402a840:	52800000 	mov	w0, #0x0                   	// #0
 402a844:	f84107fe 	ldr	x30, [sp], #16
 402a848:	d65f03c0 	ret

000000000402a84c <ddrtrn_training_by_rank>:
 402a84c:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402a850:	a9015bf5 	stp	x21, x22, [sp, #16]
 402a854:	52800015 	mov	w21, #0x0                   	// #0
 402a858:	d2800796 	mov	x22, #0x3c                  	// #60
 402a85c:	a90263f7 	stp	x23, x24, [sp, #32]
 402a860:	d2800197 	mov	x23, #0xc                   	// #12
 402a864:	f9001bfe 	str	x30, [sp, #48]
 402a868:	940000bd 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a86c:	b9400413 	ldr	w19, [x0, #4]
 402a870:	940000bb 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a874:	b9402401 	ldr	w1, [x0, #36]
 402a878:	2a1303e0 	mov	w0, w19
 402a87c:	52800013 	mov	w19, #0x0                   	// #0
 402a880:	940008bc 	bl	402cb70 <ddrtrn_hal_phy_switch_rank>
 402a884:	940000b6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a888:	aa0003f4 	mov	x20, x0
 402a88c:	940000af 	bl	402ab48 <ddrtrn_hal_get_phy>
 402a890:	b9402281 	ldr	w1, [x20, #32]
 402a894:	9b160021 	madd	x1, x1, x22, x0
 402a898:	b9400820 	ldr	w0, [x1, #8]
 402a89c:	6b00027f 	cmp	w19, w0
 402a8a0:	540000e3 	b.cc	402a8bc <ddrtrn_training_by_rank+0x70>  // b.lo, b.ul, b.last
 402a8a4:	2a1503e0 	mov	w0, w21
 402a8a8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402a8ac:	a94263f7 	ldp	x23, x24, [sp, #32]
 402a8b0:	f9401bfe 	ldr	x30, [sp, #48]
 402a8b4:	a8c453f3 	ldp	x19, x20, [sp], #64
 402a8b8:	d65f03c0 	ret
 402a8bc:	940000a8 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a8c0:	b9002813 	str	w19, [x0, #40]
 402a8c4:	2a1303f4 	mov	w20, w19
 402a8c8:	11000673 	add	w19, w19, #0x1
 402a8cc:	940000a4 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a8d0:	aa0003f8 	mov	x24, x0
 402a8d4:	9400009d 	bl	402ab48 <ddrtrn_hal_get_phy>
 402a8d8:	9b177e94 	mul	x20, x20, x23
 402a8dc:	b9402301 	ldr	w1, [x24, #32]
 402a8e0:	9b165021 	madd	x1, x1, x22, x20
 402a8e4:	8b010000 	add	x0, x0, x1
 402a8e8:	b9401418 	ldr	w24, [x0, #20]
 402a8ec:	9400009c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a8f0:	b9000818 	str	w24, [x0, #8]
 402a8f4:	9400009a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a8f8:	aa0003f8 	mov	x24, x0
 402a8fc:	94000093 	bl	402ab48 <ddrtrn_hal_get_phy>
 402a900:	b9402301 	ldr	w1, [x24, #32]
 402a904:	9b165034 	madd	x20, x1, x22, x20
 402a908:	8b140000 	add	x0, x0, x20
 402a90c:	b9401c14 	ldr	w20, [x0, #28]
 402a910:	94000093 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a914:	b9001014 	str	w20, [x0, #16]
 402a918:	97ffffc4 	bl	402a828 <ddrtrn_training_by_dmc>
 402a91c:	0b0002b5 	add	w21, w21, w0
 402a920:	17ffffd9 	b	402a884 <ddrtrn_training_by_rank+0x38>

000000000402a924 <ddrtrn_training_by_phy>:
 402a924:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402a928:	52800014 	mov	w20, #0x0                   	// #0
 402a92c:	a9015bf5 	stp	x21, x22, [sp, #16]
 402a930:	52800035 	mov	w21, #0x1                   	// #1
 402a934:	d2800796 	mov	x22, #0x3c                  	// #60
 402a938:	a90263f7 	stp	x23, x24, [sp, #32]
 402a93c:	f9001bfe 	str	x30, [sp, #48]
 402a940:	94000087 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a944:	b9402000 	ldr	w0, [x0, #32]
 402a948:	1ac022b5 	lsl	w21, w21, w0
 402a94c:	94000084 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a950:	aa0003f3 	mov	x19, x0
 402a954:	9400007d 	bl	402ab48 <ddrtrn_hal_get_phy>
 402a958:	b9402261 	ldr	w1, [x19, #32]
 402a95c:	52800013 	mov	w19, #0x0                   	// #0
 402a960:	9b160021 	madd	x1, x1, x22, x0
 402a964:	b9400c37 	ldr	w23, [x1, #12]
 402a968:	6b17027f 	cmp	w19, w23
 402a96c:	540001e1 	b.ne	402a9a8 <ddrtrn_training_by_phy+0x84>  // b.any
 402a970:	71000a7f 	cmp	w19, #0x2
 402a974:	540000e1 	b.ne	402a990 <ddrtrn_training_by_phy+0x6c>  // b.any
 402a978:	940005ab 	bl	402c024 <ddrtrn_hal_training_adjust_wdq>
 402a97c:	9400065c 	bl	402c2ec <ddrtrn_hal_training_adjust_wdqs>
 402a980:	94000077 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a984:	b9400400 	ldr	w0, [x0, #4]
 402a988:	52800001 	mov	w1, #0x0                   	// #0
 402a98c:	94000879 	bl	402cb70 <ddrtrn_hal_phy_switch_rank>
 402a990:	2a1403e0 	mov	w0, w20
 402a994:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402a998:	a94263f7 	ldp	x23, x24, [sp, #32]
 402a99c:	f9401bfe 	ldr	x30, [sp, #48]
 402a9a0:	a8c453f3 	ldp	x19, x20, [sp], #64
 402a9a4:	d65f03c0 	ret
 402a9a8:	9400006d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a9ac:	b9002413 	str	w19, [x0, #36]
 402a9b0:	9400006b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a9b4:	aa0003f8 	mov	x24, x0
 402a9b8:	94000064 	bl	402ab48 <ddrtrn_hal_get_phy>
 402a9bc:	d37d7e61 	ubfiz	x1, x19, #3, #32
 402a9c0:	b9402302 	ldr	w2, [x24, #32]
 402a9c4:	91008021 	add	x1, x1, #0x20
 402a9c8:	9b160441 	madd	x1, x2, x22, x1
 402a9cc:	8b010000 	add	x0, x0, x1
 402a9d0:	b9400c18 	ldr	w24, [x0, #12]
 402a9d4:	94000062 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402a9d8:	b9000c18 	str	w24, [x0, #12]
 402a9dc:	2a1503e0 	mov	w0, w21
 402a9e0:	94000486 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 402a9e4:	35000060 	cbnz	w0, 402a9f0 <ddrtrn_training_by_phy+0xcc>
 402a9e8:	97ffff99 	bl	402a84c <ddrtrn_training_by_rank>
 402a9ec:	0b000294 	add	w20, w20, w0
 402a9f0:	11000673 	add	w19, w19, #0x1
 402a9f4:	17ffffdd 	b	402a968 <ddrtrn_training_by_phy+0x44>

000000000402a9f8 <ddrtrn_training_all>:
 402a9f8:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402a9fc:	52800013 	mov	w19, #0x0                   	// #0
 402aa00:	52800014 	mov	w20, #0x0                   	// #0
 402aa04:	a9015bf5 	stp	x21, x22, [sp, #16]
 402aa08:	d2800795 	mov	x21, #0x3c                  	// #60
 402aa0c:	f90013fe 	str	x30, [sp, #32]
 402aa10:	94000053 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402aa14:	b9400000 	ldr	w0, [x0]
 402aa18:	6b00027f 	cmp	w19, w0
 402aa1c:	540000c3 	b.cc	402aa34 <ddrtrn_training_all+0x3c>  // b.lo, b.ul, b.last
 402aa20:	2a1403e0 	mov	w0, w20
 402aa24:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402aa28:	f94013fe 	ldr	x30, [sp, #32]
 402aa2c:	a8c353f3 	ldp	x19, x20, [sp], #48
 402aa30:	d65f03c0 	ret
 402aa34:	9400004a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402aa38:	b9002013 	str	w19, [x0, #32]
 402aa3c:	94000043 	bl	402ab48 <ddrtrn_hal_get_phy>
 402aa40:	2a1303e1 	mov	w1, w19
 402aa44:	11000673 	add	w19, w19, #0x1
 402aa48:	9b157c21 	mul	x1, x1, x21
 402aa4c:	b8616816 	ldr	w22, [x0, x1]
 402aa50:	94000043 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402aa54:	b9000416 	str	w22, [x0, #4]
 402aa58:	97ffffb3 	bl	402a924 <ddrtrn_training_by_phy>
 402aa5c:	0b000294 	add	w20, w20, w0
 402aa60:	17ffffec 	b	402aa10 <ddrtrn_training_all+0x18>

000000000402aa64 <ddrtrn_sw_training_func>:
 402aa64:	d100c3ff 	sub	sp, sp, #0x30
 402aa68:	f0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402aa6c:	a9027bf3 	stp	x19, x30, [sp, #32]
 402aa70:	f9477001 	ldr	x1, [x0, #3808]
 402aa74:	f9000fe1 	str	x1, [sp, #24]
 402aa78:	d2800001 	mov	x1, #0x0                   	// #0
 402aa7c:	9400085c 	bl	402cbec <ddrtrn_hal_version_flag>
 402aa80:	94000861 	bl	402cc04 <ddrtrn_hal_check_sw_item>
 402aa84:	2a0003f3 	mov	w19, w0
 402aa88:	340002e0 	cbz	w0, 402aae4 <ddrtrn_sw_training_func+0x80>
 402aa8c:	94000fe1 	bl	402ea10 <ddrtrn_training_start>
 402aa90:	52800202 	mov	w2, #0x10                  	// #16
 402aa94:	52800001 	mov	w1, #0x0                   	// #0
 402aa98:	910023e0 	add	x0, sp, #0x8
 402aa9c:	94000e24 	bl	402e32c <ddrtrn_set_data>
 402aaa0:	910023e0 	add	x0, sp, #0x8
 402aaa4:	940001f5 	bl	402b278 <ddrtrn_hal_boot_cmd_save>
 402aaa8:	2a0003f3 	mov	w19, w0
 402aaac:	350002e0 	cbnz	w0, 402ab08 <ddrtrn_sw_training_func+0xa4>
 402aab0:	9400087a 	bl	402cc98 <ddrtrn_hal_clear_sysctrl_stat_reg>
 402aab4:	9400002a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402aab8:	b9002c1f 	str	wzr, [x0, #44]
 402aabc:	9400085f 	bl	402cc38 <ddrtrn_hal_check_not_dcc_item>
 402aac0:	35000060 	cbnz	w0, 402aacc <ddrtrn_sw_training_func+0x68>
 402aac4:	97ffffcd 	bl	402a9f8 <ddrtrn_training_all>
 402aac8:	2a0003f3 	mov	w19, w0
 402aacc:	94000bb8 	bl	402d9ac <ddrtrn_dcc_training_func>
 402aad0:	2b130013 	adds	w19, w0, w19
 402aad4:	54000161 	b.ne	402ab00 <ddrtrn_sw_training_func+0x9c>  // b.any
 402aad8:	94000fcd 	bl	402ea0c <ddrtrn_training_success>
 402aadc:	910023e0 	add	x0, sp, #0x8
 402aae0:	9400020d 	bl	402b314 <ddrtrn_hal_boot_cmd_restore>
 402aae4:	f0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402aae8:	f9400fe1 	ldr	x1, [sp, #24]
 402aaec:	f9477002 	ldr	x2, [x0, #3808]
 402aaf0:	eb020021 	subs	x1, x1, x2
 402aaf4:	d2800002 	mov	x2, #0x0                   	// #0
 402aaf8:	540000c0 	b.eq	402ab10 <ddrtrn_sw_training_func+0xac>  // b.none
 402aafc:	97ffedc0 	bl	40261fc <__stack_chk_fail>
 402ab00:	94000872 	bl	402ccc8 <ddrtrn_training_console_if>
 402ab04:	17fffff6 	b	402aadc <ddrtrn_sw_training_func+0x78>
 402ab08:	12800013 	mov	w19, #0xffffffff            	// #-1
 402ab0c:	17fffff6 	b	402aae4 <ddrtrn_sw_training_func+0x80>
 402ab10:	2a1303e0 	mov	w0, w19
 402ab14:	a9427bf3 	ldp	x19, x30, [sp, #32]
 402ab18:	9100c3ff 	add	sp, sp, #0x30
 402ab1c:	d65f03c0 	ret

000000000402ab20 <ddrtrn_hw_training_func>:
 402ab20:	14000ddf 	b	402e29c <ddrtrn_hw_training>

000000000402ab24 <ddrtrn_sw_training_if>:
 402ab24:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402ab28:	2a0003f3 	mov	w19, w0
 402ab2c:	94000018 	bl	402ab8c <ddrtrn_hal_cfg_init>
 402ab30:	2a1303e0 	mov	w0, w19
 402ab34:	9400041f 	bl	402bbb0 <ddrtrn_hal_sw_item_cfg>
 402ab38:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402ab3c:	17ffffca 	b	402aa64 <ddrtrn_sw_training_func>

000000000402ab40 <ddrtrn_hw_training_if>:
 402ab40:	14000dd7 	b	402e29c <ddrtrn_hw_training>

000000000402ab44 <ddrtrn_trace>:
 402ab44:	d65f03c0 	ret

000000000402ab48 <ddrtrn_hal_get_phy>:
 402ab48:	d2801780 	mov	x0, #0xbc                  	// #188
 402ab4c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ab50:	b9400000 	ldr	w0, [x0]
 402ab54:	2a0003e0 	mov	w0, w0
 402ab58:	d65f03c0 	ret

000000000402ab5c <ddrtrn_hal_get_ctx>:
 402ab5c:	d2801700 	mov	x0, #0xb8                  	// #184
 402ab60:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ab64:	b9400000 	ldr	w0, [x0]
 402ab68:	2a0003e0 	mov	w0, w0
 402ab6c:	d65f03c0 	ret

000000000402ab70 <ddrtrn_hal_set_cfg_addr>:
 402ab70:	d2801702 	mov	x2, #0xb8                  	// #184
 402ab74:	f2a22042 	movk	x2, #0x1102, lsl #16
 402ab78:	b9000040 	str	w0, [x2]
 402ab7c:	d2801780 	mov	x0, #0xbc                  	// #188
 402ab80:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ab84:	b9000001 	str	w1, [x0]
 402ab88:	d65f03c0 	ret

000000000402ab8c <ddrtrn_hal_cfg_init>:
 402ab8c:	d2801700 	mov	x0, #0xb8                  	// #184
 402ab90:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402ab94:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ab98:	52800682 	mov	w2, #0x34                  	// #52
 402ab9c:	52800001 	mov	w1, #0x0                   	// #0
 402aba0:	b9400014 	ldr	w20, [x0]
 402aba4:	b9400413 	ldr	w19, [x0, #4]
 402aba8:	2a1403f4 	mov	w20, w20
 402abac:	f9000bfe 	str	x30, [sp, #16]
 402abb0:	2a1303f3 	mov	w19, w19
 402abb4:	aa1403e0 	mov	x0, x20
 402abb8:	94000ddd 	bl	402e32c <ddrtrn_set_data>
 402abbc:	52800782 	mov	w2, #0x3c                  	// #60
 402abc0:	52800001 	mov	w1, #0x0                   	// #0
 402abc4:	aa1303e0 	mov	x0, x19
 402abc8:	94000dd9 	bl	402e32c <ddrtrn_set_data>
 402abcc:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402abd0:	52800022 	mov	w2, #0x1                   	// #1
 402abd4:	b9000282 	str	w2, [x20]
 402abd8:	d2801501 	mov	x1, #0xa8                  	// #168
 402abdc:	b9000260 	str	w0, [x19]
 402abe0:	d2800580 	mov	x0, #0x2c                  	// #44
 402abe4:	f2a222a0 	movk	x0, #0x1115, lsl #16
 402abe8:	f2a22041 	movk	x1, #0x1102, lsl #16
 402abec:	b9400000 	ldr	w0, [x0]
 402abf0:	12000c00 	and	w0, w0, #0xf
 402abf4:	b9000660 	str	w0, [x19, #4]
 402abf8:	7100181f 	cmp	w0, #0x6
 402abfc:	52900000 	mov	w0, #0x8000                	// #32768
 402ac00:	72a22280 	movk	w0, #0x1114, lsl #16
 402ac04:	54000581 	b.ne	402acb4 <ddrtrn_hal_cfg_init+0x128>  // b.any
 402ac08:	52800042 	mov	w2, #0x2                   	// #2
 402ac0c:	b9000a62 	str	w2, [x19, #8]
 402ac10:	b9400034 	ldr	w20, [x1]
 402ac14:	b9001660 	str	w0, [x19, #20]
 402ac18:	12003e81 	and	w1, w20, #0xffff
 402ac1c:	b9001e61 	str	w1, [x19, #28]
 402ac20:	53107e94 	lsr	w20, w20, #16
 402ac24:	940002de 	bl	402b79c <ddrtrn_hal_phy_get_byte_num>
 402ac28:	b9001a60 	str	w0, [x19, #24]
 402ac2c:	52920000 	mov	w0, #0x9000                	// #36864
 402ac30:	b9002a74 	str	w20, [x19, #40]
 402ac34:	72a22280 	movk	w0, #0x1114, lsl #16
 402ac38:	b9002260 	str	w0, [x19, #32]
 402ac3c:	940002d8 	bl	402b79c <ddrtrn_hal_phy_get_byte_num>
 402ac40:	b9002660 	str	w0, [x19, #36]
 402ac44:	b9401a61 	ldr	w1, [x19, #24]
 402ac48:	0b000020 	add	w0, w1, w0
 402ac4c:	52800021 	mov	w1, #0x1                   	// #1
 402ac50:	f9400bfe 	ldr	x30, [sp, #16]
 402ac54:	29018261 	stp	w1, w0, [x19, #12]
 402ac58:	d2801400 	mov	x0, #0xa0                  	// #160
 402ac5c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ac60:	b9400000 	ldr	w0, [x0]
 402ac64:	b9002e60 	str	w0, [x19, #44]
 402ac68:	d2801200 	mov	x0, #0x90                  	// #144
 402ac6c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ac70:	b9400000 	ldr	w0, [x0]
 402ac74:	b9003260 	str	w0, [x19, #48]
 402ac78:	d2801480 	mov	x0, #0xa4                  	// #164
 402ac7c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ac80:	b9400000 	ldr	w0, [x0]
 402ac84:	b9003660 	str	w0, [x19, #52]
 402ac88:	d2801280 	mov	x0, #0x94                  	// #148
 402ac8c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402ac90:	b9400002 	ldr	w2, [x0]
 402ac94:	b9003a62 	str	w2, [x19, #56]
 402ac98:	b9400000 	ldr	w0, [x0]
 402ac9c:	7100001f 	cmp	w0, #0x0
 402aca0:	52800040 	mov	w0, #0x2                   	// #2
 402aca4:	1a811000 	csel	w0, w0, w1, ne  // ne = any
 402aca8:	b9000e60 	str	w0, [x19, #12]
 402acac:	a8c253f3 	ldp	x19, x20, [sp], #32
 402acb0:	d65f03c0 	ret
 402acb4:	b9000a62 	str	w2, [x19, #8]
 402acb8:	b9001660 	str	w0, [x19, #20]
 402acbc:	b9400021 	ldr	w1, [x1]
 402acc0:	b9001e61 	str	w1, [x19, #28]
 402acc4:	940002b6 	bl	402b79c <ddrtrn_hal_phy_get_byte_num>
 402acc8:	b9001a60 	str	w0, [x19, #24]
 402accc:	17ffffe0 	b	402ac4c <ddrtrn_hal_cfg_init+0xc0>

000000000402acd0 <ddrtrn_hal_get_cur_phy_rank_num>:
 402acd0:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402acd4:	97ffffa2 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402acd8:	aa0003f3 	mov	x19, x0
 402acdc:	97ffff9b 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ace0:	d2800782 	mov	x2, #0x3c                  	// #60
 402ace4:	b9402261 	ldr	w1, [x19, #32]
 402ace8:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402acec:	d503201f 	nop
 402acf0:	9b020021 	madd	x1, x1, x2, x0
 402acf4:	b9400c20 	ldr	w0, [x1, #12]
 402acf8:	d65f03c0 	ret

000000000402acfc <ddrtrn_hal_get_cur_phy_dram_type>:
 402acfc:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402ad00:	97ffff97 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ad04:	aa0003f3 	mov	x19, x0
 402ad08:	97ffff90 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ad0c:	d2800782 	mov	x2, #0x3c                  	// #60
 402ad10:	b9402261 	ldr	w1, [x19, #32]
 402ad14:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402ad18:	d503201f 	nop
 402ad1c:	9b020021 	madd	x1, x1, x2, x0
 402ad20:	b9400420 	ldr	w0, [x1, #4]
 402ad24:	d65f03c0 	ret

000000000402ad28 <ddrtrn_hal_get_cur_phy_dmc_num>:
 402ad28:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402ad2c:	97ffff8c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ad30:	aa0003f3 	mov	x19, x0
 402ad34:	97ffff85 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ad38:	d2800782 	mov	x2, #0x3c                  	// #60
 402ad3c:	b9402261 	ldr	w1, [x19, #32]
 402ad40:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402ad44:	d503201f 	nop
 402ad48:	9b020021 	madd	x1, x1, x2, x0
 402ad4c:	b9400820 	ldr	w0, [x1, #8]
 402ad50:	d65f03c0 	ret

000000000402ad54 <ddrtrn_hal_get_cur_dmc_addr>:
 402ad54:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402ad58:	2a0003f3 	mov	w19, w0
 402ad5c:	f9000bfe 	str	x30, [sp, #16]
 402ad60:	97ffff7f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ad64:	aa0003f4 	mov	x20, x0
 402ad68:	97ffff78 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ad6c:	d2800782 	mov	x2, #0x3c                  	// #60
 402ad70:	f9400bfe 	ldr	x30, [sp, #16]
 402ad74:	b9402281 	ldr	w1, [x20, #32]
 402ad78:	9b027c21 	mul	x1, x1, x2
 402ad7c:	d2800182 	mov	x2, #0xc                   	// #12
 402ad80:	9b020673 	madd	x19, x19, x2, x1
 402ad84:	8b130013 	add	x19, x0, x19
 402ad88:	b9401660 	ldr	w0, [x19, #20]
 402ad8c:	a8c253f3 	ldp	x19, x20, [sp], #32
 402ad90:	d65f03c0 	ret

000000000402ad94 <ddrtrn_winding_identification>:
 402ad94:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402ad98:	2a0103f4 	mov	w20, w1
 402ad9c:	2a0003f3 	mov	w19, w0
 402ada0:	a9015bf5 	stp	x21, x22, [sp, #16]
 402ada4:	2a0203f5 	mov	w21, w2
 402ada8:	a90263f7 	stp	x23, x24, [sp, #32]
 402adac:	52800017 	mov	w23, #0x0                   	// #0
 402adb0:	f9001bfe 	str	x30, [sp, #48]
 402adb4:	97ffffc7 	bl	402acd0 <ddrtrn_hal_get_cur_phy_rank_num>
 402adb8:	2a0003f6 	mov	w22, w0
 402adbc:	97ffffdb 	bl	402ad28 <ddrtrn_hal_get_cur_phy_dmc_num>
 402adc0:	2a0003f8 	mov	w24, w0
 402adc4:	6b17031f 	cmp	w24, w23
 402adc8:	54000261 	b.ne	402ae14 <ddrtrn_winding_identification+0x80>  // b.any
 402adcc:	d2a80005 	mov	x5, #0x40000000            	// #1073741824
 402add0:	528acf07 	mov	w7, #0x5678                	// #22136
 402add4:	529530e6 	mov	w6, #0xa987                	// #43399
 402add8:	2a0503e8 	mov	w8, w5
 402addc:	52800004 	mov	w4, #0x0                   	// #0
 402ade0:	72a24687 	movk	w7, #0x1234, lsl #16
 402ade4:	72bdb966 	movk	w6, #0xedcb, lsl #16
 402ade8:	11000484 	add	w4, w4, #0x1
 402adec:	b90000a7 	str	w7, [x5]
 402adf0:	1b137c81 	mul	w1, w4, w19
 402adf4:	6b14003f 	cmp	w1, w20
 402adf8:	540002c3 	b.cc	402ae50 <ddrtrn_winding_identification+0xbc>  // b.lo, b.ul, b.last
 402adfc:	52a00800 	mov	w0, #0x400000              	// #4194304
 402ae00:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402ae04:	a94263f7 	ldp	x23, x24, [sp, #32]
 402ae08:	f9401bfe 	ldr	x30, [sp, #48]
 402ae0c:	a8c453f3 	ldp	x19, x20, [sp], #64
 402ae10:	d65f03c0 	ret
 402ae14:	2a1703e0 	mov	w0, w23
 402ae18:	97ffffcf 	bl	402ad54 <ddrtrn_hal_get_cur_dmc_addr>
 402ae1c:	11014001 	add	w1, w0, #0x50
 402ae20:	b9400023 	ldr	w3, [x1]
 402ae24:	710006df 	cmp	w22, #0x1
 402ae28:	540000a1 	b.ne	402ae3c <ddrtrn_winding_identification+0xa8>  // b.any
 402ae2c:	120a7463 	and	w3, w3, #0xffcfffff
 402ae30:	b9000023 	str	w3, [x1]
 402ae34:	110006f7 	add	w23, w23, #0x1
 402ae38:	17ffffe3 	b	402adc4 <ddrtrn_winding_identification+0x30>
 402ae3c:	71000adf 	cmp	w22, #0x2
 402ae40:	54ffffa1 	b.ne	402ae34 <ddrtrn_winding_identification+0xa0>  // b.any
 402ae44:	120a7463 	and	w3, w3, #0xffcfffff
 402ae48:	320c0063 	orr	w3, w3, #0x100000
 402ae4c:	17fffff9 	b	402ae30 <ddrtrn_winding_identification+0x9c>
 402ae50:	0b012902 	add	w2, w8, w1, lsl #10
 402ae54:	0b130023 	add	w3, w1, w19
 402ae58:	b9400040 	ldr	w0, [x2]
 402ae5c:	4a070000 	eor	w0, w0, w7
 402ae60:	6a15001f 	tst	w0, w21
 402ae64:	54000080 	b.eq	402ae74 <ddrtrn_winding_identification+0xe0>  // b.none
 402ae68:	11000484 	add	w4, w4, #0x1
 402ae6c:	2a0303e1 	mov	w1, w3
 402ae70:	17ffffe1 	b	402adf4 <ddrtrn_winding_identification+0x60>
 402ae74:	b90000a6 	str	w6, [x5]
 402ae78:	b9400043 	ldr	w3, [x2]
 402ae7c:	4a060063 	eor	w3, w3, w6
 402ae80:	6a150060 	ands	w0, w3, w21
 402ae84:	54000061 	b.ne	402ae90 <ddrtrn_winding_identification+0xfc>  // b.any
 402ae88:	7100049f 	cmp	w4, #0x1
 402ae8c:	54fffba0 	b.eq	402ae00 <ddrtrn_winding_identification+0x6c>  // b.none
 402ae90:	b9400040 	ldr	w0, [x2]
 402ae94:	4a060000 	eor	w0, w0, w6
 402ae98:	6a15001f 	tst	w0, w21
 402ae9c:	54fffa61 	b.ne	402ade8 <ddrtrn_winding_identification+0x54>  // b.any
 402aea0:	2a0103e0 	mov	w0, w1
 402aea4:	17ffffd7 	b	402ae00 <ddrtrn_winding_identification+0x6c>

000000000402aea8 <ddrtrn_capat_adapt_func>:
 402aea8:	d10343ff 	sub	sp, sp, #0xd0
 402aeac:	f0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402aeb0:	a90753f3 	stp	x19, x20, [sp, #112]
 402aeb4:	d2800013 	mov	x19, #0x0                   	// #0
 402aeb8:	a9085bf5 	stp	x21, x22, [sp, #128]
 402aebc:	9100c3f5 	add	x21, sp, #0x30
 402aec0:	a90963f7 	stp	x23, x24, [sp, #144]
 402aec4:	52974597 	mov	w23, #0xba2c                	// #47660
 402aec8:	72bddd77 	movk	w23, #0xeeeb, lsl #16
 402aecc:	a90a6bf9 	stp	x25, x26, [sp, #160]
 402aed0:	1281eef8 	mov	w24, #0xfffff088            	// #-3960
 402aed4:	a90b73fb 	stp	x27, x28, [sp, #176]
 402aed8:	f90063fe 	str	x30, [sp, #192]
 402aedc:	f9477001 	ldr	x1, [x0, #3808]
 402aee0:	f90037e1 	str	x1, [sp, #104]
 402aee4:	d2800001 	mov	x1, #0x0                   	// #0
 402aee8:	97ffff29 	bl	402ab8c <ddrtrn_hal_cfg_init>
 402aeec:	97ffff1c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402aef0:	b940001c 	ldr	w28, [x0]
 402aef4:	52800282 	mov	w2, #0x14                  	// #20
 402aef8:	9100c3e0 	add	x0, sp, #0x30
 402aefc:	52800001 	mov	w1, #0x0                   	// #0
 402af00:	94000d0b 	bl	402e32c <ddrtrn_set_data>
 402af04:	6b13039f 	cmp	w28, w19
 402af08:	54000268 	b.hi	402af54 <ddrtrn_capat_adapt_func+0xac>  // b.pmore
 402af0c:	97ffff7c 	bl	402acfc <ddrtrn_hal_get_cur_phy_dram_type>
 402af10:	7100281f 	cmp	w0, #0xa
 402af14:	54000061 	b.ne	402af20 <ddrtrn_capat_adapt_func+0x78>  // b.any
 402af18:	9100c3e0 	add	x0, sp, #0x30
 402af1c:	94000b32 	bl	402dbe4 <ddrtrn_hal_timing8_trfc_ab_cfg>
 402af20:	97ffff77 	bl	402acfc <ddrtrn_hal_get_cur_phy_dram_type>
 402af24:	7100181f 	cmp	w0, #0x6
 402af28:	54001860 	b.eq	402b234 <ddrtrn_capat_adapt_func+0x38c>  // b.none
 402af2c:	9100c3e0 	add	x0, sp, #0x30
 402af30:	94000edf 	bl	402eaac <ddrtrn_capat_adpat_cfg_nonlpddr4>
 402af34:	f0ffffa1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402af38:	b9403fe0 	ldr	w0, [sp, #60]
 402af3c:	f94037e2 	ldr	x2, [sp, #104]
 402af40:	f9477023 	ldr	x3, [x1, #3808]
 402af44:	eb030042 	subs	x2, x2, x3
 402af48:	d2800003 	mov	x3, #0x0                   	// #0
 402af4c:	54001800 	b.eq	402b24c <ddrtrn_capat_adapt_func+0x3a4>  // b.none
 402af50:	97ffecab 	bl	40261fc <__stack_chk_fail>
 402af54:	97ffff02 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402af58:	b9002013 	str	w19, [x0, #32]
 402af5c:	97fffefb 	bl	402ab48 <ddrtrn_hal_get_phy>
 402af60:	2a1303e1 	mov	w1, w19
 402af64:	d2800782 	mov	x2, #0x3c                  	// #60
 402af68:	9b027c21 	mul	x1, x1, x2
 402af6c:	b8616814 	ldr	w20, [x0, x1]
 402af70:	97fffefb 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402af74:	b9000414 	str	w20, [x0, #4]
 402af78:	97ffff56 	bl	402acd0 <ddrtrn_hal_get_cur_phy_rank_num>
 402af7c:	9100c3e1 	add	x1, sp, #0x30
 402af80:	8b130821 	add	x1, x1, x19, lsl #2
 402af84:	b9001020 	str	w0, [x1, #16]
 402af88:	97fffef5 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402af8c:	b9402019 	ldr	w25, [x0, #32]
 402af90:	94000ea1 	bl	402ea14 <ddrtrn_chsel_remap_func>
 402af94:	d2a80000 	mov	x0, #0x40000000            	// #1073741824
 402af98:	b940001b 	ldr	w27, [x0]
 402af9c:	97ffff58 	bl	402acfc <ddrtrn_hal_get_cur_phy_dram_type>
 402afa0:	7100281f 	cmp	w0, #0xa
 402afa4:	54000741 	b.ne	402b08c <ddrtrn_capat_adapt_func+0x1e4>  // b.any
 402afa8:	97ffff60 	bl	402ad28 <ddrtrn_hal_get_cur_phy_dmc_num>
 402afac:	910123f4 	add	x20, sp, #0x48
 402afb0:	2a0003f6 	mov	w22, w0
 402afb4:	97ffff47 	bl	402acd0 <ddrtrn_hal_get_cur_phy_rank_num>
 402afb8:	5288c684 	mov	w4, #0x4634                	// #17972
 402afbc:	2a0003e9 	mov	w9, w0
 402afc0:	aa1403e8 	mov	x8, x20
 402afc4:	72a22284 	movk	w4, #0x1114, lsl #16
 402afc8:	5280001a 	mov	w26, #0x0                   	// #0
 402afcc:	6b1a013f 	cmp	w9, w26
 402afd0:	54000500 	b.eq	402b070 <ddrtrn_capat_adapt_func+0x1c8>  // b.none
 402afd4:	0b17008b 	add	w11, w4, w23
 402afd8:	aa0803e5 	mov	x5, x8
 402afdc:	2a0403e7 	mov	w7, w4
 402afe0:	52800003 	mov	w3, #0x0                   	// #0
 402afe4:	1400001d 	b	402b058 <ddrtrn_capat_adapt_func+0x1b0>
 402afe8:	2a0303e0 	mov	w0, w3
 402afec:	2901a7e3 	stp	w3, w9, [sp, #12]
 402aff0:	b90017eb 	str	w11, [sp, #20]
 402aff4:	f9000fe5 	str	x5, [sp, #24]
 402aff8:	290413e7 	stp	w7, w4, [sp, #32]
 402affc:	f90017e8 	str	x8, [sp, #40]
 402b000:	97ffff55 	bl	402ad54 <ddrtrn_hal_get_cur_dmc_addr>
 402b004:	294413e7 	ldp	w7, w4, [sp, #32]
 402b008:	52814c4a 	mov	w10, #0xa62                 	// #2658
 402b00c:	b94017eb 	ldr	w11, [sp, #20]
 402b010:	f9400fe5 	ldr	x5, [sp, #24]
 402b014:	0b0b0006 	add	w6, w0, w11
 402b018:	2a0703e0 	mov	w0, w7
 402b01c:	110040e7 	add	w7, w7, #0x10
 402b020:	2941a7e3 	ldp	w3, w9, [sp, #12]
 402b024:	910010a5 	add	x5, x5, #0x4
 402b028:	b94000c2 	ldr	w2, [x6]
 402b02c:	b9400001 	ldr	w1, [x0]
 402b030:	11000463 	add	w3, w3, #0x1
 402b034:	b81fc0a2 	stur	w2, [x5, #-4]
 402b038:	0a180042 	and	w2, w2, w24
 402b03c:	f94017e8 	ldr	x8, [sp, #40]
 402b040:	2a0a0042 	orr	w2, w2, w10
 402b044:	b90000c2 	str	w2, [x6]
 402b048:	b90004a1 	str	w1, [x5, #4]
 402b04c:	0a180021 	and	w1, w1, w24
 402b050:	2a0a0021 	orr	w1, w1, w10
 402b054:	b9000001 	str	w1, [x0]
 402b058:	6b0302df 	cmp	w22, w3
 402b05c:	54fffc61 	b.ne	402afe8 <ddrtrn_capat_adapt_func+0x140>  // b.any
 402b060:	1100075a 	add	w26, w26, #0x1
 402b064:	11001084 	add	w4, w4, #0x4
 402b068:	91004108 	add	x8, x8, #0x10
 402b06c:	17ffffd8 	b	402afcc <ddrtrn_capat_adapt_func+0x124>
 402b070:	12800002 	mov	w2, #0xffffffff            	// #-1
 402b074:	52800201 	mov	w1, #0x10                  	// #16
 402b078:	52800040 	mov	w0, #0x2                   	// #2
 402b07c:	97ffff46 	bl	402ad94 <ddrtrn_winding_identification>
 402b080:	7100101f 	cmp	w0, #0x4
 402b084:	7a481804 	ccmp	w0, #0x8, #0x4, ne  // ne = any
 402b088:	54000600 	b.eq	402b148 <ddrtrn_capat_adapt_func+0x2a0>  // b.none
 402b08c:	97ffff27 	bl	402ad28 <ddrtrn_hal_get_cur_phy_dmc_num>
 402b090:	b9000fe0 	str	w0, [sp, #12]
 402b094:	97ffff1a 	bl	402acfc <ddrtrn_hal_get_cur_phy_dram_type>
 402b098:	7100181f 	cmp	w0, #0x6
 402b09c:	54000be1 	b.ne	402b218 <ddrtrn_capat_adapt_func+0x370>  // b.any
 402b0a0:	52a00801 	mov	w1, #0x400000              	// #4194304
 402b0a4:	12800002 	mov	w2, #0xffffffff            	// #-1
 402b0a8:	52a00040 	mov	w0, #0x20000               	// #131072
 402b0ac:	97ffff3a 	bl	402ad94 <ddrtrn_winding_identification>
 402b0b0:	2a1903f9 	mov	w25, w25
 402b0b4:	9101c3e1 	add	x1, sp, #0x70
 402b0b8:	d2800183 	mov	x3, #0xc                   	// #12
 402b0bc:	530a7c00 	lsr	w0, w0, #10
 402b0c0:	9b030739 	madd	x25, x25, x3, x1
 402b0c4:	b81c8320 	stur	w0, [x25, #-56]
 402b0c8:	14000042 	b	402b1d0 <ddrtrn_capat_adapt_func+0x328>
 402b0cc:	2a0103e0 	mov	w0, w1
 402b0d0:	290197e1 	stp	w1, w5, [sp, #12]
 402b0d4:	b90017e6 	str	w6, [sp, #20]
 402b0d8:	f9000fe2 	str	x2, [sp, #24]
 402b0dc:	29040fe4 	stp	w4, w3, [sp, #32]
 402b0e0:	97ffff1d 	bl	402ad54 <ddrtrn_hal_get_cur_dmc_addr>
 402b0e4:	b94017e6 	ldr	w6, [sp, #20]
 402b0e8:	f9400fe2 	ldr	x2, [sp, #24]
 402b0ec:	0b060000 	add	w0, w0, w6
 402b0f0:	29440fe4 	ldp	w4, w3, [sp, #32]
 402b0f4:	91001042 	add	x2, x2, #0x4
 402b0f8:	b85fc047 	ldur	w7, [x2, #-4]
 402b0fc:	294197e1 	ldp	w1, w5, [sp, #12]
 402b100:	b9000007 	str	w7, [x0]
 402b104:	2a0403e0 	mov	w0, w4
 402b108:	b9400447 	ldr	w7, [x2, #4]
 402b10c:	11000421 	add	w1, w1, #0x1
 402b110:	11004084 	add	w4, w4, #0x10
 402b114:	b9000007 	str	w7, [x0]
 402b118:	6b0102df 	cmp	w22, w1
 402b11c:	54fffd81 	b.ne	402b0cc <ddrtrn_capat_adapt_func+0x224>  // b.any
 402b120:	110004a5 	add	w5, w5, #0x1
 402b124:	91004294 	add	x20, x20, #0x10
 402b128:	11001063 	add	w3, w3, #0x4
 402b12c:	6b05035f 	cmp	w26, w5
 402b130:	54fffae0 	b.eq	402b08c <ddrtrn_capat_adapt_func+0x1e4>  // b.none
 402b134:	0b170066 	add	w6, w3, w23
 402b138:	2a0303e4 	mov	w4, w3
 402b13c:	aa1403e2 	mov	x2, x20
 402b140:	52800001 	mov	w1, #0x0                   	// #0
 402b144:	17fffff5 	b	402b118 <ddrtrn_capat_adapt_func+0x270>
 402b148:	5288c683 	mov	w3, #0x4634                	// #17972
 402b14c:	52800005 	mov	w5, #0x0                   	// #0
 402b150:	72a22283 	movk	w3, #0x1114, lsl #16
 402b154:	17fffff6 	b	402b12c <ddrtrn_capat_adapt_func+0x284>
 402b158:	2a1403e0 	mov	w0, w20
 402b15c:	97fffefe 	bl	402ad54 <ddrtrn_hal_get_cur_dmc_addr>
 402b160:	b90013e0 	str	w0, [sp, #16]
 402b164:	97fffe7e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b168:	b94013e1 	ldr	w1, [sp, #16]
 402b16c:	b9000801 	str	w1, [x0, #8]
 402b170:	9400022e 	bl	402ba28 <ddrtrn_hal_ddrt_get_mem_width>
 402b174:	7100081f 	cmp	w0, #0x2
 402b178:	540003e1 	b.ne	402b1f4 <ddrtrn_capat_adapt_func+0x34c>  // b.any
 402b17c:	529fffe2 	mov	w2, #0xffff                	// #65535
 402b180:	52a00801 	mov	w1, #0x400000              	// #4194304
 402b184:	52a00040 	mov	w0, #0x20000               	// #131072
 402b188:	97ffff03 	bl	402ad94 <ddrtrn_winding_identification>
 402b18c:	9100c3e1 	add	x1, sp, #0x30
 402b190:	530b7c00 	lsr	w0, w0, #11
 402b194:	52bfffe2 	mov	w2, #0xffff0000            	// #-65536
 402b198:	b8366820 	str	w0, [x1, x22]
 402b19c:	52a00801 	mov	w1, #0x400000              	// #4194304
 402b1a0:	52a00040 	mov	w0, #0x20000               	// #131072
 402b1a4:	97fffefc 	bl	402ad94 <ddrtrn_winding_identification>
 402b1a8:	530b7c00 	lsr	w0, w0, #11
 402b1ac:	9100c3e1 	add	x1, sp, #0x30
 402b1b0:	b8366b20 	str	w0, [x25, x22]
 402b1b4:	b8766821 	ldr	w1, [x1, x22]
 402b1b8:	0b000020 	add	w0, w1, w0
 402b1bc:	11000694 	add	w20, w20, #0x1
 402b1c0:	b8366b40 	str	w0, [x26, x22]
 402b1c4:	b9400fe0 	ldr	w0, [sp, #12]
 402b1c8:	6b14001f 	cmp	w0, w20
 402b1cc:	54fffc61 	b.ne	402b158 <ddrtrn_capat_adapt_func+0x2b0>  // b.any
 402b1d0:	d2a80000 	mov	x0, #0x40000000            	// #1073741824
 402b1d4:	91000673 	add	x19, x19, #0x1
 402b1d8:	910032b5 	add	x21, x21, #0xc
 402b1dc:	b900001b 	str	w27, [x0]
 402b1e0:	b85fc2a1 	ldur	w1, [x21, #-4]
 402b1e4:	b9403fe0 	ldr	w0, [sp, #60]
 402b1e8:	0b010000 	add	w0, w0, w1
 402b1ec:	b9003fe0 	str	w0, [sp, #60]
 402b1f0:	17ffff45 	b	402af04 <ddrtrn_capat_adapt_func+0x5c>
 402b1f4:	9100c3e0 	add	x0, sp, #0x30
 402b1f8:	b8366b3f 	str	wzr, [x25, x22]
 402b1fc:	12800002 	mov	w2, #0xffffffff            	// #-1
 402b200:	52a00801 	mov	w1, #0x400000              	// #4194304
 402b204:	b836681f 	str	wzr, [x0, x22]
 402b208:	52a00040 	mov	w0, #0x20000               	// #131072
 402b20c:	97fffee2 	bl	402ad94 <ddrtrn_winding_identification>
 402b210:	530a7c00 	lsr	w0, w0, #10
 402b214:	17ffffea 	b	402b1bc <ddrtrn_capat_adapt_func+0x314>
 402b218:	2a1903e3 	mov	w3, w25
 402b21c:	d2800180 	mov	x0, #0xc                   	// #12
 402b220:	9100d3f9 	add	x25, sp, #0x34
 402b224:	9100e3fa 	add	x26, sp, #0x38
 402b228:	52800014 	mov	w20, #0x0                   	// #0
 402b22c:	9b007c76 	mul	x22, x3, x0
 402b230:	17ffffe5 	b	402b1c4 <ddrtrn_capat_adapt_func+0x31c>
 402b234:	97fffeb2 	bl	402acfc <ddrtrn_hal_get_cur_phy_dram_type>
 402b238:	7100181f 	cmp	w0, #0x6
 402b23c:	54ffe7c1 	b.ne	402af34 <ddrtrn_capat_adapt_func+0x8c>  // b.any
 402b240:	9100c3e0 	add	x0, sp, #0x30
 402b244:	94000e3c 	bl	402eb34 <ddrtrn_capat_adpat_cfg_lpddr4>
 402b248:	17ffff3b 	b	402af34 <ddrtrn_capat_adapt_func+0x8c>
 402b24c:	a94753f3 	ldp	x19, x20, [sp, #112]
 402b250:	a9485bf5 	ldp	x21, x22, [sp, #128]
 402b254:	a94963f7 	ldp	x23, x24, [sp, #144]
 402b258:	a94a6bf9 	ldp	x25, x26, [sp, #160]
 402b25c:	a94b73fb 	ldp	x27, x28, [sp, #176]
 402b260:	f94063fe 	ldr	x30, [sp, #192]
 402b264:	910343ff 	add	sp, sp, #0xd0
 402b268:	d65f03c0 	ret

000000000402b26c <ddrtrn_hal_cmd_prepare_copy>:
 402b26c:	d65f03c0 	ret

000000000402b270 <ddrtrn_hal_cmd_site_save>:
 402b270:	d65f03c0 	ret

000000000402b274 <ddrtrn_hal_cmd_site_restore>:
 402b274:	d65f03c0 	ret

000000000402b278 <ddrtrn_hal_boot_cmd_save>:
 402b278:	b5000060 	cbnz	x0, 402b284 <ddrtrn_hal_boot_cmd_save+0xc>
 402b27c:	12800000 	mov	w0, #0xffffffff            	// #-1
 402b280:	d65f03c0 	ret
 402b284:	d2884282 	mov	x2, #0x4214                	// #16916
 402b288:	f2a22042 	movk	x2, #0x1102, lsl #16
 402b28c:	b9400041 	ldr	w1, [x2]
 402b290:	b9000c01 	str	w1, [x0, #12]
 402b294:	32000021 	orr	w1, w1, #0x1
 402b298:	b9000041 	str	w1, [x2]
 402b29c:	d2845401 	mov	x1, #0x22a0                	// #8864
 402b2a0:	f2a22021 	movk	x1, #0x1101, lsl #16
 402b2a4:	b9400022 	ldr	w2, [x1]
 402b2a8:	b9000002 	str	w2, [x0]
 402b2ac:	321c0042 	orr	w2, w2, #0x10
 402b2b0:	b9000022 	str	w2, [x1]
 402b2b4:	d503201f 	nop
 402b2b8:	b9400022 	ldr	w2, [x1]
 402b2bc:	121f7842 	and	w2, w2, #0xfffffffe
 402b2c0:	b9000022 	str	w2, [x1]
 402b2c4:	d2801602 	mov	x2, #0xb0                  	// #176
 402b2c8:	f2a222a2 	movk	x2, #0x1115, lsl #16
 402b2cc:	b9400041 	ldr	w1, [x2]
 402b2d0:	b9000401 	str	w1, [x0, #4]
 402b2d4:	12007821 	and	w1, w1, #0x7fffffff
 402b2d8:	b9000041 	str	w1, [x2]
 402b2dc:	9120c042 	add	x2, x2, #0x830
 402b2e0:	b9400041 	ldr	w1, [x2]
 402b2e4:	b9000801 	str	w1, [x0, #8]
 402b2e8:	52800000 	mov	w0, #0x0                   	// #0
 402b2ec:	121f7821 	and	w1, w1, #0xfffffffe
 402b2f0:	b9000041 	str	w1, [x2]
 402b2f4:	b9400041 	ldr	w1, [x2]
 402b2f8:	11000400 	add	w0, w0, #0x1
 402b2fc:	36000081 	tbz	w1, #0, 402b30c <ddrtrn_hal_boot_cmd_save+0x94>
 402b300:	3100041f 	cmn	w0, #0x1
 402b304:	54ffff81 	b.ne	402b2f4 <ddrtrn_hal_boot_cmd_save+0x7c>  // b.any
 402b308:	17ffffdd 	b	402b27c <ddrtrn_hal_boot_cmd_save+0x4>
 402b30c:	52800000 	mov	w0, #0x0                   	// #0
 402b310:	17ffffdc 	b	402b280 <ddrtrn_hal_boot_cmd_save+0x8>

000000000402b314 <ddrtrn_hal_boot_cmd_restore>:
 402b314:	b4000220 	cbz	x0, 402b358 <ddrtrn_hal_boot_cmd_restore+0x44>
 402b318:	d2884281 	mov	x1, #0x4214                	// #16916
 402b31c:	b9400c02 	ldr	w2, [x0, #12]
 402b320:	f2a22041 	movk	x1, #0x1102, lsl #16
 402b324:	b9000022 	str	w2, [x1]
 402b328:	d2845401 	mov	x1, #0x22a0                	// #8864
 402b32c:	f2a22021 	movk	x1, #0x1101, lsl #16
 402b330:	b9400002 	ldr	w2, [x0]
 402b334:	b9000022 	str	w2, [x1]
 402b338:	d2801601 	mov	x1, #0xb0                  	// #176
 402b33c:	f2a222a1 	movk	x1, #0x1115, lsl #16
 402b340:	b9400402 	ldr	w2, [x0, #4]
 402b344:	b9000022 	str	w2, [x1]
 402b348:	b9400801 	ldr	w1, [x0, #8]
 402b34c:	d2811c00 	mov	x0, #0x8e0                 	// #2272
 402b350:	f2a222a0 	movk	x0, #0x1115, lsl #16
 402b354:	b9000001 	str	w1, [x0]
 402b358:	d65f03c0 	ret

000000000402b35c <ddrtrn_hw_training_init>:
 402b35c:	d10103ff 	sub	sp, sp, #0x40
 402b360:	52800202 	mov	w2, #0x10                  	// #16
 402b364:	a90253f3 	stp	x19, x20, [sp, #32]
 402b368:	2a0003f3 	mov	w19, w0
 402b36c:	d0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402b370:	a9037bf5 	stp	x21, x30, [sp, #48]
 402b374:	2a0103f5 	mov	w21, w1
 402b378:	f9477001 	ldr	x1, [x0, #3808]
 402b37c:	f9000fe1 	str	x1, [sp, #24]
 402b380:	d2800001 	mov	x1, #0x0                   	// #0
 402b384:	910023e0 	add	x0, sp, #0x8
 402b388:	94000be9 	bl	402e32c <ddrtrn_set_data>
 402b38c:	d2800580 	mov	x0, #0x2c                  	// #44
 402b390:	f2a222a0 	movk	x0, #0x1115, lsl #16
 402b394:	b9400014 	ldr	w20, [x0]
 402b398:	97fffdfd 	bl	402ab8c <ddrtrn_hal_cfg_init>
 402b39c:	910023e0 	add	x0, sp, #0x8
 402b3a0:	97ffffb6 	bl	402b278 <ddrtrn_hal_boot_cmd_save>
 402b3a4:	35000280 	cbnz	w0, 402b3f4 <ddrtrn_hw_training_init+0x98>
 402b3a8:	12000e94 	and	w20, w20, #0xf
 402b3ac:	710006bf 	cmp	w21, #0x1
 402b3b0:	7a460a80 	ccmp	w20, #0x6, #0x0, eq  // eq = none
 402b3b4:	2a1303e0 	mov	w0, w19
 402b3b8:	54000181 	b.ne	402b3e8 <ddrtrn_hw_training_init+0x8c>  // b.any
 402b3bc:	94000edd 	bl	402ef30 <ddrtrn_low_freq_start>
 402b3c0:	2a0003e3 	mov	w3, w0
 402b3c4:	910023e0 	add	x0, sp, #0x8
 402b3c8:	97ffffd3 	bl	402b314 <ddrtrn_hal_boot_cmd_restore>
 402b3cc:	d0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402b3d0:	f9400fe1 	ldr	x1, [sp, #24]
 402b3d4:	f9477002 	ldr	x2, [x0, #3808]
 402b3d8:	eb020021 	subs	x1, x1, x2
 402b3dc:	d2800002 	mov	x2, #0x0                   	// #0
 402b3e0:	540000e0 	b.eq	402b3fc <ddrtrn_hw_training_init+0xa0>  // b.none
 402b3e4:	97ffeb86 	bl	40261fc <__stack_chk_fail>
 402b3e8:	940001e0 	bl	402bb68 <ddrtrn_hal_hw_item_cfg>
 402b3ec:	97fffdd5 	bl	402ab40 <ddrtrn_hw_training_if>
 402b3f0:	17fffff4 	b	402b3c0 <ddrtrn_hw_training_init+0x64>
 402b3f4:	12800003 	mov	w3, #0xffffffff            	// #-1
 402b3f8:	17fffff5 	b	402b3cc <ddrtrn_hw_training_init+0x70>
 402b3fc:	2a0303e0 	mov	w0, w3
 402b400:	a94253f3 	ldp	x19, x20, [sp, #32]
 402b404:	a9437bf5 	ldp	x21, x30, [sp, #48]
 402b408:	910103ff 	add	sp, sp, #0x40
 402b40c:	d65f03c0 	ret

000000000402b410 <ddrtrn_hal_get_cur_dmc_addr>:
 402b410:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402b414:	2a0003f3 	mov	w19, w0
 402b418:	f9000bfe 	str	x30, [sp, #16]
 402b41c:	97fffdd0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b420:	aa0003f4 	mov	x20, x0
 402b424:	97fffdc9 	bl	402ab48 <ddrtrn_hal_get_phy>
 402b428:	d2800782 	mov	x2, #0x3c                  	// #60
 402b42c:	f9400bfe 	ldr	x30, [sp, #16]
 402b430:	b9402281 	ldr	w1, [x20, #32]
 402b434:	9b027c21 	mul	x1, x1, x2
 402b438:	d2800182 	mov	x2, #0xc                   	// #12
 402b43c:	9b020673 	madd	x19, x19, x2, x1
 402b440:	8b130013 	add	x19, x0, x19
 402b444:	b9401660 	ldr	w0, [x19, #20]
 402b448:	a8c253f3 	ldp	x19, x20, [sp], #32
 402b44c:	d65f03c0 	ret

000000000402b450 <ddrtrn_hal_set_timing>:
 402b450:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402b454:	2a0003f3 	mov	w19, w0
 402b458:	11042273 	add	w19, w19, #0x108
 402b45c:	2a0103f4 	mov	w20, w1
 402b460:	52807d00 	mov	w0, #0x3e8                 	// #1000
 402b464:	f9000bfe 	str	x30, [sp, #16]
 402b468:	9400046b 	bl	402c614 <ddrtrn_hal_training_delay>
 402b46c:	f9400bfe 	ldr	x30, [sp, #16]
 402b470:	b9000274 	str	w20, [x19]
 402b474:	52807d00 	mov	w0, #0x3e8                 	// #1000
 402b478:	a8c253f3 	ldp	x19, x20, [sp], #32
 402b47c:	14000466 	b	402c614 <ddrtrn_hal_training_delay>

000000000402b480 <ddrtrn_hal_ddrc_get_bank_group>:
 402b480:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402b484:	97fffdb6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b488:	b9400813 	ldr	w19, [x0, #8]
 402b48c:	97fffdb4 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b490:	b9402400 	ldr	w0, [x0, #36]
 402b494:	11018273 	add	w19, w19, #0x60
 402b498:	0b000a60 	add	w0, w19, w0, lsl #2
 402b49c:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402b4a0:	b9400000 	ldr	w0, [x0]
 402b4a4:	d34a2c00 	ubfx	x0, x0, #10, #2
 402b4a8:	d65f03c0 	ret

000000000402b4ac <ddrtrn_hal_save_reg>:
 402b4ac:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402b4b0:	aa0003f3 	mov	x19, x0
 402b4b4:	a9015bf5 	stp	x21, x22, [sp, #16]
 402b4b8:	2a0103f5 	mov	w21, w1
 402b4bc:	a90263f7 	stp	x23, x24, [sp, #32]
 402b4c0:	a9037bf9 	stp	x25, x30, [sp, #48]
 402b4c4:	97fffda6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b4c8:	b9400816 	ldr	w22, [x0, #8]
 402b4cc:	97fffda4 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b4d0:	b9400414 	ldr	w20, [x0, #4]
 402b4d4:	110422c0 	add	w0, w22, #0x108
 402b4d8:	52820c81 	mov	w1, #0x1064                	// #4196
 402b4dc:	1100a2d9 	add	w25, w22, #0x28
 402b4e0:	0b010297 	add	w23, w20, w1
 402b4e4:	1101c298 	add	w24, w20, #0x70
 402b4e8:	b9400001 	ldr	w1, [x0]
 402b4ec:	b9000261 	str	w1, [x19]
 402b4f0:	b9400320 	ldr	w0, [x25]
 402b4f4:	b9000660 	str	w0, [x19, #4]
 402b4f8:	b9400300 	ldr	w0, [x24]
 402b4fc:	b9001260 	str	w0, [x19, #16]
 402b500:	b94002e0 	ldr	w0, [x23]
 402b504:	b9001660 	str	w0, [x19, #20]
 402b508:	b94002e0 	ldr	w0, [x23]
 402b50c:	b9001660 	str	w0, [x19, #20]
 402b510:	714402bf 	cmp	w21, #0x100, lsl #12
 402b514:	540004a0 	b.eq	402b5a8 <ddrtrn_hal_save_reg+0xfc>  // b.none
 402b518:	54000368 	b.hi	402b584 <ddrtrn_hal_save_reg+0xd8>  // b.pmore
 402b51c:	710042bf 	cmp	w21, #0x10
 402b520:	54000360 	b.eq	402b58c <ddrtrn_hal_save_reg+0xe0>  // b.none
 402b524:	710402bf 	cmp	w21, #0x100
 402b528:	540003a0 	b.eq	402b59c <ddrtrn_hal_save_reg+0xf0>  // b.none
 402b52c:	b9400660 	ldr	w0, [x19, #4]
 402b530:	2a1403e1 	mov	w1, w20
 402b534:	121f7800 	and	w0, w0, #0xfffffffe
 402b538:	b9000320 	str	w0, [x25]
 402b53c:	b9401260 	ldr	w0, [x19, #16]
 402b540:	120f7800 	and	w0, w0, #0xfffeffff
 402b544:	b9000300 	str	w0, [x24]
 402b548:	91006260 	add	x0, x19, #0x18
 402b54c:	94000582 	bl	402cb54 <ddrtrn_hal_dqsswap_save_func>
 402b550:	aa1303e0 	mov	x0, x19
 402b554:	940000c3 	bl	402b860 <ddrtrn_hal_axi_save_func>
 402b558:	2a1603e1 	mov	w1, w22
 402b55c:	aa1303e0 	mov	x0, x19
 402b560:	94000114 	bl	402b9b0 <ddrtrn_hal_rnkvol_save_func>
 402b564:	2a1403e0 	mov	w0, w20
 402b568:	940001b3 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402b56c:	d5033f9f 	dsb	sy
 402b570:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402b574:	a94263f7 	ldp	x23, x24, [sp, #32]
 402b578:	a9437bf9 	ldp	x25, x30, [sp, #48]
 402b57c:	a8c453f3 	ldp	x19, x20, [sp], #64
 402b580:	d65f03c0 	ret
 402b584:	716002bf 	cmp	w21, #0x800, lsl #12
 402b588:	54fffd21 	b.ne	402b52c <ddrtrn_hal_save_reg+0x80>  // b.any
 402b58c:	12144c21 	and	w1, w1, #0xfffff000
 402b590:	2a1603e0 	mov	w0, w22
 402b594:	97ffffaf 	bl	402b450 <ddrtrn_hal_set_timing>
 402b598:	17ffffe5 	b	402b52c <ddrtrn_hal_save_reg+0x80>
 402b59c:	12144c21 	and	w1, w1, #0xfffff000
 402b5a0:	2a1603e0 	mov	w0, w22
 402b5a4:	97ffffab 	bl	402b450 <ddrtrn_hal_set_timing>
 402b5a8:	1100b280 	add	w0, w20, #0x2c
 402b5ac:	b9400000 	ldr	w0, [x0]
 402b5b0:	372ffbe0 	tbnz	w0, #5, 402b52c <ddrtrn_hal_save_reg+0x80>
 402b5b4:	b90002ff 	str	wzr, [x23]
 402b5b8:	17ffffdd 	b	402b52c <ddrtrn_hal_save_reg+0x80>

000000000402b5bc <ddrtrn_hal_restore_reg>:
 402b5bc:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402b5c0:	aa0003f3 	mov	x19, x0
 402b5c4:	a9017bf5 	stp	x21, x30, [sp, #16]
 402b5c8:	97fffd65 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b5cc:	b9400815 	ldr	w21, [x0, #8]
 402b5d0:	97fffd63 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b5d4:	b9400414 	ldr	w20, [x0, #4]
 402b5d8:	b9400261 	ldr	w1, [x19]
 402b5dc:	2a1503e0 	mov	w0, w21
 402b5e0:	97ffff9c 	bl	402b450 <ddrtrn_hal_set_timing>
 402b5e4:	1100a2a0 	add	w0, w21, #0x28
 402b5e8:	b9400661 	ldr	w1, [x19, #4]
 402b5ec:	b9000001 	str	w1, [x0]
 402b5f0:	1101c280 	add	w0, w20, #0x70
 402b5f4:	b9401261 	ldr	w1, [x19, #16]
 402b5f8:	b9000001 	str	w1, [x0]
 402b5fc:	1100b280 	add	w0, w20, #0x2c
 402b600:	b9400000 	ldr	w0, [x0]
 402b604:	372800a0 	tbnz	w0, #5, 402b618 <ddrtrn_hal_restore_reg+0x5c>
 402b608:	52820c80 	mov	w0, #0x1064                	// #4196
 402b60c:	0b000280 	add	w0, w20, w0
 402b610:	b9401661 	ldr	w1, [x19, #20]
 402b614:	b9000001 	str	w1, [x0]
 402b618:	b9401a60 	ldr	w0, [x19, #24]
 402b61c:	2a1403e1 	mov	w1, w20
 402b620:	94000552 	bl	402cb68 <ddrtrn_hal_dqsswap_restore_func>
 402b624:	aa1303e0 	mov	x0, x19
 402b628:	9400009b 	bl	402b894 <ddrtrn_hal_axi_restore_func>
 402b62c:	2a1503e1 	mov	w1, w21
 402b630:	aa1303e0 	mov	x0, x19
 402b634:	940000e3 	bl	402b9c0 <ddrtrn_hal_rnkvol_restore_func>
 402b638:	2a1403e0 	mov	w0, w20
 402b63c:	9400017e 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402b640:	d5033f9f 	dsb	sy
 402b644:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402b648:	a8c253f3 	ldp	x19, x20, [sp], #32
 402b64c:	d65f03c0 	ret

000000000402b650 <ddrtrn_hal_training_switch_axi>:
 402b650:	f81f0ffe 	str	x30, [sp, #-16]!
 402b654:	9400009b 	bl	402b8c0 <ddrtrn_hal_axi_chsel_remap_func>
 402b658:	940000b4 	bl	402b928 <ddrtrn_hal_axi_switch_func>
 402b65c:	f84107fe 	ldr	x30, [sp], #16
 402b660:	140000dc 	b	402b9d0 <ddrtrn_hal_rnkvol_set_func>

000000000402b664 <ddrtrn_hal_ddrc_easr>:
 402b664:	7100083f 	cmp	w1, #0x2
 402b668:	540002a1 	b.ne	402b6bc <ddrtrn_hal_ddrc_easr+0x58>  // b.any
 402b66c:	2a0003e2 	mov	w2, w0
 402b670:	110a5000 	add	w0, w0, #0x294
 402b674:	b9000041 	str	w1, [x2]
 402b678:	52800001 	mov	w1, #0x0                   	// #0
 402b67c:	b9400002 	ldr	w2, [x0]
 402b680:	11000421 	add	w1, w1, #0x1
 402b684:	37000062 	tbnz	w2, #0, 402b690 <ddrtrn_hal_ddrc_easr+0x2c>
 402b688:	52800000 	mov	w0, #0x0                   	// #0
 402b68c:	d65f03c0 	ret
 402b690:	3100043f 	cmn	w1, #0x1
 402b694:	54ffff41 	b.ne	402b67c <ddrtrn_hal_ddrc_easr+0x18>  // b.any
 402b698:	f81f0ffe 	str	x30, [sp, #-16]!
 402b69c:	12800003 	mov	w3, #0xffffffff            	// #-1
 402b6a0:	52800200 	mov	w0, #0x10                  	// #16
 402b6a4:	2a0303e2 	mov	w2, w3
 402b6a8:	2a0303e1 	mov	w1, w3
 402b6ac:	940003df 	bl	402c628 <ddrtrn_hal_training_stat>
 402b6b0:	12800000 	mov	w0, #0xffffffff            	// #-1
 402b6b4:	f84107fe 	ldr	x30, [sp], #16
 402b6b8:	d65f03c0 	ret
 402b6bc:	7100043f 	cmp	w1, #0x1
 402b6c0:	54fffec1 	b.ne	402b698 <ddrtrn_hal_ddrc_easr+0x34>  // b.any
 402b6c4:	2a0003e2 	mov	w2, w0
 402b6c8:	110a5000 	add	w0, w0, #0x294
 402b6cc:	b9000041 	str	w1, [x2]
 402b6d0:	52800001 	mov	w1, #0x0                   	// #0
 402b6d4:	b9400002 	ldr	w2, [x0]
 402b6d8:	11000421 	add	w1, w1, #0x1
 402b6dc:	3707fd62 	tbnz	w2, #0, 402b688 <ddrtrn_hal_ddrc_easr+0x24>
 402b6e0:	3100043f 	cmn	w1, #0x1
 402b6e4:	54ffff81 	b.ne	402b6d4 <ddrtrn_hal_ddrc_easr+0x70>  // b.any
 402b6e8:	17ffffec 	b	402b698 <ddrtrn_hal_ddrc_easr+0x34>

000000000402b6ec <ddrtrn_hal_save_timing>:
 402b6ec:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402b6f0:	aa0003f4 	mov	x20, x0
 402b6f4:	52800013 	mov	w19, #0x0                   	// #0
 402b6f8:	a9015bf5 	stp	x21, x22, [sp, #16]
 402b6fc:	d2800796 	mov	x22, #0x3c                  	// #60
 402b700:	f90013fe 	str	x30, [sp, #32]
 402b704:	97fffd16 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b708:	aa0003f5 	mov	x21, x0
 402b70c:	97fffd0f 	bl	402ab48 <ddrtrn_hal_get_phy>
 402b710:	b94022a1 	ldr	w1, [x21, #32]
 402b714:	9b160021 	madd	x1, x1, x22, x0
 402b718:	b9400820 	ldr	w0, [x1, #8]
 402b71c:	6b00027f 	cmp	w19, w0
 402b720:	540000a3 	b.cc	402b734 <ddrtrn_hal_save_timing+0x48>  // b.lo, b.ul, b.last
 402b724:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402b728:	f94013fe 	ldr	x30, [sp, #32]
 402b72c:	a8c353f3 	ldp	x19, x20, [sp], #48
 402b730:	d65f03c0 	ret
 402b734:	2a1303e0 	mov	w0, w19
 402b738:	97ffff36 	bl	402b410 <ddrtrn_hal_get_cur_dmc_addr>
 402b73c:	11042000 	add	w0, w0, #0x108
 402b740:	d37e7e75 	ubfiz	x21, x19, #2, #32
 402b744:	b9400000 	ldr	w0, [x0]
 402b748:	b8356a80 	str	w0, [x20, x21]
 402b74c:	2a1303e0 	mov	w0, w19
 402b750:	11000673 	add	w19, w19, #0x1
 402b754:	97ffff2f 	bl	402b410 <ddrtrn_hal_get_cur_dmc_addr>
 402b758:	b8756a81 	ldr	w1, [x20, x21]
 402b75c:	12144c21 	and	w1, w1, #0xfffff000
 402b760:	97ffff3c 	bl	402b450 <ddrtrn_hal_set_timing>
 402b764:	17ffffe8 	b	402b704 <ddrtrn_hal_save_timing+0x18>

000000000402b768 <ddrtrn_hal_dmc_get_sref_cfg>:
 402b768:	f81f0ffe 	str	x30, [sp, #-16]!
 402b76c:	97ffff29 	bl	402b410 <ddrtrn_hal_get_cur_dmc_addr>
 402b770:	11008000 	add	w0, w0, #0x20
 402b774:	f84107fe 	ldr	x30, [sp], #16
 402b778:	b9400000 	ldr	w0, [x0]
 402b77c:	d65f03c0 	ret

000000000402b780 <ddrtrn_hal_dmc_set_sref_cfg>:
 402b780:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402b784:	2a0103f3 	mov	w19, w1
 402b788:	97ffff22 	bl	402b410 <ddrtrn_hal_get_cur_dmc_addr>
 402b78c:	11008000 	add	w0, w0, #0x20
 402b790:	b9000013 	str	w19, [x0]
 402b794:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402b798:	d65f03c0 	ret

000000000402b79c <ddrtrn_hal_phy_get_byte_num>:
 402b79c:	11014000 	add	w0, w0, #0x50
 402b7a0:	52800081 	mov	w1, #0x4                   	// #4
 402b7a4:	b9400000 	ldr	w0, [x0]
 402b7a8:	53047c00 	lsr	w0, w0, #4
 402b7ac:	531f0400 	ubfiz	w0, w0, #1, #2
 402b7b0:	7100101f 	cmp	w0, #0x4
 402b7b4:	1a819000 	csel	w0, w0, w1, ls  // ls = plast
 402b7b8:	d65f03c0 	ret

000000000402b7bc <ddrtrn_hal_get_rank_size>:
 402b7bc:	f81f0ffe 	str	x30, [sp, #-16]!
 402b7c0:	97fffce7 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b7c4:	b9400801 	ldr	w1, [x0, #8]
 402b7c8:	f84107fe 	ldr	x30, [sp], #16
 402b7cc:	11014020 	add	w0, w1, #0x50
 402b7d0:	11018021 	add	w1, w1, #0x60
 402b7d4:	b9400000 	ldr	w0, [x0]
 402b7d8:	b9400021 	ldr	w1, [x1]
 402b7dc:	d3441400 	ubfx	x0, x0, #4, #2
 402b7e0:	d3482422 	ubfx	x2, x1, #8, #2
 402b7e4:	0b020000 	add	w0, w0, w2
 402b7e8:	12000822 	and	w2, w1, #0x7
 402b7ec:	11005442 	add	w2, w2, #0x15
 402b7f0:	d3441821 	ubfx	x1, x1, #4, #3
 402b7f4:	0b020000 	add	w0, w0, w2
 402b7f8:	0b010000 	add	w0, w0, w1
 402b7fc:	d2800021 	mov	x1, #0x1                   	// #1
 402b800:	9ac02020 	lsl	x0, x1, x0
 402b804:	d65f03c0 	ret

000000000402b808 <ddrtrn_hal_axi_special_intlv_en>:
 402b808:	d2800401 	mov	x1, #0x20                  	// #32
 402b80c:	f2a22281 	movk	x1, #0x1114, lsl #16
 402b810:	b9400020 	ldr	w0, [x1]
 402b814:	321d0000 	orr	w0, w0, #0x8
 402b818:	b9000020 	str	w0, [x1]
 402b81c:	d65f03c0 	ret

000000000402b820 <ddrtrn_hal_timing8_trfc_ab_cfg_by_dmc>:
 402b820:	f81f0ffe 	str	x30, [sp, #-16]!
 402b824:	97fffcce 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b828:	b9400801 	ldr	w1, [x0, #8]
 402b82c:	52802bc3 	mov	w3, #0x15e                 	// #350
 402b830:	11048021 	add	w1, w1, #0x120
 402b834:	f84107fe 	ldr	x30, [sp], #16
 402b838:	b9400022 	ldr	w2, [x1]
 402b83c:	d3536c40 	ubfx	x0, x2, #19, #9
 402b840:	12045842 	and	w2, w2, #0xf007ffff
 402b844:	1b037c00 	mul	w0, w0, w3
 402b848:	528044c3 	mov	w3, #0x226                 	// #550
 402b84c:	1ac30800 	udiv	w0, w0, w3
 402b850:	11000400 	add	w0, w0, #0x1
 402b854:	2a004c40 	orr	w0, w2, w0, lsl #19
 402b858:	b9000020 	str	w0, [x1]
 402b85c:	d65f03c0 	ret

000000000402b860 <ddrtrn_hal_axi_save_func>:
 402b860:	d2802081 	mov	x1, #0x104                 	// #260
 402b864:	f2a22281 	movk	x1, #0x1114, lsl #16
 402b868:	b9400021 	ldr	w1, [x1]
 402b86c:	b9002c01 	str	w1, [x0, #44]
 402b870:	d2802281 	mov	x1, #0x114                 	// #276
 402b874:	f2a22281 	movk	x1, #0x1114, lsl #16
 402b878:	b9400021 	ldr	w1, [x1]
 402b87c:	b9003001 	str	w1, [x0, #48]
 402b880:	d2800801 	mov	x1, #0x40                  	// #64
 402b884:	f2a22281 	movk	x1, #0x1114, lsl #16
 402b888:	b9400021 	ldr	w1, [x1]
 402b88c:	b9003801 	str	w1, [x0, #56]
 402b890:	d65f03c0 	ret

000000000402b894 <ddrtrn_hal_axi_restore_func>:
 402b894:	d2802081 	mov	x1, #0x104                 	// #260
 402b898:	b9402c02 	ldr	w2, [x0, #44]
 402b89c:	f2a22281 	movk	x1, #0x1114, lsl #16
 402b8a0:	b9000022 	str	w2, [x1]
 402b8a4:	b9403002 	ldr	w2, [x0, #48]
 402b8a8:	b9001022 	str	w2, [x1, #16]
 402b8ac:	b9403801 	ldr	w1, [x0, #56]
 402b8b0:	d2800800 	mov	x0, #0x40                  	// #64
 402b8b4:	f2a22280 	movk	x0, #0x1114, lsl #16
 402b8b8:	b9000001 	str	w1, [x0]
 402b8bc:	d65f03c0 	ret

000000000402b8c0 <ddrtrn_hal_axi_chsel_remap_func>:
 402b8c0:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402b8c4:	97fffca6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b8c8:	aa0003f3 	mov	x19, x0
 402b8cc:	97fffc9f 	bl	402ab48 <ddrtrn_hal_get_phy>
 402b8d0:	d2800782 	mov	x2, #0x3c                  	// #60
 402b8d4:	b9402261 	ldr	w1, [x19, #32]
 402b8d8:	9b020021 	madd	x1, x1, x2, x0
 402b8dc:	b9400420 	ldr	w0, [x1, #4]
 402b8e0:	7100181f 	cmp	w0, #0x6
 402b8e4:	d2800800 	mov	x0, #0x40                  	// #64
 402b8e8:	f2a22280 	movk	x0, #0x1114, lsl #16
 402b8ec:	540000c1 	b.ne	402b904 <ddrtrn_hal_axi_chsel_remap_func+0x44>  // b.any
 402b8f0:	52864201 	mov	w1, #0x3210                	// #12816
 402b8f4:	72aeca81 	movk	w1, #0x7654, lsl #16
 402b8f8:	b9000001 	str	w1, [x0]
 402b8fc:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402b900:	d65f03c0 	ret
 402b904:	52828401 	mov	w1, #0x1420                	// #5152
 402b908:	72aeca61 	movk	w1, #0x7653, lsl #16
 402b90c:	17fffffb 	b	402b8f8 <ddrtrn_hal_axi_chsel_remap_func+0x38>

000000000402b910 <ddrtrn_hal_dmc_sfc_cmd_write>:
 402b910:	32020000 	orr	w0, w0, #0x40000000
 402b914:	b9000020 	str	w0, [x1]
 402b918:	d65f03c0 	ret

000000000402b91c <ddrtrn_hal_dmc_sfc_bank_write>:
 402b91c:	32100000 	orr	w0, w0, #0x10000
 402b920:	b9000020 	str	w0, [x1]
 402b924:	d65f03c0 	ret

000000000402b928 <ddrtrn_hal_axi_switch_func>:
 402b928:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402b92c:	f9000bfe 	str	x30, [sp, #16]
 402b930:	97fffc8b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b934:	b9402013 	ldr	w19, [x0, #32]
 402b938:	97fffc89 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b93c:	aa0003f4 	mov	x20, x0
 402b940:	97fffc82 	bl	402ab48 <ddrtrn_hal_get_phy>
 402b944:	d2800782 	mov	x2, #0x3c                  	// #60
 402b948:	b9402281 	ldr	w1, [x20, #32]
 402b94c:	9b020021 	madd	x1, x1, x2, x0
 402b950:	b9400420 	ldr	w0, [x1, #4]
 402b954:	7100181f 	cmp	w0, #0x6
 402b958:	540000e1 	b.ne	402b974 <ddrtrn_hal_axi_switch_func+0x4c>  // b.any
 402b95c:	97fffc80 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b960:	b9402013 	ldr	w19, [x0, #32]
 402b964:	97fffc7e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b968:	b9402800 	ldr	w0, [x0, #40]
 402b96c:	531f7a73 	lsl	w19, w19, #1
 402b970:	0b000273 	add	w19, w19, w0
 402b974:	d2802081 	mov	x1, #0x104                 	// #260
 402b978:	f2a22281 	movk	x1, #0x1114, lsl #16
 402b97c:	f9400bfe 	ldr	x30, [sp, #16]
 402b980:	b9400020 	ldr	w0, [x1]
 402b984:	121a6400 	and	w0, w0, #0xffffffc0
 402b988:	2a130000 	orr	w0, w0, w19
 402b98c:	321d0000 	orr	w0, w0, #0x8
 402b990:	b9000020 	str	w0, [x1]
 402b994:	b9401020 	ldr	w0, [x1, #16]
 402b998:	121a6400 	and	w0, w0, #0xffffffc0
 402b99c:	2a130000 	orr	w0, w0, w19
 402b9a0:	321d0000 	orr	w0, w0, #0x8
 402b9a4:	b9001020 	str	w0, [x1, #16]
 402b9a8:	a8c253f3 	ldp	x19, x20, [sp], #32
 402b9ac:	d65f03c0 	ret

000000000402b9b0 <ddrtrn_hal_rnkvol_save_func>:
 402b9b0:	11018021 	add	w1, w1, #0x60
 402b9b4:	b9400021 	ldr	w1, [x1]
 402b9b8:	b9003401 	str	w1, [x0, #52]
 402b9bc:	d65f03c0 	ret

000000000402b9c0 <ddrtrn_hal_rnkvol_restore_func>:
 402b9c0:	11018021 	add	w1, w1, #0x60
 402b9c4:	b9403400 	ldr	w0, [x0, #52]
 402b9c8:	b9000020 	str	w0, [x1]
 402b9cc:	d65f03c0 	ret

000000000402b9d0 <ddrtrn_hal_rnkvol_set_func>:
 402b9d0:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402b9d4:	97fffc62 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b9d8:	b9402400 	ldr	w0, [x0, #36]
 402b9dc:	7100041f 	cmp	w0, #0x1
 402b9e0:	54000141 	b.ne	402ba08 <ddrtrn_hal_rnkvol_set_func+0x38>  // b.any
 402b9e4:	97fffc5e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b9e8:	b9400800 	ldr	w0, [x0, #8]
 402b9ec:	11018000 	add	w0, w0, #0x60
 402b9f0:	b9400013 	ldr	w19, [x0]
 402b9f4:	97fffc5a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402b9f8:	b9400800 	ldr	w0, [x0, #8]
 402b9fc:	121d7273 	and	w19, w19, #0xfffffff8
 402ba00:	11018000 	add	w0, w0, #0x60
 402ba04:	b9000013 	str	w19, [x0]
 402ba08:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402ba0c:	d65f03c0 	ret

000000000402ba10 <ddrt_reg_read>:
 402ba10:	2a0003e0 	mov	w0, w0
 402ba14:	b9400000 	ldr	w0, [x0]
 402ba18:	d65f03c0 	ret

000000000402ba1c <ddrt_reg_write>:
 402ba1c:	2a0103e1 	mov	w1, w1
 402ba20:	b9000020 	str	w0, [x1]
 402ba24:	d65f03c0 	ret

000000000402ba28 <ddrtrn_hal_ddrt_get_mem_width>:
 402ba28:	f81f0ffe 	str	x30, [sp, #-16]!
 402ba2c:	97fffc4c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ba30:	b9400800 	ldr	w0, [x0, #8]
 402ba34:	f84107fe 	ldr	x30, [sp], #16
 402ba38:	11014000 	add	w0, w0, #0x50
 402ba3c:	b9400000 	ldr	w0, [x0]
 402ba40:	d3441400 	ubfx	x0, x0, #4, #2
 402ba44:	d65f03c0 	ret

000000000402ba48 <ddrtrn_hal_ddrt_get_addr>:
 402ba48:	d2800400 	mov	x0, #0x20                  	// #32
 402ba4c:	f2a222c0 	movk	x0, #0x1116, lsl #16
 402ba50:	b9400000 	ldr	w0, [x0]
 402ba54:	d65f03c0 	ret

000000000402ba58 <ddrtrn_hal_get_cur_byte>:
 402ba58:	f81f0ffe 	str	x30, [sp, #-16]!
 402ba5c:	97fffc40 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ba60:	b9401800 	ldr	w0, [x0, #24]
 402ba64:	f84107fe 	ldr	x30, [sp], #16
 402ba68:	d65f03c0 	ret

000000000402ba6c <ddrtrn_hal_get_rank_id>:
 402ba6c:	f81f0ffe 	str	x30, [sp, #-16]!
 402ba70:	97fffc3b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ba74:	b9402400 	ldr	w0, [x0, #36]
 402ba78:	f84107fe 	ldr	x30, [sp], #16
 402ba7c:	d65f03c0 	ret

000000000402ba80 <ddrtrn_hal_get_cur_mode>:
 402ba80:	f81f0ffe 	str	x30, [sp, #-16]!
 402ba84:	97fffc36 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ba88:	b9401400 	ldr	w0, [x0, #20]
 402ba8c:	f84107fe 	ldr	x30, [sp], #16
 402ba90:	d65f03c0 	ret

000000000402ba94 <ddrtrn_hal_get_cur_phy>:
 402ba94:	f81f0ffe 	str	x30, [sp, #-16]!
 402ba98:	97fffc31 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ba9c:	b9400400 	ldr	w0, [x0, #4]
 402baa0:	f84107fe 	ldr	x30, [sp], #16
 402baa4:	d65f03c0 	ret

000000000402baa8 <ddrtrn_hal_set_rank_id>:
 402baa8:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402baac:	2a0003f3 	mov	w19, w0
 402bab0:	97fffc2b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402bab4:	b9002413 	str	w19, [x0, #36]
 402bab8:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402babc:	d65f03c0 	ret

000000000402bac0 <ddrtrn_hal_get_cur_phy_total_byte_num>:
 402bac0:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402bac4:	97fffc26 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402bac8:	aa0003f3 	mov	x19, x0
 402bacc:	97fffc1f 	bl	402ab48 <ddrtrn_hal_get_phy>
 402bad0:	d2800782 	mov	x2, #0x3c                  	// #60
 402bad4:	b9402261 	ldr	w1, [x19, #32]
 402bad8:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402badc:	d503201f 	nop
 402bae0:	9b020021 	madd	x1, x1, x2, x0
 402bae4:	b9401020 	ldr	w0, [x1, #16]
 402bae8:	d65f03c0 	ret

000000000402baec <ddrtrn_hal_phy_get_dq_bdl>:
 402baec:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402baf0:	a9017bf5 	stp	x21, x30, [sp, #16]
 402baf4:	97ffffd9 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402baf8:	2a0003f3 	mov	w19, w0
 402bafc:	97ffffdc 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402bb00:	2a0003f4 	mov	w20, w0
 402bb04:	97fffc16 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402bb08:	b9401c15 	ldr	w21, [x0, #28]
 402bb0c:	97ffffdd 	bl	402ba80 <ddrtrn_hal_get_cur_mode>
 402bb10:	53196273 	lsl	w19, w19, #7
 402bb14:	7100081f 	cmp	w0, #0x2
 402bb18:	53165680 	lsl	w0, w20, #10
 402bb1c:	121e02a1 	and	w1, w21, #0x4
 402bb20:	0b000273 	add	w19, w19, w0
 402bb24:	540001c1 	b.ne	402bb5c <ddrtrn_hal_phy_get_dq_bdl+0x70>  // b.any
 402bb28:	11084260 	add	w0, w19, #0x210
 402bb2c:	11085273 	add	w19, w19, #0x214
 402bb30:	7100003f 	cmp	w1, #0x0
 402bb34:	531d06b5 	ubfiz	w21, w21, #3, #2
 402bb38:	1a801273 	csel	w19, w19, w0, ne  // ne = any
 402bb3c:	97ffffd6 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402bb40:	0b130013 	add	w19, w0, w19
 402bb44:	b9400260 	ldr	w0, [x19]
 402bb48:	1ad52400 	lsr	w0, w0, w21
 402bb4c:	12001800 	and	w0, w0, #0x7f
 402bb50:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402bb54:	a8c253f3 	ldp	x19, x20, [sp], #32
 402bb58:	d65f03c0 	ret
 402bb5c:	11087260 	add	w0, w19, #0x21c
 402bb60:	11088273 	add	w19, w19, #0x220
 402bb64:	17fffff3 	b	402bb30 <ddrtrn_hal_phy_get_dq_bdl+0x44>

000000000402bb68 <ddrtrn_hal_hw_item_cfg>:
 402bb68:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402bb6c:	2a0003f3 	mov	w19, w0
 402bb70:	d2801200 	mov	x0, #0x90                  	// #144
 402bb74:	f2a22040 	movk	x0, #0x1102, lsl #16
 402bb78:	b9400014 	ldr	w20, [x0]
 402bb7c:	f9000bfe 	str	x30, [sp, #16]
 402bb80:	97fffbf2 	bl	402ab48 <ddrtrn_hal_get_phy>
 402bb84:	0a140274 	and	w20, w19, w20
 402bb88:	b9003014 	str	w20, [x0, #48]
 402bb8c:	d2801280 	mov	x0, #0x94                  	// #148
 402bb90:	f2a22040 	movk	x0, #0x1102, lsl #16
 402bb94:	b9400000 	ldr	w0, [x0]
 402bb98:	0a000273 	and	w19, w19, w0
 402bb9c:	97fffbeb 	bl	402ab48 <ddrtrn_hal_get_phy>
 402bba0:	f9400bfe 	ldr	x30, [sp, #16]
 402bba4:	b9003813 	str	w19, [x0, #56]
 402bba8:	a8c253f3 	ldp	x19, x20, [sp], #32
 402bbac:	d65f03c0 	ret

000000000402bbb0 <ddrtrn_hal_sw_item_cfg>:
 402bbb0:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402bbb4:	2a0003f3 	mov	w19, w0
 402bbb8:	d2801400 	mov	x0, #0xa0                  	// #160
 402bbbc:	f2a22040 	movk	x0, #0x1102, lsl #16
 402bbc0:	b9400014 	ldr	w20, [x0]
 402bbc4:	f9000bfe 	str	x30, [sp, #16]
 402bbc8:	97fffbe0 	bl	402ab48 <ddrtrn_hal_get_phy>
 402bbcc:	0a140274 	and	w20, w19, w20
 402bbd0:	b9002c14 	str	w20, [x0, #44]
 402bbd4:	d2801480 	mov	x0, #0xa4                  	// #164
 402bbd8:	f2a22040 	movk	x0, #0x1102, lsl #16
 402bbdc:	b9400000 	ldr	w0, [x0]
 402bbe0:	0a000273 	and	w19, w19, w0
 402bbe4:	97fffbd9 	bl	402ab48 <ddrtrn_hal_get_phy>
 402bbe8:	f9400bfe 	ldr	x30, [sp, #16]
 402bbec:	b9003413 	str	w19, [x0, #52]
 402bbf0:	a8c253f3 	ldp	x19, x20, [sp], #32
 402bbf4:	d65f03c0 	ret

000000000402bbf8 <ddrtrn_hal_check_bypass>:
 402bbf8:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402bbfc:	2a0003f3 	mov	w19, w0
 402bc00:	97fffbd7 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402bc04:	b9400c00 	ldr	w0, [x0, #12]
 402bc08:	6a00027f 	tst	w19, w0
 402bc0c:	1a9f07e0 	cset	w0, ne  // ne = any
 402bc10:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402bc14:	d65f03c0 	ret

000000000402bc18 <ddrtrn_hal_phy_reset>:
 402bc18:	11001000 	add	w0, w0, #0x4
 402bc1c:	b9400001 	ldr	w1, [x0]
 402bc20:	32130022 	orr	w2, w1, #0x2000
 402bc24:	b9000002 	str	w2, [x0]
 402bc28:	12127821 	and	w1, w1, #0xffffdfff
 402bc2c:	b9000001 	str	w1, [x0]
 402bc30:	d65f03c0 	ret

000000000402bc34 <ddrtrn_hal_phy_cfg_update>:
 402bc34:	1101c002 	add	w2, w0, #0x70
 402bc38:	11001000 	add	w0, w0, #0x4
 402bc3c:	b9400041 	ldr	w1, [x2]
 402bc40:	320d0023 	orr	w3, w1, #0x80000
 402bc44:	b9000043 	str	w3, [x2]
 402bc48:	120c7821 	and	w1, w1, #0xfff7ffff
 402bc4c:	b9000041 	str	w1, [x2]
 402bc50:	b9400001 	ldr	w1, [x0]
 402bc54:	32110022 	orr	w2, w1, #0x8000
 402bc58:	b9000002 	str	w2, [x0]
 402bc5c:	12107821 	and	w1, w1, #0xffff7fff
 402bc60:	b9000001 	str	w1, [x0]
 402bc64:	d5033f9f 	dsb	sy
 402bc68:	d65f03c0 	ret

000000000402bc6c <ddrtrn_hal_ck_cfg>:
 402bc6c:	52820e01 	mov	w1, #0x1070                	// #4208
 402bc70:	0b010001 	add	w1, w0, w1
 402bc74:	5281f802 	mov	w2, #0xfc0                 	// #4032
 402bc78:	72a06002 	movk	w2, #0x300, lsl #16
 402bc7c:	b9400023 	ldr	w3, [x1]
 402bc80:	4a020062 	eor	w2, w3, w2
 402bc84:	b9000022 	str	w2, [x1]
 402bc88:	b9000023 	str	w3, [x1]
 402bc8c:	17ffffea 	b	402bc34 <ddrtrn_hal_phy_cfg_update>

000000000402bc90 <ddrtrn_hal_phy_set_dq_bdl>:
 402bc90:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402bc94:	2a0003f4 	mov	w20, w0
 402bc98:	a9015bf5 	stp	x21, x22, [sp, #16]
 402bc9c:	a9027bf7 	stp	x23, x30, [sp, #32]
 402bca0:	97ffff7d 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402bca4:	2a0003f7 	mov	w23, w0
 402bca8:	97ffff6c 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402bcac:	2a0003f5 	mov	w21, w0
 402bcb0:	97ffff6f 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402bcb4:	2a0003f3 	mov	w19, w0
 402bcb8:	97fffba9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402bcbc:	b9401c16 	ldr	w22, [x0, #28]
 402bcc0:	97ffff70 	bl	402ba80 <ddrtrn_hal_get_cur_mode>
 402bcc4:	7100081f 	cmp	w0, #0x2
 402bcc8:	53165661 	lsl	w1, w19, #10
 402bccc:	531962a2 	lsl	w2, w21, #7
 402bcd0:	121e02c0 	and	w0, w22, #0x4
 402bcd4:	0b020021 	add	w1, w1, w2
 402bcd8:	54000281 	b.ne	402bd28 <ddrtrn_hal_phy_set_dq_bdl+0x98>  // b.any
 402bcdc:	11084022 	add	w2, w1, #0x210
 402bce0:	11085021 	add	w1, w1, #0x214
 402bce4:	7100001f 	cmp	w0, #0x0
 402bce8:	531d06d6 	ubfiz	w22, w22, #3, #2
 402bcec:	1a821021 	csel	w1, w1, w2, ne  // ne = any
 402bcf0:	12001a94 	and	w20, w20, #0x7f
 402bcf4:	0b170021 	add	w1, w1, w23
 402bcf8:	52801fe2 	mov	w2, #0xff                  	// #255
 402bcfc:	1ad62042 	lsl	w2, w2, w22
 402bd00:	1ad62294 	lsl	w20, w20, w22
 402bd04:	b9400020 	ldr	w0, [x1]
 402bd08:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402bd0c:	0a220002 	bic	w2, w0, w2
 402bd10:	2a020294 	orr	w20, w20, w2
 402bd14:	2a1703e0 	mov	w0, w23
 402bd18:	a9427bf7 	ldp	x23, x30, [sp, #32]
 402bd1c:	b9000034 	str	w20, [x1]
 402bd20:	a8c353f3 	ldp	x19, x20, [sp], #48
 402bd24:	17ffffc4 	b	402bc34 <ddrtrn_hal_phy_cfg_update>
 402bd28:	11087022 	add	w2, w1, #0x21c
 402bd2c:	11088021 	add	w1, w1, #0x220
 402bd30:	17ffffed 	b	402bce4 <ddrtrn_hal_phy_set_dq_bdl+0x54>

000000000402bd34 <ddrtrn_hal_switch_rank_all_phy>:
 402bd34:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402bd38:	2a0003f4 	mov	w20, w0
 402bd3c:	52800013 	mov	w19, #0x0                   	// #0
 402bd40:	a9017bf5 	stp	x21, x30, [sp, #16]
 402bd44:	d2800795 	mov	x21, #0x3c                  	// #60
 402bd48:	97fffb85 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402bd4c:	b9400000 	ldr	w0, [x0]
 402bd50:	6b00027f 	cmp	w19, w0
 402bd54:	54000083 	b.cc	402bd64 <ddrtrn_hal_switch_rank_all_phy+0x30>  // b.lo, b.ul, b.last
 402bd58:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402bd5c:	a8c253f3 	ldp	x19, x20, [sp], #32
 402bd60:	d65f03c0 	ret
 402bd64:	97fffb79 	bl	402ab48 <ddrtrn_hal_get_phy>
 402bd68:	2a1303e2 	mov	w2, w19
 402bd6c:	2a1403e1 	mov	w1, w20
 402bd70:	11000673 	add	w19, w19, #0x1
 402bd74:	9b157c42 	mul	x2, x2, x21
 402bd78:	b8626800 	ldr	w0, [x0, x2]
 402bd7c:	9400037d 	bl	402cb70 <ddrtrn_hal_phy_switch_rank>
 402bd80:	17fffff2 	b	402bd48 <ddrtrn_hal_switch_rank_all_phy+0x14>

000000000402bd84 <ddrtrn_hal_adjust_get_val>:
 402bd84:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402bd88:	f9000bfe 	str	x30, [sp, #16]
 402bd8c:	97ffff3d 	bl	402ba80 <ddrtrn_hal_get_cur_mode>
 402bd90:	7100041f 	cmp	w0, #0x1
 402bd94:	54000161 	b.ne	402bdc0 <ddrtrn_hal_adjust_get_val+0x3c>  // b.any
 402bd98:	97ffff3f 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402bd9c:	2a0003f3 	mov	w19, w0
 402bda0:	1108b273 	add	w19, w19, #0x22c
 402bda4:	97ffff2d 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402bda8:	0b001e73 	add	w19, w19, w0, lsl #7
 402bdac:	b9400260 	ldr	w0, [x19]
 402bdb0:	12002000 	and	w0, w0, #0x1ff
 402bdb4:	f9400bfe 	ldr	x30, [sp, #16]
 402bdb8:	a8c253f3 	ldp	x19, x20, [sp], #32
 402bdbc:	d65f03c0 	ret
 402bdc0:	97ffff35 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402bdc4:	2a0003f4 	mov	w20, w0
 402bdc8:	97ffff29 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402bdcc:	2a0003f3 	mov	w19, w0
 402bdd0:	97ffff22 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402bdd4:	53196000 	lsl	w0, w0, #7
 402bdd8:	0b132813 	add	w19, w0, w19, lsl #10
 402bddc:	1108d294 	add	w20, w20, #0x234
 402bde0:	0b140273 	add	w19, w19, w20
 402bde4:	b9400260 	ldr	w0, [x19]
 402bde8:	d3483400 	ubfx	x0, x0, #8, #6
 402bdec:	17fffff2 	b	402bdb4 <ddrtrn_hal_adjust_get_val+0x30>

000000000402bdf0 <ddrtrn_hal_adjust_set_val>:
 402bdf0:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402bdf4:	2a0003f3 	mov	w19, w0
 402bdf8:	a9017bf5 	stp	x21, x30, [sp, #16]
 402bdfc:	97ffff21 	bl	402ba80 <ddrtrn_hal_get_cur_mode>
 402be00:	7100041f 	cmp	w0, #0x1
 402be04:	540002a1 	b.ne	402be58 <ddrtrn_hal_adjust_set_val+0x68>  // b.any
 402be08:	97ffff23 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402be0c:	2a0003f4 	mov	w20, w0
 402be10:	1108b294 	add	w20, w20, #0x22c
 402be14:	97ffff11 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402be18:	0b001e94 	add	w20, w20, w0, lsl #7
 402be1c:	2a1303e0 	mov	w0, w19
 402be20:	b9400294 	ldr	w20, [x20]
 402be24:	9400034b 	bl	402cb50 <ddrtrn_hal_phy_rdqs_sync_rdm>
 402be28:	97ffff1b 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402be2c:	12175a94 	and	w20, w20, #0xfffffe00
 402be30:	2a130293 	orr	w19, w20, w19
 402be34:	2a0003f4 	mov	w20, w0
 402be38:	1108b294 	add	w20, w20, #0x22c
 402be3c:	97ffff07 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402be40:	0b001e94 	add	w20, w20, w0, lsl #7
 402be44:	b9000293 	str	w19, [x20]
 402be48:	97ffff13 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402be4c:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402be50:	a8c253f3 	ldp	x19, x20, [sp], #32
 402be54:	17ffff78 	b	402bc34 <ddrtrn_hal_phy_cfg_update>
 402be58:	97ffff0f 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402be5c:	2a0003f5 	mov	w21, w0
 402be60:	97ffff03 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402be64:	2a0003f4 	mov	w20, w0
 402be68:	97fffefc 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402be6c:	53196000 	lsl	w0, w0, #7
 402be70:	0b142814 	add	w20, w0, w20, lsl #10
 402be74:	1108d2b5 	add	w21, w21, #0x234
 402be78:	0b150294 	add	w20, w20, w21
 402be7c:	b9400294 	ldr	w20, [x20]
 402be80:	97ffff05 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402be84:	2a0003f5 	mov	w21, w0
 402be88:	97fffef9 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402be8c:	12126694 	and	w20, w20, #0xffffc0ff
 402be90:	1108d2b5 	add	w21, w21, #0x234
 402be94:	2a132294 	orr	w20, w20, w19, lsl #8
 402be98:	2a0003f3 	mov	w19, w0
 402be9c:	97fffeef 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402bea0:	53196000 	lsl	w0, w0, #7
 402bea4:	0b132813 	add	w19, w0, w19, lsl #10
 402bea8:	0b150273 	add	w19, w19, w21
 402beac:	b9000274 	str	w20, [x19]
 402beb0:	17ffffe6 	b	402be48 <ddrtrn_hal_adjust_set_val+0x58>

000000000402beb4 <ddrtrn_hal_get_dly_value>:
 402beb4:	0b031c23 	add	w3, w1, w3, lsl #7
 402beb8:	0b022862 	add	w2, w3, w2, lsl #10
 402bebc:	1108c041 	add	w1, w2, #0x230
 402bec0:	b9400021 	ldr	w1, [x1]
 402bec4:	b9000001 	str	w1, [x0]
 402bec8:	12001823 	and	w3, w1, #0x7f
 402becc:	b9000803 	str	w3, [x0, #8]
 402bed0:	d3482c21 	ubfx	x1, x1, #8, #4
 402bed4:	b9000401 	str	w1, [x0, #4]
 402bed8:	1108d041 	add	w1, w2, #0x234
 402bedc:	1108e042 	add	w2, w2, #0x238
 402bee0:	b9400021 	ldr	w1, [x1]
 402bee4:	b9000c01 	str	w1, [x0, #12]
 402bee8:	d3483421 	ubfx	x1, x1, #8, #6
 402beec:	b9001001 	str	w1, [x0, #16]
 402bef0:	b9400041 	ldr	w1, [x2]
 402bef4:	b9001401 	str	w1, [x0, #20]
 402bef8:	d3504421 	ubfx	x1, x1, #16, #2
 402befc:	b9001801 	str	w1, [x0, #24]
 402bf00:	d65f03c0 	ret

000000000402bf04 <ddrtrn_hal_restore_dly_value>:
 402bf04:	0b031c23 	add	w3, w1, w3, lsl #7
 402bf08:	aa0003e4 	mov	x4, x0
 402bf0c:	0b022862 	add	w2, w3, w2, lsl #10
 402bf10:	2a0103e0 	mov	w0, w1
 402bf14:	1108c041 	add	w1, w2, #0x230
 402bf18:	b9400085 	ldr	w5, [x4]
 402bf1c:	b9000025 	str	w5, [x1]
 402bf20:	1108d041 	add	w1, w2, #0x234
 402bf24:	1108e042 	add	w2, w2, #0x238
 402bf28:	b9400c83 	ldr	w3, [x4, #12]
 402bf2c:	b9000023 	str	w3, [x1]
 402bf30:	b9401481 	ldr	w1, [x4, #20]
 402bf34:	b9000041 	str	w1, [x2]
 402bf38:	17ffff3f 	b	402bc34 <ddrtrn_hal_phy_cfg_update>

000000000402bf3c <ddrtrn_hal_wdqs_bdl2phase>:
 402bf3c:	1108b003 	add	w3, w0, #0x22c
 402bf40:	0b012801 	add	w1, w0, w1, lsl #10
 402bf44:	0b021c63 	add	w3, w3, w2, lsl #7
 402bf48:	0b021c22 	add	w2, w1, w2, lsl #7
 402bf4c:	1108c04b 	add	w11, w2, #0x230
 402bf50:	1108d04a 	add	w10, w2, #0x234
 402bf54:	1108e042 	add	w2, w2, #0x238
 402bf58:	b9400063 	ldr	w3, [x3]
 402bf5c:	b9400164 	ldr	w4, [x11]
 402bf60:	b9400149 	ldr	w9, [x10]
 402bf64:	d3506063 	ubfx	x3, x3, #16, #9
 402bf68:	b9400045 	ldr	w5, [x2]
 402bf6c:	71002c7f 	cmp	w3, #0xb
 402bf70:	54000589 	b.ls	402c020 <ddrtrn_hal_wdqs_bdl2phase+0xe4>  // b.plast
 402bf74:	52800181 	mov	w1, #0xc                   	// #12
 402bf78:	d3482c87 	ubfx	x7, x4, #8, #4
 402bf7c:	d3483526 	ubfx	x6, x9, #8, #6
 402bf80:	1ac10861 	udiv	w1, w3, w1
 402bf84:	d35044a3 	ubfx	x3, x5, #16, #2
 402bf88:	12001885 	and	w5, w4, #0x7f
 402bf8c:	4b0100a5 	sub	w5, w5, w1
 402bf90:	0b050028 	add	w8, w1, w5
 402bf94:	6b01011f 	cmp	w8, w1
 402bf98:	540001a9 	b.ls	402bfcc <ddrtrn_hal_wdqs_bdl2phase+0x90>  // b.plast
 402bf9c:	710034ff 	cmp	w7, #0xd
 402bfa0:	54000128 	b.hi	402bfc4 <ddrtrn_hal_wdqs_bdl2phase+0x88>  // b.pmore
 402bfa4:	110004e8 	add	w8, w7, #0x1
 402bfa8:	1200050c 	and	w12, w8, #0x3
 402bfac:	71000d9f 	cmp	w12, #0x3
 402bfb0:	54000041 	b.ne	402bfb8 <ddrtrn_hal_wdqs_bdl2phase+0x7c>  // b.any
 402bfb4:	110008e8 	add	w8, w7, #0x2
 402bfb8:	4b0100a5 	sub	w5, w5, w1
 402bfbc:	2a0803e7 	mov	w7, w8
 402bfc0:	17fffff4 	b	402bf90 <ddrtrn_hal_wdqs_bdl2phase+0x54>
 402bfc4:	7100047f 	cmp	w3, #0x1
 402bfc8:	540001c9 	b.ls	402c000 <ddrtrn_hal_wdqs_bdl2phase+0xc4>  // b.plast
 402bfcc:	12196084 	and	w4, w4, #0xffffff80
 402bfd0:	12126529 	and	w9, w9, #0xffffc0ff
 402bfd4:	2a080084 	orr	w4, w4, w8
 402bfd8:	2a062126 	orr	w6, w9, w6, lsl #8
 402bfdc:	12146c84 	and	w4, w4, #0xfffff0ff
 402bfe0:	2a072084 	orr	w4, w4, w7, lsl #8
 402bfe4:	b9000164 	str	w4, [x11]
 402bfe8:	b9400041 	ldr	w1, [x2]
 402bfec:	120e7421 	and	w1, w1, #0xfffcffff
 402bff0:	2a034023 	orr	w3, w1, w3, lsl #16
 402bff4:	b9000043 	str	w3, [x2]
 402bff8:	b9000146 	str	w6, [x10]
 402bffc:	17ffff0e 	b	402bc34 <ddrtrn_hal_phy_cfg_update>
 402c000:	11000463 	add	w3, w3, #0x1
 402c004:	71003cdf 	cmp	w6, #0xf
 402c008:	54000089 	b.ls	402c018 <ddrtrn_hal_wdqs_bdl2phase+0xdc>  // b.plast
 402c00c:	510040c6 	sub	w6, w6, #0x10
 402c010:	52800008 	mov	w8, #0x0                   	// #0
 402c014:	17ffffe9 	b	402bfb8 <ddrtrn_hal_wdqs_bdl2phase+0x7c>
 402c018:	52800006 	mov	w6, #0x0                   	// #0
 402c01c:	17fffffd 	b	402c010 <ddrtrn_hal_wdqs_bdl2phase+0xd4>
 402c020:	d65f03c0 	ret

000000000402c024 <ddrtrn_hal_training_adjust_wdq>:
 402c024:	d10343ff 	sub	sp, sp, #0xd0
 402c028:	b0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402c02c:	a90753f3 	stp	x19, x20, [sp, #112]
 402c030:	a9085bf5 	stp	x21, x22, [sp, #128]
 402c034:	52800016 	mov	w22, #0x0                   	// #0
 402c038:	a90963f7 	stp	x23, x24, [sp, #144]
 402c03c:	52804698 	mov	w24, #0x234                 	// #564
 402c040:	a90a6bf9 	stp	x25, x26, [sp, #160]
 402c044:	5280019a 	mov	w26, #0xc                   	// #12
 402c048:	a90b73fb 	stp	x27, x28, [sp, #176]
 402c04c:	5280009b 	mov	w27, #0x4                   	// #4
 402c050:	f90063fe 	str	x30, [sp, #192]
 402c054:	f9477001 	ldr	x1, [x0, #3808]
 402c058:	f90037e1 	str	x1, [sp, #104]
 402c05c:	d2800001 	mov	x1, #0x0                   	// #0
 402c060:	97fffe8d 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c064:	2a0003f7 	mov	w23, w0
 402c068:	97fffe81 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c06c:	2a0003f9 	mov	w25, w0
 402c070:	97fffe94 	bl	402bac0 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402c074:	6b16001f 	cmp	w0, w22
 402c078:	54000188 	b.hi	402c0a8 <ddrtrn_hal_training_adjust_wdq+0x84>  // b.pmore
 402c07c:	2a1903e0 	mov	w0, w25
 402c080:	97fffe8a 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c084:	2a1703e0 	mov	w0, w23
 402c088:	97fffeeb 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402c08c:	b0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402c090:	f94037e1 	ldr	x1, [sp, #104]
 402c094:	f9477002 	ldr	x2, [x0, #3808]
 402c098:	eb020021 	subs	x1, x1, x2
 402c09c:	d2800002 	mov	x2, #0x0                   	// #0
 402c0a0:	54001160 	b.eq	402c2cc <ddrtrn_hal_training_adjust_wdq+0x2a8>  // b.none
 402c0a4:	97ffe856 	bl	40261fc <__stack_chk_fail>
 402c0a8:	97fffaad 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c0ac:	b9001816 	str	w22, [x0, #24]
 402c0b0:	2a1603e3 	mov	w3, w22
 402c0b4:	2a1703e1 	mov	w1, w23
 402c0b8:	910063e0 	add	x0, sp, #0x18
 402c0bc:	52800002 	mov	w2, #0x0                   	// #0
 402c0c0:	97ffff7d 	bl	402beb4 <ddrtrn_hal_get_dly_value>
 402c0c4:	910103e0 	add	x0, sp, #0x40
 402c0c8:	2a1703e1 	mov	w1, w23
 402c0cc:	2a1603e3 	mov	w3, w22
 402c0d0:	52800022 	mov	w2, #0x1                   	// #1
 402c0d4:	97ffff78 	bl	402beb4 <ddrtrn_hal_get_dly_value>
 402c0d8:	97fffe6f 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c0dc:	b9402bf4 	ldr	w20, [sp, #40]
 402c0e0:	b94053fc 	ldr	w28, [sp, #80]
 402c0e4:	51002301 	sub	w1, w24, #0x8
 402c0e8:	12000e94 	and	w20, w20, #0xf
 402c0ec:	0b000021 	add	w1, w1, w0
 402c0f0:	11000693 	add	w19, w20, #0x1
 402c0f4:	12000f9c 	and	w28, w28, #0xf
 402c0f8:	b9400035 	ldr	w21, [x1]
 402c0fc:	1adb0e73 	sdiv	w19, w19, w27
 402c100:	d35062b5 	ubfx	x21, x21, #16, #9
 402c104:	4b130280 	sub	w0, w20, w19
 402c108:	11000793 	add	w19, w28, #0x1
 402c10c:	1ada0ab5 	udiv	w21, w21, w26
 402c110:	1adb0e73 	sdiv	w19, w19, w27
 402c114:	4b130393 	sub	w19, w28, w19
 402c118:	6b13001f 	cmp	w0, w19
 402c11c:	54000a8b 	b.lt	402c26c <ddrtrn_hal_training_adjust_wdq+0x248>  // b.tstop
 402c120:	4b130013 	sub	w19, w0, w19
 402c124:	52800000 	mov	w0, #0x0                   	// #0
 402c128:	97fffe60 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c12c:	71001a7f 	cmp	w19, #0x6
 402c130:	5400012d 	b.le	402c154 <ddrtrn_hal_training_adjust_wdq+0x130>
 402c134:	b94053e0 	ldr	w0, [sp, #80]
 402c138:	71003c1f 	cmp	w0, #0xf
 402c13c:	540000c9 	b.ls	402c154 <ddrtrn_hal_training_adjust_wdq+0x130>  // b.plast
 402c140:	51004000 	sub	w0, w0, #0x10
 402c144:	4b130353 	sub	w19, w26, w19
 402c148:	b90053e0 	str	w0, [sp, #80]
 402c14c:	52800020 	mov	w0, #0x1                   	// #1
 402c150:	97fffe56 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c154:	97fffe46 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c158:	35000a40 	cbnz	w0, 402c2a0 <ddrtrn_hal_training_adjust_wdq+0x27c>
 402c15c:	b9402be0 	ldr	w0, [sp, #40]
 402c160:	0b00039c 	add	w28, w28, w0
 402c164:	4b140394 	sub	w20, w28, w20
 402c168:	b94027fc 	ldr	w28, [sp, #36]
 402c16c:	b9002bf4 	str	w20, [sp, #40]
 402c170:	1212679c 	and	w28, w28, #0xffffc0ff
 402c174:	2a142394 	orr	w20, w28, w20, lsl #8
 402c178:	97fffe47 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c17c:	2a0003fc 	mov	w28, w0
 402c180:	0b1c031c 	add	w28, w24, w28
 402c184:	97fffe3a 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c188:	0b002b9c 	add	w28, w28, w0, lsl #10
 402c18c:	b9000394 	str	w20, [x28]
 402c190:	97fffe3c 	bl	402ba80 <ddrtrn_hal_get_cur_mode>
 402c194:	2a0003f4 	mov	w20, w0
 402c198:	97fffa71 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c19c:	1b137eb3 	mul	w19, w21, w19
 402c1a0:	52800041 	mov	w1, #0x2                   	// #2
 402c1a4:	b9001401 	str	w1, [x0, #20]
 402c1a8:	97fffe3b 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c1ac:	2a0003fc 	mov	w28, w0
 402c1b0:	97fffe2f 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c1b4:	2a0003f5 	mov	w21, w0
 402c1b8:	97fffe28 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c1bc:	53196000 	lsl	w0, w0, #7
 402c1c0:	0b152815 	add	w21, w0, w21, lsl #10
 402c1c4:	1108639c 	add	w28, w28, #0x218
 402c1c8:	0b1c02b5 	add	w21, w21, w28
 402c1cc:	52800ffc 	mov	w28, #0x7f                  	// #127
 402c1d0:	b94002a0 	ldr	w0, [x21]
 402c1d4:	12001801 	and	w1, w0, #0x7f
 402c1d8:	12196000 	and	w0, w0, #0xffffff80
 402c1dc:	0b010261 	add	w1, w19, w1
 402c1e0:	6b1c003f 	cmp	w1, w28
 402c1e4:	1a9cd021 	csel	w1, w1, w28, le
 402c1e8:	7100003f 	cmp	w1, #0x0
 402c1ec:	1a9fa021 	csel	w1, w1, wzr, ge  // ge = tcont
 402c1f0:	2a010001 	orr	w1, w0, w1
 402c1f4:	b9000fe1 	str	w1, [sp, #12]
 402c1f8:	97fffe27 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c1fc:	b9000be0 	str	w0, [sp, #8]
 402c200:	97fffe1b 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c204:	2a0003f5 	mov	w21, w0
 402c208:	97fffe14 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c20c:	53196000 	lsl	w0, w0, #7
 402c210:	294107e2 	ldp	w2, w1, [sp, #8]
 402c214:	0b152815 	add	w21, w0, w21, lsl #10
 402c218:	11086042 	add	w2, w2, #0x218
 402c21c:	0b0202b5 	add	w21, w21, w2
 402c220:	b90002a1 	str	w1, [x21]
 402c224:	52800015 	mov	w21, #0x0                   	// #0
 402c228:	97fffa4d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c22c:	b9001c15 	str	w21, [x0, #28]
 402c230:	110006b5 	add	w21, w21, #0x1
 402c234:	97fffe2e 	bl	402baec <ddrtrn_hal_phy_get_dq_bdl>
 402c238:	0b000260 	add	w0, w19, w0
 402c23c:	7101fc1f 	cmp	w0, #0x7f
 402c240:	1a9cd000 	csel	w0, w0, w28, le
 402c244:	7100001f 	cmp	w0, #0x0
 402c248:	1a9fa000 	csel	w0, w0, wzr, ge  // ge = tcont
 402c24c:	97fffe91 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402c250:	710022bf 	cmp	w21, #0x8
 402c254:	54fffea1 	b.ne	402c228 <ddrtrn_hal_training_adjust_wdq+0x204>  // b.any
 402c258:	97fffa41 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c25c:	110006d6 	add	w22, w22, #0x1
 402c260:	11020318 	add	w24, w24, #0x80
 402c264:	b9001414 	str	w20, [x0, #20]
 402c268:	17ffff82 	b	402c070 <ddrtrn_hal_training_adjust_wdq+0x4c>
 402c26c:	4b000273 	sub	w19, w19, w0
 402c270:	52800020 	mov	w0, #0x1                   	// #1
 402c274:	97fffe0d 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c278:	71001a7f 	cmp	w19, #0x6
 402c27c:	54fff6cd 	b.le	402c154 <ddrtrn_hal_training_adjust_wdq+0x130>
 402c280:	b9402be0 	ldr	w0, [sp, #40]
 402c284:	71003c1f 	cmp	w0, #0xf
 402c288:	54fff669 	b.ls	402c154 <ddrtrn_hal_training_adjust_wdq+0x130>  // b.plast
 402c28c:	51004000 	sub	w0, w0, #0x10
 402c290:	4b130353 	sub	w19, w26, w19
 402c294:	b9002be0 	str	w0, [sp, #40]
 402c298:	52800000 	mov	w0, #0x0                   	// #0
 402c29c:	17ffffad 	b	402c150 <ddrtrn_hal_training_adjust_wdq+0x12c>
 402c2a0:	97fffdf3 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c2a4:	7100041f 	cmp	w0, #0x1
 402c2a8:	54fff741 	b.ne	402c190 <ddrtrn_hal_training_adjust_wdq+0x16c>  // b.any
 402c2ac:	b94053e0 	ldr	w0, [sp, #80]
 402c2b0:	0b000294 	add	w20, w20, w0
 402c2b4:	b9404fe0 	ldr	w0, [sp, #76]
 402c2b8:	4b1c0294 	sub	w20, w20, w28
 402c2bc:	b90053f4 	str	w20, [sp, #80]
 402c2c0:	12126400 	and	w0, w0, #0xffffc0ff
 402c2c4:	2a142014 	orr	w20, w0, w20, lsl #8
 402c2c8:	17ffffac 	b	402c178 <ddrtrn_hal_training_adjust_wdq+0x154>
 402c2cc:	a94753f3 	ldp	x19, x20, [sp, #112]
 402c2d0:	a9485bf5 	ldp	x21, x22, [sp, #128]
 402c2d4:	a94963f7 	ldp	x23, x24, [sp, #144]
 402c2d8:	a94a6bf9 	ldp	x25, x26, [sp, #160]
 402c2dc:	a94b73fb 	ldp	x27, x28, [sp, #176]
 402c2e0:	f94063fe 	ldr	x30, [sp, #192]
 402c2e4:	910343ff 	add	sp, sp, #0xd0
 402c2e8:	d65f03c0 	ret

000000000402c2ec <ddrtrn_hal_training_adjust_wdqs>:
 402c2ec:	d10343ff 	sub	sp, sp, #0xd0
 402c2f0:	b0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402c2f4:	a90753f3 	stp	x19, x20, [sp, #112]
 402c2f8:	a9085bf5 	stp	x21, x22, [sp, #128]
 402c2fc:	52804616 	mov	w22, #0x230                 	// #560
 402c300:	a90963f7 	stp	x23, x24, [sp, #144]
 402c304:	52800017 	mov	w23, #0x0                   	// #0
 402c308:	a90a6bf9 	stp	x25, x26, [sp, #160]
 402c30c:	5280019a 	mov	w26, #0xc                   	// #12
 402c310:	a90b73fb 	stp	x27, x28, [sp, #176]
 402c314:	5280009b 	mov	w27, #0x4                   	// #4
 402c318:	f90063fe 	str	x30, [sp, #192]
 402c31c:	f9477001 	ldr	x1, [x0, #3808]
 402c320:	f90037e1 	str	x1, [sp, #104]
 402c324:	d2800001 	mov	x1, #0x0                   	// #0
 402c328:	97fffddb 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c32c:	2a0003f8 	mov	w24, w0
 402c330:	97fffdcf 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c334:	2a0003f9 	mov	w25, w0
 402c338:	97fffde2 	bl	402bac0 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402c33c:	6b17001f 	cmp	w0, w23
 402c340:	54000188 	b.hi	402c370 <ddrtrn_hal_training_adjust_wdqs+0x84>  // b.pmore
 402c344:	2a1903e0 	mov	w0, w25
 402c348:	97fffdd8 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c34c:	2a1803e0 	mov	w0, w24
 402c350:	97fffe39 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402c354:	b0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402c358:	f94037e1 	ldr	x1, [sp, #104]
 402c35c:	f9477002 	ldr	x2, [x0, #3808]
 402c360:	eb020021 	subs	x1, x1, x2
 402c364:	d2800002 	mov	x2, #0x0                   	// #0
 402c368:	54001460 	b.eq	402c5f4 <ddrtrn_hal_training_adjust_wdqs+0x308>  // b.none
 402c36c:	97ffe7a4 	bl	40261fc <__stack_chk_fail>
 402c370:	97fff9fb 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c374:	b9001817 	str	w23, [x0, #24]
 402c378:	2a1703e3 	mov	w3, w23
 402c37c:	2a1803e1 	mov	w1, w24
 402c380:	910063e0 	add	x0, sp, #0x18
 402c384:	52800002 	mov	w2, #0x0                   	// #0
 402c388:	97fffecb 	bl	402beb4 <ddrtrn_hal_get_dly_value>
 402c38c:	910103e0 	add	x0, sp, #0x40
 402c390:	2a1803e1 	mov	w1, w24
 402c394:	2a1703e3 	mov	w3, w23
 402c398:	52800022 	mov	w2, #0x1                   	// #1
 402c39c:	97fffec6 	bl	402beb4 <ddrtrn_hal_get_dly_value>
 402c3a0:	97fffdbd 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c3a4:	b9401ff4 	ldr	w20, [sp, #28]
 402c3a8:	b94047fc 	ldr	w28, [sp, #68]
 402c3ac:	510012c1 	sub	w1, w22, #0x4
 402c3b0:	12000e94 	and	w20, w20, #0xf
 402c3b4:	0b000021 	add	w1, w1, w0
 402c3b8:	11000693 	add	w19, w20, #0x1
 402c3bc:	12000f9c 	and	w28, w28, #0xf
 402c3c0:	b9400035 	ldr	w21, [x1]
 402c3c4:	1adb0e73 	sdiv	w19, w19, w27
 402c3c8:	d35062b5 	ubfx	x21, x21, #16, #9
 402c3cc:	4b130280 	sub	w0, w20, w19
 402c3d0:	11000793 	add	w19, w28, #0x1
 402c3d4:	1ada0ab5 	udiv	w21, w21, w26
 402c3d8:	1adb0e73 	sdiv	w19, w19, w27
 402c3dc:	4b130393 	sub	w19, w28, w19
 402c3e0:	6b13001f 	cmp	w0, w19
 402c3e4:	54000aab 	b.lt	402c538 <ddrtrn_hal_training_adjust_wdqs+0x24c>  // b.tstop
 402c3e8:	4b130013 	sub	w19, w0, w19
 402c3ec:	52800000 	mov	w0, #0x0                   	// #0
 402c3f0:	97fffdae 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c3f4:	71001a7f 	cmp	w19, #0x6
 402c3f8:	5400040d 	b.le	402c478 <ddrtrn_hal_training_adjust_wdqs+0x18c>
 402c3fc:	b9405be0 	ldr	w0, [sp, #88]
 402c400:	340003c0 	cbz	w0, 402c478 <ddrtrn_hal_training_adjust_wdqs+0x18c>
 402c404:	b94053e1 	ldr	w1, [sp, #80]
 402c408:	7100b83f 	cmp	w1, #0x2e
 402c40c:	54000368 	b.hi	402c478 <ddrtrn_hal_training_adjust_wdqs+0x18c>  // b.pmore
 402c410:	11004021 	add	w1, w1, #0x10
 402c414:	b90053e1 	str	w1, [sp, #80]
 402c418:	b94057e1 	ldr	w1, [sp, #84]
 402c41c:	51000400 	sub	w0, w0, #0x1
 402c420:	b9005be0 	str	w0, [sp, #88]
 402c424:	4b130353 	sub	w19, w26, w19
 402c428:	120e7421 	and	w1, w1, #0xfffcffff
 402c42c:	2a004021 	orr	w1, w1, w0, lsl #16
 402c430:	b9000fe1 	str	w1, [sp, #12]
 402c434:	97fffd98 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c438:	111022c2 	add	w2, w22, #0x408
 402c43c:	b9400fe1 	ldr	w1, [sp, #12]
 402c440:	0b000042 	add	w2, w2, w0
 402c444:	b9000041 	str	w1, [x2]
 402c448:	b9404fe2 	ldr	w2, [sp, #76]
 402c44c:	12126440 	and	w0, w2, #0xffffc0ff
 402c450:	b94053e2 	ldr	w2, [sp, #80]
 402c454:	2a022002 	orr	w2, w0, w2, lsl #8
 402c458:	b9000fe2 	str	w2, [sp, #12]
 402c45c:	97fffd8e 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c460:	111012c1 	add	w1, w22, #0x404
 402c464:	b9400fe2 	ldr	w2, [sp, #12]
 402c468:	0b000021 	add	w1, w1, w0
 402c46c:	52800020 	mov	w0, #0x1                   	// #1
 402c470:	b9000022 	str	w2, [x1]
 402c474:	97fffd8d 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c478:	97fffd7d 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c47c:	35000a60 	cbnz	w0, 402c5c8 <ddrtrn_hal_training_adjust_wdqs+0x2dc>
 402c480:	b9401fe0 	ldr	w0, [sp, #28]
 402c484:	0b00039c 	add	w28, w28, w0
 402c488:	4b140394 	sub	w20, w28, w20
 402c48c:	b9401bfc 	ldr	w28, [sp, #24]
 402c490:	b9001ff4 	str	w20, [sp, #28]
 402c494:	12146f9c 	and	w28, w28, #0xfffff0ff
 402c498:	2a142394 	orr	w20, w28, w20, lsl #8
 402c49c:	97fffd7e 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c4a0:	2a0003fc 	mov	w28, w0
 402c4a4:	0b1c02dc 	add	w28, w22, w28
 402c4a8:	97fffd71 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c4ac:	0b002b9c 	add	w28, w28, w0, lsl #10
 402c4b0:	b9000394 	str	w20, [x28]
 402c4b4:	97fffd78 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c4b8:	1b137eb3 	mul	w19, w21, w19
 402c4bc:	2a0003f5 	mov	w21, w0
 402c4c0:	97fffd6b 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c4c4:	2a0003f4 	mov	w20, w0
 402c4c8:	97fffd64 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c4cc:	53196000 	lsl	w0, w0, #7
 402c4d0:	1108c2b5 	add	w21, w21, #0x230
 402c4d4:	0b142814 	add	w20, w0, w20, lsl #10
 402c4d8:	110006f7 	add	w23, w23, #0x1
 402c4dc:	0b150294 	add	w20, w20, w21
 402c4e0:	110202d6 	add	w22, w22, #0x80
 402c4e4:	b9400280 	ldr	w0, [x20]
 402c4e8:	12001801 	and	w1, w0, #0x7f
 402c4ec:	12196000 	and	w0, w0, #0xffffff80
 402c4f0:	0b010273 	add	w19, w19, w1
 402c4f4:	52800fe1 	mov	w1, #0x7f                  	// #127
 402c4f8:	7101fe7f 	cmp	w19, #0x7f
 402c4fc:	1a81d273 	csel	w19, w19, w1, le
 402c500:	7100027f 	cmp	w19, #0x0
 402c504:	1a9fa273 	csel	w19, w19, wzr, ge  // ge = tcont
 402c508:	2a130013 	orr	w19, w0, w19
 402c50c:	97fffd62 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c510:	2a0003f5 	mov	w21, w0
 402c514:	97fffd56 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c518:	2a0003f4 	mov	w20, w0
 402c51c:	97fffd4f 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c520:	53196000 	lsl	w0, w0, #7
 402c524:	0b142814 	add	w20, w0, w20, lsl #10
 402c528:	1108c2b5 	add	w21, w21, #0x230
 402c52c:	0b150294 	add	w20, w20, w21
 402c530:	b9000293 	str	w19, [x20]
 402c534:	17ffff81 	b	402c338 <ddrtrn_hal_training_adjust_wdqs+0x4c>
 402c538:	4b000273 	sub	w19, w19, w0
 402c53c:	52800020 	mov	w0, #0x1                   	// #1
 402c540:	97fffd5a 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c544:	71001a7f 	cmp	w19, #0x6
 402c548:	54fff98d 	b.le	402c478 <ddrtrn_hal_training_adjust_wdqs+0x18c>
 402c54c:	b94033e0 	ldr	w0, [sp, #48]
 402c550:	34fff940 	cbz	w0, 402c478 <ddrtrn_hal_training_adjust_wdqs+0x18c>
 402c554:	b9402be1 	ldr	w1, [sp, #40]
 402c558:	7100b83f 	cmp	w1, #0x2e
 402c55c:	54fff8e8 	b.hi	402c478 <ddrtrn_hal_training_adjust_wdqs+0x18c>  // b.pmore
 402c560:	11004021 	add	w1, w1, #0x10
 402c564:	b9002be1 	str	w1, [sp, #40]
 402c568:	b9402fe1 	ldr	w1, [sp, #44]
 402c56c:	51000400 	sub	w0, w0, #0x1
 402c570:	b90033e0 	str	w0, [sp, #48]
 402c574:	4b130353 	sub	w19, w26, w19
 402c578:	120e7421 	and	w1, w1, #0xfffcffff
 402c57c:	2a004021 	orr	w1, w1, w0, lsl #16
 402c580:	b9000fe1 	str	w1, [sp, #12]
 402c584:	97fffd44 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c588:	110022c2 	add	w2, w22, #0x8
 402c58c:	b9400fe1 	ldr	w1, [sp, #12]
 402c590:	0b000042 	add	w2, w2, w0
 402c594:	b9000041 	str	w1, [x2]
 402c598:	b94027e2 	ldr	w2, [sp, #36]
 402c59c:	12126440 	and	w0, w2, #0xffffc0ff
 402c5a0:	b9402be2 	ldr	w2, [sp, #40]
 402c5a4:	2a022002 	orr	w2, w0, w2, lsl #8
 402c5a8:	b9000fe2 	str	w2, [sp, #12]
 402c5ac:	97fffd3a 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c5b0:	110012c1 	add	w1, w22, #0x4
 402c5b4:	b9400fe2 	ldr	w2, [sp, #12]
 402c5b8:	0b000021 	add	w1, w1, w0
 402c5bc:	52800000 	mov	w0, #0x0                   	// #0
 402c5c0:	b9000022 	str	w2, [x1]
 402c5c4:	17ffffac 	b	402c474 <ddrtrn_hal_training_adjust_wdqs+0x188>
 402c5c8:	97fffd29 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c5cc:	7100041f 	cmp	w0, #0x1
 402c5d0:	54fff721 	b.ne	402c4b4 <ddrtrn_hal_training_adjust_wdqs+0x1c8>  // b.any
 402c5d4:	b94047e0 	ldr	w0, [sp, #68]
 402c5d8:	0b000294 	add	w20, w20, w0
 402c5dc:	b94043e0 	ldr	w0, [sp, #64]
 402c5e0:	4b1c0294 	sub	w20, w20, w28
 402c5e4:	b90047f4 	str	w20, [sp, #68]
 402c5e8:	12146c00 	and	w0, w0, #0xfffff0ff
 402c5ec:	2a142014 	orr	w20, w0, w20, lsl #8
 402c5f0:	17ffffab 	b	402c49c <ddrtrn_hal_training_adjust_wdqs+0x1b0>
 402c5f4:	a94753f3 	ldp	x19, x20, [sp, #112]
 402c5f8:	a9485bf5 	ldp	x21, x22, [sp, #128]
 402c5fc:	a94963f7 	ldp	x23, x24, [sp, #144]
 402c600:	a94a6bf9 	ldp	x25, x26, [sp, #160]
 402c604:	a94b73fb 	ldp	x27, x28, [sp, #176]
 402c608:	f94063fe 	ldr	x30, [sp, #192]
 402c60c:	910343ff 	add	sp, sp, #0xd0
 402c610:	d65f03c0 	ret

000000000402c614 <ddrtrn_hal_training_delay>:
 402c614:	35000040 	cbnz	w0, 402c61c <ddrtrn_hal_training_delay+0x8>
 402c618:	d65f03c0 	ret
 402c61c:	d503201f 	nop
 402c620:	51000400 	sub	w0, w0, #0x1
 402c624:	17fffffc 	b	402c614 <ddrtrn_hal_training_delay>

000000000402c628 <ddrtrn_hal_training_stat>:
 402c628:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402c62c:	2a0003f3 	mov	w19, w0
 402c630:	2a0303f4 	mov	w20, w3
 402c634:	a9015bf5 	stp	x21, x22, [sp, #16]
 402c638:	2a0103f6 	mov	w22, w1
 402c63c:	2a0203f5 	mov	w21, w2
 402c640:	f90013fe 	str	x30, [sp, #32]
 402c644:	940008f1 	bl	402ea08 <ddrtrn_training_error>
 402c648:	d2801600 	mov	x0, #0xb0                  	// #176
 402c64c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402c650:	b9400000 	ldr	w0, [x0]
 402c654:	35000220 	cbnz	w0, 402c698 <ddrtrn_hal_training_stat+0x70>
 402c658:	340000f6 	cbz	w22, 402c674 <ddrtrn_hal_training_stat+0x4c>
 402c65c:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402c660:	6b0002df 	cmp	w22, w0
 402c664:	52840001 	mov	w1, #0x2000                	// #8192
 402c668:	52820000 	mov	w0, #0x1000                	// #4096
 402c66c:	1a810000 	csel	w0, w0, w1, eq  // eq = none
 402c670:	2a000273 	orr	w19, w19, w0
 402c674:	310006bf 	cmn	w21, #0x1
 402c678:	54000040 	b.eq	402c680 <ddrtrn_hal_training_stat+0x58>  // b.none
 402c67c:	2a156273 	orr	w19, w19, w21, lsl #24
 402c680:	3100069f 	cmn	w20, #0x1
 402c684:	54000040 	b.eq	402c68c <ddrtrn_hal_training_stat+0x64>  // b.none
 402c688:	2a145273 	orr	w19, w19, w20, lsl #20
 402c68c:	d2801600 	mov	x0, #0xb0                  	// #176
 402c690:	f2a22040 	movk	x0, #0x1102, lsl #16
 402c694:	b9000013 	str	w19, [x0]
 402c698:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402c69c:	f94013fe 	ldr	x30, [sp, #32]
 402c6a0:	a8c353f3 	ldp	x19, x20, [sp], #48
 402c6a4:	d65f03c0 	ret

000000000402c6a8 <ddrtrn_hal_rdqs_sync_rank_rdq>:
 402c6a8:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402c6ac:	2a0003f4 	mov	w20, w0
 402c6b0:	a9015bf5 	stp	x21, x22, [sp, #16]
 402c6b4:	a90263f7 	stp	x23, x24, [sp, #32]
 402c6b8:	f9001bfe 	str	x30, [sp, #48]
 402c6bc:	97fffcf1 	bl	402ba80 <ddrtrn_hal_get_cur_mode>
 402c6c0:	2a0003f5 	mov	w21, w0
 402c6c4:	97fff926 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c6c8:	52800021 	mov	w1, #0x1                   	// #1
 402c6cc:	b9001401 	str	w1, [x0, #20]
 402c6d0:	97fffcf1 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c6d4:	2a0003f6 	mov	w22, w0
 402c6d8:	97fffce5 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c6dc:	2a0003f3 	mov	w19, w0
 402c6e0:	97fffcde 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c6e4:	53196000 	lsl	w0, w0, #7
 402c6e8:	0b132813 	add	w19, w0, w19, lsl #10
 402c6ec:	110892d6 	add	w22, w22, #0x224
 402c6f0:	0b160273 	add	w19, w19, w22
 402c6f4:	52800ff6 	mov	w22, #0x7f                  	// #127
 402c6f8:	b9400260 	ldr	w0, [x19]
 402c6fc:	12001801 	and	w1, w0, #0x7f
 402c700:	12196000 	and	w0, w0, #0xffffff80
 402c704:	0b010281 	add	w1, w20, w1
 402c708:	6b16003f 	cmp	w1, w22
 402c70c:	1a96d021 	csel	w1, w1, w22, le
 402c710:	7100003f 	cmp	w1, #0x0
 402c714:	1a9fa021 	csel	w1, w1, wzr, ge  // ge = tcont
 402c718:	2a010017 	orr	w23, w0, w1
 402c71c:	97fffcde 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c720:	2a0003f8 	mov	w24, w0
 402c724:	97fffcd2 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c728:	2a0003f3 	mov	w19, w0
 402c72c:	97fffccb 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c730:	53196000 	lsl	w0, w0, #7
 402c734:	0b132813 	add	w19, w0, w19, lsl #10
 402c738:	11089318 	add	w24, w24, #0x224
 402c73c:	0b180273 	add	w19, w19, w24
 402c740:	b9000277 	str	w23, [x19]
 402c744:	52800013 	mov	w19, #0x0                   	// #0
 402c748:	97fff905 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c74c:	b9001c13 	str	w19, [x0, #28]
 402c750:	11000673 	add	w19, w19, #0x1
 402c754:	97fffce6 	bl	402baec <ddrtrn_hal_phy_get_dq_bdl>
 402c758:	0b000280 	add	w0, w20, w0
 402c75c:	7101fc1f 	cmp	w0, #0x7f
 402c760:	1a96d000 	csel	w0, w0, w22, le
 402c764:	7100001f 	cmp	w0, #0x0
 402c768:	1a9fa000 	csel	w0, w0, wzr, ge  // ge = tcont
 402c76c:	97fffd49 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402c770:	7100227f 	cmp	w19, #0x8
 402c774:	54fffea1 	b.ne	402c748 <ddrtrn_hal_rdqs_sync_rank_rdq+0xa0>  // b.any
 402c778:	97fff8f9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c77c:	a94263f7 	ldp	x23, x24, [sp, #32]
 402c780:	f9401bfe 	ldr	x30, [sp, #48]
 402c784:	b9001415 	str	w21, [x0, #20]
 402c788:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402c78c:	a8c453f3 	ldp	x19, x20, [sp], #64
 402c790:	d65f03c0 	ret

000000000402c794 <ddrtrn_hal_rdqbdl_adj>:
 402c794:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402c798:	aa0003f3 	mov	x19, x0
 402c79c:	a9015bf5 	stp	x21, x22, [sp, #16]
 402c7a0:	a90263f7 	stp	x23, x24, [sp, #32]
 402c7a4:	a9037bf9 	stp	x25, x30, [sp, #48]
 402c7a8:	97fffcb1 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c7ac:	2a0003f4 	mov	w20, w0
 402c7b0:	97fffcb9 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c7b4:	2a0003f6 	mov	w22, w0
 402c7b8:	97fffca8 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c7bc:	2a0003f5 	mov	w21, w0
 402c7c0:	97fffcb0 	bl	402ba80 <ddrtrn_hal_get_cur_mode>
 402c7c4:	2a0003f7 	mov	w23, w0
 402c7c8:	97fff8e5 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c7cc:	53165694 	lsl	w20, w20, #10
 402c7d0:	52800021 	mov	w1, #0x1                   	// #1
 402c7d4:	b9001401 	str	w1, [x0, #20]
 402c7d8:	110892c0 	add	w0, w22, #0x224
 402c7dc:	1108b2d6 	add	w22, w22, #0x22c
 402c7e0:	0b151e94 	add	w20, w20, w21, lsl #7
 402c7e4:	d2800019 	mov	x25, #0x0                   	// #0
 402c7e8:	0b000294 	add	w20, w20, w0
 402c7ec:	0b151ed5 	add	w21, w22, w21, lsl #7
 402c7f0:	b9400298 	ldr	w24, [x20]
 402c7f4:	b94002b6 	ldr	w22, [x21]
 402c7f8:	97fff8d9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c7fc:	b9001c19 	str	w25, [x0, #28]
 402c800:	97fffcbb 	bl	402baec <ddrtrn_hal_phy_get_dq_bdl>
 402c804:	b8397a60 	str	w0, [x19, x25, lsl #2]
 402c808:	91000739 	add	x25, x25, #0x1
 402c80c:	f100233f 	cmp	x25, #0x8
 402c810:	54ffff41 	b.ne	402c7f8 <ddrtrn_hal_rdqbdl_adj+0x64>  // b.any
 402c814:	12001b00 	and	w0, w24, #0x7f
 402c818:	b9002260 	str	w0, [x19, #32]
 402c81c:	120022c0 	and	w0, w22, #0x1ff
 402c820:	12800001 	mov	w1, #0xffffffff            	// #-1
 402c824:	b9002660 	str	w0, [x19, #36]
 402c828:	d2800000 	mov	x0, #0x0                   	// #0
 402c82c:	b8607a62 	ldr	w2, [x19, x0, lsl #2]
 402c830:	91000400 	add	x0, x0, #0x1
 402c834:	6b02003f 	cmp	w1, w2
 402c838:	1a829021 	csel	w1, w1, w2, ls  // ls = plast
 402c83c:	f100281f 	cmp	x0, #0xa
 402c840:	54ffff61 	b.ne	402c82c <ddrtrn_hal_rdqbdl_adj+0x98>  // b.any
 402c844:	d2800000 	mov	x0, #0x0                   	// #0
 402c848:	b8607a62 	ldr	w2, [x19, x0, lsl #2]
 402c84c:	4b010042 	sub	w2, w2, w1
 402c850:	b8207a62 	str	w2, [x19, x0, lsl #2]
 402c854:	91000400 	add	x0, x0, #0x1
 402c858:	f100281f 	cmp	x0, #0xa
 402c85c:	54ffff61 	b.ne	402c848 <ddrtrn_hal_rdqbdl_adj+0xb4>  // b.any
 402c860:	d2800019 	mov	x25, #0x0                   	// #0
 402c864:	97fff8be 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c868:	b9001c19 	str	w25, [x0, #28]
 402c86c:	b8797a60 	ldr	w0, [x19, x25, lsl #2]
 402c870:	91000739 	add	x25, x25, #0x1
 402c874:	97fffd07 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402c878:	f100233f 	cmp	x25, #0x8
 402c87c:	54ffff41 	b.ne	402c864 <ddrtrn_hal_rdqbdl_adj+0xd0>  // b.any
 402c880:	b9402260 	ldr	w0, [x19, #32]
 402c884:	12196318 	and	w24, w24, #0xffffff80
 402c888:	12175ad6 	and	w22, w22, #0xfffffe00
 402c88c:	2a000318 	orr	w24, w24, w0
 402c890:	b9402660 	ldr	w0, [x19, #36]
 402c894:	b9000298 	str	w24, [x20]
 402c898:	2a0002d6 	orr	w22, w22, w0
 402c89c:	b90002b6 	str	w22, [x21]
 402c8a0:	97fff8af 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c8a4:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402c8a8:	a9437bf9 	ldp	x25, x30, [sp, #48]
 402c8ac:	b9001417 	str	w23, [x0, #20]
 402c8b0:	a94263f7 	ldp	x23, x24, [sp, #32]
 402c8b4:	a8c453f3 	ldp	x19, x20, [sp], #64
 402c8b8:	d65f03c0 	ret

000000000402c8bc <ddrtrn_hal_rdqs_sync>:
 402c8bc:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402c8c0:	2a0003f3 	mov	w19, w0
 402c8c4:	a9017bf5 	stp	x21, x30, [sp, #16]
 402c8c8:	97fffc69 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c8cc:	2a0003f5 	mov	w21, w0
 402c8d0:	97fffc71 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c8d4:	2a0003f4 	mov	w20, w0
 402c8d8:	1108b294 	add	w20, w20, #0x22c
 402c8dc:	97fffc5f 	bl	402ba58 <ddrtrn_hal_get_cur_byte>
 402c8e0:	0b001e94 	add	w20, w20, w0, lsl #7
 402c8e4:	b9400280 	ldr	w0, [x20]
 402c8e8:	12002000 	and	w0, w0, #0x1ff
 402c8ec:	4b000273 	sub	w19, w19, w0
 402c8f0:	2a1303e0 	mov	w0, w19
 402c8f4:	97ffff6d 	bl	402c6a8 <ddrtrn_hal_rdqs_sync_rank_rdq>
 402c8f8:	97fff899 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402c8fc:	aa0003f4 	mov	x20, x0
 402c900:	97fff892 	bl	402ab48 <ddrtrn_hal_get_phy>
 402c904:	d2800782 	mov	x2, #0x3c                  	// #60
 402c908:	b9402281 	ldr	w1, [x20, #32]
 402c90c:	9b020021 	madd	x1, x1, x2, x0
 402c910:	b9400c20 	ldr	w0, [x1, #12]
 402c914:	7100041f 	cmp	w0, #0x1
 402c918:	54000140 	b.eq	402c940 <ddrtrn_hal_rdqs_sync+0x84>  // b.none
 402c91c:	52800020 	mov	w0, #0x1                   	// #1
 402c920:	4b150000 	sub	w0, w0, w21
 402c924:	97fffc61 	bl	402baa8 <ddrtrn_hal_set_rank_id>
 402c928:	2a1303e0 	mov	w0, w19
 402c92c:	97ffff5f 	bl	402c6a8 <ddrtrn_hal_rdqs_sync_rank_rdq>
 402c930:	2a1503e0 	mov	w0, w21
 402c934:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402c938:	a8c253f3 	ldp	x19, x20, [sp], #32
 402c93c:	17fffc5b 	b	402baa8 <ddrtrn_hal_set_rank_id>
 402c940:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402c944:	a8c253f3 	ldp	x19, x20, [sp], #32
 402c948:	d65f03c0 	ret

000000000402c94c <ddrtrn_hal_save_rdqbdl>:
 402c94c:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402c950:	aa0003f3 	mov	x19, x0
 402c954:	a9017bf5 	stp	x21, x30, [sp, #16]
 402c958:	97fffc4f 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c95c:	2a0003f4 	mov	w20, w0
 402c960:	97fffc43 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c964:	2a0003f5 	mov	w21, w0
 402c968:	97fffc56 	bl	402bac0 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402c96c:	0b152a84 	add	w4, w20, w21, lsl #10
 402c970:	aa1303e1 	mov	x1, x19
 402c974:	11087083 	add	w3, w4, #0x21c
 402c978:	0b001c84 	add	w4, w4, w0, lsl #7
 402c97c:	1108b282 	add	w2, w20, #0x22c
 402c980:	11087084 	add	w4, w4, #0x21c
 402c984:	6b04007f 	cmp	w3, w4
 402c988:	54000081 	b.ne	402c998 <ddrtrn_hal_save_rdqbdl+0x4c>  // b.any
 402c98c:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402c990:	a8c253f3 	ldp	x19, x20, [sp], #32
 402c994:	d65f03c0 	ret
 402c998:	2a0303e0 	mov	w0, w3
 402c99c:	91001021 	add	x1, x1, #0x4
 402c9a0:	b9400000 	ldr	w0, [x0]
 402c9a4:	b81fc020 	stur	w0, [x1, #-4]
 402c9a8:	11001060 	add	w0, w3, #0x4
 402c9ac:	b9400000 	ldr	w0, [x0]
 402c9b0:	b9000c20 	str	w0, [x1, #12]
 402c9b4:	11002060 	add	w0, w3, #0x8
 402c9b8:	11020063 	add	w3, w3, #0x80
 402c9bc:	b9400000 	ldr	w0, [x0]
 402c9c0:	b9002c20 	str	w0, [x1, #44]
 402c9c4:	2a0203e0 	mov	w0, w2
 402c9c8:	11020042 	add	w2, w2, #0x80
 402c9cc:	b9400000 	ldr	w0, [x0]
 402c9d0:	b9001c20 	str	w0, [x1, #28]
 402c9d4:	17ffffec 	b	402c984 <ddrtrn_hal_save_rdqbdl+0x38>

000000000402c9d8 <ddrtrn_hal_restore_rdqbdl>:
 402c9d8:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402c9dc:	aa0003f3 	mov	x19, x0
 402c9e0:	a9017bf5 	stp	x21, x30, [sp, #16]
 402c9e4:	97fffc2c 	bl	402ba94 <ddrtrn_hal_get_cur_phy>
 402c9e8:	2a0003f4 	mov	w20, w0
 402c9ec:	97fffc20 	bl	402ba6c <ddrtrn_hal_get_rank_id>
 402c9f0:	2a0003f5 	mov	w21, w0
 402c9f4:	97fffc33 	bl	402bac0 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402c9f8:	0b152a84 	add	w4, w20, w21, lsl #10
 402c9fc:	aa1303e1 	mov	x1, x19
 402ca00:	11087083 	add	w3, w4, #0x21c
 402ca04:	0b001c84 	add	w4, w4, w0, lsl #7
 402ca08:	1108b282 	add	w2, w20, #0x22c
 402ca0c:	11087084 	add	w4, w4, #0x21c
 402ca10:	6b04007f 	cmp	w3, w4
 402ca14:	54000081 	b.ne	402ca24 <ddrtrn_hal_restore_rdqbdl+0x4c>  // b.any
 402ca18:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402ca1c:	a8c253f3 	ldp	x19, x20, [sp], #32
 402ca20:	d65f03c0 	ret
 402ca24:	2a0303e0 	mov	w0, w3
 402ca28:	b9400025 	ldr	w5, [x1]
 402ca2c:	91001021 	add	x1, x1, #0x4
 402ca30:	b9000005 	str	w5, [x0]
 402ca34:	11001060 	add	w0, w3, #0x4
 402ca38:	b9400c25 	ldr	w5, [x1, #12]
 402ca3c:	b9000005 	str	w5, [x0]
 402ca40:	11002060 	add	w0, w3, #0x8
 402ca44:	11020063 	add	w3, w3, #0x80
 402ca48:	b9402c25 	ldr	w5, [x1, #44]
 402ca4c:	b9000005 	str	w5, [x0]
 402ca50:	2a0203e0 	mov	w0, w2
 402ca54:	11020042 	add	w2, w2, #0x80
 402ca58:	b9401c25 	ldr	w5, [x1, #28]
 402ca5c:	b9000005 	str	w5, [x0]
 402ca60:	17ffffec 	b	402ca10 <ddrtrn_hal_restore_rdqbdl+0x38>

000000000402ca64 <ddrtrn_hal_get_gt_status>:
 402ca64:	11002000 	add	w0, w0, #0x8
 402ca68:	b9400000 	ldr	w0, [x0]
 402ca6c:	121b0000 	and	w0, w0, #0x20
 402ca70:	d65f03c0 	ret

000000000402ca74 <ddrtrn_hal_get_trfc_ctrl_val>:
 402ca74:	11238000 	add	w0, w0, #0x8e0
 402ca78:	b9400000 	ldr	w0, [x0]
 402ca7c:	d65f03c0 	ret

000000000402ca80 <ddrtrn_hal_set_trfc_ctrl>:
 402ca80:	11238021 	add	w1, w1, #0x8e0
 402ca84:	b9000020 	str	w0, [x1]
 402ca88:	d65f03c0 	ret

000000000402ca8c <ddrtrn_hal_get_misc_val>:
 402ca8c:	1101c000 	add	w0, w0, #0x70
 402ca90:	b9400000 	ldr	w0, [x0]
 402ca94:	d65f03c0 	ret

000000000402ca98 <ddrtrn_hal_set_misc_val>:
 402ca98:	1101c021 	add	w1, w1, #0x70
 402ca9c:	b9000020 	str	w0, [x1]
 402caa0:	d65f03c0 	ret

000000000402caa4 <ddrtrn_hal_get_trfc_threshold1_val>:
 402caa4:	11234000 	add	w0, w0, #0x8d0
 402caa8:	b9400000 	ldr	w0, [x0]
 402caac:	d65f03c0 	ret

000000000402cab0 <ddrtrn_hal_get_dmsel>:
 402cab0:	11021000 	add	w0, w0, #0x84
 402cab4:	b9400000 	ldr	w0, [x0]
 402cab8:	d65f03c0 	ret

000000000402cabc <ddrtrn_hal_get_phyctrl0>:
 402cabc:	1101e000 	add	w0, w0, #0x78
 402cac0:	b9400000 	ldr	w0, [x0]
 402cac4:	d65f03c0 	ret

000000000402cac8 <ddrtrn_hal_read_repeatedly>:
 402cac8:	0b010000 	add	w0, w0, w1
 402cacc:	b9400001 	ldr	w1, [x0]
 402cad0:	b9400001 	ldr	w1, [x0]
 402cad4:	b9400000 	ldr	w0, [x0]
 402cad8:	d65f03c0 	ret

000000000402cadc <ddrtrn_hal_enable_rdqs_anti_aging>:
 402cadc:	1102c000 	add	w0, w0, #0xb0
 402cae0:	b9400001 	ldr	w1, [x0]
 402cae4:	32010021 	orr	w1, w1, #0x80000000
 402cae8:	b9000001 	str	w1, [x0]
 402caec:	d65f03c0 	ret

000000000402caf0 <ddrtrn_hal_vref_get_host_max>:
 402caf0:	7100001f 	cmp	w0, #0x0
 402caf4:	528001e2 	mov	w2, #0xf                   	// #15
 402caf8:	52800fe0 	mov	w0, #0x7f                  	// #127
 402cafc:	1a820000 	csel	w0, w0, w2, eq  // eq = none
 402cb00:	b9000020 	str	w0, [x1]
 402cb04:	d65f03c0 	ret

000000000402cb08 <ddrtrn_hal_vref_phy_host_get>:
 402cb08:	53196042 	lsl	w2, w2, #7
 402cb0c:	1109d042 	add	w2, w2, #0x274
 402cb10:	350000c1 	cbnz	w1, 402cb28 <ddrtrn_hal_vref_phy_host_get+0x20>
 402cb14:	2a0203e1 	mov	w1, w2
 402cb18:	b8606820 	ldr	w0, [x1, x0]
 402cb1c:	12001800 	and	w0, w0, #0x7f
 402cb20:	b9000060 	str	w0, [x3]
 402cb24:	d65f03c0 	ret
 402cb28:	0b012841 	add	w1, w2, w1, lsl #10
 402cb2c:	b8606820 	ldr	w0, [x1, x0]
 402cb30:	d3586c00 	ubfx	x0, x0, #24, #4
 402cb34:	17fffffb 	b	402cb20 <ddrtrn_hal_vref_phy_host_get+0x18>

000000000402cb38 <ddrtrn_hal_vref_phy_dram_get>:
 402cb38:	53196042 	lsl	w2, w2, #7
 402cb3c:	1109c042 	add	w2, w2, #0x270
 402cb40:	b8606840 	ldr	w0, [x2, x0]
 402cb44:	12001400 	and	w0, w0, #0x3f
 402cb48:	b9000020 	str	w0, [x1]
 402cb4c:	d65f03c0 	ret

000000000402cb50 <ddrtrn_hal_phy_rdqs_sync_rdm>:
 402cb50:	17ffff5b 	b	402c8bc <ddrtrn_hal_rdqs_sync>

000000000402cb54 <ddrtrn_hal_dqsswap_save_func>:
 402cb54:	b9408422 	ldr	w2, [x1, #132]
 402cb58:	b9000002 	str	w2, [x0]
 402cb5c:	12057042 	and	w2, w2, #0xf8ffffff
 402cb60:	b9008422 	str	w2, [x1, #132]
 402cb64:	d65f03c0 	ret

000000000402cb68 <ddrtrn_hal_dqsswap_restore_func>:
 402cb68:	b9008420 	str	w0, [x1, #132]
 402cb6c:	d65f03c0 	ret

000000000402cb70 <ddrtrn_hal_phy_switch_rank>:
 402cb70:	11012004 	add	w4, w0, #0x48
 402cb74:	52826285 	mov	w5, #0x1314                	// #4884
 402cb78:	0b050002 	add	w2, w0, w5
 402cb7c:	b9400083 	ldr	w3, [x4]
 402cb80:	121c6c63 	and	w3, w3, #0xfffffff0
 402cb84:	2a010063 	orr	w3, w3, w1
 402cb88:	b9000083 	str	w3, [x4]
 402cb8c:	11032004 	add	w4, w0, #0xc8
 402cb90:	b9400083 	ldr	w3, [x4]
 402cb94:	12007863 	and	w3, w3, #0x7fffffff
 402cb98:	2a017c63 	orr	w3, w3, w1, lsl #31
 402cb9c:	b9000083 	str	w3, [x4]
 402cba0:	52824283 	mov	w3, #0x1214                	// #4628
 402cba4:	0b030004 	add	w4, w0, w3
 402cba8:	b9400083 	ldr	w3, [x4]
 402cbac:	12117863 	and	w3, w3, #0xffffbfff
 402cbb0:	2a013863 	orr	w3, w3, w1, lsl #14
 402cbb4:	b9000083 	str	w3, [x4]
 402cbb8:	b9400043 	ldr	w3, [x2]
 402cbbc:	12117863 	and	w3, w3, #0xffffbfff
 402cbc0:	2a013861 	orr	w1, w3, w1, lsl #14
 402cbc4:	b9000041 	str	w1, [x2]
 402cbc8:	17fffc1b 	b	402bc34 <ddrtrn_hal_phy_cfg_update>

000000000402cbcc <ddrtrn_hal_get_sysctrl_cfg>:
 402cbcc:	52a22041 	mov	w1, #0x11020000            	// #285343744
 402cbd0:	0b010000 	add	w0, w0, w1
 402cbd4:	b9400000 	ldr	w0, [x0]
 402cbd8:	d65f03c0 	ret

000000000402cbdc <ddrtrn_hal_set_sysctrl_cfg>:
 402cbdc:	d2801401 	mov	x1, #0xa0                  	// #160
 402cbe0:	f2a22041 	movk	x1, #0x1102, lsl #16
 402cbe4:	b9000020 	str	w0, [x1]
 402cbe8:	d65f03c0 	ret

000000000402cbec <ddrtrn_hal_version_flag>:
 402cbec:	d2801681 	mov	x1, #0xb4                  	// #180
 402cbf0:	f2a22041 	movk	x1, #0x1102, lsl #16
 402cbf4:	b9400020 	ldr	w0, [x1]
 402cbf8:	72804460 	movk	w0, #0x223
 402cbfc:	b9000020 	str	w0, [x1]
 402cc00:	d65f03c0 	ret

000000000402cc04 <ddrtrn_hal_check_sw_item>:
 402cc04:	d2801400 	mov	x0, #0xa0                  	// #160
 402cc08:	f2a22040 	movk	x0, #0x1102, lsl #16
 402cc0c:	b9400000 	ldr	w0, [x0]
 402cc10:	3100041f 	cmn	w0, #0x1
 402cc14:	540000e1 	b.ne	402cc30 <ddrtrn_hal_check_sw_item+0x2c>  // b.any
 402cc18:	d2801480 	mov	x0, #0xa4                  	// #164
 402cc1c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402cc20:	b9400000 	ldr	w0, [x0]
 402cc24:	3100041f 	cmn	w0, #0x1
 402cc28:	5a9f03e0 	csetm	w0, ne  // ne = any
 402cc2c:	d65f03c0 	ret
 402cc30:	12800000 	mov	w0, #0xffffffff            	// #-1
 402cc34:	17fffffe 	b	402cc2c <ddrtrn_hal_check_sw_item+0x28>

000000000402cc38 <ddrtrn_hal_check_not_dcc_item>:
 402cc38:	d2801400 	mov	x0, #0xa0                  	// #160
 402cc3c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402cc40:	b9400001 	ldr	w1, [x0]
 402cc44:	529fff00 	mov	w0, #0xfff8                	// #65528
 402cc48:	72beffe0 	movk	w0, #0xf7ff, lsl #16
 402cc4c:	0a000022 	and	w2, w1, w0
 402cc50:	6a21001f 	bics	wzr, w0, w1
 402cc54:	540000e1 	b.ne	402cc70 <ddrtrn_hal_check_not_dcc_item+0x38>  // b.any
 402cc58:	d2801480 	mov	x0, #0xa4                  	// #164
 402cc5c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402cc60:	b9400000 	ldr	w0, [x0]
 402cc64:	6a20005f 	bics	wzr, w2, w0
 402cc68:	5a9f13e0 	csetm	w0, eq  // eq = none
 402cc6c:	d65f03c0 	ret
 402cc70:	52800000 	mov	w0, #0x0                   	// #0
 402cc74:	17fffffe 	b	402cc6c <ddrtrn_hal_check_not_dcc_item+0x34>

000000000402cc78 <ddrtrn_hal_set_adjust>:
 402cc78:	d2801801 	mov	x1, #0xc0                  	// #192
 402cc7c:	f2a22041 	movk	x1, #0x1102, lsl #16
 402cc80:	b9000020 	str	w0, [x1]
 402cc84:	d65f03c0 	ret

000000000402cc88 <ddrtrn_hal_get_adjust>:
 402cc88:	d2801800 	mov	x0, #0xc0                  	// #192
 402cc8c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402cc90:	b9400000 	ldr	w0, [x0]
 402cc94:	d65f03c0 	ret

000000000402cc98 <ddrtrn_hal_clear_sysctrl_stat_reg>:
 402cc98:	d2801600 	mov	x0, #0xb0                  	// #176
 402cc9c:	f2a22040 	movk	x0, #0x1102, lsl #16
 402cca0:	b900001f 	str	wzr, [x0]
 402cca4:	d65f03c0 	ret

000000000402cca8 <ddrtrn_dmc_auto_power_down_cfg>:
 402cca8:	52920002 	mov	w2, #0x9000                	// #36864
 402ccac:	52900001 	mov	w1, #0x8000                	// #32768
 402ccb0:	72a22282 	movk	w2, #0x1114, lsl #16
 402ccb4:	72a22281 	movk	w1, #0x1114, lsl #16
 402ccb8:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402ccbc:	14000aba 	b	402f7a4 <ddrtrn_hal_dmc_auto_pd_by_phy>

000000000402ccc0 <ddrtrn_training_console>:
 402ccc0:	52800000 	mov	w0, #0x0                   	// #0
 402ccc4:	d65f03c0 	ret

000000000402ccc8 <ddrtrn_training_console_if>:
 402ccc8:	52800000 	mov	w0, #0x0                   	// #0
 402cccc:	d65f03c0 	ret

000000000402ccd0 <ddrtrn_dataeye_check_dq>:
 402ccd0:	f81f0ffe 	str	x30, [sp, #-16]!
 402ccd4:	94000b74 	bl	402faa4 <ddrtrn_hal_get_dq_type>
 402ccd8:	7100041f 	cmp	w0, #0x1
 402ccdc:	54000061 	b.ne	402cce8 <ddrtrn_dataeye_check_dq+0x18>  // b.any
 402cce0:	f84107fe 	ldr	x30, [sp], #16
 402cce4:	14000ac0 	b	402f7e4 <ddrtrn_ddrt_check>
 402cce8:	52800000 	mov	w0, #0x0                   	// #0
 402ccec:	f84107fe 	ldr	x30, [sp], #16
 402ccf0:	d65f03c0 	ret

000000000402ccf4 <ddrtrn_dataeye_search_dq>:
 402ccf4:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402ccf8:	2a0003f3 	mov	w19, w0
 402ccfc:	2a0303f4 	mov	w20, w3
 402cd00:	a9015bf5 	stp	x21, x22, [sp, #16]
 402cd04:	2a0103f5 	mov	w21, w1
 402cd08:	a90263f7 	stp	x23, x24, [sp, #32]
 402cd0c:	aa0203f7 	mov	x23, x2
 402cd10:	a9037bf9 	stp	x25, x30, [sp, #48]
 402cd14:	528000d9 	mov	w25, #0x6                   	// #6
 402cd18:	4b1302a0 	sub	w0, w21, w19
 402cd1c:	2a1303f8 	mov	w24, w19
 402cd20:	0b400673 	add	w19, w19, w0, lsr #1
 402cd24:	2a1303e0 	mov	w0, w19
 402cd28:	97fffbda 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402cd2c:	97ffffe9 	bl	402ccd0 <ddrtrn_dataeye_check_dq>
 402cd30:	71000a9f 	cmp	w20, #0x2
 402cd34:	54000180 	b.eq	402cd64 <ddrtrn_dataeye_search_dq+0x70>  // b.none
 402cd38:	7100129f 	cmp	w20, #0x4
 402cd3c:	540001e0 	b.eq	402cd78 <ddrtrn_dataeye_search_dq+0x84>  // b.none
 402cd40:	7100069f 	cmp	w20, #0x1
 402cd44:	1a9f0336 	csel	w22, w25, wzr, eq  // eq = none
 402cd48:	35000200 	cbnz	w0, 402cd88 <ddrtrn_dataeye_search_dq+0x94>
 402cd4c:	b90002f3 	str	w19, [x23]
 402cd50:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402cd54:	a94263f7 	ldp	x23, x24, [sp, #32]
 402cd58:	a9437bf9 	ldp	x25, x30, [sp, #48]
 402cd5c:	a8c453f3 	ldp	x19, x20, [sp], #64
 402cd60:	d65f03c0 	ret
 402cd64:	35000100 	cbnz	w0, 402cd84 <ddrtrn_dataeye_search_dq+0x90>
 402cd68:	2a1803e0 	mov	w0, w24
 402cd6c:	97fffbc9 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402cd70:	97ffffd8 	bl	402ccd0 <ddrtrn_dataeye_check_dq>
 402cd74:	17fffff6 	b	402cd4c <ddrtrn_dataeye_search_dq+0x58>
 402cd78:	350001e0 	cbnz	w0, 402cdb4 <ddrtrn_dataeye_search_dq+0xc0>
 402cd7c:	2a1503e0 	mov	w0, w21
 402cd80:	17fffffb 	b	402cd6c <ddrtrn_dataeye_search_dq+0x78>
 402cd84:	52800096 	mov	w22, #0x4                   	// #4
 402cd88:	6b13031f 	cmp	w24, w19
 402cd8c:	7a5312a4 	ccmp	w21, w19, #0x4, ne  // ne = any
 402cd90:	54fffe00 	b.eq	402cd50 <ddrtrn_dataeye_search_dq+0x5c>  // b.none
 402cd94:	360800d6 	tbz	w22, #1, 402cdac <ddrtrn_dataeye_search_dq+0xb8>
 402cd98:	2a1403e3 	mov	w3, w20
 402cd9c:	aa1703e2 	mov	x2, x23
 402cda0:	2a1303e1 	mov	w1, w19
 402cda4:	2a1803e0 	mov	w0, w24
 402cda8:	97ffffd3 	bl	402ccf4 <ddrtrn_dataeye_search_dq>
 402cdac:	3617fd36 	tbz	w22, #2, 402cd50 <ddrtrn_dataeye_search_dq+0x5c>
 402cdb0:	17ffffda 	b	402cd18 <ddrtrn_dataeye_search_dq+0x24>
 402cdb4:	52800056 	mov	w22, #0x2                   	// #2
 402cdb8:	17fffff4 	b	402cd88 <ddrtrn_dataeye_search_dq+0x94>

000000000402cdbc <ddrtrn_dataeye_deskew>:
 402cdbc:	d10243ff 	sub	sp, sp, #0x90
 402cdc0:	a90353f3 	stp	x19, x20, [sp, #48]
 402cdc4:	52800013 	mov	w19, #0x0                   	// #0
 402cdc8:	a9045bf5 	stp	x21, x22, [sp, #64]
 402cdcc:	aa0003f5 	mov	x21, x0
 402cdd0:	b0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402cdd4:	a90563f7 	stp	x23, x24, [sp, #80]
 402cdd8:	52800016 	mov	w22, #0x0                   	// #0
 402cddc:	52800017 	mov	w23, #0x0                   	// #0
 402cde0:	a9066bf9 	stp	x25, x26, [sp, #96]
 402cde4:	5280001a 	mov	w26, #0x0                   	// #0
 402cde8:	a90773fb 	stp	x27, x28, [sp, #112]
 402cdec:	f90043fe 	str	x30, [sp, #128]
 402cdf0:	f9477001 	ldr	x1, [x0, #3808]
 402cdf4:	f90017e1 	str	x1, [sp, #40]
 402cdf8:	d2800001 	mov	x1, #0x0                   	// #0
 402cdfc:	94000b07 	bl	402fa18 <ddrtrn_hal_dataeye_get_dm>
 402ce00:	b90102bf 	str	wzr, [x21, #256]
 402ce04:	b9000be0 	str	w0, [sp, #8]
 402ce08:	97fff755 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ce0c:	b940181c 	ldr	w28, [x0, #24]
 402ce10:	531d7380 	lsl	w0, w28, #3
 402ce14:	b9000fe0 	str	w0, [sp, #12]
 402ce18:	97fff751 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ce1c:	b9001c16 	str	w22, [x0, #28]
 402ce20:	b9400fe0 	ldr	w0, [sp, #12]
 402ce24:	0b0002db 	add	w27, w22, w0
 402ce28:	97fffb31 	bl	402baec <ddrtrn_hal_phy_get_dq_bdl>
 402ce2c:	2a0003f9 	mov	w25, w0
 402ce30:	0b0002f7 	add	w23, w23, w0
 402ce34:	97fff74a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ce38:	b9401814 	ldr	w20, [x0, #24]
 402ce3c:	97fff748 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ce40:	b9401c00 	ldr	w0, [x0, #28]
 402ce44:	531d7294 	lsl	w20, w20, #3
 402ce48:	0b000294 	add	w20, w20, w0
 402ce4c:	97fffb28 	bl	402baec <ddrtrn_hal_phy_get_dq_bdl>
 402ce50:	2a0003f8 	mov	w24, w0
 402ce54:	b9001fe0 	str	w0, [sp, #28]
 402ce58:	97ffff9e 	bl	402ccd0 <ddrtrn_dataeye_check_dq>
 402ce5c:	340006a0 	cbz	w0, 402cf30 <ddrtrn_dataeye_deskew+0x174>
 402ce60:	12800000 	mov	w0, #0xffffffff            	// #-1
 402ce64:	910073e2 	add	x2, sp, #0x1c
 402ce68:	52800023 	mov	w3, #0x1                   	// #1
 402ce6c:	52800fe1 	mov	w1, #0x7f                  	// #127
 402ce70:	b9001fe0 	str	w0, [sp, #28]
 402ce74:	52800000 	mov	w0, #0x0                   	// #0
 402ce78:	97ffff9f 	bl	402ccf4 <ddrtrn_dataeye_search_dq>
 402ce7c:	b9401fe0 	ldr	w0, [sp, #28]
 402ce80:	3100041f 	cmn	w0, #0x1
 402ce84:	54000561 	b.ne	402cf30 <ddrtrn_dataeye_deskew+0x174>  // b.any
 402ce88:	d37e7e94 	ubfiz	x20, x20, #2, #32
 402ce8c:	8b1402a0 	add	x0, x21, x20
 402ce90:	b8346abf 	str	wzr, [x21, x20]
 402ce94:	b900801f 	str	wzr, [x0, #128]
 402ce98:	2a1803e0 	mov	w0, w24
 402ce9c:	97fffb7d 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402cea0:	8b3b4abb 	add	x27, x21, w27, uxtw #2
 402cea4:	b9408374 	ldr	w20, [x27, #128]
 402cea8:	53107e98 	lsr	w24, w20, #16
 402ceac:	71001f1f 	cmp	w24, #0x7
 402ceb0:	54000b28 	b.hi	402d014 <ddrtrn_dataeye_deskew+0x258>  // b.pmore
 402ceb4:	350009da 	cbnz	w26, 402cfec <ddrtrn_dataeye_deskew+0x230>
 402ceb8:	510006d6 	sub	w22, w22, #0x1
 402cebc:	5280003a 	mov	w26, #0x1                   	// #1
 402cec0:	110006d6 	add	w22, w22, #0x1
 402cec4:	710022df 	cmp	w22, #0x8
 402cec8:	54fffa81 	b.ne	402ce18 <ddrtrn_dataeye_deskew+0x5c>  // b.any
 402cecc:	b9400be0 	ldr	w0, [sp, #8]
 402ced0:	53037ef7 	lsr	w23, w23, #3
 402ced4:	0b530c13 	add	w19, w0, w19, lsr #3
 402ced8:	6b17027f 	cmp	w19, w23
 402cedc:	54000ae3 	b.cc	402d038 <ddrtrn_dataeye_deskew+0x27c>  // b.lo, b.ul, b.last
 402cee0:	4b170273 	sub	w19, w19, w23
 402cee4:	52800fe0 	mov	w0, #0x7f                  	// #127
 402cee8:	7101fe7f 	cmp	w19, #0x7f
 402ceec:	1a809273 	csel	w19, w19, w0, ls  // ls = plast
 402cef0:	97fff71b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402cef4:	b9401400 	ldr	w0, [x0, #20]
 402cef8:	7100081f 	cmp	w0, #0x2
 402cefc:	54000061 	b.ne	402cf08 <ddrtrn_dataeye_deskew+0x14c>  // b.any
 402cf00:	2a1303e0 	mov	w0, w19
 402cf04:	94000ad6 	bl	402fa5c <ddrtrn_hal_dataeye_set_dq_sum>
 402cf08:	97fff715 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402cf0c:	b9400400 	ldr	w0, [x0, #4]
 402cf10:	97fffb49 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402cf14:	b0ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402cf18:	f94017e1 	ldr	x1, [sp, #40]
 402cf1c:	f9477002 	ldr	x2, [x0, #3808]
 402cf20:	eb020021 	subs	x1, x1, x2
 402cf24:	d2800002 	mov	x2, #0x0                   	// #0
 402cf28:	540008c0 	b.eq	402d040 <ddrtrn_dataeye_deskew+0x284>  // b.none
 402cf2c:	97ffe4b4 	bl	40261fc <__stack_chk_fail>
 402cf30:	b9401fe1 	ldr	w1, [sp, #28]
 402cf34:	910083e2 	add	x2, sp, #0x20
 402cf38:	52800043 	mov	w3, #0x2                   	// #2
 402cf3c:	52800000 	mov	w0, #0x0                   	// #0
 402cf40:	b90023e1 	str	w1, [sp, #32]
 402cf44:	97ffff6c 	bl	402ccf4 <ddrtrn_dataeye_search_dq>
 402cf48:	b94023e0 	ldr	w0, [sp, #32]
 402cf4c:	7100001f 	cmp	w0, #0x0
 402cf50:	5400012d 	b.le	402cf74 <ddrtrn_dataeye_deskew+0x1b8>
 402cf54:	51000400 	sub	w0, w0, #0x1
 402cf58:	b90023e0 	str	w0, [sp, #32]
 402cf5c:	97fffb4d 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402cf60:	97ffff5c 	bl	402ccd0 <ddrtrn_dataeye_check_dq>
 402cf64:	34ffff20 	cbz	w0, 402cf48 <ddrtrn_dataeye_deskew+0x18c>
 402cf68:	b94023e0 	ldr	w0, [sp, #32]
 402cf6c:	11000400 	add	w0, w0, #0x1
 402cf70:	b90023e0 	str	w0, [sp, #32]
 402cf74:	b9401fe0 	ldr	w0, [sp, #28]
 402cf78:	910093e2 	add	x2, sp, #0x24
 402cf7c:	52800083 	mov	w3, #0x4                   	// #4
 402cf80:	52800fe1 	mov	w1, #0x7f                  	// #127
 402cf84:	b90027e0 	str	w0, [sp, #36]
 402cf88:	97ffff5b 	bl	402ccf4 <ddrtrn_dataeye_search_dq>
 402cf8c:	b94027e0 	ldr	w0, [sp, #36]
 402cf90:	7101f81f 	cmp	w0, #0x7e
 402cf94:	5400012c 	b.gt	402cfb8 <ddrtrn_dataeye_deskew+0x1fc>
 402cf98:	11000400 	add	w0, w0, #0x1
 402cf9c:	b90027e0 	str	w0, [sp, #36]
 402cfa0:	97fffb3c 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402cfa4:	97ffff4b 	bl	402ccd0 <ddrtrn_dataeye_check_dq>
 402cfa8:	34ffff20 	cbz	w0, 402cf8c <ddrtrn_dataeye_deskew+0x1d0>
 402cfac:	b94027e0 	ldr	w0, [sp, #36]
 402cfb0:	51000400 	sub	w0, w0, #0x1
 402cfb4:	b90027e0 	str	w0, [sp, #36]
 402cfb8:	2a1803e0 	mov	w0, w24
 402cfbc:	97fffb35 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402cfc0:	29440be1 	ldp	w1, w2, [sp, #32]
 402cfc4:	d37e7e94 	ubfiz	x20, x20, #2, #32
 402cfc8:	8b1402a3 	add	x3, x21, x20
 402cfcc:	11000440 	add	w0, w2, #0x1
 402cfd0:	4b010000 	sub	w0, w0, w1
 402cfd4:	2a014042 	orr	w2, w2, w1, lsl #16
 402cfd8:	b8346aa2 	str	w2, [x21, x20]
 402cfdc:	0b400421 	add	w1, w1, w0, lsr #1
 402cfe0:	2a004020 	orr	w0, w1, w0, lsl #16
 402cfe4:	b9008060 	str	w0, [x3, #128]
 402cfe8:	17ffffae 	b	402cea0 <ddrtrn_dataeye_deskew+0xe4>
 402cfec:	35000158 	cbnz	w24, 402d014 <ddrtrn_dataeye_deskew+0x258>
 402cff0:	2a1903e0 	mov	w0, w25
 402cff4:	97fffb27 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402cff8:	97fff6d9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402cffc:	b9400401 	ldr	w1, [x0, #4]
 402d000:	2a1603e3 	mov	w3, w22
 402d004:	2a1c03e2 	mov	w2, w28
 402d008:	52800800 	mov	w0, #0x40                  	// #64
 402d00c:	97fffd87 	bl	402c628 <ddrtrn_hal_training_stat>
 402d010:	17ffffac 	b	402cec0 <ddrtrn_dataeye_deskew+0x104>
 402d014:	12003e94 	and	w20, w20, #0xffff
 402d018:	5280001a 	mov	w26, #0x0                   	// #0
 402d01c:	2a1403e0 	mov	w0, w20
 402d020:	97fffb1c 	bl	402bc90 <ddrtrn_hal_phy_set_dq_bdl>
 402d024:	b94102a0 	ldr	w0, [x21, #256]
 402d028:	0b140273 	add	w19, w19, w20
 402d02c:	0b180000 	add	w0, w0, w24
 402d030:	b90102a0 	str	w0, [x21, #256]
 402d034:	17ffffa3 	b	402cec0 <ddrtrn_dataeye_deskew+0x104>
 402d038:	52800013 	mov	w19, #0x0                   	// #0
 402d03c:	17ffffad 	b	402cef0 <ddrtrn_dataeye_deskew+0x134>
 402d040:	52800000 	mov	w0, #0x0                   	// #0
 402d044:	a94353f3 	ldp	x19, x20, [sp, #48]
 402d048:	a9445bf5 	ldp	x21, x22, [sp, #64]
 402d04c:	a94563f7 	ldp	x23, x24, [sp, #80]
 402d050:	a9466bf9 	ldp	x25, x26, [sp, #96]
 402d054:	a94773fb 	ldp	x27, x28, [sp, #112]
 402d058:	f94043fe 	ldr	x30, [sp, #128]
 402d05c:	910243ff 	add	sp, sp, #0x90
 402d060:	d65f03c0 	ret

000000000402d064 <ddrtrn_dataeye_process>:
 402d064:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402d068:	aa0003f4 	mov	x20, x0
 402d06c:	52800013 	mov	w19, #0x0                   	// #0
 402d070:	a9015bf5 	stp	x21, x22, [sp, #16]
 402d074:	52800015 	mov	w21, #0x0                   	// #0
 402d078:	a90263f7 	stp	x23, x24, [sp, #32]
 402d07c:	d2800197 	mov	x23, #0xc                   	// #12
 402d080:	d2800798 	mov	x24, #0x3c                  	// #60
 402d084:	f9001bfe 	str	x30, [sp, #48]
 402d088:	97fff6b5 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d08c:	aa0003f6 	mov	x22, x0
 402d090:	97fff6ae 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d094:	b94022c2 	ldr	w2, [x22, #32]
 402d098:	b9402ac1 	ldr	w1, [x22, #40]
 402d09c:	9b187c42 	mul	x2, x2, x24
 402d0a0:	9b170821 	madd	x1, x1, x23, x2
 402d0a4:	8b010000 	add	x0, x0, x1
 402d0a8:	b9401800 	ldr	w0, [x0, #24]
 402d0ac:	6b0002bf 	cmp	w21, w0
 402d0b0:	54000183 	b.cc	402d0e0 <ddrtrn_dataeye_process+0x7c>  // b.lo, b.ul, b.last
 402d0b4:	350002b3 	cbnz	w19, 402d108 <ddrtrn_dataeye_process+0xa4>
 402d0b8:	aa1403e0 	mov	x0, x20
 402d0bc:	94000e02 	bl	40308c4 <ddrtrn_adjust_dataeye>
 402d0c0:	aa1403e0 	mov	x0, x20
 402d0c4:	9400064d 	bl	402e9f8 <ddrtrn_result_data_save>
 402d0c8:	2a1303e0 	mov	w0, w19
 402d0cc:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402d0d0:	a94263f7 	ldp	x23, x24, [sp, #32]
 402d0d4:	f9401bfe 	ldr	x30, [sp, #48]
 402d0d8:	a8c453f3 	ldp	x19, x20, [sp], #64
 402d0dc:	d65f03c0 	ret
 402d0e0:	97fff69f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d0e4:	b9402816 	ldr	w22, [x0, #40]
 402d0e8:	97fff69d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d0ec:	0b1606b6 	add	w22, w21, w22, lsl #1
 402d0f0:	b9001816 	str	w22, [x0, #24]
 402d0f4:	110006b5 	add	w21, w21, #0x1
 402d0f8:	aa1403e0 	mov	x0, x20
 402d0fc:	97ffff30 	bl	402cdbc <ddrtrn_dataeye_deskew>
 402d100:	0b000273 	add	w19, w19, w0
 402d104:	17ffffe1 	b	402d088 <ddrtrn_dataeye_process+0x24>
 402d108:	12800013 	mov	w19, #0xffffffff            	// #-1
 402d10c:	17ffffed 	b	402d0c0 <ddrtrn_dataeye_process+0x5c>

000000000402d110 <ddrtrn_dataeye_training>:
 402d110:	d10483ff 	sub	sp, sp, #0x120
 402d114:	90ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402d118:	a9117bf3 	stp	x19, x30, [sp, #272]
 402d11c:	f9477001 	ldr	x1, [x0, #3808]
 402d120:	f90087e1 	str	x1, [sp, #264]
 402d124:	d2800001 	mov	x1, #0x0                   	// #0
 402d128:	97fff68d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d12c:	52800041 	mov	w1, #0x2                   	// #2
 402d130:	b9001401 	str	w1, [x0, #20]
 402d134:	52802082 	mov	w2, #0x104                 	// #260
 402d138:	52800001 	mov	w1, #0x0                   	// #0
 402d13c:	910003e0 	mov	x0, sp
 402d140:	9400047b 	bl	402e32c <ddrtrn_set_data>
 402d144:	910003e0 	mov	x0, sp
 402d148:	97ffffc7 	bl	402d064 <ddrtrn_dataeye_process>
 402d14c:	2a0003f3 	mov	w19, w0
 402d150:	97fff683 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d154:	52800021 	mov	w1, #0x1                   	// #1
 402d158:	b9001401 	str	w1, [x0, #20]
 402d15c:	52802082 	mov	w2, #0x104                 	// #260
 402d160:	52800001 	mov	w1, #0x0                   	// #0
 402d164:	910003e0 	mov	x0, sp
 402d168:	94000471 	bl	402e32c <ddrtrn_set_data>
 402d16c:	910003e0 	mov	x0, sp
 402d170:	97ffffbd 	bl	402d064 <ddrtrn_dataeye_process>
 402d174:	2a000273 	orr	w19, w19, w0
 402d178:	90ffffa1 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402d17c:	7100027f 	cmp	w19, #0x0
 402d180:	5a9f03e0 	csetm	w0, ne  // ne = any
 402d184:	f94087e2 	ldr	x2, [sp, #264]
 402d188:	f9477023 	ldr	x3, [x1, #3808]
 402d18c:	eb030042 	subs	x2, x2, x3
 402d190:	d2800003 	mov	x3, #0x0                   	// #0
 402d194:	54000040 	b.eq	402d19c <ddrtrn_dataeye_training+0x8c>  // b.none
 402d198:	97ffe419 	bl	40261fc <__stack_chk_fail>
 402d19c:	a9517bf3 	ldp	x19, x30, [sp, #272]
 402d1a0:	910483ff 	add	sp, sp, #0x120
 402d1a4:	d65f03c0 	ret

000000000402d1a8 <ddrtrn_dataeye_training_func>:
 402d1a8:	d10183ff 	sub	sp, sp, #0x60
 402d1ac:	90ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402d1b0:	a9057bf3 	stp	x19, x30, [sp, #80]
 402d1b4:	f9477001 	ldr	x1, [x0, #3808]
 402d1b8:	f90027e1 	str	x1, [sp, #72]
 402d1bc:	d2800001 	mov	x1, #0x0                   	// #0
 402d1c0:	52800020 	mov	w0, #0x1                   	// #1
 402d1c4:	94000a3c 	bl	402fab4 <ddrtrn_hal_set_dq_type>
 402d1c8:	52a00020 	mov	w0, #0x10000               	// #65536
 402d1cc:	97fffa8b 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 402d1d0:	35000280 	cbnz	w0, 402d220 <ddrtrn_dataeye_training_func+0x78>
 402d1d4:	910023e0 	add	x0, sp, #0x8
 402d1d8:	52a00021 	mov	w1, #0x10000               	// #65536
 402d1dc:	97fff8b4 	bl	402b4ac <ddrtrn_hal_save_reg>
 402d1e0:	97fff91c 	bl	402b650 <ddrtrn_hal_training_switch_axi>
 402d1e4:	52800040 	mov	w0, #0x2                   	// #2
 402d1e8:	940002ad 	bl	402dc9c <ddrtrn_ddrt_init>
 402d1ec:	52800020 	mov	w0, #0x1                   	// #1
 402d1f0:	97fffea2 	bl	402cc78 <ddrtrn_hal_set_adjust>
 402d1f4:	97ffffc7 	bl	402d110 <ddrtrn_dataeye_training>
 402d1f8:	2a0003f3 	mov	w19, w0
 402d1fc:	910023e0 	add	x0, sp, #0x8
 402d200:	97fff8ef 	bl	402b5bc <ddrtrn_hal_restore_reg>
 402d204:	90ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402d208:	f94027e1 	ldr	x1, [sp, #72]
 402d20c:	f9477002 	ldr	x2, [x0, #3808]
 402d210:	eb020021 	subs	x1, x1, x2
 402d214:	d2800002 	mov	x2, #0x0                   	// #0
 402d218:	54000080 	b.eq	402d228 <ddrtrn_dataeye_training_func+0x80>  // b.none
 402d21c:	97ffe3f8 	bl	40261fc <__stack_chk_fail>
 402d220:	52800013 	mov	w19, #0x0                   	// #0
 402d224:	17fffff8 	b	402d204 <ddrtrn_dataeye_training_func+0x5c>
 402d228:	2a1303e0 	mov	w0, w19
 402d22c:	a9457bf3 	ldp	x19, x30, [sp, #80]
 402d230:	910183ff 	add	sp, sp, #0x60
 402d234:	d65f03c0 	ret

000000000402d238 <ddrtrn_hal_get_cur_phy_dram_type>:
 402d238:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402d23c:	97fff648 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d240:	aa0003f3 	mov	x19, x0
 402d244:	97fff641 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d248:	d2800782 	mov	x2, #0x3c                  	// #60
 402d24c:	b9402261 	ldr	w1, [x19, #32]
 402d250:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402d254:	d503201f 	nop
 402d258:	9b020021 	madd	x1, x1, x2, x0
 402d25c:	b9400420 	ldr	w0, [x1, #4]
 402d260:	d65f03c0 	ret

000000000402d264 <ddrtrn_dcc_get_min_win>:
 402d264:	937a7c21 	sbfiz	x1, x1, #6, #32
 402d268:	8b010000 	add	x0, x0, x1
 402d26c:	29540001 	ldp	w1, w0, [x0, #160]
 402d270:	12002422 	and	w2, w1, #0x3ff
 402d274:	d3506421 	ubfx	x1, x1, #16, #10
 402d278:	4b010042 	sub	w2, w2, w1
 402d27c:	12002401 	and	w1, w0, #0x3ff
 402d280:	d3506400 	ubfx	x0, x0, #16, #10
 402d284:	4b000020 	sub	w0, w1, w0
 402d288:	6b00005f 	cmp	w2, w0
 402d28c:	1a809040 	csel	w0, w2, w0, ls  // ls = plast
 402d290:	d65f03c0 	ret

000000000402d294 <ddrtrn_sref_cfg>:
 402d294:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402d298:	52800013 	mov	w19, #0x0                   	// #0
 402d29c:	a9015bf5 	stp	x21, x22, [sp, #16]
 402d2a0:	aa0003f5 	mov	x21, x0
 402d2a4:	2a0103f6 	mov	w22, w1
 402d2a8:	a9027bf7 	stp	x23, x30, [sp, #32]
 402d2ac:	d2800797 	mov	x23, #0x3c                  	// #60
 402d2b0:	97fff62b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d2b4:	aa0003f4 	mov	x20, x0
 402d2b8:	97fff624 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d2bc:	b9402282 	ldr	w2, [x20, #32]
 402d2c0:	9b170042 	madd	x2, x2, x23, x0
 402d2c4:	b9400840 	ldr	w0, [x2, #8]
 402d2c8:	6b00027f 	cmp	w19, w0
 402d2cc:	540000a3 	b.cc	402d2e0 <ddrtrn_sref_cfg+0x4c>  // b.lo, b.ul, b.last
 402d2d0:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402d2d4:	a9427bf7 	ldp	x23, x30, [sp, #32]
 402d2d8:	a8c353f3 	ldp	x19, x20, [sp], #48
 402d2dc:	d65f03c0 	ret
 402d2e0:	2a1303e0 	mov	w0, w19
 402d2e4:	97fff921 	bl	402b768 <ddrtrn_hal_dmc_get_sref_cfg>
 402d2e8:	b8335aa0 	str	w0, [x21, w19, uxtw #2]
 402d2ec:	121c7400 	and	w0, w0, #0xfffffff3
 402d2f0:	2a160001 	orr	w1, w0, w22
 402d2f4:	2a1303e0 	mov	w0, w19
 402d2f8:	11000673 	add	w19, w19, #0x1
 402d2fc:	97fff921 	bl	402b780 <ddrtrn_hal_dmc_set_sref_cfg>
 402d300:	17ffffec 	b	402d2b0 <ddrtrn_sref_cfg+0x1c>

000000000402d304 <ddrtrn_dcc_training>:
 402d304:	d10743ff 	sub	sp, sp, #0x1d0
 402d308:	90ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402d30c:	a91753f3 	stp	x19, x20, [sp, #368]
 402d310:	52800a14 	mov	w20, #0x50                  	// #80
 402d314:	a9185bf5 	stp	x21, x22, [sp, #384]
 402d318:	a91963f7 	stp	x23, x24, [sp, #400]
 402d31c:	a91a6bf9 	stp	x25, x26, [sp, #416]
 402d320:	a91b73fb 	stp	x27, x28, [sp, #432]
 402d324:	f900e3fe 	str	x30, [sp, #448]
 402d328:	f9477001 	ldr	x1, [x0, #3808]
 402d32c:	f900b7e1 	str	x1, [sp, #360]
 402d330:	d2800001 	mov	x1, #0x0                   	// #0
 402d334:	97fff60a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d338:	aa0003f3 	mov	x19, x0
 402d33c:	97fff603 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d340:	d2800782 	mov	x2, #0x3c                  	// #60
 402d344:	b9402261 	ldr	w1, [x19, #32]
 402d348:	52800013 	mov	w19, #0x0                   	// #0
 402d34c:	9b020021 	madd	x1, x1, x2, x0
 402d350:	b9400c3b 	ldr	w27, [x1, #12]
 402d354:	6b1b027f 	cmp	w19, w27
 402d358:	540008a1 	b.ne	402d46c <ddrtrn_dcc_training+0x168>  // b.any
 402d35c:	9100c3e0 	add	x0, sp, #0x30
 402d360:	97fff8e3 	bl	402b6ec <ddrtrn_hal_save_timing>
 402d364:	52800195 	mov	w21, #0xc                   	// #12
 402d368:	97ffffb4 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d36c:	7100181f 	cmp	w0, #0x6
 402d370:	910463e1 	add	x1, sp, #0x118
 402d374:	12800000 	mov	w0, #0xffffffff            	// #-1
 402d378:	291cffe0 	stp	w0, wzr, [sp, #228]
 402d37c:	1a9f17f6 	cset	w22, eq  // eq = none
 402d380:	b90103ff 	str	wzr, [sp, #256]
 402d384:	110006d6 	add	w22, w22, #0x1
 402d388:	2901fc20 	stp	w0, wzr, [x1, #12]
 402d38c:	52800300 	mov	w0, #0x18                  	// #24
 402d390:	b9010fe0 	str	w0, [sp, #268]
 402d394:	52800160 	mov	w0, #0xb                   	// #11
 402d398:	b90113e0 	str	w0, [sp, #272]
 402d39c:	52800100 	mov	w0, #0x8                   	// #8
 402d3a0:	b90117e0 	str	w0, [sp, #276]
 402d3a4:	52800320 	mov	w0, #0x19                  	// #25
 402d3a8:	293dfc3f 	stp	wzr, wzr, [x1, #-20]
 402d3ac:	b9014fe0 	str	w0, [sp, #332]
 402d3b0:	528001e0 	mov	w0, #0xf                   	// #15
 402d3b4:	b90143ff 	str	wzr, [sp, #320]
 402d3b8:	b90147ff 	str	wzr, [sp, #324]
 402d3bc:	b9014bff 	str	wzr, [sp, #328]
 402d3c0:	b90153e0 	str	w0, [sp, #336]
 402d3c4:	b90157f5 	str	w21, [sp, #340]
 402d3c8:	940009d0 	bl	402fb08 <ddrtrn_hal_dcc_get_gated_bypass>
 402d3cc:	940009cf 	bl	402fb08 <ddrtrn_hal_dcc_get_gated_bypass>
 402d3d0:	2a0003f4 	mov	w20, w0
 402d3d4:	940009bc 	bl	402fac4 <ddrtrn_hal_dcc_get_ioctl21>
 402d3d8:	940009bb 	bl	402fac4 <ddrtrn_hal_dcc_get_ioctl21>
 402d3dc:	2a0003f3 	mov	w19, w0
 402d3e0:	97ffff96 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d3e4:	7100181f 	cmp	w0, #0x6
 402d3e8:	54000081 	b.ne	402d3f8 <ddrtrn_dcc_training+0xf4>  // b.any
 402d3ec:	2a1503e1 	mov	w1, w21
 402d3f0:	9100a3e0 	add	x0, sp, #0x28
 402d3f4:	97ffffa8 	bl	402d294 <ddrtrn_sref_cfg>
 402d3f8:	52800020 	mov	w0, #0x1                   	// #1
 402d3fc:	940001d2 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d400:	34000480 	cbz	w0, 402d490 <ddrtrn_dcc_training+0x18c>
 402d404:	12800015 	mov	w21, #0xffffffff            	// #-1
 402d408:	d2800013 	mov	x19, #0x0                   	// #0
 402d40c:	d2800796 	mov	x22, #0x3c                  	// #60
 402d410:	52a00058 	mov	w24, #0x20000               	// #131072
 402d414:	6b13037f 	cmp	w27, w19
 402d418:	54001f88 	b.hi	402d808 <ddrtrn_dcc_training+0x504>  // b.pmore
 402d41c:	940002f8 	bl	402dffc <ddrtrn_hw_training_by_phy>
 402d420:	0b150014 	add	w20, w0, w21
 402d424:	d2800013 	mov	x19, #0x0                   	// #0
 402d428:	d2800796 	mov	x22, #0x3c                  	// #60
 402d42c:	6b13037f 	cmp	w27, w19
 402d430:	540021a8 	b.hi	402d864 <ddrtrn_dcc_training+0x560>  // b.pmore
 402d434:	9100c3e0 	add	x0, sp, #0x30
 402d438:	940001db 	bl	402dba4 <ddrtrn_training_restore_timing>
 402d43c:	97ffff7f 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d440:	7100181f 	cmp	w0, #0x6
 402d444:	54000061 	b.ne	402d450 <ddrtrn_dcc_training+0x14c>  // b.any
 402d448:	9100a3e0 	add	x0, sp, #0x28
 402d44c:	940001af 	bl	402db08 <ddrtrn_sref_cfg_restore>
 402d450:	90ffffa0 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402d454:	f940b7e1 	ldr	x1, [sp, #360]
 402d458:	f9477002 	ldr	x2, [x0, #3808]
 402d45c:	eb020021 	subs	x1, x1, x2
 402d460:	d2800002 	mov	x2, #0x0                   	// #0
 402d464:	54002900 	b.eq	402d984 <ddrtrn_dcc_training+0x680>  // b.none
 402d468:	97ffe365 	bl	40261fc <__stack_chk_fail>
 402d46c:	97fff5bc 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d470:	b9002413 	str	w19, [x0, #36]
 402d474:	11000673 	add	w19, w19, #0x1
 402d478:	97fff5b9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d47c:	b9402400 	ldr	w0, [x0, #36]
 402d480:	9100e3e1 	add	x1, sp, #0x38
 402d484:	9bb40400 	umaddl	x0, w0, w20, x1
 402d488:	97fffd31 	bl	402c94c <ddrtrn_hal_save_rdqbdl>
 402d48c:	17ffffb2 	b	402d354 <ddrtrn_dcc_training+0x50>
 402d490:	97ffff6a 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d494:	2a1403e0 	mov	w0, w20
 402d498:	9400099d 	bl	402fb0c <ddrtrn_hal_dcc_set_gated_bypass>
 402d49c:	52800040 	mov	w0, #0x2                   	// #2
 402d4a0:	940001a9 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d4a4:	35fffb00 	cbnz	w0, 402d404 <ddrtrn_dcc_training+0x100>
 402d4a8:	12910000 	mov	w0, #0xffff77ff            	// #-34817
 402d4ac:	52800019 	mov	w25, #0x0                   	// #0
 402d4b0:	0a000260 	and	w0, w19, w0
 402d4b4:	b9001be0 	str	w0, [sp, #24]
 402d4b8:	12800000 	mov	w0, #0xffffffff            	// #-1
 402d4bc:	291dffe0 	stp	w0, wzr, [sp, #236]
 402d4c0:	71000adf 	cmp	w22, #0x2
 402d4c4:	54000061 	b.ne	402d4d0 <ddrtrn_dcc_training+0x1cc>  // b.any
 402d4c8:	b9012fe0 	str	w0, [sp, #300]
 402d4cc:	b90133ff 	str	wzr, [sp, #304]
 402d4d0:	52800020 	mov	w0, #0x1                   	// #1
 402d4d4:	9400019c 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d4d8:	35fff960 	cbnz	w0, 402d404 <ddrtrn_dcc_training+0x100>
 402d4dc:	97ffff57 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d4e0:	53155321 	lsl	w1, w25, #11
 402d4e4:	7100181f 	cmp	w0, #0x6
 402d4e8:	540008e1 	b.ne	402d604 <ddrtrn_dcc_training+0x300>  // b.any
 402d4ec:	b9401be0 	ldr	w0, [sp, #24]
 402d4f0:	2a193c35 	orr	w21, w1, w25, lsl #15
 402d4f4:	2a0002b5 	orr	w21, w21, w0
 402d4f8:	2a1503e0 	mov	w0, w21
 402d4fc:	9400097a 	bl	402fae4 <ddrtrn_hal_dcc_set_ioctl21>
 402d500:	52800040 	mov	w0, #0x2                   	// #2
 402d504:	b90163f5 	str	w21, [sp, #352]
 402d508:	9400018f 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d50c:	35fff7c0 	cbnz	w0, 402d404 <ddrtrn_dcc_training+0x100>
 402d510:	52800017 	mov	w23, #0x0                   	// #0
 402d514:	d280079c 	mov	x28, #0x3c                  	// #60
 402d518:	12800000 	mov	w0, #0xffffffff            	// #-1
 402d51c:	b900e3e0 	str	w0, [sp, #224]
 402d520:	71000adf 	cmp	w22, #0x2
 402d524:	54000041 	b.ne	402d52c <ddrtrn_dcc_training+0x228>  // b.any
 402d528:	b90123e0 	str	w0, [sp, #288]
 402d52c:	52800020 	mov	w0, #0x1                   	// #1
 402d530:	94000185 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d534:	35fff680 	cbnz	w0, 402d404 <ddrtrn_dcc_training+0x100>
 402d538:	b94163f8 	ldr	w24, [sp, #352]
 402d53c:	97ffff3f 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d540:	53185ee1 	lsl	w1, w23, #8
 402d544:	7100181f 	cmp	w0, #0x6
 402d548:	54000641 	b.ne	402d610 <ddrtrn_dcc_training+0x30c>  // b.any
 402d54c:	128ee015 	mov	w21, #0xffff88ff            	// #-30465
 402d550:	2a173021 	orr	w1, w1, w23, lsl #12
 402d554:	0a150315 	and	w21, w24, w21
 402d558:	2a0102b5 	orr	w21, w21, w1
 402d55c:	2a1503e0 	mov	w0, w21
 402d560:	94000961 	bl	402fae4 <ddrtrn_hal_dcc_set_ioctl21>
 402d564:	52800040 	mov	w0, #0x2                   	// #2
 402d568:	b90163f5 	str	w21, [sp, #352]
 402d56c:	94000176 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d570:	2a0003f5 	mov	w21, w0
 402d574:	35fff480 	cbnz	w0, 402d404 <ddrtrn_dcc_training+0x100>
 402d578:	97fff579 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d57c:	aa0003f8 	mov	x24, x0
 402d580:	97fff572 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d584:	5280001a 	mov	w26, #0x0                   	// #0
 402d588:	b9402301 	ldr	w1, [x24, #32]
 402d58c:	52800018 	mov	w24, #0x0                   	// #0
 402d590:	9b1c0021 	madd	x1, x1, x28, x0
 402d594:	b9400c20 	ldr	w0, [x1, #12]
 402d598:	b9000fe0 	str	w0, [sp, #12]
 402d59c:	b9400fe0 	ldr	w0, [sp, #12]
 402d5a0:	6b00035f 	cmp	w26, w0
 402d5a4:	540003a1 	b.ne	402d618 <ddrtrn_dcc_training+0x314>  // b.any
 402d5a8:	34000f38 	cbz	w24, 402d78c <ddrtrn_dcc_training+0x488>
 402d5ac:	9100e3f7 	add	x23, sp, #0x38
 402d5b0:	aa1703e0 	mov	x0, x23
 402d5b4:	b940ac02 	ldr	w2, [x0, #172]
 402d5b8:	b940b401 	ldr	w1, [x0, #180]
 402d5bc:	6b02003f 	cmp	w1, w2
 402d5c0:	54000042 	b.cs	402d5c8 <ddrtrn_dcc_training+0x2c4>  // b.hs, b.nlast
 402d5c4:	b900ac01 	str	w1, [x0, #172]
 402d5c8:	b940b002 	ldr	w2, [x0, #176]
 402d5cc:	b940b801 	ldr	w1, [x0, #184]
 402d5d0:	6b02003f 	cmp	w1, w2
 402d5d4:	540000a9 	b.ls	402d5e8 <ddrtrn_dcc_training+0x2e4>  // b.plast
 402d5d8:	b900b001 	str	w1, [x0, #176]
 402d5dc:	b940c801 	ldr	w1, [x0, #200]
 402d5e0:	b900cc01 	str	w1, [x0, #204]
 402d5e4:	b900d019 	str	w25, [x0, #208]
 402d5e8:	110006a1 	add	w1, w21, #0x1
 402d5ec:	91010000 	add	x0, x0, #0x40
 402d5f0:	6b0102df 	cmp	w22, w1
 402d5f4:	54000f21 	b.ne	402d7d8 <ddrtrn_dcc_training+0x4d4>  // b.any
 402d5f8:	35001579 	cbnz	w25, 402d8a4 <ddrtrn_dcc_training+0x5a0>
 402d5fc:	52800039 	mov	w25, #0x1                   	// #1
 402d600:	17ffffae 	b	402d4b8 <ddrtrn_dcc_training+0x1b4>
 402d604:	12147a75 	and	w21, w19, #0xfffff7ff
 402d608:	2a0102b5 	orr	w21, w21, w1
 402d60c:	17ffffbb 	b	402d4f8 <ddrtrn_dcc_training+0x1f4>
 402d610:	12157315 	and	w21, w24, #0xfffff8ff
 402d614:	17ffffd1 	b	402d558 <ddrtrn_dcc_training+0x254>
 402d618:	97fff551 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d61c:	b900241a 	str	w26, [x0, #36]
 402d620:	97fff54f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d624:	b9400402 	ldr	w2, [x0, #4]
 402d628:	b90007e2 	str	w2, [sp, #4]
 402d62c:	97fff54c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d630:	b9402401 	ldr	w1, [x0, #36]
 402d634:	b94007e2 	ldr	w2, [sp, #4]
 402d638:	2a0203e0 	mov	w0, w2
 402d63c:	97fffd4d 	bl	402cb70 <ddrtrn_hal_phy_switch_rank>
 402d640:	52802000 	mov	w0, #0x100                 	// #256
 402d644:	9400099b 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402d648:	b9000be0 	str	w0, [sp, #8]
 402d64c:	94000931 	bl	402fb10 <ddrtrn_hal_dcc_rdet_enable>
 402d650:	b9400be0 	ldr	w0, [sp, #8]
 402d654:	340001a0 	cbz	w0, 402d688 <ddrtrn_dcc_training+0x384>
 402d658:	b900e3ff 	str	wzr, [sp, #224]
 402d65c:	b90123ff 	str	wzr, [sp, #288]
 402d660:	97fff53f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d664:	b9402400 	ldr	w0, [x0, #36]
 402d668:	9100e3e2 	add	x2, sp, #0x38
 402d66c:	52800a01 	mov	w1, #0x50                  	// #80
 402d670:	1100075a 	add	w26, w26, #0x1
 402d674:	9ba10800 	umaddl	x0, w0, w1, x2
 402d678:	97fffcd8 	bl	402c9d8 <ddrtrn_hal_restore_rdqbdl>
 402d67c:	b9400be0 	ldr	w0, [sp, #8]
 402d680:	0b000318 	add	w24, w24, w0
 402d684:	17ffffc6 	b	402d59c <ddrtrn_dcc_training+0x298>
 402d688:	b940e3e0 	ldr	w0, [sp, #224]
 402d68c:	b90007e0 	str	w0, [sp, #4]
 402d690:	97fff533 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d694:	f9000be0 	str	x0, [sp, #16]
 402d698:	97fff52c 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d69c:	f9400be2 	ldr	x2, [sp, #16]
 402d6a0:	b9402041 	ldr	w1, [x2, #32]
 402d6a4:	9b1c0021 	madd	x1, x1, x28, x0
 402d6a8:	b9401022 	ldr	w2, [x1, #16]
 402d6ac:	d2800001 	mov	x1, #0x0                   	// #0
 402d6b0:	53017c42 	lsr	w2, w2, #1
 402d6b4:	6b01005f 	cmp	w2, w1
 402d6b8:	540003e8 	b.hi	402d734 <ddrtrn_dcc_training+0x430>  // b.pmore
 402d6bc:	52800001 	mov	w1, #0x0                   	// #0
 402d6c0:	9100e3e0 	add	x0, sp, #0x38
 402d6c4:	97fffee8 	bl	402d264 <ddrtrn_dcc_get_min_win>
 402d6c8:	b94007e1 	ldr	w1, [sp, #4]
 402d6cc:	6b01001f 	cmp	w0, w1
 402d6d0:	1a819000 	csel	w0, w0, w1, ls  // ls = plast
 402d6d4:	b900e3e0 	str	w0, [sp, #224]
 402d6d8:	97fffed8 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d6dc:	7100181f 	cmp	w0, #0x6
 402d6e0:	54fffc01 	b.ne	402d660 <ddrtrn_dcc_training+0x35c>  // b.any
 402d6e4:	b94123e0 	ldr	w0, [sp, #288]
 402d6e8:	b90007e0 	str	w0, [sp, #4]
 402d6ec:	97fff51c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d6f0:	f9000be0 	str	x0, [sp, #16]
 402d6f4:	97fff515 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d6f8:	f9400be2 	ldr	x2, [sp, #16]
 402d6fc:	b9402041 	ldr	w1, [x2, #32]
 402d700:	9b1c0021 	madd	x1, x1, x28, x0
 402d704:	b9401022 	ldr	w2, [x1, #16]
 402d708:	d2800041 	mov	x1, #0x2                   	// #2
 402d70c:	6b01005f 	cmp	w2, w1
 402d710:	54000288 	b.hi	402d760 <ddrtrn_dcc_training+0x45c>  // b.pmore
 402d714:	52800021 	mov	w1, #0x1                   	// #1
 402d718:	9100e3e0 	add	x0, sp, #0x38
 402d71c:	97fffed2 	bl	402d264 <ddrtrn_dcc_get_min_win>
 402d720:	b94007e1 	ldr	w1, [sp, #4]
 402d724:	6b01001f 	cmp	w0, w1
 402d728:	1a819000 	csel	w0, w0, w1, ls  // ls = plast
 402d72c:	b90123e0 	str	w0, [sp, #288]
 402d730:	17ffffcc 	b	402d660 <ddrtrn_dcc_training+0x35c>
 402d734:	2a0103e0 	mov	w0, w1
 402d738:	f9000be1 	str	x1, [sp, #16]
 402d73c:	b9001fe2 	str	w2, [sp, #28]
 402d740:	940008fc 	bl	402fb30 <ddrtrn_hal_dcc_get_dxnrdbound>
 402d744:	f9400be1 	ldr	x1, [sp, #16]
 402d748:	9100e3e2 	add	x2, sp, #0x38
 402d74c:	8b010843 	add	x3, x2, x1, lsl #2
 402d750:	b9401fe2 	ldr	w2, [sp, #28]
 402d754:	91000421 	add	x1, x1, #0x1
 402d758:	b900a060 	str	w0, [x3, #160]
 402d75c:	17ffffd6 	b	402d6b4 <ddrtrn_dcc_training+0x3b0>
 402d760:	2a0103e0 	mov	w0, w1
 402d764:	f9000be1 	str	x1, [sp, #16]
 402d768:	b9001fe2 	str	w2, [sp, #28]
 402d76c:	940008f1 	bl	402fb30 <ddrtrn_hal_dcc_get_dxnrdbound>
 402d770:	f9400be1 	ldr	x1, [sp, #16]
 402d774:	9100e3e2 	add	x2, sp, #0x38
 402d778:	8b010843 	add	x3, x2, x1, lsl #2
 402d77c:	b9401fe2 	ldr	w2, [sp, #28]
 402d780:	91000421 	add	x1, x1, #0x1
 402d784:	b900d860 	str	w0, [x3, #216]
 402d788:	17ffffe1 	b	402d70c <ddrtrn_dcc_training+0x408>
 402d78c:	9100e3e0 	add	x0, sp, #0x38
 402d790:	b940a801 	ldr	w1, [x0, #168]
 402d794:	b940b402 	ldr	w2, [x0, #180]
 402d798:	6b02003f 	cmp	w1, w2
 402d79c:	54000042 	b.cs	402d7a4 <ddrtrn_dcc_training+0x4a0>  // b.hs, b.nlast
 402d7a0:	b900b401 	str	w1, [x0, #180]
 402d7a4:	b940b802 	ldr	w2, [x0, #184]
 402d7a8:	6b02003f 	cmp	w1, w2
 402d7ac:	54000069 	b.ls	402d7b8 <ddrtrn_dcc_training+0x4b4>  // b.plast
 402d7b0:	b900b801 	str	w1, [x0, #184]
 402d7b4:	b900c817 	str	w23, [x0, #200]
 402d7b8:	11000718 	add	w24, w24, #0x1
 402d7bc:	91010000 	add	x0, x0, #0x40
 402d7c0:	6b1802df 	cmp	w22, w24
 402d7c4:	54fffe61 	b.ne	402d790 <ddrtrn_dcc_training+0x48c>  // b.any
 402d7c8:	110006f7 	add	w23, w23, #0x1
 402d7cc:	710012ff 	cmp	w23, #0x4
 402d7d0:	54ffea41 	b.ne	402d518 <ddrtrn_dcc_training+0x214>  // b.any
 402d7d4:	17ffff76 	b	402d5ac <ddrtrn_dcc_training+0x2a8>
 402d7d8:	2a0103f5 	mov	w21, w1
 402d7dc:	17ffff76 	b	402d5b4 <ddrtrn_dcc_training+0x2b0>
 402d7e0:	1ac222c0 	lsl	w0, w22, w2
 402d7e4:	1ac12303 	lsl	w3, w24, w1
 402d7e8:	2a030000 	orr	w0, w0, w3
 402d7ec:	0a200273 	bic	w19, w19, w0
 402d7f0:	b940d2e0 	ldr	w0, [x23, #208]
 402d7f4:	1ac22002 	lsl	w2, w0, w2
 402d7f8:	b940cee0 	ldr	w0, [x23, #204]
 402d7fc:	1ac12001 	lsl	w1, w0, w1
 402d800:	2a010040 	orr	w0, w2, w1
 402d804:	14000055 	b	402d958 <ddrtrn_dcc_training+0x654>
 402d808:	97fff4d5 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d80c:	b9002413 	str	w19, [x0, #36]
 402d810:	d37d7e74 	ubfiz	x20, x19, #3, #32
 402d814:	97fff4d2 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d818:	aa0003f7 	mov	x23, x0
 402d81c:	97fff4cb 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d820:	91008294 	add	x20, x20, #0x20
 402d824:	b94022e1 	ldr	w1, [x23, #32]
 402d828:	9b165021 	madd	x1, x1, x22, x20
 402d82c:	8b010000 	add	x0, x0, x1
 402d830:	b9401001 	ldr	w1, [x0, #16]
 402d834:	9100e3e0 	add	x0, sp, #0x38
 402d838:	8b130800 	add	x0, x0, x19, lsl #2
 402d83c:	91000673 	add	x19, x19, #0x1
 402d840:	b9012001 	str	w1, [x0, #288]
 402d844:	97fff4c6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d848:	aa0003f7 	mov	x23, x0
 402d84c:	97fff4bf 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d850:	b94022e1 	ldr	w1, [x23, #32]
 402d854:	9b165034 	madd	x20, x1, x22, x20
 402d858:	8b140000 	add	x0, x0, x20
 402d85c:	b9001018 	str	w24, [x0, #16]
 402d860:	17fffeed 	b	402d414 <ddrtrn_dcc_training+0x110>
 402d864:	97fff4be 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d868:	b9002413 	str	w19, [x0, #36]
 402d86c:	9100e3e0 	add	x0, sp, #0x38
 402d870:	8b130800 	add	x0, x0, x19, lsl #2
 402d874:	b9412017 	ldr	w23, [x0, #288]
 402d878:	97fff4b9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d87c:	aa0003f5 	mov	x21, x0
 402d880:	97fff4b2 	bl	402ab48 <ddrtrn_hal_get_phy>
 402d884:	d37d7e62 	ubfiz	x2, x19, #3, #32
 402d888:	91000673 	add	x19, x19, #0x1
 402d88c:	91008042 	add	x2, x2, #0x20
 402d890:	b94022a1 	ldr	w1, [x21, #32]
 402d894:	9b160821 	madd	x1, x1, x22, x2
 402d898:	8b010000 	add	x0, x0, x1
 402d89c:	b9001017 	str	w23, [x0, #16]
 402d8a0:	17fffee3 	b	402d42c <ddrtrn_dcc_training+0x128>
 402d8a4:	97fffe65 	bl	402d238 <ddrtrn_hal_get_cur_phy_dram_type>
 402d8a8:	7100181f 	cmp	w0, #0x6
 402d8ac:	54000081 	b.ne	402d8bc <ddrtrn_dcc_training+0x5b8>  // b.any
 402d8b0:	9100a3e0 	add	x0, sp, #0x28
 402d8b4:	52800181 	mov	w1, #0xc                   	// #12
 402d8b8:	97fffe77 	bl	402d294 <ddrtrn_sref_cfg>
 402d8bc:	52800020 	mov	w0, #0x1                   	// #1
 402d8c0:	940000a1 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d8c4:	35ffda00 	cbnz	w0, 402d404 <ddrtrn_dcc_training+0x100>
 402d8c8:	9101e3e0 	add	x0, sp, #0x78
 402d8cc:	52800801 	mov	w1, #0x40                  	// #64
 402d8d0:	52800036 	mov	w22, #0x1                   	// #1
 402d8d4:	528000f8 	mov	w24, #0x7                   	// #7
 402d8d8:	9ba102b5 	umaddl	x21, w21, w1, x0
 402d8dc:	295582e1 	ldp	w1, w0, [x23, #172]
 402d8e0:	b940dae2 	ldr	w2, [x23, #216]
 402d8e4:	4b010000 	sub	w0, w0, w1
 402d8e8:	b940dee1 	ldr	w1, [x23, #220]
 402d8ec:	7100081f 	cmp	w0, #0x2
 402d8f0:	54fff788 	b.hi	402d7e0 <ddrtrn_dcc_training+0x4dc>  // b.pmore
 402d8f4:	b940d6e4 	ldr	w4, [x23, #212]
 402d8f8:	1ac22662 	lsr	w2, w19, w2
 402d8fc:	1ac12661 	lsr	w1, w19, w1
 402d900:	12000042 	and	w2, w2, #0x1
 402d904:	12000821 	and	w1, w1, #0x7
 402d908:	b900c6e1 	str	w1, [x23, #196]
 402d90c:	1ac42683 	lsr	w3, w20, w4
 402d910:	12000063 	and	w3, w3, #0x1
 402d914:	1ac422c0 	lsl	w0, w22, w4
 402d918:	0a200294 	bic	w20, w20, w0
 402d91c:	29178ae3 	stp	w3, w2, [x23, #188]
 402d920:	1ac42063 	lsl	w3, w3, w4
 402d924:	2a140074 	orr	w20, w3, w20
 402d928:	2a1403e0 	mov	w0, w20
 402d92c:	94000878 	bl	402fb0c <ddrtrn_hal_dcc_set_gated_bypass>
 402d930:	295b0ae1 	ldp	w1, w2, [x23, #216]
 402d934:	1ac122c0 	lsl	w0, w22, w1
 402d938:	1ac22303 	lsl	w3, w24, w2
 402d93c:	2a030000 	orr	w0, w0, w3
 402d940:	0a200273 	bic	w19, w19, w0
 402d944:	b940c2e0 	ldr	w0, [x23, #192]
 402d948:	1ac12000 	lsl	w0, w0, w1
 402d94c:	b940c6e1 	ldr	w1, [x23, #196]
 402d950:	1ac22021 	lsl	w1, w1, w2
 402d954:	2a010000 	orr	w0, w0, w1
 402d958:	2a130013 	orr	w19, w0, w19
 402d95c:	910102f7 	add	x23, x23, #0x40
 402d960:	2a1303e0 	mov	w0, w19
 402d964:	94000860 	bl	402fae4 <ddrtrn_hal_dcc_set_ioctl21>
 402d968:	eb1502ff 	cmp	x23, x21
 402d96c:	54fffb81 	b.ne	402d8dc <ddrtrn_dcc_training+0x5d8>  // b.any
 402d970:	52800040 	mov	w0, #0x2                   	// #2
 402d974:	94000074 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402d978:	7100001f 	cmp	w0, #0x0
 402d97c:	5a9f03f5 	csetm	w21, ne  // ne = any
 402d980:	17fffea2 	b	402d408 <ddrtrn_dcc_training+0x104>
 402d984:	2a1403e0 	mov	w0, w20
 402d988:	a95753f3 	ldp	x19, x20, [sp, #368]
 402d98c:	a9585bf5 	ldp	x21, x22, [sp, #384]
 402d990:	a95963f7 	ldp	x23, x24, [sp, #400]
 402d994:	a95a6bf9 	ldp	x25, x26, [sp, #416]
 402d998:	a95b73fb 	ldp	x27, x28, [sp, #432]
 402d99c:	f940e3fe 	ldr	x30, [sp, #448]
 402d9a0:	910743ff 	add	sp, sp, #0x1d0
 402d9a4:	d65f03c0 	ret

000000000402d9a8 <ddrtrn_training_break_point>:
 402d9a8:	d65f03c0 	ret

000000000402d9ac <ddrtrn_dcc_training_func>:
 402d9ac:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402d9b0:	52800013 	mov	w19, #0x0                   	// #0
 402d9b4:	a9015bf5 	stp	x21, x22, [sp, #16]
 402d9b8:	52800015 	mov	w21, #0x0                   	// #0
 402d9bc:	d2800796 	mov	x22, #0x3c                  	// #60
 402d9c0:	a90263f7 	stp	x23, x24, [sp, #32]
 402d9c4:	52800037 	mov	w23, #0x1                   	// #1
 402d9c8:	f9001bfe 	str	x30, [sp, #48]
 402d9cc:	97fff464 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d9d0:	b9400000 	ldr	w0, [x0]
 402d9d4:	6b00027f 	cmp	w19, w0
 402d9d8:	540000e3 	b.cc	402d9f4 <ddrtrn_dcc_training_func+0x48>  // b.lo, b.ul, b.last
 402d9dc:	2a1503e0 	mov	w0, w21
 402d9e0:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402d9e4:	a94263f7 	ldp	x23, x24, [sp, #32]
 402d9e8:	f9401bfe 	ldr	x30, [sp, #48]
 402d9ec:	a8c453f3 	ldp	x19, x20, [sp], #64
 402d9f0:	d65f03c0 	ret
 402d9f4:	2a1303f4 	mov	w20, w19
 402d9f8:	97fff459 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402d9fc:	b9002013 	str	w19, [x0, #32]
 402da00:	9b167e94 	mul	x20, x20, x22
 402da04:	97fff451 	bl	402ab48 <ddrtrn_hal_get_phy>
 402da08:	b8746818 	ldr	w24, [x0, x20]
 402da0c:	97fff454 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402da10:	b9000418 	str	w24, [x0, #4]
 402da14:	97fff44d 	bl	402ab48 <ddrtrn_hal_get_phy>
 402da18:	8b140014 	add	x20, x0, x20
 402da1c:	b9402e94 	ldr	w20, [x20, #44]
 402da20:	97fff44f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402da24:	b9000c14 	str	w20, [x0, #12]
 402da28:	97fff44d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402da2c:	b9402000 	ldr	w0, [x0, #32]
 402da30:	1ac022e0 	lsl	w0, w23, w0
 402da34:	97fff871 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 402da38:	350000c0 	cbnz	w0, 402da50 <ddrtrn_dcc_training_func+0xa4>
 402da3c:	52a10000 	mov	w0, #0x8000000             	// #134217728
 402da40:	97fff86e 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 402da44:	35000060 	cbnz	w0, 402da50 <ddrtrn_dcc_training_func+0xa4>
 402da48:	97fffe2f 	bl	402d304 <ddrtrn_dcc_training>
 402da4c:	0b0002b5 	add	w21, w21, w0
 402da50:	11000673 	add	w19, w19, #0x1
 402da54:	17ffffde 	b	402d9cc <ddrtrn_dcc_training_func+0x20>

000000000402da58 <ddrtrn_hal_get_cur_dmc_addr>:
 402da58:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402da5c:	2a0003f3 	mov	w19, w0
 402da60:	f9000bfe 	str	x30, [sp, #16]
 402da64:	97fff43e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402da68:	aa0003f4 	mov	x20, x0
 402da6c:	97fff437 	bl	402ab48 <ddrtrn_hal_get_phy>
 402da70:	d2800782 	mov	x2, #0x3c                  	// #60
 402da74:	f9400bfe 	ldr	x30, [sp, #16]
 402da78:	b9402281 	ldr	w1, [x20, #32]
 402da7c:	9b027c21 	mul	x1, x1, x2
 402da80:	d2800182 	mov	x2, #0xc                   	// #12
 402da84:	9b020673 	madd	x19, x19, x2, x1
 402da88:	8b130013 	add	x19, x0, x19
 402da8c:	b9401660 	ldr	w0, [x19, #20]
 402da90:	a8c253f3 	ldp	x19, x20, [sp], #32
 402da94:	d65f03c0 	ret

000000000402da98 <ddrtrn_hal_get_cur_phy_dmc_num>:
 402da98:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402da9c:	97fff430 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402daa0:	aa0003f3 	mov	x19, x0
 402daa4:	97fff429 	bl	402ab48 <ddrtrn_hal_get_phy>
 402daa8:	d2800782 	mov	x2, #0x3c                  	// #60
 402daac:	b9402261 	ldr	w1, [x19, #32]
 402dab0:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402dab4:	d503201f 	nop
 402dab8:	9b020021 	madd	x1, x1, x2, x0
 402dabc:	b9400820 	ldr	w0, [x1, #8]
 402dac0:	d65f03c0 	ret

000000000402dac4 <ddrtrn_sref_cfg_save>:
 402dac4:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402dac8:	aa0003f4 	mov	x20, x0
 402dacc:	52800013 	mov	w19, #0x0                   	// #0
 402dad0:	f9000bfe 	str	x30, [sp, #16]
 402dad4:	97fffff1 	bl	402da98 <ddrtrn_hal_get_cur_phy_dmc_num>
 402dad8:	6b13001f 	cmp	w0, w19
 402dadc:	54000088 	b.hi	402daec <ddrtrn_sref_cfg_save+0x28>  // b.pmore
 402dae0:	f9400bfe 	ldr	x30, [sp, #16]
 402dae4:	a8c253f3 	ldp	x19, x20, [sp], #32
 402dae8:	d65f03c0 	ret
 402daec:	2a1303e0 	mov	w0, w19
 402daf0:	97ffffda 	bl	402da58 <ddrtrn_hal_get_cur_dmc_addr>
 402daf4:	11008000 	add	w0, w0, #0x20
 402daf8:	b9400000 	ldr	w0, [x0]
 402dafc:	b8335a80 	str	w0, [x20, w19, uxtw #2]
 402db00:	11000673 	add	w19, w19, #0x1
 402db04:	17fffff4 	b	402dad4 <ddrtrn_sref_cfg_save+0x10>

000000000402db08 <ddrtrn_sref_cfg_restore>:
 402db08:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402db0c:	aa0003f4 	mov	x20, x0
 402db10:	52800013 	mov	w19, #0x0                   	// #0
 402db14:	f9000bfe 	str	x30, [sp, #16]
 402db18:	97ffffe0 	bl	402da98 <ddrtrn_hal_get_cur_phy_dmc_num>
 402db1c:	6b13001f 	cmp	w0, w19
 402db20:	54000088 	b.hi	402db30 <ddrtrn_sref_cfg_restore+0x28>  // b.pmore
 402db24:	f9400bfe 	ldr	x30, [sp, #16]
 402db28:	a8c253f3 	ldp	x19, x20, [sp], #32
 402db2c:	d65f03c0 	ret
 402db30:	b8735a81 	ldr	w1, [x20, w19, uxtw #2]
 402db34:	2a1303e0 	mov	w0, w19
 402db38:	11000673 	add	w19, w19, #0x1
 402db3c:	97fff711 	bl	402b780 <ddrtrn_hal_dmc_set_sref_cfg>
 402db40:	17fffff6 	b	402db18 <ddrtrn_sref_cfg_restore+0x10>

000000000402db44 <ddrtrn_training_ctrl_easr>:
 402db44:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402db48:	a9017bf5 	stp	x21, x30, [sp, #16]
 402db4c:	2a0003f5 	mov	w21, w0
 402db50:	97ffffd2 	bl	402da98 <ddrtrn_hal_get_cur_phy_dmc_num>
 402db54:	7100081f 	cmp	w0, #0x2
 402db58:	54000228 	b.hi	402db9c <ddrtrn_training_ctrl_easr+0x58>  // b.pmore
 402db5c:	52800014 	mov	w20, #0x0                   	// #0
 402db60:	52800013 	mov	w19, #0x0                   	// #0
 402db64:	97ffffcd 	bl	402da98 <ddrtrn_hal_get_cur_phy_dmc_num>
 402db68:	6b14001f 	cmp	w0, w20
 402db6c:	540000a8 	b.hi	402db80 <ddrtrn_training_ctrl_easr+0x3c>  // b.pmore
 402db70:	2a1303e0 	mov	w0, w19
 402db74:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402db78:	a8c253f3 	ldp	x19, x20, [sp], #32
 402db7c:	d65f03c0 	ret
 402db80:	2a1403e0 	mov	w0, w20
 402db84:	97ffffb5 	bl	402da58 <ddrtrn_hal_get_cur_dmc_addr>
 402db88:	2a1503e1 	mov	w1, w21
 402db8c:	11000694 	add	w20, w20, #0x1
 402db90:	97fff6b5 	bl	402b664 <ddrtrn_hal_ddrc_easr>
 402db94:	0b000273 	add	w19, w19, w0
 402db98:	17fffff3 	b	402db64 <ddrtrn_training_ctrl_easr+0x20>
 402db9c:	12800013 	mov	w19, #0xffffffff            	// #-1
 402dba0:	17fffff4 	b	402db70 <ddrtrn_training_ctrl_easr+0x2c>

000000000402dba4 <ddrtrn_training_restore_timing>:
 402dba4:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402dba8:	aa0003f4 	mov	x20, x0
 402dbac:	52800013 	mov	w19, #0x0                   	// #0
 402dbb0:	f9000bfe 	str	x30, [sp, #16]
 402dbb4:	97ffffb9 	bl	402da98 <ddrtrn_hal_get_cur_phy_dmc_num>
 402dbb8:	6b13001f 	cmp	w0, w19
 402dbbc:	54000088 	b.hi	402dbcc <ddrtrn_training_restore_timing+0x28>  // b.pmore
 402dbc0:	f9400bfe 	ldr	x30, [sp, #16]
 402dbc4:	a8c253f3 	ldp	x19, x20, [sp], #32
 402dbc8:	d65f03c0 	ret
 402dbcc:	2a1303e0 	mov	w0, w19
 402dbd0:	97ffffa2 	bl	402da58 <ddrtrn_hal_get_cur_dmc_addr>
 402dbd4:	b8735a81 	ldr	w1, [x20, w19, uxtw #2]
 402dbd8:	11000673 	add	w19, w19, #0x1
 402dbdc:	97fff61d 	bl	402b450 <ddrtrn_hal_set_timing>
 402dbe0:	17fffff5 	b	402dbb4 <ddrtrn_training_restore_timing+0x10>

000000000402dbe4 <ddrtrn_hal_timing8_trfc_ab_cfg>:
 402dbe4:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402dbe8:	aa0003f3 	mov	x19, x0
 402dbec:	a9015bf5 	stp	x21, x22, [sp, #16]
 402dbf0:	52800015 	mov	w21, #0x0                   	// #0
 402dbf4:	a90263f7 	stp	x23, x24, [sp, #32]
 402dbf8:	f9001bfe 	str	x30, [sp, #48]
 402dbfc:	97fff3d8 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dc00:	b9400018 	ldr	w24, [x0]
 402dc04:	6b1802bf 	cmp	w21, w24
 402dc08:	540000c1 	b.ne	402dc20 <ddrtrn_hal_timing8_trfc_ab_cfg+0x3c>  // b.any
 402dc0c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402dc10:	a94263f7 	ldp	x23, x24, [sp, #32]
 402dc14:	f9401bfe 	ldr	x30, [sp, #48]
 402dc18:	a8c453f3 	ldp	x19, x20, [sp], #64
 402dc1c:	d65f03c0 	ret
 402dc20:	97fff3cf 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dc24:	b9002015 	str	w21, [x0, #32]
 402dc28:	52800014 	mov	w20, #0x0                   	// #0
 402dc2c:	97ffff9b 	bl	402da98 <ddrtrn_hal_get_cur_phy_dmc_num>
 402dc30:	2a0003f6 	mov	w22, w0
 402dc34:	6b1402df 	cmp	w22, w20
 402dc38:	540001c0 	b.eq	402dc70 <ddrtrn_hal_timing8_trfc_ab_cfg+0x8c>  // b.none
 402dc3c:	97fff3c8 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dc40:	b9002814 	str	w20, [x0, #40]
 402dc44:	2a1403e0 	mov	w0, w20
 402dc48:	97ffff84 	bl	402da58 <ddrtrn_hal_get_cur_dmc_addr>
 402dc4c:	2a0003f7 	mov	w23, w0
 402dc50:	97fff3c3 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dc54:	b9000817 	str	w23, [x0, #8]
 402dc58:	97fff774 	bl	402ba28 <ddrtrn_hal_ddrt_get_mem_width>
 402dc5c:	7100081f 	cmp	w0, #0x2
 402dc60:	540001a1 	b.ne	402dc94 <ddrtrn_hal_timing8_trfc_ab_cfg+0xb0>  // b.any
 402dc64:	b9400260 	ldr	w0, [x19]
 402dc68:	711ffc1f 	cmp	w0, #0x7ff
 402dc6c:	54000089 	b.ls	402dc7c <ddrtrn_hal_timing8_trfc_ab_cfg+0x98>  // b.plast
 402dc70:	110006b5 	add	w21, w21, #0x1
 402dc74:	91003273 	add	x19, x19, #0xc
 402dc78:	17ffffe3 	b	402dc04 <ddrtrn_hal_timing8_trfc_ab_cfg+0x20>
 402dc7c:	b9400660 	ldr	w0, [x19, #4]
 402dc80:	711ffc1f 	cmp	w0, #0x7ff
 402dc84:	54ffff68 	b.hi	402dc70 <ddrtrn_hal_timing8_trfc_ab_cfg+0x8c>  // b.pmore
 402dc88:	11000694 	add	w20, w20, #0x1
 402dc8c:	97fff6e5 	bl	402b820 <ddrtrn_hal_timing8_trfc_ab_cfg_by_dmc>
 402dc90:	17ffffe9 	b	402dc34 <ddrtrn_hal_timing8_trfc_ab_cfg+0x50>
 402dc94:	b9400a60 	ldr	w0, [x19, #8]
 402dc98:	17fffffa 	b	402dc80 <ddrtrn_hal_timing8_trfc_ab_cfg+0x9c>

000000000402dc9c <ddrtrn_ddrt_init>:
 402dc9c:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402dca0:	2a0003f4 	mov	w20, w0
 402dca4:	f9000bfe 	str	x30, [sp, #16]
 402dca8:	97fff3ad 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dcac:	b9402400 	ldr	w0, [x0, #36]
 402dcb0:	7100041f 	cmp	w0, #0x1
 402dcb4:	54000621 	b.ne	402dd78 <ddrtrn_ddrt_init+0xdc>  // b.any
 402dcb8:	97fff6c1 	bl	402b7bc <ddrtrn_hal_get_rank_size>
 402dcbc:	2a0003f3 	mov	w19, w0
 402dcc0:	97fff75a 	bl	402ba28 <ddrtrn_hal_ddrt_get_mem_width>
 402dcc4:	51000400 	sub	w0, w0, #0x1
 402dcc8:	52802a42 	mov	w2, #0x152                 	// #338
 402dccc:	52800181 	mov	w1, #0xc                   	// #12
 402dcd0:	2a003040 	orr	w0, w2, w0, lsl #12
 402dcd4:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dcd8:	97fff751 	bl	402ba1c <ddrt_reg_write>
 402dcdc:	52800381 	mov	w1, #0x1c                  	// #28
 402dce0:	52a80000 	mov	w0, #0x40000000            	// #1073741824
 402dce4:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dce8:	97fff74d 	bl	402ba1c <ddrt_reg_write>
 402dcec:	94000345 	bl	402ea00 <ddrtrn_ddrt_get_test_addr>
 402dcf0:	0b130000 	add	w0, w0, w19
 402dcf4:	52800401 	mov	w1, #0x20                  	// #32
 402dcf8:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dcfc:	97fff748 	bl	402ba1c <ddrt_reg_write>
 402dd00:	52800701 	mov	w1, #0x38                  	// #56
 402dd04:	528dada0 	mov	w0, #0x6d6d                	// #28013
 402dd08:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dd0c:	72adada0 	movk	w0, #0x6d6d, lsl #16
 402dd10:	97fff743 	bl	402ba1c <ddrt_reg_write>
 402dd14:	52800101 	mov	w1, #0x8                   	// #8
 402dd18:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dd1c:	7100069f 	cmp	w20, #0x1
 402dd20:	54000301 	b.ne	402dd80 <ddrtrn_ddrt_init+0xe4>  // b.any
 402dd24:	52800860 	mov	w0, #0x43                  	// #67
 402dd28:	97fff73d 	bl	402ba1c <ddrt_reg_write>
 402dd2c:	52800201 	mov	w1, #0x10                  	// #16
 402dd30:	52800000 	mov	w0, #0x0                   	// #0
 402dd34:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dd38:	97fff739 	bl	402ba1c <ddrt_reg_write>
 402dd3c:	52800281 	mov	w1, #0x14                  	// #20
 402dd40:	52800000 	mov	w0, #0x0                   	// #0
 402dd44:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dd48:	97fff735 	bl	402ba1c <ddrt_reg_write>
 402dd4c:	52800301 	mov	w1, #0x18                  	// #24
 402dd50:	52800000 	mov	w0, #0x0                   	// #0
 402dd54:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dd58:	97fff731 	bl	402ba1c <ddrt_reg_write>
 402dd5c:	52800601 	mov	w1, #0x30                  	// #48
 402dd60:	528ab540 	mov	w0, #0x55aa                	// #21930
 402dd64:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dd68:	72aab540 	movk	w0, #0x55aa, lsl #16
 402dd6c:	f9400bfe 	ldr	x30, [sp, #16]
 402dd70:	a8c253f3 	ldp	x19, x20, [sp], #32
 402dd74:	17fff72a 	b	402ba1c <ddrt_reg_write>
 402dd78:	52800013 	mov	w19, #0x0                   	// #0
 402dd7c:	17ffffd1 	b	402dcc0 <ddrtrn_ddrt_init+0x24>
 402dd80:	528009e0 	mov	w0, #0x4f                  	// #79
 402dd84:	97fff726 	bl	402ba1c <ddrt_reg_write>
 402dd88:	97fff375 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dd8c:	b9402813 	ldr	w19, [x0, #40]
 402dd90:	97fff373 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dd94:	aa0003f4 	mov	x20, x0
 402dd98:	97fff36c 	bl	402ab48 <ddrtrn_hal_get_phy>
 402dd9c:	2a1303f3 	mov	w19, w19
 402dda0:	d2800782 	mov	x2, #0x3c                  	// #60
 402dda4:	b9402281 	ldr	w1, [x20, #32]
 402dda8:	9b027c21 	mul	x1, x1, x2
 402ddac:	d2800182 	mov	x2, #0xc                   	// #12
 402ddb0:	9b020673 	madd	x19, x19, x2, x1
 402ddb4:	52800601 	mov	w1, #0x30                  	// #48
 402ddb8:	72a222c1 	movk	w1, #0x1116, lsl #16
 402ddbc:	8b130013 	add	x19, x0, x19
 402ddc0:	b9401e60 	ldr	w0, [x19, #28]
 402ddc4:	97fff716 	bl	402ba1c <ddrt_reg_write>
 402ddc8:	52800201 	mov	w1, #0x10                  	// #16
 402ddcc:	52800fe0 	mov	w0, #0x7f                  	// #127
 402ddd0:	72a222c1 	movk	w1, #0x1116, lsl #16
 402ddd4:	97fff712 	bl	402ba1c <ddrt_reg_write>
 402ddd8:	52800281 	mov	w1, #0x14                  	// #20
 402dddc:	12800000 	mov	w0, #0xffffffff            	// #-1
 402dde0:	72a222c1 	movk	w1, #0x1116, lsl #16
 402dde4:	97fff70e 	bl	402ba1c <ddrt_reg_write>
 402dde8:	52800301 	mov	w1, #0x18                  	// #24
 402ddec:	52800000 	mov	w0, #0x0                   	// #0
 402ddf0:	72a222c1 	movk	w1, #0x1116, lsl #16
 402ddf4:	17ffffde 	b	402dd6c <ddrtrn_ddrt_init+0xd0>

000000000402ddf8 <ddrtrn_ddrt_test>:
 402ddf8:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402ddfc:	2a0203f3 	mov	w19, w2
 402de00:	2a0103f4 	mov	w20, w1
 402de04:	52a222c1 	mov	w1, #0x11160000            	// #286654464
 402de08:	a9015bf5 	stp	x21, x22, [sp, #16]
 402de0c:	2a0003f5 	mov	w21, w0
 402de10:	32000000 	orr	w0, w0, #0x1
 402de14:	a90263f7 	stp	x23, x24, [sp, #32]
 402de18:	f9001bfe 	str	x30, [sp, #48]
 402de1c:	97fff700 	bl	402ba1c <ddrt_reg_write>
 402de20:	52800081 	mov	w1, #0x4                   	// #4
 402de24:	52800000 	mov	w0, #0x0                   	// #0
 402de28:	72a222c1 	movk	w1, #0x1116, lsl #16
 402de2c:	97fff6fc 	bl	402ba1c <ddrt_reg_write>
 402de30:	d5033f9f 	dsb	sy
 402de34:	52800098 	mov	w24, #0x4                   	// #4
 402de38:	52884817 	mov	w23, #0x4240                	// #16960
 402de3c:	52800016 	mov	w22, #0x0                   	// #0
 402de40:	72a222d8 	movk	w24, #0x1116, lsl #16
 402de44:	72a001f7 	movk	w23, #0xf, lsl #16
 402de48:	2a1803e0 	mov	w0, w24
 402de4c:	110006d6 	add	w22, w22, #0x1
 402de50:	97fff6f0 	bl	402ba10 <ddrt_reg_read>
 402de54:	370001c0 	tbnz	w0, #0, 402de8c <ddrtrn_ddrt_test+0x94>
 402de58:	6b1702df 	cmp	w22, w23
 402de5c:	54ffff61 	b.ne	402de48 <ddrtrn_ddrt_test+0x50>  // b.any
 402de60:	12800003 	mov	w3, #0xffffffff            	// #-1
 402de64:	52800001 	mov	w1, #0x0                   	// #0
 402de68:	2a0303e2 	mov	w2, w3
 402de6c:	52800100 	mov	w0, #0x8                   	// #8
 402de70:	97fff9ee 	bl	402c628 <ddrtrn_hal_training_stat>
 402de74:	12800000 	mov	w0, #0xffffffff            	// #-1
 402de78:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402de7c:	a94263f7 	ldp	x23, x24, [sp, #32]
 402de80:	f9401bfe 	ldr	x30, [sp, #48]
 402de84:	a8c453f3 	ldp	x19, x20, [sp], #64
 402de88:	d65f03c0 	ret
 402de8c:	6b1702df 	cmp	w22, w23
 402de90:	54fffe80 	b.eq	402de60 <ddrtrn_ddrt_test+0x68>  // b.none
 402de94:	121806b5 	and	w21, w21, #0x300
 402de98:	710802bf 	cmp	w21, #0x200
 402de9c:	54000620 	b.eq	402df60 <ddrtrn_ddrt_test+0x168>  // b.none
 402dea0:	37080600 	tbnz	w0, #1, 402df60 <ddrtrn_ddrt_test+0x168>
 402dea4:	3100067f 	cmn	w19, #0x1
 402dea8:	54000320 	b.eq	402df0c <ddrtrn_ddrt_test+0x114>  // b.none
 402deac:	52801000 	mov	w0, #0x80                  	// #128
 402deb0:	0b140e75 	add	w21, w19, w20, lsl #3
 402deb4:	72a222c0 	movk	w0, #0x1116, lsl #16
 402deb8:	97fff6d6 	bl	402ba10 <ddrt_reg_read>
 402debc:	52800021 	mov	w1, #0x1                   	// #1
 402dec0:	1ad52021 	lsl	w1, w1, w21
 402dec4:	6a00003f 	tst	w1, w0
 402dec8:	54fffd61 	b.ne	402de74 <ddrtrn_ddrt_test+0x7c>  // b.any
 402decc:	531d7261 	lsl	w1, w19, #3
 402ded0:	7100127f 	cmp	w19, #0x4
 402ded4:	51001275 	sub	w21, w19, #0x4
 402ded8:	53027e73 	lsr	w19, w19, #2
 402dedc:	0b140673 	add	w19, w19, w20, lsl #1
 402dee0:	52800c00 	mov	w0, #0x60                  	// #96
 402dee4:	531d72b5 	lsl	w21, w21, #3
 402dee8:	72a222c0 	movk	w0, #0x1116, lsl #16
 402deec:	1a95b035 	csel	w21, w1, w21, lt  // lt = tstop
 402def0:	0b130800 	add	w0, w0, w19, lsl #2
 402def4:	97fff6c7 	bl	402ba10 <ddrt_reg_read>
 402def8:	52801fe1 	mov	w1, #0xff                  	// #255
 402defc:	1ad52021 	lsl	w1, w1, w21
 402df00:	6a00003f 	tst	w1, w0
 402df04:	5a9f03e0 	csetm	w0, ne  // ne = any
 402df08:	17ffffdc 	b	402de78 <ddrtrn_ddrt_test+0x80>
 402df0c:	3100069f 	cmn	w20, #0x1
 402df10:	54000280 	b.eq	402df60 <ddrtrn_ddrt_test+0x168>  // b.none
 402df14:	52801000 	mov	w0, #0x80                  	// #128
 402df18:	72a222c0 	movk	w0, #0x1116, lsl #16
 402df1c:	97fff6bd 	bl	402ba10 <ddrt_reg_read>
 402df20:	531d7282 	lsl	w2, w20, #3
 402df24:	52801fe1 	mov	w1, #0xff                  	// #255
 402df28:	1ac22021 	lsl	w1, w1, w2
 402df2c:	6a00003f 	tst	w1, w0
 402df30:	54fffa21 	b.ne	402de74 <ddrtrn_ddrt_test+0x7c>  // b.any
 402df34:	52800c13 	mov	w19, #0x60                  	// #96
 402df38:	72a222d3 	movk	w19, #0x1116, lsl #16
 402df3c:	0b130040 	add	w0, w2, w19
 402df40:	0b140e73 	add	w19, w19, w20, lsl #3
 402df44:	97fff6b3 	bl	402ba10 <ddrt_reg_read>
 402df48:	2a0003f5 	mov	w21, w0
 402df4c:	11001260 	add	w0, w19, #0x4
 402df50:	97fff6b0 	bl	402ba10 <ddrt_reg_read>
 402df54:	2b0002bf 	cmn	w21, w0
 402df58:	5a9f03e0 	csetm	w0, ne  // ne = any
 402df5c:	17ffffc7 	b	402de78 <ddrtrn_ddrt_test+0x80>
 402df60:	52800000 	mov	w0, #0x0                   	// #0
 402df64:	17ffffc5 	b	402de78 <ddrtrn_ddrt_test+0x80>

000000000402df68 <ddrtrn_gating_func>:
 402df68:	52800000 	mov	w0, #0x0                   	// #0
 402df6c:	d65f03c0 	ret

000000000402df70 <ddrtrn_hal_get_cur_phy_total_byte_num>:
 402df70:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402df74:	97fff2fa 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402df78:	aa0003f3 	mov	x19, x0
 402df7c:	97fff2f3 	bl	402ab48 <ddrtrn_hal_get_phy>
 402df80:	d2800782 	mov	x2, #0x3c                  	// #60
 402df84:	b9402261 	ldr	w1, [x19, #32]
 402df88:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402df8c:	d503201f 	nop
 402df90:	9b020021 	madd	x1, x1, x2, x0
 402df94:	b9401020 	ldr	w0, [x1, #16]
 402df98:	d65f03c0 	ret

000000000402df9c <ddrtrn_hw_dataeye_read>:
 402df9c:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402dfa0:	52800013 	mov	w19, #0x0                   	// #0
 402dfa4:	a9017bf5 	stp	x21, x30, [sp, #16]
 402dfa8:	97fff2ed 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402dfac:	b9400415 	ldr	w21, [x0, #4]
 402dfb0:	97fffff0 	bl	402df70 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402dfb4:	2a0003f4 	mov	w20, w0
 402dfb8:	97fff2f5 	bl	402ab8c <ddrtrn_hal_cfg_init>
 402dfbc:	6b14027f 	cmp	w19, w20
 402dfc0:	54000161 	b.ne	402dfec <ddrtrn_hw_dataeye_read+0x50>  // b.any
 402dfc4:	2a1503e0 	mov	w0, w21
 402dfc8:	97fff71b 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402dfcc:	52802000 	mov	w0, #0x100                 	// #256
 402dfd0:	94000738 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402dfd4:	2a0003f3 	mov	w19, w0
 402dfd8:	94000700 	bl	402fbd8 <ddrtrn_hal_hw_read_adj>
 402dfdc:	2a1303e0 	mov	w0, w19
 402dfe0:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402dfe4:	a8c253f3 	ldp	x19, x20, [sp], #32
 402dfe8:	d65f03c0 	ret
 402dfec:	2a1303e0 	mov	w0, w19
 402dff0:	11000673 	add	w19, w19, #0x1
 402dff4:	9400087b 	bl	40301e0 <ddrtrn_hal_hw_clear_rdq>
 402dff8:	17fffff1 	b	402dfbc <ddrtrn_hw_dataeye_read+0x20>

000000000402dffc <ddrtrn_hw_training_by_phy>:
 402dffc:	d10383ff 	sub	sp, sp, #0xe0
 402e000:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e004:	a90853f3 	stp	x19, x20, [sp, #128]
 402e008:	52800014 	mov	w20, #0x0                   	// #0
 402e00c:	a9095bf5 	stp	x21, x22, [sp, #144]
 402e010:	52800016 	mov	w22, #0x0                   	// #0
 402e014:	a90a63f7 	stp	x23, x24, [sp, #160]
 402e018:	d2800798 	mov	x24, #0x3c                  	// #60
 402e01c:	9100a3f7 	add	x23, sp, #0x28
 402e020:	a90b6bf9 	stp	x25, x26, [sp, #176]
 402e024:	a90c73fb 	stp	x27, x28, [sp, #192]
 402e028:	f9006bfe 	str	x30, [sp, #208]
 402e02c:	f9477001 	ldr	x1, [x0, #3808]
 402e030:	f9003fe1 	str	x1, [sp, #120]
 402e034:	d2800001 	mov	x1, #0x0                   	// #0
 402e038:	97fff2c9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e03c:	aa0003f3 	mov	x19, x0
 402e040:	97fff2c2 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e044:	b9402261 	ldr	w1, [x19, #32]
 402e048:	9b180021 	madd	x1, x1, x24, x0
 402e04c:	910043e0 	add	x0, sp, #0x10
 402e050:	b9400c39 	ldr	w25, [x1, #12]
 402e054:	97fff5a6 	bl	402b6ec <ddrtrn_hal_save_timing>
 402e058:	6b19029f 	cmp	w20, w25
 402e05c:	54000401 	b.ne	402e0dc <ddrtrn_hw_training_by_phy+0xe0>  // b.any
 402e060:	71000a9f 	cmp	w20, #0x2
 402e064:	540002a1 	b.ne	402e0b8 <ddrtrn_hw_training_by_phy+0xbc>  // b.any
 402e068:	97ffffc2 	bl	402df70 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402e06c:	2a0003f5 	mov	w21, w0
 402e070:	97fff2bb 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e074:	b9402417 	ldr	w23, [x0, #36]
 402e078:	910063f4 	add	x20, sp, #0x18
 402e07c:	52800013 	mov	w19, #0x0                   	// #0
 402e080:	91001294 	add	x20, x20, #0x4
 402e084:	6b1302bf 	cmp	w21, w19
 402e088:	54000ea1 	b.ne	402e25c <ddrtrn_hw_training_by_phy+0x260>  // b.any
 402e08c:	97fff2b4 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e090:	b9002417 	str	w23, [x0, #36]
 402e094:	97fff2b2 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e098:	b9400400 	ldr	w0, [x0, #4]
 402e09c:	97fff6e6 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402e0a0:	97fff7e1 	bl	402c024 <ddrtrn_hal_training_adjust_wdq>
 402e0a4:	97fff892 	bl	402c2ec <ddrtrn_hal_training_adjust_wdqs>
 402e0a8:	97fff2ad 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e0ac:	b9400400 	ldr	w0, [x0, #4]
 402e0b0:	52800001 	mov	w1, #0x0                   	// #0
 402e0b4:	97fffaaf 	bl	402cb70 <ddrtrn_hal_phy_switch_rank>
 402e0b8:	910043e0 	add	x0, sp, #0x10
 402e0bc:	97fffeba 	bl	402dba4 <ddrtrn_training_restore_timing>
 402e0c0:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e0c4:	f9403fe1 	ldr	x1, [sp, #120]
 402e0c8:	f9477002 	ldr	x2, [x0, #3808]
 402e0cc:	eb020021 	subs	x1, x1, x2
 402e0d0:	d2800002 	mov	x2, #0x0                   	// #0
 402e0d4:	54000d20 	b.eq	402e278 <ddrtrn_hw_training_by_phy+0x27c>  // b.none
 402e0d8:	97ffe049 	bl	40261fc <__stack_chk_fail>
 402e0dc:	97fff2a0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e0e0:	b9002414 	str	w20, [x0, #36]
 402e0e4:	97fff29e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e0e8:	aa0003f3 	mov	x19, x0
 402e0ec:	97fff297 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e0f0:	d37d7e81 	ubfiz	x1, x20, #3, #32
 402e0f4:	b9402262 	ldr	w2, [x19, #32]
 402e0f8:	91008021 	add	x1, x1, #0x20
 402e0fc:	9b180441 	madd	x1, x2, x24, x1
 402e100:	8b010000 	add	x0, x0, x1
 402e104:	b9401013 	ldr	w19, [x0, #16]
 402e108:	97fff295 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e10c:	b9000c13 	str	w19, [x0, #12]
 402e110:	97fff293 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e114:	b9400413 	ldr	w19, [x0, #4]
 402e118:	97fff291 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e11c:	b9402401 	ldr	w1, [x0, #36]
 402e120:	2a1303e0 	mov	w0, w19
 402e124:	97fffa93 	bl	402cb70 <ddrtrn_hal_phy_switch_rank>
 402e128:	97fff28d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e12c:	b9400c1a 	ldr	w26, [x0, #12]
 402e130:	97fff28b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e134:	b9400415 	ldr	w21, [x0, #4]
 402e138:	97ffff8e 	bl	402df70 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402e13c:	2a0003fc 	mov	w28, w0
 402e140:	340008ba 	cbz	w26, 402e254 <ddrtrn_hw_training_by_phy+0x258>
 402e144:	2a1503e0 	mov	w0, w21
 402e148:	97fff6bb 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402e14c:	12110340 	and	w0, w26, #0x8000
 402e150:	940006d8 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402e154:	2a0003f3 	mov	w19, w0
 402e158:	121f0b40 	and	w0, w26, #0xe
 402e15c:	940006d5 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402e160:	0b000273 	add	w19, w19, w0
 402e164:	3608007a 	tbz	w26, #1, 402e170 <ddrtrn_hw_training_by_phy+0x174>
 402e168:	2a1503e0 	mov	w0, w21
 402e16c:	97fff6c0 	bl	402bc6c <ddrtrn_hal_ck_cfg>
 402e170:	3618007a 	tbz	w26, #3, 402e17c <ddrtrn_hw_training_by_phy+0x180>
 402e174:	94000863 	bl	4030300 <ddrtrn_ac_oe_enable>
 402e178:	9400087f 	bl	4030374 <ddrtrn_dummy_io_oe_enable>
 402e17c:	97fff278 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e180:	b9400c00 	ldr	w0, [x0, #12]
 402e184:	36100140 	tbz	w0, #2, 402e1ac <ddrtrn_hw_training_by_phy+0x1b0>
 402e188:	97fff275 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e18c:	aa0003fb 	mov	x27, x0
 402e190:	97fff26e 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e194:	b9402361 	ldr	w1, [x27, #32]
 402e198:	9b180021 	madd	x1, x1, x24, x0
 402e19c:	2a1503e0 	mov	w0, w21
 402e1a0:	b9400422 	ldr	w2, [x1, #4]
 402e1a4:	2a1c03e1 	mov	w1, w28
 402e1a8:	9400068d 	bl	402fbdc <ddrtrn_hal_hw_rdqs_offset_cfg>
 402e1ac:	97fff26c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e1b0:	b9402400 	ldr	w0, [x0, #36]
 402e1b4:	35000080 	cbnz	w0, 402e1c4 <ddrtrn_hw_training_by_phy+0x1c8>
 402e1b8:	910063e0 	add	x0, sp, #0x18
 402e1bc:	94000697 	bl	402fc18 <ddrtrn_hal_training_get_rdqs>
 402e1c0:	94000840 	bl	40302c0 <ddrtrn_hal_hw_save_rdqsbdl>
 402e1c4:	5280001b 	mov	w27, #0x0                   	// #0
 402e1c8:	6b1b039f 	cmp	w28, w27
 402e1cc:	54000381 	b.ne	402e23c <ddrtrn_hw_training_by_phy+0x240>  // b.any
 402e1d0:	910033e0 	add	x0, sp, #0xc
 402e1d4:	940007a1 	bl	4030058 <ddrtrn_hal_hw_dataeye_adapt>
 402e1d8:	0b000273 	add	w19, w19, w0
 402e1dc:	12150340 	and	w0, w26, #0x800
 402e1e0:	940006b4 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402e1e4:	0b000273 	add	w19, w19, w0
 402e1e8:	120a0340 	and	w0, w26, #0x400000
 402e1ec:	940006b1 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402e1f0:	0b000273 	add	w19, w19, w0
 402e1f4:	940007da 	bl	403015c <ddrtrn_hal_hw_dataeye_vref_set>
 402e1f8:	0b000273 	add	w19, w19, w0
 402e1fc:	940006d8 	bl	402fd5c <ddrtrn_hal_hw_training_normal_conf>
 402e200:	0b000273 	add	w19, w19, w0
 402e204:	2a1503e0 	mov	w0, w21
 402e208:	97fff68b 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402e20c:	0b1302d6 	add	w22, w22, w19
 402e210:	71000b3f 	cmp	w25, #0x2
 402e214:	54fff521 	b.ne	402e0b8 <ddrtrn_hw_training_by_phy+0xbc>  // b.any
 402e218:	aa1703e0 	mov	x0, x23
 402e21c:	9400067f 	bl	402fc18 <ddrtrn_hal_training_get_rdqs>
 402e220:	7100069f 	cmp	w20, #0x1
 402e224:	54000060 	b.eq	402e230 <ddrtrn_hw_training_by_phy+0x234>  // b.none
 402e228:	910063e0 	add	x0, sp, #0x18
 402e22c:	9400068e 	bl	402fc64 <ddrtrn_hal_training_set_rdqs>
 402e230:	11000694 	add	w20, w20, #0x1
 402e234:	910042f7 	add	x23, x23, #0x10
 402e238:	17ffff88 	b	402e058 <ddrtrn_hw_training_by_phy+0x5c>
 402e23c:	97fff248 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e240:	b900181b 	str	w27, [x0, #24]
 402e244:	910123e0 	add	x0, sp, #0x48
 402e248:	1100077b 	add	w27, w27, #0x1
 402e24c:	97fff952 	bl	402c794 <ddrtrn_hal_rdqbdl_adj>
 402e250:	17ffffde 	b	402e1c8 <ddrtrn_hw_training_by_phy+0x1cc>
 402e254:	52800013 	mov	w19, #0x0                   	// #0
 402e258:	17ffffed 	b	402e20c <ddrtrn_hw_training_by_phy+0x210>
 402e25c:	b9401e81 	ldr	w1, [x20, #28]
 402e260:	2a1303e2 	mov	w2, w19
 402e264:	b9400e80 	ldr	w0, [x20, #12]
 402e268:	11000673 	add	w19, w19, #0x1
 402e26c:	940007f4 	bl	403023c <ddrtrn_hal_hw_restore_rdqsbdl>
 402e270:	97fff90e 	bl	402c6a8 <ddrtrn_hal_rdqs_sync_rank_rdq>
 402e274:	17ffff83 	b	402e080 <ddrtrn_hw_training_by_phy+0x84>
 402e278:	2a1603e0 	mov	w0, w22
 402e27c:	a94853f3 	ldp	x19, x20, [sp, #128]
 402e280:	a9495bf5 	ldp	x21, x22, [sp, #144]
 402e284:	a94a63f7 	ldp	x23, x24, [sp, #160]
 402e288:	a94b6bf9 	ldp	x25, x26, [sp, #176]
 402e28c:	a94c73fb 	ldp	x27, x28, [sp, #192]
 402e290:	f9406bfe 	ldr	x30, [sp, #208]
 402e294:	910383ff 	add	sp, sp, #0xe0
 402e298:	d65f03c0 	ret

000000000402e29c <ddrtrn_hw_training>:
 402e29c:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402e2a0:	52800013 	mov	w19, #0x0                   	// #0
 402e2a4:	52800014 	mov	w20, #0x0                   	// #0
 402e2a8:	a9015bf5 	stp	x21, x22, [sp, #16]
 402e2ac:	d2800795 	mov	x21, #0x3c                  	// #60
 402e2b0:	f90013fe 	str	x30, [sp, #32]
 402e2b4:	97fff22a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e2b8:	b9400000 	ldr	w0, [x0]
 402e2bc:	6b00027f 	cmp	w19, w0
 402e2c0:	540000c3 	b.cc	402e2d8 <ddrtrn_hw_training+0x3c>  // b.lo, b.ul, b.last
 402e2c4:	2a1403e0 	mov	w0, w20
 402e2c8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402e2cc:	f94013fe 	ldr	x30, [sp, #32]
 402e2d0:	a8c353f3 	ldp	x19, x20, [sp], #48
 402e2d4:	d65f03c0 	ret
 402e2d8:	97fff221 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e2dc:	b9002013 	str	w19, [x0, #32]
 402e2e0:	97fff21a 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e2e4:	2a1303e1 	mov	w1, w19
 402e2e8:	11000673 	add	w19, w19, #0x1
 402e2ec:	9b157c21 	mul	x1, x1, x21
 402e2f0:	b8616816 	ldr	w22, [x0, x1]
 402e2f4:	97fff21a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e2f8:	b9000416 	str	w22, [x0, #4]
 402e2fc:	97ffff40 	bl	402dffc <ddrtrn_hw_training_by_phy>
 402e300:	0b000294 	add	w20, w20, w0
 402e304:	17ffffec 	b	402e2b4 <ddrtrn_hw_training+0x18>

000000000402e308 <ddrtrn_copy_data>:
 402e308:	2a0203e2 	mov	w2, w2
 402e30c:	d2800003 	mov	x3, #0x0                   	// #0
 402e310:	eb02007f 	cmp	x3, x2
 402e314:	54000041 	b.ne	402e31c <ddrtrn_copy_data+0x14>  // b.any
 402e318:	d65f03c0 	ret
 402e31c:	38636824 	ldrb	w4, [x1, x3]
 402e320:	38236804 	strb	w4, [x0, x3]
 402e324:	91000463 	add	x3, x3, #0x1
 402e328:	17fffffa 	b	402e310 <ddrtrn_copy_data+0x8>

000000000402e32c <ddrtrn_set_data>:
 402e32c:	2a0203e2 	mov	w2, w2
 402e330:	d2800003 	mov	x3, #0x0                   	// #0
 402e334:	eb02007f 	cmp	x3, x2
 402e338:	54000041 	b.ne	402e340 <ddrtrn_set_data+0x14>  // b.any
 402e33c:	d65f03c0 	ret
 402e340:	38236801 	strb	w1, [x0, x3]
 402e344:	91000463 	add	x3, x3, #0x1
 402e348:	17fffffb 	b	402e334 <ddrtrn_set_data+0x8>

000000000402e34c <ddrtrn_rdq_offset_cfg_by_phy>:
 402e34c:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402e350:	52800013 	mov	w19, #0x0                   	// #0
 402e354:	a9015bf5 	stp	x21, x22, [sp, #16]
 402e358:	d2800796 	mov	x22, #0x3c                  	// #60
 402e35c:	a90263f7 	stp	x23, x24, [sp, #32]
 402e360:	f9001bfe 	str	x30, [sp, #48]
 402e364:	97fff1fe 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e368:	b9400000 	ldr	w0, [x0]
 402e36c:	6b00027f 	cmp	w19, w0
 402e370:	540000c3 	b.cc	402e388 <ddrtrn_rdq_offset_cfg_by_phy+0x3c>  // b.lo, b.ul, b.last
 402e374:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402e378:	a94263f7 	ldp	x23, x24, [sp, #32]
 402e37c:	f9401bfe 	ldr	x30, [sp, #48]
 402e380:	a8c453f3 	ldp	x19, x20, [sp], #64
 402e384:	d65f03c0 	ret
 402e388:	97fff1f5 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e38c:	b9002013 	str	w19, [x0, #32]
 402e390:	97fff1ee 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e394:	2a1303e1 	mov	w1, w19
 402e398:	9b167c21 	mul	x1, x1, x22
 402e39c:	b8616814 	ldr	w20, [x0, x1]
 402e3a0:	97fff1ef 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e3a4:	b9000414 	str	w20, [x0, #4]
 402e3a8:	97fff1ed 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e3ac:	aa0003f4 	mov	x20, x0
 402e3b0:	97fff1e6 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e3b4:	b9402281 	ldr	w1, [x20, #32]
 402e3b8:	52800014 	mov	w20, #0x0                   	// #0
 402e3bc:	9b160021 	madd	x1, x1, x22, x0
 402e3c0:	b9400c37 	ldr	w23, [x1, #12]
 402e3c4:	6b17029f 	cmp	w20, w23
 402e3c8:	54000061 	b.ne	402e3d4 <ddrtrn_rdq_offset_cfg_by_phy+0x88>  // b.any
 402e3cc:	11000673 	add	w19, w19, #0x1
 402e3d0:	17ffffe5 	b	402e364 <ddrtrn_rdq_offset_cfg_by_phy+0x18>
 402e3d4:	97fff1e2 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e3d8:	b9002414 	str	w20, [x0, #36]
 402e3dc:	97fff1e0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e3e0:	aa0003f5 	mov	x21, x0
 402e3e4:	97fff1d9 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e3e8:	b94022a1 	ldr	w1, [x21, #32]
 402e3ec:	52800015 	mov	w21, #0x0                   	// #0
 402e3f0:	9b160021 	madd	x1, x1, x22, x0
 402e3f4:	b9401038 	ldr	w24, [x1, #16]
 402e3f8:	6b1802bf 	cmp	w21, w24
 402e3fc:	54000061 	b.ne	402e408 <ddrtrn_rdq_offset_cfg_by_phy+0xbc>  // b.any
 402e400:	11000694 	add	w20, w20, #0x1
 402e404:	17fffff0 	b	402e3c4 <ddrtrn_rdq_offset_cfg_by_phy+0x78>
 402e408:	97fff1d5 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e40c:	b9001815 	str	w21, [x0, #24]
 402e410:	52800060 	mov	w0, #0x3                   	// #3
 402e414:	110006b5 	add	w21, w21, #0x1
 402e418:	97fff8a4 	bl	402c6a8 <ddrtrn_hal_rdqs_sync_rank_rdq>
 402e41c:	17fffff7 	b	402e3f8 <ddrtrn_rdq_offset_cfg_by_phy+0xac>

000000000402e420 <ddrtrn_retrain_enable>:
 402e420:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402e424:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402e428:	a9015bf5 	stp	x21, x22, [sp, #16]
 402e42c:	f90013fe 	str	x30, [sp, #32]
 402e430:	97fff9ab 	bl	402cadc <ddrtrn_hal_enable_rdqs_anti_aging>
 402e434:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402e438:	97fff98f 	bl	402ca74 <ddrtrn_hal_get_trfc_ctrl_val>
 402e43c:	34000460 	cbz	w0, 402e4c8 <ddrtrn_retrain_enable+0xa8>
 402e440:	2a0003f6 	mov	w22, w0
 402e444:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402e448:	97fff991 	bl	402ca8c <ddrtrn_hal_get_misc_val>
 402e44c:	2a0003f4 	mov	w20, w0
 402e450:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402e454:	97fff994 	bl	402caa4 <ddrtrn_hal_get_trfc_threshold1_val>
 402e458:	2a0003f3 	mov	w19, w0
 402e45c:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402e460:	97fff994 	bl	402cab0 <ddrtrn_hal_get_dmsel>
 402e464:	2a0003f5 	mov	w21, w0
 402e468:	52a222a0 	mov	w0, #0x11150000            	// #286588928
 402e46c:	97fff994 	bl	402cabc <ddrtrn_hal_get_phyctrl0>
 402e470:	d3524a81 	ubfx	x1, x20, #18, #1
 402e474:	d3410a73 	ubfx	x19, x19, #1, #2
 402e478:	11001024 	add	w4, w1, #0x4
 402e47c:	d34822a1 	ubfx	x1, x21, #8, #1
 402e480:	12000400 	and	w0, w0, #0x3
 402e484:	d3483e82 	ubfx	x2, x20, #8, #8
 402e488:	12001e83 	and	w3, w20, #0xff
 402e48c:	12103e94 	and	w20, w20, #0xffff0000
 402e490:	1b019261 	msub	w1, w19, w1, w4
 402e494:	1b007c21 	mul	w1, w1, w0
 402e498:	4b010463 	sub	w3, w3, w1, lsl #1
 402e49c:	4b010442 	sub	w2, w2, w1, lsl #1
 402e4a0:	2a140074 	orr	w20, w3, w20
 402e4a4:	52a222a1 	mov	w1, #0x11150000            	// #286588928
 402e4a8:	2a022280 	orr	w0, w20, w2, lsl #8
 402e4ac:	97fff97b 	bl	402ca98 <ddrtrn_hal_set_misc_val>
 402e4b0:	320002c0 	orr	w0, w22, #0x1
 402e4b4:	52a222a1 	mov	w1, #0x11150000            	// #286588928
 402e4b8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402e4bc:	f94013fe 	ldr	x30, [sp, #32]
 402e4c0:	a8c353f3 	ldp	x19, x20, [sp], #48
 402e4c4:	17fff96f 	b	402ca80 <ddrtrn_hal_set_trfc_ctrl>
 402e4c8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402e4cc:	f94013fe 	ldr	x30, [sp, #32]
 402e4d0:	a8c353f3 	ldp	x19, x20, [sp], #48
 402e4d4:	d65f03c0 	ret

000000000402e4d8 <ddrtrn_hal_get_byte_num>:
 402e4d8:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402e4dc:	97fff1a0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e4e0:	aa0003f3 	mov	x19, x0
 402e4e4:	97fff199 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e4e8:	d2800783 	mov	x3, #0x3c                  	// #60
 402e4ec:	b9402262 	ldr	w2, [x19, #32]
 402e4f0:	b9402a61 	ldr	w1, [x19, #40]
 402e4f4:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402e4f8:	9b037c42 	mul	x2, x2, x3
 402e4fc:	d2800183 	mov	x3, #0xc                   	// #12
 402e500:	9b030821 	madd	x1, x1, x3, x2
 402e504:	8b010000 	add	x0, x0, x1
 402e508:	b9401800 	ldr	w0, [x0, #24]
 402e50c:	d65f03c0 	ret

000000000402e510 <ddrtrn_vref_get_win>:
 402e510:	d100c3ff 	sub	sp, sp, #0x30
 402e514:	a90153f3 	stp	x19, x20, [sp, #16]
 402e518:	aa0003f4 	mov	x20, x0
 402e51c:	2a0103f3 	mov	w19, w1
 402e520:	f90013fe 	str	x30, [sp, #32]
 402e524:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e528:	f9477001 	ldr	x1, [x0, #3808]
 402e52c:	f90007e1 	str	x1, [sp, #8]
 402e530:	d2800001 	mov	x1, #0x0                   	// #0
 402e534:	52800640 	mov	w0, #0x32                  	// #50
 402e538:	b901029f 	str	wzr, [x20, #256]
 402e53c:	b90007e0 	str	w0, [sp, #4]
 402e540:	97fff187 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e544:	b9401400 	ldr	w0, [x0, #20]
 402e548:	7100041f 	cmp	w0, #0x1
 402e54c:	540000a1 	b.ne	402e560 <ddrtrn_vref_get_win+0x50>  // b.any
 402e550:	97fff183 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e554:	b9402400 	ldr	w0, [x0, #36]
 402e558:	910013e1 	add	x1, sp, #0x4
 402e55c:	97fff965 	bl	402caf0 <ddrtrn_hal_vref_get_host_max>
 402e560:	37f801f3 	tbnz	w19, #31, 402e59c <ddrtrn_vref_get_win+0x8c>
 402e564:	b94007e0 	ldr	w0, [sp, #4]
 402e568:	6b13001f 	cmp	w0, w19
 402e56c:	1a80a260 	csel	w0, w19, w0, ge  // ge = tcont
 402e570:	94000821 	bl	40305f4 <ddrtrn_hal_vref_status_set>
 402e574:	aa1403e0 	mov	x0, x20
 402e578:	97fffa11 	bl	402cdbc <ddrtrn_dataeye_deskew>
 402e57c:	f0ffff81 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402e580:	b9410280 	ldr	w0, [x20, #256]
 402e584:	f94007e2 	ldr	x2, [sp, #8]
 402e588:	f9477023 	ldr	x3, [x1, #3808]
 402e58c:	eb030042 	subs	x2, x2, x3
 402e590:	d2800003 	mov	x3, #0x0                   	// #0
 402e594:	54000080 	b.eq	402e5a4 <ddrtrn_vref_get_win+0x94>  // b.none
 402e598:	97ffdf19 	bl	40261fc <__stack_chk_fail>
 402e59c:	52800000 	mov	w0, #0x0                   	// #0
 402e5a0:	17fffff4 	b	402e570 <ddrtrn_vref_get_win+0x60>
 402e5a4:	a94153f3 	ldp	x19, x20, [sp, #16]
 402e5a8:	f94013fe 	ldr	x30, [sp, #32]
 402e5ac:	9100c3ff 	add	sp, sp, #0x30
 402e5b0:	d65f03c0 	ret

000000000402e5b4 <ddrtrn_vref_find_best>:
 402e5b4:	d10143ff 	sub	sp, sp, #0x50
 402e5b8:	a90153f3 	stp	x19, x20, [sp, #16]
 402e5bc:	2a0103f3 	mov	w19, w1
 402e5c0:	a9025bf5 	stp	x21, x22, [sp, #32]
 402e5c4:	2a0203f6 	mov	w22, w2
 402e5c8:	a90363f7 	stp	x23, x24, [sp, #48]
 402e5cc:	aa0003f7 	mov	x23, x0
 402e5d0:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e5d4:	f90023fe 	str	x30, [sp, #64]
 402e5d8:	f9477001 	ldr	x1, [x0, #3808]
 402e5dc:	f90007e1 	str	x1, [sp, #8]
 402e5e0:	d2800001 	mov	x1, #0x0                   	// #0
 402e5e4:	52800640 	mov	w0, #0x32                  	// #50
 402e5e8:	b90007e0 	str	w0, [sp, #4]
 402e5ec:	97fff15c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e5f0:	b9401400 	ldr	w0, [x0, #20]
 402e5f4:	7100041f 	cmp	w0, #0x1
 402e5f8:	540000a1 	b.ne	402e60c <ddrtrn_vref_find_best+0x58>  // b.any
 402e5fc:	97fff158 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e600:	b9402400 	ldr	w0, [x0, #36]
 402e604:	910013e1 	add	x1, sp, #0x4
 402e608:	97fff93a 	bl	402caf0 <ddrtrn_hal_vref_get_host_max>
 402e60c:	0b160274 	add	w20, w19, w22
 402e610:	37f80333 	tbnz	w19, #31, 402e674 <ddrtrn_vref_find_best+0xc0>
 402e614:	b94007e1 	ldr	w1, [sp, #4]
 402e618:	6b13003f 	cmp	w1, w19
 402e61c:	1a81a273 	csel	w19, w19, w1, ge  // ge = tcont
 402e620:	52800015 	mov	w21, #0x0                   	// #0
 402e624:	52800018 	mov	w24, #0x0                   	// #0
 402e628:	37f80194 	tbnz	w20, #31, 402e658 <ddrtrn_vref_find_best+0xa4>
 402e62c:	b94007e0 	ldr	w0, [sp, #4]
 402e630:	6b14001f 	cmp	w0, w20
 402e634:	5400012b 	b.lt	402e658 <ddrtrn_vref_find_best+0xa4>  // b.tstop
 402e638:	2a1403e1 	mov	w1, w20
 402e63c:	aa1703e0 	mov	x0, x23
 402e640:	97ffffb4 	bl	402e510 <ddrtrn_vref_get_win>
 402e644:	6b00031f 	cmp	w24, w0
 402e648:	540001a9 	b.ls	402e67c <ddrtrn_vref_find_best+0xc8>  // b.plast
 402e64c:	110006b5 	add	w21, w21, #0x1
 402e650:	71000ebf 	cmp	w21, #0x3
 402e654:	540001a1 	b.ne	402e688 <ddrtrn_vref_find_best+0xd4>  // b.any
 402e658:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e65c:	f94007e1 	ldr	x1, [sp, #8]
 402e660:	f9477002 	ldr	x2, [x0, #3808]
 402e664:	eb020021 	subs	x1, x1, x2
 402e668:	d2800002 	mov	x2, #0x0                   	// #0
 402e66c:	54000120 	b.eq	402e690 <ddrtrn_vref_find_best+0xdc>  // b.none
 402e670:	97ffdee3 	bl	40261fc <__stack_chk_fail>
 402e674:	52800013 	mov	w19, #0x0                   	// #0
 402e678:	17ffffea 	b	402e620 <ddrtrn_vref_find_best+0x6c>
 402e67c:	2a1403f3 	mov	w19, w20
 402e680:	2a0003f8 	mov	w24, w0
 402e684:	52800015 	mov	w21, #0x0                   	// #0
 402e688:	0b160294 	add	w20, w20, w22
 402e68c:	17ffffe7 	b	402e628 <ddrtrn_vref_find_best+0x74>
 402e690:	2a1303e0 	mov	w0, w19
 402e694:	a94153f3 	ldp	x19, x20, [sp, #16]
 402e698:	a9425bf5 	ldp	x21, x22, [sp, #32]
 402e69c:	a94363f7 	ldp	x23, x24, [sp, #48]
 402e6a0:	f94023fe 	ldr	x30, [sp, #64]
 402e6a4:	910143ff 	add	sp, sp, #0x50
 402e6a8:	d65f03c0 	ret

000000000402e6ac <ddrtrn_vref_cal>:
 402e6ac:	d100c3ff 	sub	sp, sp, #0x30
 402e6b0:	a90153f3 	stp	x19, x20, [sp, #16]
 402e6b4:	aa0003f3 	mov	x19, x0
 402e6b8:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e6bc:	a9027bf5 	stp	x21, x30, [sp, #32]
 402e6c0:	f9477001 	ldr	x1, [x0, #3808]
 402e6c4:	f90007e1 	str	x1, [sp, #8]
 402e6c8:	d2800001 	mov	x1, #0x0                   	// #0
 402e6cc:	b90007ff 	str	wzr, [sp, #4]
 402e6d0:	97fff123 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e6d4:	b9401400 	ldr	w0, [x0, #20]
 402e6d8:	7100041f 	cmp	w0, #0x1
 402e6dc:	54000401 	b.ne	402e75c <ddrtrn_vref_cal+0xb0>  // b.any
 402e6e0:	97fff11f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e6e4:	b9400414 	ldr	w20, [x0, #4]
 402e6e8:	97fff11d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e6ec:	b9402415 	ldr	w21, [x0, #36]
 402e6f0:	97fff11b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e6f4:	b9401802 	ldr	w2, [x0, #24]
 402e6f8:	910013e3 	add	x3, sp, #0x4
 402e6fc:	2a1503e1 	mov	w1, w21
 402e700:	aa1403e0 	mov	x0, x20
 402e704:	97fff901 	bl	402cb08 <ddrtrn_hal_vref_phy_host_get>
 402e708:	b94007f4 	ldr	w20, [sp, #4]
 402e70c:	aa1303e0 	mov	x0, x19
 402e710:	51000e81 	sub	w1, w20, #0x3
 402e714:	97ffff7f 	bl	402e510 <ddrtrn_vref_get_win>
 402e718:	2a0003f5 	mov	w21, w0
 402e71c:	11000e81 	add	w1, w20, #0x3
 402e720:	aa1303e0 	mov	x0, x19
 402e724:	97ffff7b 	bl	402e510 <ddrtrn_vref_get_win>
 402e728:	6b0002bf 	cmp	w21, w0
 402e72c:	54000282 	b.cs	402e77c <ddrtrn_vref_cal+0xd0>  // b.hs, b.nlast
 402e730:	52800022 	mov	w2, #0x1                   	// #1
 402e734:	2a1403e1 	mov	w1, w20
 402e738:	aa1303e0 	mov	x0, x19
 402e73c:	97ffff9e 	bl	402e5b4 <ddrtrn_vref_find_best>
 402e740:	f0ffff81 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402e744:	f94007e2 	ldr	x2, [sp, #8]
 402e748:	f9477023 	ldr	x3, [x1, #3808]
 402e74c:	eb030042 	subs	x2, x2, x3
 402e750:	d2800003 	mov	x3, #0x0                   	// #0
 402e754:	54000360 	b.eq	402e7c0 <ddrtrn_vref_cal+0x114>  // b.none
 402e758:	97ffdea9 	bl	40261fc <__stack_chk_fail>
 402e75c:	97fff100 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e760:	b9400414 	ldr	w20, [x0, #4]
 402e764:	97fff0fe 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e768:	b9401802 	ldr	w2, [x0, #24]
 402e76c:	910013e1 	add	x1, sp, #0x4
 402e770:	aa1403e0 	mov	x0, x20
 402e774:	97fff8f1 	bl	402cb38 <ddrtrn_hal_vref_phy_dram_get>
 402e778:	17ffffe4 	b	402e708 <ddrtrn_vref_cal+0x5c>
 402e77c:	54000069 	b.ls	402e788 <ddrtrn_vref_cal+0xdc>  // b.plast
 402e780:	12800002 	mov	w2, #0xffffffff            	// #-1
 402e784:	17ffffec 	b	402e734 <ddrtrn_vref_cal+0x88>
 402e788:	52800640 	mov	w0, #0x32                  	// #50
 402e78c:	b90007e0 	str	w0, [sp, #4]
 402e790:	97fff0f3 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e794:	b9401400 	ldr	w0, [x0, #20]
 402e798:	7100041f 	cmp	w0, #0x1
 402e79c:	540000a1 	b.ne	402e7b0 <ddrtrn_vref_cal+0x104>  // b.any
 402e7a0:	97fff0ef 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e7a4:	b9402400 	ldr	w0, [x0, #36]
 402e7a8:	910013e1 	add	x1, sp, #0x4
 402e7ac:	97fff8d1 	bl	402caf0 <ddrtrn_hal_vref_get_host_max>
 402e7b0:	b94007e0 	ldr	w0, [sp, #4]
 402e7b4:	6b40069f 	cmp	w20, w0, lsr #1
 402e7b8:	54fffe42 	b.cs	402e780 <ddrtrn_vref_cal+0xd4>  // b.hs, b.nlast
 402e7bc:	17ffffdd 	b	402e730 <ddrtrn_vref_cal+0x84>
 402e7c0:	a94153f3 	ldp	x19, x20, [sp, #16]
 402e7c4:	a9427bf5 	ldp	x21, x30, [sp, #32]
 402e7c8:	9100c3ff 	add	sp, sp, #0x30
 402e7cc:	1400078a 	b	40305f4 <ddrtrn_hal_vref_status_set>

000000000402e7d0 <ddrtrn_vref_training>:
 402e7d0:	d10603ff 	sub	sp, sp, #0x180
 402e7d4:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e7d8:	a91653f3 	stp	x19, x20, [sp, #352]
 402e7dc:	a9177bf5 	stp	x21, x30, [sp, #368]
 402e7e0:	f9477001 	ldr	x1, [x0, #3808]
 402e7e4:	f900afe1 	str	x1, [sp, #344]
 402e7e8:	d2800001 	mov	x1, #0x0                   	// #0
 402e7ec:	910003e0 	mov	x0, sp
 402e7f0:	94000728 	bl	4030490 <ddrtrn_hal_vref_save_bdl>
 402e7f4:	52802082 	mov	w2, #0x104                 	// #260
 402e7f8:	52800001 	mov	w1, #0x0                   	// #0
 402e7fc:	910143e0 	add	x0, sp, #0x50
 402e800:	97fffecb 	bl	402e32c <ddrtrn_set_data>
 402e804:	97fff0d6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e808:	b9401400 	ldr	w0, [x0, #20]
 402e80c:	7100041f 	cmp	w0, #0x1
 402e810:	540007c0 	b.eq	402e908 <ddrtrn_vref_training+0x138>  // b.none
 402e814:	97fff0d2 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e818:	aa0003f3 	mov	x19, x0
 402e81c:	97fff0cb 	bl	402ab48 <ddrtrn_hal_get_phy>
 402e820:	d2800782 	mov	x2, #0x3c                  	// #60
 402e824:	b9402261 	ldr	w1, [x19, #32]
 402e828:	9b020021 	madd	x1, x1, x2, x0
 402e82c:	b9400433 	ldr	w19, [x1, #4]
 402e830:	97fff314 	bl	402b480 <ddrtrn_hal_ddrc_get_bank_group>
 402e834:	71001a7f 	cmp	w19, #0x6
 402e838:	7a4a1a64 	ccmp	w19, #0xa, #0x4, ne  // ne = any
 402e83c:	54000581 	b.ne	402e8ec <ddrtrn_vref_training+0x11c>  // b.any
 402e840:	71001a7f 	cmp	w19, #0x6
 402e844:	52800013 	mov	w19, #0x0                   	// #0
 402e848:	1a9f1414 	csinc	w20, w0, wzr, ne  // ne = any
 402e84c:	97ffff23 	bl	402e4d8 <ddrtrn_hal_get_byte_num>
 402e850:	6b13001f 	cmp	w0, w19
 402e854:	54000449 	b.ls	402e8dc <ddrtrn_vref_training+0x10c>  // b.plast
 402e858:	97fff0c1 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e85c:	b9402815 	ldr	w21, [x0, #40]
 402e860:	97fff0bf 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e864:	0b150675 	add	w21, w19, w21, lsl #1
 402e868:	b9001815 	str	w21, [x0, #24]
 402e86c:	7100069f 	cmp	w20, #0x1
 402e870:	54000081 	b.ne	402e880 <ddrtrn_vref_training+0xb0>  // b.any
 402e874:	121e7a60 	and	w0, w19, #0xfffffffd
 402e878:	7100041f 	cmp	w0, #0x1
 402e87c:	54000060 	b.eq	402e888 <ddrtrn_vref_training+0xb8>  // b.none
 402e880:	910143e0 	add	x0, sp, #0x50
 402e884:	97ffff8a 	bl	402e6ac <ddrtrn_vref_cal>
 402e888:	11000673 	add	w19, w19, #0x1
 402e88c:	17fffff0 	b	402e84c <ddrtrn_vref_training+0x7c>
 402e890:	97fff0b3 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e894:	b9402814 	ldr	w20, [x0, #40]
 402e898:	97fff0b1 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e89c:	0b140674 	add	w20, w19, w20, lsl #1
 402e8a0:	b9001814 	str	w20, [x0, #24]
 402e8a4:	97fff0ae 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e8a8:	b9401800 	ldr	w0, [x0, #24]
 402e8ac:	7100041f 	cmp	w0, #0x1
 402e8b0:	540000e0 	b.eq	402e8cc <ddrtrn_vref_training+0xfc>  // b.none
 402e8b4:	97fff0aa 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e8b8:	b9401800 	ldr	w0, [x0, #24]
 402e8bc:	71000c1f 	cmp	w0, #0x3
 402e8c0:	54000060 	b.eq	402e8cc <ddrtrn_vref_training+0xfc>  // b.none
 402e8c4:	910143e0 	add	x0, sp, #0x50
 402e8c8:	97ffff79 	bl	402e6ac <ddrtrn_vref_cal>
 402e8cc:	11000673 	add	w19, w19, #0x1
 402e8d0:	97ffff02 	bl	402e4d8 <ddrtrn_hal_get_byte_num>
 402e8d4:	6b13001f 	cmp	w0, w19
 402e8d8:	54fffdc8 	b.hi	402e890 <ddrtrn_vref_training+0xc0>  // b.pmore
 402e8dc:	910003e0 	mov	x0, sp
 402e8e0:	9400071a 	bl	4030548 <ddrtrn_hal_vref_restore_bdl>
 402e8e4:	910143e0 	add	x0, sp, #0x50
 402e8e8:	94000044 	bl	402e9f8 <ddrtrn_result_data_save>
 402e8ec:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e8f0:	f940afe1 	ldr	x1, [sp, #344]
 402e8f4:	f9477002 	ldr	x2, [x0, #3808]
 402e8f8:	eb020021 	subs	x1, x1, x2
 402e8fc:	d2800002 	mov	x2, #0x0                   	// #0
 402e900:	54000080 	b.eq	402e910 <ddrtrn_vref_training+0x140>  // b.none
 402e904:	97ffde3e 	bl	40261fc <__stack_chk_fail>
 402e908:	52800013 	mov	w19, #0x0                   	// #0
 402e90c:	17fffff1 	b	402e8d0 <ddrtrn_vref_training+0x100>
 402e910:	52800000 	mov	w0, #0x0                   	// #0
 402e914:	a95653f3 	ldp	x19, x20, [sp, #352]
 402e918:	a9577bf5 	ldp	x21, x30, [sp, #368]
 402e91c:	910603ff 	add	sp, sp, #0x180
 402e920:	d65f03c0 	ret

000000000402e924 <ddrtrn_vref_training_func>:
 402e924:	d10183ff 	sub	sp, sp, #0x60
 402e928:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e92c:	a9057bf3 	stp	x19, x30, [sp, #80]
 402e930:	f9477001 	ldr	x1, [x0, #3808]
 402e934:	f90027e1 	str	x1, [sp, #72]
 402e938:	d2800001 	mov	x1, #0x0                   	// #0
 402e93c:	52800020 	mov	w0, #0x1                   	// #1
 402e940:	9400045d 	bl	402fab4 <ddrtrn_hal_set_dq_type>
 402e944:	910023e0 	add	x0, sp, #0x8
 402e948:	52a02001 	mov	w1, #0x1000000             	// #16777216
 402e94c:	97fff2d8 	bl	402b4ac <ddrtrn_hal_save_reg>
 402e950:	97fff340 	bl	402b650 <ddrtrn_hal_training_switch_axi>
 402e954:	52800040 	mov	w0, #0x2                   	// #2
 402e958:	97fffcd1 	bl	402dc9c <ddrtrn_ddrt_init>
 402e95c:	52a02000 	mov	w0, #0x1000000             	// #16777216
 402e960:	97fff4a6 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 402e964:	350002e0 	cbnz	w0, 402e9c0 <ddrtrn_vref_training_func+0x9c>
 402e968:	97fff07d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e96c:	52800021 	mov	w1, #0x1                   	// #1
 402e970:	b9001401 	str	w1, [x0, #20]
 402e974:	97ffff97 	bl	402e7d0 <ddrtrn_vref_training>
 402e978:	2a0003f3 	mov	w19, w0
 402e97c:	52a04000 	mov	w0, #0x2000000             	// #33554432
 402e980:	97fff49e 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 402e984:	350000c0 	cbnz	w0, 402e99c <ddrtrn_vref_training_func+0x78>
 402e988:	97fff075 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402e98c:	52800041 	mov	w1, #0x2                   	// #2
 402e990:	b9001401 	str	w1, [x0, #20]
 402e994:	97ffff8f 	bl	402e7d0 <ddrtrn_vref_training>
 402e998:	0b000273 	add	w19, w19, w0
 402e99c:	910023e0 	add	x0, sp, #0x8
 402e9a0:	97fff307 	bl	402b5bc <ddrtrn_hal_restore_reg>
 402e9a4:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402e9a8:	f94027e1 	ldr	x1, [sp, #72]
 402e9ac:	f9477002 	ldr	x2, [x0, #3808]
 402e9b0:	eb020021 	subs	x1, x1, x2
 402e9b4:	d2800002 	mov	x2, #0x0                   	// #0
 402e9b8:	54000080 	b.eq	402e9c8 <ddrtrn_vref_training_func+0xa4>  // b.none
 402e9bc:	97ffde10 	bl	40261fc <__stack_chk_fail>
 402e9c0:	52800013 	mov	w19, #0x0                   	// #0
 402e9c4:	17ffffee 	b	402e97c <ddrtrn_vref_training_func+0x58>
 402e9c8:	2a1303e0 	mov	w0, w19
 402e9cc:	a9457bf3 	ldp	x19, x30, [sp, #80]
 402e9d0:	910183ff 	add	sp, sp, #0x60
 402e9d4:	d65f03c0 	ret

000000000402e9d8 <ddrtrn_wl_func>:
 402e9d8:	52800000 	mov	w0, #0x0                   	// #0
 402e9dc:	d65f03c0 	ret

000000000402e9e0 <bsp_ddrtrn_resume>:
 402e9e0:	1400036f 	b	402f79c <ddrtrn_resume>

000000000402e9e4 <bsp_ddrtrn_suspend>:
 402e9e4:	f81f0ffe 	str	x30, [sp, #-16]!
 402e9e8:	9400036c 	bl	402f798 <ddrtrn_suspend_store_para>
 402e9ec:	52800000 	mov	w0, #0x0                   	// #0
 402e9f0:	f84107fe 	ldr	x30, [sp], #16
 402e9f4:	d65f03c0 	ret

000000000402e9f8 <ddrtrn_result_data_save>:
 402e9f8:	d65f03c0 	ret

000000000402e9fc <ddrtrn_lpca_data_save>:
 402e9fc:	d65f03c0 	ret

000000000402ea00 <ddrtrn_ddrt_get_test_addr>:
 402ea00:	52a80000 	mov	w0, #0x40000000            	// #1073741824
 402ea04:	d65f03c0 	ret

000000000402ea08 <ddrtrn_training_error>:
 402ea08:	d65f03c0 	ret

000000000402ea0c <ddrtrn_training_success>:
 402ea0c:	d65f03c0 	ret

000000000402ea10 <ddrtrn_training_start>:
 402ea10:	d65f03c0 	ret

000000000402ea14 <ddrtrn_chsel_remap_func>:
 402ea14:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402ea18:	97fff051 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ea1c:	b9402013 	ldr	w19, [x0, #32]
 402ea20:	97fff37a 	bl	402b808 <ddrtrn_hal_axi_special_intlv_en>
 402ea24:	35000233 	cbnz	w19, 402ea68 <ddrtrn_chsel_remap_func+0x54>
 402ea28:	97fff04d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ea2c:	aa0003f3 	mov	x19, x0
 402ea30:	97fff046 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ea34:	d2800782 	mov	x2, #0x3c                  	// #60
 402ea38:	b9402261 	ldr	w1, [x19, #32]
 402ea3c:	9b020021 	madd	x1, x1, x2, x0
 402ea40:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402ea44:	91029000 	add	x0, x0, #0xa4
 402ea48:	b9400422 	ldr	w2, [x1, #4]
 402ea4c:	91014001 	add	x1, x0, #0x50
 402ea50:	9100a000 	add	x0, x0, #0x28
 402ea54:	7100185f 	cmp	w2, #0x6
 402ea58:	9a810000 	csel	x0, x0, x1, eq  // eq = none
 402ea5c:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402ea60:	528000a1 	mov	w1, #0x5                   	// #5
 402ea64:	140007c5 	b	4030978 <ddrtrn_reg_config>
 402ea68:	7100067f 	cmp	w19, #0x1
 402ea6c:	540001c1 	b.ne	402eaa4 <ddrtrn_chsel_remap_func+0x90>  // b.any
 402ea70:	97fff03b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ea74:	aa0003f3 	mov	x19, x0
 402ea78:	97fff034 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ea7c:	d2800782 	mov	x2, #0x3c                  	// #60
 402ea80:	b9402261 	ldr	w1, [x19, #32]
 402ea84:	9b020021 	madd	x1, x1, x2, x0
 402ea88:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402ea8c:	b9400422 	ldr	w2, [x1, #4]
 402ea90:	91029001 	add	x1, x0, #0xa4
 402ea94:	9101e020 	add	x0, x1, #0x78
 402ea98:	7100185f 	cmp	w2, #0x6
 402ea9c:	9a811000 	csel	x0, x0, x1, ne  // ne = any
 402eaa0:	17ffffef 	b	402ea5c <ddrtrn_chsel_remap_func+0x48>
 402eaa4:	d2800000 	mov	x0, #0x0                   	// #0
 402eaa8:	17ffffed 	b	402ea5c <ddrtrn_chsel_remap_func+0x48>

000000000402eaac <ddrtrn_capat_adpat_cfg_nonlpddr4>:
 402eaac:	b9400401 	ldr	w1, [x0, #4]
 402eab0:	350000c1 	cbnz	w1, 402eac8 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x1c>
 402eab4:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402eab8:	91029000 	add	x0, x0, #0xa4
 402eabc:	91028000 	add	x0, x0, #0xa0
 402eac0:	52800081 	mov	w1, #0x4                   	// #4
 402eac4:	140007ad 	b	4030978 <ddrtrn_reg_config>
 402eac8:	b9400000 	ldr	w0, [x0]
 402eacc:	7110001f 	cmp	w0, #0x400
 402ead0:	54000101 	b.ne	402eaf0 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x44>  // b.any
 402ead4:	7108003f 	cmp	w1, #0x200
 402ead8:	540001e1 	b.ne	402eb14 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x68>  // b.any
 402eadc:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402eae0:	91029000 	add	x0, x0, #0xa4
 402eae4:	91030000 	add	x0, x0, #0xc0
 402eae8:	52800121 	mov	w1, #0x9                   	// #9
 402eaec:	17fffff6 	b	402eac4 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x18>
 402eaf0:	7120001f 	cmp	w0, #0x800
 402eaf4:	54000101 	b.ne	402eb14 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x68>  // b.any
 402eaf8:	7110003f 	cmp	w1, #0x400
 402eafc:	540000c1 	b.ne	402eb14 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x68>  // b.any
 402eb00:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402eb04:	91029000 	add	x0, x0, #0xa4
 402eb08:	91042000 	add	x0, x0, #0x108
 402eb0c:	52800121 	mov	w1, #0x9                   	// #9
 402eb10:	17ffffed 	b	402eac4 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x18>
 402eb14:	6b00003f 	cmp	w1, w0
 402eb18:	540000c1 	b.ne	402eb30 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x84>  // b.any
 402eb1c:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402eb20:	91029000 	add	x0, x0, #0xa4
 402eb24:	91054000 	add	x0, x0, #0x150
 402eb28:	52800081 	mov	w1, #0x4                   	// #4
 402eb2c:	17ffffe6 	b	402eac4 <ddrtrn_capat_adpat_cfg_nonlpddr4+0x18>
 402eb30:	d65f03c0 	ret

000000000402eb34 <ddrtrn_capat_adpat_cfg_lpddr4>:
 402eb34:	b9401001 	ldr	w1, [x0, #16]
 402eb38:	7100043f 	cmp	w1, #0x1
 402eb3c:	540000c1 	b.ne	402eb54 <ddrtrn_capat_adpat_cfg_lpddr4+0x20>  // b.any
 402eb40:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402eb44:	91029000 	add	x0, x0, #0xa4
 402eb48:	9105c000 	add	x0, x0, #0x170
 402eb4c:	52800101 	mov	w1, #0x8                   	// #8
 402eb50:	1400078a 	b	4030978 <ddrtrn_reg_config>
 402eb54:	7100083f 	cmp	w1, #0x2
 402eb58:	54000201 	b.ne	402eb98 <ddrtrn_capat_adpat_cfg_lpddr4+0x64>  // b.any
 402eb5c:	b9400800 	ldr	w0, [x0, #8]
 402eb60:	7120001f 	cmp	w0, #0x800
 402eb64:	540000c1 	b.ne	402eb7c <ddrtrn_capat_adpat_cfg_lpddr4+0x48>  // b.any
 402eb68:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402eb6c:	91029000 	add	x0, x0, #0xa4
 402eb70:	9106c000 	add	x0, x0, #0x1b0
 402eb74:	52800181 	mov	w1, #0xc                   	// #12
 402eb78:	17fffff6 	b	402eb50 <ddrtrn_capat_adpat_cfg_lpddr4+0x1c>
 402eb7c:	7140041f 	cmp	w0, #0x1, lsl #12
 402eb80:	540000c1 	b.ne	402eb98 <ddrtrn_capat_adpat_cfg_lpddr4+0x64>  // b.any
 402eb84:	90000020 	adrp	x0, 4032000 <g_svb_plus+0x10>
 402eb88:	91029000 	add	x0, x0, #0xa4
 402eb8c:	91084000 	add	x0, x0, #0x210
 402eb90:	52800181 	mov	w1, #0xc                   	// #12
 402eb94:	17ffffef 	b	402eb50 <ddrtrn_capat_adpat_cfg_lpddr4+0x1c>
 402eb98:	d65f03c0 	ret

000000000402eb9c <ddrtrn_hal_get_ck_phase>:
 402eb9c:	52820e02 	mov	w2, #0x1070                	// #4208
 402eba0:	0b020000 	add	w0, w0, w2
 402eba4:	b9400000 	ldr	w0, [x0]
 402eba8:	53197c02 	lsr	w2, w0, #25
 402ebac:	d3462003 	ubfx	x3, x0, #6, #3
 402ebb0:	531d0042 	ubfiz	w2, w2, #3, #1
 402ebb4:	2a030042 	orr	w2, w2, w3
 402ebb8:	b9000022 	str	w2, [x1]
 402ebbc:	53187c02 	lsr	w2, w0, #24
 402ebc0:	d3492c00 	ubfx	x0, x0, #9, #3
 402ebc4:	531d0042 	ubfiz	w2, w2, #3, #1
 402ebc8:	2a000040 	orr	w0, w2, w0
 402ebcc:	b9000420 	str	w0, [x1, #4]
 402ebd0:	d65f03c0 	ret

000000000402ebd4 <ddrtrn_hal_low_freq_cfg_freq_process>:
 402ebd4:	a9bb53f3 	stp	x19, x20, [sp, #-80]!
 402ebd8:	d2840013 	mov	x19, #0x2000                	// #8192
 402ebdc:	f2a22033 	movk	x19, #0x1101, lsl #16
 402ebe0:	a9015bf5 	stp	x21, x22, [sp, #16]
 402ebe4:	d2803215 	mov	x21, #0x190                 	// #400
 402ebe8:	f2a22035 	movk	x21, #0x1101, lsl #16
 402ebec:	a90263f7 	stp	x23, x24, [sp, #32]
 402ebf0:	2a0003f4 	mov	w20, w0
 402ebf4:	b9400277 	ldr	w23, [x19]
 402ebf8:	b9400260 	ldr	w0, [x19]
 402ebfc:	b94002b6 	ldr	w22, [x21]
 402ec00:	120d72e0 	and	w0, w23, #0xfff8ffff
 402ec04:	a9036bf9 	stp	x25, x26, [sp, #48]
 402ec08:	2a0103f9 	mov	w25, w1
 402ec0c:	f90023fe 	str	x30, [sp, #64]
 402ec10:	b9000260 	str	w0, [x19]
 402ec14:	5289c400 	mov	w0, #0x4e20                	// #20000
 402ec18:	97fff67f 	bl	402c614 <ddrtrn_hal_training_delay>
 402ec1c:	b9400278 	ldr	w24, [x19]
 402ec20:	12117300 	and	w0, w24, #0xffff8fff
 402ec24:	b9000260 	str	w0, [x19]
 402ec28:	5289c400 	mov	w0, #0x4e20                	// #20000
 402ec2c:	97fff67a 	bl	402c614 <ddrtrn_hal_training_delay>
 402ec30:	d2840400 	mov	x0, #0x2020                	// #8224
 402ec34:	f2a22020 	movk	x0, #0x1101, lsl #16
 402ec38:	b9400000 	ldr	w0, [x0]
 402ec3c:	f274081f 	tst	x0, #0x7000
 402ec40:	540004e1 	b.ne	402ecdc <ddrtrn_hal_low_freq_cfg_freq_process+0x108>  // b.any
 402ec44:	528000a0 	mov	w0, #0x5                   	// #5
 402ec48:	2a0002c0 	orr	w0, w22, w0
 402ec4c:	b90002a0 	str	w0, [x21]
 402ec50:	5290d400 	mov	w0, #0x86a0                	// #34464
 402ec54:	72a00020 	movk	w0, #0x1, lsl #16
 402ec58:	d280309a 	mov	x26, #0x184                 	// #388
 402ec5c:	97fff66e 	bl	402c614 <ddrtrn_hal_training_delay>
 402ec60:	f2a2203a 	movk	x26, #0x1101, lsl #16
 402ec64:	128000a0 	mov	w0, #0xfffffffa            	// #-6
 402ec68:	0a0002c0 	and	w0, w22, w0
 402ec6c:	321e0000 	orr	w0, w0, #0x4
 402ec70:	b90002a0 	str	w0, [x21]
 402ec74:	320c0280 	orr	w0, w20, #0x100000
 402ec78:	b9000340 	str	w0, [x26]
 402ec7c:	d2803000 	mov	x0, #0x180                 	// #384
 402ec80:	120b7a94 	and	w20, w20, #0xffefffff
 402ec84:	f2a22020 	movk	x0, #0x1101, lsl #16
 402ec88:	b9000019 	str	w25, [x0]
 402ec8c:	5280fa00 	mov	w0, #0x7d0                 	// #2000
 402ec90:	97fff661 	bl	402c614 <ddrtrn_hal_training_delay>
 402ec94:	b9000354 	str	w20, [x26]
 402ec98:	5281a800 	mov	w0, #0xd40                 	// #3392
 402ec9c:	72a00060 	movk	w0, #0x3, lsl #16
 402eca0:	97fff65d 	bl	402c614 <ddrtrn_hal_training_delay>
 402eca4:	321e02c0 	orr	w0, w22, #0x4
 402eca8:	b90002a0 	str	w0, [x21]
 402ecac:	5280fa00 	mov	w0, #0x7d0                 	// #2000
 402ecb0:	97fff659 	bl	402c614 <ddrtrn_hal_training_delay>
 402ecb4:	a9436bf9 	ldp	x25, x26, [sp, #48]
 402ecb8:	5289c400 	mov	w0, #0x4e20                	// #20000
 402ecbc:	f94023fe 	ldr	x30, [sp, #64]
 402ecc0:	b90002b6 	str	w22, [x21]
 402ecc4:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402ecc8:	b9000278 	str	w24, [x19]
 402eccc:	b9000277 	str	w23, [x19]
 402ecd0:	a94263f7 	ldp	x23, x24, [sp, #32]
 402ecd4:	a8c553f3 	ldp	x19, x20, [sp], #80
 402ecd8:	17fff64f 	b	402c614 <ddrtrn_hal_training_delay>
 402ecdc:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402ece0:	a94263f7 	ldp	x23, x24, [sp, #32]
 402ece4:	a9436bf9 	ldp	x25, x26, [sp, #48]
 402ece8:	f94023fe 	ldr	x30, [sp, #64]
 402ecec:	a8c553f3 	ldp	x19, x20, [sp], #80
 402ecf0:	d65f03c0 	ret

000000000402ecf4 <ddrtrn_hal_get_phy_addr>:
 402ecf4:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402ecf8:	2a0003f3 	mov	w19, w0
 402ecfc:	97ffef93 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ed00:	d2800781 	mov	x1, #0x3c                  	// #60
 402ed04:	9b017e73 	mul	x19, x19, x1
 402ed08:	b8736800 	ldr	w0, [x0, x19]
 402ed0c:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402ed10:	d65f03c0 	ret

000000000402ed14 <ddrtrn_hal_get_cur_phy_total_byte_num>:
 402ed14:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402ed18:	97ffef91 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ed1c:	aa0003f3 	mov	x19, x0
 402ed20:	97ffef8a 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ed24:	d2800782 	mov	x2, #0x3c                  	// #60
 402ed28:	b9402261 	ldr	w1, [x19, #32]
 402ed2c:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402ed30:	d503201f 	nop
 402ed34:	9b020021 	madd	x1, x1, x2, x0
 402ed38:	b9401020 	ldr	w0, [x1, #16]
 402ed3c:	d65f03c0 	ret

000000000402ed40 <ddrtrn_hal_get_phy_num>:
 402ed40:	f81f0ffe 	str	x30, [sp, #-16]!
 402ed44:	97ffef86 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ed48:	b9400000 	ldr	w0, [x0]
 402ed4c:	f84107fe 	ldr	x30, [sp], #16
 402ed50:	d65f03c0 	ret

000000000402ed54 <ddrtrn_hal_low_freq_pll_power>:
 402ed54:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402ed58:	2a0003f4 	mov	w20, w0
 402ed5c:	52800013 	mov	w19, #0x0                   	// #0
 402ed60:	a9017bf5 	stp	x21, x30, [sp, #16]
 402ed64:	d2800795 	mov	x21, #0x3c                  	// #60
 402ed68:	97fffff6 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402ed6c:	6b13001f 	cmp	w0, w19
 402ed70:	540000a8 	b.hi	402ed84 <ddrtrn_hal_low_freq_pll_power+0x30>  // b.pmore
 402ed74:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402ed78:	5289c400 	mov	w0, #0x4e20                	// #20000
 402ed7c:	a8c253f3 	ldp	x19, x20, [sp], #32
 402ed80:	17fff625 	b	402c614 <ddrtrn_hal_training_delay>
 402ed84:	97ffef71 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ed88:	2a1303e1 	mov	w1, w19
 402ed8c:	9b150021 	madd	x1, x1, x21, x0
 402ed90:	b9403020 	ldr	w0, [x1, #48]
 402ed94:	34000100 	cbz	w0, 402edb4 <ddrtrn_hal_low_freq_pll_power+0x60>
 402ed98:	2a1303e0 	mov	w0, w19
 402ed9c:	97ffffd6 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402eda0:	11006002 	add	w2, w0, #0x18
 402eda4:	b9400041 	ldr	w1, [x2]
 402eda8:	121d7021 	and	w1, w1, #0xfffffff8
 402edac:	2a140021 	orr	w1, w1, w20
 402edb0:	b9000041 	str	w1, [x2]
 402edb4:	11000673 	add	w19, w19, #0x1
 402edb8:	17ffffec 	b	402ed68 <ddrtrn_hal_low_freq_pll_power+0x14>

000000000402edbc <ddrtrn_pll_lock>:
 402edbc:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402edc0:	52800013 	mov	w19, #0x0                   	// #0
 402edc4:	52800034 	mov	w20, #0x1                   	// #1
 402edc8:	f9000bfe 	str	x30, [sp, #16]
 402edcc:	97ffffdd 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402edd0:	6b13001f 	cmp	w0, w19
 402edd4:	540000a8 	b.hi	402ede8 <ddrtrn_pll_lock+0x2c>  // b.pmore
 402edd8:	2a1403e0 	mov	w0, w20
 402eddc:	f9400bfe 	ldr	x30, [sp, #16]
 402ede0:	a8c253f3 	ldp	x19, x20, [sp], #32
 402ede4:	d65f03c0 	ret
 402ede8:	2a1303e0 	mov	w0, w19
 402edec:	97ffffc2 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402edf0:	11026000 	add	w0, w0, #0x98
 402edf4:	11000673 	add	w19, w19, #0x1
 402edf8:	b9400000 	ldr	w0, [x0]
 402edfc:	0a407e94 	and	w20, w20, w0, lsr #31
 402ee00:	17fffff3 	b	402edcc <ddrtrn_pll_lock+0x10>

000000000402ee04 <ddrtrn_hal_low_freq_phy_clk>:
 402ee04:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402ee08:	2a0003f4 	mov	w20, w0
 402ee0c:	52800013 	mov	w19, #0x0                   	// #0
 402ee10:	a9017bf5 	stp	x21, x30, [sp, #16]
 402ee14:	d2800795 	mov	x21, #0x3c                  	// #60
 402ee18:	97ffffca 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402ee1c:	6b13001f 	cmp	w0, w19
 402ee20:	54000088 	b.hi	402ee30 <ddrtrn_hal_low_freq_phy_clk+0x2c>  // b.pmore
 402ee24:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402ee28:	a8c253f3 	ldp	x19, x20, [sp], #32
 402ee2c:	d65f03c0 	ret
 402ee30:	97ffef46 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ee34:	2a1303e1 	mov	w1, w19
 402ee38:	9b150021 	madd	x1, x1, x21, x0
 402ee3c:	b9403020 	ldr	w0, [x1, #48]
 402ee40:	34000100 	cbz	w0, 402ee60 <ddrtrn_hal_low_freq_phy_clk+0x5c>
 402ee44:	2a1303e0 	mov	w0, w19
 402ee48:	97ffffab 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402ee4c:	11003002 	add	w2, w0, #0xc
 402ee50:	b9400041 	ldr	w1, [x2]
 402ee54:	12113c21 	and	w1, w1, #0x7fff8000
 402ee58:	2a140021 	orr	w1, w1, w20
 402ee5c:	b9000041 	str	w1, [x2]
 402ee60:	11000673 	add	w19, w19, #0x1
 402ee64:	17ffffed 	b	402ee18 <ddrtrn_hal_low_freq_phy_clk+0x14>

000000000402ee68 <ddrtrn_hal_low_freq_ctrl_easr>:
 402ee68:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402ee6c:	aa0003f3 	mov	x19, x0
 402ee70:	a9015bf5 	stp	x21, x22, [sp, #16]
 402ee74:	2a0103f5 	mov	w21, w1
 402ee78:	a9027bf7 	stp	x23, x30, [sp, #32]
 402ee7c:	97ffffb1 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402ee80:	7100041f 	cmp	w0, #0x1
 402ee84:	54000108 	b.hi	402eea4 <ddrtrn_hal_low_freq_ctrl_easr+0x3c>  // b.pmore
 402ee88:	52800014 	mov	w20, #0x0                   	// #0
 402ee8c:	d2800797 	mov	x23, #0x3c                  	// #60
 402ee90:	97ffffac 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402ee94:	6b00029f 	cmp	w20, w0
 402ee98:	54000103 	b.cc	402eeb8 <ddrtrn_hal_low_freq_ctrl_easr+0x50>  // b.lo, b.ul, b.last
 402ee9c:	52800000 	mov	w0, #0x0                   	// #0
 402eea0:	14000002 	b	402eea8 <ddrtrn_hal_low_freq_ctrl_easr+0x40>
 402eea4:	12800000 	mov	w0, #0xffffffff            	// #-1
 402eea8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402eeac:	a9427bf7 	ldp	x23, x30, [sp, #32]
 402eeb0:	a8c353f3 	ldp	x19, x20, [sp], #48
 402eeb4:	d65f03c0 	ret
 402eeb8:	97ffef29 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402eebc:	b9002014 	str	w20, [x0, #32]
 402eec0:	97ffef27 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402eec4:	aa0003f6 	mov	x22, x0
 402eec8:	97ffef20 	bl	402ab48 <ddrtrn_hal_get_phy>
 402eecc:	b94022c1 	ldr	w1, [x22, #32]
 402eed0:	9b170021 	madd	x1, x1, x23, x0
 402eed4:	b9400420 	ldr	w0, [x1, #4]
 402eed8:	7100181f 	cmp	w0, #0x6
 402eedc:	540000c1 	b.ne	402eef4 <ddrtrn_hal_low_freq_ctrl_easr+0x8c>  // b.any
 402eee0:	aa1303e0 	mov	x0, x19
 402eee4:	97fffaf8 	bl	402dac4 <ddrtrn_sref_cfg_save>
 402eee8:	aa1303e0 	mov	x0, x19
 402eeec:	52800181 	mov	w1, #0xc                   	// #12
 402eef0:	97fff8e9 	bl	402d294 <ddrtrn_sref_cfg>
 402eef4:	2a1503e0 	mov	w0, w21
 402eef8:	97fffb13 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402eefc:	35fffd40 	cbnz	w0, 402eea4 <ddrtrn_hal_low_freq_ctrl_easr+0x3c>
 402ef00:	97ffef17 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ef04:	aa0003f6 	mov	x22, x0
 402ef08:	97ffef10 	bl	402ab48 <ddrtrn_hal_get_phy>
 402ef0c:	b94022c1 	ldr	w1, [x22, #32]
 402ef10:	9b170021 	madd	x1, x1, x23, x0
 402ef14:	b9400420 	ldr	w0, [x1, #4]
 402ef18:	7100181f 	cmp	w0, #0x6
 402ef1c:	54000061 	b.ne	402ef28 <ddrtrn_hal_low_freq_ctrl_easr+0xc0>  // b.any
 402ef20:	aa1303e0 	mov	x0, x19
 402ef24:	97fffaf9 	bl	402db08 <ddrtrn_sref_cfg_restore>
 402ef28:	11000694 	add	w20, w20, #0x1
 402ef2c:	17ffffd9 	b	402ee90 <ddrtrn_hal_low_freq_ctrl_easr+0x28>

000000000402ef30 <ddrtrn_low_freq_start>:
 402ef30:	a9ba53f3 	stp	x19, x20, [sp, #-96]!
 402ef34:	52802402 	mov	w2, #0x120                 	// #288
 402ef38:	52807d14 	mov	w20, #0x3e8                 	// #1000
 402ef3c:	a9015bf5 	stp	x21, x22, [sp, #16]
 402ef40:	2a0003f6 	mov	w22, w0
 402ef44:	f0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402ef48:	a90263f7 	stp	x23, x24, [sp, #32]
 402ef4c:	52955555 	mov	w21, #0xaaaa                	// #43690
 402ef50:	52955557 	mov	w23, #0xaaaa                	// #43690
 402ef54:	a9036bf9 	stp	x25, x26, [sp, #48]
 402ef58:	52955558 	mov	w24, #0xaaaa                	// #43690
 402ef5c:	72a23555 	movk	w21, #0x11aa, lsl #16
 402ef60:	a90473fb 	stp	x27, x28, [sp, #64]
 402ef64:	72a29557 	movk	w23, #0x14aa, lsl #16
 402ef68:	72a25558 	movk	w24, #0x12aa, lsl #16
 402ef6c:	f9002bfe 	str	x30, [sp, #80]
 402ef70:	d10943ff 	sub	sp, sp, #0x250
 402ef74:	f9477001 	ldr	x1, [x0, #3808]
 402ef78:	f90127e1 	str	x1, [sp, #584]
 402ef7c:	d2800001 	mov	x1, #0x0                   	// #0
 402ef80:	9104a3e0 	add	x0, sp, #0x128
 402ef84:	97fffcea 	bl	402e32c <ddrtrn_set_data>
 402ef88:	910223e0 	add	x0, sp, #0x88
 402ef8c:	94000685 	bl	40309a0 <ddrtrn_save_rdqbdl_phy>
 402ef90:	d2803080 	mov	x0, #0x184                 	// #388
 402ef94:	f2a22020 	movk	x0, #0x1101, lsl #16
 402ef98:	b9400000 	ldr	w0, [x0]
 402ef9c:	b9000be0 	str	w0, [sp, #8]
 402efa0:	d2803000 	mov	x0, #0x180                 	// #384
 402efa4:	f2a22020 	movk	x0, #0x1101, lsl #16
 402efa8:	b9400000 	ldr	w0, [x0]
 402efac:	b90013e0 	str	w0, [sp, #16]
 402efb0:	52800000 	mov	w0, #0x0                   	// #0
 402efb4:	97ffff94 	bl	402ee04 <ddrtrn_hal_low_freq_phy_clk>
 402efb8:	528000e0 	mov	w0, #0x7                   	// #7
 402efbc:	97ffff66 	bl	402ed54 <ddrtrn_hal_low_freq_pll_power>
 402efc0:	97ffeee2 	bl	402ab48 <ddrtrn_hal_get_phy>
 402efc4:	b9400000 	ldr	w0, [x0]
 402efc8:	1101e000 	add	w0, w0, #0x78
 402efcc:	b9400000 	ldr	w0, [x0]
 402efd0:	72000400 	ands	w0, w0, #0x3
 402efd4:	54000260 	b.eq	402f020 <ddrtrn_low_freq_start+0xf0>  // b.none
 402efd8:	7100041f 	cmp	w0, #0x1
 402efdc:	1a9812e1 	csel	w1, w23, w24, ne  // ne = any
 402efe0:	52800013 	mov	w19, #0x0                   	// #0
 402efe4:	5284e219 	mov	w25, #0x2710                	// #10000
 402efe8:	52820420 	mov	w0, #0x1021                	// #4129
 402efec:	97fffefa 	bl	402ebd4 <ddrtrn_hal_low_freq_cfg_freq_process>
 402eff0:	52800000 	mov	w0, #0x0                   	// #0
 402eff4:	97ffff58 	bl	402ed54 <ddrtrn_hal_low_freq_pll_power>
 402eff8:	97ffff71 	bl	402edbc <ddrtrn_pll_lock>
 402effc:	6b19027f 	cmp	w19, w25
 402f000:	350001c0 	cbnz	w0, 402f038 <ddrtrn_low_freq_start+0x108>
 402f004:	54000121 	b.ne	402f028 <ddrtrn_low_freq_start+0xf8>  // b.any
 402f008:	71000694 	subs	w20, w20, #0x1
 402f00c:	54fffd61 	b.ne	402efb8 <ddrtrn_low_freq_start+0x88>  // b.any
 402f010:	d0000000 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402f014:	913ac400 	add	x0, x0, #0xeb1
 402f018:	97ffe710 	bl	4028c58 <log_serial_puts>
 402f01c:	14000008 	b	402f03c <ddrtrn_low_freq_start+0x10c>
 402f020:	2a1503e1 	mov	w1, w21
 402f024:	17ffffef 	b	402efe0 <ddrtrn_low_freq_start+0xb0>
 402f028:	5280fa00 	mov	w0, #0x7d0                 	// #2000
 402f02c:	11000673 	add	w19, w19, #0x1
 402f030:	97fff579 	bl	402c614 <ddrtrn_hal_training_delay>
 402f034:	17fffff1 	b	402eff8 <ddrtrn_low_freq_start+0xc8>
 402f038:	54fffe80 	b.eq	402f008 <ddrtrn_low_freq_start+0xd8>  // b.none
 402f03c:	52800013 	mov	w19, #0x0                   	// #0
 402f040:	32013fe0 	mov	w0, #0x80007fff            	// #-2147450881
 402f044:	97ffff70 	bl	402ee04 <ddrtrn_hal_low_freq_phy_clk>
 402f048:	97ffff3e 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402f04c:	6b13001f 	cmp	w0, w19
 402f050:	54000488 	b.hi	402f0e0 <ddrtrn_low_freq_start+0x1b0>  // b.pmore
 402f054:	528281a0 	mov	w0, #0x140d                	// #5133
 402f058:	72a00480 	movk	w0, #0x24, lsl #16
 402f05c:	0a0002c0 	and	w0, w22, w0
 402f060:	97fff2c2 	bl	402bb68 <ddrtrn_hal_hw_item_cfg>
 402f064:	97fffc8e 	bl	402e29c <ddrtrn_hw_training>
 402f068:	b9000fe0 	str	w0, [sp, #12]
 402f06c:	97ffff35 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402f070:	7100041f 	cmp	w0, #0x1
 402f074:	54000108 	b.hi	402f094 <ddrtrn_low_freq_start+0x164>  // b.pmore
 402f078:	52800015 	mov	w21, #0x0                   	// #0
 402f07c:	52800a17 	mov	w23, #0x50                  	// #80
 402f080:	d2800798 	mov	x24, #0x3c                  	// #60
 402f084:	52800199 	mov	w25, #0xc                   	// #12
 402f088:	97ffff2e 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402f08c:	6b0002bf 	cmp	w21, w0
 402f090:	54000323 	b.cc	402f0f4 <ddrtrn_low_freq_start+0x1c4>  // b.lo, b.ul, b.last
 402f094:	52800220 	mov	w0, #0x11                  	// #17
 402f098:	97fff2b4 	bl	402bb68 <ddrtrn_hal_hw_item_cfg>
 402f09c:	d280079a 	mov	x26, #0x3c                  	// #60
 402f0a0:	97fffc7f 	bl	402e29c <ddrtrn_hw_training>
 402f0a4:	97ffeeae 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f0a8:	b900201f 	str	wzr, [x0, #32]
 402f0ac:	97ffeeac 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f0b0:	aa0003f4 	mov	x20, x0
 402f0b4:	97ffeea5 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f0b8:	b9402281 	ldr	w1, [x20, #32]
 402f0bc:	9b1a7c21 	mul	x1, x1, x26
 402f0c0:	b8616814 	ldr	w20, [x0, x1]
 402f0c4:	97ffeea6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f0c8:	b9000414 	str	w20, [x0, #4]
 402f0cc:	97ffee9f 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f0d0:	b9403000 	ldr	w0, [x0, #48]
 402f0d4:	34000800 	cbz	w0, 402f1d4 <ddrtrn_low_freq_start+0x2a4>
 402f0d8:	52800015 	mov	w21, #0x0                   	// #0
 402f0dc:	14000036 	b	402f1b4 <ddrtrn_low_freq_start+0x284>
 402f0e0:	2a1303e0 	mov	w0, w19
 402f0e4:	11000673 	add	w19, w19, #0x1
 402f0e8:	97ffff03 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402f0ec:	97fff2e0 	bl	402bc6c <ddrtrn_hal_ck_cfg>
 402f0f0:	17ffffd6 	b	402f048 <ddrtrn_low_freq_start+0x118>
 402f0f4:	9bb77eba 	umull	x26, w21, w23
 402f0f8:	2a1503e0 	mov	w0, w21
 402f0fc:	97fffefe 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402f100:	2a0003f4 	mov	w20, w0
 402f104:	9100e3e2 	add	x2, sp, #0x38
 402f108:	91010341 	add	x1, x26, #0x40
 402f10c:	8b010041 	add	x1, x2, x1
 402f110:	97fffea3 	bl	402eb9c <ddrtrn_hal_get_ck_phase>
 402f114:	97ffee8d 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f118:	2a1503e1 	mov	w1, w21
 402f11c:	9b187c3b 	mul	x27, x1, x24
 402f120:	8b1b0000 	add	x0, x0, x27
 402f124:	b9401000 	ldr	w0, [x0, #16]
 402f128:	7100101f 	cmp	w0, #0x4
 402f12c:	54fffb48 	b.hi	402f094 <ddrtrn_low_freq_start+0x164>  // b.pmore
 402f130:	9100e3e0 	add	x0, sp, #0x38
 402f134:	1108b294 	add	w20, w20, #0x22c
 402f138:	8b1a001a 	add	x26, x0, x26
 402f13c:	52800013 	mov	w19, #0x0                   	// #0
 402f140:	97ffee82 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f144:	8b1b0000 	add	x0, x0, x27
 402f148:	b9401000 	ldr	w0, [x0, #16]
 402f14c:	6b00027f 	cmp	w19, w0
 402f150:	54000063 	b.cc	402f15c <ddrtrn_low_freq_start+0x22c>  // b.lo, b.ul, b.last
 402f154:	110006b5 	add	w21, w21, #0x1
 402f158:	17ffffcc 	b	402f088 <ddrtrn_low_freq_start+0x158>
 402f15c:	2a1403e0 	mov	w0, w20
 402f160:	11000673 	add	w19, w19, #0x1
 402f164:	11020294 	add	w20, w20, #0x80
 402f168:	9100135a 	add	x26, x26, #0x4
 402f16c:	b9400000 	ldr	w0, [x0]
 402f170:	d3506000 	ubfx	x0, x0, #16, #9
 402f174:	b81fc340 	stur	w0, [x26, #-4]
 402f178:	1ad90800 	udiv	w0, w0, w25
 402f17c:	b9001f40 	str	w0, [x26, #28]
 402f180:	17fffff0 	b	402f140 <ddrtrn_low_freq_start+0x210>
 402f184:	97ffee76 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f188:	b9002415 	str	w21, [x0, #36]
 402f18c:	97ffee74 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f190:	b9402014 	ldr	w20, [x0, #32]
 402f194:	97ffee72 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f198:	b9402419 	ldr	w25, [x0, #36]
 402f19c:	97ffee70 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f1a0:	b9400418 	ldr	w24, [x0, #4]
 402f1a4:	97fffedc 	bl	402ed14 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402f1a8:	7100101f 	cmp	w0, #0x4
 402f1ac:	540002c9 	b.ls	402f204 <ddrtrn_low_freq_start+0x2d4>  // b.plast
 402f1b0:	110006b5 	add	w21, w21, #0x1
 402f1b4:	97ffee6a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f1b8:	aa0003f4 	mov	x20, x0
 402f1bc:	97ffee63 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f1c0:	b9402281 	ldr	w1, [x20, #32]
 402f1c4:	9b1a0021 	madd	x1, x1, x26, x0
 402f1c8:	b9400c20 	ldr	w0, [x1, #12]
 402f1cc:	6b0002bf 	cmp	w21, w0
 402f1d0:	54fffda3 	b.cc	402f184 <ddrtrn_low_freq_start+0x254>  // b.lo, b.ul, b.last
 402f1d4:	9100c3e0 	add	x0, sp, #0x30
 402f1d8:	52800021 	mov	w1, #0x1                   	// #1
 402f1dc:	97ffff23 	bl	402ee68 <ddrtrn_hal_low_freq_ctrl_easr>
 402f1e0:	340008e0 	cbz	w0, 402f2fc <ddrtrn_low_freq_start+0x3cc>
 402f1e4:	12800000 	mov	w0, #0xffffffff            	// #-1
 402f1e8:	d0ffff81 	adrp	x1, 4021000 <gs_misc_record-0xa80>
 402f1ec:	f94127e2 	ldr	x2, [sp, #584]
 402f1f0:	f9477023 	ldr	x3, [x1, #3808]
 402f1f4:	eb030042 	subs	x2, x2, x3
 402f1f8:	d2800003 	mov	x3, #0x0                   	// #0
 402f1fc:	54002be0 	b.eq	402f778 <ddrtrn_low_freq_start+0x848>  // b.none
 402f200:	97ffdbff 	bl	40261fc <__stack_chk_fail>
 402f204:	52801200 	mov	w0, #0x90                  	// #144
 402f208:	9100e3e1 	add	x1, sp, #0x38
 402f20c:	11084305 	add	w5, w24, #0x210
 402f210:	52800013 	mov	w19, #0x0                   	// #0
 402f214:	9ba07f24 	umull	x4, w25, w0
 402f218:	52802400 	mov	w0, #0x120                 	// #288
 402f21c:	0b1928bc 	add	w28, w5, w25, lsl #10
 402f220:	9ba01284 	umaddl	x4, w20, w0, x4
 402f224:	9104a3e0 	add	x0, sp, #0x128
 402f228:	8b04001b 	add	x27, x0, x4
 402f22c:	52800a00 	mov	w0, #0x50                  	// #80
 402f230:	aa1b03f7 	mov	x23, x27
 402f234:	9ba00694 	umaddl	x20, w20, w0, x1
 402f238:	97fffeb7 	bl	402ed14 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402f23c:	6b00027f 	cmp	w19, w0
 402f240:	54fffb82 	b.cs	402f1b0 <ddrtrn_low_freq_start+0x280>  // b.hs, b.nlast
 402f244:	2a1903e2 	mov	w2, w25
 402f248:	2a1803e1 	mov	w1, w24
 402f24c:	aa1703e0 	mov	x0, x23
 402f250:	2a1303e3 	mov	w3, w19
 402f254:	97fff318 	bl	402beb4 <ddrtrn_hal_get_dly_value>
 402f258:	b9400762 	ldr	w2, [x27, #4]
 402f25c:	52800081 	mov	w1, #0x4                   	// #4
 402f260:	11000440 	add	w0, w2, #0x1
 402f264:	1ac10c00 	sdiv	w0, w0, w1
 402f268:	4b000042 	sub	w2, w2, w0
 402f26c:	71003c5f 	cmp	w2, #0xf
 402f270:	54fffa0c 	b.gt	402f1b0 <ddrtrn_low_freq_start+0x280>
 402f274:	b9401b61 	ldr	w1, [x27, #24]
 402f278:	71000c3f 	cmp	w1, #0x3
 402f27c:	54fff9a8 	b.hi	402f1b0 <ddrtrn_low_freq_start+0x280>  // b.pmore
 402f280:	b9400287 	ldr	w7, [x20]
 402f284:	7107fcff 	cmp	w7, #0x1ff
 402f288:	54fff948 	b.hi	402f1b0 <ddrtrn_low_freq_start+0x280>  // b.pmore
 402f28c:	b9402286 	ldr	w6, [x20, #32]
 402f290:	9100937b 	add	x27, x27, #0x24
 402f294:	910092f7 	add	x23, x23, #0x24
 402f298:	91001294 	add	x20, x20, #0x4
 402f29c:	7100a8df 	cmp	w6, #0x2a
 402f2a0:	54fff888 	b.hi	402f1b0 <ddrtrn_low_freq_start+0x280>  // b.pmore
 402f2a4:	b85e4360 	ldur	w0, [x27, #-28]
 402f2a8:	11000673 	add	w19, w19, #0x1
 402f2ac:	1b0200c2 	madd	w2, w6, w2, w0
 402f2b0:	51000420 	sub	w0, w1, #0x1
 402f2b4:	b85f8361 	ldur	w1, [x27, #-8]
 402f2b8:	1b070800 	madd	w0, w0, w7, w2
 402f2bc:	0b010000 	add	w0, w0, w1
 402f2c0:	293f0360 	stp	w0, w0, [x27, #-8]
 402f2c4:	2a1c03e0 	mov	w0, w28
 402f2c8:	b900001f 	str	wzr, [x0]
 402f2cc:	11001380 	add	w0, w28, #0x4
 402f2d0:	b900001f 	str	wzr, [x0]
 402f2d4:	11002380 	add	w0, w28, #0x8
 402f2d8:	b900001f 	str	wzr, [x0]
 402f2dc:	11008380 	add	w0, w28, #0x20
 402f2e0:	b900001f 	str	wzr, [x0]
 402f2e4:	11009380 	add	w0, w28, #0x24
 402f2e8:	1102039c 	add	w28, w28, #0x80
 402f2ec:	b900001f 	str	wzr, [x0]
 402f2f0:	2a1803e0 	mov	w0, w24
 402f2f4:	97fff250 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402f2f8:	17ffffd0 	b	402f238 <ddrtrn_low_freq_start+0x308>
 402f2fc:	52807d15 	mov	w21, #0x3e8                 	// #1000
 402f300:	5284e217 	mov	w23, #0x2710                	// #10000
 402f304:	97fffec0 	bl	402ee04 <ddrtrn_hal_low_freq_phy_clk>
 402f308:	528000e0 	mov	w0, #0x7                   	// #7
 402f30c:	97fffe92 	bl	402ed54 <ddrtrn_hal_low_freq_pll_power>
 402f310:	b9400be0 	ldr	w0, [sp, #8]
 402f314:	52800014 	mov	w20, #0x0                   	// #0
 402f318:	b94013e1 	ldr	w1, [sp, #16]
 402f31c:	97fffe2e 	bl	402ebd4 <ddrtrn_hal_low_freq_cfg_freq_process>
 402f320:	52800000 	mov	w0, #0x0                   	// #0
 402f324:	97fffe8c 	bl	402ed54 <ddrtrn_hal_low_freq_pll_power>
 402f328:	97fffea5 	bl	402edbc <ddrtrn_pll_lock>
 402f32c:	6b17029f 	cmp	w20, w23
 402f330:	35000180 	cbnz	w0, 402f360 <ddrtrn_low_freq_start+0x430>
 402f334:	540000e1 	b.ne	402f350 <ddrtrn_low_freq_start+0x420>  // b.any
 402f338:	710006b5 	subs	w21, w21, #0x1
 402f33c:	54fffe61 	b.ne	402f308 <ddrtrn_low_freq_start+0x3d8>  // b.any
 402f340:	d0000000 	adrp	x0, 4031000 <sync_exception_sp_el0>
 402f344:	913ac400 	add	x0, x0, #0xeb1
 402f348:	97ffe644 	bl	4028c58 <log_serial_puts>
 402f34c:	14000006 	b	402f364 <ddrtrn_low_freq_start+0x434>
 402f350:	5280fa00 	mov	w0, #0x7d0                 	// #2000
 402f354:	11000694 	add	w20, w20, #0x1
 402f358:	97fff4af 	bl	402c614 <ddrtrn_hal_training_delay>
 402f35c:	17fffff3 	b	402f328 <ddrtrn_low_freq_start+0x3f8>
 402f360:	54fffec0 	b.eq	402f338 <ddrtrn_low_freq_start+0x408>  // b.none
 402f364:	52800014 	mov	w20, #0x0                   	// #0
 402f368:	32013fe0 	mov	w0, #0x80007fff            	// #-2147450881
 402f36c:	97fffea6 	bl	402ee04 <ddrtrn_hal_low_freq_phy_clk>
 402f370:	97fffe74 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402f374:	6b14001f 	cmp	w0, w20
 402f378:	540003c8 	b.hi	402f3f0 <ddrtrn_low_freq_start+0x4c0>  // b.pmore
 402f37c:	9100c3e0 	add	x0, sp, #0x30
 402f380:	52800041 	mov	w1, #0x2                   	// #2
 402f384:	97fffeb9 	bl	402ee68 <ddrtrn_hal_low_freq_ctrl_easr>
 402f388:	35fff2e0 	cbnz	w0, 402f1e4 <ddrtrn_low_freq_start+0x2b4>
 402f38c:	528000a0 	mov	w0, #0x5                   	// #5
 402f390:	0a0002c0 	and	w0, w22, w0
 402f394:	97fff1f5 	bl	402bb68 <ddrtrn_hal_hw_item_cfg>
 402f398:	97fffbc1 	bl	402e29c <ddrtrn_hw_training>
 402f39c:	b9400fe1 	ldr	w1, [sp, #12]
 402f3a0:	0b000020 	add	w0, w1, w0
 402f3a4:	b9001be0 	str	w0, [sp, #24]
 402f3a8:	97fffe66 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402f3ac:	7100041f 	cmp	w0, #0x1
 402f3b0:	54000789 	b.ls	402f4a0 <ddrtrn_low_freq_start+0x570>  // b.plast
 402f3b4:	52800015 	mov	w21, #0x0                   	// #0
 402f3b8:	d280029a 	mov	x26, #0x14                  	// #20
 402f3bc:	97fffe61 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402f3c0:	6b0002bf 	cmp	w21, w0
 402f3c4:	54000783 	b.cc	402f4b4 <ddrtrn_low_freq_start+0x584>  // b.lo, b.ul, b.last
 402f3c8:	910223e0 	add	x0, sp, #0x88
 402f3cc:	940005a2 	bl	4030a54 <ddrtrn_restore_rdqbdl_phy>
 402f3d0:	529d7ea0 	mov	w0, #0xebf5                	// #60405
 402f3d4:	72bffb60 	movk	w0, #0xffdb, lsl #16
 402f3d8:	0a0002c0 	and	w0, w22, w0
 402f3dc:	97fff1e3 	bl	402bb68 <ddrtrn_hal_hw_item_cfg>
 402f3e0:	97fffbaf 	bl	402e29c <ddrtrn_hw_training>
 402f3e4:	b9401be1 	ldr	w1, [sp, #24]
 402f3e8:	0b000020 	add	w0, w1, w0
 402f3ec:	17ffff7f 	b	402f1e8 <ddrtrn_low_freq_start+0x2b8>
 402f3f0:	2a1403e0 	mov	w0, w20
 402f3f4:	11000694 	add	w20, w20, #0x1
 402f3f8:	97fffe3f 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402f3fc:	97fff21c 	bl	402bc6c <ddrtrn_hal_ck_cfg>
 402f400:	17ffffdc 	b	402f370 <ddrtrn_low_freq_start+0x440>
 402f404:	9bb77eb3 	umull	x19, w21, w23
 402f408:	2a1503e0 	mov	w0, w21
 402f40c:	2a1503fa 	mov	w26, w21
 402f410:	97fffe39 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402f414:	9100e3e2 	add	x2, sp, #0x38
 402f418:	91012261 	add	x1, x19, #0x48
 402f41c:	8b010041 	add	x1, x2, x1
 402f420:	2a0003f4 	mov	w20, w0
 402f424:	9b187f5a 	mul	x26, x26, x24
 402f428:	97fffddd 	bl	402eb9c <ddrtrn_hal_get_ck_phase>
 402f42c:	97ffedc7 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f430:	8b1a0000 	add	x0, x0, x26
 402f434:	b9401000 	ldr	w0, [x0, #16]
 402f438:	7100101f 	cmp	w0, #0x4
 402f43c:	54fffbc8 	b.hi	402f3b4 <ddrtrn_low_freq_start+0x484>  // b.pmore
 402f440:	9100e3e0 	add	x0, sp, #0x38
 402f444:	1108b294 	add	w20, w20, #0x22c
 402f448:	8b130013 	add	x19, x0, x19
 402f44c:	5280001b 	mov	w27, #0x0                   	// #0
 402f450:	97ffedbe 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f454:	8b1a0000 	add	x0, x0, x26
 402f458:	91001273 	add	x19, x19, #0x4
 402f45c:	b9401000 	ldr	w0, [x0, #16]
 402f460:	6b00037f 	cmp	w27, w0
 402f464:	540000c3 	b.cc	402f47c <ddrtrn_low_freq_start+0x54c>  // b.lo, b.ul, b.last
 402f468:	110006b5 	add	w21, w21, #0x1
 402f46c:	97fffe35 	bl	402ed40 <ddrtrn_hal_get_phy_num>
 402f470:	6b0002bf 	cmp	w21, w0
 402f474:	54fffc83 	b.cc	402f404 <ddrtrn_low_freq_start+0x4d4>  // b.lo, b.ul, b.last
 402f478:	17ffffcf 	b	402f3b4 <ddrtrn_low_freq_start+0x484>
 402f47c:	2a1403e0 	mov	w0, w20
 402f480:	1100077b 	add	w27, w27, #0x1
 402f484:	11020294 	add	w20, w20, #0x80
 402f488:	b9400000 	ldr	w0, [x0]
 402f48c:	d3506000 	ubfx	x0, x0, #16, #9
 402f490:	b9000e60 	str	w0, [x19, #12]
 402f494:	1ad90800 	udiv	w0, w0, w25
 402f498:	b9002e60 	str	w0, [x19, #44]
 402f49c:	17ffffed 	b	402f450 <ddrtrn_low_freq_start+0x520>
 402f4a0:	52800015 	mov	w21, #0x0                   	// #0
 402f4a4:	52800a17 	mov	w23, #0x50                  	// #80
 402f4a8:	d2800798 	mov	x24, #0x3c                  	// #60
 402f4ac:	52800199 	mov	w25, #0xc                   	// #12
 402f4b0:	17ffffef 	b	402f46c <ddrtrn_low_freq_start+0x53c>
 402f4b4:	97ffedaa 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f4b8:	b9002015 	str	w21, [x0, #32]
 402f4bc:	2a1503e0 	mov	w0, w21
 402f4c0:	97fffe0d 	bl	402ecf4 <ddrtrn_hal_get_phy_addr>
 402f4c4:	2a0003f3 	mov	w19, w0
 402f4c8:	97ffeda5 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f4cc:	b9000413 	str	w19, [x0, #4]
 402f4d0:	97ffed9e 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f4d4:	2a1503e1 	mov	w1, w21
 402f4d8:	d2800782 	mov	x2, #0x3c                  	// #60
 402f4dc:	9b020021 	madd	x1, x1, x2, x0
 402f4e0:	b9403020 	ldr	w0, [x1, #48]
 402f4e4:	35001460 	cbnz	w0, 402f770 <ddrtrn_low_freq_start+0x840>
 402f4e8:	110006b5 	add	w21, w21, #0x1
 402f4ec:	17ffffb4 	b	402f3bc <ddrtrn_low_freq_start+0x48c>
 402f4f0:	97ffed9b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f4f4:	b9002419 	str	w25, [x0, #36]
 402f4f8:	97ffed99 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f4fc:	b940201b 	ldr	w27, [x0, #32]
 402f500:	97ffed97 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f504:	b9402413 	ldr	w19, [x0, #36]
 402f508:	97fffe03 	bl	402ed14 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402f50c:	7100101f 	cmp	w0, #0x4
 402f510:	54000228 	b.hi	402f554 <ddrtrn_low_freq_start+0x624>  // b.pmore
 402f514:	52801200 	mov	w0, #0x90                  	// #144
 402f518:	9100e3f8 	add	x24, sp, #0x38
 402f51c:	52800a14 	mov	w20, #0x50                  	// #80
 402f520:	52800017 	mov	w23, #0x0                   	// #0
 402f524:	9ba07e60 	umull	x0, w19, w0
 402f528:	52802413 	mov	w19, #0x120                 	// #288
 402f52c:	9bb46374 	umaddl	x20, w27, w20, x24
 402f530:	9bb30373 	umaddl	x19, w27, w19, x0
 402f534:	2a1b03fb 	mov	w27, w27
 402f538:	9104a3e0 	add	x0, sp, #0x128
 402f53c:	8b130013 	add	x19, x0, x19
 402f540:	9b1a7f60 	mul	x0, x27, x26
 402f544:	f90013e0 	str	x0, [sp, #32]
 402f548:	97fffdf3 	bl	402ed14 <ddrtrn_hal_get_cur_phy_total_byte_num>
 402f54c:	6b0002ff 	cmp	w23, w0
 402f550:	54000183 	b.cc	402f580 <ddrtrn_low_freq_start+0x650>  // b.lo, b.ul, b.last
 402f554:	11000739 	add	w25, w25, #0x1
 402f558:	97ffed81 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f55c:	aa0003f3 	mov	x19, x0
 402f560:	97ffed7a 	bl	402ab48 <ddrtrn_hal_get_phy>
 402f564:	d2800782 	mov	x2, #0x3c                  	// #60
 402f568:	b9402261 	ldr	w1, [x19, #32]
 402f56c:	9b020021 	madd	x1, x1, x2, x0
 402f570:	b9400c20 	ldr	w0, [x1, #12]
 402f574:	6b00033f 	cmp	w25, w0
 402f578:	54fffbc3 	b.cc	402f4f0 <ddrtrn_low_freq_start+0x5c0>  // b.lo, b.ul, b.last
 402f57c:	17ffffdb 	b	402f4e8 <ddrtrn_low_freq_start+0x5b8>
 402f580:	97ffed77 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f584:	b9001817 	str	w23, [x0, #24]
 402f588:	f94013e0 	ldr	x0, [sp, #32]
 402f58c:	710006ff 	cmp	w23, #0x1
 402f590:	9a809401 	cinc	x1, x0, hi  // hi = pmore
 402f594:	91004020 	add	x0, x1, #0x10
 402f598:	91004821 	add	x1, x1, #0x12
 402f59c:	b8607b02 	ldr	w2, [x24, x0, lsl #2]
 402f5a0:	b8617b01 	ldr	w1, [x24, x1, lsl #2]
 402f5a4:	11000440 	add	w0, w2, #0x1
 402f5a8:	4b400840 	sub	w0, w2, w0, lsr #2
 402f5ac:	11000422 	add	w2, w1, #0x1
 402f5b0:	4b420822 	sub	w2, w1, w2, lsr #2
 402f5b4:	71003c1f 	cmp	w0, #0xf
 402f5b8:	54fffce8 	b.hi	402f554 <ddrtrn_low_freq_start+0x624>  // b.pmore
 402f5bc:	71003c5f 	cmp	w2, #0xf
 402f5c0:	54fffca8 	b.hi	402f554 <ddrtrn_low_freq_start+0x624>  // b.pmore
 402f5c4:	b9402283 	ldr	w3, [x20, #32]
 402f5c8:	7100a87f 	cmp	w3, #0x2a
 402f5cc:	54fffc48 	b.hi	402f554 <ddrtrn_low_freq_start+0x624>  // b.pmore
 402f5d0:	b9403281 	ldr	w1, [x20, #48]
 402f5d4:	7100a83f 	cmp	w1, #0x2a
 402f5d8:	54fffbe8 	b.hi	402f554 <ddrtrn_low_freq_start+0x624>  // b.pmore
 402f5dc:	b9402264 	ldr	w4, [x19, #32]
 402f5e0:	5280003b 	mov	w27, #0x1                   	// #1
 402f5e4:	1b011041 	madd	w1, w2, w1, w4
 402f5e8:	1b038401 	msub	w1, w0, w3, w1
 402f5ec:	b9401283 	ldr	w3, [x20, #16]
 402f5f0:	12800040 	mov	w0, #0xfffffffd            	// #-3
 402f5f4:	1ac00c60 	sdiv	w0, w3, w0
 402f5f8:	0b010000 	add	w0, w0, w1
 402f5fc:	6a407f7f 	tst	w27, w0, lsr #31
 402f600:	540008c1 	b.ne	402f718 <ddrtrn_low_freq_start+0x7e8>  // b.any
 402f604:	6b00007f 	cmp	w3, w0
 402f608:	5400006c 	b.gt	402f614 <ddrtrn_low_freq_start+0x6e4>
 402f60c:	71000b7f 	cmp	w27, #0x2
 402f610:	540008a1 	b.ne	402f724 <ddrtrn_low_freq_start+0x7f4>  // b.any
 402f614:	7101fc1f 	cmp	w0, #0x7f
 402f618:	52800fe1 	mov	w1, #0x7f                  	// #127
 402f61c:	1a81d01c 	csel	w28, w0, w1, le
 402f620:	97ffed4f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f624:	b9400400 	ldr	w0, [x0, #4]
 402f628:	b9000fe0 	str	w0, [sp, #12]
 402f62c:	97ffed4c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f630:	b9402400 	ldr	w0, [x0, #36]
 402f634:	b9002be0 	str	w0, [sp, #40]
 402f638:	97ffed49 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f63c:	b9401800 	ldr	w0, [x0, #24]
 402f640:	b9000be0 	str	w0, [sp, #8]
 402f644:	7100039f 	cmp	w28, #0x0
 402f648:	b9400260 	ldr	w0, [x19]
 402f64c:	1a9fa381 	csel	w1, w28, wzr, ge  // ge = tcont
 402f650:	b9001fe0 	str	w0, [sp, #28]
 402f654:	5280001c 	mov	w28, #0x0                   	// #0
 402f658:	b9400be0 	ldr	w0, [sp, #8]
 402f65c:	f9000be0 	str	x0, [sp, #16]
 402f660:	b9002fe1 	str	w1, [sp, #44]
 402f664:	97ffed3e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f668:	b9402000 	ldr	w0, [x0, #32]
 402f66c:	f9400be1 	ldr	x1, [sp, #16]
 402f670:	9b1a0400 	madd	x0, x0, x26, x1
 402f674:	b9402fe1 	ldr	w1, [sp, #44]
 402f678:	91003000 	add	x0, x0, #0xc
 402f67c:	b8607b00 	ldr	w0, [x24, x0, lsl #2]
 402f680:	6b00003f 	cmp	w1, w0
 402f684:	54000069 	b.ls	402f690 <ddrtrn_low_freq_start+0x760>  // b.plast
 402f688:	7100179f 	cmp	w28, #0x5
 402f68c:	54000529 	b.ls	402f730 <ddrtrn_low_freq_start+0x800>  // b.plast
 402f690:	b9401fe0 	ldr	w0, [sp, #28]
 402f694:	110006f7 	add	w23, w23, #0x1
 402f698:	b9402be2 	ldr	w2, [sp, #40]
 402f69c:	91001294 	add	x20, x20, #0x4
 402f6a0:	12196000 	and	w0, w0, #0xffffff80
 402f6a4:	91009273 	add	x19, x19, #0x24
 402f6a8:	2a000021 	orr	w1, w1, w0
 402f6ac:	b9400be0 	ldr	w0, [sp, #8]
 402f6b0:	12146c21 	and	w1, w1, #0xfffff0ff
 402f6b4:	2a1c2021 	orr	w1, w1, w28, lsl #8
 402f6b8:	53196000 	lsl	w0, w0, #7
 402f6bc:	0b022800 	add	w0, w0, w2, lsl #10
 402f6c0:	b9400fe2 	ldr	w2, [sp, #12]
 402f6c4:	0b020000 	add	w0, w0, w2
 402f6c8:	1108c003 	add	w3, w0, #0x230
 402f6cc:	b9000061 	str	w1, [x3]
 402f6d0:	b85f0261 	ldur	w1, [x19, #-16]
 402f6d4:	120e7421 	and	w1, w1, #0xfffcffff
 402f6d8:	2a1b4022 	orr	w2, w1, w27, lsl #16
 402f6dc:	1108e001 	add	w1, w0, #0x238
 402f6e0:	b9000022 	str	w2, [x1]
 402f6e4:	1108d002 	add	w2, w0, #0x234
 402f6e8:	b85e8261 	ldur	w1, [x19, #-24]
 402f6ec:	12126421 	and	w1, w1, #0xffffc0ff
 402f6f0:	b9000041 	str	w1, [x2]
 402f6f4:	11084001 	add	w1, w0, #0x210
 402f6f8:	b900003f 	str	wzr, [x1]
 402f6fc:	11085001 	add	w1, w0, #0x214
 402f700:	11086000 	add	w0, w0, #0x218
 402f704:	b900003f 	str	wzr, [x1]
 402f708:	b900001f 	str	wzr, [x0]
 402f70c:	b9400fe0 	ldr	w0, [sp, #12]
 402f710:	97fff149 	bl	402bc34 <ddrtrn_hal_phy_cfg_update>
 402f714:	17ffff8d 	b	402f548 <ddrtrn_low_freq_start+0x618>
 402f718:	0b030000 	add	w0, w0, w3
 402f71c:	5280001b 	mov	w27, #0x0                   	// #0
 402f720:	17ffffb7 	b	402f5fc <ddrtrn_low_freq_start+0x6cc>
 402f724:	4b030000 	sub	w0, w0, w3
 402f728:	1100077b 	add	w27, w27, #0x1
 402f72c:	17ffffb6 	b	402f604 <ddrtrn_low_freq_start+0x6d4>
 402f730:	b9002fe1 	str	w1, [sp, #44]
 402f734:	97ffed0a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f738:	b9402000 	ldr	w0, [x0, #32]
 402f73c:	f9400be1 	ldr	x1, [sp, #16]
 402f740:	9b1a0400 	madd	x0, x0, x26, x1
 402f744:	b9402fe1 	ldr	w1, [sp, #44]
 402f748:	91003000 	add	x0, x0, #0xc
 402f74c:	b8607b00 	ldr	w0, [x24, x0, lsl #2]
 402f750:	4b000021 	sub	w1, w1, w0
 402f754:	11000780 	add	w0, w28, #0x1
 402f758:	12000404 	and	w4, w0, #0x3
 402f75c:	71000c9f 	cmp	w4, #0x3
 402f760:	54000041 	b.ne	402f768 <ddrtrn_low_freq_start+0x838>  // b.any
 402f764:	11000b80 	add	w0, w28, #0x2
 402f768:	2a0003fc 	mov	w28, w0
 402f76c:	17ffffbd 	b	402f660 <ddrtrn_low_freq_start+0x730>
 402f770:	52800019 	mov	w25, #0x0                   	// #0
 402f774:	17ffff79 	b	402f558 <ddrtrn_low_freq_start+0x628>
 402f778:	910943ff 	add	sp, sp, #0x250
 402f77c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402f780:	a94263f7 	ldp	x23, x24, [sp, #32]
 402f784:	a9436bf9 	ldp	x25, x26, [sp, #48]
 402f788:	a94473fb 	ldp	x27, x28, [sp, #64]
 402f78c:	f9402bfe 	ldr	x30, [sp, #80]
 402f790:	a8c653f3 	ldp	x19, x20, [sp], #96
 402f794:	d65f03c0 	ret

000000000402f798 <ddrtrn_suspend_store_para>:
 402f798:	d65f03c0 	ret

000000000402f79c <ddrtrn_resume>:
 402f79c:	52800000 	mov	w0, #0x0                   	// #0
 402f7a0:	d65f03c0 	ret

000000000402f7a4 <ddrtrn_hal_dmc_auto_pd_by_phy>:
 402f7a4:	1100a021 	add	w1, w1, #0x28
 402f7a8:	b9400023 	ldr	w3, [x1]
 402f7ac:	f27c2c7f 	tst	x3, #0xfff0
 402f7b0:	54000180 	b.eq	402f7e0 <ddrtrn_hal_dmc_auto_pd_by_phy+0x3c>  // b.none
 402f7b4:	1100b000 	add	w0, w0, #0x2c
 402f7b8:	b9400004 	ldr	w4, [x0]
 402f7bc:	32000060 	orr	w0, w3, #0x1
 402f7c0:	b9000020 	str	w0, [x1]
 402f7c4:	12000c83 	and	w3, w4, #0xf
 402f7c8:	7100187f 	cmp	w3, #0x6
 402f7cc:	540000a1 	b.ne	402f7e0 <ddrtrn_hal_dmc_auto_pd_by_phy+0x3c>  // b.any
 402f7d0:	1100a042 	add	w2, w2, #0x28
 402f7d4:	b9400040 	ldr	w0, [x2]
 402f7d8:	32000000 	orr	w0, w0, #0x1
 402f7dc:	b9000040 	str	w0, [x2]
 402f7e0:	d65f03c0 	ret

000000000402f7e4 <ddrtrn_ddrt_check>:
 402f7e4:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402f7e8:	a9017bf5 	stp	x21, x30, [sp, #16]
 402f7ec:	97ffecdc 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f7f0:	b9401813 	ldr	w19, [x0, #24]
 402f7f4:	97ffecda 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f7f8:	b9401c14 	ldr	w20, [x0, #28]
 402f7fc:	97ffecd8 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f800:	b9401815 	ldr	w21, [x0, #24]
 402f804:	97ffecd6 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f808:	b9402800 	ldr	w0, [x0, #40]
 402f80c:	6b0006bf 	cmp	w21, w0, lsl #1
 402f810:	540000c3 	b.cc	402f828 <ddrtrn_ddrt_check+0x44>  // b.lo, b.ul, b.last
 402f814:	97ffecd2 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f818:	b9401815 	ldr	w21, [x0, #24]
 402f81c:	97ffecd0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f820:	b9402813 	ldr	w19, [x0, #40]
 402f824:	4b1306b3 	sub	w19, w21, w19, lsl #1
 402f828:	52800801 	mov	w1, #0x40                  	// #64
 402f82c:	528330c0 	mov	w0, #0x1986                	// #6534
 402f830:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f834:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f838:	97fff079 	bl	402ba1c <ddrt_reg_write>
 402f83c:	52800881 	mov	w1, #0x44                  	// #68
 402f840:	528330c0 	mov	w0, #0x1986                	// #6534
 402f844:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f848:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f84c:	97fff074 	bl	402ba1c <ddrt_reg_write>
 402f850:	52800901 	mov	w1, #0x48                  	// #72
 402f854:	528330c0 	mov	w0, #0x1986                	// #6534
 402f858:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f85c:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f860:	97fff06f 	bl	402ba1c <ddrt_reg_write>
 402f864:	52800981 	mov	w1, #0x4c                  	// #76
 402f868:	528330c0 	mov	w0, #0x1986                	// #6534
 402f86c:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f870:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f874:	97fff06a 	bl	402ba1c <ddrt_reg_write>
 402f878:	52800a01 	mov	w1, #0x50                  	// #80
 402f87c:	528330c0 	mov	w0, #0x1986                	// #6534
 402f880:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f884:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f888:	97fff065 	bl	402ba1c <ddrt_reg_write>
 402f88c:	52800a81 	mov	w1, #0x54                  	// #84
 402f890:	528330c0 	mov	w0, #0x1986                	// #6534
 402f894:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f898:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f89c:	97fff060 	bl	402ba1c <ddrt_reg_write>
 402f8a0:	52800b01 	mov	w1, #0x58                  	// #88
 402f8a4:	528330c0 	mov	w0, #0x1986                	// #6534
 402f8a8:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f8ac:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f8b0:	97fff05b 	bl	402ba1c <ddrt_reg_write>
 402f8b4:	52800b81 	mov	w1, #0x5c                  	// #92
 402f8b8:	528330c0 	mov	w0, #0x1986                	// #6534
 402f8bc:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f8c0:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f8c4:	97fff056 	bl	402ba1c <ddrt_reg_write>
 402f8c8:	52800601 	mov	w1, #0x30                  	// #48
 402f8cc:	52800000 	mov	w0, #0x0                   	// #0
 402f8d0:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f8d4:	97fff052 	bl	402ba1c <ddrt_reg_write>
 402f8d8:	2a1403e2 	mov	w2, w20
 402f8dc:	2a1303e1 	mov	w1, w19
 402f8e0:	52800000 	mov	w0, #0x0                   	// #0
 402f8e4:	97fff945 	bl	402ddf8 <ddrtrn_ddrt_test>
 402f8e8:	340000a0 	cbz	w0, 402f8fc <ddrtrn_ddrt_check+0x118>
 402f8ec:	12800000 	mov	w0, #0xffffffff            	// #-1
 402f8f0:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402f8f4:	a8c253f3 	ldp	x19, x20, [sp], #32
 402f8f8:	d65f03c0 	ret
 402f8fc:	97ffec98 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f900:	b9401000 	ldr	w0, [x0, #16]
 402f904:	52800601 	mov	w1, #0x30                  	// #48
 402f908:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f90c:	97fff044 	bl	402ba1c <ddrt_reg_write>
 402f910:	2a1403e2 	mov	w2, w20
 402f914:	2a1303e1 	mov	w1, w19
 402f918:	52800000 	mov	w0, #0x0                   	// #0
 402f91c:	97fff937 	bl	402ddf8 <ddrtrn_ddrt_test>
 402f920:	35fffe60 	cbnz	w0, 402f8ec <ddrtrn_ddrt_check+0x108>
 402f924:	52800801 	mov	w1, #0x40                  	// #64
 402f928:	528330c0 	mov	w0, #0x1986                	// #6534
 402f92c:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f930:	72a224a0 	movk	w0, #0x1125, lsl #16
 402f934:	97fff03a 	bl	402ba1c <ddrt_reg_write>
 402f938:	52800881 	mov	w1, #0x44                  	// #68
 402f93c:	5284a320 	mov	w0, #0x2519                	// #9497
 402f940:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f944:	72b6c220 	movk	w0, #0xb611, lsl #16
 402f948:	97fff035 	bl	402ba1c <ddrt_reg_write>
 402f94c:	52800901 	mov	w1, #0x48                  	// #72
 402f950:	528224a0 	mov	w0, #0x1125                	// #4389
 402f954:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f958:	72b756c0 	movk	w0, #0xbab6, lsl #16
 402f95c:	97fff030 	bl	402ba1c <ddrt_reg_write>
 402f960:	52800981 	mov	w1, #0x4c                  	// #76
 402f964:	5296c220 	mov	w0, #0xb611                	// #46609
 402f968:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f96c:	72a03740 	movk	w0, #0x1ba, lsl #16
 402f970:	97fff02b 	bl	402ba1c <ddrt_reg_write>
 402f974:	52800a01 	mov	w1, #0x50                  	// #80
 402f978:	529756c0 	mov	w0, #0xbab6                	// #47798
 402f97c:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f980:	72ba6020 	movk	w0, #0xd301, lsl #16
 402f984:	97fff026 	bl	402ba1c <ddrt_reg_write>
 402f988:	52800a81 	mov	w1, #0x54                  	// #84
 402f98c:	52803740 	mov	w0, #0x1ba                 	// #442
 402f990:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f994:	72bc1a60 	movk	w0, #0xe0d3, lsl #16
 402f998:	97fff021 	bl	402ba1c <ddrt_reg_write>
 402f99c:	52800b01 	mov	w1, #0x58                  	// #88
 402f9a0:	529a6020 	mov	w0, #0xd301                	// #54017
 402f9a4:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f9a8:	72b1bc00 	movk	w0, #0x8de0, lsl #16
 402f9ac:	97fff01c 	bl	402ba1c <ddrt_reg_write>
 402f9b0:	52800b81 	mov	w1, #0x5c                  	// #92
 402f9b4:	529c1a60 	mov	w0, #0xe0d3                	// #57555
 402f9b8:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f9bc:	72ac31a0 	movk	w0, #0x618d, lsl #16
 402f9c0:	97fff017 	bl	402ba1c <ddrt_reg_write>
 402f9c4:	52800601 	mov	w1, #0x30                  	// #48
 402f9c8:	52800000 	mov	w0, #0x0                   	// #0
 402f9cc:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f9d0:	97fff013 	bl	402ba1c <ddrt_reg_write>
 402f9d4:	2a1403e2 	mov	w2, w20
 402f9d8:	2a1303e1 	mov	w1, w19
 402f9dc:	52800000 	mov	w0, #0x0                   	// #0
 402f9e0:	97fff906 	bl	402ddf8 <ddrtrn_ddrt_test>
 402f9e4:	35fff840 	cbnz	w0, 402f8ec <ddrtrn_ddrt_check+0x108>
 402f9e8:	97ffec5d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402f9ec:	b9401000 	ldr	w0, [x0, #16]
 402f9f0:	52800601 	mov	w1, #0x30                  	// #48
 402f9f4:	72a222c1 	movk	w1, #0x1116, lsl #16
 402f9f8:	97fff009 	bl	402ba1c <ddrt_reg_write>
 402f9fc:	2a1403e2 	mov	w2, w20
 402fa00:	2a1303e1 	mov	w1, w19
 402fa04:	52800000 	mov	w0, #0x0                   	// #0
 402fa08:	97fff8fc 	bl	402ddf8 <ddrtrn_ddrt_test>
 402fa0c:	7100001f 	cmp	w0, #0x0
 402fa10:	5a9f03e0 	csetm	w0, ne  // ne = any
 402fa14:	17ffffb7 	b	402f8f0 <ddrtrn_ddrt_check+0x10c>

000000000402fa18 <ddrtrn_hal_dataeye_get_dm>:
 402fa18:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402fa1c:	f9000bfe 	str	x30, [sp, #16]
 402fa20:	97ffec4f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fa24:	b9401814 	ldr	w20, [x0, #24]
 402fa28:	97ffec4d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fa2c:	b9400400 	ldr	w0, [x0, #4]
 402fa30:	11086013 	add	w19, w0, #0x218
 402fa34:	53196294 	lsl	w20, w20, #7
 402fa38:	97ffec49 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fa3c:	b9402400 	ldr	w0, [x0, #36]
 402fa40:	f9400bfe 	ldr	x30, [sp, #16]
 402fa44:	0b002a80 	add	w0, w20, w0, lsl #10
 402fa48:	0b130000 	add	w0, w0, w19
 402fa4c:	a8c253f3 	ldp	x19, x20, [sp], #32
 402fa50:	b9400000 	ldr	w0, [x0]
 402fa54:	12001800 	and	w0, w0, #0x7f
 402fa58:	d65f03c0 	ret

000000000402fa5c <ddrtrn_hal_dataeye_set_dq_sum>:
 402fa5c:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402fa60:	2a0003f3 	mov	w19, w0
 402fa64:	12001a73 	and	w19, w19, #0x7f
 402fa68:	a9017bf5 	stp	x21, x30, [sp, #16]
 402fa6c:	97ffec3c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fa70:	b9401815 	ldr	w21, [x0, #24]
 402fa74:	97ffec3a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fa78:	b9400400 	ldr	w0, [x0, #4]
 402fa7c:	11086014 	add	w20, w0, #0x218
 402fa80:	531962b5 	lsl	w21, w21, #7
 402fa84:	97ffec36 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fa88:	b9402400 	ldr	w0, [x0, #36]
 402fa8c:	0b002aa0 	add	w0, w21, w0, lsl #10
 402fa90:	0b140000 	add	w0, w0, w20
 402fa94:	a9417bf5 	ldp	x21, x30, [sp, #16]
 402fa98:	b9000013 	str	w19, [x0]
 402fa9c:	a8c253f3 	ldp	x19, x20, [sp], #32
 402faa0:	d65f03c0 	ret

000000000402faa4 <ddrtrn_hal_get_dq_type>:
 402faa4:	d2801880 	mov	x0, #0xc4                  	// #196
 402faa8:	f2a22040 	movk	x0, #0x1102, lsl #16
 402faac:	b9400000 	ldr	w0, [x0]
 402fab0:	d65f03c0 	ret

000000000402fab4 <ddrtrn_hal_set_dq_type>:
 402fab4:	d2801881 	mov	x1, #0xc4                  	// #196
 402fab8:	f2a22041 	movk	x1, #0x1102, lsl #16
 402fabc:	b9000020 	str	w0, [x1]
 402fac0:	d65f03c0 	ret

000000000402fac4 <ddrtrn_hal_dcc_get_ioctl21>:
 402fac4:	f81f0ffe 	str	x30, [sp, #-16]!
 402fac8:	97ffec25 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402facc:	b9400400 	ldr	w0, [x0, #4]
 402fad0:	52821481 	mov	w1, #0x10a4                	// #4260
 402fad4:	0b010000 	add	w0, w0, w1
 402fad8:	f84107fe 	ldr	x30, [sp], #16
 402fadc:	b9400000 	ldr	w0, [x0]
 402fae0:	d65f03c0 	ret

000000000402fae4 <ddrtrn_hal_dcc_set_ioctl21>:
 402fae4:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402fae8:	2a0003f3 	mov	w19, w0
 402faec:	97ffec1c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402faf0:	b9400401 	ldr	w1, [x0, #4]
 402faf4:	52821480 	mov	w0, #0x10a4                	// #4260
 402faf8:	0b000021 	add	w1, w1, w0
 402fafc:	b9000033 	str	w19, [x1]
 402fb00:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402fb04:	d65f03c0 	ret

000000000402fb08 <ddrtrn_hal_dcc_get_gated_bypass>:
 402fb08:	17ffffef 	b	402fac4 <ddrtrn_hal_dcc_get_ioctl21>

000000000402fb0c <ddrtrn_hal_dcc_set_gated_bypass>:
 402fb0c:	17fffff6 	b	402fae4 <ddrtrn_hal_dcc_set_ioctl21>

000000000402fb10 <ddrtrn_hal_dcc_rdet_enable>:
 402fb10:	f81f0ffe 	str	x30, [sp, #-16]!
 402fb14:	97ffec12 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fb18:	b9400400 	ldr	w0, [x0, #4]
 402fb1c:	52802001 	mov	w1, #0x100                 	// #256
 402fb20:	11002000 	add	w0, w0, #0x8
 402fb24:	f84107fe 	ldr	x30, [sp], #16
 402fb28:	b9000001 	str	w1, [x0]
 402fb2c:	d65f03c0 	ret

000000000402fb30 <ddrtrn_hal_dcc_get_dxnrdbound>:
 402fb30:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402fb34:	2a0003f3 	mov	w19, w0
 402fb38:	97ffec09 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fb3c:	53196273 	lsl	w19, w19, #7
 402fb40:	b9400400 	ldr	w0, [x0, #4]
 402fb44:	11094273 	add	w19, w19, #0x250
 402fb48:	0b000273 	add	w19, w19, w0
 402fb4c:	b9400260 	ldr	w0, [x19]
 402fb50:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402fb54:	d65f03c0 	ret

000000000402fb58 <ddrtrn_hal_get_cur_phy>:
 402fb58:	f81f0ffe 	str	x30, [sp, #-16]!
 402fb5c:	97ffec00 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fb60:	b9400400 	ldr	w0, [x0, #4]
 402fb64:	f84107fe 	ldr	x30, [sp], #16
 402fb68:	d65f03c0 	ret

000000000402fb6c <ddrtrn_hal_get_cur_phy_total_byte_num>:
 402fb6c:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 402fb70:	97ffebfb 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fb74:	aa0003f3 	mov	x19, x0
 402fb78:	97ffebf4 	bl	402ab48 <ddrtrn_hal_get_phy>
 402fb7c:	d2800782 	mov	x2, #0x3c                  	// #60
 402fb80:	b9402261 	ldr	w1, [x19, #32]
 402fb84:	a8c17bf3 	ldp	x19, x30, [sp], #16
 402fb88:	d503201f 	nop
 402fb8c:	9b020021 	madd	x1, x1, x2, x0
 402fb90:	b9401020 	ldr	w0, [x1, #16]
 402fb94:	d65f03c0 	ret

000000000402fb98 <ddrtrn_hal_set_rank1_wdq_to_rank0>:
 402fb98:	1118c002 	add	w2, w0, #0x630
 402fb9c:	52800003 	mov	w3, #0x0                   	// #0
 402fba0:	6b01007f 	cmp	w3, w1
 402fba4:	54000041 	b.ne	402fbac <ddrtrn_hal_set_rank1_wdq_to_rank0+0x14>  // b.any
 402fba8:	17fff023 	b	402bc34 <ddrtrn_hal_phy_cfg_update>
 402fbac:	2a0203e4 	mov	w4, w2
 402fbb0:	11000463 	add	w3, w3, #0x1
 402fbb4:	b9400085 	ldr	w5, [x4]
 402fbb8:	51100044 	sub	w4, w2, #0x400
 402fbbc:	b9000085 	str	w5, [x4]
 402fbc0:	11001044 	add	w4, w2, #0x4
 402fbc4:	b9400085 	ldr	w5, [x4]
 402fbc8:	510ff044 	sub	w4, w2, #0x3fc
 402fbcc:	11020042 	add	w2, w2, #0x80
 402fbd0:	b9000085 	str	w5, [x4]
 402fbd4:	17fffff3 	b	402fba0 <ddrtrn_hal_set_rank1_wdq_to_rank0+0x8>

000000000402fbd8 <ddrtrn_hal_hw_read_adj>:
 402fbd8:	d65f03c0 	ret

000000000402fbdc <ddrtrn_hal_hw_rdqs_offset_cfg>:
 402fbdc:	1108b000 	add	w0, w0, #0x22c
 402fbe0:	52800004 	mov	w4, #0x0                   	// #0
 402fbe4:	6b01009f 	cmp	w4, w1
 402fbe8:	54000041 	b.ne	402fbf0 <ddrtrn_hal_hw_rdqs_offset_cfg+0x14>  // b.any
 402fbec:	d65f03c0 	ret
 402fbf0:	2a0003e5 	mov	w5, w0
 402fbf4:	b94000a3 	ldr	w3, [x5]
 402fbf8:	7100185f 	cmp	w2, #0x6
 402fbfc:	540000a1 	b.ne	402fc10 <ddrtrn_hal_hw_rdqs_offset_cfg+0x34>  // b.any
 402fc00:	11000484 	add	w4, w4, #0x1
 402fc04:	11020000 	add	w0, w0, #0x80
 402fc08:	b90000a3 	str	w3, [x5]
 402fc0c:	17fffff6 	b	402fbe4 <ddrtrn_hal_hw_rdqs_offset_cfg+0x8>
 402fc10:	11002863 	add	w3, w3, #0xa
 402fc14:	17fffffb 	b	402fc00 <ddrtrn_hal_hw_rdqs_offset_cfg+0x24>

000000000402fc18 <ddrtrn_hal_training_get_rdqs>:
 402fc18:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402fc1c:	aa0003f4 	mov	x20, x0
 402fc20:	f9000bfe 	str	x30, [sp, #16]
 402fc24:	97ffffd2 	bl	402fb6c <ddrtrn_hal_get_cur_phy_total_byte_num>
 402fc28:	2a0003f3 	mov	w19, w0
 402fc2c:	97ffffcb 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 402fc30:	1108b001 	add	w1, w0, #0x22c
 402fc34:	d2800002 	mov	x2, #0x0                   	// #0
 402fc38:	6b02027f 	cmp	w19, w2
 402fc3c:	54000088 	b.hi	402fc4c <ddrtrn_hal_training_get_rdqs+0x34>  // b.pmore
 402fc40:	f9400bfe 	ldr	x30, [sp, #16]
 402fc44:	a8c253f3 	ldp	x19, x20, [sp], #32
 402fc48:	d65f03c0 	ret
 402fc4c:	2a0103e0 	mov	w0, w1
 402fc50:	11020021 	add	w1, w1, #0x80
 402fc54:	b9400000 	ldr	w0, [x0]
 402fc58:	b8227a80 	str	w0, [x20, x2, lsl #2]
 402fc5c:	91000442 	add	x2, x2, #0x1
 402fc60:	17fffff6 	b	402fc38 <ddrtrn_hal_training_get_rdqs+0x20>

000000000402fc64 <ddrtrn_hal_training_set_rdqs>:
 402fc64:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 402fc68:	aa0003f4 	mov	x20, x0
 402fc6c:	f9000bfe 	str	x30, [sp, #16]
 402fc70:	97ffffbf 	bl	402fb6c <ddrtrn_hal_get_cur_phy_total_byte_num>
 402fc74:	2a0003f3 	mov	w19, w0
 402fc78:	97ffffb8 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 402fc7c:	1108b001 	add	w1, w0, #0x22c
 402fc80:	d2800002 	mov	x2, #0x0                   	// #0
 402fc84:	6b02027f 	cmp	w19, w2
 402fc88:	54000088 	b.hi	402fc98 <ddrtrn_hal_training_set_rdqs+0x34>  // b.pmore
 402fc8c:	f9400bfe 	ldr	x30, [sp, #16]
 402fc90:	a8c253f3 	ldp	x19, x20, [sp], #32
 402fc94:	d65f03c0 	ret
 402fc98:	2a0103e0 	mov	w0, w1
 402fc9c:	b8627a83 	ldr	w3, [x20, x2, lsl #2]
 402fca0:	11020021 	add	w1, w1, #0x80
 402fca4:	91000442 	add	x2, x2, #0x1
 402fca8:	b9000003 	str	w3, [x0]
 402fcac:	17fffff6 	b	402fc84 <ddrtrn_hal_training_set_rdqs+0x20>

000000000402fcb0 <ddrtrn_hal_hw_training_process>:
 402fcb0:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402fcb4:	a9015bf5 	stp	x21, x22, [sp, #16]
 402fcb8:	2a0003f5 	mov	w21, w0
 402fcbc:	f90013fe 	str	x30, [sp, #32]
 402fcc0:	97ffffa6 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 402fcc4:	11001016 	add	w22, w0, #0x4
 402fcc8:	2a0003f4 	mov	w20, w0
 402fccc:	b94002c0 	ldr	w0, [x22]
 402fcd0:	350000d5 	cbnz	w21, 402fce8 <ddrtrn_hal_hw_training_process+0x38>
 402fcd4:	52800000 	mov	w0, #0x0                   	// #0
 402fcd8:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402fcdc:	f94013fe 	ldr	x30, [sp, #32]
 402fce0:	a8c353f3 	ldp	x19, x20, [sp], #48
 402fce4:	d65f03c0 	ret
 402fce8:	2a0002a0 	orr	w0, w21, w0
 402fcec:	32000000 	orr	w0, w0, #0x1
 402fcf0:	b90002c0 	str	w0, [x22]
 402fcf4:	52828000 	mov	w0, #0x1400                	// #5120
 402fcf8:	6a35001f 	bics	wzr, w0, w21
 402fcfc:	540001c0 	b.eq	402fd34 <ddrtrn_hal_hw_training_process+0x84>  // b.none
 402fd00:	52800013 	mov	w19, #0x0                   	// #0
 402fd04:	b94002c0 	ldr	w0, [x22]
 402fd08:	11000673 	add	w19, w19, #0x1
 402fd0c:	360001e0 	tbz	w0, #0, 402fd48 <ddrtrn_hal_hw_training_process+0x98>
 402fd10:	3100067f 	cmn	w19, #0x1
 402fd14:	54ffff81 	b.ne	402fd04 <ddrtrn_hal_hw_training_process+0x54>  // b.any
 402fd18:	11002280 	add	w0, w20, #0x8
 402fd1c:	b9400003 	ldr	w3, [x0]
 402fd20:	2a1503e2 	mov	w2, w21
 402fd24:	2a1403e1 	mov	w1, w20
 402fd28:	52800200 	mov	w0, #0x10                  	// #16
 402fd2c:	97fff23f 	bl	402c628 <ddrtrn_hal_training_stat>
 402fd30:	14000004 	b	402fd40 <ddrtrn_hal_hw_training_process+0x90>
 402fd34:	52800040 	mov	w0, #0x2                   	// #2
 402fd38:	97fff783 	bl	402db44 <ddrtrn_training_ctrl_easr>
 402fd3c:	34fffe20 	cbz	w0, 402fd00 <ddrtrn_hal_hw_training_process+0x50>
 402fd40:	12800000 	mov	w0, #0xffffffff            	// #-1
 402fd44:	17ffffe5 	b	402fcd8 <ddrtrn_hal_hw_training_process+0x28>
 402fd48:	11002280 	add	w0, w20, #0x8
 402fd4c:	b9400001 	ldr	w1, [x0]
 402fd50:	721c783f 	tst	w1, #0xfffffff7
 402fd54:	54fffc00 	b.eq	402fcd4 <ddrtrn_hal_hw_training_process+0x24>  // b.none
 402fd58:	17fffff1 	b	402fd1c <ddrtrn_hal_hw_training_process+0x6c>

000000000402fd5c <ddrtrn_hal_hw_training_normal_conf>:
 402fd5c:	d10383ff 	sub	sp, sp, #0xe0
 402fd60:	d0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402fd64:	a90a53f3 	stp	x19, x20, [sp, #160]
 402fd68:	a90b5bf5 	stp	x21, x22, [sp, #176]
 402fd6c:	a90c63f7 	stp	x23, x24, [sp, #192]
 402fd70:	f9006bfe 	str	x30, [sp, #208]
 402fd74:	f9477001 	ldr	x1, [x0, #3808]
 402fd78:	f9004fe1 	str	x1, [sp, #152]
 402fd7c:	d2800001 	mov	x1, #0x0                   	// #0
 402fd80:	97ffeb77 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fd84:	b9400c16 	ldr	w22, [x0, #12]
 402fd88:	52801202 	mov	w2, #0x90                  	// #144
 402fd8c:	52800001 	mov	w1, #0x0                   	// #0
 402fd90:	910023e0 	add	x0, sp, #0x8
 402fd94:	97fff966 	bl	402e32c <ddrtrn_set_data>
 402fd98:	97ffff70 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 402fd9c:	2a0003f5 	mov	w21, w0
 402fda0:	97ffff73 	bl	402fb6c <ddrtrn_hal_get_cur_phy_total_byte_num>
 402fda4:	2a0003f4 	mov	w20, w0
 402fda8:	97ffeb6d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fdac:	b9402417 	ldr	w23, [x0, #36]
 402fdb0:	34000597 	cbz	w23, 402fe60 <ddrtrn_hal_hw_training_normal_conf+0x104>
 402fdb4:	910023f7 	add	x23, sp, #0x8
 402fdb8:	52800013 	mov	w19, #0x0                   	// #0
 402fdbc:	aa1703f8 	mov	x24, x23
 402fdc0:	6b14027f 	cmp	w19, w20
 402fdc4:	54000801 	b.ne	402fec4 <ddrtrn_hal_hw_training_normal_conf+0x168>  // b.any
 402fdc8:	121c02c0 	and	w0, w22, #0x10
 402fdcc:	97ffffb9 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402fdd0:	2a0003f3 	mov	w19, w0
 402fdd4:	52800018 	mov	w24, #0x0                   	// #0
 402fdd8:	6b14031f 	cmp	w24, w20
 402fddc:	54000841 	b.ne	402fee4 <ddrtrn_hal_hw_training_normal_conf+0x188>  // b.any
 402fde0:	2a1403e1 	mov	w1, w20
 402fde4:	2a1503e0 	mov	w0, w21
 402fde8:	97ffff6c 	bl	402fb98 <ddrtrn_hal_set_rank1_wdq_to_rank0>
 402fdec:	121b12c0 	and	w0, w22, #0x3e0
 402fdf0:	97ffffb0 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402fdf4:	0b000273 	add	w19, w19, w0
 402fdf8:	2a1403e1 	mov	w1, w20
 402fdfc:	2a1503e0 	mov	w0, w21
 402fe00:	97ffff66 	bl	402fb98 <ddrtrn_hal_set_rank1_wdq_to_rank0>
 402fe04:	120f02c0 	and	w0, w22, #0x20000
 402fe08:	97ffffaa 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402fe0c:	0b000273 	add	w19, w19, w0
 402fe10:	121002c0 	and	w0, w22, #0x10000
 402fe14:	97ffffa7 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402fe18:	2a1403e1 	mov	w1, w20
 402fe1c:	0b000273 	add	w19, w19, w0
 402fe20:	2a1503e0 	mov	w0, w21
 402fe24:	97ffff5d 	bl	402fb98 <ddrtrn_hal_set_rank1_wdq_to_rank0>
 402fe28:	120402c0 	and	w0, w22, #0x10000000
 402fe2c:	52800016 	mov	w22, #0x0                   	// #0
 402fe30:	97ffffa0 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402fe34:	0b000273 	add	w19, w19, w0
 402fe38:	6b1402df 	cmp	w22, w20
 402fe3c:	54000260 	b.eq	402fe88 <ddrtrn_hal_hw_training_normal_conf+0x12c>  // b.none
 402fe40:	2a1603e3 	mov	w3, w22
 402fe44:	aa1703e0 	mov	x0, x23
 402fe48:	2a1503e1 	mov	w1, w21
 402fe4c:	52800002 	mov	w2, #0x0                   	// #0
 402fe50:	110006d6 	add	w22, w22, #0x1
 402fe54:	910092f7 	add	x23, x23, #0x24
 402fe58:	97fff02b 	bl	402bf04 <ddrtrn_hal_restore_dly_value>
 402fe5c:	17fffff7 	b	402fe38 <ddrtrn_hal_hw_training_normal_conf+0xdc>
 402fe60:	121c02c0 	and	w0, w22, #0x10
 402fe64:	97ffff93 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402fe68:	2a0003f3 	mov	w19, w0
 402fe6c:	6b1402ff 	cmp	w23, w20
 402fe70:	540001a1 	b.ne	402fea4 <ddrtrn_hal_hw_training_normal_conf+0x148>  // b.any
 402fe74:	52807c00 	mov	w0, #0x3e0                 	// #992
 402fe78:	72a20060 	movk	w0, #0x1003, lsl #16
 402fe7c:	0a0002c0 	and	w0, w22, w0
 402fe80:	97ffff8c 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402fe84:	0b000273 	add	w19, w19, w0
 402fe88:	d0ffff80 	adrp	x0, 4021000 <gs_misc_record-0xa80>
 402fe8c:	f9404fe1 	ldr	x1, [sp, #152]
 402fe90:	f9477002 	ldr	x2, [x0, #3808]
 402fe94:	eb020021 	subs	x1, x1, x2
 402fe98:	d2800002 	mov	x2, #0x0                   	// #0
 402fe9c:	54000340 	b.eq	402ff04 <ddrtrn_hal_hw_training_normal_conf+0x1a8>  // b.none
 402fea0:	97ffd8d7 	bl	40261fc <__stack_chk_fail>
 402fea4:	97ffeb2e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fea8:	b9001817 	str	w23, [x0, #24]
 402feac:	2a1703e2 	mov	w2, w23
 402feb0:	2a1503e0 	mov	w0, w21
 402feb4:	52800001 	mov	w1, #0x0                   	// #0
 402feb8:	110006f7 	add	w23, w23, #0x1
 402febc:	97fff020 	bl	402bf3c <ddrtrn_hal_wdqs_bdl2phase>
 402fec0:	17ffffeb 	b	402fe6c <ddrtrn_hal_hw_training_normal_conf+0x110>
 402fec4:	2a1303e3 	mov	w3, w19
 402fec8:	aa1803e0 	mov	x0, x24
 402fecc:	2a1503e1 	mov	w1, w21
 402fed0:	52800002 	mov	w2, #0x0                   	// #0
 402fed4:	11000673 	add	w19, w19, #0x1
 402fed8:	91009318 	add	x24, x24, #0x24
 402fedc:	97ffeff6 	bl	402beb4 <ddrtrn_hal_get_dly_value>
 402fee0:	17ffffb8 	b	402fdc0 <ddrtrn_hal_hw_training_normal_conf+0x64>
 402fee4:	97ffeb1e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402fee8:	b9001818 	str	w24, [x0, #24]
 402feec:	2a1803e2 	mov	w2, w24
 402fef0:	2a1503e0 	mov	w0, w21
 402fef4:	52800021 	mov	w1, #0x1                   	// #1
 402fef8:	11000718 	add	w24, w24, #0x1
 402fefc:	97fff010 	bl	402bf3c <ddrtrn_hal_wdqs_bdl2phase>
 402ff00:	17ffffb6 	b	402fdd8 <ddrtrn_hal_hw_training_normal_conf+0x7c>
 402ff04:	2a1303e0 	mov	w0, w19
 402ff08:	a94a53f3 	ldp	x19, x20, [sp, #160]
 402ff0c:	a94b5bf5 	ldp	x21, x22, [sp, #176]
 402ff10:	a94c63f7 	ldp	x23, x24, [sp, #192]
 402ff14:	f9406bfe 	ldr	x30, [sp, #208]
 402ff18:	910383ff 	add	sp, sp, #0xe0
 402ff1c:	d65f03c0 	ret

000000000402ff20 <ddrtrn_hal_hw_ca_vref_sync>:
 402ff20:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 402ff24:	a9015bf5 	stp	x21, x22, [sp, #16]
 402ff28:	a9027bf7 	stp	x23, x30, [sp, #32]
 402ff2c:	97ffff0b 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 402ff30:	2a0003f3 	mov	w19, w0
 402ff34:	11012273 	add	w19, w19, #0x48
 402ff38:	97ffeb09 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ff3c:	b9400c14 	ldr	w20, [x0, #12]
 402ff40:	b9400277 	ldr	w23, [x19]
 402ff44:	120b0294 	and	w20, w20, #0x200000
 402ff48:	2a1403e0 	mov	w0, w20
 402ff4c:	121c6ef5 	and	w21, w23, #0xfffffff0
 402ff50:	b9000275 	str	w21, [x19]
 402ff54:	320002b5 	orr	w21, w21, #0x1
 402ff58:	97ffff56 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402ff5c:	b9000275 	str	w21, [x19]
 402ff60:	2a0003f6 	mov	w22, w0
 402ff64:	2a1403e0 	mov	w0, w20
 402ff68:	97ffff52 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 402ff6c:	0b0002c0 	add	w0, w22, w0
 402ff70:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402ff74:	b9000277 	str	w23, [x19]
 402ff78:	a9427bf7 	ldp	x23, x30, [sp, #32]
 402ff7c:	a8c353f3 	ldp	x19, x20, [sp], #48
 402ff80:	d65f03c0 	ret

000000000402ff84 <ddrtrn_hal_hw_dram_mr_init>:
 402ff84:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 402ff88:	a9015bf5 	stp	x21, x22, [sp, #16]
 402ff8c:	a90263f7 	stp	x23, x24, [sp, #32]
 402ff90:	f9001bfe 	str	x30, [sp, #48]
 402ff94:	97fffef1 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 402ff98:	2a0003f3 	mov	w19, w0
 402ff9c:	97ffeaf0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 402ffa0:	b9400c16 	ldr	w22, [x0, #12]
 402ffa4:	52821900 	mov	w0, #0x10c8                	// #4296
 402ffa8:	0b000260 	add	w0, w19, w0
 402ffac:	121602d6 	and	w22, w22, #0x400
 402ffb0:	b9400000 	ldr	w0, [x0]
 402ffb4:	f240041f 	tst	x0, #0x3
 402ffb8:	540000e0 	b.eq	402ffd4 <ddrtrn_hal_hw_dram_mr_init+0x50>  // b.none
 402ffbc:	2a1603e0 	mov	w0, w22
 402ffc0:	a9415bf5 	ldp	x21, x22, [sp, #16]
 402ffc4:	a94263f7 	ldp	x23, x24, [sp, #32]
 402ffc8:	f9401bfe 	ldr	x30, [sp, #48]
 402ffcc:	a8c453f3 	ldp	x19, x20, [sp], #64
 402ffd0:	17ffff38 	b	402fcb0 <ddrtrn_hal_hw_training_process>
 402ffd4:	11013274 	add	w20, w19, #0x4c
 402ffd8:	1101a273 	add	w19, w19, #0x68
 402ffdc:	b9400298 	ldr	w24, [x20]
 402ffe0:	2a1803e0 	mov	w0, w24
 402ffe4:	72800020 	movk	w0, #0x1
 402ffe8:	b9000280 	str	w0, [x20]
 402ffec:	b9400277 	ldr	w23, [x19]
 402fff0:	121a72e0 	and	w0, w23, #0xffffffc7
 402fff4:	b9000260 	str	w0, [x19]
 402fff8:	2a1603e0 	mov	w0, w22
 402fffc:	97ffff2d 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 4030000:	b9000298 	str	w24, [x20]
 4030004:	b9000277 	str	w23, [x19]
 4030008:	2a0003f5 	mov	w21, w0
 403000c:	52800501 	mov	w1, #0x28                  	// #40
 4030010:	b9400298 	ldr	w24, [x20]
 4030014:	2a1803e0 	mov	w0, w24
 4030018:	72800040 	movk	w0, #0x2
 403001c:	b9000280 	str	w0, [x20]
 4030020:	b9400277 	ldr	w23, [x19]
 4030024:	121a72e0 	and	w0, w23, #0xffffffc7
 4030028:	2a010000 	orr	w0, w0, w1
 403002c:	b9000260 	str	w0, [x19]
 4030030:	2a1603e0 	mov	w0, w22
 4030034:	97ffff1f 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 4030038:	0b0002a0 	add	w0, w21, w0
 403003c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4030040:	f9401bfe 	ldr	x30, [sp, #48]
 4030044:	b9000298 	str	w24, [x20]
 4030048:	b9000277 	str	w23, [x19]
 403004c:	a94263f7 	ldp	x23, x24, [sp, #32]
 4030050:	a8c453f3 	ldp	x19, x20, [sp], #64
 4030054:	d65f03c0 	ret

0000000004030058 <ddrtrn_hal_hw_dataeye_adapt>:
 4030058:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 403005c:	aa0003f3 	mov	x19, x0
 4030060:	a9015bf5 	stp	x21, x22, [sp, #16]
 4030064:	a90263f7 	stp	x23, x24, [sp, #32]
 4030068:	f9001bfe 	str	x30, [sp, #48]
 403006c:	97fffebb 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 4030070:	2a0003f5 	mov	w21, w0
 4030074:	97ffeaba 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030078:	aa0003f4 	mov	x20, x0
 403007c:	97ffeab3 	bl	402ab48 <ddrtrn_hal_get_phy>
 4030080:	d2800782 	mov	x2, #0x3c                  	// #60
 4030084:	b9402281 	ldr	w1, [x20, #32]
 4030088:	9b020021 	madd	x1, x1, x2, x0
 403008c:	b9400420 	ldr	w0, [x1, #4]
 4030090:	7100181f 	cmp	w0, #0x6
 4030094:	54000160 	b.eq	40300c0 <ddrtrn_hal_hw_dataeye_adapt+0x68>  // b.none
 4030098:	b900027f 	str	wzr, [x19]
 403009c:	97ffeab0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40300a0:	b9400c01 	ldr	w1, [x0, #12]
 40300a4:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40300a8:	52828000 	mov	w0, #0x1400                	// #5120
 40300ac:	a94263f7 	ldp	x23, x24, [sp, #32]
 40300b0:	0a000020 	and	w0, w1, w0
 40300b4:	f9401bfe 	ldr	x30, [sp, #48]
 40300b8:	a8c453f3 	ldp	x19, x20, [sp], #64
 40300bc:	17fffefd 	b	402fcb0 <ddrtrn_hal_hw_training_process>
 40300c0:	1100d2b6 	add	w22, w21, #0x34
 40300c4:	b94002d7 	ldr	w23, [x22]
 40300c8:	12176ae0 	and	w0, w23, #0xfffffe0f
 40300cc:	b90002c0 	str	w0, [x22]
 40300d0:	97fffea2 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 40300d4:	2a0003f4 	mov	w20, w0
 40300d8:	11019294 	add	w20, w20, #0x64
 40300dc:	97ffeaa0 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40300e0:	b9400c01 	ldr	w1, [x0, #12]
 40300e4:	b9400298 	ldr	w24, [x20]
 40300e8:	12017300 	and	w0, w24, #0x8fffffff
 40300ec:	b9000280 	str	w0, [x20]
 40300f0:	52828000 	mov	w0, #0x1400                	// #5120
 40300f4:	0a000020 	and	w0, w1, w0
 40300f8:	97fffeee 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 40300fc:	b9000298 	str	w24, [x20]
 4030100:	110392b4 	add	w20, w21, #0xe4
 4030104:	2a0003f3 	mov	w19, w0
 4030108:	97ffff86 	bl	402ff20 <ddrtrn_hal_hw_ca_vref_sync>
 403010c:	0b130013 	add	w19, w0, w19
 4030110:	97ffff9d 	bl	402ff84 <ddrtrn_hal_hw_dram_mr_init>
 4030114:	0b000273 	add	w19, w19, w0
 4030118:	b9400295 	ldr	w21, [x20]
 403011c:	321a02a0 	orr	w0, w21, #0x40
 4030120:	b9000280 	str	w0, [x20]
 4030124:	12197ab5 	and	w21, w21, #0xffffffbf
 4030128:	97ffff97 	bl	402ff84 <ddrtrn_hal_hw_dram_mr_init>
 403012c:	0b000273 	add	w19, w19, w0
 4030130:	97ffff7c 	bl	402ff20 <ddrtrn_hal_hw_ca_vref_sync>
 4030134:	b9000295 	str	w21, [x20]
 4030138:	0b000273 	add	w19, w19, w0
 403013c:	97ffff92 	bl	402ff84 <ddrtrn_hal_hw_dram_mr_init>
 4030140:	0b000260 	add	w0, w19, w0
 4030144:	f9401bfe 	ldr	x30, [sp, #48]
 4030148:	b90002d7 	str	w23, [x22]
 403014c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4030150:	a94263f7 	ldp	x23, x24, [sp, #32]
 4030154:	a8c453f3 	ldp	x19, x20, [sp], #64
 4030158:	d65f03c0 	ret

000000000403015c <ddrtrn_hal_hw_dataeye_vref_set>:
 403015c:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 4030160:	a9015bf5 	stp	x21, x22, [sp, #16]
 4030164:	a9027bf7 	stp	x23, x30, [sp, #32]
 4030168:	97fffe7c 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 403016c:	2a0003f3 	mov	w19, w0
 4030170:	11031276 	add	w22, w19, #0xc4
 4030174:	97ffea7a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030178:	11030273 	add	w19, w19, #0xc0
 403017c:	b9400c15 	ldr	w21, [x0, #12]
 4030180:	b94002d7 	ldr	w23, [x22]
 4030184:	120e02b5 	and	w21, w21, #0x40000
 4030188:	12007ae0 	and	w0, w23, #0x7fffffff
 403018c:	b90002c0 	str	w0, [x22]
 4030190:	b9400260 	ldr	w0, [x19]
 4030194:	32010000 	orr	w0, w0, #0x80000000
 4030198:	b9000260 	str	w0, [x19]
 403019c:	2a1503e0 	mov	w0, w21
 40301a0:	97fffec4 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 40301a4:	2a0003f4 	mov	w20, w0
 40301a8:	2a1503e0 	mov	w0, w21
 40301ac:	97fffec1 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 40301b0:	0b000294 	add	w20, w20, w0
 40301b4:	b9400260 	ldr	w0, [x19]
 40301b8:	12007800 	and	w0, w0, #0x7fffffff
 40301bc:	b9000260 	str	w0, [x19]
 40301c0:	2a1503e0 	mov	w0, w21
 40301c4:	97fffebb 	bl	402fcb0 <ddrtrn_hal_hw_training_process>
 40301c8:	b90002d7 	str	w23, [x22]
 40301cc:	0b000280 	add	w0, w20, w0
 40301d0:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40301d4:	a9427bf7 	ldp	x23, x30, [sp, #32]
 40301d8:	a8c353f3 	ldp	x19, x20, [sp], #48
 40301dc:	d65f03c0 	ret

00000000040301e0 <ddrtrn_hal_hw_clear_rdq>:
 40301e0:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 40301e4:	53196013 	lsl	w19, w0, #7
 40301e8:	f9000bfe 	str	x30, [sp, #16]
 40301ec:	97fffe5b 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 40301f0:	2a0003f4 	mov	w20, w0
 40301f4:	97ffea5a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40301f8:	b9402400 	ldr	w0, [x0, #36]
 40301fc:	11087261 	add	w1, w19, #0x21c
 4030200:	0b002a80 	add	w0, w20, w0, lsl #10
 4030204:	0b010000 	add	w0, w0, w1
 4030208:	b900001f 	str	wzr, [x0]
 403020c:	97ffea54 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030210:	b9402400 	ldr	w0, [x0, #36]
 4030214:	11088261 	add	w1, w19, #0x220
 4030218:	1108b273 	add	w19, w19, #0x22c
 403021c:	0b140273 	add	w19, w19, w20
 4030220:	0b002a80 	add	w0, w20, w0, lsl #10
 4030224:	0b010000 	add	w0, w0, w1
 4030228:	f9400bfe 	ldr	x30, [sp, #16]
 403022c:	b900001f 	str	wzr, [x0]
 4030230:	b900027f 	str	wzr, [x19]
 4030234:	a8c253f3 	ldp	x19, x20, [sp], #32
 4030238:	d65f03c0 	ret

000000000403023c <ddrtrn_hal_hw_restore_rdqsbdl>:
 403023c:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 4030240:	2a0203f3 	mov	w19, w2
 4030244:	12002014 	and	w20, w0, #0x1ff
 4030248:	a9015bf5 	stp	x21, x22, [sp, #16]
 403024c:	2a0103f6 	mov	w22, w1
 4030250:	12002035 	and	w21, w1, #0x1ff
 4030254:	a9027bf7 	stp	x23, x30, [sp, #32]
 4030258:	2a0003f7 	mov	w23, w0
 403025c:	97ffea40 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030260:	b9001813 	str	w19, [x0, #24]
 4030264:	53196273 	lsl	w19, w19, #7
 4030268:	1108b273 	add	w19, w19, #0x22c
 403026c:	6b15029f 	cmp	w20, w21
 4030270:	540001a9 	b.ls	40302a4 <ddrtrn_hal_hw_restore_rdqsbdl+0x68>  // b.plast
 4030274:	97fffe39 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 4030278:	0b130013 	add	w19, w0, w19
 403027c:	4b150294 	sub	w20, w20, w21
 4030280:	b9000277 	str	w23, [x19]
 4030284:	97ffea36 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030288:	52800021 	mov	w1, #0x1                   	// #1
 403028c:	b9002401 	str	w1, [x0, #36]
 4030290:	2a1403e0 	mov	w0, w20
 4030294:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4030298:	a9427bf7 	ldp	x23, x30, [sp, #32]
 403029c:	a8c353f3 	ldp	x19, x20, [sp], #48
 40302a0:	d65f03c0 	ret
 40302a4:	97fffe2d 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 40302a8:	0b130013 	add	w19, w0, w19
 40302ac:	4b1402b4 	sub	w20, w21, w20
 40302b0:	b9000276 	str	w22, [x19]
 40302b4:	97ffea2a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40302b8:	b900241f 	str	wzr, [x0, #36]
 40302bc:	17fffff5 	b	4030290 <ddrtrn_hal_hw_restore_rdqsbdl+0x54>

00000000040302c0 <ddrtrn_hal_hw_save_rdqsbdl>:
 40302c0:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 40302c4:	f9000bfe 	str	x30, [sp, #16]
 40302c8:	97fffe24 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 40302cc:	1108b000 	add	w0, w0, #0x22c
 40302d0:	b9400014 	ldr	w20, [x0]
 40302d4:	97fffe21 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 40302d8:	1107a000 	add	w0, w0, #0x1e8
 40302dc:	53127e94 	lsr	w20, w20, #18
 40302e0:	b9400013 	ldr	w19, [x0]
 40302e4:	97fffe1d 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 40302e8:	1107a000 	add	w0, w0, #0x1e8
 40302ec:	f9400bfe 	ldr	x30, [sp, #16]
 40302f0:	33161a93 	bfi	w19, w20, #10, #7
 40302f4:	b9000013 	str	w19, [x0]
 40302f8:	a8c253f3 	ldp	x19, x20, [sp], #32
 40302fc:	d65f03c0 	ret

0000000004030300 <ddrtrn_ac_oe_enable>:
 4030300:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4030304:	f9000bfe 	str	x30, [sp, #16]
 4030308:	97fffe14 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 403030c:	1101e001 	add	w1, w0, #0x78
 4030310:	2a0003f3 	mov	w19, w0
 4030314:	52820280 	mov	w0, #0x1014                	// #4116
 4030318:	0b000273 	add	w19, w19, w0
 403031c:	b9400020 	ldr	w0, [x1]
 4030320:	32190000 	orr	w0, w0, #0x80
 4030324:	b9000020 	str	w0, [x1]
 4030328:	97ffea0d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 403032c:	aa0003f4 	mov	x20, x0
 4030330:	97ffea06 	bl	402ab48 <ddrtrn_hal_get_phy>
 4030334:	d2800782 	mov	x2, #0x3c                  	// #60
 4030338:	b9402281 	ldr	w1, [x20, #32]
 403033c:	9b020021 	madd	x1, x1, x2, x0
 4030340:	b9400420 	ldr	w0, [x1, #4]
 4030344:	7100181f 	cmp	w0, #0x6
 4030348:	b9400260 	ldr	w0, [x19]
 403034c:	540000c1 	b.ne	4030364 <ddrtrn_ac_oe_enable+0x64>  // b.any
 4030350:	32002c00 	orr	w0, w0, #0xfff
 4030354:	f9400bfe 	ldr	x30, [sp, #16]
 4030358:	b9000260 	str	w0, [x19]
 403035c:	a8c253f3 	ldp	x19, x20, [sp], #32
 4030360:	d65f03c0 	ret
 4030364:	12144c00 	and	w0, w0, #0xfffff000
 4030368:	5281fee1 	mov	w1, #0xff7                 	// #4087
 403036c:	2a010000 	orr	w0, w0, w1
 4030370:	17fffff9 	b	4030354 <ddrtrn_ac_oe_enable+0x54>

0000000004030374 <ddrtrn_dummy_io_oe_enable>:
 4030374:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 4030378:	97fffdfd 	bl	402fb6c <ddrtrn_hal_get_cur_phy_total_byte_num>
 403037c:	2a0003f3 	mov	w19, w0
 4030380:	97fffdf6 	bl	402fb58 <ddrtrn_hal_get_cur_phy>
 4030384:	52825401 	mov	w1, #0x12a0                	// #4768
 4030388:	52825382 	mov	w2, #0x129c                	// #4764
 403038c:	0b010004 	add	w4, w0, w1
 4030390:	0b020002 	add	w2, w0, w2
 4030394:	52800003 	mov	w3, #0x0                   	// #0
 4030398:	6b13007f 	cmp	w3, w19
 403039c:	54000061 	b.ne	40303a8 <ddrtrn_dummy_io_oe_enable+0x34>  // b.any
 40303a0:	a8c17bf3 	ldp	x19, x30, [sp], #16
 40303a4:	d65f03c0 	ret
 40303a8:	53017c61 	lsr	w1, w3, #1
 40303ac:	53185c21 	lsl	w1, w1, #8
 40303b0:	370000e3 	tbnz	w3, #0, 40303cc <ddrtrn_dummy_io_oe_enable+0x58>
 40303b4:	0b010041 	add	w1, w2, w1
 40303b8:	b9400020 	ldr	w0, [x1]
 40303bc:	11000463 	add	w3, w3, #0x1
 40303c0:	32010000 	orr	w0, w0, #0x80000000
 40303c4:	b9000020 	str	w0, [x1]
 40303c8:	17fffff4 	b	4030398 <ddrtrn_dummy_io_oe_enable+0x24>
 40303cc:	0b010081 	add	w1, w4, w1
 40303d0:	17fffffa 	b	40303b8 <ddrtrn_dummy_io_oe_enable+0x44>

00000000040303d4 <ddrtrn_hal_get_cur_phy>:
 40303d4:	f81f0ffe 	str	x30, [sp, #-16]!
 40303d8:	97ffe9e1 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40303dc:	b9400400 	ldr	w0, [x0, #4]
 40303e0:	f84107fe 	ldr	x30, [sp], #16
 40303e4:	d65f03c0 	ret

00000000040303e8 <ddrtrn_hal_get_byte_num>:
 40303e8:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 40303ec:	97ffe9dc 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40303f0:	aa0003f3 	mov	x19, x0
 40303f4:	97ffe9d5 	bl	402ab48 <ddrtrn_hal_get_phy>
 40303f8:	d2800783 	mov	x3, #0x3c                  	// #60
 40303fc:	b9402262 	ldr	w2, [x19, #32]
 4030400:	b9402a61 	ldr	w1, [x19, #40]
 4030404:	a8c17bf3 	ldp	x19, x30, [sp], #16
 4030408:	9b037c42 	mul	x2, x2, x3
 403040c:	d2800183 	mov	x3, #0xc                   	// #12
 4030410:	9b030821 	madd	x1, x1, x3, x2
 4030414:	8b010000 	add	x0, x0, x1
 4030418:	b9401800 	ldr	w0, [x0, #24]
 403041c:	d65f03c0 	ret

0000000004030420 <ddrtrn_hal_vref_phy_dram_set.isra.0>:
 4030420:	0b021c05 	add	w5, w0, w2, lsl #7
 4030424:	11031006 	add	w6, w0, #0xc4
 4030428:	1109c0a5 	add	w5, w5, #0x270
 403042c:	b94000c4 	ldr	w4, [x6]
 4030430:	b94000a3 	ldr	w3, [x5]
 4030434:	32010087 	orr	w7, w4, #0x80000000
 4030438:	b90000c7 	str	w7, [x6]
 403043c:	121a6463 	and	w3, w3, #0xffffffc0
 4030440:	2a010063 	orr	w3, w3, w1
 4030444:	b90000a3 	str	w3, [x5]
 4030448:	11001003 	add	w3, w0, #0x4
 403044c:	52800021 	mov	w1, #0x1                   	// #1
 4030450:	72a00081 	movk	w1, #0x4, lsl #16
 4030454:	b9000061 	str	w1, [x3]
 4030458:	52800001 	mov	w1, #0x0                   	// #0
 403045c:	b9400065 	ldr	w5, [x3]
 4030460:	11000421 	add	w1, w1, #0x1
 4030464:	36000105 	tbz	w5, #0, 4030484 <ddrtrn_hal_vref_phy_dram_set.isra.0+0x64>
 4030468:	3100043f 	cmn	w1, #0x1
 403046c:	54ffff81 	b.ne	403045c <ddrtrn_hal_vref_phy_dram_set.isra.0+0x3c>  // b.any
 4030470:	11002001 	add	w1, w0, #0x8
 4030474:	b9400023 	ldr	w3, [x1]
 4030478:	2a0003e1 	mov	w1, w0
 403047c:	52800200 	mov	w0, #0x10                  	// #16
 4030480:	17fff06a 	b	402c628 <ddrtrn_hal_training_stat>
 4030484:	12007884 	and	w4, w4, #0x7fffffff
 4030488:	b90000c4 	str	w4, [x6]
 403048c:	d65f03c0 	ret

0000000004030490 <ddrtrn_hal_vref_save_bdl>:
 4030490:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 4030494:	aa0003f4 	mov	x20, x0
 4030498:	a9015bf5 	stp	x21, x22, [sp, #16]
 403049c:	52800015 	mov	w21, #0x0                   	// #0
 40304a0:	f90013fe 	str	x30, [sp, #32]
 40304a4:	97ffffcc 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 40304a8:	2a0003f6 	mov	w22, w0
 40304ac:	97ffe9ac 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40304b0:	b9402400 	ldr	w0, [x0, #36]
 40304b4:	0b002ad6 	add	w22, w22, w0, lsl #10
 40304b8:	97ffffcc 	bl	40303e8 <ddrtrn_hal_get_byte_num>
 40304bc:	6b15001f 	cmp	w0, w21
 40304c0:	540000a8 	b.hi	40304d4 <ddrtrn_hal_vref_save_bdl+0x44>  // b.pmore
 40304c4:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40304c8:	f94013fe 	ldr	x30, [sp, #32]
 40304cc:	a8c353f3 	ldp	x19, x20, [sp], #48
 40304d0:	d65f03c0 	ret
 40304d4:	97ffe9a2 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40304d8:	b9402801 	ldr	w1, [x0, #40]
 40304dc:	0b0106b3 	add	w19, w21, w1, lsl #1
 40304e0:	97ffe99f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40304e4:	b9401400 	ldr	w0, [x0, #20]
 40304e8:	0b131ec1 	add	w1, w22, w19, lsl #7
 40304ec:	7100081f 	cmp	w0, #0x2
 40304f0:	2a1503e0 	mov	w0, w21
 40304f4:	540001a1 	b.ne	4030528 <ddrtrn_hal_vref_save_bdl+0x98>  // b.any
 40304f8:	11084022 	add	w2, w1, #0x210
 40304fc:	b9400043 	ldr	w3, [x2]
 4030500:	8b000a82 	add	x2, x20, x0, lsl #2
 4030504:	b8207a83 	str	w3, [x20, x0, lsl #2]
 4030508:	11085020 	add	w0, w1, #0x214
 403050c:	11086021 	add	w1, w1, #0x218
 4030510:	b9400000 	ldr	w0, [x0]
 4030514:	b9001040 	str	w0, [x2, #16]
 4030518:	b9400020 	ldr	w0, [x1]
 403051c:	b9004040 	str	w0, [x2, #64]
 4030520:	110006b5 	add	w21, w21, #0x1
 4030524:	17ffffe5 	b	40304b8 <ddrtrn_hal_vref_save_bdl+0x28>
 4030528:	11087022 	add	w2, w1, #0x21c
 403052c:	11088021 	add	w1, w1, #0x220
 4030530:	b9400043 	ldr	w3, [x2]
 4030534:	8b000a82 	add	x2, x20, x0, lsl #2
 4030538:	b8207a83 	str	w3, [x20, x0, lsl #2]
 403053c:	b9400020 	ldr	w0, [x1]
 4030540:	b9001040 	str	w0, [x2, #16]
 4030544:	17fffff7 	b	4030520 <ddrtrn_hal_vref_save_bdl+0x90>

0000000004030548 <ddrtrn_hal_vref_restore_bdl>:
 4030548:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 403054c:	aa0003f4 	mov	x20, x0
 4030550:	a9015bf5 	stp	x21, x22, [sp, #16]
 4030554:	52800015 	mov	w21, #0x0                   	// #0
 4030558:	f90013fe 	str	x30, [sp, #32]
 403055c:	97ffff9e 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 4030560:	2a0003f6 	mov	w22, w0
 4030564:	97ffe97e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030568:	b9402400 	ldr	w0, [x0, #36]
 403056c:	0b002ad6 	add	w22, w22, w0, lsl #10
 4030570:	97ffff9e 	bl	40303e8 <ddrtrn_hal_get_byte_num>
 4030574:	6b15001f 	cmp	w0, w21
 4030578:	540000a8 	b.hi	403058c <ddrtrn_hal_vref_restore_bdl+0x44>  // b.pmore
 403057c:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4030580:	f94013fe 	ldr	x30, [sp, #32]
 4030584:	a8c353f3 	ldp	x19, x20, [sp], #48
 4030588:	d65f03c0 	ret
 403058c:	97ffe974 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030590:	b9402801 	ldr	w1, [x0, #40]
 4030594:	0b0106b3 	add	w19, w21, w1, lsl #1
 4030598:	97ffe971 	bl	402ab5c <ddrtrn_hal_get_ctx>
 403059c:	b9401400 	ldr	w0, [x0, #20]
 40305a0:	0b131ec1 	add	w1, w22, w19, lsl #7
 40305a4:	7100081f 	cmp	w0, #0x2
 40305a8:	2a1503e0 	mov	w0, w21
 40305ac:	8b000a82 	add	x2, x20, x0, lsl #2
 40305b0:	b8607a83 	ldr	w3, [x20, x0, lsl #2]
 40305b4:	54000161 	b.ne	40305e0 <ddrtrn_hal_vref_restore_bdl+0x98>  // b.any
 40305b8:	11084020 	add	w0, w1, #0x210
 40305bc:	b9000003 	str	w3, [x0]
 40305c0:	11085020 	add	w0, w1, #0x214
 40305c4:	11086021 	add	w1, w1, #0x218
 40305c8:	b9401043 	ldr	w3, [x2, #16]
 40305cc:	b9000003 	str	w3, [x0]
 40305d0:	b9404040 	ldr	w0, [x2, #64]
 40305d4:	110006b5 	add	w21, w21, #0x1
 40305d8:	b9000020 	str	w0, [x1]
 40305dc:	17ffffe5 	b	4030570 <ddrtrn_hal_vref_restore_bdl+0x28>
 40305e0:	11087020 	add	w0, w1, #0x21c
 40305e4:	11088021 	add	w1, w1, #0x220
 40305e8:	b9000003 	str	w3, [x0]
 40305ec:	b9401040 	ldr	w0, [x2, #16]
 40305f0:	17fffff9 	b	40305d4 <ddrtrn_hal_vref_restore_bdl+0x8c>

00000000040305f4 <ddrtrn_hal_vref_status_set>:
 40305f4:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 40305f8:	2a0003f3 	mov	w19, w0
 40305fc:	a9017bf5 	stp	x21, x30, [sp, #16]
 4030600:	97ffe957 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030604:	b9401400 	ldr	w0, [x0, #20]
 4030608:	7100041f 	cmp	w0, #0x1
 403060c:	54000361 	b.ne	4030678 <ddrtrn_hal_vref_status_set+0x84>  // b.any
 4030610:	97ffff71 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 4030614:	2a0003f4 	mov	w20, w0
 4030618:	97ffe951 	bl	402ab5c <ddrtrn_hal_get_ctx>
 403061c:	b9402415 	ldr	w21, [x0, #36]
 4030620:	97ffe94f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030624:	b9401802 	ldr	w2, [x0, #24]
 4030628:	1109d280 	add	w0, w20, #0x274
 403062c:	53196043 	lsl	w3, w2, #7
 4030630:	11000442 	add	w2, w2, #0x1
 4030634:	35000175 	cbnz	w21, 4030660 <ddrtrn_hal_vref_status_set+0x6c>
 4030638:	0b000063 	add	w3, w3, w0
 403063c:	b9400061 	ldr	w1, [x3]
 4030640:	12196021 	and	w1, w1, #0xffffff80
 4030644:	2a130033 	orr	w19, w1, w19
 4030648:	0b021c02 	add	w2, w0, w2, lsl #7
 403064c:	b9000073 	str	w19, [x3]
 4030650:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4030654:	b9000053 	str	w19, [x2]
 4030658:	a8c253f3 	ldp	x19, x20, [sp], #32
 403065c:	d65f03c0 	ret
 4030660:	0b152800 	add	w0, w0, w21, lsl #10
 4030664:	0b000063 	add	w3, w3, w0
 4030668:	b9400061 	ldr	w1, [x3]
 403066c:	12046c21 	and	w1, w1, #0xf0ffffff
 4030670:	2a136033 	orr	w19, w1, w19, lsl #24
 4030674:	17fffff5 	b	4030648 <ddrtrn_hal_vref_status_set+0x54>
 4030678:	97ffe939 	bl	402ab5c <ddrtrn_hal_get_ctx>
 403067c:	b9400800 	ldr	w0, [x0, #8]
 4030680:	11042000 	add	w0, w0, #0x108
 4030684:	b9400015 	ldr	w21, [x0]
 4030688:	97ffe935 	bl	402ab5c <ddrtrn_hal_get_ctx>
 403068c:	b9400800 	ldr	w0, [x0, #8]
 4030690:	12144ea1 	and	w1, w21, #0xfffff000
 4030694:	97ffeb6f 	bl	402b450 <ddrtrn_hal_set_timing>
 4030698:	97ffff4f 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 403069c:	11030000 	add	w0, w0, #0xc0
 40306a0:	b9400014 	ldr	w20, [x0]
 40306a4:	97ffff4c 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 40306a8:	11030000 	add	w0, w0, #0xc0
 40306ac:	32010294 	orr	w20, w20, #0x80000000
 40306b0:	b9000014 	str	w20, [x0]
 40306b4:	97ffff48 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 40306b8:	2a0003f4 	mov	w20, w0
 40306bc:	97ffe928 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40306c0:	b9401802 	ldr	w2, [x0, #24]
 40306c4:	2a1303e1 	mov	w1, w19
 40306c8:	2a1403e0 	mov	w0, w20
 40306cc:	97ffff55 	bl	4030420 <ddrtrn_hal_vref_phy_dram_set.isra.0>
 40306d0:	97ffff41 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 40306d4:	2a0003f4 	mov	w20, w0
 40306d8:	97ffe921 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40306dc:	b9401802 	ldr	w2, [x0, #24]
 40306e0:	2a1303e1 	mov	w1, w19
 40306e4:	2a1403e0 	mov	w0, w20
 40306e8:	97ffff4e 	bl	4030420 <ddrtrn_hal_vref_phy_dram_set.isra.0>
 40306ec:	97ffff3a 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 40306f0:	11030000 	add	w0, w0, #0xc0
 40306f4:	b9400014 	ldr	w20, [x0]
 40306f8:	97ffff37 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 40306fc:	11030000 	add	w0, w0, #0xc0
 4030700:	12007a94 	and	w20, w20, #0x7fffffff
 4030704:	b9000014 	str	w20, [x0]
 4030708:	97ffff33 	bl	40303d4 <ddrtrn_hal_get_cur_phy>
 403070c:	2a0003f4 	mov	w20, w0
 4030710:	97ffe913 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030714:	b9401802 	ldr	w2, [x0, #24]
 4030718:	2a1303e1 	mov	w1, w19
 403071c:	2a1403e0 	mov	w0, w20
 4030720:	97ffff40 	bl	4030420 <ddrtrn_hal_vref_phy_dram_set.isra.0>
 4030724:	97ffe90e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030728:	b9400800 	ldr	w0, [x0, #8]
 403072c:	2a1503e1 	mov	w1, w21
 4030730:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4030734:	a8c253f3 	ldp	x19, x20, [sp], #32
 4030738:	17ffeb46 	b	402b450 <ddrtrn_hal_set_timing>

000000000403073c <ddrtrn_adjust_move_win.constprop.0>:
 403073c:	a9b953f3 	stp	x19, x20, [sp, #-112]!
 4030740:	2a0103f4 	mov	w20, w1
 4030744:	a9015bf5 	stp	x21, x22, [sp, #16]
 4030748:	aa0003f6 	mov	x22, x0
 403074c:	a90263f7 	stp	x23, x24, [sp, #32]
 4030750:	528007f8 	mov	w24, #0x3f                  	// #63
 4030754:	52800017 	mov	w23, #0x0                   	// #0
 4030758:	a9036bf9 	stp	x25, x26, [sp, #48]
 403075c:	52800039 	mov	w25, #0x1                   	// #1
 4030760:	5280081a 	mov	w26, #0x40                  	// #64
 4030764:	a90473fb 	stp	x27, x28, [sp, #64]
 4030768:	5280009b 	mov	w27, #0x4                   	// #4
 403076c:	f9002bfe 	str	x30, [sp, #80]
 4030770:	97ffe8fb 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030774:	b9401400 	ldr	w0, [x0, #20]
 4030778:	7100081f 	cmp	w0, #0x2
 403077c:	52803fe0 	mov	w0, #0x1ff                 	// #511
 4030780:	1a800318 	csel	w24, w24, w0, eq  // eq = none
 4030784:	97ffed80 	bl	402bd84 <ddrtrn_hal_adjust_get_val>
 4030788:	2a0003f5 	mov	w21, w0
 403078c:	2a0003fc 	mov	w28, w0
 4030790:	97ffe8f3 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030794:	b9401401 	ldr	w1, [x0, #20]
 4030798:	7100083f 	cmp	w1, #0x2
 403079c:	54000161 	b.ne	40307c8 <ddrtrn_adjust_move_win.constprop.0+0x8c>  // b.any
 40307a0:	b9006fe1 	str	w1, [sp, #108]
 40307a4:	940000d9 	bl	4030b08 <ddrtrn_hal_adjust_get_average>
 40307a8:	b9406fe1 	ldr	w1, [sp, #108]
 40307ac:	7100dc1f 	cmp	w0, #0x37
 40307b0:	52800033 	mov	w19, #0x1                   	// #1
 40307b4:	54000169 	b.ls	40307e0 <ddrtrn_adjust_move_win.constprop.0+0xa4>  // b.plast
 40307b8:	7101201f 	cmp	w0, #0x48
 40307bc:	540005e8 	b.hi	4030878 <ddrtrn_adjust_move_win.constprop.0+0x13c>  // b.pmore
 40307c0:	52800021 	mov	w1, #0x1                   	// #1
 40307c4:	14000007 	b	40307e0 <ddrtrn_adjust_move_win.constprop.0+0xa4>
 40307c8:	940000d0 	bl	4030b08 <ddrtrn_hal_adjust_get_average>
 40307cc:	7101001f 	cmp	w0, #0x40
 40307d0:	54000489 	b.ls	4030860 <ddrtrn_adjust_move_win.constprop.0+0x124>  // b.plast
 40307d4:	7101201f 	cmp	w0, #0x48
 40307d8:	51010013 	sub	w19, w0, #0x40
 40307dc:	1a998361 	csel	w1, w27, w25, hi  // hi = pmore
 40307e0:	7100043f 	cmp	w1, #0x1
 40307e4:	7a541024 	ccmp	w1, w20, #0x4, ne  // ne = any
 40307e8:	540005c0 	b.eq	40308a0 <ddrtrn_adjust_move_win.constprop.0+0x164>  // b.none
 40307ec:	97ffe8dc 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40307f0:	b9401400 	ldr	w0, [x0, #20]
 40307f4:	7100041f 	cmp	w0, #0x1
 40307f8:	54000441 	b.ne	4030880 <ddrtrn_adjust_move_win.constprop.0+0x144>  // b.any
 40307fc:	0b130380 	add	w0, w28, w19
 4030800:	4b130393 	sub	w19, w28, w19
 4030804:	7100129f 	cmp	w20, #0x4
 4030808:	1a80127c 	csel	w28, w19, w0, ne  // ne = any
 403080c:	97ffe8d4 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030810:	b9401400 	ldr	w0, [x0, #20]
 4030814:	7100041f 	cmp	w0, #0x1
 4030818:	540003a1 	b.ne	403088c <ddrtrn_adjust_move_win.constprop.0+0x150>  // b.any
 403081c:	7107ff9f 	cmp	w28, #0x1ff
 4030820:	54000408 	b.hi	40308a0 <ddrtrn_adjust_move_win.constprop.0+0x164>  // b.pmore
 4030824:	2a1c03e0 	mov	w0, w28
 4030828:	97ffed72 	bl	402bdf0 <ddrtrn_hal_adjust_set_val>
 403082c:	aa1603e0 	mov	x0, x22
 4030830:	97fff163 	bl	402cdbc <ddrtrn_dataeye_deskew>
 4030834:	34000300 	cbz	w0, 4030894 <ddrtrn_adjust_move_win.constprop.0+0x158>
 4030838:	2a1503e0 	mov	w0, w21
 403083c:	97ffed6d 	bl	402bdf0 <ddrtrn_hal_adjust_set_val>
 4030840:	aa1603e0 	mov	x0, x22
 4030844:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4030848:	a94263f7 	ldp	x23, x24, [sp, #32]
 403084c:	a9436bf9 	ldp	x25, x26, [sp, #48]
 4030850:	a94473fb 	ldp	x27, x28, [sp, #64]
 4030854:	f9402bfe 	ldr	x30, [sp, #80]
 4030858:	a8c753f3 	ldp	x19, x20, [sp], #112
 403085c:	17fff158 	b	402cdbc <ddrtrn_dataeye_deskew>
 4030860:	540002e0 	b.eq	40308bc <ddrtrn_adjust_move_win.constprop.0+0x180>  // b.none
 4030864:	4b000353 	sub	w19, w26, w0
 4030868:	7100dc1f 	cmp	w0, #0x37
 403086c:	54fffaa8 	b.hi	40307c0 <ddrtrn_adjust_move_win.constprop.0+0x84>  // b.pmore
 4030870:	52800041 	mov	w1, #0x2                   	// #2
 4030874:	17ffffdb 	b	40307e0 <ddrtrn_adjust_move_win.constprop.0+0xa4>
 4030878:	52800081 	mov	w1, #0x4                   	// #4
 403087c:	17ffffd9 	b	40307e0 <ddrtrn_adjust_move_win.constprop.0+0xa4>
 4030880:	4b130380 	sub	w0, w28, w19
 4030884:	0b130393 	add	w19, w28, w19
 4030888:	17ffffdf 	b	4030804 <ddrtrn_adjust_move_win.constprop.0+0xc8>
 403088c:	7100ff9f 	cmp	w28, #0x3f
 4030890:	17ffffe4 	b	4030820 <ddrtrn_adjust_move_win.constprop.0+0xe4>
 4030894:	110006f7 	add	w23, w23, #0x1
 4030898:	6b1802ff 	cmp	w23, w24
 403089c:	54fff7a9 	b.ls	4030790 <ddrtrn_adjust_move_win.constprop.0+0x54>  // b.plast
 40308a0:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40308a4:	a94263f7 	ldp	x23, x24, [sp, #32]
 40308a8:	a9436bf9 	ldp	x25, x26, [sp, #48]
 40308ac:	a94473fb 	ldp	x27, x28, [sp, #64]
 40308b0:	f9402bfe 	ldr	x30, [sp, #80]
 40308b4:	a8c753f3 	ldp	x19, x20, [sp], #112
 40308b8:	d65f03c0 	ret
 40308bc:	52800033 	mov	w19, #0x1                   	// #1
 40308c0:	17ffffc0 	b	40307c0 <ddrtrn_adjust_move_win.constprop.0+0x84>

00000000040308c4 <ddrtrn_adjust_dataeye>:
 40308c4:	a9bd53f3 	stp	x19, x20, [sp, #-48]!
 40308c8:	aa0003f3 	mov	x19, x0
 40308cc:	52a20000 	mov	w0, #0x10000000            	// #268435456
 40308d0:	a9015bf5 	stp	x21, x22, [sp, #16]
 40308d4:	a9027bf7 	stp	x23, x30, [sp, #32]
 40308d8:	97ffecc8 	bl	402bbf8 <ddrtrn_hal_check_bypass>
 40308dc:	35000220 	cbnz	w0, 4030920 <ddrtrn_adjust_dataeye+0x5c>
 40308e0:	97fff0ea 	bl	402cc88 <ddrtrn_hal_get_adjust>
 40308e4:	340001e0 	cbz	w0, 4030920 <ddrtrn_adjust_dataeye+0x5c>
 40308e8:	52800014 	mov	w20, #0x0                   	// #0
 40308ec:	d2800196 	mov	x22, #0xc                   	// #12
 40308f0:	d2800797 	mov	x23, #0x3c                  	// #60
 40308f4:	97ffe89a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40308f8:	aa0003f5 	mov	x21, x0
 40308fc:	97ffe893 	bl	402ab48 <ddrtrn_hal_get_phy>
 4030900:	b94022a2 	ldr	w2, [x21, #32]
 4030904:	b9402aa1 	ldr	w1, [x21, #40]
 4030908:	9b177c42 	mul	x2, x2, x23
 403090c:	9b160821 	madd	x1, x1, x22, x2
 4030910:	8b010000 	add	x0, x0, x1
 4030914:	b9401800 	ldr	w0, [x0, #24]
 4030918:	6b00029f 	cmp	w20, w0
 403091c:	540000a3 	b.cc	4030930 <ddrtrn_adjust_dataeye+0x6c>  // b.lo, b.ul, b.last
 4030920:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4030924:	a9427bf7 	ldp	x23, x30, [sp, #32]
 4030928:	a8c353f3 	ldp	x19, x20, [sp], #48
 403092c:	d65f03c0 	ret
 4030930:	97ffe88b 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030934:	b9402815 	ldr	w21, [x0, #40]
 4030938:	97ffe889 	bl	402ab5c <ddrtrn_hal_get_ctx>
 403093c:	0b150695 	add	w21, w20, w21, lsl #1
 4030940:	b9001815 	str	w21, [x0, #24]
 4030944:	94000071 	bl	4030b08 <ddrtrn_hal_adjust_get_average>
 4030948:	7100dc1f 	cmp	w0, #0x37
 403094c:	540000a8 	b.hi	4030960 <ddrtrn_adjust_dataeye+0x9c>  // b.pmore
 4030950:	52800081 	mov	w1, #0x4                   	// #4
 4030954:	aa1303e0 	mov	x0, x19
 4030958:	97ffff79 	bl	403073c <ddrtrn_adjust_move_win.constprop.0>
 403095c:	14000003 	b	4030968 <ddrtrn_adjust_dataeye+0xa4>
 4030960:	7101201f 	cmp	w0, #0x48
 4030964:	54000068 	b.hi	4030970 <ddrtrn_adjust_dataeye+0xac>  // b.pmore
 4030968:	11000694 	add	w20, w20, #0x1
 403096c:	17ffffe2 	b	40308f4 <ddrtrn_adjust_dataeye+0x30>
 4030970:	52800041 	mov	w1, #0x2                   	// #2
 4030974:	17fffff8 	b	4030954 <ddrtrn_adjust_dataeye+0x90>

0000000004030978 <ddrtrn_reg_config>:
 4030978:	52800002 	mov	w2, #0x0                   	// #0
 403097c:	6b01005f 	cmp	w2, w1
 4030980:	54000041 	b.ne	4030988 <ddrtrn_reg_config+0x10>  // b.any
 4030984:	d65f03c0 	ret
 4030988:	34000042 	cbz	w2, 4030990 <ddrtrn_reg_config+0x18>
 403098c:	91002000 	add	x0, x0, #0x8
 4030990:	29401003 	ldp	w3, w4, [x0]
 4030994:	11000442 	add	w2, w2, #0x1
 4030998:	b9000064 	str	w4, [x3]
 403099c:	17fffff8 	b	403097c <ddrtrn_reg_config+0x4>

00000000040309a0 <ddrtrn_save_rdqbdl_phy>:
 40309a0:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 40309a4:	52800013 	mov	w19, #0x0                   	// #0
 40309a8:	a9015bf5 	stp	x21, x22, [sp, #16]
 40309ac:	aa0003f5 	mov	x21, x0
 40309b0:	a90263f7 	stp	x23, x24, [sp, #32]
 40309b4:	d2800797 	mov	x23, #0x3c                  	// #60
 40309b8:	52801418 	mov	w24, #0xa0                  	// #160
 40309bc:	a9037bf9 	stp	x25, x30, [sp, #48]
 40309c0:	97ffe867 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40309c4:	b9400000 	ldr	w0, [x0]
 40309c8:	6b00027f 	cmp	w19, w0
 40309cc:	540000c3 	b.cc	40309e4 <ddrtrn_save_rdqbdl_phy+0x44>  // b.lo, b.ul, b.last
 40309d0:	a9415bf5 	ldp	x21, x22, [sp, #16]
 40309d4:	a94263f7 	ldp	x23, x24, [sp, #32]
 40309d8:	a9437bf9 	ldp	x25, x30, [sp, #48]
 40309dc:	a8c453f3 	ldp	x19, x20, [sp], #64
 40309e0:	d65f03c0 	ret
 40309e4:	97ffe85e 	bl	402ab5c <ddrtrn_hal_get_ctx>
 40309e8:	b9002013 	str	w19, [x0, #32]
 40309ec:	d503201f 	nop
 40309f0:	9bb85676 	umaddl	x22, w19, w24, x21
 40309f4:	97ffe855 	bl	402ab48 <ddrtrn_hal_get_phy>
 40309f8:	2a1303e1 	mov	w1, w19
 40309fc:	9b177c21 	mul	x1, x1, x23
 4030a00:	b8616814 	ldr	w20, [x0, x1]
 4030a04:	97ffe856 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030a08:	b9000414 	str	w20, [x0, #4]
 4030a0c:	97ffe854 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030a10:	aa0003f4 	mov	x20, x0
 4030a14:	97ffe84d 	bl	402ab48 <ddrtrn_hal_get_phy>
 4030a18:	b9402281 	ldr	w1, [x20, #32]
 4030a1c:	52800014 	mov	w20, #0x0                   	// #0
 4030a20:	9b170021 	madd	x1, x1, x23, x0
 4030a24:	b9400c39 	ldr	w25, [x1, #12]
 4030a28:	6b19029f 	cmp	w20, w25
 4030a2c:	54000061 	b.ne	4030a38 <ddrtrn_save_rdqbdl_phy+0x98>  // b.any
 4030a30:	11000673 	add	w19, w19, #0x1
 4030a34:	17ffffe3 	b	40309c0 <ddrtrn_save_rdqbdl_phy+0x20>
 4030a38:	97ffe849 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030a3c:	b9002414 	str	w20, [x0, #36]
 4030a40:	aa1603e0 	mov	x0, x22
 4030a44:	11000694 	add	w20, w20, #0x1
 4030a48:	910142d6 	add	x22, x22, #0x50
 4030a4c:	97ffefc0 	bl	402c94c <ddrtrn_hal_save_rdqbdl>
 4030a50:	17fffff6 	b	4030a28 <ddrtrn_save_rdqbdl_phy+0x88>

0000000004030a54 <ddrtrn_restore_rdqbdl_phy>:
 4030a54:	a9bc53f3 	stp	x19, x20, [sp, #-64]!
 4030a58:	52800013 	mov	w19, #0x0                   	// #0
 4030a5c:	a9015bf5 	stp	x21, x22, [sp, #16]
 4030a60:	aa0003f5 	mov	x21, x0
 4030a64:	a90263f7 	stp	x23, x24, [sp, #32]
 4030a68:	d2800797 	mov	x23, #0x3c                  	// #60
 4030a6c:	52801418 	mov	w24, #0xa0                  	// #160
 4030a70:	a9037bf9 	stp	x25, x30, [sp, #48]
 4030a74:	97ffe83a 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030a78:	b9400000 	ldr	w0, [x0]
 4030a7c:	6b00027f 	cmp	w19, w0
 4030a80:	540000c3 	b.cc	4030a98 <ddrtrn_restore_rdqbdl_phy+0x44>  // b.lo, b.ul, b.last
 4030a84:	a9415bf5 	ldp	x21, x22, [sp, #16]
 4030a88:	a94263f7 	ldp	x23, x24, [sp, #32]
 4030a8c:	a9437bf9 	ldp	x25, x30, [sp, #48]
 4030a90:	a8c453f3 	ldp	x19, x20, [sp], #64
 4030a94:	d65f03c0 	ret
 4030a98:	97ffe831 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030a9c:	b9002013 	str	w19, [x0, #32]
 4030aa0:	d503201f 	nop
 4030aa4:	9bb85676 	umaddl	x22, w19, w24, x21
 4030aa8:	97ffe828 	bl	402ab48 <ddrtrn_hal_get_phy>
 4030aac:	2a1303e1 	mov	w1, w19
 4030ab0:	9b177c21 	mul	x1, x1, x23
 4030ab4:	b8616814 	ldr	w20, [x0, x1]
 4030ab8:	97ffe829 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030abc:	b9000414 	str	w20, [x0, #4]
 4030ac0:	97ffe827 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030ac4:	aa0003f4 	mov	x20, x0
 4030ac8:	97ffe820 	bl	402ab48 <ddrtrn_hal_get_phy>
 4030acc:	b9402281 	ldr	w1, [x20, #32]
 4030ad0:	52800014 	mov	w20, #0x0                   	// #0
 4030ad4:	9b170021 	madd	x1, x1, x23, x0
 4030ad8:	b9400c39 	ldr	w25, [x1, #12]
 4030adc:	6b19029f 	cmp	w20, w25
 4030ae0:	54000061 	b.ne	4030aec <ddrtrn_restore_rdqbdl_phy+0x98>  // b.any
 4030ae4:	11000673 	add	w19, w19, #0x1
 4030ae8:	17ffffe3 	b	4030a74 <ddrtrn_restore_rdqbdl_phy+0x20>
 4030aec:	97ffe81c 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030af0:	b9002414 	str	w20, [x0, #36]
 4030af4:	aa1603e0 	mov	x0, x22
 4030af8:	11000694 	add	w20, w20, #0x1
 4030afc:	910142d6 	add	x22, x22, #0x50
 4030b00:	97ffefb6 	bl	402c9d8 <ddrtrn_hal_restore_rdqbdl>
 4030b04:	17fffff6 	b	4030adc <ddrtrn_restore_rdqbdl_phy+0x88>

0000000004030b08 <ddrtrn_hal_adjust_get_average>:
 4030b08:	a9be53f3 	stp	x19, x20, [sp, #-32]!
 4030b0c:	a9017bf5 	stp	x21, x30, [sp, #16]
 4030b10:	97ffe813 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030b14:	b9400415 	ldr	w21, [x0, #4]
 4030b18:	97ffe811 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030b1c:	b9401814 	ldr	w20, [x0, #24]
 4030b20:	97ffe80f 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030b24:	b9402413 	ldr	w19, [x0, #36]
 4030b28:	97ffe80d 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030b2c:	b9401400 	ldr	w0, [x0, #20]
 4030b30:	53196281 	lsl	w1, w20, #7
 4030b34:	7100081f 	cmp	w0, #0x2
 4030b38:	53165660 	lsl	w0, w19, #10
 4030b3c:	0b010000 	add	w0, w0, w1
 4030b40:	0b150000 	add	w0, w0, w21
 4030b44:	54000301 	b.ne	4030ba4 <ddrtrn_hal_adjust_get_average+0x9c>  // b.any
 4030b48:	11084001 	add	w1, w0, #0x210
 4030b4c:	11085000 	add	w0, w0, #0x214
 4030b50:	b9400021 	ldr	w1, [x1]
 4030b54:	b9400002 	ldr	w2, [x0]
 4030b58:	d3505823 	ubfx	x3, x1, #16, #7
 4030b5c:	d3483820 	ubfx	x0, x1, #8, #7
 4030b60:	12001844 	and	w4, w2, #0x7f
 4030b64:	0b030000 	add	w0, w0, w3
 4030b68:	12001823 	and	w3, w1, #0x7f
 4030b6c:	d3587821 	ubfx	x1, x1, #24, #7
 4030b70:	0b040063 	add	w3, w3, w4
 4030b74:	0b030000 	add	w0, w0, w3
 4030b78:	d3483843 	ubfx	x3, x2, #8, #7
 4030b7c:	0b030021 	add	w1, w1, w3
 4030b80:	0b010000 	add	w0, w0, w1
 4030b84:	d3505841 	ubfx	x1, x2, #16, #7
 4030b88:	d3587842 	ubfx	x2, x2, #24, #7
 4030b8c:	0b020022 	add	w2, w1, w2
 4030b90:	0b020000 	add	w0, w0, w2
 4030b94:	a9417bf5 	ldp	x21, x30, [sp, #16]
 4030b98:	53037c00 	lsr	w0, w0, #3
 4030b9c:	a8c253f3 	ldp	x19, x20, [sp], #32
 4030ba0:	d65f03c0 	ret
 4030ba4:	11087001 	add	w1, w0, #0x21c
 4030ba8:	11088000 	add	w0, w0, #0x220
 4030bac:	b9400021 	ldr	w1, [x1]
 4030bb0:	17ffffe9 	b	4030b54 <ddrtrn_hal_adjust_get_average+0x4c>

0000000004030bb4 <ddrtrn_hal_adjust_get_rdqs>:
 4030bb4:	a9bf7bf3 	stp	x19, x30, [sp, #-16]!
 4030bb8:	97ffe7e9 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030bbc:	b9400413 	ldr	w19, [x0, #4]
 4030bc0:	97ffe7e7 	bl	402ab5c <ddrtrn_hal_get_ctx>
 4030bc4:	b9401800 	ldr	w0, [x0, #24]
 4030bc8:	1108b273 	add	w19, w19, #0x22c
 4030bcc:	0b001e60 	add	w0, w19, w0, lsl #7
 4030bd0:	a8c17bf3 	ldp	x19, x30, [sp], #16
 4030bd4:	b9400000 	ldr	w0, [x0]
 4030bd8:	12002000 	and	w0, w0, #0x1ff
 4030bdc:	d65f03c0 	ret

Disassembly of section .vectors:

0000000004031000 <sync_exception_sp_el0>:
	...

0000000004031080 <irq_sp_el0>:
	...

0000000004031100 <fiq_sp_el0>:
	...

0000000004031180 <serror_sp_el0>:
	...

0000000004031200 <sync_exception_sp_elx>:
	...

0000000004031280 <irq_sp_elx>:
	...

0000000004031300 <fiq_sp_elx>:
	...

0000000004031380 <serror_sp_elx>:
	...

0000000004031400 <sync_exception_aarch64>:
 4031400:	94000020 	bl	4031480 <el1_to_el3_entry>
	...

0000000004031480 <el1_to_el3_entry>:
 4031480:	58000540 	ldr	x0, 4031528 <power_down_sequence_end>
 4031484:	927cec1f 	and	sp, x0, #0xfffffffffffffff0
 4031488:	58000553 	ldr	x19, 4031530 <power_down_sequence_end+0x8>
 403148c:	97ffc9ee 	bl	4023c44 <get_ddr_param_data_end_addr>
 4031490:	cb130014 	sub	x20, x0, x19
 4031494:	58000520 	ldr	x0, 4031538 <power_down_sequence_end+0x10>
 4031498:	58000541 	ldr	x1, 4031540 <power_down_sequence_end+0x18>
 403149c:	cb000022 	sub	x2, x1, x0
 40314a0:	58000541 	ldr	x1, 4031548 <power_down_sequence_end+0x20>
 40314a4:	97ffc6b1 	bl	4022f68 <copy_code_to_sram>
 40314a8:	97ffd374 	bl	4026278 <sram_to_npu_info>
 40314ac:	580004fe 	ldr	x30, 4031548 <power_down_sequence_end+0x20>
 40314b0:	d65f03c0 	ret
 40314b4:	d503201f 	nop

00000000040314b8 <power_down_sequence_start>:
 40314b8:	aa1303e0 	mov	x0, x19
 40314bc:	d2800001 	mov	x1, #0x0                   	// #0
 40314c0:	aa1403e2 	mov	x2, x20
 40314c4:	94000010 	bl	4031504 <clear_data>
 40314c8:	d28a0481 	mov	x1, #0x5024                	// #20516
 40314cc:	f2a22041 	movk	x1, #0x1102, lsl #16
 40314d0:	b9400020 	ldr	w0, [x1]
 40314d4:	32000000 	orr	w0, w0, #0x1
 40314d8:	b9000020 	str	w0, [x1]
 40314dc:	58000223 	ldr	x3, 4031520 <_MISC_REG_CPU_CTRL6>
 40314e0:	b9400061 	ldr	w1, [x3]
 40314e4:	321a0021 	orr	w1, w1, #0x40
 40314e8:	b9000061 	str	w1, [x3]
 40314ec:	d538f2e0 	mrs	x0, s3_0_c15_c2_7
 40314f0:	b2400000 	orr	x0, x0, #0x1
 40314f4:	d518f2e0 	msr	s3_0_c15_c2_7, x0
 40314f8:	d5033fdf 	isb
 40314fc:	d503207f 	wfi
 4031500:	d65f03c0 	ret

0000000004031504 <clear_data>:
 4031504:	8b020002 	add	x2, x0, x2
 4031508:	d2800103 	mov	x3, #0x8                   	// #8

000000000403150c <clear_data_loop>:
 403150c:	f9000001 	str	x1, [x0]
 4031510:	8b030000 	add	x0, x0, x3
 4031514:	eb02001f 	cmp	x0, x2
 4031518:	54ffffab 	b.lt	403150c <clear_data_loop>  // b.tstop
 403151c:	d65f03c0 	ret

0000000004031520 <_MISC_REG_CPU_CTRL6>:
 4031520:	11024128 	add	w8, w9, #0x90
 4031524:	00000000 	udf	#0

0000000004031528 <power_down_sequence_end>:
 4031528:	0403f000 	msb	z0.b, p4/m, z3.b, z0.b
 403152c:	00000000 	udf	#0
 4031530:	04020000 	.inst	0x04020000 ; undefined
 4031534:	00000000 	udf	#0
 4031538:	040314b8 	subr	z24.b, p5/m, z24.b, z5.b
 403153c:	00000000 	udf	#0
 4031540:	04031528 	subr	z8.b, p5/m, z8.b, z9.b
 4031544:	00000000 	udf	#0
 4031548:	4000f000 	.inst	0x4000f000 ; undefined
 403154c:	00000000 	udf	#0

Disassembly of section .rodata:

0000000004031550 <g_rom_ecc>:
 4031550:	04031630 	subr	z16.b, p5/m, z16.b, z17.b
 4031554:	00000000 	udf	#0
 4031558:	04031610 	subr	z16.b, p5/m, z16.b, z16.b
 403155c:	00000000 	udf	#0
 4031560:	040315f0 	subr	z16.b, p5/m, z16.b, z15.b
 4031564:	00000000 	udf	#0
 4031568:	040315d0 	subr	z16.b, p5/m, z16.b, z14.b
 403156c:	00000000 	udf	#0
 4031570:	040315b0 	subr	z16.b, p5/m, z16.b, z13.b
 4031574:	00000000 	udf	#0
 4031578:	04031590 	subr	z16.b, p5/m, z16.b, z12.b
 403157c:	00000000 	udf	#0
 4031580:	00000001 	udf	#1
 4031584:	00000020 	udf	#32
	...

0000000004031590 <rom_ecc_n>:
 4031590:	db57fba9 	.inst	0xdb57fba9 ; undefined
 4031594:	bca9eea1 	.inst	0xbca9eea1 ; undefined
 4031598:	900a663e 	adrp	x30, 18cf5000 <_end+0x14cc0c00>
 403159c:	718d839d 	.inst	0x718d839d ; undefined
 40315a0:	a37a398c 	.inst	0xa37a398c ; undefined
 40315a4:	f7a661b5 	.inst	0xf7a661b5 ; undefined
 40315a8:	820e1e90 	.inst	0x820e1e90 ; undefined
 40315ac:	a7564897 	.inst	0xa7564897 ; undefined

00000000040315b0 <rom_ecc_gy>:
 40315b0:	35f87e54 	cbnz	w20, 4022578 <__bss_end+0x528>
 40315b4:	fdc4dac3 	.inst	0xfdc4dac3 ; undefined
 40315b8:	1a46f897 	.inst	0x1a46f897 ; undefined
 40315bc:	c91d6114 	.inst	0xc91d6114 ; undefined
 40315c0:	134577c2 	.inst	0x134577c2 ; undefined
 40315c4:	548eed2d 	b.le	3f4f368 <gs_misc_record-0xd2718>
 40315c8:	c7541d5c 	.inst	0xc7541d5c ; undefined
 40315cc:	9769042f 	bl	1a72688 <gs_misc_record-0x25af3f8>

00000000040315d0 <rom_ecc_gx>:
 40315d0:	b9aed28b 	ldrsw	x11, [x20, #11984]
 40315d4:	cb577ecb 	sub	x11, x22, x23, lsr #31
 40315d8:	2f484b2c 	mls	v12.4h, v25.4h, v8.h[4]
 40315dc:	afb781fc 	.inst	0xafb781fc ; undefined
 40315e0:	e127deb9 	.inst	0xe127deb9 ; undefined
 40315e4:	c223bde3 	.inst	0xc223bde3 ; undefined
 40315e8:	bd53443a 	ldr	s26, [x1, #4932]
 40315ec:	6232ce9a 	.inst	0x6232ce9a ; undefined

00000000040315f0 <rom_ecc_b>:
 40315f0:	6c5cdc26 	ldnp	d6, d23, [x1, #456]
 40315f4:	444b4ae9 	umlalb	z9.h, z23.b, z11.b
 40315f8:	d9b530f3 	.inst	0xd9b530f3 ; undefined
 40315fc:	bf7cd7bb 	.inst	0xbf7cd7bb ; undefined
 4031600:	29168495 	stp	w21, w1, [x4, #180]
 4031604:	cee1f75c 	.inst	0xcee1f75c ; undefined
 4031608:	18dccc6b 	ldr	w11, 3feaf94 <gs_misc_record-0x36aec>
 403160c:	b6078cff 	tbz	xzr, #32, 40307a8 <ddrtrn_adjust_move_win.constprop.0+0x6c>

0000000004031610 <rom_ecc_a>:
 4031610:	75095a7d 	.inst	0x75095a7d ; undefined
 4031614:	57302cfc 	.inst	0x57302cfc ; undefined
 4031618:	3075f6ee 	adr	x14, 411d4f5 <_end+0xe90f5>
 403161c:	e7ff7a41 	.inst	0xe7ff7a41 ; undefined
 4031620:	c15580fb 	.inst	0xc15580fb ; undefined
 4031624:	6c5cdc26 	ldnp	d6, d23, [x1, #456]
 4031628:	444b4ae9 	umlalb	z9.h, z23.b, z11.b
 403162c:	d9b530f3 	.inst	0xd9b530f3 ; undefined

0000000004031630 <rom_ecc_p>:
 4031630:	db57fba9 	.inst	0xdb57fba9 ; undefined
 4031634:	bca9eea1 	.inst	0xbca9eea1 ; undefined
 4031638:	900a663e 	adrp	x30, 18cf5000 <_end+0x14cc0c00>
 403163c:	728d839d 	movk	w29, #0x6c1c
 4031640:	23f63b6e 	.inst	0x23f63b6e ; undefined
 4031644:	282026d5 	stnp	w21, w9, [x22, #-256]
 4031648:	1d481320 	.inst	0x1d481320 ; undefined
 403164c:	77536e1f 	.inst	0x77536e1f ; undefined

0000000004031650 <g_crc_ta>:
 4031650:	10210000 	adr	x0, 4073650 <_end+0x3f250>
 4031654:	30632042 	adr	x2, 40f7a5d <_end+0xc365d>
 4031658:	50a54084 	adr	x4, 3f7be6a <gs_misc_record-0xa5c16>
 403165c:	70e760c6 	adr	x6, 4000277 <gs_misc_record-0x21809>
 4031660:	91298108 	add	x8, x8, #0xa60
 4031664:	b16ba14a 	adds	x10, x10, #0xae8, lsl #12
 4031668:	d1adc18c 	.inst	0xd1adc18c ; undefined
 403166c:	f1efe1ce 	.inst	0xf1efe1ce ; undefined

0000000004031670 <crc32_tab>:
 4031670:	00000000 	udf	#0
 4031674:	77073096 	.inst	0x77073096 ; undefined
 4031678:	ee0e612c 	.inst	0xee0e612c ; undefined
 403167c:	990951ba 	stlur	w26, [x13, #149]
 4031680:	076dc419 	.inst	0x076dc419 ; undefined
 4031684:	706af48f 	adr	x15, 4107517 <_end+0xd3117>
 4031688:	e963a535 	.inst	0xe963a535 ; undefined
 403168c:	9e6495a3 	.inst	0x9e6495a3 ; undefined
 4031690:	0edb8832 	.inst	0x0edb8832 ; undefined
 4031694:	79dcb8a4 	ldrsh	w4, [x5, #3676]
 4031698:	e0d5e91e 	.inst	0xe0d5e91e ; undefined
 403169c:	97d2d988 	bl	34e7cbc <gs_misc_record-0xb39dc4>
 40316a0:	09b64c2b 	.inst	0x09b64c2b ; undefined
 40316a4:	7eb17cbd 	.inst	0x7eb17cbd ; undefined
 40316a8:	e7b82d07 	.inst	0xe7b82d07 ; undefined
 40316ac:	90bf1d91 	adrp	x17, ffffffff823e1000 <_end+0xffffffff7e3acc00>
 40316b0:	1db71064 	.inst	0x1db71064 ; undefined
 40316b4:	6ab020f2 	bics	w18, w7, w16, asr #8
 40316b8:	f3b97148 	.inst	0xf3b97148 ; undefined
 40316bc:	84be41de 	ld1h	{z30.s}, p0/z, [x14, z30.s, uxtw #1]
 40316c0:	1adad47d 	.inst	0x1adad47d ; undefined
 40316c4:	6ddde4eb 	ldp	d11, d25, [x7, #472]!
 40316c8:	f4d4b551 	.inst	0xf4d4b551 ; undefined
 40316cc:	83d385c7 	.inst	0x83d385c7 ; undefined
 40316d0:	136c9856 	.inst	0x136c9856 ; undefined
 40316d4:	646ba8c0 	.inst	0x646ba8c0 ; undefined
 40316d8:	fd62f97a 	ldr	d26, [x11, #17904]
 40316dc:	8a65c9ec 	bic	x12, x15, x5, lsr #50
 40316e0:	14015c4f 	b	408881c <_end+0x5441c>
 40316e4:	63066cd9 	.inst	0x63066cd9 ; undefined
 40316e8:	fa0f3d63 	.inst	0xfa0f3d63 ; undefined
 40316ec:	8d080df5 	.inst	0x8d080df5 ; undefined
 40316f0:	3b6e20c8 	.inst	0x3b6e20c8 ; undefined
 40316f4:	4c69105e 	.inst	0x4c69105e ; undefined
 40316f8:	d56041e4 	.inst	0xd56041e4 ; undefined
 40316fc:	a2677172 	.inst	0xa2677172 ; undefined
 4031700:	3c03e4d1 	str	b17, [x6], #62
 4031704:	4b04d447 	.inst	0x4b04d447 ; undefined
 4031708:	d20d85fd 	eor	x29, x15, #0x18001800180018
 403170c:	a50ab56b 	ld1sh	{z11.d}, p5/z, [x11, #-6, mul vl]
 4031710:	35b5a8fa 	cbnz	w26, 3f9cc2c <gs_misc_record-0x84e54>
 4031714:	42b2986c 	.inst	0x42b2986c ; undefined
 4031718:	dbbbc9d6 	.inst	0xdbbbc9d6 ; undefined
 403171c:	acbcf940 	stp	q0, q30, [x10], #-112
 4031720:	32d86ce3 	.inst	0x32d86ce3 ; undefined
 4031724:	45df5c75 	usubwt	z21.d, z3.d, z31.s
 4031728:	dcd60dcf 	.inst	0xdcd60dcf ; undefined
 403172c:	abd13d59 	.inst	0xabd13d59 ; undefined
 4031730:	26d930ac 	.inst	0x26d930ac ; undefined
 4031734:	51de003a 	.inst	0x51de003a ; undefined
 4031738:	c8d75180 	ldlar	x0, [x12]
 403173c:	bfd06116 	.inst	0xbfd06116 ; undefined
 4031740:	21b4f4b5 	.inst	0x21b4f4b5 ; undefined
 4031744:	56b3c423 	.inst	0x56b3c423 ; undefined
 4031748:	cfba9599 	.inst	0xcfba9599 ; undefined
 403174c:	b8bda50f 	.inst	0xb8bda50f ; undefined
 4031750:	2802b89e 	stnp	w30, w14, [x4, #20]
 4031754:	5f058808 	.inst	0x5f058808 ; undefined
 4031758:	c60cd9b2 	.inst	0xc60cd9b2 ; undefined
 403175c:	b10be924 	adds	x4, x9, #0x2fa
 4031760:	2f6f7c87 	.inst	0x2f6f7c87 ; undefined
 4031764:	58684c11 	ldr	x17, 41020e4 <_end+0xcdce4>
 4031768:	c1611dab 	.inst	0xc1611dab ; undefined
 403176c:	b6662d3d 	tbz	x29, #44, 402dd10 <ddrtrn_ddrt_init+0x74>
 4031770:	76dc4190 	.inst	0x76dc4190 ; undefined
 4031774:	01db7106 	.inst	0x01db7106 ; undefined
 4031778:	98d220bc 	ldrsw	x28, 3fd5b8c <gs_misc_record-0x4bef4>
 403177c:	efd5102a 	.inst	0xefd5102a ; undefined
 4031780:	71b18589 	.inst	0x71b18589 ; undefined
 4031784:	06b6b51f 	.inst	0x06b6b51f ; undefined
 4031788:	9fbfe4a5 	.inst	0x9fbfe4a5 ; undefined
 403178c:	e8b8d433 	.inst	0xe8b8d433 ; undefined
 4031790:	7807c9a2 	sttrh	w2, [x13, #124]
 4031794:	0f00f934 	sudot	v20.2s, v9.8b, v0.4b[2]
 4031798:	9609a88e 	bl	fffffffffc29b9d0 <_end+0xfffffffff82675d0>
 403179c:	e10e9818 	.inst	0xe10e9818 ; undefined
 40317a0:	7f6a0dbb 	.inst	0x7f6a0dbb ; undefined
 40317a4:	086d3d2d 	.inst	0x086d3d2d ; undefined
 40317a8:	91646c97 	add	x23, x4, #0x91b, lsl #12
 40317ac:	e6635c01 	.inst	0xe6635c01 ; undefined
 40317b0:	6b6b51f4 	.inst	0x6b6b51f4 ; undefined
 40317b4:	1c6c6162 	ldr	s2, 410a3e0 <_end+0xd5fe0>
 40317b8:	856530d8 	.inst	0x856530d8 ; undefined
 40317bc:	f262004e 	ands	x14, x2, #0x40000000
 40317c0:	6c0695ed 	stnp	d13, d5, [x15, #104]
 40317c4:	1b01a57b 	msub	w27, w11, w1, w9
 40317c8:	8208f4c1 	.inst	0x8208f4c1 ; undefined
 40317cc:	f50fc457 	.inst	0xf50fc457 ; undefined
 40317d0:	65b0d9c6 	fnmad	z6.s, p6/m, z14.s, z16.s
 40317d4:	12b7e950 	mov	w16, #0x40b5ffff            	// #1085669375
 40317d8:	8bbeb8ea 	.inst	0x8bbeb8ea ; undefined
 40317dc:	fcb9887c 	.inst	0xfcb9887c ; undefined
 40317e0:	62dd1ddf 	.inst	0x62dd1ddf ; undefined
 40317e4:	15da2d49 	b	b6bcd08 <_end+0x7688908>
 40317e8:	8cd37cf3 	.inst	0x8cd37cf3 ; undefined
 40317ec:	fbd44c65 	.inst	0xfbd44c65 ; undefined
 40317f0:	4db26158 	st4	{v24.h-v27.h}[4], [x10], x18
 40317f4:	3ab551ce 	.inst	0x3ab551ce ; undefined
 40317f8:	a3bc0074 	.inst	0xa3bc0074 ; undefined
 40317fc:	d4bb30e2 	dcps2	#0xd987
 4031800:	4adfa541 	.inst	0x4adfa541 ; undefined
 4031804:	3dd895d7 	ldr	q23, [x14, #25168]
 4031808:	a4d1c46d 	ld3h	{z13.h-z15.h}, p1/z, [x3, x17, lsl #1]
 403180c:	d3d6f4fb 	.inst	0xd3d6f4fb ; undefined
 4031810:	4369e96a 	.inst	0x4369e96a ; undefined
 4031814:	346ed9fc 	cbz	w28, 410f350 <_end+0xdaf50>
 4031818:	ad678846 	ldp	q6, q2, [x2, #-784]
 403181c:	da60b8d0 	.inst	0xda60b8d0 ; undefined
 4031820:	44042d73 	cmla	z19.b, z11.b, z4.b, #270
 4031824:	33031de5 	bfxil	w5, w15, #3, #5
 4031828:	aa0a4c5f 	orr	xzr, x2, x10, lsl #19
 403182c:	dd0d7cc9 	.inst	0xdd0d7cc9 ; undefined
 4031830:	5005713c 	adr	x28, 403c656 <_end+0x8256>
 4031834:	270241aa 	.inst	0x270241aa ; undefined
 4031838:	be0b1010 	.inst	0xbe0b1010 ; undefined
 403183c:	c90c2086 	.inst	0xc90c2086 ; undefined
 4031840:	5768b525 	.inst	0x5768b525 ; undefined
 4031844:	206f85b3 	.inst	0x206f85b3 ; undefined
 4031848:	b966d409 	ldr	w9, [x0, #9940]
 403184c:	ce61e49f 	.inst	0xce61e49f ; undefined
 4031850:	5edef90e 	.inst	0x5edef90e ; undefined
 4031854:	29d9c998 	ldp	w24, w18, [x12, #204]!
 4031858:	b0d09822 	adrp	x2, ffffffffa5336000 <_end+0xffffffffa1301c00>
 403185c:	c7d7a8b4 	.inst	0xc7d7a8b4 ; undefined
 4031860:	59b33d17 	.inst	0x59b33d17 ; undefined
 4031864:	2eb40d81 	uqadd	v1.2s, v12.2s, v20.2s
 4031868:	b7bd5c3b 	tbnz	x27, #55, 402c3ec <ddrtrn_hal_training_adjust_wdqs+0x100>
 403186c:	c0ba6cad 	.inst	0xc0ba6cad ; undefined
 4031870:	edb88320 	.inst	0xedb88320 ; undefined
 4031874:	9abfb3b6 	.inst	0x9abfb3b6 ; undefined
 4031878:	03b6e20c 	.inst	0x03b6e20c ; undefined
 403187c:	74b1d29a 	.inst	0x74b1d29a ; undefined
 4031880:	ead54739 	ands	x25, x25, x21, ror #17
 4031884:	9dd277af 	.inst	0x9dd277af ; undefined
 4031888:	04db2615 	.inst	0x04db2615 ; undefined
 403188c:	73dc1683 	.inst	0x73dc1683 ; undefined
 4031890:	e3630b12 	.inst	0xe3630b12 ; undefined
 4031894:	94643b84 	bl	59406a4 <_end+0x190c2a4>
 4031898:	0d6d6a3e 	.inst	0x0d6d6a3e ; undefined
 403189c:	7a6a5aa8 	.inst	0x7a6a5aa8 ; undefined
 40318a0:	e40ecf0b 	st1b	{z11.d}, p3, [x24, z14.d, sxtw]
 40318a4:	9309ff9d 	.inst	0x9309ff9d ; undefined
 40318a8:	0a00ae27 	.inst	0x0a00ae27 ; undefined
 40318ac:	7d079eb1 	str	h17, [x21, #974]
 40318b0:	f00f9344 	adrp	x4, 2329c000 <_end+0x1f267c00>
 40318b4:	8708a3d2 	.inst	0x8708a3d2 ; undefined
 40318b8:	1e01f268 	.inst	0x1e01f268 ; undefined
 40318bc:	6906c2fe 	stgp	x30, x16, [x23, #208]
 40318c0:	f762575d 	.inst	0xf762575d ; undefined
 40318c4:	806567cb 	.inst	0x806567cb ; undefined
 40318c8:	196c3671 	.inst	0x196c3671 ; undefined
 40318cc:	6e6b06e7 	uhadd	v7.8h, v23.8h, v11.8h
 40318d0:	fed41b76 	.inst	0xfed41b76 ; undefined
 40318d4:	89d32be0 	.inst	0x89d32be0 ; undefined
 40318d8:	10da7a5a 	adr	x26, 3fe6820 <gs_misc_record-0x3b260>
 40318dc:	67dd4acc 	.inst	0x67dd4acc ; undefined
 40318e0:	f9b9df6f 	prfm	#0x0f, [x27, #29624]
 40318e4:	8ebeeff9 	.inst	0x8ebeeff9 ; undefined
 40318e8:	17b7be43 	b	2e211f4 <gs_misc_record-0x120088c>
 40318ec:	60b08ed5 	.inst	0x60b08ed5 ; undefined
 40318f0:	d6d6a3e8 	.inst	0xd6d6a3e8 ; undefined
 40318f4:	a1d1937e 	.inst	0xa1d1937e ; undefined
 40318f8:	38d8c2c4 	ldursb	w4, [x22, #-116]
 40318fc:	4fdff252 	bfmlalt	v18.4s, v18.8h, v15.h[1]
 4031900:	d1bb67f1 	.inst	0xd1bb67f1 ; undefined
 4031904:	a6bc5767 	.inst	0xa6bc5767 ; undefined
 4031908:	3fb506dd 	.inst	0x3fb506dd ; undefined
 403190c:	48b2364b 	.inst	0x48b2364b ; undefined
 4031910:	d80d2bda 	prfm	#0x1a, 404be88 <_end+0x17a88>
 4031914:	af0a1b4c 	.inst	0xaf0a1b4c ; undefined
 4031918:	36034af6 	tbz	w22, #0, 4038274 <_end+0x3e74>
 403191c:	41047a60 	.inst	0x41047a60 ; undefined
 4031920:	df60efc3 	.inst	0xdf60efc3 ; undefined
 4031924:	a867df55 	ldnp	x21, x23, [x26, #-392]
 4031928:	316e8eef 	adds	w15, w23, #0xba3, lsl #12
 403192c:	4669be79 	.inst	0x4669be79 ; undefined
 4031930:	cb61b38c 	.inst	0xcb61b38c ; undefined
 4031934:	bc66831a 	.inst	0xbc66831a ; undefined
 4031938:	256fd2a0 	.inst	0x256fd2a0 ; undefined
 403193c:	5268e236 	.inst	0x5268e236 ; undefined
 4031940:	cc0c7795 	.inst	0xcc0c7795 ; undefined
 4031944:	bb0b4703 	.inst	0xbb0b4703 ; undefined
 4031948:	220216b9 	.inst	0x220216b9 ; undefined
 403194c:	5505262f 	.inst	0x5505262f ; undefined
 4031950:	c5ba3bbe 	.inst	0xc5ba3bbe ; undefined
 4031954:	b2bd0b28 	.inst	0xb2bd0b28 ; undefined
 4031958:	2bb45a92 	.inst	0x2bb45a92 ; undefined
 403195c:	5cb36a04 	ldr	d4, 3f9869c <gs_misc_record-0x893e4>
 4031960:	c2d7ffa7 	.inst	0xc2d7ffa7 ; undefined
 4031964:	b5d0cf31 	cbnz	x17, 3fd3348 <gs_misc_record-0x4e738>
 4031968:	2cd99e8b 	ldp	s11, s7, [x20], #204
 403196c:	5bdeae1d 	.inst	0x5bdeae1d ; undefined
 4031970:	9b64c2b0 	.inst	0x9b64c2b0 ; undefined
 4031974:	ec63f226 	.inst	0xec63f226 ; undefined
 4031978:	756aa39c 	.inst	0x756aa39c ; undefined
 403197c:	026d930a 	.inst	0x026d930a ; undefined
 4031980:	9c0906a9 	ldr	q9, 4043a54 <_end+0xf654>
 4031984:	eb0e363f 	cmp	x17, x14, lsl #13
 4031988:	72076785 	ands	w5, w28, #0xfe07ffff
 403198c:	05005713 	orr	z19.b, z19.b, #0x44
 4031990:	95bf4a82 	bl	b004398 <_end+0x6fcff98>
 4031994:	e2b87a14 	.inst	0xe2b87a14 ; undefined
 4031998:	7bb12bae 	.inst	0x7bb12bae ; undefined
 403199c:	0cb61b38 	.inst	0x0cb61b38 ; undefined
 40319a0:	92d28e9b 	mov	x27, #0xffff6b8bffffffff    	// #-163225937117185
 40319a4:	e5d5be0d 	st1d	{z13.d}, p7, [z16.d, #168]
 40319a8:	7cdcefb7 	.inst	0x7cdcefb7 ; undefined
 40319ac:	0bdbdf21 	.inst	0x0bdbdf21 ; undefined
 40319b0:	86d3d2d4 	.inst	0x86d3d2d4 ; undefined
 40319b4:	f1d4e242 	.inst	0xf1d4e242 ; undefined
 40319b8:	68ddb3f8 	ldpsw	x24, x12, [sp], #236
 40319bc:	1fda836e 	fmsub	h14, h27, h26, h0
 40319c0:	81be16cd 	.inst	0x81be16cd ; undefined
 40319c4:	f6b9265b 	.inst	0xf6b9265b ; undefined
 40319c8:	6fb077e1 	.inst	0x6fb077e1 ; undefined
 40319cc:	18b74777 	ldr	w23, 3fa02b8 <gs_misc_record-0x817c8>
 40319d0:	88085ae6 	stxr	w8, w6, [x23]
 40319d4:	ff0f6a70 	.inst	0xff0f6a70 ; undefined
 40319d8:	66063bca 	.inst	0x66063bca ; undefined
 40319dc:	11010b5c 	add	w28, w26, #0x42
 40319e0:	8f659eff 	.inst	0x8f659eff ; undefined
 40319e4:	f862ae69 	ldraa	x9, [x19, #-3760]!
 40319e8:	616bffd3 	.inst	0x616bffd3 ; undefined
 40319ec:	166ccf45 	b	fffffffffdb65700 <_end+0xfffffffff9b31300>
 40319f0:	a00ae278 	.inst	0xa00ae278 ; undefined
 40319f4:	d70dd2ee 	.inst	0xd70dd2ee ; undefined
 40319f8:	4e048354 	.inst	0x4e048354 ; undefined
 40319fc:	3903b3c2 	strb	w2, [x30, #236]
 4031a00:	a7672661 	.inst	0xa7672661 ; undefined
 4031a04:	d06016f7 	adrp	x23, c430f000 <_end+0xc02dac00>
 4031a08:	4969474d 	.inst	0x4969474d ; undefined
 4031a0c:	3e6e77db 	.inst	0x3e6e77db ; undefined
 4031a10:	aed16a4a 	.inst	0xaed16a4a ; undefined
 4031a14:	d9d65adc 	.inst	0xd9d65adc ; undefined
 4031a18:	40df0b66 	.inst	0x40df0b66 ; undefined
 4031a1c:	37d83bf0 	tbnz	w16, #27, 4032198 <ddr4_1p5g_table+0x34>
 4031a20:	a9bcae53 	stp	x19, x11, [x18, #-56]!
 4031a24:	debb9ec5 	.inst	0xdebb9ec5 ; undefined
 4031a28:	47b2cf7f 	.inst	0x47b2cf7f ; undefined
 4031a2c:	30b5ffe9 	adr	x9, 3f9da29 <gs_misc_record-0x84057>
 4031a30:	bdbdf21c 	.inst	0xbdbdf21c ; undefined
 4031a34:	cabac28a 	eon	x10, x20, x26, asr #48
 4031a38:	53b39330 	.inst	0x53b39330 ; undefined
 4031a3c:	24b4a3a6 	cmplo	p6.s, p0/z, z29.s, #82
 4031a40:	bad03605 	.inst	0xbad03605 ; undefined
 4031a44:	cdd70693 	.inst	0xcdd70693 ; undefined
 4031a48:	54de5729 	b.ls	3fee52c <gs_misc_record-0x33554>  // b.plast
 4031a4c:	23d967bf 	.inst	0x23d967bf ; undefined
 4031a50:	b3667a2e 	bfi	x14, x17, #26, #31
 4031a54:	c4614ab8 	.inst	0xc4614ab8 ; undefined
 4031a58:	5d681b02 	.inst	0x5d681b02 ; undefined
 4031a5c:	2a6f2b94 	orn	w20, w28, w15, lsr #10
 4031a60:	b40bbe37 	cbz	x23, 4049224 <_end+0x14e24>
 4031a64:	c30c8ea1 	.inst	0xc30c8ea1 ; undefined
 4031a68:	5a05df1b 	.inst	0x5a05df1b ; undefined
 4031a6c:	2d02ef8d 	stp	s13, s27, [x28, #20]

0000000004031a70 <malloc_sizes_ro>:
 4031a70:	00020050 	.inst	0x00020050 ; undefined
 4031a74:	00050100 	.inst	0x00050100 ; undefined
 4031a78:	00040200 	.inst	0x00040200 ; undefined
 4031a7c:	00010300 	.inst	0x00010300 ; undefined
 4031a80:	00010800 	.inst	0x00010800 ; undefined
 4031a84:	00010b00 	.inst	0x00010b00 ; undefined
 4031a88:	00010c00 	.inst	0x00010c00 ; undefined
 4031a8c:	00014000 	.inst	0x00014000 ; undefined
 4031a90:	00000000 	udf	#0

Disassembly of section .rodata.str1.1:

0000000004031a94 <.rodata.str1.1>:
 4031a94:	6f620a0d 	mla	v13.8h, v16.8h, v2.h[6]
 4031a98:	7420746f 	.inst	0x7420746f ; undefined
 4031a9c:	656c6261 	fnmls	z1.h, p0/m, z19.h, z12.h
 4031aa0:	616f6220 	.inst	0x616f6220 ; undefined
 4031aa4:	73206472 	.inst	0x73206472 ; undefined
 4031aa8:	63746977 	.inst	0x63746977 ; undefined
 4031aac:	78303a68 	.inst	0x78303a68 ; undefined
 4031ab0:	670a0d00 	.inst	0x670a0d00 ; undefined
 4031ab4:	6d207465 	stp	d5, d29, [x3, #-512]
 4031ab8:	20637369 	.inst	0x20637369 ; undefined
 4031abc:	61746164 	.inst	0x61746164 ; undefined
 4031ac0:	69616620 	ldpsw	x0, x25, [x17, #-248]
 4031ac4:	0d64656c 	.inst	0x0d64656c ; undefined
 4031ac8:	0a0d000a 	and	w10, w0, w13
 4031acc:	61647075 	.inst	0x61647075 ; undefined
 4031ad0:	6d206574 	stp	d20, d25, [x11, #-512]
 4031ad4:	20637369 	.inst	0x20637369 ; undefined
 4031ad8:	62206f74 	.inst	0x62206f74 ; undefined
 4031adc:	6b636f6c 	.inst	0x6b636f6c ; undefined
 4031ae0:	000a0d30 	.inst	0x000a0d30 ; undefined
 4031ae4:	70750a0d 	adr	x13, 411bc27 <_end+0xe7827>
 4031ae8:	65746164 	fnmls	z4.h, p0/m, z11.h, z20.h
 4031aec:	73696d20 	.inst	0x73696d20 ; undefined
 4031af0:	6f742063 	umlal2	v3.4s, v3.8h, v4.h[3]
 4031af4:	6f6c6220 	umlsl2	v0.4s, v17.8h, v12.h[2]
 4031af8:	0d316b63 	.inst	0x0d316b63 ; undefined
 4031afc:	6c0a000a 	stnp	d10, d0, [x0, #160]
 4031b00:	20736470 	.inst	0x20736470 ; undefined
 4031b04:	20706d63 	.inst	0x20706d63 ; undefined
 4031b08:	6c696166 	ldnp	d6, d24, [x11, #-368]
 4031b0c:	6c0a000a 	stnp	d10, d0, [x0, #160]
 4031b10:	20736470 	.inst	0x20736470 ; undefined
 4031b14:	5f656572 	.inst	0x5f656572 ; undefined
 4031b18:	5f79656b 	.inst	0x5f79656b ; undefined
 4031b1c:	61657261 	.inst	0x61657261 ; undefined
 4031b20:	004b4f20 	.inst	0x004b4f20 ; undefined
 4031b24:	72650a0d 	.inst	0x72650a0d ; undefined
 4031b28:	6d3a3072 	stp	d18, d12, [x3, #-96]
 4031b2c:	6e207861 	sqneg	v1.16b, v3.16b
 4031b30:	30206d75 	adr	x21, 40728dd <_end+0x3e4dd>
 4031b34:	0a0d0078 	and	w24, w3, w13
 4031b38:	30727265 	adr	x5, 4116985 <_end+0xe2585>
 4031b3c:	766e693a 	.inst	0x766e693a ; undefined
 4031b40:	64696c61 	.inst	0x64696c61 ; undefined
 4031b44:	72617020 	.inst	0x72617020 ; undefined
 4031b48:	74206d61 	.inst	0x74206d61 ; undefined
 4031b4c:	656c6261 	fnmls	z1.h, p0/m, z19.h, z12.h
 4031b50:	616d6920 	.inst	0x616d6920 ; undefined
 4031b54:	6f206567 	sqshlu	v7.4s, v11.4s, #0
 4031b58:	65736666 	fnmls	z6.h, p1/m, z19.h, z19.h
 4031b5c:	78302074 	ldeorh	w16, w20, [x3]
 4031b60:	650a0d00 	.inst	0x650a0d00 ; undefined
 4031b64:	3a317272 	.inst	0x3a317272 ; undefined
 4031b68:	2078616d 	.inst	0x2078616d ; undefined
 4031b6c:	206d756e 	.inst	0x206d756e ; undefined
 4031b70:	0a007830 	and	w16, w1, w0, lsl #30
 4031b74:	32727265 	.inst	0x32727265 ; undefined
 4031b78:	78616d3a 	.inst	0x78616d3a ; undefined
 4031b7c:	6e656c20 	umin	v0.8h, v1.8h, v5.8h
 4031b80:	30206874 	adr	x20, 407288d <_end+0x3e48d>
 4031b84:	650a0078 	.inst	0x650a0078 ; undefined
 4031b88:	3a347272 	.inst	0x3a347272 ; undefined
 4031b8c:	746f6e20 	.inst	0x746f6e20 ; undefined
 4031b90:	32313520 	orr	w0, w9, #0x1fff8000
 4031b94:	65747962 	fnmls	z2.h, p6/m, z11.h, z20.h
 4031b98:	696c6120 	ldpsw	x0, x24, [x9, #-160]
 4031b9c:	00216e67 	.inst	0x00216e67 ; NYI
 4031ba0:	64706c0a 	.inst	0x64706c0a ; undefined
 4031ba4:	61702073 	.inst	0x61702073 ; undefined
 4031ba8:	5f6d6172 	.inst	0x5f6d6172 ; undefined
 4031bac:	6f666e69 	.inst	0x6f666e69 ; undefined
 4031bb0:	004b4f20 	.inst	0x004b4f20 ; undefined
 4031bb4:	64706c0a 	.inst	0x64706c0a ; undefined
 4031bb8:	62752073 	.inst	0x62752073 ; undefined
 4031bbc:	5f746f6f 	.inst	0x5f746f6f ; undefined
 4031bc0:	6f666e69 	.inst	0x6f666e69 ; undefined
 4031bc4:	004b4f20 	.inst	0x004b4f20 ; undefined
 4031bc8:	34333231 	cbz	w17, 409820c <_end+0x63e0c>
 4031bcc:	38373635 	.inst	0x38373635 ; undefined
 4031bd0:	34333231 	cbz	w17, 4098214 <_end+0x63e14>
 4031bd4:	38373635 	.inst	0x38373635 ; undefined
 4031bd8:	73696d00 	.inst	0x73696d00 ; undefined
 4031bdc:	0a0d0063 	and	w3, w3, w13
 4031be0:	72724520 	.inst	0x72724520 ; undefined
 4031be4:	203a726f 	.inst	0x203a726f ; undefined
 4031be8:	6373696d 	.inst	0x6373696d ; undefined
 4031bec:	65726120 	fnmls	z0.h, p0/m, z9.h, z18.h
 4031bf0:	6f6e2061 	umlal2	v1.4s, v3.8h, v14.h[2]
 4031bf4:	6f662074 	umlal2	v20.4s, v3.8h, v6.h[2]
 4031bf8:	2e646e75 	umin	v21.4h, v19.4h, v4.4h
 4031bfc:	6f000a0d 	.inst	0x6f000a0d ; undefined
 4031c00:	006c6674 	.inst	0x006c6674 ; undefined
 4031c04:	6f6f6275 	umlsl2	v21.4s, v19.8h, v15.h[2]
 4031c08:	62750074 	.inst	0x62750074 ; undefined
 4031c0c:	5f746f6f 	.inst	0x5f746f6f ; undefined
 4031c10:	62750061 	.inst	0x62750061 ; undefined
 4031c14:	5f746f6f 	.inst	0x5f746f6f ; undefined
 4031c18:	544f0062 	b.cs	40cfc24 <_end+0x9b824>  // b.hs, b.nlast
 4031c1c:	203a4c46 	.inst	0x203a4c46 ; undefined
 4031c20:	72677075 	.inst	0x72677075 ; undefined
 4031c24:	20656461 	.inst	0x20656461 ; undefined
 4031c28:	67616c66 	.inst	0x67616c66 ; undefined
 4031c2c:	74656420 	.inst	0x74656420 ; undefined
 4031c30:	65746365 	fnmls	z5.h, p0/m, z27.h, z20.h
 4031c34:	6c202c64 	stnp	d4, d11, [x3, #-512]
 4031c38:	2064616f 	.inst	0x2064616f ; undefined
 4031c3c:	6f422d55 	.inst	0x6f422d55 ; undefined
 4031c40:	6620746f 	.inst	0x6620746f ; undefined
 4031c44:	206d6f72 	.inst	0x206d6f72 ; undefined
 4031c48:	6c66746f 	ldnp	d15, d29, [x3, #-416]
 4031c4c:	0d000a0d 	st1	{v13.b}[2], [x16]
 4031c50:	6f62750a 	uqshl	v10.2d, v8.2d, #34
 4031c54:	7020746f 	adr	x15, 4072ae3 <_end+0x3e6e3>
 4031c58:	20747261 	.inst	0x20747261 ; undefined
 4031c5c:	656d616e 	fnmls	z14.h, p0/m, z11.h, z13.h
 4031c60:	6e65003a 	uaddl2	v26.4s, v1.8h, v5.8h
 4031c64:	656c6261 	fnmls	z1.h, p0/m, z19.h, z12.h
 4031c68:	64706c5f 	.inst	0x64706c5f ; undefined
 4031c6c:	6f6c5f73 	.inst	0x6f6c5f73 ; undefined
 4031c70:	65206b63 	.inst	0x65206b63 ; undefined
 4031c74:	726f7272 	.inst	0x726f7272 ; undefined
 4031c78:	0a000a0d 	and	w13, w16, w0, lsl #2
 4031c7c:	7364706c 	.inst	0x7364706c ; undefined
 4031c80:	656c6320 	fnmls	z0.h, p0/m, z25.h, z12.h
 4031c84:	6f207261 	.inst	0x6f207261 ; undefined
 4031c88:	0a000a6b 	and	w11, w19, w0, lsl #2
 4031c8c:	423a470d 	.inst	0x423a470d ; undefined
 4031c90:	20746f6f 	.inst	0x20746f6f ; undefined
 4031c94:	6c696166 	ldnp	d6, d24, [x11, #-368]
 4031c98:	0a006465 	and	w5, w3, w0, lsl #25
 4031c9c:	733a470d 	.inst	0x733a470d ; undefined
 4031ca0:	2074666f 	.inst	0x2074666f ; undefined
 4031ca4:	00747372 	.inst	0x00747372 ; undefined
 4031ca8:	61680a0d 	.inst	0x61680a0d ; undefined
 4031cac:	656c646e 	fnmls	z14.h, p1/m, z3.h, z12.h
 4031cb0:	73696d20 	.inst	0x73696d20 ; undefined
 4031cb4:	72612063 	.inst	0x72612063 ; undefined
 4031cb8:	66206165 	.inst	0x66206165 ; undefined
 4031cbc:	006c6961 	.inst	0x006c6961 ; undefined
 4031cc0:	6e6e6f63 	umin	v3.8h, v27.8h, v14.8h
 4031cc4:	20746365 	.inst	0x20746365 ; undefined
 4031cc8:	656d6974 	fnmls	z20.h, p2/m, z11.h, z13.h
 4031ccc:	0a74756f 	bic	w15, w11, w20, lsr #29
 4031cd0:	6e657300 	uabdl2	v0.4s, v24.8h, v5.8h
 4031cd4:	6f625f64 	.inst	0x6f625f64 ; undefined
 4031cd8:	5f647261 	sqdmlsl	s1, h19, v4.h[2]
 4031cdc:	65707974 	fnmls	z20.h, p6/m, z11.h, z16.h
 4031ce0:	5f6f745f 	sqshl	d31, d2, #47
 4031ce4:	20627375 	.inst	0x20627375 ; undefined
 4031ce8:	000a6b6f 	.inst	0x000a6b6f ; undefined
 4031cec:	75636573 	.inst	0x75636573 ; undefined
 4031cf0:	73206572 	.inst	0x73206572 ; undefined
 4031cf4:	206d6172 	.inst	0x206d6172 ; undefined
 4031cf8:	20746573 	.inst	0x20746573 ; undefined
 4031cfc:	6f727265 	fcmla	v5.8h, v19.8h, v18.h[1], #270
 4031d00:	63000a72 	.inst	0x63000a72 ; undefined
 4031d04:	3065726f 	adr	x15, 40fcb51 <_end+0xc8751>
 4031d08:	746f6720 	.inst	0x746f6720 ; undefined
 4031d0c:	67736d20 	.inst	0x67736d20 ; undefined
 4031d10:	72726520 	.inst	0x72726520 ; undefined
 4031d14:	000a726f 	.inst	0x000a726f ; undefined
 4031d18:	79706f63 	ldrh	w3, [x27, #6198]
 4031d1c:	66746120 	.inst	0x66746120 ; undefined
 4031d20:	646f6320 	.inst	0x646f6320 ; undefined
 4031d24:	72652065 	.inst	0x72652065 ; undefined
 4031d28:	0a726f72 	bic	w18, w27, w18, lsr #27
 4031d2c:	65685400 	fnmla	z0.h, p5/m, z0.h, z8.h
 4031d30:	62617420 	.inst	0x62617420 ; undefined
 4031d34:	6920656c 	stgp	x12, x25, [x11, #-1024]
 4031d38:	6e692073 	usubl2	v19.4s, v3.8h, v9.8h
 4031d3c:	72726f63 	.inst	0x72726f63 ; undefined
 4031d40:	2e746365 	rsubhn	v5.4h, v27.4s, v20.4s
 4031d44:	0d0a000a 	.inst	0x0d0a000a ; undefined
 4031d48:	6f727245 	fcmla	v5.8h, v18.8h, v18.h[1], #270
 4031d4c:	57203a72 	.inst	0x57203a72 ; undefined
 4031d50:	20746961 	.inst	0x20746961 ; undefined
 4031d54:	20646d63 	.inst	0x20646d63 ; undefined
 4031d58:	20757063 	.inst	0x20757063 ; undefined
 4031d5c:	696e6966 	ldpsw	x6, x26, [x11, #-144]
 4031d60:	74206873 	.inst	0x74206873 ; undefined
 4031d64:	6f656d69 	.inst	0x6f656d69 ; undefined
 4031d68:	0a217475 	bic	w21, w3, w1, lsl #29
 4031d6c:	74656700 	.inst	0x74656700 ; undefined
 4031d70:	61656620 	.inst	0x61656620 ; undefined
 4031d74:	65727574 	fnmls	z20.h, p5/m, z11.h, z18.h
 4031d78:	69616620 	ldpsw	x0, x25, [x17, #-248]
 4031d7c:	2164656c 	.inst	0x2164656c ; undefined
 4031d80:	7300200a 	.inst	0x7300200a ; undefined
 4031d84:	675f6970 	.inst	0x675f6970 ; undefined
 4031d88:	72656e65 	.inst	0x72656e65 ; undefined
 4031d8c:	775f6c61 	.inst	0x775f6c61 ; undefined
 4031d90:	5f746961 	.inst	0x5f746961 ; undefined
 4031d94:	64616572 	.inst	0x64616572 ; undefined
 4031d98:	61662079 	.inst	0x61662079 ; undefined
 4031d9c:	64656c69 	.inst	0x64656c69 ; undefined
 4031da0:	00200a21 	.inst	0x00200a21 ; NYI
 4031da4:	72450d0a 	.inst	0x72450d0a ; undefined
 4031da8:	3a726f72 	.inst	0x3a726f72 ; undefined
 4031dac:	69615720 	ldpsw	x0, x21, [x25, #-248]
 4031db0:	6d642074 	ldp	d20, d8, [x3, #-448]
 4031db4:	6e692061 	usubl2	v1.4s, v3.8h, v9.8h
 4031db8:	69662074 	ldpsw	x20, x8, [x3, #-208]
 4031dbc:	6873696e 	.inst	0x6873696e ; undefined
 4031dc0:	6d697420 	ldp	d0, d29, [x1, #-368]
 4031dc4:	74756f65 	.inst	0x74756f65 ; undefined
 4031dc8:	45000a21 	.inst	0x45000a21 ; undefined
 4031dcc:	726f7272 	.inst	0x726f7272 ; undefined
 4031dd0:	6547203a 	fminv	h26, p0, z1.h
 4031dd4:	65662074 	fmls	z20.h, p0/m, z3.h, z6.h
 4031dd8:	72757461 	.inst	0x72757461 ; undefined
 4031ddc:	65722065 	fmls	z5.h, p0/m, z3.h, z18.h
 4031de0:	61662067 	.inst	0x61662067 ; undefined
 4031de4:	64656c69 	.inst	0x64656c69 ; undefined
 4031de8:	5053000a 	adr	x10, 40d7dea <_end+0xa39ea>
 4031dec:	614e2049 	.inst	0x614e2049 ; undefined
 4031df0:	6520646e 	.inst	0x6520646e ; undefined
 4031df4:	6c62616e 	ldnp	d14, d24, [x11, #-480]
 4031df8:	75512065 	.inst	0x75512065 ; undefined
 4031dfc:	73206461 	.inst	0x73206461 ; undefined
 4031e00:	65636375 	fnmls	z21.h, p0/m, z27.h, z3.h
 4031e04:	0a216465 	bic	w5, w3, w1, lsl #25
 4031e08:	49505300 	.inst	0x49505300 ; undefined
 4031e0c:	6e614e20 	uqshl	v0.8h, v17.8h, v1.8h
 4031e10:	6e652064 	usubl2	v4.4s, v3.8h, v5.8h
 4031e14:	656c6261 	fnmls	z1.h, p0/m, z19.h, z12.h
 4031e18:	61755120 	.inst	0x61755120 ; undefined
 4031e1c:	61662064 	.inst	0x61662064 ; undefined
 4031e20:	64656c69 	.inst	0x64656c69 ; undefined
 4031e24:	73000a21 	.inst	0x73000a21 ; undefined
 4031e28:	70207465 	adr	x5, 4072cb7 <_end+0x3e8b7>
 4031e2c:	65746f72 	fnmls	z18.h, p3/m, z27.h, z20.h
 4031e30:	72207463 	ands	w3, w3, #0x3fffffff
 4031e34:	66206765 	.inst	0x66206765 ; undefined
 4031e38:	656c6961 	fnmls	z1.h, p2/m, z11.h, z12.h
 4031e3c:	000a2164 	.inst	0x000a2164 ; undefined
 4031e40:	20746567 	.inst	0x20746567 ; undefined
 4031e44:	746f7270 	.inst	0x746f7270 ; undefined
 4031e48:	20746365 	.inst	0x20746365 ; undefined
 4031e4c:	20676572 	.inst	0x20676572 ; undefined
 4031e50:	6c696166 	ldnp	d6, d24, [x11, #-368]
 4031e54:	0a216465 	bic	w5, w3, w1, lsl #25
 4031e58:	72724500 	.inst	0x72724500 ; undefined
 4031e5c:	203a726f 	.inst	0x203a726f ; undefined
 4031e60:	74697257 	.inst	0x74697257 ; undefined
 4031e64:	72702065 	.inst	0x72702065 ; undefined
 4031e68:	6365746f 	.inst	0x6365746f ; undefined
 4031e6c:	6e6f6974 	.inst	0x6e6f6974 ; undefined
 4031e70:	73696420 	.inst	0x73696420 ; undefined
 4031e74:	656c6261 	fnmls	z1.h, p0/m, z19.h, z12.h
 4031e78:	69616620 	ldpsw	x0, x25, [x17, #-248]
 4031e7c:	2164656c 	.inst	0x2164656c ; undefined
 4031e80:	6d65000a 	ldp	d10, d0, [x0, #-432]
 4031e84:	775f636d 	.inst	0x775f636d ; undefined
 4031e88:	65746972 	fnmls	z18.h, p2/m, z11.h, z20.h
 4031e8c:	746e6920 	.inst	0x746e6920 ; undefined
 4031e90:	6d697420 	ldp	d0, d29, [x1, #-368]
 4031e94:	756f2065 	.inst	0x756f2065 ; undefined
 4031e98:	65000a74 	.inst	0x65000a74 ; undefined
 4031e9c:	5f636d6d 	.inst	0x5f636d6d ; undefined
 4031ea0:	74697277 	.inst	0x74697277 ; undefined
 4031ea4:	6e692065 	usubl2	v5.4s, v3.8h, v9.8h
 4031ea8:	72652074 	.inst	0x72652074 ; undefined
 4031eac:	0a726f72 	bic	w18, w27, w18, lsr #27
 4031eb0:	44440a00 	sqdmlalbt	z0.h, z16.b, z4.b
 4031eb4:	4c502052 	.inst	0x4c502052 ; undefined
 4031eb8:	4f4c204c 	smlal2	v12.4s, v2.8h, v12.h[0]
 4031ebc:	46204b43 	.inst	0x46204b43 ; undefined
 4031ec0:	2e6c6961 	.inst	0x2e6c6961 ; undefined
 4031ec4:	Address 0x0000000004031ec4 is out of bounds.


Disassembly of section .eh_frame:

0000000004031ec8 <.eh_frame>:
 4031ec8:	00000010 	udf	#16
 4031ecc:	00000000 	udf	#0
 4031ed0:	00527a03 	.inst	0x00527a03 ; undefined
 4031ed4:	011e7801 	.inst	0x011e7801 ; undefined
 4031ed8:	001f0c1b 	.inst	0x001f0c1b ; undefined
 4031edc:	0000002c 	udf	#44
 4031ee0:	00000018 	udf	#24
 4031ee4:	ffff41d8 	.inst	0xffff41d8 ; undefined
 4031ee8:	0000004c 	udf	#76
 4031eec:	00040400 	.inst	0x00040400 ; undefined
 4031ef0:	300e0000 	adr	x0, 404def1 <_end+0x19af1>
 4031ef4:	059e069d 	mov	z29.s, p14/z, #52
 4031ef8:	00000c04 	udf	#3076
 4031efc:	04049300 	asrd	z0.h, p4/m, z0.h, #8
 4031f00:	00000038 	udf	#56
 4031f04:	0ed3ddde 	.inst	0x0ed3ddde ; undefined
 4031f08:	00000000 	udf	#0
 4031f0c:	00000024 	udf	#36
 4031f10:	00000048 	udf	#72
 4031f14:	ffff42e8 	.inst	0xffff42e8 ; undefined
 4031f18:	0000003c 	udf	#60
 4031f1c:	00040400 	.inst	0x00040400 ; undefined
 4031f20:	100e0000 	adr	x0, 404df20 <_end+0x19b20>
 4031f24:	019e029d 	.inst	0x019e029d ; undefined
 4031f28:	00003404 	udf	#13316
 4031f2c:	0eddde00 	.inst	0x0eddde00 ; undefined
 4031f30:	00000000 	udf	#0

Disassembly of section .data:

0000000004031f38 <gs_active_slot>:
 4031f38:	000000ff 	udf	#255

0000000004031f3c <g_svb_plus_battery>:
 4031f3c:	0002ab98 	.inst	0x0002ab98 ; undefined
 4031f40:	087cda00 	.inst	0x087cda00 ; undefined
 4031f44:	0000034e 	udf	#846
 4031f48:	000003d4 	udf	#980
 4031f4c:	00000000 	udf	#0
 4031f50:	0002c251 	.inst	0x0002c251 ; undefined
 4031f54:	08719bb5 	.inst	0x08719bb5 ; undefined
 4031f58:	00000346 	udf	#838
 4031f5c:	0000040d 	udf	#1037
 4031f60:	00000000 	udf	#0
 4031f64:	00031045 	.inst	0x00031045 ; undefined
 4031f68:	0938626f 	.inst	0x0938626f ; undefined
 4031f6c:	00000374 	udf	#884
 4031f70:	00000405 	udf	#1029
 4031f74:	00000000 	udf	#0

0000000004031f78 <g_svb_pt_battery>:
 4031f78:	0002560c 	.inst	0x0002560c ; undefined
 4031f7c:	07f189f0 	.inst	0x07f189f0 ; undefined
 4031f80:	00000343 	udf	#835
 4031f84:	000003b6 	udf	#950
 4031f88:	00000000 	udf	#0
 4031f8c:	00026e44 	.inst	0x00026e44 ; undefined
 4031f90:	07d53059 	.inst	0x07d53059 ; undefined
 4031f94:	0000032a 	udf	#810
 4031f98:	00000403 	udf	#1027
 4031f9c:	00000000 	udf	#0
 4031fa0:	0002fa26 	.inst	0x0002fa26 ; undefined
 4031fa4:	088012c0 	stllrb	w0, [x22]
 4031fa8:	0000032a 	udf	#810
 4031fac:	00000419 	udf	#1049
 4031fb0:	0000032a 	udf	#810

0000000004031fb4 <g_svb_lite_battery>:
 4031fb4:	0002560c 	.inst	0x0002560c ; undefined
 4031fb8:	07f189f0 	.inst	0x07f189f0 ; undefined
 4031fbc:	00000343 	udf	#835
 4031fc0:	000003b6 	udf	#950
 4031fc4:	00000000 	udf	#0
 4031fc8:	00026e44 	.inst	0x00026e44 ; undefined
 4031fcc:	07dcd179 	.inst	0x07dcd179 ; undefined
 4031fd0:	00000329 	udf	#809
 4031fd4:	00000408 	udf	#1032
 4031fd8:	00000000 	udf	#0
 4031fdc:	0002fa26 	.inst	0x0002fa26 ; undefined
 4031fe0:	08bd1bc0 	.inst	0x08bd1bc0 ; undefined
 4031fe4:	00000336 	udf	#822
 4031fe8:	00000403 	udf	#1027
 4031fec:	00000000 	udf	#0

0000000004031ff0 <g_svb_plus>:
 4031ff0:	0002ab98 	.inst	0x0002ab98 ; undefined
 4031ff4:	089b5e80 	stllrb	w0, [x20]
 4031ff8:	00000362 	udf	#866
 4031ffc:	000003e8 	udf	#1000
 4032000:	00000000 	udf	#0
 4032004:	0002c251 	.inst	0x0002c251 ; undefined
 4032008:	08902035 	stllrb	w21, [x1]
 403200c:	0000035a 	udf	#858
 4032010:	00000419 	udf	#1049
 4032014:	00000000 	udf	#0
 4032018:	00031045 	.inst	0x00031045 ; undefined
 403201c:	0956e6ef 	.inst	0x0956e6ef ; undefined
 4032020:	00000388 	udf	#904
 4032024:	00000419 	udf	#1049
 4032028:	00000000 	udf	#0

000000000403202c <g_svb_pt>:
 403202c:	0002560c 	.inst	0x0002560c ; undefined
 4032030:	08100e70 	stxrb	w16, w16, [x19]
 4032034:	0000034d 	udf	#845
 4032038:	000003ca 	udf	#970
 403203c:	00000000 	udf	#0
 4032040:	00026e44 	.inst	0x00026e44 ; undefined
 4032044:	07f3b4d9 	.inst	0x07f3b4d9 ; undefined
 4032048:	0000032a 	udf	#810
 403204c:	00000419 	udf	#1049
 4032050:	00000000 	udf	#0
 4032054:	0002fa26 	.inst	0x0002fa26 ; undefined
 4032058:	08add980 	.inst	0x08add980 ; undefined
 403205c:	0000032a 	udf	#810
 4032060:	00000419 	udf	#1049
 4032064:	0000033e 	udf	#830

0000000004032068 <g_svb_lite>:
 4032068:	0002560c 	.inst	0x0002560c ; undefined
 403206c:	08100e70 	stxrb	w16, w16, [x19]
 4032070:	00000357 	udf	#855
 4032074:	000003ca 	udf	#970
 4032078:	00000000 	udf	#0
 403207c:	00026e44 	.inst	0x00026e44 ; undefined
 4032080:	07fb55f9 	.inst	0x07fb55f9 ; undefined
 4032084:	0000033d 	udf	#829
 4032088:	00000419 	udf	#1049
 403208c:	00000000 	udf	#0
 4032090:	0002fa26 	.inst	0x0002fa26 ; undefined
 4032094:	08dba040 	.inst	0x08dba040 ; undefined
 4032098:	0000034a 	udf	#842
 403209c:	00000417 	udf	#1047
 40320a0:	00000000 	udf	#0

00000000040320a4 <chsel_remap_reg_val_phy1_lpddr4>:
 40320a4:	11140100 	add	w0, w8, #0x500
 40320a8:	00001700 	udf	#5888
 40320ac:	11140104 	add	w4, w8, #0x500
 40320b0:	70050052 	adr	x18, 403c0bb <_end+0x7cbb>
 40320b4:	11140804 	add	w4, w0, #0x502
 40320b8:	01ff0040 	.inst	0x01ff0040 ; undefined
 40320bc:	11140808 	add	w8, w0, #0x502
 40320c0:	00000000 	udf	#0
 40320c4:	11140110 	add	w16, w8, #0x500
 40320c8:	00000000 	udf	#0

00000000040320cc <chsel_remap_reg_val_phy0_lpddr4>:
 40320cc:	11140100 	add	w0, w8, #0x500
 40320d0:	00001700 	udf	#5888
 40320d4:	11140104 	add	w4, w8, #0x500
 40320d8:	70050050 	adr	x16, 403c0e3 <_end+0x7ce3>
 40320dc:	11140804 	add	w4, w0, #0x502
 40320e0:	01ff0040 	.inst	0x01ff0040 ; undefined
 40320e4:	11140808 	add	w8, w0, #0x502
 40320e8:	00000000 	udf	#0
 40320ec:	11140110 	add	w16, w8, #0x500
 40320f0:	00000000 	udf	#0

00000000040320f4 <chsel_remap_reg_val_phy0_nonlpddr4>:
 40320f4:	11140100 	add	w0, w8, #0x500
 40320f8:	00001700 	udf	#5888
 40320fc:	11140104 	add	w4, w8, #0x500
 4032100:	70050088 	adr	x8, 403c113 <_end+0x7d13>
 4032104:	11140804 	add	w4, w0, #0x502
 4032108:	01ff0040 	.inst	0x01ff0040 ; undefined
 403210c:	11140808 	add	w8, w0, #0x502
 4032110:	00000000 	udf	#0
 4032114:	11140110 	add	w16, w8, #0x500
 4032118:	00000000 	udf	#0

000000000403211c <chsel_remap_reg_val_phy1_nonlpddr4>:
 403211c:	11140100 	add	w0, w8, #0x500
 4032120:	00001700 	udf	#5888
 4032124:	11140104 	add	w4, w8, #0x500
 4032128:	70050089 	adr	x9, 403c13b <_end+0x7d3b>
 403212c:	11140804 	add	w4, w0, #0x502
 4032130:	01ff0040 	.inst	0x01ff0040 ; undefined
 4032134:	11140808 	add	w8, w0, #0x502
 4032138:	00000000 	udf	#0
 403213c:	11140110 	add	w16, w8, #0x500
 4032140:	00000000 	udf	#0

0000000004032144 <ddr4_2g_table>:
 4032144:	11140100 	add	w0, w8, #0x500
 4032148:	00001700 	udf	#5888
 403214c:	11140104 	add	w4, w8, #0x500
 4032150:	40050088 	.inst	0x40050088 ; undefined
 4032154:	11140804 	add	w4, w0, #0x502
 4032158:	00ff0040 	.inst	0x00ff0040 ; undefined
 403215c:	11140808 	add	w8, w0, #0x502
 4032160:	00000000 	udf	#0

0000000004032164 <ddr4_1p5g_table>:
 4032164:	11140100 	add	w0, w8, #0x500
 4032168:	00001700 	udf	#5888
 403216c:	11140104 	add	w4, w8, #0x500
 4032170:	30050088 	adr	x8, 403c181 <_end+0x7d81>
 4032174:	11140804 	add	w4, w0, #0x502
 4032178:	003f0040 	.inst	0x003f0040 ; NYI
 403217c:	11140808 	add	w8, w0, #0x502
 4032180:	00000000 	udf	#0
 4032184:	11140110 	add	w16, w8, #0x500
 4032188:	00001700 	udf	#5888
 403218c:	11140114 	add	w20, w8, #0x500
 4032190:	30040088 	adr	x8, 403a1a1 <_end+0x5da1>
 4032194:	11140814 	add	w20, w0, #0x502
 4032198:	001f0080 	.inst	0x001f0080 ; undefined
 403219c:	11140818 	add	w24, w0, #0x502
 40321a0:	00000040 	udf	#64
 40321a4:	11148600 	add	w0, w16, #0x521
 40321a8:	0004480d 	.inst	0x0004480d ; undefined

00000000040321ac <ddr4_3g_table>:
 40321ac:	11140100 	add	w0, w8, #0x500
 40321b0:	00001700 	udf	#5888
 40321b4:	11140104 	add	w4, w8, #0x500
 40321b8:	30050088 	adr	x8, 403c1c9 <_end+0x7dc9>
 40321bc:	11140804 	add	w4, w0, #0x502
 40321c0:	007f0040 	.inst	0x007f0040 ; undefined
 40321c4:	11140808 	add	w8, w0, #0x502
 40321c8:	00000000 	udf	#0
 40321cc:	11140110 	add	w16, w8, #0x500
 40321d0:	00001700 	udf	#5888
 40321d4:	11140114 	add	w20, w8, #0x500
 40321d8:	30040088 	adr	x8, 403a1e9 <_end+0x5de9>
 40321dc:	11140814 	add	w20, w0, #0x502
 40321e0:	003f00c0 	.inst	0x003f00c0 ; NYI
 40321e4:	11140818 	add	w24, w0, #0x502
 40321e8:	00000080 	udf	#128
 40321ec:	11148600 	add	w0, w16, #0x521
 40321f0:	0004480f 	.inst	0x0004480f ; undefined

00000000040321f4 <ddr4_4g_table>:
 40321f4:	11140100 	add	w0, w8, #0x500
 40321f8:	00001700 	udf	#5888
 40321fc:	11140104 	add	w4, w8, #0x500
 4032200:	30050088 	adr	x8, 403c211 <_end+0x7e11>
 4032204:	11140804 	add	w4, w0, #0x502
 4032208:	00ff0040 	.inst	0x00ff0040 ; undefined
 403220c:	11140808 	add	w8, w0, #0x502
 4032210:	00000000 	udf	#0

0000000004032214 <lpddr4_4g_1rank_table>:
 4032214:	11140100 	add	w0, w8, #0x500
 4032218:	00001700 	udf	#5888
 403221c:	11140104 	add	w4, w8, #0x500
 4032220:	70050050 	adr	x16, 403c22b <_end+0x7e2b>
 4032224:	11140804 	add	w4, w0, #0x502
 4032228:	00ff0040 	.inst	0x00ff0040 ; undefined
 403222c:	11140808 	add	w8, w0, #0x502
 4032230:	00000000 	udf	#0
 4032234:	11148060 	add	w0, w3, #0x520
 4032238:	00000162 	udf	#354
 403223c:	11149060 	add	w0, w3, #0x524
 4032240:	00000162 	udf	#354
 4032244:	11144634 	add	w20, w17, #0x511
 4032248:	00000162 	udf	#354
 403224c:	11144644 	add	w4, w18, #0x511
 4032250:	00000162 	udf	#354

0000000004032254 <lpddr4_2g_2rank_table>:
 4032254:	11140100 	add	w0, w8, #0x500
 4032258:	00001700 	udf	#5888
 403225c:	11140104 	add	w4, w8, #0x500
 4032260:	70050050 	adr	x16, 403c26b <_end+0x7e6b>
 4032264:	11140804 	add	w4, w0, #0x502
 4032268:	00ff0040 	.inst	0x00ff0040 ; undefined
 403226c:	11140808 	add	w8, w0, #0x502
 4032270:	00000000 	udf	#0
 4032274:	11148060 	add	w0, w3, #0x520
 4032278:	00000142 	udf	#322
 403227c:	11149060 	add	w0, w3, #0x524
 4032280:	00000142 	udf	#322
 4032284:	11148064 	add	w4, w3, #0x520
 4032288:	00000142 	udf	#322
 403228c:	11149064 	add	w4, w3, #0x524
 4032290:	00000142 	udf	#322
 4032294:	11144634 	add	w20, w17, #0x511
 4032298:	00000142 	udf	#322
 403229c:	11144644 	add	w4, w18, #0x511
 40322a0:	00000142 	udf	#322
 40322a4:	11144638 	add	w24, w17, #0x511
 40322a8:	00000142 	udf	#322
 40322ac:	11144648 	add	w8, w18, #0x511
 40322b0:	00000142 	udf	#322

00000000040322b4 <lpddr4_4g_2rank_table>:
 40322b4:	11140100 	add	w0, w8, #0x500
 40322b8:	00001700 	udf	#5888
 40322bc:	11140104 	add	w4, w8, #0x500
 40322c0:	70050050 	adr	x16, 403c2cb <_end+0x7ecb>
 40322c4:	11140804 	add	w4, w0, #0x502
 40322c8:	00ff0040 	.inst	0x00ff0040 ; undefined
 40322cc:	11140808 	add	w8, w0, #0x502
 40322d0:	00000000 	udf	#0
 40322d4:	11148060 	add	w0, w3, #0x520
 40322d8:	00000152 	udf	#338
 40322dc:	11149060 	add	w0, w3, #0x524
 40322e0:	00000152 	udf	#338
 40322e4:	11148064 	add	w4, w3, #0x520
 40322e8:	00000152 	udf	#338
 40322ec:	11149064 	add	w4, w3, #0x524
 40322f0:	00000152 	udf	#338
 40322f4:	11144634 	add	w20, w17, #0x511
 40322f8:	00000152 	udf	#338
 40322fc:	11144644 	add	w4, w18, #0x511
 4032300:	00000152 	udf	#338
 4032304:	11144638 	add	w24, w17, #0x511
 4032308:	00000152 	udf	#338
 403230c:	11144648 	add	w8, w18, #0x511
 4032310:	00000152 	udf	#338
	...

Disassembly of section .partition:

0000000004032340 <__partition_start>:
	...

Disassembly of section .bss:

0000000004021a80 <gs_misc_record>:
	...

0000000004021aa0 <hash_verify_buf>:
	...

0000000004021ac0 <nand_block_size>:
 4021ac0:	00000000 	udf	#0

0000000004021ac4 <uboot_part_offset>:
 4021ac4:	00000000 	udf	#0

0000000004021ac8 <init.1>:
 4021ac8:	00000000 	udf	#0

0000000004021acc <table.0>:
	...

0000000004021ecc <uboot_part_length>:
 4021ecc:	00000000 	udf	#0

0000000004021ed0 <gs_flash_init>:
 4021ed0:	00000000 	udf	#0

0000000004021ed4 <gs_last_rblock>:
 4021ed4:	00000000 	udf	#0

0000000004021ed8 <gs_bad_count>:
 4021ed8:	00000000 	udf	#0

0000000004021edc <ree_flash_offset>:
 4021edc:	00000000 	udf	#0

0000000004021ee0 <__stack_chk_guard>:
	...

0000000004021ee8 <g_backup_params>:
	...

0000000004021ef8 <jump_addr>:
	...

0000000004021f00 <fw_addr>:
	...

0000000004021f10 <malloc_sizes>:
	...

0000000004022030 <gs_phy_erase_shift>:
 4022030:	00000000 	udf	#0

0000000004022034 <gs_page_shift>:
 4022034:	00000000 	udf	#0

0000000004022038 <gs_page_size>:
 4022038:	00000000 	udf	#0

000000000402203c <gs_is_quad_boot>:
 402203c:	00000000 	udf	#0

0000000004022040 <gs_dma_buf>:
	...

0000000004022048 <gs_block_size>:
 4022048:	00000000 	udf	#0

Disassembly of section .comment:

0000000000000000 <.comment>:
   0:	3a434347 	ccmn	w26, w3, #0x7, mi  // mi = first
   4:	43472820 	.inst	0x43472820 ; undefined
   8:	30312d43 	adr	x3, 625b1 <gs_misc_record-0x3fbf4cf>
   c:	302e332e 	adr	x14, 5c671 <gs_misc_record-0x3fc540f>
  10:	696c6720 	ldpsw	x0, x25, [x25, #-160]
  14:	322d6362 	orr	w2, w27, #0xfff80fff
  18:	2034332e 	.inst	0x2034332e ; undefined
  1c:	31726576 	adds	w22, w11, #0xc99, lsl #12
  20:	312e3030 	adds	w16, w1, #0xb8c
  24:	32203034 	orr	w20, w1, #0x1fff
  28:	2d333230 	stp	s16, s12, [x17, #-104]
  2c:	322d3131 	orr	w17, w9, #0xfff80000
  30:	32312033 	orr	w19, w1, #0xff8000
  34:	3a30303a 	.inst	0x3a30303a ; undefined
  38:	20293030 	.inst	0x20293030 ; undefined
  3c:	332e3031 	.inst	0x332e3031 ; undefined
  40:	Address 0x0000000000000040 is out of bounds.


Disassembly of section .debug_frame:

0000000000000000 <.debug_frame>:
   0:	0000000c 	udf	#12
   4:	ffffffff 	.inst	0xffffffff ; undefined
   8:	78040001 	sturh	w1, [x0, #64]
   c:	001f0c1e 	.inst	0x001f0c1e ; undefined
  10:	00000014 	udf	#20
  14:	00000000 	udf	#0
  18:	04031000 	subr	z0.b, p4/m, z0.b, z0.b
	...
  28:	00000014 	udf	#20
  2c:	00000000 	udf	#0
  30:	04031080 	subr	z0.b, p4/m, z0.b, z4.b
	...
  40:	00000014 	udf	#20
  44:	00000000 	udf	#0
  48:	04031100 	subr	z0.b, p4/m, z0.b, z8.b
	...
  58:	00000014 	udf	#20
  5c:	00000000 	udf	#0
  60:	04031180 	subr	z0.b, p4/m, z0.b, z12.b
	...
  70:	00000014 	udf	#20
  74:	00000000 	udf	#0
  78:	04031200 	subr	z0.b, p4/m, z0.b, z16.b
	...
  88:	00000014 	udf	#20
  8c:	00000000 	udf	#0
  90:	04031280 	subr	z0.b, p4/m, z0.b, z20.b
	...
  a0:	00000014 	udf	#20
  a4:	00000000 	udf	#0
  a8:	04031300 	subr	z0.b, p4/m, z0.b, z24.b
	...
  b8:	00000014 	udf	#20
  bc:	00000000 	udf	#0
  c0:	04031380 	subr	z0.b, p4/m, z0.b, z28.b
	...
  d0:	00000014 	udf	#20
  d4:	00000000 	udf	#0
  d8:	04031400 	subr	z0.b, p5/m, z0.b, z0.b
  dc:	00000000 	udf	#0
  e0:	00000004 	udf	#4
  e4:	00000000 	udf	#0
