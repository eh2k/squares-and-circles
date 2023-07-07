	.cpu cortex-m7
	.arch armv7e-m
	.fpu fpv5-d16
	.eabi_attribute 28, 1
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"FV1.S.cpp"
	.text
	.section	.qspiflash_data,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	OEM1_4_spn
	.syntax unified
	.thumb
	.thumb_func
	.type	OEM1_4_spn, %function
OEM1_4_spn:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	add	ip, r0, #131072
	ldrb	r3, [ip, #328]	@ zero_extendqisi2
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	sub	sp, sp, #12
	cmp	r3, #0
	beq	.L30
	movs	r3, #0
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r3
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r4, #1
	mov	r2, r3
	ldr	r7, .L31
	mov	r6, r3
	str	r1, [r0, #32]
	str	r3, [ip, #304]
.L3:
	ldr	r3, [r0]
	mov	lr, #2097152
	ldr	r5, [r0, #92]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, r8, r5, lr
	mov      r8, r8, asl #9
	orr      r1, r8, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, r3
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r5, [r0, #96]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, r8, r5, lr
	mov      r8, r8, asl #9
	orr      r1, r8, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, r3
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r3, [ip, #268]
	movs	r5, #0
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, ip, r1, r5
	mov      ip, ip, asl #9
	orr      lr, ip, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	ubfx	ip, r3, #0, #15
	add	ip, r0, ip, lsl #2
	str	r1, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r2, r7
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r1, ip
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r3
	str	r2, [r0, #4]
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0, #8]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, r7, r4, r6
	mov      r7, r7, asl #9
	orr      r1, r7, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, r2
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r4, #4101
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	str	r1, [r4, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r4, r1, r5
	mov      r4, r4, asl #9
	orr      r2, r4, r2, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, .L31+4
	ldr	r7, [r0, #80]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, ip, r7, r4
	mov      ip, ip, asl #9
	orr      r2, ip, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	str	r2, [r0, #28]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r6, r2, r5
	mov      r6, r6, asl #9
	orr      r4, r6, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #8388608
	ldr	r5, [r0, #76]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r7, r5, r6
	mov      r7, r7, asl #9
	orr      r2, r7, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	and	r4, r2, #15728640
	lsls	r2, r2, #8
	it	mi
	orrmi	r4, r4, #-16777216
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #8388608
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r5, r4, r6
	mov      r5, r5, asl #9
	orr      r2, r5, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	sub	r2, r2, #7864320
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L5
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r4, r2
	bgt	.L6
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L7
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L8
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r4, r2
	bgt	.L9
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r4, r2
	bgt	.L10
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L11
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L12
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r4, r2
	bgt	.L13
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r4, #0
	bgt	.L14
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r5, r4, r6
	mov      r5, r5, asl #9
	orr      r2, r5, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r4, r2
	bgt	.L15
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L16
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L17
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r4, #0
	mov	r2, r4
	bgt	.L18
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r5, r4, r6
	mov      r5, r5, asl #9
	orr      r2, r5, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r2, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r4, #0
	mov	r2, r4
	bgt	.L19
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r5, r4, r6
	mov      r5, r5, asl #9
	orr      r2, r5, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r2, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r4, #0
	mov	r2, r4
	bgt	.L20
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r5, r4, r6
	mov      r5, r5, asl #9
	orr      r2, r5, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r2, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r4, #0
	mov	r2, r4
	bgt	.L21
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r5, r4, r6
	mov      r5, r5, asl #9
	orr      r2, r5, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r2, #0
	mov	r5, r2
	bgt	.L22
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r2, r6
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r4, #0
	mov	r2, r4
	str	r4, [r0]
	bgt	.L23
	b	.L32
.L33:
	.align	2
.L31:
	.word	4194303
	.word	-4194304
.L32:
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r2, r4, r6
	mov      r2, r2, asl #9
	orr      r5, r2, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #4]
	add	r5, r5, #524288
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r5, #0
	str	r5, [r0]
	bgt	.L24
.L5:
	movw	r2, #18435
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r6, .L34
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r4, r7, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
.L6:
	movw	r2, #18124
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
.L7:
	movw	r2, #17609
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r6, .L34
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r4, r7, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
.L8:
	movw	r2, #17202
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r4, r7, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
.L9:
	movw	r2, #16861
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
.L10:
	movw	r2, #16221
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r6, .L34
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
.L11:
	movw	r2, #15866
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r4, r7, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
.L12:
	movw	r2, #15413
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r4, r7, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
.L13:
	movw	r2, #14851
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r5, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r7, r5, r6
	mov      r7, r7, asl #9
	orr      r2, r7, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r2
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
.L14:
	movw	r2, #14313
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
.L15:
	movw	r2, #13806
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r6
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
.L16:
	movw	r4, #13468
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r7, r4, r6
	mov      r7, r7, asl #9
	orr      r2, r7, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
.L17:
	movw	r4, #13006
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r6, .L34
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r7, r4, r6
	mov      r7, r7, asl #9
	orr      r2, r7, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
.L18:
	movw	r4, #12676
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r5, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r4, r5
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
.L19:
	movw	r4, #12316
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r5, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r4, r5
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
.L20:
	movw	r4, #11513
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r5, .L34
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r4, r5
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
.L21:
	movw	r4, #11245
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r4, r6
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
.L22:
	movw	r4, #10614
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r7, r4, r6
	mov      r7, r7, asl #9
	orr      r2, r7, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
.L23:
	movw	r4, #9923
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r6, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r4, r6
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r2
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L35
.L36:
	.align	2
.L34:
	.word	3350528
.L35:
.L24:
	movw	r2, #9421
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r4, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #9239
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #8677
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #8008
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r7, .L37
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, ip, r2, r7
	mov      ip, ip, asl #9
	orr      r6, ip, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #7521
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #7075
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #6631
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #6211
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #5441
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r3, #5024
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r2, #4601
	add	r2, r2, r3
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r2, .L37+4
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r1, r2
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	mov	ip, #8388608
	str	r6, [r0, #144]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, r5, r6, ip
	mov      r5, r5, asl #9
	orr      r1, r5, r1, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r5, #19336
	add	r5, r5, r3
	ubfx	r5, r5, #0, #15
	add	r5, r0, r5, lsl #2
	ldr	r5, [r5, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r1, r8, r5, r2
	mov      r8, r8, asl #9
	orr      r1, r8, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r5, #19102
	add	r5, r5, r3
	ubfx	r5, r5, #0, #15
	add	r5, r0, r5, lsl #2
	str	r1, [r5, #268]
	ldr	r5, .L37+8
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, r8, r1, r5
	mov      r8, r8, asl #9
	orr      r7, r8, r7, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r1, #19783
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r1
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r9, r1, r2
	mov      r9, r9, asl #9
	orr      r7, r9, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	movw	lr, #19337
	add	lr, lr, r3
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	r7, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, lr, r7, r5
	mov      lr, lr, asl #9
	orr      r1, lr, r1, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r7, #20336
	add	r7, r7, r3
	ubfx	r7, r7, #0, #15
	add	r7, r0, r7, lsl #2
	ldr	r7, [r7, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r7
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r1, r9, r7, r2
	mov      r9, r9, asl #9
	orr      r1, r9, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	movw	lr, #19784
	add	lr, lr, r3
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	r1, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, lr, r1, r5
	mov      lr, lr, asl #9
	orr      r7, lr, r7, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #0
	str	r7, [r0, #100]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, r7, r1
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r8, r6, ip
	mov      r8, r8, asl #9
	orr      r7, r8, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r7, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r7, #20538
	add	r7, r7, r3
	ubfx	r7, r7, #0, #15
	add	r7, r0, r7, lsl #2
	ldr	r7, [r7, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r7
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r9, r7, r2
	mov      r9, r9, asl #9
	orr      r6, r9, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	lr, #20337
	add	lr, lr, r3
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	r6, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, lr, r6, r5
	mov      lr, lr, asl #9
	orr      r7, lr, r7, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r6, #20928
	add	r6, r6, r3
	ubfx	r6, r6, #0, #15
	add	r6, r0, r6, lsl #2
	ldr	r6, [r6, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r9, r6, r2
	mov      r9, r9, asl #9
	orr      r7, r9, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	movw	lr, #20539
	add	lr, lr, r3
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	r7, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, lr, r7, r5
	mov      lr, lr, asl #9
	orr      r6, lr, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r7, #21556
	add	r7, r7, r3
	ubfx	r7, r7, #0, #15
	add	r7, r0, r7, lsl #2
	ldr	lr, [r7, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r9, lr, r2
	mov      r9, r9, asl #9
	orr      r6, r9, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r2, lr, r6, r5
	mov      lr, lr, asl #9
	orr      r2, lr, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r5, #20929
	add	r5, r5, r3
	ubfx	r5, r5, #0, #15
	add	r5, r0, r5, lsl #2
	str	r6, [r5, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	str	r2, [r0, #104]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r5, r2, r1
	mov      r5, r5, asl #9
	orr      r6, r5, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r5, #6601
	add	r5, r5, r3
	ubfx	r5, r5, #0, #15
	add	r5, r0, r5, lsl #2
	ldr	r8, [r5, #268]
	b	.L38
.L39:
	.align	2
.L37:
	.word	3350528
	.word	5029888
	.word	-5038080
.L38:
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r2, .L40
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, lr, r8, r2
	mov      lr, lr, asl #9
	orr      r7, lr, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r7, [r0, #148]
	sub	r9, r6, r7
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r10, r9, r4
	mov      r10, r10, asl #9
	orr      lr, r10, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r4, lr, r2
	mov      r4, r4, asl #9
	orr      r7, r4, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [r0, #84]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r9, r7, r4
	mov      r9, r9, asl #9
	orr      r6, r9, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r6
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r9, r8, ip
	mov      r9, r9, asl #9
	orr      r6, r9, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	str	r6, [r5, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, r5, r6, r1
	mov      r5, r5, asl #9
	orr      r7, r5, r7, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r5, #8601
	add	r5, r5, r3
	ubfx	r5, r5, #0, #15
	add	r5, r0, r5, lsl #2
	ldr	r6, [r5, #268]
	str	r5, [sp, #4]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r9, r6, r2
	mov      r9, r9, asl #9
	orr      r7, r9, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r5, [r0, #152]
	ldr	r9, .L40+8
	sub	r8, r7, r5
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r10, fp, r8, r9
	mov      fp, fp, asl #9
	orr      r10, fp, r10, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r10, r10, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r10
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, r10, r2
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r10, [r0, #152]
	add	r7, r7, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, r7, r4
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r9, r6, ip
	mov      r9, r9, asl #9
	orr      r7, r9, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r7, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r5, [sp, #4]
	str	r6, [r5, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, r5, r6, r1
	mov      r5, r5, asl #9
	orr      r7, r5, r7, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r5, #11101
	add	r5, r5, r3
	ubfx	r5, r5, #0, #15
	add	r5, r0, r5, lsl #2
	ldr	r8, [r5, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r9, r8, r2
	mov      r9, r9, asl #9
	orr      r7, r9, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r10, .L40+12
	sub	r7, r9, lr
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, fp, r7, r10
	mov      fp, fp, asl #9
	orr      r6, fp, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r6, lr, r2
	mov      r6, r6, asl #9
	orr      r7, r6, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r9, r7, r4
	mov      r9, r9, asl #9
	orr      r6, r9, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r6
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r9, r8, ip
	mov      r9, r9, asl #9
	orr      r6, r9, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	str	r6, [r5, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r6, r1
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r5, #14101
	add	r5, r5, r3
	ubfx	r5, r5, #0, #15
	add	r5, r0, r5, lsl #2
	ldr	r6, [r5, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r3, r8, r6, r2
	mov      r8, r8, asl #9
	orr      r3, r8, r3, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r3, r3, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r9, .L40+16
	sub	r7, r8, lr
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r3, r10, r7, r9
	mov      r10, r10, asl #9
	orr      r3, r10, r3, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r3
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, lr, r3, r2
	mov      lr, lr, asl #9
	orr      r7, lr, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r0, #148]
	add	r2, r7, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r3, r7, r2, r4
	mov      r7, r7, asl #9
	orr      r3, r7, r3, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r3
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r3, r7, r6, ip
	mov      r7, r7, asl #9
	orr      r3, r7, r3, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #8]
	add	r3, r3, r2
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r0, #4]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r4, r3, r1
	mov      r4, r4, asl #9
	orr      r2, r4, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r5, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	str	r2, [r0]
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L30:
	ldr	r6, [ip, #304]
	ldr	r7, .L40+4
	asrs	r2, r6, #10
	subs	r7, r7, r6
	adds	r4, r2, #1
	b	.L3
.L41:
	.align	2
.L40:
	.word	-8388608
	.word	4194303
	.word	3355392
	.word	2516480
	.word	1677568
	.size	OEM1_4_spn, .-OEM1_4_spn
	.section	.text._Z15execute_programP3FV1,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	_Z15execute_programP3FV1
	.syntax unified
	.thumb
	.thumb_func
	.type	_Z15execute_programP3FV1, %function
_Z15execute_programP3FV1:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	cmp	r0, #0
	beq	.L208
	add	r2, r0, #131072
	vldr.32	s10, .L214
	vldr.32	s11, .L214+4
	ldr	r3, [r2, #336]
	vldr.32	s12, .L214+8
	ldr	r1, [r3]
	orr	r1, r1, #1
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	ldr	r6, .L214+12
	ldr	fp, .L214+32
	bx	r1
.L46:
	ldr	r4, [r2, #268]
	ldr	r1, [r3, #4]
	ldr	r5, [r3, #8]
	add	r1, r1, r4
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r4, [r1, #268]
	ldr	r1, [r0]
	str	r1, [r0, #4]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r4
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r8, r4, r5
	mov      r8, r8, asl #9
	orr      ip, r8, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	lr, [r0, #8]
.L189:
	add	r1, r1, ip
.L191:
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
.L188:
	str	r1, [r0]
.L190:
	ldr	r1, [r3, #16]
	adds	r3, r3, #16
	orr	r1, r1, #1
	bx	r1
.L44:
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L130:
	ldr	r1, [r0]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [r3, #4]
	ldr	r5, [r3, #12]
	str	r1, [r4]
	ldr	r1, [r3, #8]
	ldr	r4, [r1]
.L187:
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r4, r5
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0]
	adds	r3, r3, #16
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L129:
	ldr	r4, [r3, #4]
	adds	r3, r3, #16
	ldr	r1, [r0]
	str	r1, [r0, #4]
	str	r1, [r4]
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L128:
	ldr	r4, [r3, #4]
	ldr	r1, [r0]
	str	r1, [r0, #4]
	ldr	r4, [r4]
.L186:
	add	r1, r1, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0]
	adds	r3, r3, #16
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L131:
	ldr	r1, [r3, #4]
	adds	r3, r3, #16
	ldr	r4, [r0]
	str	r4, [r0, #4]
	ldr	r1, [r1]
	str	r1, [r0]
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L125:
	ldr	r1, [r0]
	cmp	r1, #0
	str	r1, [r0, #4]
	blt	.L209
	ldr	r1, .L214+16
	b	.L188
.L124:
	ldr	r4, [r0]
	ldr	r5, [r3, #4]
	b	.L187
.L123:
	ldr	r1, [r3, #8]
	ldr	r4, [r0]
	b	.L186
.L122:
	ldr	r1, [r3, #8]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0]
	adds	r3, r3, #16
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L109:
	ldrd	r1, ip, [r3, #4]
	ldr	r4, [r3, #12]
	cmp	r1, #1
	bls	.L210
	subs	r1, r1, #2
	cmp	r1, #1
	bls	.L211
	mov	lr, #0
.L114:
	ldr	r1, [r0]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, ip, r1, lr
	mov      ip, ip, asl #9
	orr      r5, ip, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0, #4]
	add	r4, r4, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r1, [r3, #16]
	adds	r3, r3, #16
	str	r4, [r0]
	orr	r1, r1, #1
	bx	r1
.L121:
	b	.L190
.L100:
	ldr	r1, [r3, #4]
	ldr	r4, [r0]
	str	r4, [r0, #4]
	cmp	r1, #9
	bhi	.L138
	tbh	[pc, r1, lsl #1]
.L103:
	.2byte	(.L108-.L103)/2
	.2byte	(.L107-.L103)/2
	.2byte	(.L106-.L103)/2
	.2byte	(.L105-.L103)/2
	.2byte	(.L104-.L103)/2
	.2byte	(.L102-.L103)/2
	.2byte	(.L138-.L103)/2
	.2byte	(.L138-.L103)/2
	.2byte	(.L104-.L103)/2
	.2byte	(.L102-.L103)/2
	.p2align 1
.L97:
	ldr	r1, [r3, #12]
	ldrd	r5, r4, [r3, #4]
	lsls	r1, r1, #21
	lsls	r4, r4, #8
	cmp	r5, #0
	bne	.L98
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #28]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0, #32]
	str	r5, [r2, #304]
	b	.L190
.L82:
	ldrd	r1, r4, [r3, #4]
	ldr	ip, [r3, #12]
	cmp	r1, #1
	bls	.L212
	subs	r1, r1, #2
	cmp	r1, #1
	bls	.L89
	mov	lr, #0
	and	r9, r4, #32
	mov	r1, lr
.L88:
	cmp	r9, #0
	bne	.L96
.L133:
	add	ip, ip, r1, asr #10
.L96:
	ldr	r1, [r2, #268]
	add	r1, r1, ip
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r4, [r1, #268]
	ldr	r1, [r0]
	str	r1, [r0, #4]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, r4
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r8, r4, lr
	mov      r8, r8, asl #9
	orr      r5, r8, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	ip, [r0, #8]
	add	r1, r1, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0]
	adds	r3, r3, #16
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L81:
	ldr	r1, [r3, #4]
	movs	r5, #0
	ldr	r4, .L214+20
	adds	r3, r3, #16
	add	r1, r1, r1, lsl #1
	add	r1, r0, r1, lsl #2
	str	r5, [r1, r4]
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L78:
	ldrd	r5, r4, [r3, #4]
	ldr	r1, [r3, #12]
	lsls	r4, r4, #14
	cmp	r5, #0
	bne	.L79
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	lsls	r1, r1, #8
	str	r4, [r0, #12]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0, #16]
	ldr	r1, .L214+24
	str	r5, [r2, #272]
	str	r1, [r2, #276]
	b	.L190
.L71:
	ldrd	r4, r5, [r3, #4]
	ands	r1, r4, #16
	beq	.L72
	ldrb	r1, [r2, #328]	@ zero_extendqisi2
	eor	r1, r1, #1
.L72:
	lsls	r7, r4, #29
	bpl	.L73
	ldr	r7, [r0]
	cmp	r7, #0
	it	eq
	orreq	r1, r1, #1
.L73:
	lsls	r7, r4, #30
	bpl	.L74
	ldr	r7, [r0]
	cmp	r7, #0
	it	gt
	orrgt	r1, r1, #1
.L74:
	lsls	r7, r4, #31
	bpl	.L75
	ldr	r7, [r0]
	orr	r1, r1, r7, lsr #31
.L75:
	lsls	r4, r4, #28
	bpl	.L76
	ldrd	r4, r7, [r0]
	eors	r4, r4, r7
	ubfx	r4, r4, #23, #1
	orrs	r1, r1, r4
.L76:
	cmp	r1, #0
	beq	.L190
	add	r3, r3, r5, lsl #4
	b	.L190
.L67:
	ldr	r4, [r0]
	ldr	r1, [r3, #4]
	str	r4, [r0, #4]
	orrs	r1, r1, r4
	ldr	r4, .L214+28
	ands	r4, r4, r1
	cmp	r4, #0
	beq	.L191
	orr	r1, r1, #-16777216
	b	.L191
.L69:
	ldr	r4, [r0]
	ldr	r1, [r3, #4]
	str	r4, [r0, #4]
	eors	r1, r1, r4
	ldr	r4, .L214+28
	ands	r4, r4, r1
	cmp	r4, #0
	beq	.L191
	orr	r1, r1, #-16777216
	b	.L191
.L215:
	.align	2
.L214:
	.word	872415232
	.word	-1096111445
	.word	1059760811
	.word	exp2tab
	.word	8388607
	.word	131376
	.word	-8388607
	.word	-8388608
	.word	4194303
.L65:
	ldr	r4, [r0]
	ldr	r1, [r3, #4]
	str	r4, [r0, #4]
	ands	r1, r1, r4
	ldr	r4, .L216
	ands	r4, r4, r1
	cmp	r4, #0
	beq	.L191
	orr	r1, r1, #-16777216
	b	.L191
.L64:
	ldr	r4, [r0]
	ldr	r5, [r3, #4]
	ldr	r1, [r3, #8]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r4, r5
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #4]
	b	.L189
.L61:
	ldr	r4, [r0]
	cmp	r4, #0
	ldrd	ip, r5, [r3, #4]
	str	r4, [r0, #4]
	blt	.L213
	ldr	r1, .L216+4
.L62:
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, lr, r1, ip
	mov      lr, lr, asl #9
	orr      r4, lr, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	adds	r1, r5, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0]
	adds	r3, r3, #16
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L58:
	ldr	r1, [r0]
	cmp	r1, #0
	ldrd	r5, r4, [r3, #4]
	str	r1, [r0, #4]
	ble	.L59
.L192:
	vmov	s15, r1	@ int
	vmov.f32	s13, #2.0e+0
	vcvt.f32.s32	s15, s15
	vmul.f32	s15, s15, s10
	vmov	r1, s15
	bic	ip, r1, #2139095040
	ubfx	r1, r1, #23, #8
	add	r7, ip, #1065353216
	subs	r1, r1, #128
	vmov	s14, r7	@ int
	vmov	s15, r1	@ int
	vfma.f32	s13, s14, s11
	vcvt.f32.s32	s15, s15
	vsub.f32	s15, s15, s12
	vfma.f32	s15, s14, s13
	vmov.f32	s14, s15
	vcvt.s32.f32	s14, s14, #19
	vmov	lr, s14	@ int
.L60:
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, lr, r5
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L186
.L57:
	ldr	r1, [r3, #4]
	ldr	r4, [r0]
	str	r4, [r0, #4]
	ldr	r5, [r1]
	b	.L187
.L56:
	ldr	r4, [r3, #4]
	ldr	r1, [r0]
	ldr	ip, [r3, #8]
	str	r1, [r0, #4]
	ldr	r5, [r4]
	cmp	r5, #0
	it	lt
	rsblt	r5, r5, #0
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, lr, r5, ip
	mov      lr, lr, asl #9
	orr      r4, lr, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r1, #0
	it	lt
	rsblt	r1, r1, #0
	cmp	r4, #0
	it	lt
	rsblt	r4, r4, #0
	cmp	r1, r4
	it	lt
	movlt	r1, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0]
	adds	r3, r3, #16
	ldr	r1, [r3]
	orr	r1, r1, #1
	bx	r1
.L55:
	ldrd	r4, r1, [r0]
	ldr	ip, [r3, #4]
	ldr	r5, [r3, #8]
	str	r4, [r0, #4]
	str	r4, [ip]
	ldr	r4, [r0]
	subs	r4, r1, r4
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r4, r5
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L189
.L54:
	ldr	ip, [r3, #4]
	ldr	r4, [r0]
	ldr	r5, [r3, #8]
	ldr	r1, [r0, #4]
	str	r4, [r0, #4]
	str	r4, [ip]
	ldr	r4, [r0]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r4, r5
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L189
.L53:
	ldr	r4, [r3, #4]
	ldr	r1, [r0]
	ldr	r5, [r3, #8]
	str	r1, [r0, #4]
	str	r1, [r4]
	ldr	r4, [r0]
	b	.L187
.L52:
	ldr	r1, [r3, #4]
	ldr	r4, [r0]
	ldr	r5, [r3, #8]
	str	r4, [r0, #4]
	ldr	r1, [r1]
	subs	r4, r4, r1
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r4, r5
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L189
.L51:
	ldr	r4, [r3, #4]
	ldr	r1, [r0]
	ldr	r5, [r3, #8]
	str	r1, [r0, #4]
	ldr	r4, [r4]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r4, r5
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L189
.L50:
	ldr	r4, [r2, #268]
	ldr	r1, [r3, #4]
	ldr	r5, [r3, #8]
	add	r1, r1, r4
	ldr	r4, [r0]
	ubfx	r1, r1, #0, #15
	str	r4, [r0, #4]
	add	r1, r0, r1, lsl #2
	str	r4, [r1, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r4, r5
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [r0, #8]
	b	.L186
.L47:
	ldr	r1, [r2, #268]
	ldr	r4, [r0, #108]
	ldr	ip, [r3, #4]
	add	r1, r1, r4, asr #8
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r5, [r1, #268]
	ldr	r1, [r0]
	str	r1, [r0, #4]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #8]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, lr, r5, ip
	mov      lr, lr, asl #9
	orr      r4, lr, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L186
.L49:
	ldr	r4, [r2, #268]
	ldr	r1, [r3, #4]
	ldr	r5, [r3, #8]
	add	r1, r1, r4
	ldr	r4, [r0]
	ubfx	r1, r1, #0, #15
	str	r4, [r0, #4]
	add	r1, r0, r1, lsl #2
	str	r4, [r1, #268]
	b	.L187
.L79:
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	lsls	r1, r1, #8
	str	r4, [r0, #20]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	ip, .L216+8
	str	r1, [r0, #24]
	movs	r1, #0
	strd	r1, ip, [r2, #288]
	b	.L190
.L98:
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #36]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0, #40]
	movs	r1, #0
	str	r1, [r2, #316]
	b	.L190
.L212:
	movw	r5, #8209
	lsls	r7, r4, #31
	and	r9, r4, #32
	add	r5, r5, r1
	add	r1, r0, r1, lsl #4
	lsl	lr, r5, #4
	add	r5, r0, r5, lsl #4
	it	pl
	ldrpl	r5, [r0, lr]
	ldr	lr, .L216+12
	it	mi
	ldrmi	r5, [r5, #4]
	lsls	r7, r4, #29
	ldr	r1, [r1, lr]
	ldr	lr, [r1]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, r8, r5, lr
	mov      r8, r8, asl #9
	orr      r1, r8, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	ite	mi
	submi	lr, fp, r1
	movpl	lr, r1
	lsls	r5, r4, #28
	it	mi
	rsbmi	r1, r1, #0
	b	.L88
.L59:
	beq	.L134
	rsbs	r1, r1, #0
	b	.L192
.L213:
	sbfx	lr, r4, #8, #19
	movw	r1, #2046
	lsls	r4, r4, #5
	and	lr, lr, r1
	ubfx	r1, r4, #0, #14
	asrs	r4, r4, #24
	ldr	r9, [r6, lr, lsl #2]
	add	r8, lr, #1
	rsb	r4, r4, #6
	smull	lr, r9, r1, r9
	ldr	r1, [r6, r8, lsl #2]
	lsr	lr, lr, #14
	orr	lr, lr, r9, lsl #18
	add	r1, r1, lr
	asrs	r1, r1, r4
	it	mi
	addmi	r1, r1, #1
	asrs	r1, r1, #1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L62
.L217:
	.align	2
.L216:
	.word	-8388608
	.word	8388607
	.word	-8388607
	.word	131352
.L210:
	movw	r5, #8209
	tst	ip, #1
	add	r5, r5, r1
	add	r1, r0, r1, lsl #4
	lsl	lr, r5, #4
	add	r5, r0, r5, lsl #4
	it	eq
	ldreq	r5, [r0, lr]
	ldr	lr, .L218+16
	it	ne
	ldrne	r5, [r5, #4]
	ldr	r1, [r1, lr]
	ldr	r1, [r1]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, r5, r1
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
.L119:
	tst	ip, #4
	beq	.L114
	sub	lr, fp, lr
	b	.L114
.L209:
	sbfx	ip, r1, #8, #19
	movw	r4, #2046
	lsls	r1, r1, #5
	and	ip, ip, r4
	ubfx	r5, r1, #0, #14
	asrs	r1, r1, #24
	rsb	r4, r1, #6
	ldr	r1, [r6, ip, lsl #2]
	add	ip, ip, #1
	smull	r5, lr, r5, r1
	ldr	r1, [r6, ip, lsl #2]
	lsrs	r5, r5, #14
	orr	r5, r5, lr, lsl #18
	add	r1, r1, r5
	asrs	r1, r1, r4
	it	mi
	addmi	r1, r1, #1
	asrs	r1, r1, #1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L188
.L208:
	ldr	r3, .L218
	ldr	r2, .L218+4
	str	r2, [r3]
	bx	lr
.L102:
	ldr	r1, [r2, #296]
	ldr	r4, [r2, #292]
	ldr	r5, [r1]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r4, r5
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L191
.L104:
	ldr	r1, [r2, #280]
	ldr	r4, [r2, #276]
	ldr	r5, [r1]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r4, r5
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L191
.L134:
	ldr	lr, .L218+20
	b	.L60
.L105:
	ldr	r1, [r2, #316]
	b	.L191
.L107:
	ldr	r1, [r2, #296]
	ldr	r4, [r2, #288]
	ldr	r5, [r1]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r4, r5
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L191
.L106:
	ldr	r1, [r2, #304]
	b	.L191
.L108:
	ldr	r1, [r2, #280]
	ldr	r4, [r2, #272]
	ldr	r5, [r1]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r4, r5
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L191
.L138:
	movs	r1, #0
	b	.L191
.L211:
	add	lr, r1, r1, lsl #1
	ldr	r5, .L218+8
	tst	ip, #16
	lsl	r10, r1, #1
	add	lr, r0, lr, lsl #2
	ldr	r5, [lr, r5]
	ldr	r5, [r5]
	asr	r5, r5, #21
	asr	r8, fp, r5
	beq	.L115
	cmp	r8, #0
	ldr	r7, .L218+12
	mov	r9, r8
	ldr	lr, [lr, r7]
	it	lt
	addlt	r9, r8, #1
	add	lr, lr, r9, asr #1
	and	lr, lr, r8
.L117:
	tst	ip, #8
	it	ne
	subne	lr, r8, lr
	tst	ip, #32
	beq	.L119
	add	r1, r1, r10
	ldr	lr, .L218+12
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, lr]
	cmp	r1, r8, asr #1
	it	gt
	subgt	r1, r8, r1
	lsl	lr, r1, r5
	b	.L119
.L89:
	add	lr, r1, r1, lsl #1
	ldr	r5, .L218+8
	lsls	r7, r4, #27
	lsl	r10, r1, #1
	add	lr, r0, lr, lsl #2
	ldr	r5, [lr, r5]
	ldr	r5, [r5]
	asr	r5, r5, #21
	asr	r8, fp, r5
	bpl	.L90
	cmp	r8, #0
	ldr	r7, .L218+12
	mov	r9, r8
	ldr	lr, [lr, r7]
	it	lt
	addlt	r9, r8, #1
	add	lr, lr, r9, asr #1
	and	lr, lr, r8
.L92:
	lsls	r7, r4, #28
	it	mi
	submi	lr, r8, lr
	ands	r9, r4, #32
	beq	.L94
	add	r1, r1, r10
	ldr	lr, .L218+12
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, lr]
	cmp	r1, r8, asr #1
	it	gt
	subgt	r1, r8, r1
	lsls	r4, r4, #29
	lsl	lr, r1, r5
	bpl	.L96
.L132:
	mov	r1, lr
	sub	lr, fp, lr
	b	.L88
.L94:
	lsls	r1, r4, #29
	bmi	.L132
	mov	r1, lr
	b	.L133
.L90:
	ldr	r9, .L218+12
	ldr	lr, [lr, r9]
	b	.L92
.L115:
	ldr	r9, .L218+12
	ldr	lr, [lr, r9]
	b	.L117
.L219:
	.align	2
.L218:
	.word	.LANCHOR0
	.word	.LANCHOR1
	.word	131380
	.word	131376
	.word	131352
	.word	-8388607
	.size	_Z15execute_programP3FV1, .-_Z15execute_programP3FV1
	.section	.qspiflash_data
	.align	1
	.p2align 2,,3
	.global	dance_ir_h_l_spn
	.syntax unified
	.thumb
	.thumb_func
	.type	dance_ir_h_l_spn, %function
dance_ir_h_l_spn:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	ldr	r2, [r0]
	sub	sp, sp, #28
	ldr	r7, [r0, #76]
	ldr	r6, .L231
	str	r7, [sp]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r3, r1, r7, r6
	mov      r1, r1, asl #9
	orr      r3, r1, r3, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r3, r3, r2
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #0
	str	r3, [r0, #148]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r1, r3, r2
	mov      r1, r1, asl #9
	orr      r4, r1, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r1, .L231+4
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, r7, r1
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r3
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r1, .L231+8
	mov	r3, #8388608
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, ip, r4, r3
	mov      ip, ip, asl #9
	orr      r5, ip, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r4, r1, r6
	mov      r4, r4, asl #9
	orr      r5, r4, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	str	r5, [r0, #144]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r1, r5, r2
	mov      r1, r1, asl #9
	orr      r6, r1, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r4, r7, r3
	mov      r4, r4, asl #9
	orr      r6, r4, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	str	r6, [r0, #140]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r1, r6, r3
	mov      r1, r1, asl #9
	orr      r4, r1, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r4, r3
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	sub	r3, r1, #4194304
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r3, r2
	bgt	.L221
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r1, r3, r2
	mov      r1, r1, asl #9
	orr      r6, r1, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, #4194304
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	str	r6, [r0, #140]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r3, r1, r6, r2
	mov      r1, r1, asl #9
	orr      r3, r1, r3, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
.L221:
	movs	r2, #0
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r1, #8388608
	ldr	lr, [r0, #80]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r4, lr, r1
	mov      r4, r4, asl #9
	orr      r2, r4, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r3
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r1, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r3, r4, r2, r1
	mov      r4, r4, asl #9
	orr      r3, r4, r3, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	sub	r3, r3, #4194304
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r3, #0
	blt	.L229
	ldr	r3, .L231+12
.L222:
	mov	r1, #8388608
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r2, r3, r1
	mov      r2, r2, asl #9
	orr      r4, r2, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, #0
	mov	r4, r2
	str	r2, [sp, #8]
	str	r2, [r0, #184]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, ip, r4, r3
	mov      ip, ip, asl #9
	orr      r2, ip, r2, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [r0, #84]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r2, r8, r4, r1
	mov      r8, r8, asl #9
	orr      r2, r8, r2, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r2, r2, r3
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r1, #4194304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r3, r8, r2, r1
	mov      r8, r8, asl #9
	orr      r3, r8, r3, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	sub	r3, r3, #4194304
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r3, #0
	blt	.L230
	ldr	r2, .L231+12
.L224:
	mov	r3, #8388608
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, r8, r2, r3
	mov      r8, r8, asl #9
	orr      r1, r8, r1, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #0
	mov	r7, r1
	str	r1, [r0, #188]
	str	r1, [sp, #12]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r9, r1, r7, r2
	mov      r1, r1, asl #9
	orr      r9, r1, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
	.thumb
	.syntax unified
	sub	r2, r2, #8388608
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r8, r10, lr, r2
	mov      r10, r10, asl #9
	orr      r8, r10, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, r8, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r7, .L231+8
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r8, r9, lr, r3
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r7
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r8, lr, r3
	mov      r8, r8, asl #9
	orr      r9, r8, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r10, r9, lr
	mov      r10, r10, asl #9
	orr      r8, r10, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r10, r8, lr
	mov      r10, r10, asl #9
	orr      r9, r10, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r9
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r1, r7
	str	r7, [r0, #212]
	str	r7, [sp, #16]
	movs	r7, #0
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, r1, r7
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    lr, r9, r4, r3
	mov      r9, r9, asl #9
	orr      lr, r9, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, lr, r4
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    lr, r9, r8, r4
	mov      r9, r9, asl #9
	orr      lr, r9, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, lr, r4
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #216]
	str	r4, [sp, #20]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r4, r7
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r7, [r0, #92]
	mov	r8, #2097152
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r9, r7, r8
	mov      r9, r9, asl #9
	orr      lr, r9, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r1, r7
	add	lr, lr, ip
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [r0, #96]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, r9, r4, r8
	mov      r9, r9, asl #9
	orr      ip, r9, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [sp, #4]
	add	ip, ip, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, lr, ip, r5
	mov      lr, lr, asl #9
	orr      r8, lr, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r0, #131072
	ldr	r5, [r5, #268]
	add	ip, r5, #202
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	ldr	r7, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, .L231+16
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r7, r4
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	ubfx	lr, r5, #0, #15
	ldr	r7, .L231+20
	add	lr, r0, lr, lsl #2
	str	ip, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, ip, r7
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r10
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, r5, #744
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	ldr	r9, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r8, r9, r4
	mov      r8, r8, asl #9
	orr      ip, r8, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, r5, #203
	b	.L232
.L233:
	.align	2
.L231:
	.word	16768768
	.word	-8388608
	.word	8380160
	.word	8388607
	.word	5029888
	.word	-5038080
.L232:
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	ip, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r8, lr, ip, r7
	mov      lr, lr, asl #9
	orr      r8, lr, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	add	r8, r8, r10
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	addw	ip, r5, #1902
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	ldr	r9, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r9, r4
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	addw	lr, r5, #745
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	ip, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, ip, r7
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r10
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	addw	ip, r5, #3806
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	ldr	r8, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r10, r8, r4
	mov      r10, r10, asl #9
	orr      ip, r10, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	addw	lr, r5, #1903
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	ip, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    fp, lr, ip, r7
	mov      lr, lr, asl #9
	orr      fp, lr, fp, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat fp, #24, fp
@ 0 "" 2
	.thumb
	.syntax unified
	add	fp, fp, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat fp, #24, fp
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r8, #0
	str	fp, [r0, #192]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, ip, fp, r8
	mov      ip, ip, asl #9
	orr      lr, ip, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	movw	ip, #26831
	add	ip, ip, r5
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	ldr	r8, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r9, r8, r3
	mov      r9, r9, asl #9
	orr      ip, r9, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, lr, ip, r6
	mov      lr, lr, asl #9
	orr      r8, lr, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat ip, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, lr, fp, r3
	mov      lr, lr, asl #9
	orr      r8, lr, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r8, r8, ip
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	movw	ip, #6011
	add	ip, ip, r5
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	ldr	r9, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, lr, r9, r4
	mov      lr, lr, asl #9
	orr      ip, lr, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	addw	lr, r5, #3807
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	ip, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, ip, r7
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r10
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	movw	ip, #9313
	add	ip, ip, r5
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	ldr	r8, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r10, r8, r4
	mov      r10, r10, asl #9
	orr      ip, r10, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	movw	lr, #6012
	add	lr, lr, r5
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	ip, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r8, lr, ip, r7
	mov      lr, lr, asl #9
	orr      r8, lr, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	add	r9, r9, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
	.thumb
	.syntax unified
	movw	ip, #9314
	mov	r10, #0
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r8, lr, r9, r10
	mov      lr, lr, asl #9
	orr      r8, lr, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r5
	ubfx	ip, ip, #0, #15
	add	ip, r0, ip, lsl #2
	str	r9, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	movw	lr, #13770
	add	lr, lr, r5
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	ldr	r9, [lr, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    lr, r10, r9, r3
	mov      r10, r10, asl #9
	orr      lr, r10, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, lr, r6
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    lr, r6, fp, r3
	mov      r6, r6, asl #9
	orr      lr, r6, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r6, #17303
	add	r6, r6, r5
	ubfx	r6, r6, #0, #15
	add	r6, r0, r6, lsl #2
	ldr	r9, [r6, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r8, r9, r4
	mov      r8, r8, asl #9
	orr      r6, r8, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	lr, #13771
	add	lr, lr, r5
	ubfx	lr, lr, #0, #15
	add	lr, r0, lr, lsl #2
	str	r6, [lr, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r8, lr, r6, r7
	mov      lr, lr, asl #9
	orr      r8, lr, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	add	r8, r8, r10
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r6, #20505
	add	r6, r6, r5
	ubfx	r6, r6, #0, #15
	add	r6, r0, r6, lsl #2
	ldr	r9, [r6, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, lr, r9, r4
	mov      lr, lr, asl #9
	orr      r6, lr, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r9, #17304
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, r6, r7
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r9, r9, r5
	ubfx	r9, r9, #0, #15
	add	r9, r0, r9, lsl #2
	str	r6, [r9, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r10
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	movw	r6, #20506
	movs	r4, #0
	add	r6, r6, r5
	ubfx	r6, r6, #0, #15
	add	r6, r0, r6, lsl #2
	str	lr, [r6, #268]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r6, lr, r4
	mov      r6, r6, asl #9
	orr      r5, r6, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r8, r1, r2
	mov      r8, r8, asl #9
	orr      r6, r8, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r7, r1
	add	r6, r6, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	ip, [ip, #268]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r8, #12582912
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r9, ip, r8
	mov      r9, r9, asl #9
	orr      r5, r9, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r1, [sp]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, ip, r5, r1
	mov      ip, ip, asl #9
	orr      r6, ip, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, ip, r7, r3
	mov      ip, ip, asl #9
	orr      r6, ip, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r4, #0
	str	r6, [r0, #196]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, ip, r6, r4
	mov      ip, ip, asl #9
	orr      r5, ip, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [sp, #4]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, r9, r4, r2
	mov      r9, r9, asl #9
	orr      ip, r9, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r10, lr, r8
	mov      r10, r10, asl #9
	orr      r5, r10, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r9, [r0, #8]
	add	ip, ip, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, lr, ip, r1
	mov      lr, lr, asl #9
	orr      r5, lr, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, ip, r4, r3
	mov      ip, ip, asl #9
	orr      r5, ip, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #0
	str	r5, [r0, #200]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, ip, r5, r1
	mov      ip, ip, asl #9
	orr      r7, ip, r7, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat ip, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r8, [r0, #152]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, lr, r8, r3
	mov      lr, lr, asl #9
	orr      r7, lr, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, ip
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [sp, #8]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, ip, r7, r4
	mov      ip, ip, asl #9
	orr      lr, ip, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r7, [r0, #156]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, r9, r7, r3
	mov      r9, r9, asl #9
	orr      ip, r9, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	str	r7, [r0, #156]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, ip, r7, r2
	mov      ip, ip, asl #9
	orr      lr, ip, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r7, .L234
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, r9, r8, r7
	mov      r9, r9, asl #9
	orr      ip, r9, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    lr, r9, r6, r3
	mov      r9, r9, asl #9
	orr      lr, r9, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, ip
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r9, lr, r3
	mov      r9, r9, asl #9
	orr      ip, r9, ip, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r10, ip, r4
	mov      r10, r10, asl #9
	orr      r9, r10, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r10, r8, r3
	mov      r10, r10, asl #9
	orr      ip, r10, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #0
	str	r8, [r0, #152]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r9, ip, r8, r1
	mov      ip, ip, asl #9
	orr      r9, ip, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r8, [r0, #168]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, r10, r8, r3
	mov      r10, r10, asl #9
	orr      ip, r10, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r10, ip, r4
	mov      r10, r10, asl #9
	orr      r9, r10, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r1, [r0, #172]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    ip, r10, r1, r3
	mov      r10, r10, asl #9
	orr      ip, r10, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r1, r2
	str	ip, [r0, #172]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r9, r10, ip, r2
	mov      r10, r10, asl #9
	orr      r9, r10, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    ip, r10, r8, r7
	mov      r10, r10, asl #9
	orr      ip, r10, ip, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r10, r5, r3
	mov      r10, r10, asl #9
	orr      r9, r10, r9, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, ip, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r10, ip, r3
	mov      r10, r10, asl #9
	orr      r9, r10, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r10, fp, r9, r4
	mov      fp, fp, asl #9
	orr      r10, fp, r10, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r10, #24, r10
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r4, r9, r8, r3
	mov      r9, r9, asl #9
	orr      r4, r9, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r10, r10, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r10
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #0
	str	r4, [r0, #168]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r8, r9, r4, r2
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, lr, r1
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r2, r1
	add	r8, r8, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r4, r9, r6, r3
	mov      r9, r9, asl #9
	orr      r4, r9, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r1, [sp, #16]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r8, r4, r1
	mov      r8, r8, asl #9
	orr      r6, r8, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r6
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r6, r8, lr, r3
	mov      r8, r8, asl #9
	orr      r6, r8, r6, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r6, r6, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r8, #0
	str	r6, [r0, #204]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r4, r6, r8
	mov      r4, r4, asl #9
	orr      lr, r4, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r4, r8, ip, r2
	mov      r8, r8, asl #9
	orr      r4, r8, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r4
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r4, r8, r5, r3
	mov      r8, r8, asl #9
	orr      r4, r8, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, lr, r4, r1
	mov      lr, lr, asl #9
	orr      r5, lr, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r4, lr, ip, r3
	mov      lr, lr, asl #9
	orr      r4, lr, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	ip, r4, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat ip, #24, ip
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #0
	str	ip, [r0, #208]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r5, ip, r1
	mov      r5, r5, asl #9
	orr      r4, r5, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat lr, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r5, [r0, #160]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r8, r5, r3
	mov      r8, r8, asl #9
	orr      r4, r8, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, lr
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r1, [sp, #12]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r8, r4, r1
	mov      r8, r8, asl #9
	orr      lr, r8, lr, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r4, [r0, #164]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    lr, r9, r4, r3
	mov      r9, r9, asl #9
	orr      lr, r9, lr, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	lr, lr, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat lr, #24, lr
@ 0 "" 2
	.thumb
	.syntax unified
	str	lr, [r0, #164]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r8, lr, r2
	mov      r8, r8, asl #9
	orr      r4, r8, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, r5, r7
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r8, r8, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r8, r9, r6, r3
	mov      r9, r9, asl #9
	orr      r8, r9, r8, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r8, r8, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r8
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r4, r9, r8, r1
	mov      r9, r9, asl #9
	orr      r4, r9, r4, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r8, #24, r4
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r4, r9, r5, r3
	mov      r9, r9, asl #9
	orr      r4, r9, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r8
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r9, #0
	str	r4, [r0, #160]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r8, r4, r9
	mov      r8, r8, asl #9
	orr      r5, r8, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r8, [r0, #176]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r9, r8, r3
	mov      r9, r9, asl #9
	orr      r5, r9, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r4
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r4, r5, r1
	mov      r4, r4, asl #9
	orr      r9, r4, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r5, [r0, #180]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r4, r10, r5, r3
	mov      r10, r10, asl #9
	orr      r4, r10, r4, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r4, r4, r9
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r4, #24, r4
@ 0 "" 2
	.thumb
	.syntax unified
	str	r4, [r0, #180]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r9, r5, r4, r2
	mov      r5, r5, asl #9
	orr      r9, r5, r9, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r10, r8, r7
	mov      r10, r10, asl #9
	orr      r5, r10, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r9, r9, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r9, r7, ip, r3
	mov      r7, r7, asl #9
	orr      r9, r7, r9, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r9, r9, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r9, #24, r9
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r9, r1
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r9, r8, r3
	mov      r9, r9, asl #9
	orr      r5, r9, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #0
	str	r5, [r0, #176]
	b	.L235
.L236:
	.align	2
.L234:
	.word	-1677824
.L235:
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r7, r8, r5, r1
	mov      r8, r8, asl #9
	orr      r7, r8, r7, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r7
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r7, r8, lr, r2
	mov      r8, r8, asl #9
	orr      r7, r8, r7, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r7, r7, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r7, #24, r7
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r8, r6, r3
	mov      r8, r8, asl #9
	orr      r5, r8, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	adds	r6, r5, r7
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r2, [sp, #20]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r6, r2
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r7, lr, r3
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r5, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #0
	str	r5, [r0, #100]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r6, r7, r5, r1
	mov      r7, r7, asl #9
	orr      r6, r7, r6, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r6, #24, r6
@ 0 "" 2
	.thumb
	.syntax unified
	sub	r1, r1, #8388608
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r5, r7, r4, r1
	mov      r7, r7, asl #9
	orr      r5, r7, r5, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	adds	r1, r5, r6
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r1
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r1, r6, ip, r3
	mov      r6, r6, asl #9
	orr      r1, r6, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r1, r1, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r5, r6, r1, r2
	mov      r6, r6, asl #9
	orr      r5, r6, r5, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r5, #24, r5
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r1, r6, r4, r3
	mov      r6, r6, asl #9
	orr      r1, r6, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	adds	r3, r1, r5
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #0
	str	r3, [r0, #4]
	str	r3, [r0, #104]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, r4, r3, r2
	mov      r4, r4, asl #9
	orr      r1, r4, r1, lsr #23
	
@ 0 "" 2
@ 65 "./FV1.hpp" 1
	ssat r1, #24, r1
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r0]
	add	sp, sp, #28
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L229:
	sbfx	r4, r3, #8, #19
	movw	r2, #2046
	lsls	r3, r3, #5
	ands	r4, r4, r2
	ldr	r2, .L237
	ubfx	r1, r3, #0, #14
	asrs	r3, r3, #24
	ldr	ip, [r2, r4, lsl #2]
	adds	r4, r4, #1
	rsb	r3, r3, #6
	smull	r1, ip, r1, ip
	ldr	r2, [r2, r4, lsl #2]
	lsrs	r1, r1, #14
	orr	r1, r1, ip, lsl #18
	add	r2, r2, r1
	asrs	r3, r2, r3
	it	mi
	addmi	r3, r3, #1
	asrs	r3, r3, #1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L222
.L230:
	sbfx	r8, r3, #8, #19
	movw	r2, #2046
	lsls	r3, r3, #5
	and	r8, r8, r2
	ldr	r2, .L237
	ubfx	r1, r3, #0, #14
	asrs	r3, r3, #24
	ldr	r9, [r2, r8, lsl #2]
	add	r8, r8, #1
	rsb	r3, r3, #6
	smull	r1, r9, r1, r9
	ldr	r2, [r2, r8, lsl #2]
	lsrs	r1, r1, #14
	orr	r1, r1, r9, lsl #18
	add	r2, r2, r1
	asrs	r2, r2, r3
	it	mi
	addmi	r2, r2, #1
	asrs	r2, r2, #1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L224
.L238:
	.align	2
.L237:
	.word	exp2tab
	.size	dance_ir_h_l_spn, .-dance_ir_h_l_spn
	.section	.text.fv1_load,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_load
	.syntax unified
	.thumb
	.thumb_func
	.type	fv1_load, %function
fv1_load:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L318
	cmp	r2, #0
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	mov	r10, r0
	sub	sp, sp, #20
	it	eq
	moveq	r2, r3
	mov	r0, #2064
	mov	r4, r1
	blx	r2
	add	r3, r10, #131072
	mov	r6, r0
	movs	r0, #0
	str	r6, [r3, #336]
	str	r3, [sp, #12]
	bl	_Z15execute_programP3FV1
	cmp	r4, #0
	beq	.L241
	ldr	r3, .L318+4
	add	r7, r4, #512
	mov	r1, r4
	ldr	r5, [r3]
	mov	lr, r7
.L287:
	ldrb	r3, [r1, #1]	@ zero_extendqisi2
	ldrb	r2, [r1]	@ zero_extendqisi2
	lsls	r3, r3, #16
	orr	r3, r3, r2, lsl #24
	ldrb	r2, [r1, #3]	@ zero_extendqisi2
	orrs	r3, r3, r2
	ldrb	r2, [r1, #2]	@ zero_extendqisi2
	orr	r3, r3, r2, lsl #8
	cmp	r3, #17
	beq	.L242
	and	ip, r3, #31
	cmp	ip, #20
	bhi	.L292
	tbh	[pc, ip, lsl #1]
.L245:
	.2byte	(.L252-.L245)/2
	.2byte	(.L253-.L245)/2
	.2byte	(.L252-.L245)/2
	.2byte	(.L252-.L245)/2
	.2byte	(.L251-.L245)/2
	.2byte	(.L251-.L245)/2
	.2byte	(.L251-.L245)/2
	.2byte	(.L251-.L245)/2
	.2byte	(.L251-.L245)/2
	.2byte	(.L251-.L245)/2
	.2byte	(.L251-.L245)/2
	.2byte	(.L250-.L245)/2
	.2byte	(.L250-.L245)/2
	.2byte	(.L250-.L245)/2
	.2byte	(.L249-.L245)/2
	.2byte	(.L249-.L245)/2
	.2byte	(.L249-.L245)/2
	.2byte	(.L248-.L245)/2
	.2byte	(.L247-.L245)/2
	.2byte	(.L246-.L245)/2
	.2byte	(.L244-.L245)/2
	.p2align 1
.L251:
	ldr	r7, .L318+8
	ubfx	r2, r3, #5, #6
	mov	r4, ip
	ands	r7, r7, r3
	cmp	r7, #0
	it	lt
	addlt	r7, r7, #255
	asrs	r7, r7, #8
	lsls	r3, r7, #1
	str	r3, [sp, #4]
	movs	r3, #0
	str	r3, [sp, #8]
.L261:
	cmp	r4, #4
	beq	.L312
	cmp	r4, #6
	bne	.L313
	ldr	r3, [sp, #4]
	mov	ip, r0
	cmp	r3, #8388608
	itte	eq
	moveq	r4, #31
	moveq	r8, #124
	movne	r8, #24
	b	.L272
.L252:
	ldr	r7, .L318+12
	ubfx	r2, r3, #5, #16
	mov	r4, ip
	lsl	r8, ip, #2
	ands	r3, r3, r7
	mov	ip, r0
	it	mi
	addmi	r3, r3, #255
	asrs	r3, r3, #8
	lsls	r3, r3, #1
	str	r3, [sp, #4]
	movs	r3, #0
	str	r3, [sp, #8]
	b	.L272
.L249:
	lsrs	r2, r3, #8
	movs	r3, #0
	mov	r4, ip
	lsl	r8, ip, #2
	str	r3, [sp, #8]
	mov	ip, r0
	str	r3, [sp, #4]
	b	.L272
.L250:
	ldr	r2, .L318+8
	mov	r4, ip
	ldr	r7, .L318+12
	ands	r2, r2, r3
	cmp	r2, #0
	it	lt
	addlt	r2, r2, #255
	ands	r3, r7, r3, lsl #16
	it	mi
	addmi	r3, r3, #255
	asrs	r2, r2, #8
	asrs	r3, r3, #8
	lsls	r2, r2, #1
	str	r3, [sp, #4]
	movs	r3, #0
	str	r3, [sp, #8]
.L259:
	cmp	r4, #13
	bne	.L274
	cmp	r2, #0
	beq	.L314
	cmp	r2, #8388608
	beq	.L315
	ldr	r3, [sp, #4]
	mov	ip, r0
	cmp	r3, #0
	itte	eq
	moveq	r4, #28
	moveq	r8, #112
	movne	r8, #52
	b	.L272
.L247:
	ldr	r2, .L318+16
	movs	r4, #29
	bics	r2, r2, r3
	and	r2, r3, #536870912
	beq	.L265
.L267:
	subs	r4, r4, #1
	lsr	r2, r2, #1
	bne	.L267
	ldr	r7, .L318+20
	movs	r4, #20
	ands	r7, r7, r3
.L268:
	subs	r4, r4, #1
	lsr	r7, r7, #1
	bne	.L268
	ubfx	r3, r3, #5, #15
	mov	r8, #72
	movs	r4, #18
	mov	ip, r0
	str	r7, [sp, #4]
	str	r3, [sp, #8]
	b	.L272
.L265:
	subs	r4, r4, #1
	lsr	r2, r2, #1
	bne	.L265
	ubfx	r4, r3, #13, #16
	ubfx	r3, r3, #5, #2
	str	r4, [sp, #4]
	str	r3, [sp, #8]
.L266:
	mov	ip, r0
	mov	r8, #84
	movs	r4, #21
	b	.L272
.L248:
	and	r2, r3, #-134217728
	movs	r4, #27
.L254:
	subs	r4, r4, #1
	lsr	r2, r2, #1
	bne	.L254
	and	r7, r3, #132120576
	movs	r3, #21
.L255:
	subs	r3, r3, #1
	lsr	r7, r7, #1
	bne	.L255
	mov	r8, #68
	movs	r4, #17
	mov	ip, r0
	strd	r7, r3, [sp, #4]
	b	.L272
.L253:
	ldr	r2, .L318+12
	mov	r8, #4
	movs	r4, #1
	mov	ip, r0
	ands	r3, r3, r2
	it	mi
	addmi	r3, r3, #255
	asrs	r3, r3, #8
	lsls	r2, r3, #1
	movs	r3, #0
	str	r3, [sp, #8]
	str	r3, [sp, #4]
	b	.L272
.L244:
	ldr	r4, .L318+24
	and	r2, r3, #6291456
	movs	r7, #21
	ands	r4, r4, r3
.L269:
	subs	r7, r7, #1
	lsr	r2, r2, #1
	bne	.L269
	and	r7, r3, #1056964608
	mov	ip, #24
.L270:
	subs	ip, ip, #1
	lsr	r7, r7, #1
	bne	.L270
	str	r7, [sp, #4]
	ubfx	r3, r3, #5, #16
	ldr	r7, .L318+16
	str	r3, [sp, #8]
	cmp	r4, r7
	beq	.L266
	ldr	r7, .L318+28
	cmp	r4, r7
	bne	.L273
	mov	ip, r0
	mov	r8, #88
	movs	r4, #22
.L272:
	ldr	r9, [r5, r8]
	subs	r4, r4, #4
	add	r8, r6, ip, lsl #4
	ldr	r3, [sp, #4]
	lsl	fp, ip, #4
	cmp	r4, #29
	add	r0, r0, #1
	str	r9, [r6, fp]
	str	r3, [r8, #8]
	ldr	r3, [sp, #8]
	str	r2, [r8, #4]
	str	r3, [r8, #12]
	bhi	.L283
	movs	r3, #1
	lsl	r4, r3, r4
	ldr	r3, .L318+32
	tst	r4, r3
	beq	.L283
	adds	r2, r2, #3
	add	r2, r10, r2, lsl #2
	str	r2, [r8, #4]
.L283:
	adds	r1, r1, #4
	cmp	lr, r1
	bne	.L287
.L242:
	lsls	r0, r0, #4
.L288:
	ldr	r3, [r5, #96]
	movs	r1, #0
	ldr	r4, [sp, #12]
	movs	r2, #124
	str	r3, [r6, r0]
	add	r0, r0, r6
	movs	r3, #1
	str	r1, [r0, #12]
	strd	r1, r1, [r0, #4]
	strb	r3, [r4, #328]
	add	r0, r10, #140
	strd	r1, r1, [r10]
	bl	memset
	ldr	r3, .L318+36
	ldr	r0, [r4, #336]
	str	r3, [r4, #332]
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L246:
	ubfx	r2, r3, #6, #1
	movs	r3, #0
	mov	r8, #76
	movs	r4, #19
	mov	ip, r0
	str	r3, [sp, #8]
	str	r3, [sp, #4]
	b	.L272
.L273:
	ldr	r7, .L318+40
	cmp	r4, r7
	bne	.L259
	mov	ip, r0
	mov	r8, #92
	movs	r4, #23
	b	.L272
.L274:
	cmp	r4, #12
	bne	.L261
	cmp	r2, #8388608
	beq	.L278
	mov	ip, r0
	mov	r8, #48
	b	.L272
.L314:
	mov	ip, r0
	mov	r8, #104
	movs	r4, #26
	b	.L272
.L319:
	.align	2
.L318:
	.word	malloc
	.word	.LANCHOR0
	.word	-65536
	.word	-2097152
	.word	1073741842
	.word	535822336
	.word	-1073741793
	.word	-2147483628
	.word	738197631
	.word	_Z15execute_programP3FV1
	.word	-1073741804
.L313:
	cmp	r4, #5
	bne	.L308
	ldr	r3, [sp, #4]
	mov	ip, r0
	cmp	r3, #0
	itte	eq
	moveq	r4, #33
	moveq	r8, #132
	movne	r8, #20
	b	.L272
.L278:
	ldr	r3, [sp, #4]
	mov	ip, r0
	cmp	r3, #0
	itte	eq
	moveq	r4, #29
	moveq	r8, #116
	movne	r8, #48
	b	.L272
.L312:
	ldr	r3, [sp, #4]
	cmp	r3, #8388608
	beq	.L316
	mvn	r8, #-268435456
	ldr	r3, [r5, #24]
	mov	ip, r0
	add	r8, r8, r0
	lsl	r9, r8, #4
	add	r8, r6, r8, lsl #4
	ldr	r7, [r6, r9]
	cmp	r7, r3
	beq	.L317
.L294:
	mov	r8, #16
	b	.L272
.L241:
	ldr	r3, .L320
	add	r1, r6, #2048
	ldr	r5, [r3]
	mov	r3, r6
.L289:
	adds	r3, r3, #16
	ldr	r2, [r5, #100]
	cmp	r3, r1
	str	r2, [r3, #-16]
	str	r4, [r3, #-4]
	strd	r4, r4, [r3, #-12]
	bne	.L289
	mov	r0, #2048
	b	.L288
.L292:
	movs	r4, #0
	mov	r2, r4
	strd	r4, r4, [sp, #4]
.L308:
	mov	ip, r0
	lsl	r8, r4, #2
	b	.L272
.L315:
	mov	ip, r0
	mov	r8, #108
	movs	r4, #27
	b	.L272
.L316:
	mov	ip, r0
	mov	r8, #120
	movs	r4, #30
	b	.L272
.L317:
	ldr	r3, [r8, #8]
	cmp	r3, #0
	bne	.L294
	ldr	r3, [r5, #128]
	adds	r2, r2, #3
	str	r3, [r6, r9]
	add	r2, r10, r2, lsl #2
	ldr	r3, [sp, #4]
	str	r2, [r8, #8]
	str	r3, [r8, #12]
	b	.L283
.L321:
	.align	2
.L320:
	.word	.LANCHOR0
	.size	fv1_load, .-fv1_load
	.section	.text.fv1_init,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_init
	.syntax unified
	.thumb
	.thumb_func
	.type	fv1_init, %function
fv1_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r3, r0
	ldr	r2, .L329
	ldr	r0, .L329+4
	cmp	r3, #0
	it	eq
	moveq	r3, r2
	blx	r3
	mov	r4, r0
	cmp	r0, #0
	beq	.L322
	movs	r5, #0
	mov	r2, #131072
	add	r0, r0, #268
	mov	r1, r5
	strd	r5, r5, [r4]
	strd	r5, r5, [r4, #8]
	strd	r5, r5, [r4, #16]
	strd	r5, r5, [r4, #24]
	strd	r5, r5, [r4, #32]
	strd	r5, r5, [r4, #40]
	strd	r5, r5, [r4, #48]
	strd	r5, r5, [r4, #56]
	strd	r5, r5, [r4, #64]
	strd	r5, r5, [r4, #72]
	strd	r5, r5, [r4, #80]
	strd	r5, r5, [r4, #88]
	strd	r5, r5, [r4, #96]
	strd	r5, r5, [r4, #104]
	strd	r5, r5, [r4, #112]
	strd	r5, r5, [r4, #120]
	strd	r5, r5, [r4, #128]
	strd	r5, r5, [r4, #136]
	strd	r5, r5, [r4, #144]
	strd	r5, r5, [r4, #152]
	strd	r5, r5, [r4, #160]
	strd	r5, r5, [r4, #168]
	strd	r5, r5, [r4, #176]
	strd	r5, r5, [r4, #184]
	strd	r5, r5, [r4, #192]
	strd	r5, r5, [r4, #200]
	strd	r5, r5, [r4, #208]
	strd	r5, r5, [r4, #216]
	strd	r5, r5, [r4, #224]
	strd	r5, r5, [r4, #232]
	strd	r5, r5, [r4, #240]
	strd	r5, r5, [r4, #248]
	strd	r5, r5, [r4, #256]
	str	r5, [r4, #264]
	bl	memset
	add	r3, r4, #131072
	movs	r2, #1
	ldr	r1, .L329+8
	add	r0, r4, #12
	str	r5, [r3, #268]
	strb	r2, [r3, #328]
	add	r2, r4, #16
	str	r0, [r3, #284]
	add	r0, r4, #28
	str	r2, [r3, #280]
	add	r2, r4, #24
	str	r5, [r3, #304]
	str	r2, [r3, #296]
	add	r2, r4, #32
	str	r5, [r3, #316]
	str	r2, [r3, #308]
	add	r2, r4, #36
	str	r0, [r3, #312]
	strd	r5, r1, [r3, #272]
	strd	r5, r1, [r3, #288]
	add	r1, r4, #20
	str	r1, [r3, #300]
	add	r1, r4, #40
	strd	r5, r5, [r3, #332]
	strd	r1, r2, [r3, #320]
.L322:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L330:
	.align	2
.L329:
	.word	malloc
	.word	131412
	.word	-8388607
	.size	fv1_init, .-fv1_init
	.section	.text.fv1_set_fx,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_set_fx
	.syntax unified
	.thumb
	.thumb_func
	.type	fv1_set_fx, %function
fv1_set_fx:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	cbz	r0, .L337
	ldr	r2, .L340
	push	{r3, r4, r5, lr}
	ldr	r5, [r2, r1, lsl #2]
	add	r4, r0, #131072
	movs	r3, #1
	movs	r1, #0
	movs	r2, #124
	strb	r3, [r4, #328]
	strd	r1, r1, [r0]
	adds	r0, r0, #140
	bl	memset
	str	r5, [r4, #332]
	pop	{r3, r4, r5, pc}
.L337:
	bx	lr
.L341:
	.align	2
.L340:
	.word	.LANCHOR2
	.size	fv1_set_fx, .-fv1_set_fx
	.section	.text.fv1_process,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_process
	.syntax unified
	.thumb
	.thumb_func
	.type	fv1_process, %function
fv1_process:
	@ args = 8, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	cmp	r0, #0
	beq	.L353
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	vpush.64	{d8}
	sub	sp, sp, #20
	mov	r5, r0
	ldr	r0, [sp, #68]
	cmp	r0, #0
	beq	.L342
	vmov.f32	s15, s0
	mov	r9, r1
	mov	r8, r2
	vldr.32	s16, .L356
	ldr	fp, [sp, #64]
	movs	r6, #0
	vcvt.s32.f32	s15, s15, #23
	add	r4, r5, #131072
	ldr	r10, .L356+4
	str	r3, [sp]
	vstr.32	s15, [sp, #4]	@ int
	vmov.f32	s15, s1
	vcvt.s32.f32	s15, s15, #23
	vstr.32	s15, [sp, #8]	@ int
	vmov.f32	s15, s2
	vcvt.s32.f32	s15, s15, #23
	vstr.32	s15, [sp, #12]	@ int
.L344:
	vldmia.32	r9!, {s15}
	vcvt.s32.f32	s15, s15, #23
	vmov	r3, s15	@ int
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	vldmia.32	r8!, {s15}
	str	r3, [r5, #92]
	vcvt.s32.f32	s15, s15, #23
	vmov	r3, s15	@ int
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r5, #96]
	ldr	r3, [sp, #4]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r5, #76]
	ldr	r3, [sp, #8]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r5, #80]
	ldr	r3, [sp, #12]
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
	.thumb
	.syntax unified
	mov	r0, r5
	str	r3, [r5, #84]
	ldr	r3, [r4, #332]
	blx	r3
	ldr	r3, [r4, #268]
	subs	r3, r3, #1
	str	r3, [r4, #268]
	ldr	r3, [r4, #284]
	ldr	r0, [r3]
	movs	r3, #0
	strb	r3, [r4, #328]
	asrs	r0, r0, #8
	ldrd	r2, r3, [r4, #272]
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r2, r0
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r3, r3, r1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r3, r0
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r4, #276]
	subs	r2, r2, r1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r3, [r4, #300]
	str	r2, [r4, #272]
	ldr	r0, [r3]
	ldrd	r2, r3, [r4, #288]
	asrs	r0, r0, #8
	.syntax unified
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r2, r0
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	add	r3, r3, r1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r3, #24, r3
@ 0 "" 2
@ 75 "./FV1.hpp" 1
	smull    r1, ip, r3, r0
	mov      ip, ip, asl #9
	orr      r1, ip, r1, lsr #23
	
@ 0 "" 2
	.thumb
	.syntax unified
	str	r3, [r4, #292]
	subs	r2, r2, r1
	.syntax unified
@ 65 "./FV1.hpp" 1
	ssat r2, #24, r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r3, [r4, #308]
	adds	r6, r6, #1
	str	r2, [r4, #288]
	ldr	r0, [r3]
	ldr	r3, [r4, #320]
	ldr	r7, [r4, #304]
	asrs	r0, r0, #21
	ldr	r1, [r3]
	ldr	r3, [r4, #312]
	asr	r0, r10, r0
	vldr.32	s15, [r5, #100]	@ int
	asrs	r1, r1, #21
	ldr	r2, [r3]
	ldr	r3, [r4, #324]
	vcvt.f32.s32	s14, s15
	sub	r2, r7, r2, asr #12
	vldr.32	s15, [r5, #104]	@ int
	ldr	r3, [r3]
	asr	r1, r10, r1
	ands	r2, r2, r0
	vmul.f32	s14, s14, s16
	vcvt.f32.s32	s15, s15
	str	r2, [r4, #304]
	ldr	r2, [r4, #316]
	vmul.f32	s15, s15, s16
	sub	r3, r2, r3, asr #12
	ands	r1, r1, r3
	ldr	r3, [sp]
	str	r1, [r4, #316]
	vstmia.32	r3!, {s14}
	str	r3, [sp]
	ldr	r3, [sp, #68]
	vstmia.32	fp!, {s15}
	cmp	r3, r6
	bne	.L344
.L342:
	add	sp, sp, #20
	@ sp needed
	vldm	sp!, {d8}
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L353:
	bx	lr
.L357:
	.align	2
.L356:
	.word	872415232
	.word	4194303
	.size	fv1_process, .-fv1_process
	.section	.text.fv1_random,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_random
	.syntax unified
	.thumb
	.thumb_func
	.type	fv1_random, %function
fv1_random:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r4, r5}
	ldr	r5, .L360
	ldrd	r0, [r5]
	lsrs	r3, r0, #12
	eor	r2, r1, r1, lsr #12
	orr	r3, r3, r1, lsl #20
	lsls	r1, r2, #25
	eors	r3, r3, r0
	eor	r4, r3, r3, lsl #25
	orr	r1, r1, r3, lsr #7
	lsrs	r0, r4, #27
	eors	r1, r1, r2
	orr	r0, r0, r1, lsl #5
	eor	r3, r1, r1, lsr #27
	eor	r2, r0, r4
	ldr	r0, .L360+4
	ldr	r4, .L360+8
	mul	r0, r2, r0
	strd	r2, [r5]
	mla	r0, r4, r3, r0
	umull	r2, r4, r2, r4
	add	r0, r0, r4
	pop	{r4, r5}
	bx	lr
.L361:
	.align	2
.L360:
	.word	.LANCHOR3
	.word	625341585
	.word	1332534557
	.size	fv1_random, .-fv1_random
	.section	.text.fv1_fake_cv_trig,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_fake_cv_trig
	.syntax unified
	.thumb
	.thumb_func
	.type	fv1_fake_cv_trig, %function
fv1_fake_cv_trig:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	cbz	r0, .L363
	cmp	r2, #0
	beq	.L376
	mov	r3, #1065353216
	add	r0, r1, r2, lsl #2
.L365:
	str	r3, [r1], #4	@ float
	cmp	r0, r1
	bne	.L365
	bx	lr
.L363:
	cbz	r2, .L376
	vmov.f32	s14, #5.0e-1
	add	r0, r1, r2, lsl #2
	ldr	ip, .L379+8
	push	{r4, r5, r6, r7, r8, r9, lr}
	ldr	r7, .L379
	ldr	lr, .L379+12
	ldr	r6, .L379+4
.L366:
	ldrd	r2, [r7]
	lsr	r9, r2, #12
	eor	r8, r3, r3, lsr #12
	orr	r9, r9, r3, lsl #20
	lsl	r3, r8, #25
	eor	r2, r2, r9
	eor	r9, r2, r2, lsl #25
	orr	r3, r3, r2, lsr #7
	lsr	r2, r9, #27
	eor	r3, r8, r3
	orr	r2, r2, r3, lsl #5
	eor	r5, r3, r3, lsr #27
	eor	r4, r9, r2
	mul	r3, ip, r4
	umull	r8, r2, r4, r6
	mla	r3, r6, r5, r3
	add	r3, r3, r2
	and	r3, r3, #63
	add	r3, lr, r3, lsl #2
	vldr.32	s15, [r3]
	vmul.f32	s15, s15, s14
	strd	r4, [r7]
	vstmia.32	r1!, {s15}
	cmp	r1, r0
	bne	.L366
	pop	{r4, r5, r6, r7, r8, r9, pc}
.L376:
	bx	lr
.L380:
	.align	2
.L379:
	.word	.LANCHOR3
	.word	1332534557
	.word	625341585
	.word	.LANCHOR4
	.size	fv1_fake_cv_trig, .-fv1_fake_cv_trig
	.global	fv1_programs
	.section	.bss._ZL15_dispatch_table,"aw",%nobits
	.align	2
	.set	.LANCHOR0,. + 0
	.type	_ZL15_dispatch_table, %object
	.size	_ZL15_dispatch_table, 4
_ZL15_dispatch_table:
	.space	4
	.section	.data._ZL10prng_state,"aw"
	.align	3
	.set	.LANCHOR3,. + 0
	.type	_ZL10prng_state, %object
	.size	_ZL10prng_state, 8
_ZL10prng_state:
	.word	1234
	.word	0
	.section	.data._ZZ15execute_programP3FV1E14dispatch_table,"aw"
	.align	2
	.set	.LANCHOR1,. + 0
	.type	_ZZ15execute_programP3FV1E14dispatch_table, %object
	.size	_ZZ15execute_programP3FV1E14dispatch_table, 136
_ZZ15execute_programP3FV1E14dispatch_table:
	.word	.L46
	.word	.L47
	.word	.L49
	.word	.L50
	.word	.L51
	.word	.L52
	.word	.L53
	.word	.L54
	.word	.L55
	.word	.L56
	.word	.L57
	.word	.L58
	.word	.L61
	.word	.L64
	.word	.L65
	.word	.L67
	.word	.L69
	.word	.L71
	.word	.L78
	.word	.L81
	.word	.L82
	.word	.L97
	.word	.L109
	.word	.L100
	.word	.L44
	.word	.L121
	.word	.L122
	.word	.L123
	.word	.L124
	.word	.L125
	.word	.L128
	.word	.L129
	.word	.L130
	.word	.L131
	.section	.data.fv1_programs,"aw"
	.align	2
	.set	.LANCHOR2,. + 0
	.type	fv1_programs, %object
	.size	fv1_programs, 1024
fv1_programs:
	.word	dance_ir_h_l_spn
	.word	OEM1_4_spn
	.space	1016
	.section	.rodata._ZZ16fv1_fake_cv_trigE8trig_wav,"a"
	.align	2
	.set	.LANCHOR4,. + 0
	.type	_ZZ16fv1_fake_cv_trigE8trig_wav, %object
	.size	_ZZ16fv1_fake_cv_trigE8trig_wav, 256
_ZZ16fv1_fake_cv_trigE8trig_wav:
	.word	-1111036177
	.word	-1110896367
	.word	-1110896367
	.word	-1111036177
	.word	-1110896367
	.word	-1111036177
	.word	-1110896367
	.word	-1111036177
	.word	-1111141035
	.word	-1111036177
	.word	-1111036177
	.word	-1111141035
	.word	-1111036177
	.word	-1111036177
	.word	-1111280845
	.word	-1111141035
	.word	-1111141035
	.word	-1111036177
	.word	-1110756557
	.word	-1111141035
	.word	-1111141035
	.word	-1111036177
	.word	-1110896367
	.word	-1111036177
	.word	-1111280845
	.word	-1111280845
	.word	-1110511889
	.word	-1110896367
	.word	-1111036177
	.word	-1110896367
	.word	-1111036177
	.word	-1111141035
	.word	-1110896367
	.word	-1111141035
	.word	-1111036177
	.word	-1110896367
	.word	-1110896367
	.word	-1111141035
	.word	-1111141035
	.word	-1110896367
	.word	-1111141035
	.word	-1111036177
	.word	-1110896367
	.word	-1111141035
	.word	-1110896367
	.word	-1111141035
	.word	-1110756557
	.word	-1110896367
	.word	-1111036177
	.word	-1111280845
	.word	-1110896367
	.word	-1110756557
	.word	-1110756557
	.word	-1111036177
	.word	-1110756557
	.word	-1111036177
	.word	-1110756557
	.word	-1110896367
	.word	-1110756557
	.word	-1111665323
	.word	-1111036177
	.word	-1110896367
	.word	-1111141035
	.word	-1110896367
	.ident	"GCC: (xPack GNU Arm Embedded GCC x86_64) 10.3.1 20210824 (release)"
