	.cpu cortex-m7
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"FV1.cpp"
	.text
	.section	.flashmem,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	OEM1_4_spn
	.arch armv7e-m
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-d16
	.type	OEM1_4_spn, %function
OEM1_4_spn:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L116
	ldrb	r3, [r0, r3]	@ zero_extendqisi2
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	sub	sp, sp, #28
	cmp	r3, #0
	bne	.L2
	ldr	r3, .L116+4
	ldr	r4, .L116+8
	ldr	r1, [r0, r3]
	ldr	r3, [r0]
	subs	r4, r4, r1
	mov	r5, r1
	asrs	r6, r1, #31
	ssat	r9, #25, r1
	asrs	r2, r1, #10
	add	ip, r3, #1
	strd	r5, [sp, #8]
	add	fp, r2, #1
	mov	r5, r4
	asrs	r6, r4, #31
	strd	r5, [sp, #16]
.L3:
	ldr	r3, [r0, #96]
	ldr	r1, .L116+12
	asrs	r7, r3, #31
	lsls	r4, r3, #21
	ldr	lr, [r0, r1]
	lsls	r1, r7, #21
	cmp	r4, #0
	ubfx	r8, lr, #0, #15
	orr	r5, r1, r3, lsr #11
	add	r8, r0, r8, lsl #2
	sbcs	r3, r5, #0
	bge	.L4
	ldr	r3, .L116+16
	movs	r1, #0
	adds	r4, r4, r3
	adc	r5, r1, r5
.L4:
	lsrs	r1, r4, #23
	ldr	r3, [r0, #100]
	mvn	r10, #-16777216
	orr	r1, r1, r5, lsl #9
	lsls	r4, r3, #21
	cmp	r1, #-16777216
	asr	r7, r3, #31
	str	r4, [sp]
	it	lt
	movlt	r1, #-16777216
	ldr	r4, [r0, #4]
	lsls	r6, r7, #21
	cmp	r1, r10
	ite	le
	addle	r4, r4, r1
	addgt	r4, r4, r10
	orr	r3, r6, r3, lsr #11
	cmp	r4, #-16777216
	mov	r1, r4
	str	r3, [sp, #4]
	it	lt
	movlt	r1, #-16777216
	ldrd	r4, [sp]
	cmp	r1, r10
	it	ge
	movge	r1, r10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L5
	ldr	r3, .L116+16
	movs	r6, #0
	adds	r3, r4, r3
	str	r3, [sp]
	ldr	r3, [sp, #4]
	adc	r3, r6, r3
	str	r3, [sp, #4]
.L5:
	mvn	r4, #-16777216
	add	r2, r2, lr
	ldrd	r5, [sp]
	ubfx	r2, r2, #0, #15
	lsrs	r3, r5, #23
	add	r2, r0, r2, lsl #2
	orr	r3, r3, r6, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r4
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r4
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r4, r1
	it	ge
	movge	r4, r1
	ldrd	r6, [sp, #16]
	str	r4, [r8, #272]
	ldr	r2, [r2, #272]
	asrs	r5, r2, #31
	mul	r1, r2, r7
	umull	r2, r3, r2, r6
	mla	r1, r6, r5, r1
	cmp	r2, #0
	add	r3, r3, r1
	sbcs	r1, r3, #0
	bge	.L6
	ldr	r1, .L116+16
	movs	r4, #0
	adds	r2, r2, r1
	adc	r3, r4, r3
.L6:
	lsrs	r6, r2, #23
	add	fp, fp, lr
	orr	r7, r6, r3, lsl #9
	ubfx	fp, fp, #0, #15
	mvn	r6, #-16777216
	cmp	r7, #-16777216
	add	fp, r0, fp, lsl #2
	it	lt
	movlt	r7, #-16777216
	ldr	r1, [fp, #272]
	cmp	r7, r6
	mov	r3, r1
	asr	r4, r1, #31
	it	ge
	movge	r7, r6
	cmp	r1, #-16777216
	ldrd	r10, [sp, #8]
	strd	r3, [sp]
	umull	r2, r3, r1, r10
	mul	r4, r1, fp
	it	lt
	movlt	r1, #-16777216
	mov	r8, r1
	ldr	r1, [sp, #4]
	cmp	r8, r6
	mla	r4, r10, r1, r4
	mov	r1, r8
	add	r3, r3, r4
	it	ge
	movge	r1, r6
	cmp	r2, #0
	str	r1, [r0, #12]
	sbcs	r1, r3, #0
	bge	.L7
	ldr	r1, .L116+16
	movs	r4, #0
	adds	r2, r2, r1
	adc	r3, r4, r3
.L7:
	lsrs	r6, r2, #23
	mvn	r4, #-16777216
	ldr	r2, [r0, #84]
	orr	r6, r6, r3, lsl #9
	ldr	r5, .L116+20
	movw	r3, #4101
	cmp	r6, #-16777216
	add	r3, r3, lr
	it	lt
	movlt	r6, #-16777216
	ubfx	r3, r3, #0, #15
	cmp	r6, r4
	ite	le
	addle	r6, r7, r6
	addgt	r6, r7, r4
	smull	r7, r8, r2, r5
	add	r1, r0, r3, lsl #2
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r4
	it	ge
	movge	r6, r4
	cmp	r7, #0
	sbcs	r3, r8, #0
	str	r6, [r1, #272]
	bge	.L8
	ldr	r1, .L116+16
	movs	r4, #0
	adds	r7, r7, r1
	adc	r8, r4, r8
.L8:
	ldr	r2, [r0, #80]
	lsrs	r1, r7, #23
	mvn	r4, #-16777216
	cmp	r2, #-16777216
	orr	r1, r1, r8, lsl #9
	it	lt
	movlt	r2, #-16777216
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r2, r4
	it	ge
	movge	r2, r4
	cmp	r1, r4
	ite	le
	addle	r3, r9, r1
	addgt	r3, r9, r4
	cmp	r3, #-16777216
	and	r7, r2, #15728640
	it	lt
	movlt	r3, #-16777216
	sub	r2, r7, #7864320
	cmp	r3, r4
	it	ge
	movge	r3, r4
	cmp	r2, #0
	str	r3, [r0, #32]
	bgt	.L100
	sub	r5, r7, #7340032
	cmp	r5, #0
	bgt	.L101
	sub	r1, r7, #6815744
	cmp	r1, #0
	bgt	.L102
	sub	r5, r7, #6291456
	cmp	r5, #0
	bgt	.L103
	sub	r3, r7, #5767168
	cmp	r3, #0
	bgt	.L104
	sub	r3, r7, #5242880
	cmp	r3, #0
	bgt	.L105
	sub	r3, r7, #4718592
	cmp	r3, #0
	bgt	.L106
	sub	r5, r7, #4194304
	cmp	r5, #0
	bgt	.L107
	sub	r4, r7, #3670016
	cmp	r4, #0
	bgt	.L108
	sub	r1, r7, #3145728
	cmp	r1, #0
	bgt	.L109
	sub	r2, r7, #2621440
	cmp	r2, #0
	bgt	.L110
	sub	r3, r7, #2097152
	cmp	r3, #0
	bgt	.L111
	sub	r1, r7, #1572864
	cmp	r1, #0
	bgt	.L112
	sub	r1, r7, #1048576
	cmp	r1, #0
	bgt	.L113
	sub	r1, r7, #524288
	cmp	r1, #0
	bgt	.L114
	add	r3, ip, #22
	mov	r2, #524288
	str	r3, [r0]
	mov	r3, r2
	str	r2, [r0, #4]
	b	.L39
.L117:
	.align	2
.L116:
	.word	131404
	.word	131380
	.word	4194303
	.word	131344
	.word	8388607
	.word	-4194304
.L100:
	movw	r3, #18435
	add	r4, ip, #38
	ldr	r1, .L118
	add	r3, r3, lr
	str	r4, [r0]
	ubfx	r3, r3, #0, #15
	add	r3, r0, r3, lsl #2
	ldr	r4, [r3, #272]
	smull	r4, r5, r4, r1
	cmp	r4, #0
	sbcs	r3, r5, #0
	blt	.L115
.L10:
	lsrs	r3, r4, #23
	mvn	r1, #-16777216
	orr	r3, r3, r5, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r1
	ite	le
	addle	r5, r2, r3
	addgt	r5, r2, r1
	cmp	r5, r1
	it	ge
	movge	r5, r1
.L11:
	movw	r2, #18124
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r1, [r2, #272]
	asrs	r3, r1, #31
	lsl	r8, r1, #22
	lsls	r3, r3, #22
	cmp	r8, #0
	orr	r9, r3, r1, lsr #10
	sbcs	r3, r9, #0
	bge	.L40
	ldr	r3, .L118+4
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L40:
	lsr	r3, r8, #23
	mvn	r2, #-16777216
	orr	r3, r3, r9, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	ite	le
	addle	r1, r5, r3
	addgt	r1, r5, r2
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r2
	it	ge
	movge	r1, r2
.L14:
	movw	r2, #17609
	ldr	r3, .L118
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r4, [r2, #272]
	smull	r4, r5, r4, r3
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L41
	ldr	r3, .L118+4
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L41:
	lsrs	r3, r4, #23
	mvn	r2, #-16777216
	orr	r3, r3, r5, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	ite	le
	addle	r5, r1, r3
	addgt	r5, r1, r2
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r2
	it	ge
	movge	r5, r2
.L16:
	movw	r2, #17202
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r1, [r2, #272]
	asrs	r3, r1, #31
	lsl	r8, r1, #22
	lsls	r3, r3, #22
	cmp	r8, #0
	orr	r9, r3, r1, lsr #10
	sbcs	r3, r9, #0
	bge	.L42
	ldr	r3, .L118+4
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L42:
	lsr	r3, r8, #23
	mvn	r2, #-16777216
	orr	r3, r3, r9, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	ite	le
	addle	r3, r5, r3
	addgt	r3, r5, r2
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	it	ge
	movge	r3, r2
.L18:
	movw	r2, #16861
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #272]
	asr	r9, r2, #31
	lsl	r10, r2, #22
	lsl	r1, r9, #22
	cmp	r10, #0
	orr	fp, r1, r2, lsr #10
	sbcs	r2, fp, #0
	bge	.L43
	ldr	r2, .L118+4
	movs	r1, #0
	adds	r10, r10, r2
	adc	fp, r1, fp
.L43:
	lsr	r4, r10, #23
	mvn	r2, #-16777216
	orr	r4, r4, fp, lsl #9
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r2
	ite	le
	addle	r3, r3, r4
	addgt	r3, r3, r2
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	it	ge
	movge	r3, r2
.L20:
	movw	r7, #16221
	ldr	r2, .L118
	add	r7, r7, lr
	ubfx	r7, r7, #0, #15
	add	r7, r0, r7, lsl #2
	ldr	r1, [r7, #272]
	smull	r8, r9, r1, r2
	cmp	r8, #0
	sbcs	r2, r9, #0
	bge	.L44
	ldr	r2, .L118+4
	movs	r1, #0
	adds	r8, r8, r2
	adc	r9, r9, r1
.L44:
	lsr	r4, r8, #23
	mvn	r2, #-16777216
	orr	r4, r4, r9, lsl #9
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r2
	ite	le
	addle	r3, r3, r4
	addgt	r3, r3, r2
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	it	ge
	movge	r3, r2
.L22:
	movw	r7, #15866
	add	r7, r7, lr
	ubfx	r7, r7, #0, #15
	add	r7, r0, r7, lsl #2
	ldr	r2, [r7, #272]
	asr	r9, r2, #31
	lsl	r10, r2, #22
	lsl	r1, r9, #22
	cmp	r10, #0
	orr	fp, r1, r2, lsr #10
	sbcs	r2, fp, #0
	bge	.L45
	ldr	r2, .L118+4
	movs	r1, #0
	adds	r10, r10, r2
	adc	fp, r1, fp
.L45:
	lsr	r4, r10, #23
	mvn	r2, #-16777216
	orr	r4, r4, fp, lsl #9
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r2
	ite	le
	addle	r5, r3, r4
	addgt	r5, r3, r2
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r2
	it	ge
	movge	r5, r2
.L24:
	movw	r3, #15413
	add	r3, r3, lr
	ubfx	r3, r3, #0, #15
	add	r3, r0, r3, lsl #2
	ldr	r1, [r3, #272]
	asr	r9, r1, #31
	lsl	r10, r1, #22
	lsl	r4, r9, #22
	cmp	r10, #0
	orr	fp, r4, r1, lsr #10
	sbcs	r3, fp, #0
	bge	.L46
	ldr	r1, .L118+4
	movs	r4, #0
	adds	r10, r10, r1
	adc	fp, r4, fp
.L46:
	lsr	r1, r10, #23
	mvn	r2, #-16777216
	orr	r1, r1, fp, lsl #9
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r2
	ite	le
	addle	r4, r5, r1
	addgt	r4, r5, r2
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r2
	it	ge
	movge	r4, r2
.L26:
	movw	r3, #14851
	add	r3, r3, lr
	ubfx	r3, r3, #0, #15
	add	r3, r0, r3, lsl #2
	ldr	r1, [r3, #272]
	asr	r9, r1, #31
	lsl	r10, r1, #22
	lsl	r5, r9, #22
	cmp	r10, #0
	orr	fp, r5, r1, lsr #10
	sbcs	r3, fp, #0
	bge	.L47
	ldr	r1, .L118+4
	movs	r5, #0
	adds	r10, r10, r1
	adc	fp, r5, fp
.L47:
	lsr	r2, r10, #23
	mvn	r5, #-16777216
	orr	r2, r2, fp, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r5
	ite	le
	addle	r1, r4, r2
	addgt	r1, r4, r5
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r5
	it	ge
	movge	r1, r5
.L28:
	movw	r3, #14313
	add	r3, r3, lr
	ubfx	r3, r3, #0, #15
	add	r3, r0, r3, lsl #2
	ldr	r7, [r3, #272]
	asrs	r3, r7, #31
	lsl	r8, r7, #22
	lsls	r3, r3, #22
	cmp	r8, #0
	orr	r9, r3, r7, lsr #10
	sbcs	r3, r9, #0
	bge	.L48
	ldr	r3, .L118+4
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r2, r9
	b	.L119
.L120:
	.align	2
.L118:
	.word	3350528
	.word	8388607
.L119:
.L48:
	lsr	r3, r8, #23
	mvn	r4, #-16777216
	orr	r3, r3, r9, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r4
	ite	le
	addle	r2, r1, r3
	addgt	r2, r1, r4
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r4
	it	ge
	movge	r2, r4
.L30:
	movw	r3, #13806
	add	r3, r3, lr
	ubfx	r3, r3, #0, #15
	add	r3, r0, r3, lsl #2
	ldr	r3, [r3, #272]
	asr	r9, r3, #31
	lsls	r4, r3, #22
	lsl	r1, r9, #22
	cmp	r4, #0
	orr	r5, r1, r3, lsr #10
	sbcs	r3, r5, #0
	bge	.L49
	ldr	r3, .L121
	movs	r1, #0
	adds	r4, r4, r3
	adc	r5, r1, r5
.L49:
	lsrs	r3, r4, #23
	mvn	r1, #-16777216
	orr	r3, r3, r5, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r1
	ite	le
	addle	r3, r2, r3
	addgt	r3, r2, r1
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r1
	it	ge
	movge	r3, r1
.L32:
	movw	r2, #13468
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #272]
	asr	r9, r2, #31
	lsl	r10, r2, #22
	lsl	r1, r9, #22
	cmp	r10, #0
	orr	fp, r1, r2, lsr #10
	sbcs	r2, fp, #0
	bge	.L50
	ldr	r2, .L121
	movs	r1, #0
	adds	r10, r10, r2
	adc	fp, r1, fp
.L50:
	lsr	r2, r10, #23
	mvn	r4, #-16777216
	orr	r2, r2, fp, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r4
	ite	le
	addle	r1, r3, r2
	addgt	r1, r3, r4
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r4
	it	ge
	movge	r1, r4
.L34:
	movw	r2, #13006
	ldr	r3, .L121+4
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r4, [r2, #272]
	smull	r4, r5, r4, r3
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L51
	ldr	r3, .L121
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L51:
	lsrs	r3, r4, #23
	mvn	r2, #-16777216
	orr	r3, r3, r5, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r2
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r2
	it	ge
	movge	r1, r2
.L36:
	movw	r2, #12676
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r7, [r2, #272]
	asrs	r3, r7, #31
	lsls	r4, r7, #22
	lsls	r3, r3, #22
	cmp	r4, #0
	orr	r5, r3, r7, lsr #10
	sbcs	r3, r5, #0
	bge	.L52
	ldr	r3, .L121
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L52:
	lsrs	r3, r4, #23
	mvn	r2, #-16777216
	orr	r3, r3, r5, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r2
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r2
	it	ge
	movge	r1, r2
.L38:
	movw	r2, #12316
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r7, [r2, #272]
	asrs	r3, r7, #31
	lsl	r8, r7, #22
	lsls	r3, r3, #22
	cmp	r8, #0
	orr	r9, r3, r7, lsr #10
	sbcs	r3, r9, #0
	bge	.L53
	ldr	r3, .L121
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r2, r9
.L53:
	lsr	r3, r8, #23
	movw	r2, #11513
	mvn	r7, #-16777216
	ldr	r4, .L121+4
	orr	r3, r3, r9, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r3, r1, r3
	addgt	r3, r1, r7
	ldr	r2, [r2, #272]
	cmp	r3, #-16777216
	smull	r4, r5, r2, r4
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r7
	it	ge
	movge	r3, r7
	cmp	r4, #0
	sbcs	r2, r5, #0
	bge	.L54
	ldr	r2, .L121
	movs	r1, #0
	adds	r4, r4, r2
	adc	r5, r1, r5
.L54:
	lsrs	r1, r4, #23
	mvn	r2, #-16777216
	orr	r1, r1, r5, lsl #9
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r2
	ite	le
	addle	r3, r3, r1
	addgt	r3, r3, r2
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	it	ge
	movge	r3, r2
	str	r3, [r0, #4]
.L39:
	movw	r2, #11245
	add	r2, r2, lr
	ubfx	r2, r2, #0, #15
	add	r2, r0, r2, lsl #2
	ldr	r2, [r2, #272]
	asr	r9, r2, #31
	lsls	r4, r2, #22
	lsl	r1, r9, #22
	cmp	r4, #0
	orr	r5, r1, r2, lsr #10
	sbcs	r2, r5, #0
	bge	.L55
	ldr	r2, .L121
	movs	r1, #0
	adds	r4, r4, r2
	adc	r5, r1, r5
.L55:
	lsrs	r1, r4, #23
	movw	r2, #10614
	mvn	r7, #-16777216
	orr	r1, r1, r5, lsl #9
	add	r2, r2, lr
	cmp	r1, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r1, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r1, r7
	ite	le
	addle	r3, r3, r1
	addgt	r3, r3, r7
	ldr	r2, [r2, #272]
	cmp	r3, #-16777216
	asr	r9, r2, #31
	lsl	r4, r2, #22
	it	lt
	movlt	r3, #-16777216
	lsl	r1, r9, #22
	cmp	r3, r7
	orr	r5, r1, r2, lsr #10
	it	ge
	movge	r3, r7
	cmp	r4, #0
	sbcs	r2, r5, #0
	bge	.L56
	ldr	r2, .L121
	movs	r1, #0
	adds	r4, r4, r2
	adc	r5, r1, r5
.L56:
	lsrs	r1, r4, #23
	movw	r2, #9923
	mvn	r7, #-16777216
	orr	r1, r1, r5, lsl #9
	add	r2, r2, lr
	cmp	r1, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r1, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r1, r7
	ite	le
	addle	r1, r3, r1
	addgt	r1, r3, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L57
	ldr	r3, .L121
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L57:
	lsrs	r3, r4, #23
	movw	r2, #9421
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L58
	ldr	r3, .L121
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
	b	.L122
.L123:
	.align	2
.L121:
	.word	8388607
	.word	3350528
.L122:
.L58:
	lsrs	r3, r4, #23
	movw	r2, #9239
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L59
	ldr	r3, .L124
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L59:
	lsrs	r3, r4, #23
	movw	r2, #8677
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r3, r1, r3
	addgt	r3, r1, r7
	ldr	r2, [r2, #272]
	cmp	r3, #-16777216
	asr	r9, r2, #31
	lsl	r10, r2, #22
	it	lt
	movlt	r3, #-16777216
	lsl	r1, r9, #22
	cmp	r3, r7
	orr	fp, r1, r2, lsr #10
	it	ge
	movge	r3, r7
	cmp	r10, #0
	sbcs	r2, fp, #0
	bge	.L60
	ldr	r2, .L124
	movs	r1, #0
	adds	r10, r10, r2
	adc	fp, r1, fp
.L60:
	lsr	r1, r10, #23
	movw	r2, #8008
	mvn	r7, #-16777216
	ldr	r4, .L124+4
	orr	r1, r1, fp, lsl #9
	add	r2, r2, lr
	cmp	r1, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r1, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r1, r7
	ite	le
	addle	r3, r3, r1
	addgt	r3, r3, r7
	ldr	r2, [r2, #272]
	cmp	r3, #-16777216
	smull	r4, r5, r2, r4
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r7
	it	ge
	movge	r3, r7
	cmp	r4, #0
	sbcs	r2, r5, #0
	bge	.L61
	ldr	r2, .L124
	movs	r1, #0
	adds	r4, r4, r2
	adc	r5, r1, r5
.L61:
	lsrs	r1, r4, #23
	movw	r2, #7521
	mvn	r7, #-16777216
	orr	r1, r1, r5, lsl #9
	add	r2, r2, lr
	cmp	r1, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r1, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r1, r7
	ite	le
	addle	r3, r3, r1
	addgt	r3, r3, r7
	ldr	r2, [r2, #272]
	cmp	r3, #-16777216
	asr	r9, r2, #31
	lsl	r4, r2, #22
	it	lt
	movlt	r3, #-16777216
	lsl	r1, r9, #22
	cmp	r3, r7
	orr	r5, r1, r2, lsr #10
	it	ge
	movge	r3, r7
	cmp	r4, #0
	sbcs	r2, r5, #0
	bge	.L62
	ldr	r2, .L124
	movs	r1, #0
	adds	r4, r4, r2
	adc	r5, r1, r5
.L62:
	lsrs	r1, r4, #23
	movw	r2, #7075
	mvn	r7, #-16777216
	orr	r1, r1, r5, lsl #9
	add	r2, r2, lr
	cmp	r1, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r1, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r1, r7
	ite	le
	addle	r1, r3, r1
	addgt	r1, r3, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L63
	ldr	r3, .L124
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L63:
	lsrs	r3, r4, #23
	movw	r2, #6631
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L64
	ldr	r3, .L124
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L64:
	lsrs	r3, r4, #23
	movw	r2, #6211
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L65
	ldr	r3, .L124
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L65:
	lsrs	r3, r4, #23
	movw	r2, #5441
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L66
	ldr	r3, .L124
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L66:
	lsrs	r3, r4, #23
	add	r2, lr, #5024
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	ubfx	r2, r2, #0, #15
	cmp	r3, #-16777216
	add	r2, r0, r2, lsl #2
	it	lt
	movlt	r3, #-16777216
	ldr	ip, [r2, #272]
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	mov	r2, ip
	lsl	r4, ip, #22
	cmp	r1, #-16777216
	asr	r3, r2, #31
	it	lt
	movlt	r1, #-16777216
	lsls	r3, r3, #22
	cmp	r1, r7
	orr	r5, r3, ip, lsr #10
	it	ge
	movge	r1, r7
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L67
	ldr	r3, .L124
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L67:
	lsrs	r3, r4, #23
	movw	r2, #4601
	mvn	r7, #-16777216
	orr	r3, r3, r5, lsl #9
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	ldr	ip, [r2, #272]
	cmp	r1, #-16777216
	mov	r2, ip
	lsl	r4, ip, #22
	it	lt
	movlt	r1, #-16777216
	asrs	r3, r2, #31
	cmp	r1, r7
	lsl	r3, r3, #22
	it	ge
	movge	r1, r7
	orr	r5, r3, ip, lsr #10
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L68
	ldr	r3, .L124
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L68:
	lsrs	r3, r4, #23
	mvn	r2, #-16777216
	ldr	r4, .L124+8
	orr	r3, r3, r5, lsl #9
	smull	r6, r7, r6, r4
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	b	.L125
.L126:
	.align	2
.L124:
	.word	8388607
	.word	3350528
	.word	5029888
.L125:
	cmp	r3, r2
	ite	le
	addle	r3, r1, r3
	addgt	r3, r1, r2
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	it	ge
	movge	r3, r2
	cmp	r6, #0
	sbcs	r2, r7, #0
	bge	.L69
	ldr	r2, .L127
	movs	r1, #0
	adds	r6, r6, r2
	adc	r7, r1, r7
.L69:
	lsrs	r5, r6, #23
	movw	r2, #19336
	ldr	r1, .L127+4
	orr	r5, r5, r7, lsl #9
	add	r2, r2, lr
	add	r5, r5, r3
	ubfx	r2, r2, #0, #15
	ssat	r5, #25, r5
	add	r2, r0, r2, lsl #2
	str	r5, [r0, #148]
	ldr	r6, [r2, #272]
	smull	r8, r9, r6, r1
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L70
	ldr	r1, .L127
	movs	r4, #0
	adds	r8, r8, r1
	adc	r9, r4, r9
.L70:
	lsr	r2, r8, #23
	mvn	r7, #-16777216
	movw	r1, #19102
	movw	r3, #19783
	orr	r2, r2, r9, lsl #9
	add	r1, r1, lr
	add	r3, r3, lr
	cmp	r2, #-16777216
	ubfx	r1, r1, #0, #15
	ubfx	r3, r3, #0, #15
	it	lt
	movlt	r2, #-16777216
	add	r4, r0, r1, lsl #2
	cmp	r2, r7
	ite	le
	addle	r2, r5, r2
	addgt	r2, r5, r7
	add	r1, r0, r3, lsl #2
	ldr	r3, .L127+8
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r7
	it	ge
	movge	r2, r7
	smull	r8, r9, r2, r3
	str	r2, [r4, #272]
	cmp	r8, #0
	ldr	r4, [r1, #272]
	sbcs	r3, r9, #0
	bge	.L71
	ldr	r1, .L127
	movs	r7, #0
	adds	r8, r8, r1
	adc	r9, r7, r9
.L71:
	cmp	r6, #-16777216
	lsr	r2, r8, #23
	mvn	r1, #-16777216
	ldr	r3, .L127+4
	it	lt
	movlt	r6, #-16777216
	orr	r2, r2, r9, lsl #9
	smull	r8, r9, r4, r3
	cmp	r6, r1
	ite	le
	addle	r6, r2, r6
	addgt	r6, r2, r1
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r1
	it	ge
	movge	r6, r1
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L72
	ldr	r3, .L127
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L72:
	lsr	r2, r8, #23
	mvn	r7, #-16777216
	movw	r1, #19337
	movw	r3, #20336
	orr	r2, r2, r9, lsl #9
	add	r1, r1, lr
	add	r3, r3, lr
	cmp	r2, #-16777216
	ubfx	r1, r1, #0, #15
	ubfx	r3, r3, #0, #15
	it	lt
	movlt	r2, #-16777216
	add	r3, r0, r3, lsl #2
	cmp	r2, r7
	ite	le
	addle	r2, r6, r2
	addgt	r2, r6, r7
	ldr	r6, .L127+8
	add	r1, r0, r1, lsl #2
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r7
	it	ge
	movge	r2, r7
	smull	r6, r7, r2, r6
	str	r2, [r1, #272]
	cmp	r6, #0
	ldr	ip, [r3, #272]
	sbcs	r3, r7, #0
	bge	.L73
	ldr	r3, .L127
	movs	r2, #0
	adds	r6, r6, r3
	adc	r7, r2, r7
.L73:
	cmp	r4, #-16777216
	lsr	r2, r6, #23
	mvn	r1, #-16777216
	ldr	r3, .L127+4
	it	lt
	movlt	r4, #-16777216
	orr	r2, r2, r7, lsl #9
	smull	r8, r9, ip, r3
	cmp	r4, r1
	ite	le
	addle	r4, r2, r4
	addgt	r4, r2, r1
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r1
	it	ge
	movge	r4, r1
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L74
	sub	r1, r1, #8388608
	movs	r6, #0
	adds	r8, r8, r1
	adc	r9, r6, r9
.L74:
	lsr	r2, r8, #23
	mvn	r7, #-16777216
	movw	r1, #19784
	ldr	r6, .L127+8
	orr	r2, r2, r9, lsl #9
	add	r1, r1, lr
	cmp	r2, #-16777216
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r2, #-16777216
	add	r1, r0, r1, lsl #2
	cmp	r2, r7
	ite	le
	addle	r2, r4, r2
	addgt	r2, r4, r7
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r7
	it	ge
	movge	r2, r7
	smull	r6, r7, r2, r6
	str	r2, [r1, #272]
	cmp	r6, #0
	sbcs	r3, r7, #0
	bge	.L75
	ldr	r3, .L127
	movs	r2, #0
	adds	r6, r6, r3
	adc	r7, r2, r7
.L75:
	cmp	ip, #-16777216
	lsr	r3, r6, #23
	mvn	r1, #-16777216
	movw	r2, #20538
	it	lt
	movlt	ip, #-16777216
	orr	r3, r3, r7, lsl #9
	add	r2, r2, lr
	ldr	r6, .L127+4
	cmp	ip, r1
	ite	le
	addle	r3, r3, ip
	addgt	r3, r3, r1
	ubfx	r2, r2, #0, #15
	cmp	r3, #-16777216
	add	r2, r0, r2, lsl #2
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r1
	it	ge
	movge	r3, r1
	str	r3, [r0, #104]
	ldr	r1, [r2, #272]
	smull	r6, r7, r1, r6
	cmp	r6, #0
	sbcs	r3, r7, #0
	bge	.L76
	ldr	r3, .L127
	movs	r2, #0
	adds	r6, r6, r3
	adc	r7, r2, r7
.L76:
	lsrs	r3, r6, #23
	mvn	r4, #-16777216
	movw	r6, #20337
	movw	r2, #20928
	orr	r3, r3, r7, lsl #9
	add	r6, r6, lr
	add	r2, r2, lr
	cmp	r3, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r3, r4
	ite	le
	addle	r3, r5, r3
	addgt	r3, r5, r4
	ubfx	r5, r6, #0, #15
	ldr	r6, .L127+8
	cmp	r3, #-16777216
	add	r5, r0, r5, lsl #2
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r4
	it	ge
	movge	r3, r4
	smull	r6, r7, r3, r6
	str	r3, [r5, #272]
	cmp	r6, #0
	ldr	r2, [r2, #272]
	sbcs	r3, r7, #0
	bge	.L77
	ldr	r3, .L127
	movs	r4, #0
	adds	r6, r6, r3
	adc	r7, r4, r7
.L77:
	cmp	r1, #-16777216
	lsr	r4, r6, #23
	mvn	r3, #-16777216
	ldr	r6, .L127+4
	it	lt
	movlt	r1, #-16777216
	orr	r4, r4, r7, lsl #9
	smull	r6, r7, r2, r6
	cmp	r1, r3
	ite	le
	addle	r4, r4, r1
	addgt	r4, r4, r3
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r3
	it	ge
	movge	r4, r3
	cmp	r6, #0
	sbcs	r3, r7, #0
	bge	.L78
	ldr	r3, .L127
	movs	r1, #0
	adds	r6, r6, r3
	adc	r7, r1, r7
.L78:
	lsrs	r1, r6, #23
	mvn	r3, #-16777216
	movw	r6, #20539
	movw	r5, #21556
	orr	r1, r1, r7, lsl #9
	add	r6, r6, lr
	add	r5, r5, lr
	cmp	r1, #-16777216
	ubfx	r6, r6, #0, #15
	ubfx	r5, r5, #0, #15
	it	lt
	movlt	r1, #-16777216
	add	r7, r0, r6, lsl #2
	cmp	r1, r3
	ite	le
	addle	r1, r4, r1
	addgt	r1, r4, r3
	ldr	r4, .L127+8
	add	r6, r0, r5, lsl #2
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r3
	it	ge
	movge	r1, r3
	smull	r4, r5, r1, r4
	str	r1, [r7, #272]
	cmp	r4, #0
	ldr	r6, [r6, #272]
	sbcs	r3, r5, #0
	bge	.L79
	b	.L128
.L129:
	.align	2
.L127:
	.word	8388607
	.word	5029888
	.word	-5038080
.L128:
	ldr	r3, .L130
	movs	r1, #0
	adds	r4, r4, r3
	adc	r5, r1, r5
.L79:
	cmp	r2, #-16777216
	lsr	r3, r4, #23
	mvn	r1, #-16777216
	ldr	r4, .L130+4
	orr	r3, r3, r5, lsl #9
	it	lt
	movlt	r2, #-16777216
	smull	r4, r5, r6, r4
	cmp	r2, r1
	ite	le
	addle	r2, r3, r2
	addgt	r2, r3, r1
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r1
	it	ge
	movge	r2, r1
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L80
	ldr	r3, .L130
	movs	r1, #0
	adds	r4, r4, r3
	adc	r5, r1, r5
.L80:
	lsrs	r3, r4, #23
	mvn	r7, #-16777216
	movw	r1, #20929
	ldr	r4, .L130+8
	orr	r3, r3, r5, lsl #9
	add	r1, r1, lr
	cmp	r3, #-16777216
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r3, #-16777216
	add	r1, r0, r1, lsl #2
	cmp	r3, r7
	ite	le
	addle	r3, r2, r3
	addgt	r3, r2, r7
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r7
	it	ge
	movge	r3, r7
	smull	r4, r5, r3, r4
	str	r3, [r1, #272]
	cmp	r4, #0
	sbcs	r3, r5, #0
	bge	.L81
	ldr	r3, .L130
	movs	r2, #0
	adds	r4, r4, r3
	adc	r5, r2, r5
.L81:
	cmp	r6, #-16777216
	lsr	r3, r4, #23
	mvn	r1, #-16777216
	movw	ip, #6601
	it	lt
	movlt	r6, #-16777216
	orr	r3, r3, r5, lsl #9
	add	ip, ip, lr
	ldr	r2, .L130+12
	cmp	r6, r1
	ite	le
	addle	r3, r3, r6
	addgt	r3, r3, r1
	ldr	r5, [r0, #152]
	ubfx	ip, ip, #0, #15
	cmp	r3, #-16777216
	add	ip, r0, ip, lsl #2
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r1
	it	ge
	movge	r3, r1
	str	r3, [r0, #108]
	ldr	r4, [ip, #272]
	smull	r7, r8, r4, r2
	cmp	r7, #0
	sbcs	r3, r8, #0
	bge	.L82
	add	r1, r1, r2
	movs	r6, #0
	adds	r7, r7, r1
	adc	r8, r6, r8
.L82:
	lsrs	r1, r7, #23
	mvn	r2, #-16777216
	orr	r1, r1, r8, lsl #9
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r2
	ite	le
	rsble	r1, r5, r1
	rsbgt	r1, r5, r2
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r2
	it	ge
	movge	r1, r2
	asrs	r7, r1, #31
	lsl	r8, r1, #22
	lsls	r6, r7, #22
	cmp	r8, #0
	orr	r9, r6, r1, lsr #10
	sbcs	r3, r9, #0
	bge	.L83
	ldr	r1, .L130
	movs	r6, #0
	adds	r8, r8, r1
	adc	r9, r6, r9
.L83:
	lsr	r2, r8, #23
	ldr	r7, .L130+12
	ldr	r1, [r0, #8]
	orr	r2, r2, r9, lsl #9
	ldr	r6, [r0, #88]
	add	r5, r5, r2
	ssat	r5, #25, r5
	smull	r8, r9, r5, r7
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L84
	ldr	r3, .L130
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L84:
	lsr	r2, r8, #23
	mvn	r3, #-16777216
	orr	r2, r2, r9, lsl #9
	cmp	r2, r3
	ite	le
	addle	r2, r1, r2
	addgt	r2, r1, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	smull	r9, r10, r2, r6
	cmp	r9, #0
	sbcs	r3, r10, #0
	bge	.L85
	ldr	r7, .L130
	mov	r8, #0
	adds	r9, r9, r7
	adc	r10, r10, r8
.L85:
	cmp	r4, #-16777216
	mvn	r2, #-16777216
	lsr	r8, r9, #23
	movw	r7, #8601
	it	lt
	movlt	r4, #-16777216
	ldr	r3, .L130+12
	orr	r8, r8, r10, lsl #9
	add	r7, r7, lr
	cmp	r4, r2
	ubfx	r7, r7, #0, #15
	it	ge
	movge	r4, r2
	cmp	r8, #-16777216
	add	r7, r0, r7, lsl #2
	it	lt
	movlt	r8, #-16777216
	cmp	r8, r2
	ite	le
	addle	r4, r4, r8
	addgt	r4, r4, r2
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r2
	it	ge
	movge	r4, r2
	str	r4, [ip, #272]
	ldr	r4, [r7, #272]
	ldr	ip, [r0, #156]
	smull	r8, r9, r4, r3
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L86
	ldr	r3, .L130
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L86:
	lsr	r2, r8, #23
	mvn	r3, #-16777216
	ldr	r8, .L130+20
	orr	r2, r2, r9, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ite	le
	rsble	r2, ip, r2
	rsbgt	r2, ip, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	smull	r10, fp, r2, r8
	cmp	r10, #0
	sbcs	r3, fp, #0
	bge	.L87
	ldr	r8, .L130
	mov	r9, #0
	adds	r10, r10, r8
	adc	fp, fp, r9
.L87:
	lsr	r2, r10, #23
	ldr	r8, .L130+12
	orr	r2, r2, fp, lsl #9
	add	r2, r2, ip
	ssat	r2, #25, r2
	smull	r8, r9, r2, r8
	str	r2, [r0, #156]
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L88
	ldr	r3, .L130
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L88:
	lsr	r2, r8, #23
	mvn	r3, #-16777216
	orr	r2, r2, r9, lsl #9
	cmp	r2, r3
	ite	le
	addle	r2, r1, r2
	addgt	r2, r1, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	smull	r9, r10, r2, r6
	cmp	r9, #0
	sbcs	r3, r10, #0
	bge	.L89
	ldr	ip, .L130
	mov	r8, #0
	adds	r9, r9, ip
	adc	r10, r10, r8
.L89:
	cmp	r4, #-16777216
	mvn	r2, #-16777216
	lsr	r8, r9, #23
	movw	ip, #11101
	it	lt
	movlt	r4, #-16777216
	ldr	r3, .L130+12
	orr	r8, r8, r10, lsl #9
	add	ip, ip, lr
	cmp	r4, r2
	ubfx	ip, ip, #0, #15
	it	ge
	movge	r4, r2
	cmp	r8, #-16777216
	add	ip, r0, ip, lsl #2
	it	lt
	movlt	r8, #-16777216
	cmp	r8, r2
	ite	le
	addle	r4, r4, r8
	addgt	r4, r4, r2
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r2
	it	ge
	movge	r4, r2
	str	r4, [r7, #272]
	ldr	r4, [ip, #272]
	smull	r8, r9, r4, r3
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L90
	ldr	r3, .L130
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L90:
	lsr	r2, r8, #23
	mvn	r3, #-16777216
	ldr	r7, .L130+16
	orr	r2, r2, r9, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	b	.L131
.L132:
	.align	2
.L130:
	.word	8388607
	.word	5029888
	.word	-5038080
	.word	-8388608
	.word	2516480
	.word	3355392
.L131:
	cmp	r2, r3
	ite	le
	rsble	r2, r5, r2
	rsbgt	r2, r5, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	smull	r9, r10, r2, r7
	cmp	r9, #0
	sbcs	r3, r10, #0
	bge	.L91
	ldr	r7, .L133
	mov	r8, #0
	adds	r9, r9, r7
	adc	r10, r10, r8
.L91:
	lsr	r2, r9, #23
	ldr	r7, .L133+4
	orr	r2, r2, r10, lsl #9
	add	r5, r5, r2
	ssat	r5, #25, r5
	smull	r8, r9, r5, r7
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L92
	ldr	r3, .L133
	movs	r2, #0
	adds	r8, r8, r3
	adc	r9, r9, r2
.L92:
	lsr	r2, r8, #23
	mvn	r3, #-16777216
	orr	r2, r2, r9, lsl #9
	cmp	r2, r3
	ite	le
	addle	r2, r1, r2
	addgt	r2, r1, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	smull	r2, r3, r2, r6
	cmp	r2, #0
	sbcs	r7, r3, #0
	bge	.L93
	ldr	r7, .L133
	mov	r8, #0
	adds	r2, r2, r7
	adc	r3, r3, r8
.L93:
	cmp	r4, #-16777216
	mvn	r7, #-16777216
	lsr	r8, r2, #23
	movw	r2, #14101
	it	lt
	movlt	r4, #-16777216
	orr	r8, r8, r3, lsl #9
	add	r2, r2, lr
	cmp	r4, r7
	ldr	r3, .L133+4
	ubfx	lr, r2, #0, #15
	it	ge
	movge	r4, r7
	cmp	r8, #-16777216
	add	lr, r0, lr, lsl #2
	it	lt
	movlt	r8, #-16777216
	cmp	r8, r7
	ite	le
	addle	r4, r4, r8
	addgt	r4, r4, r7
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r7
	it	ge
	movge	r4, r7
	str	r4, [ip, #272]
	ldr	r2, [lr, #272]
	cmp	r2, #-16777216
	mov	r4, r2
	smull	r8, r9, r2, r3
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r7
	it	ge
	movge	r4, r7
	cmp	r8, #0
	sbcs	r3, r9, #0
	str	r4, [r0, #12]
	bge	.L94
	sub	r7, r7, #8388608
	mov	ip, #0
	adds	r8, r8, r7
	adc	r9, r9, ip
.L94:
	lsr	r2, r8, #23
	mvn	r7, #-16777216
	ldr	r3, .L133+8
	orr	r2, r2, r9, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r7
	ite	le
	rsble	r2, r5, r2
	rsbgt	r2, r5, r7
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r7
	it	ge
	movge	r2, r7
	smull	r8, r9, r2, r3
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L95
	sub	r7, r7, #8388608
	mov	ip, #0
	adds	r8, r8, r7
	adc	r9, r9, ip
.L95:
	lsr	r7, r8, #23
	ldr	r2, .L133+4
	orr	r7, r7, r9, lsl #9
	add	r5, r5, r7
	ssat	r5, #25, r5
	smull	r8, r9, r5, r2
	str	r5, [r0, #152]
	cmp	r8, #0
	sbcs	r3, r9, #0
	bge	.L96
	ldr	r5, .L133
	movs	r7, #0
	adds	r8, r8, r5
	adc	r9, r7, r9
.L96:
	lsr	r2, r8, #23
	mvn	r5, #-16777216
	orr	r2, r2, r9, lsl #9
	cmp	r2, r5
	ite	le
	addle	r1, r1, r2
	addgt	r1, r1, r5
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r5
	it	ge
	movge	r1, r5
	smull	r6, r7, r1, r6
	cmp	r6, #0
	sbcs	r3, r7, #0
	bge	.L97
	ldr	r3, .L133
	movs	r2, #0
	adds	r6, r6, r3
	adc	r7, r2, r7
.L97:
	lsrs	r3, r6, #23
	mvn	r2, #-16777216
	movs	r1, #0
	orr	r3, r3, r7, lsl #9
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r2
	ite	le
	addle	r4, r4, r3
	addgt	r4, r4, r2
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r2
	it	ge
	movge	r4, r2
	str	r4, [lr, #272]
	str	r1, [r0, #4]
	add	sp, sp, #28
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L2:
	ldr	r4, .L133+12
	movs	r5, #0
	movs	r3, #0
	ldr	r1, .L133+16
	ldr	ip, [r0]
	mov	fp, #1
	str	r3, [r0, #36]
	mov	r2, r3
	mov	r9, r3
	str	r3, [r0, r1]
	strd	r4, [sp, #16]
	movs	r4, #0
	movs	r5, #0
	strd	r4, [sp, #8]
	b	.L3
.L115:
	ldr	r3, .L133
	movs	r1, #0
	adds	r4, r4, r3
	adc	r5, r1, r5
	b	.L10
.L101:
	add	r3, ip, #37
	str	r3, [r0]
	b	.L11
.L102:
	add	r3, ip, #36
	str	r3, [r0]
	b	.L14
.L103:
	add	r3, ip, #35
	str	r3, [r0]
	b	.L16
.L104:
	add	r2, ip, #34
	str	r2, [r0]
	b	.L18
.L105:
	add	r2, ip, #33
	str	r2, [r0]
	b	.L20
.L106:
	add	r2, ip, #32
	str	r2, [r0]
	b	.L22
.L107:
	add	r3, ip, #31
	str	r3, [r0]
	b	.L24
.L108:
	add	r3, ip, #30
	str	r3, [r0]
	b	.L26
.L114:
	add	r3, ip, #24
	str	r3, [r0]
	b	.L38
.L113:
	add	r3, ip, #25
	str	r3, [r0]
	b	.L36
.L112:
	add	r3, ip, #26
	str	r3, [r0]
	b	.L34
.L111:
	add	r2, ip, #27
	str	r2, [r0]
	b	.L32
.L110:
	add	r3, ip, #28
	str	r3, [r0]
	b	.L30
.L109:
	add	r3, ip, #29
	str	r3, [r0]
	b	.L28
.L134:
	.align	2
.L133:
	.word	8388607
	.word	-8388608
	.word	1677568
	.word	4194303
	.word	131380
	.size	OEM1_4_spn, .-OEM1_4_spn
	.align	1
	.p2align 2,,3
	.global	dance_ir_h_l_spn
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-d16
	.type	dance_ir_h_l_spn, %function
dance_ir_h_l_spn:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	ldr	r2, .L208
	sub	sp, sp, #36
	ldr	r4, [r0, #80]
	mov	r9, r0
	smull	r6, r7, r4, r2
	cmp	r6, #0
	sbcs	r3, r7, #0
	bge	.L136
	ldr	r1, .L208+4
	movs	r0, #0
	adds	r6, r6, r1
	adc	r7, r0, r7
.L136:
	lsrs	r2, r6, #23
	mvn	r5, #-16777216
	ldr	r3, [r9, #4]
	orr	r2, r2, r7, lsl #9
	ldr	r0, .L208+8
	cmp	r2, #-16777216
	smull	r6, r7, r4, r0
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r5
	ite	le
	addle	r3, r3, r2
	addgt	r3, r3, r5
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r5
	it	ge
	movge	r3, r5
	cmp	r6, #0
	str	r3, [r9, #152]
	sbcs	r3, r7, #0
	bge	.L137
	ldr	r3, .L208+4
	movs	r2, #0
	adds	r6, r6, r3
	adc	r7, r2, r7
.L137:
	lsrs	r2, r6, #23
	mvn	r0, #-16777216
	ldr	r3, .L208+12
	orr	r2, r2, r7, lsl #9
	ldr	r1, .L208
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r0
	ite	le
	addle	r2, r3, r2
	addgt	r2, r3, r0
	cmp	r2, r0
	it	ge
	movge	r2, r0
	smull	r5, r6, r2, r1
	cmp	r5, #0
	sbcs	r3, r6, #0
	bge	.L138
	ldr	r1, .L208+4
	movs	r0, #0
	adds	r5, r5, r1
	adc	r6, r0, r6
.L138:
	cmp	r4, #-16777216
	mov	r1, r4
	mvn	r0, #-16777216
	lsr	r2, r5, #23
	it	lt
	movlt	r1, #-16777216
	orr	r2, r2, r6, lsl #9
	cmp	r1, r0
	it	ge
	movge	r1, r0
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	sub	r3, r1, #4194304
	str	r1, [r9, #144]
	cmp	r2, r0
	it	ge
	movge	r2, r0
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r4, #4194304
	str	r2, [r9, #148]
	str	r3, [r9, #4]
	bgt	.L206
	mov	r3, #4194304
	str	r3, [r9, #144]
.L140:
	ldr	r1, [r9, #84]
	ssat	r1, #25, r1
	asrs	r5, r1, #31
	lsls	r6, r1, #22
	lsls	r0, r5, #22
	cmp	r6, #0
	orr	r7, r0, r1, lsr #10
	sbcs	r3, r7, #0
	bge	.L141
	ldr	r1, .L208+4
	movs	r0, #0
	adds	r6, r6, r1
	adc	r7, r0, r7
.L141:
	lsrs	r2, r6, #23
	orr	r2, r2, r7, lsl #9
	sub	r2, r2, #4194304
	vmov	s15, r2	@ int
	str	r2, [r9, #4]
	vcvt.f32.s32	s15, s15
	vcmpe.f32	s15, #0
	vmrs	APSR_nzcv, FPSCR
	blt	.L207
	ldr	ip, .L208+4
.L142:
	ldr	r3, [r9, #88]
	str	ip, [r9, #188]
	ssat	r2, #25, r3
	asr	fp, r2, #31
	mov	r10, r2
	lsls	r4, r2, #22
	lsl	r6, fp, #22
	cmp	r4, #0
	orr	r5, r6, r2, lsr #10
	sbcs	r2, r5, #0
	bge	.L143
	ldr	r2, .L208+4
	movs	r6, #0
	adds	r4, r4, r2
	adc	r5, r6, r5
.L143:
	lsrs	r2, r4, #23
	orr	r2, r2, r5, lsl #9
	sub	r2, r2, #4194304
	vmov	s15, r2	@ int
	str	r2, [r9, #4]
	vcvt.f32.s32	s15, s15
	vcmpe.f32	s15, #0
	vmrs	APSR_nzcv, FPSCR
	blt	.L204
	ldr	r2, .L208+4
	movs	r1, #0
	ldr	r0, .L208+4
	str	r2, [r9, #4]
	strd	r0, [sp, #8]
.L146:
	ldr	r4, [r9, #84]
	ldr	r5, .L208+8
	str	r2, [r9, #192]
	smull	r0, r1, r4, r5
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L147
	ldr	r2, .L208+4
	movs	r6, #0
	adds	r0, r0, r2
	adc	r1, r6, r1
.L147:
	lsrs	r4, r0, #23
	mvn	r6, #-16777216
	ldr	r2, .L208+12
	orr	r4, r4, r1, lsl #9
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r6
	ite	le
	addle	r2, r2, r4
	addgt	r2, r2, r6
	cmp	r2, r6
	it	ge
	movge	r2, r6
	smull	r6, r7, r2, r2
	cmp	r6, #0
	sbcs	r1, r7, #0
	bge	.L148
	ldr	r4, .L208+4
	movs	r5, #0
	adds	r6, r6, r4
	adc	r7, r5, r7
.L148:
	lsrs	r4, r6, #23
	orr	r4, r4, r7, lsl #9
	ssat	r4, #25, r4
	smull	r0, r1, r4, r2
	cmp	r0, #0
	sbcs	r2, r1, #0
	strd	r0, [sp]
	bge	.L149
	ldr	r2, .L208+4
	movs	r6, #0
	adds	r2, r0, r2
	str	r2, [sp]
	ldr	r2, [sp, #4]
	adc	r2, r6, r2
	str	r2, [sp, #4]
.L149:
	ldr	r2, [sp]
	mul	r1, r3, fp
	ldr	r5, [sp, #4]
	asrs	r7, r3, #31
	lsrs	r2, r2, #23
	mla	r4, r10, r7, r1
	orr	r2, r2, r5, lsl #9
	umull	r0, r1, r3, r10
	ssat	r2, #25, r2
	add	r1, r1, r4
	cmp	r0, #0
	mov	r5, r0
	str	r2, [sp, #28]
	mov	r6, r1
	str	r2, [r9, #216]
	sbcs	r2, r1, #0
	bge	.L150
	ldr	r2, .L208+4
	movs	r4, #0
	adds	r5, r0, r2
	adc	r6, r4, r6
.L150:
	lsrs	r0, r5, #23
	orr	r0, r0, r6, lsl #9
	ssat	r0, #25, r0
	smull	r0, r1, r0, r3
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L151
	ldr	r2, .L208+4
	movs	r4, #0
	adds	r0, r0, r2
	adc	r1, r4, r1
.L151:
	lsrs	r2, r0, #23
	orr	r2, r2, r1, lsl #9
	ssat	r2, #25, r2
	smull	r2, r3, r2, r3
	cmp	r2, #0
	sbcs	r1, r3, #0
	bge	.L152
	ldr	r1, .L208+4
	movs	r0, #0
	adds	r2, r2, r1
	adc	r3, r0, r3
.L152:
	ldr	r8, [r9, #100]
	lsrs	r1, r2, #23
	ldr	r0, .L208+16
	lsl	r4, r8, #21
	orr	r3, r1, r3, lsl #9
	mov	r6, r4
	ldr	r4, [r9, r0]
	mov	r0, r8
	mov	r2, r3
	add	r5, r4, #202
	str	r4, [sp]
	asrs	r1, r0, #31
	ssat	r2, #25, r2
	cmp	r6, #0
	ldr	r3, [r9, #96]
	lsl	r4, r1, #21
	str	r2, [r9, #220]
	str	r2, [sp, #24]
	ubfx	r1, r5, #0, #15
	orr	r2, r4, r8, lsr #11
	add	r1, r9, r1, lsl #2
	mov	r7, r2
	ldr	lr, [r1, #272]
	sbcs	r2, r7, #0
	bge	.L153
	ldr	r1, .L208+4
	movs	r0, #0
	adds	r2, r6, r1
	mov	r6, r2
	mov	r2, r7
	adc	r2, r0, r2
	mov	r7, r2
.L153:
	asr	fp, r3, #31
	lsls	r2, r3, #21
	lsl	r1, fp, #21
	str	r2, [sp, #16]
	lsrs	r6, r6, #23
	orr	r2, r1, r3, lsr #11
	orr	r6, r6, r7, lsl #9
	str	r2, [sp, #20]
	ssat	r6, #25, r6
	ldrd	r4, [sp, #16]
	cmp	r4, #0
	sbcs	r2, r5, #0
	bge	.L154
	ldr	r1, .L208+4
	movs	r0, #0
	adds	r2, r4, r1
	str	r2, [sp, #16]
	ldr	r2, [sp, #20]
	adc	r2, r0, r2
	str	r2, [sp, #20]
	b	.L209
.L210:
	.align	2
.L208:
	.word	16768768
	.word	8388607
	.word	-8388608
	.word	8380160
	.word	131344
.L209:
.L154:
	mvn	r4, #-16777216
	ldr	r1, [r9, #148]
	ldrd	r10, [sp, #16]
	mov	r2, r10
	lsrs	r0, r2, #23
	orr	r0, r0, fp, lsl #9
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r4
	ite	le
	addle	r0, r6, r0
	addgt	r0, r6, r4
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r4
	it	ge
	movge	r0, r4
	smull	r6, r7, r0, r1
	cmp	r6, #0
	sbcs	r2, r7, #0
	bge	.L155
	sub	r4, r4, #8388608
	movs	r5, #0
	adds	r6, r6, r4
	adc	r7, r5, r7
.L155:
	ldr	r0, .L211
	lsrs	r4, r6, #23
	smull	r0, r1, lr, r0
	orr	r6, r4, r7, lsl #9
	cmp	r0, #0
	ssat	r6, #25, r6
	sbcs	r2, r1, #0
	bge	.L156
	ldr	r4, .L211+4
	movs	r5, #0
	adds	r0, r0, r4
	adc	r1, r5, r1
.L156:
	lsrs	r4, r0, #23
	mvn	r7, #-16777216
	ldr	r2, [sp]
	orr	r4, r4, r1, lsl #9
	ldr	r0, .L211+8
	add	r1, r2, #744
	ubfx	r5, r2, #0, #15
	cmp	r4, #-16777216
	ubfx	r1, r1, #0, #15
	add	r5, r9, r5, lsl #2
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r7
	ite	le
	addle	r4, r6, r4
	addgt	r4, r6, r7
	add	r6, r9, r1, lsl #2
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r7
	it	ge
	movge	r4, r7
	smull	r0, r1, r4, r0
	str	r4, [r5, #272]
	cmp	r0, #0
	ldr	r4, [r6, #272]
	sbcs	r2, r1, #0
	bge	.L157
	ldr	r5, .L211+4
	movs	r6, #0
	adds	r0, r0, r5
	adc	r1, r6, r1
.L157:
	cmp	lr, #-16777216
	lsr	r5, r0, #23
	mvn	r6, #-16777216
	ldr	r0, .L211
	it	lt
	movlt	lr, #-16777216
	orr	r5, r5, r1, lsl #9
	smull	r0, r1, r4, r0
	cmp	lr, r6
	ite	le
	addle	lr, r5, lr
	addgt	lr, r5, r6
	cmp	lr, #-16777216
	it	lt
	movlt	lr, #-16777216
	cmp	lr, r6
	it	ge
	movge	lr, r6
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L158
	ldr	r5, .L211+4
	movs	r6, #0
	adds	r0, r0, r5
	adc	r1, r6, r1
.L158:
	lsrs	r5, r0, #23
	mvn	r6, #-16777216
	ldr	r2, [sp]
	orr	r5, r5, r1, lsl #9
	add	r0, r2, #203
	addw	r1, r2, #1902
	cmp	r5, #-16777216
	ubfx	r0, r0, #0, #15
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r5, #-16777216
	add	r7, r9, r0, lsl #2
	ldr	r0, .L211+8
	cmp	r5, r6
	ite	le
	addle	r5, lr, r5
	addgt	r5, lr, r6
	add	lr, r9, r1, lsl #2
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r6
	it	ge
	movge	r5, r6
	smull	r0, r1, r5, r0
	str	r5, [r7, #272]
	cmp	r0, #0
	ldr	r5, [lr, #272]
	sbcs	r2, r1, #0
	bge	.L159
	sub	r6, r6, #8388608
	movs	r7, #0
	adds	r0, r0, r6
	adc	r1, r7, r1
.L159:
	cmp	r4, #-16777216
	lsr	r7, r0, #23
	mvn	r6, #-16777216
	ldr	r0, .L211
	it	lt
	movlt	r4, #-16777216
	orr	r7, r7, r1, lsl #9
	smull	r10, fp, r5, r0
	cmp	r4, r6
	ite	le
	addle	r4, r7, r4
	addgt	r4, r7, r6
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r6
	it	ge
	movge	r4, r6
	cmp	r10, #0
	sbcs	r2, fp, #0
	bge	.L160
	sub	r6, r6, #8388608
	movs	r7, #0
	adds	r10, r10, r6
	adc	fp, r7, fp
.L160:
	lsr	r0, r10, #23
	mvn	r6, #-16777216
	ldr	r2, [sp]
	orr	r0, r0, fp, lsl #9
	addw	r1, r2, #3806
	addw	r7, r2, #745
	cmp	r0, #-16777216
	ubfx	r1, r1, #0, #15
	ubfx	r7, r7, #0, #15
	it	lt
	movlt	r0, #-16777216
	add	lr, r9, r1, lsl #2
	cmp	r0, r6
	ite	le
	addle	r4, r4, r0
	addgt	r4, r4, r6
	ldr	r0, .L211+8
	add	r7, r9, r7, lsl #2
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r6
	it	ge
	movge	r4, r6
	smull	r0, r1, r4, r0
	str	r4, [r7, #272]
	cmp	r0, #0
	ldr	r4, [lr, #272]
	sbcs	r2, r1, #0
	bge	.L161
	sub	r6, r6, #8388608
	movs	r7, #0
	adds	r0, r0, r6
	adc	r1, r7, r1
.L161:
	cmp	r5, #-16777216
	lsr	r6, r0, #23
	mvn	r7, #-16777216
	ldr	r0, .L211
	it	lt
	movlt	r5, #-16777216
	orr	r6, r6, r1, lsl #9
	smull	r0, r1, r4, r0
	cmp	r5, r7
	ite	le
	addle	r6, r6, r5
	addgt	r6, r6, r7
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r7
	it	ge
	movge	r6, r7
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L162
	ldr	r5, .L211+4
	movs	r7, #0
	adds	r0, r0, r5
	adc	r1, r7, r1
.L162:
	lsrs	r5, r0, #23
	mvn	r7, #-16777216
	ldr	r2, [sp]
	orr	r5, r5, r1, lsl #9
	ldr	r0, .L211+8
	addw	r1, r2, #1903
	cmp	r5, #-16777216
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r7
	ite	le
	addle	r5, r6, r5
	addgt	r5, r6, r7
	add	r6, r9, r1, lsl #2
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r7
	it	ge
	movge	r5, r7
	smull	r10, fp, r5, r0
	str	r5, [r6, #272]
	cmp	r10, #0
	sbcs	r2, fp, #0
	bge	.L163
	ldr	r5, .L211+4
	movs	r6, #0
	adds	r10, r10, r5
	adc	fp, r6, fp
.L163:
	cmp	r4, #-16777216
	lsr	r5, r10, #23
	mvn	r6, #-16777216
	ldr	r2, [sp]
	orr	r5, r5, fp, lsl #9
	it	lt
	movlt	r4, #-16777216
	movw	r0, #26831
	movw	r1, #6011
	cmp	r4, r6
	ite	le
	addle	r4, r5, r4
	addgt	r4, r5, r6
	ldr	fp, [r9, #144]
	add	r0, r0, r2
	add	r1, r1, r2
	cmp	r4, #-16777216
	ubfx	r0, r0, #0, #15
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r4, #-16777216
	add	r0, r9, r0, lsl #2
	cmp	r4, r6
	add	r1, r9, r1, lsl #2
	it	ge
	movge	r4, r6
	str	r4, [r9, #196]
	ldr	r0, [r0, #272]
	ldr	r5, [r1, #272]
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r6
	it	ge
	movge	r0, r6
	smull	r0, r1, r0, fp
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L164
	sub	r6, r6, #8388608
	movs	r7, #0
	adds	r0, r0, r6
	adc	r1, r7, r1
.L164:
	lsrs	r6, r0, #23
	mvn	lr, #-16777216
	ldr	r0, .L211
	orr	r6, r6, r1, lsl #9
	smull	r0, r1, r5, r0
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, lr
	ite	le
	addle	r7, r4, r6
	addgt	r7, r4, lr
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r7, lr
	it	ge
	movge	r7, lr
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L165
	ldr	r6, .L211+4
	mov	lr, #0
	adds	r0, r0, r6
	adc	r1, r1, lr
.L165:
	lsrs	r6, r0, #23
	mvn	lr, #-16777216
	ldr	r2, [sp]
	orr	r6, r6, r1, lsl #9
	movw	r1, #9313
	ldr	r0, .L211+8
	addw	r10, r2, #3807
	cmp	r6, #-16777216
	add	r1, r1, r2
	ubfx	r10, r10, #0, #15
	b	.L212
.L213:
	.align	2
.L211:
	.word	5029888
	.word	8388607
	.word	-5038080
.L212:
	it	lt
	movlt	r6, #-16777216
	ubfx	r1, r1, #0, #15
	add	r10, r9, r10, lsl #2
	cmp	r6, lr
	ite	le
	addle	r6, r7, r6
	addgt	r6, r7, lr
	add	r7, r9, r1, lsl #2
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, lr
	it	ge
	movge	r6, lr
	smull	r0, r1, r6, r0
	str	r6, [r10, #272]
	cmp	r0, #0
	ldr	r6, [r7, #272]
	sbcs	r2, r1, #0
	bge	.L166
	ldr	r7, .L214
	mov	lr, #0
	adds	r0, r0, r7
	adc	r1, r1, lr
.L166:
	cmp	r5, #-16777216
	lsr	r7, r0, #23
	mvn	lr, #-16777216
	ldr	r0, .L214+4
	it	lt
	movlt	r5, #-16777216
	orr	r7, r7, r1, lsl #9
	smull	r0, r1, r6, r0
	cmp	r5, lr
	ite	le
	addle	r5, r7, r5
	addgt	r5, r7, lr
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, lr
	it	ge
	movge	r5, lr
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L167
	ldr	r7, .L214
	mov	lr, #0
	adds	r0, r0, r7
	adc	r1, r1, lr
.L167:
	lsrs	r7, r0, #23
	mvn	r10, #-16777216
	ldr	r2, [sp]
	movw	lr, #6012
	orr	r7, r7, r1, lsl #9
	movw	r1, #9314
	ldr	r0, .L214+8
	add	lr, lr, r2
	cmp	r7, #-16777216
	add	r1, r1, r2
	ubfx	lr, lr, #0, #15
	it	lt
	movlt	r7, #-16777216
	add	lr, r9, lr, lsl #2
	cmp	r7, r10
	ite	le
	addle	r7, r5, r7
	addgt	r7, r5, r10
	ubfx	r5, r1, #0, #15
	cmp	r7, #-16777216
	add	r5, r9, r5, lsl #2
	it	lt
	movlt	r7, #-16777216
	cmp	r7, r10
	it	ge
	movge	r7, r10
	smull	r0, r1, r7, r0
	str	r7, [lr, #272]
	cmp	r0, #0
	sbcs	r2, r1, #0
	strd	r0, [sp, #16]
	bge	.L168
	ldr	r7, .L214
	mov	lr, #0
	adds	r2, r0, r7
	str	r2, [sp, #16]
	ldr	r2, [sp, #20]
	adc	r2, r2, lr
	str	r2, [sp, #20]
.L168:
	ldr	r2, [sp, #16]
	cmp	r6, #-16777216
	mvn	r7, #-16777216
	movw	lr, #13770
	lsr	r0, r2, #23
	ldr	r2, [sp, #20]
	it	lt
	movlt	r6, #-16777216
	movw	r1, #17303
	orr	r0, r0, r2, lsl #9
	ldr	r2, [sp]
	cmp	r6, r7
	ite	le
	addle	r6, r0, r6
	addgt	r6, r0, r7
	add	lr, lr, r2
	add	r1, r1, r2
	cmp	r6, #-16777216
	ubfx	lr, lr, #0, #15
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r6, #-16777216
	add	lr, r9, lr, lsl #2
	add	r1, r9, r1, lsl #2
	cmp	r6, r7
	it	ge
	movge	r6, r7
	str	r6, [r5, #272]
	ldr	r0, [lr, #272]
	ldr	r6, [r1, #272]
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r7
	it	ge
	movge	r0, r7
	smull	r0, r1, r0, fp
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L169
	sub	r7, r7, #8388608
	mov	lr, #0
	adds	r0, r0, r7
	adc	r1, r1, lr
.L169:
	lsrs	r7, r0, #23
	mvn	lr, #-16777216
	ldr	r0, .L214+4
	orr	r7, r7, r1, lsl #9
	smull	r0, r1, r6, r0
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r7, lr
	ite	le
	addle	r7, r4, r7
	addgt	r7, r4, lr
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r7, lr
	it	ge
	movge	r7, lr
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L170
	ldr	r4, .L214
	mov	lr, #0
	adds	r0, r0, r4
	adc	r1, r1, lr
.L170:
	lsrs	r4, r0, #23
	mvn	lr, #-16777216
	ldr	r2, [sp]
	movw	r10, #13771
	orr	r4, r4, r1, lsl #9
	movw	r1, #20505
	ldr	r0, .L214+8
	add	r10, r10, r2
	cmp	r4, #-16777216
	add	r1, r1, r2
	ubfx	r10, r10, #0, #15
	it	lt
	movlt	r4, #-16777216
	ubfx	r1, r1, #0, #15
	add	r10, r9, r10, lsl #2
	cmp	r4, lr
	ite	le
	addle	r4, r7, r4
	addgt	r4, r7, lr
	add	r7, r9, r1, lsl #2
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, lr
	it	ge
	movge	r4, lr
	smull	r0, r1, r4, r0
	str	r4, [r10, #272]
	cmp	r0, #0
	ldr	r4, [r7, #272]
	sbcs	r2, r1, #0
	bge	.L171
	ldr	r7, .L214
	mov	lr, #0
	adds	r0, r0, r7
	adc	r1, r1, lr
.L171:
	cmp	r6, #-16777216
	lsr	r7, r0, #23
	mvn	lr, #-16777216
	ldr	r0, .L214+4
	it	lt
	movlt	r6, #-16777216
	orr	r7, r7, r1, lsl #9
	smull	r0, r1, r4, r0
	cmp	r6, lr
	ite	le
	addle	r7, r7, r6
	addgt	r7, r7, lr
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r7, lr
	it	ge
	movge	r7, lr
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L172
	ldr	r6, .L214
	mov	lr, #0
	adds	r0, r0, r6
	adc	r1, r1, lr
.L172:
	lsrs	r6, r0, #23
	mvn	lr, #-16777216
	ldr	r2, [sp]
	orr	r6, r6, r1, lsl #9
	ldr	r0, .L214+8
	movw	r1, #17304
	cmp	r6, #-16777216
	add	r1, r1, r2
	it	lt
	movlt	r6, #-16777216
	ubfx	r1, r1, #0, #15
	cmp	r6, lr
	ite	le
	addle	r6, r7, r6
	addgt	r6, r7, lr
	add	r7, r9, r1, lsl #2
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, lr
	it	ge
	movge	r6, lr
	smull	r10, fp, r6, r0
	str	r6, [r7, #272]
	cmp	r10, #0
	sbcs	r2, fp, #0
	bge	.L173
	ldr	r6, .L214
	movs	r7, #0
	adds	r10, r10, r6
	adc	fp, r7, fp
.L173:
	cmp	r4, #-16777216
	lsr	r0, r10, #23
	mvn	r6, #-16777216
	ldr	r2, [sp]
	it	lt
	movlt	r4, #-16777216
	orr	r0, r0, fp, lsl #9
	movw	r1, #20506
	mov	r7, #12582912
	cmp	r4, r6
	ite	le
	addle	r4, r0, r4
	addgt	r4, r0, r6
	add	r1, r1, r2
	cmp	r4, #-16777216
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r4, #-16777216
	add	r1, r9, r1, lsl #2
	cmp	r4, r6
	it	ge
	movge	r4, r6
	str	r4, [r1, #272]
	ldr	r1, [r5, #272]
	ldr	r2, [r9, #80]
	smull	r10, fp, r1, r7
	cmp	r10, #0
	sbcs	r1, fp, #0
	bge	.L174
	ldr	r1, .L214
	movs	r0, #0
	adds	r10, r10, r1
	adc	fp, fp, r0
.L174:
	ldr	r6, .L214+12
	lsr	r1, r10, #23
	smull	r6, r7, r3, r6
	orr	r1, r1, fp, lsl #9
	cmp	r6, #0
	ssat	r1, #25, r1
	sbcs	r0, r7, #0
	bge	.L175
	ldr	r0, .L214
	movs	r5, #0
	adds	r6, r6, r0
	adc	r7, r5, r7
	b	.L215
.L216:
	.align	2
.L214:
	.word	8388607
	.word	5029888
	.word	-5038080
	.word	-8388608
.L215:
.L175:
	lsrs	r0, r6, #23
	mvn	r5, #-16777216
	orr	r0, r0, r7, lsl #9
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r5
	ite	le
	addle	r0, r1, r0
	addgt	r0, r1, r5
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r5
	it	ge
	movge	r0, r5
	smull	r10, fp, r0, r2
	cmp	r10, #0
	sbcs	r1, fp, #0
	bge	.L176
	sub	r5, r5, #8388608
	movs	r6, #0
	adds	r10, r10, r5
	adc	fp, r6, fp
.L176:
	cmp	r3, #-16777216
	mvn	r0, #-16777216
	lsr	r5, r10, #23
	mov	r6, #12582912
	it	lt
	movlt	r3, #-16777216
	str	r4, [r9, #12]
	orr	r5, r5, fp, lsl #9
	cmp	r3, r0
	mov	r1, r5
	it	ge
	movge	r3, r0
	cmp	r5, #-16777216
	smull	r4, r5, r4, r6
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r0
	ite	le
	addle	r3, r3, r1
	addgt	r3, r3, r0
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r0
	it	ge
	movge	r3, r0
	cmp	r4, #0
	sbcs	r1, r5, #0
	str	r3, [r9, #200]
	bge	.L177
	ldr	r1, .L217
	movs	r0, #0
	adds	r4, r4, r1
	adc	r5, r0, r5
.L177:
	ldr	r1, .L217+4
	lsrs	r0, r4, #23
	smull	r10, fp, r8, r1
	orr	r4, r0, r5, lsl #9
	cmp	r10, #0
	ssat	r4, #25, r4
	sbcs	r1, fp, #0
	bge	.L178
	ldr	r5, .L217
	movs	r6, #0
	adds	r10, r10, r5
	adc	fp, r6, fp
.L178:
	lsr	r0, r10, #23
	mvn	r5, #-16777216
	orr	r0, r0, fp, lsl #9
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r5
	ite	le
	addle	r0, r4, r0
	addgt	r0, r4, r5
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r5
	it	ge
	movge	r0, r5
	smull	r0, r1, r0, r2
	cmp	r0, #0
	sbcs	r2, r1, #0
	bge	.L179
	ldr	r2, .L217
	movs	r4, #0
	adds	r0, r0, r2
	adc	r1, r4, r1
.L179:
	cmp	r8, #-16777216
	mvn	r4, #-16777216
	ldr	r6, [r9, #156]
	lsr	r2, r0, #23
	it	lt
	movlt	r8, #-16777216
	orr	r2, r2, r1, lsl #9
	mov	lr, r6
	cmp	r8, r4
	it	ge
	movge	r8, r4
	cmp	r6, #-16777216
	it	lt
	movlt	lr, #-16777216
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	lr, r4
	it	ge
	movge	lr, r4
	cmp	r2, r4
	ite	le
	addle	r2, r8, r2
	addgt	r2, r8, r4
	cmp	r2, #-16777216
	smull	r7, r8, lr, ip
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r4
	it	ge
	movge	r2, r4
	cmp	r7, #0
	sbcs	r1, r8, #0
	str	r2, [r9, #204]
	bge	.L180
	sub	r4, r4, #8388608
	movs	r5, #0
	adds	r7, r7, r4
	adc	r8, r5, r8
.L180:
	ldr	r0, [r9, #160]
	lsrs	r5, r7, #23
	mvn	r1, #-16777216
	ldr	r4, .L217+8
	orr	r5, r5, r8, lsl #9
	cmp	r0, #-16777216
	smull	r10, fp, r6, r4
	it	lt
	movlt	r0, #-16777216
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r0, r1
	it	ge
	movge	r0, r1
	cmp	r5, r1
	ite	le
	addle	r5, r0, r5
	addgt	r5, r0, r1
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r1
	it	ge
	movge	r5, r1
	cmp	r10, #0
	sbcs	r1, fp, #0
	str	r5, [r9, #160]
	bge	.L181
	ldr	r1, .L217
	movs	r0, #0
	adds	r10, r10, r1
	adc	fp, fp, r0
.L181:
	ldr	r4, .L217+4
	lsr	r6, r10, #23
	smull	r4, r5, r5, r4
	orr	r6, r6, fp, lsl #9
	cmp	r4, #0
	ssat	r6, #25, r6
	sbcs	r1, r5, #0
	bge	.L182
	ldr	r1, .L217
	movs	r0, #0
	adds	r4, r4, r1
	adc	r5, r0, r5
.L182:
	lsrs	r0, r4, #23
	mvn	r1, #-16777216
	orr	r0, r0, r5, lsl #9
	cmp	r0, r1
	ite	le
	addle	r0, r6, r0
	addgt	r0, r6, r1
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r1
	ite	le
	addle	r6, r3, r0
	addgt	r6, r3, r1
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r1
	it	ge
	movge	r6, r1
	smull	r0, r1, r6, ip
	cmp	r0, #0
	sbcs	r4, r1, #0
	bge	.L183
	ldr	r4, .L217
	movs	r5, #0
	adds	r0, r0, r4
	adc	r1, r5, r1
.L183:
	lsrs	r5, r0, #23
	ldr	r0, [r9, #172]
	mvn	r4, #-16777216
	orr	r5, r5, r1, lsl #9
	mov	r7, r0
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r0, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r5, r4
	ite	le
	addle	lr, lr, r5
	addgt	lr, lr, r4
	cmp	r7, r4
	it	ge
	movge	r7, r4
	cmp	lr, #-16777216
	it	lt
	movlt	lr, #-16777216
	smull	r10, fp, r7, ip
	cmp	r4, lr
	it	ge
	movge	r4, lr
	cmp	r10, #0
	sbcs	r1, fp, #0
	str	r4, [r9, #156]
	bge	.L184
	ldr	r1, .L217
	movs	r4, #0
	adds	r10, r10, r1
	adc	fp, fp, r4
.L184:
	ldr	r1, [r9, #176]
	lsr	r4, r10, #23
	mvn	r5, #-16777216
	cmp	r1, #-16777216
	orr	r4, r4, fp, lsl #9
	it	lt
	movlt	r1, #-16777216
	cmp	r4, #-16777216
	mov	lr, r1
	it	lt
	movlt	r4, #-16777216
	ldr	r1, .L217+8
	cmp	lr, r5
	smull	r10, fp, r0, r1
	it	ge
	movge	lr, r5
	cmp	r4, r5
	ite	le
	addle	r4, lr, r4
	addgt	r4, lr, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	cmp	r10, #0
	sbcs	r1, fp, #0
	str	r4, [r9, #176]
	bge	.L185
	sub	r5, r5, #8388608
	mov	lr, #0
	adds	r10, r10, r5
	adc	fp, fp, lr
.L185:
	lsr	r5, r10, #23
	ldr	r0, .L217+4
	orr	r1, r5, fp, lsl #9
	smull	r10, fp, r4, r0
	cmp	r10, #0
	ssat	r1, #25, r1
	sbcs	r0, fp, #0
	bge	.L186
	ldr	r0, .L217
	mov	lr, #0
	adds	r10, r10, r0
	adc	fp, fp, lr
.L186:
	lsr	r4, r10, #23
	mvn	r0, #-16777216
	orr	r4, r4, fp, lsl #9
	cmp	r4, r0
	ite	le
	addle	r1, r1, r4
	addgt	r1, r1, r0
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r0
	ite	le
	addle	r5, r2, r1
	addgt	r5, r2, r0
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r0
	it	ge
	movge	r5, r0
	smull	r0, r1, r5, ip
	cmp	r0, #0
	sbcs	r4, r1, #0
	bge	.L187
	ldr	r4, .L217
	mov	ip, #0
	adds	r0, r0, r4
	adc	r1, r1, ip
.L187:
	lsr	ip, r0, #23
	mvn	r4, #-16777216
	ldr	r0, .L217+4
	orr	ip, ip, r1, lsl #9
	smull	r0, r1, r6, r0
	cmp	ip, #-16777216
	mov	fp, r1
	mov	r10, r0
	it	lt
	movlt	ip, #-16777216
	cmp	ip, r4
	ite	le
	addle	r7, r7, ip
	addgt	r7, r7, r4
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r7, r4
	it	ge
	movge	r7, r4
	cmp	r0, #0
	sbcs	r1, fp, #0
	str	r7, [r9, #172]
	bge	.L188
	sub	r4, r4, #8388608
	movs	r7, #0
	adds	r1, r10, r4
	mov	r10, r1
	mov	r1, fp
	adc	r1, r7, r1
	mov	fp, r1
	b	.L218
.L219:
	.align	2
.L217:
	.word	8388607
	.word	-8388608
	.word	-1677824
.L218:
.L188:
	lsr	r10, r10, #23
	mvn	r0, #-16777216
	ldr	r1, [sp, #28]
	orr	r10, r10, fp, lsl #9
	cmp	r10, r0
	ite	le
	addle	r3, r3, r10
	addgt	r3, r3, r0
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r0
	it	ge
	movge	r3, r0
	smull	r10, fp, r3, r1
	cmp	r10, #0
	sbcs	r3, fp, #0
	bge	.L189
	ldr	r3, .L220
	movs	r1, #0
	adds	r10, r10, r3
	adc	fp, fp, r1
.L189:
	lsr	r0, r10, #23
	mvn	r3, #-16777216
	ldr	r1, .L220+4
	orr	r0, r0, fp, lsl #9
	smull	r10, fp, r5, r1
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r3
	ite	le
	addle	r0, r6, r0
	addgt	r0, r6, r3
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r3
	it	ge
	movge	r0, r3
	cmp	r10, #0
	sbcs	r3, fp, #0
	str	r0, [r9, #208]
	bge	.L190
	ldr	r3, .L220
	movs	r1, #0
	adds	r10, r10, r3
	adc	fp, fp, r1
.L190:
	lsr	r8, r10, #23
	mvn	r3, #-16777216
	orr	r8, r8, fp, lsl #9
	cmp	r8, r3
	ite	le
	addle	r2, r2, r8
	addgt	r2, r2, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	ldr	r3, [sp, #28]
	smull	r2, r3, r2, r3
	cmp	r2, #0
	sbcs	r1, r3, #0
	bge	.L191
	ldr	r1, .L220
	movs	r4, #0
	adds	r2, r2, r1
	adc	r3, r4, r3
.L191:
	ldr	r6, [r9, #164]
	mvn	r7, #-16777216
	lsrs	r1, r2, #23
	cmp	r6, #-16777216
	mov	r4, r6
	orr	r1, r1, r3, lsl #9
	ldr	r3, [sp, #12]
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r7
	it	ge
	movge	r4, r7
	cmp	r1, #-16777216
	mul	ip, r4, r3
	it	lt
	movlt	r1, #-16777216
	mov	r3, r5
	asr	fp, r4, #31
	ldr	r5, [sp, #8]
	cmp	r1, r7
	ite	le
	addle	r3, r3, r1
	addgt	r3, r3, r7
	mla	ip, r5, fp, ip
	str	r3, [sp]
	ldr	r1, [sp]
	ldr	r3, [sp, #8]
	cmp	r1, #-16777216
	umull	r2, r3, r4, r3
	it	lt
	movlt	r1, #-16777216
	add	r3, r3, ip
	cmp	r1, r7
	it	ge
	movge	r1, r7
	cmp	r2, #0
	sbcs	r5, r3, #0
	str	r1, [sp]
	str	r1, [r9, #212]
	bge	.L192
	ldr	r5, .L220
	movs	r7, #0
	adds	r2, r2, r5
	adc	r3, r7, r3
.L192:
	ldr	r5, [r9, #168]
	lsrs	r7, r2, #23
	ldr	r2, .L220+4
	orr	r7, r7, r3, lsl #9
	cmp	r5, #-16777216
	mvn	r3, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r5, r3
	it	ge
	movge	r5, r3
	cmp	r7, r3
	ite	le
	addle	r5, r5, r7
	addgt	r5, r5, r3
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r3
	it	ge
	movge	r5, r3
	smull	r10, fp, r5, r2
	str	r5, [r9, #168]
	cmp	r10, #0
	sbcs	r3, fp, #0
	bge	.L193
	ldr	r7, .L220
	mov	ip, #0
	adds	r10, r10, r7
	adc	fp, fp, ip
.L193:
	lsr	r8, r10, #23
	mvn	r3, #-16777216
	ldr	r2, .L220+8
	orr	r8, r8, fp, lsl #9
	smull	r6, r7, r6, r2
	cmp	r8, r3
	it	ge
	movge	r8, r3
	cmp	r6, #0
	sbcs	r3, r7, #0
	bge	.L194
	ldr	r3, .L220
	movs	r2, #0
	adds	r6, r6, r3
	adc	r7, r2, r7
.L194:
	lsrs	r2, r6, #23
	mvn	r3, #-16777216
	orr	r2, r2, r7, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ite	le
	addle	r2, r8, r2
	addgt	r2, r8, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ite	le
	addle	r2, r0, r2
	addgt	r2, r0, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	mov	r6, r2
	asrs	r7, r2, #31
	ldrd	r10, [sp, #8]
	strd	r6, [sp, #16]
	ldr	r1, [sp, #20]
	mul	r6, r2, fp
	umull	r2, r3, r2, r10
	mla	r6, r10, r1, r6
	cmp	r2, #0
	mov	r10, r2
	add	r3, r3, r6
	mov	fp, r3
	sbcs	r3, fp, #0
	bge	.L195
	ldr	r6, .L220
	movs	r7, #0
	adds	r10, r2, r6
	adc	fp, r7, fp
.L195:
	ldr	r6, [r9, #180]
	mvn	r7, #-16777216
	lsr	r2, r10, #23
	mov	r1, r4
	cmp	r6, #-16777216
	mov	lr, r6
	orr	r2, r2, fp, lsl #9
	ldr	r4, [sp, #8]
	it	lt
	movlt	lr, #-16777216
	mov	r3, r2
	cmp	lr, r7
	it	ge
	movge	lr, r7
	cmp	r2, #-16777216
	ldr	r2, [sp, #12]
	it	lt
	movlt	r3, #-16777216
	asr	fp, lr, #31
	mul	ip, lr, r2
	cmp	r3, r7
	ite	le
	addle	r1, r1, r3
	addgt	r1, r1, r7
	ldr	r3, [sp, #8]
	mla	ip, r4, fp, ip
	cmp	r1, #-16777216
	umull	r2, r3, lr, r3
	it	lt
	movlt	r1, #-16777216
	add	r3, r3, ip
	cmp	r7, r1
	it	ge
	movge	r7, r1
	cmp	r2, #0
	sbcs	r4, r3, #0
	str	r7, [r9, #164]
	bge	.L196
	ldr	r4, .L220
	movs	r7, #0
	adds	r2, r2, r4
	adc	r3, r7, r3
.L196:
	ldr	r4, [r9, #184]
	lsrs	r7, r2, #23
	ldr	r2, .L220+4
	orr	r7, r7, r3, lsl #9
	cmp	r4, #-16777216
	mvn	r3, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r4, r3
	it	ge
	movge	r4, r3
	cmp	r7, r3
	ite	le
	addle	r4, r4, r7
	addgt	r4, r4, r3
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r3
	it	ge
	movge	r4, r3
	smull	r10, fp, r4, r2
	str	r4, [r9, #184]
	cmp	r10, #0
	sbcs	r3, fp, #0
	bge	.L197
	ldr	r7, .L220
	mov	ip, #0
	adds	r10, r10, r7
	adc	fp, fp, ip
.L197:
	lsr	ip, r10, #23
	mvn	r3, #-16777216
	ldr	r2, .L220+8
	orr	ip, ip, fp, lsl #9
	smull	r6, r7, r6, r2
	cmp	ip, r3
	it	ge
	movge	ip, r3
	cmp	r6, #0
	sbcs	r3, r7, #0
	bge	.L198
	ldr	r3, .L220
	movs	r2, #0
	adds	r6, r6, r3
	adc	r7, r2, r7
	b	.L221
.L222:
	.align	2
.L220:
	.word	8388607
	.word	-8388608
	.word	-1677824
.L221:
.L198:
	lsrs	r2, r6, #23
	mvn	r3, #-16777216
	ldr	r1, [sp]
	orr	r2, r2, r7, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ite	le
	addle	r2, ip, r2
	addgt	r2, ip, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ite	le
	addle	r1, r1, r2
	addgt	r1, r1, r3
	cmp	r1, #-16777216
	mov	r2, r1
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	mov	r6, r2
	asrs	r7, r2, #31
	ldrd	r10, [sp, #8]
	strd	r6, [sp, #16]
	ldr	r1, [sp, #20]
	mul	r6, r2, fp
	umull	r2, r3, r2, r10
	mla	r6, r10, r1, r6
	cmp	r2, #0
	mov	r10, r2
	add	r3, r3, r6
	mov	fp, r3
	sbcs	r3, fp, #0
	bge	.L199
	ldr	r6, .L223
	movs	r7, #0
	adds	r10, r2, r6
	adc	fp, r7, fp
.L199:
	add	r8, r8, r0
	lsr	r2, r10, #23
	mvn	r6, #-16777216
	ldr	r3, [sp, #24]
	cmp	r8, #-16777216
	orr	r2, r2, fp, lsl #9
	it	lt
	movlt	r8, #-16777216
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r8, r6
	it	ge
	movge	r8, r6
	cmp	r2, r6
	ite	le
	addle	r0, lr, r2
	addgt	r0, lr, r6
	cmp	r0, #-16777216
	smull	r7, r8, r8, r3
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r6
	it	ge
	movge	r0, r6
	cmp	r7, #0
	sbcs	r3, r8, #0
	str	r0, [r9, #180]
	bge	.L200
	ldr	r0, .L223
	movs	r6, #0
	adds	r7, r7, r0
	adc	r8, r6, r8
.L200:
	ldr	r3, [sp]
	lsrs	r2, r7, #23
	mvn	r0, #-16777216
	add	r3, r3, ip
	orr	r2, r2, r8, lsl #9
	cmp	r3, #-16777216
	mov	r1, r3
	ldr	r3, [sp, #24]
	it	lt
	movlt	r1, #-16777216
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r1, r0
	it	ge
	movge	r1, r0
	cmp	r2, r0
	ite	le
	addle	r5, r5, r2
	addgt	r5, r5, r0
	cmp	r5, #-16777216
	smull	r6, r7, r1, r3
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r0
	it	ge
	movge	r5, r0
	cmp	r6, #0
	sbcs	r3, r7, #0
	str	r5, [r9, #104]
	bge	.L201
	ldr	r1, .L223
	movs	r0, #0
	adds	r6, r6, r1
	adc	r7, r0, r7
.L201:
	lsrs	r2, r6, #23
	mvn	r1, #-16777216
	movs	r3, #0
	orr	r2, r2, r7, lsl #9
	str	r3, [r9, #4]
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r1
	ite	le
	addle	r4, r4, r2
	addgt	r4, r4, r1
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r1
	it	ge
	movge	r4, r1
	str	r4, [r9, #108]
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L206:
	ldr	r3, [r9]
	adds	r3, r3, #2
	str	r3, [r9]
	b	.L140
.L204:
	vldr.32	s14, .L223+4
	vmov.f32	s0, #1.6e+1
	vmul.f32	s15, s15, s14
	vmul.f32	s0, s15, s0
	bl	exp2f
	vmov.f32	s15, s0
	ldr	r3, [r9, #88]
	mvn	r1, #-16777216
	ldr	ip, [r9, #188]
	vcvt.s32.f32	s15, s15, #23
	cmp	r3, #-16777216
	mov	r0, r3
	it	lt
	movlt	r0, #-16777216
	vmov	r2, s15	@ int
	cmp	r2, r1
	it	ge
	movge	r2, r1
	cmp	r0, r1
	mov	r4, r2
	it	ge
	movge	r0, r1
	asrs	r5, r2, #31
	str	r2, [r9, #4]
	mov	r10, r0
	asr	fp, r0, #31
	strd	r4, [sp, #8]
	b	.L146
.L207:
	vldr.32	s14, .L223+4
	vmov.f32	s0, #1.6e+1
	vmul.f32	s15, s15, s14
	vmul.f32	s0, s15, s0
	bl	exp2f
	vmov.f32	s15, s0
	mvn	r3, #-16777216
	vcvt.s32.f32	s15, s15, #23
	vmov	ip, s15	@ int
	cmp	ip, r3
	it	ge
	movge	ip, r3
	b	.L142
.L224:
	.align	2
.L223:
	.word	8388607
	.word	872415232
	.size	dance_ir_h_l_spn, .-dance_ir_h_l_spn
	.section	.text.fv1_init,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-d16
	.type	fv1_init, %function
fv1_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r3, r0
	ldr	r0, .L227
	blx	r3
	movs	r5, #0
	mov	r4, r0
	mov	r2, #131072
	str	r5, [r0]
	mov	r1, r5
	add	r0, r0, #272
	movs	r6, #1
	add	r7, r4, #28
	strd	r5, r5, [r4, #4]
	strd	r5, r5, [r4, #12]
	strd	r5, r5, [r4, #20]
	strd	r5, r5, [r4, #28]
	strd	r5, r5, [r4, #36]
	strd	r5, r5, [r4, #44]
	strd	r5, r5, [r4, #52]
	strd	r5, r5, [r4, #60]
	strd	r5, r5, [r4, #68]
	strd	r5, r5, [r4, #76]
	strd	r5, r5, [r4, #84]
	strd	r5, r5, [r4, #92]
	strd	r5, r5, [r4, #100]
	strd	r5, r5, [r4, #108]
	strd	r5, r5, [r4, #116]
	strd	r5, r5, [r4, #124]
	strd	r5, r5, [r4, #132]
	strd	r5, r5, [r4, #140]
	strd	r5, r5, [r4, #148]
	strd	r5, r5, [r4, #156]
	strd	r5, r5, [r4, #164]
	strd	r5, r5, [r4, #172]
	strd	r5, r5, [r4, #180]
	strd	r5, r5, [r4, #188]
	strd	r5, r5, [r4, #196]
	strd	r5, r5, [r4, #204]
	strd	r5, r5, [r4, #212]
	strd	r5, r5, [r4, #220]
	strd	r5, r5, [r4, #228]
	strd	r5, r5, [r4, #236]
	strd	r5, r5, [r4, #244]
	strd	r5, r5, [r4, #252]
	strd	r5, r5, [r4, #260]
	str	r5, [r4, #268]
	bl	memset
	ldr	r0, .L227+4
	ldr	r3, .L227+8
	add	r2, r4, #20
	str	r5, [r4, r0]
	mvn	r0, #274
	str	r5, [r4, r3]
	sub	r3, r3, #8519680
	ldr	r1, .L227+12
	add	ip, r4, #16
	add	r3, r3, r0
	add	r0, r4, #24
	str	r3, [r4, r1]
	adds	r1, r1, #12
	str	r5, [r4, r1]
	adds	r1, r1, #4
	str	r3, [r4, r1]
	add	r1, r4, #36
	ldr	r3, .L227+16
	str	r5, [r4, r3]
	adds	r3, r3, #12
	str	r5, [r4, r3]
	adds	r3, r3, #12
	strb	r6, [r4, r3]
	adds	r3, r3, #4
	add	r6, r4, #32
	str	r5, [r4, r3]
	subs	r3, r3, #52
	add	r5, r4, #44
	str	r2, [r4, r3]
	adds	r3, r3, #4
	add	r2, r4, #40
	str	ip, [r4, r3]
	adds	r3, r3, #12
	str	r7, [r4, r3]
	adds	r3, r3, #4
	str	r0, [r4, r3]
	adds	r3, r3, #8
	mov	r0, r4
	str	r1, [r4, r3]
	adds	r3, r3, #4
	ldr	r1, .L227+20
	str	r6, [r4, r3]
	adds	r3, r3, #12
	str	r5, [r4, r1]
	str	r2, [r4, r3]
	pop	{r3, r4, r5, r6, r7, pc}
.L228:
	.align	2
.L227:
	.word	131412
	.word	131344
	.word	131348
	.word	131352
	.word	131380
	.word	131396
	.size	fv1_init, .-fv1_init
	.section	.text.fv1_set_fx,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_set_fx
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-d16
	.type	fv1_set_fx, %function
fv1_set_fx:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	cbz	r0, .L235
	movs	r3, #0
	ldr	r2, .L238
	push	{r4, r5, r6, lr}
	mov	r4, r0
	ldr	r0, .L238+4
	movs	r6, #1
	ldr	r5, [r0, r1, lsl #2]
	mov	r0, r4
	strb	r6, [r4, r2]
	mov	r1, r3
	str	r3, [r4, #4]
	movs	r2, #124
	str	r3, [r4, #8]
	str	r3, [r0], #144
	bl	memset
	ldr	r3, .L238+8
	str	r5, [r4, r3]
	pop	{r4, r5, r6, pc}
.L235:
	bx	lr
.L239:
	.align	2
.L238:
	.word	131404
	.word	.LANCHOR0
	.word	131408
	.size	fv1_set_fx, .-fv1_set_fx
	.section	.text.fv1_process,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	fv1_process
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv5-d16
	.type	fv1_process, %function
fv1_process:
	@ args = 8, pretend = 0, frame = 72
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	mov	r4, r0
	vpush.64	{d8}
	sub	sp, sp, #76
	ldr	r0, [sp, #124]
	cmp	r4, #0
	beq	.L240
	cmp	r0, #0
	beq	.L240
	vmov.f32	s15, s0
	mvn	r5, #-16777216
	mov	lr, r1
	str	r2, [sp, #36]
	str	r3, [sp, #28]
	vcvt.s32.f32	s15, s15, #23
	add	r0, lr, r0, lsl #2
	str	r1, [sp, #12]
	str	r0, [sp, #52]
	vstr.32	s15, [sp, #16]	@ int
	vmov.f32	s15, s1
	ldr	r7, [sp, #16]
	ldr	r1, .L255
	cmp	r7, #-16777216
	vcvt.s32.f32	s15, s15, #23
	add	r1, r4, r1
	ldr	r6, .L255+4
	it	lt
	movlt	r7, #-16777216
	ldr	fp, .L255+32
	vstr.32	s15, [sp, #20]	@ int
	vmov.f32	s15, s2
	str	r7, [sp, #16]
	add	fp, fp, r4
	ldr	r7, [sp, #20]
	vcvt.s32.f32	s15, s15, #23
	str	r1, [sp, #64]
	cmp	r7, #-16777216
	ldr	r10, .L255+36
	ldr	r9, .L255+40
	it	lt
	movlt	r7, #-16777216
	vmov	ip, s15	@ int
	ldr	r8, .L255+44
	add	r10, r10, r4
	str	r7, [sp, #20]
	vmov	r7, s15	@ int
	add	r9, r9, r4
	add	r8, r8, r4
	cmp	r7, #-16777216
	ldr	r7, [sp, #16]
	vldr.32	s16, .L255+8
	it	lt
	movlt	ip, #-16777216
	mov	r2, r7
	cmp	r7, r5
	ldr	r7, [sp, #20]
	str	ip, [sp, #24]
	ldr	r3, [sp, #24]
	it	ge
	movge	r2, r5
	cmp	r7, r5
	mov	r0, r7
	ldr	r7, .L255+12
	it	ge
	movge	r0, r5
	cmp	r3, r5
	add	r1, r4, r7
	add	r7, r7, #4
	it	ge
	movge	r3, r5
	str	r2, [sp, #16]
	str	r1, [sp, #40]
	adds	r1, r4, r7
	str	r3, [sp, #24]
	adds	r7, r7, #4
	ldr	r3, .L255+16
	ldr	r2, .L255+20
	adds	r3, r4, r3
	str	r0, [sp, #20]
	adds	r2, r4, r2
	adds	r0, r4, r6
	str	r1, [sp, #8]
	adds	r1, r4, r7
	str	r3, [sp, #60]
	movs	r7, #0
	ldr	r3, [sp, #120]
	ldr	r6, .L255+24
	str	r0, [sp, #68]
	str	r1, [sp, #44]
	str	r2, [sp, #56]
	str	r3, [sp, #32]
.L246:
	ldr	r3, [sp, #12]
	mov	r0, r4
	vldmia.32	r3!, {s14}
	str	r3, [sp, #12]
	vmov.f32	s13, s14
	ldr	r3, [sp, #36]
	vldmia.32	r3!, {s15}
	vcvt.s32.f32	s13, s13, #23
	vmov.f32	s14, s15
	str	r3, [sp, #36]
	vmov	r1, s13	@ int
	movs	r3, #0
	vcvt.s32.f32	s14, s14, #23
	cmp	r1, #-16777216
	str	r3, [r4]
	it	lt
	movlt	r1, #-16777216
	ldr	r3, [sp, #16]
	vmov	r2, s14	@ int
	str	r3, [r4, #80]
	cmp	r2, #-16777216
	ldr	r3, [sp, #20]
	it	lt
	movlt	r2, #-16777216
	cmp	r1, r5
	str	r3, [r4, #84]
	it	ge
	movge	r1, r5
	ldr	r3, [sp, #24]
	cmp	r2, r5
	str	r3, [r4, #88]
	it	ge
	movge	r2, r5
	ldr	r3, [sp, #68]
	ldr	r3, [r3]
	strd	r1, r2, [r4, #96]
	blx	r3
	ldr	r3, [r4, #4]
	ldr	r0, [sp, #40]
	mov	ip, #0
	cmp	r3, #-16777216
	ldr	r1, [sp, #8]
	ldr	r2, [r0]
	it	lt
	movlt	r3, #-16777216
	ldr	r1, [r1, #12]
	subs	r2, r2, #1
	mov	lr, r3
	ldr	r3, [sp, #64]
	cmp	lr, r5
	strb	ip, [r3]
	mov	r3, lr
	str	r2, [r0]
	it	ge
	movge	r3, r5
	str	r3, [r4, #8]
	movs	r3, #0
	str	r3, [r4, #4]
	ldr	r0, [r1]
	ldr	r1, [sp, #8]
	asrs	r0, r0, #8
	ldr	ip, [r1]
	smull	r1, r2, ip, r0
	cmp	r1, #0
	sbcs	r3, r2, #0
	strd	r1, [sp]
	bge	.L242
	adds	r3, r1, r6
	str	r3, [sp]
	ldr	r3, [sp, #4]
	adc	r3, r7, r3
	str	r3, [sp, #4]
.L242:
	ldr	r3, [sp]
	lsrs	r1, r3, #23
	ldr	r3, [sp, #44]
	ldr	r2, [r3]
	str	r2, [sp, #48]
	ldr	r2, [sp, #4]
	orr	r1, r1, r2, lsl #9
	ldr	r2, [sp, #48]
	add	r2, r2, r1
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r5
	it	ge
	movge	r2, r5
	smull	r0, r1, r0, r2
	str	r2, [r3]
	cmp	r0, #0
	sbcs	r3, r1, #0
	bge	.L243
	adds	r0, r0, r6
	adc	r1, r7, r1
.L243:
	lsrs	r3, r0, #23
	ldr	r2, [fp, #12]
	orr	r3, r3, r1, lsl #9
	ldr	r1, [sp, #8]
	sub	r3, ip, r3
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r5
	it	ge
	movge	r3, r5
	str	r3, [r1]
	ldr	r0, [r2]
	ldr	ip, [fp]
	asrs	r0, r0, #8
	smull	r1, r2, ip, r0
	cmp	r1, #0
	sbcs	r3, r2, #0
	strd	r1, [sp]
	bge	.L244
	adds	r3, r1, r6
	str	r3, [sp]
	ldr	r3, [sp, #4]
	adc	r3, r7, r3
	str	r3, [sp, #4]
.L244:
	ldr	r3, [sp]
	ldr	r2, [r10]
	lsrs	r1, r3, #23
	ldr	r3, [sp, #4]
	orr	r1, r1, r3, lsl #9
	add	r2, r2, r1
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r5
	it	ge
	movge	r2, r5
	smull	r0, r1, r0, r2
	str	r2, [r10]
	cmp	r0, #0
	sbcs	r3, r1, #0
	strd	r0, [sp]
	bge	.L245
	adds	r3, r0, r6
	str	r3, [sp]
	ldr	r3, [sp, #4]
	adc	r3, r7, r3
	str	r3, [sp, #4]
.L245:
	ldr	r2, [sp, #56]
	ldr	r3, [sp]
	ldr	r0, [r2]
	lsrs	r3, r3, #23
	ldr	r2, [sp, #4]
	ldr	lr, [r9, #8]
	orr	r3, r3, r2, lsl #9
	ldr	r1, [sp, #60]
	ldr	r2, [r9]
	sub	r3, ip, r3
	ldr	ip, [r1]
	ldr	r1, [r8, #8]
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r5
	it	ge
	movge	r3, r5
	str	r3, [fp]
	ldr	r3, [r0]
	ldr	r0, [lr]
	asrs	r3, r3, #21
	ldr	ip, [ip]
	sub	r2, r2, r0, asr #12
	ldr	r0, .L255+28
	ldr	r1, [r1]
	asr	ip, ip, #21
	asr	r3, r0, r3
	vldr.32	s15, [r4, #108]	@ int
	vldr.32	s14, [r4, #104]	@ int
	asr	ip, r0, ip
	ands	r3, r3, r2
	vcvt.f32.s32	s15, s15
	vcvt.f32.s32	s14, s14
	ldr	r2, [sp, #12]
	str	r3, [r9]
	ldr	r3, [r8]
	vmul.f32	s15, s15, s16
	vmul.f32	s14, s14, s16
	sub	r3, r3, r1, asr #12
	and	r3, r3, ip
	str	r3, [r8]
	ldr	r3, [sp, #28]
	vstmia.32	r3!, {s14}
	str	r3, [sp, #28]
	ldr	r3, [sp, #32]
	vstmia.32	r3!, {s15}
	str	r3, [sp, #32]
	ldr	r3, [sp, #52]
	cmp	r3, r2
	bne	.L246
.L240:
	add	sp, sp, #76
	@ sp needed
	vldm	sp!, {d8}
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L256:
	.align	2
.L255:
	.word	131404
	.word	131408
	.word	872415232
	.word	131344
	.word	131396
	.word	131384
	.word	8388607
	.word	4194303
	.word	131364
	.word	131368
	.word	131380
	.word	131392
	.size	fv1_process, .-fv1_process
	.global	programs
	.section	.data.programs,"aw"
	.align	2
	.set	.LANCHOR0,. + 0
	.type	programs, %object
	.size	programs, 8
programs:
	.word	dance_ir_h_l_spn
	.word	OEM1_4_spn
	.ident	"GCC: (xPack GNU Arm Embedded GCC, 64-bit) 9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]"
