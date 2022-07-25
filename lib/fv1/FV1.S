	.syntax unified
	.cpu cortex-m7
	.eabi_attribute 28, 1
	.fpu fpv5-d16
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.thumb
	.syntax unified
	.file	"FV1.cpp"
	.section	.flashmem,"ax",%progbits
	.align	2
	.global	OEM1_4_spn
	.thumb
	.thumb_func
	.type	OEM1_4_spn, %function
OEM1_4_spn:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L276
	ldrb	r3, [r0, r3]	@ zero_extendqisi2
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	sub	sp, sp, #12
	cmp	r3, #0
	bne	.L2
	ldr	r2, .L276+4
	ldr	r3, .L276+8
	ldr	r7, [r0, r2]
	ldr	r6, [r0]
	subs	r3, r3, r7
	asrs	r1, r7, #10
	mov	r10, r7
	asr	fp, r7, #31
	mov	r4, r3
	asrs	r5, r3, #31
	adds	r6, r6, #1
	ssat	r7, #25, r7
	add	lr, r1, #1
	str	r6, [r0]
	strd	r4, [sp]
.L3:
	ldr	ip, [r0, #96]
	mov	r2, ip
	lsl	r4, ip, #21
	asrs	r3, r2, #31
	cmp	r4, #0
	lsl	r3, r3, #21
	orr	r5, r3, ip, lsr #11
	sbcs	r3, r5, #0
	blt	.L197
.L4:
	lsrs	r4, r4, #23
	asrs	r3, r5, #23
	orr	r2, r4, r5, lsl #9
	cmp	r2, #16777216
	sbcs	r4, r3, #0
	bge	.L149
	cmp	r2, #-16777216
	mov	ip, r2
	it	lt
	movlt	ip, #-16777216
.L5:
	ldr	r2, [r0, #100]
	ldr	r3, [r0, #4]
	asr	r9, r2, #31
	add	ip, ip, r3
	lsls	r4, r2, #21
	lsl	r3, r9, #21
	cmp	r4, #0
	ssat	ip, #25, ip
	orr	r5, r3, r2, lsr #11
	sbcs	r3, r5, #0
	blt	.L198
.L6:
	lsrs	r4, r4, #23
	asrs	r3, r5, #23
	orr	r2, r4, r5, lsl #9
	cmp	r2, #16777216
	sbcs	r4, r3, #0
	bge	.L150
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
.L7:
	ldr	r3, .L276+12
	add	ip, ip, r2
	ldr	r3, [r0, r3]
	ssat	r2, #25, ip
	add	r1, r1, r3
	ubfx	r4, r3, #0, #15
	ubfx	r1, r1, #0, #15
	add	r4, r0, r4, lsl #2
	add	r1, r0, r1, lsl #2
	str	r2, [r4, #272]
	ldr	r2, [sp, #4]
	ldr	r4, [r1, #272]
	ldr	r1, [sp]
	asr	r9, r4, #31
	mul	r2, r4, r2
	umull	r4, r5, r4, r1
	mla	r2, r1, r9, r2
	cmp	r4, #0
	add	r5, r5, r2
	sbcs	r2, r5, #0
	blt	.L199
.L8:
	lsrs	r2, r4, #23
	add	lr, lr, r3
	mvn	ip, #-16777216
	orr	r2, r2, r5, lsl #9
	ubfx	lr, lr, #0, #15
	cmp	r2, #-16777216
	add	lr, r0, lr, lsl #2
	it	lt
	movlt	r2, #-16777216
	ldr	r1, [lr, #272]
	cmp	r2, ip
	umull	r4, r5, r1, r10
	asr	r9, r1, #31
	mul	lr, r1, fp
	it	ge
	movge	r2, ip
	cmp	r1, #-16777216
	mla	r10, r10, r9, lr
	it	lt
	movlt	r1, #-16777216
	add	r5, r5, r10
	cmp	r1, ip
	it	ge
	movge	r1, ip
	cmp	r4, #0
	str	r1, [r0, #12]
	sbcs	r1, r5, #0
	blt	.L200
.L9:
	lsr	ip, r4, #23
	mvn	lr, #-16777216
	movw	r1, #4101
	ldr	r4, [r0, #84]
	orr	ip, ip, r5, lsl #9
	ldr	r5, .L276+16
	add	r1, r1, r3
	cmp	ip, #-16777216
	smull	r8, r9, r4, r5
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	ip, #-16777216
	add	r1, r0, r1, lsl #2
	cmp	ip, lr
	ite	le
	addle	r2, r2, ip
	addgt	r2, r2, lr
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, lr
	it	ge
	movge	r2, lr
	cmp	r8, #0
	str	r2, [r1, #272]
	sbcs	r1, r9, #0
	blt	.L201
.L10:
	lsr	r1, r8, #23
	asr	r5, r9, #23
	orr	r4, r1, r9, lsl #9
	cmp	r4, #16777216
	sbcs	r1, r5, #0
	bge	.L151
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L11:
	ldr	r1, [r0, #80]
	add	r7, r7, r4
	mvn	r5, #-16777216
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r1, r5
	it	ge
	movge	r1, r5
	cmp	r7, r5
	and	r4, r1, #15728640
	it	ge
	movge	r7, r5
	sub	lr, r4, #7864320
	str	r7, [r0, #32]
	cmp	lr, #0
	str	lr, [r0, #4]
	ble	.L12
	movw	r1, #18435
	ldr	r4, .L276+20
	adds	r6, r6, #38
	add	r1, r1, r3
	str	r6, [r0]
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	smull	r4, r5, r1, r4
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L202
.L13:
	lsrs	r1, r4, #23
	asrs	r7, r5, #23
	orr	r6, r1, r5, lsl #9
	cmp	r6, #16777216
	sbcs	r1, r7, #0
	blt	.L14
	mvn	r1, #-16777216
.L15:
	add	r1, r1, lr
	ssat	r4, #25, r1
.L16:
	movw	r1, #18124
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r5, r9, #22
	cmp	r6, #0
	orr	r7, r5, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L203
.L31:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L152
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L32:
	add	r1, r1, r4
	ssat	r7, #25, r1
.L17:
	movw	r1, #17609
	ldr	r4, .L276+20
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	smull	r4, r5, r1, r4
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L204
.L33:
	lsrs	r1, r4, #23
	asr	r9, r5, #23
	orr	r8, r1, r5, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L153
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L34:
	add	r1, r1, r7
	ssat	r4, #25, r1
.L18:
	movw	r1, #17202
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r5, r9, #22
	cmp	r6, #0
	orr	r7, r5, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L205
.L35:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L154
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L36:
	add	r1, r1, r4
	ssat	r5, #25, r1
.L19:
	movw	r4, #16861
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L206
.L37:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L155
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L38:
	add	r1, r1, r5
	ssat	r7, #25, r1
.L20:
	movw	r4, #16221
	ldr	r1, .L276+20
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r4, [r4, #272]
	smull	r4, r5, r4, r1
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L207
	b	.L277
.L278:
	.align	2
.L276:
	.word	131404
	.word	131380
	.word	4194303
	.word	131344
	.word	-4194304
	.word	3350528
.L277:
.L39:
	lsrs	r1, r4, #23
	asr	r9, r5, #23
	orr	r8, r1, r5, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L156
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L40:
	add	r1, r1, r7
	ssat	r1, #25, r1
.L21:
	movw	r4, #15866
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	lr, [r4, #272]
	mov	r6, lr
	lsl	r4, lr, #22
	asrs	r7, r6, #31
	cmp	r4, #0
	lsl	r6, r7, #22
	orr	r5, r6, lr, lsr #10
	sbcs	r6, r5, #0
	blt	.L208
.L41:
	lsrs	r4, r4, #23
	asrs	r7, r5, #23
	orr	r6, r4, r5, lsl #9
	cmp	r6, #16777216
	sbcs	r4, r7, #0
	bge	.L157
	cmp	r6, #-16777216
	mov	r5, r6
	it	lt
	movlt	r5, #-16777216
.L42:
	add	r5, r5, r1
	ssat	r1, #25, r5
.L22:
	movw	r6, #15413
	add	r6, r6, r3
	ubfx	r6, r6, #0, #15
	add	r6, r0, r6, lsl #2
	ldr	lr, [r6, #272]
	mov	r4, lr
	lsl	r6, lr, #22
	asrs	r5, r4, #31
	cmp	r6, #0
	lsl	r4, r5, #22
	orr	r7, r4, lr, lsr #10
	sbcs	r4, r7, #0
	blt	.L209
.L43:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L158
	cmp	r4, #-16777216
	mov	r5, r4
	it	lt
	movlt	r5, #-16777216
.L44:
	add	r5, r5, r1
	ssat	r5, #25, r5
.L23:
	movw	r1, #14851
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L210
.L45:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L159
	cmp	r8, #-16777216
	mov	r4, r8
	it	lt
	movlt	r4, #-16777216
.L46:
	add	r4, r4, r5
	ssat	r4, #25, r4
.L24:
	movw	r1, #14313
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r5, r9, #22
	cmp	r6, #0
	orr	r7, r5, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L211
.L47:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L160
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L48:
	add	r1, r1, r4
	ssat	r1, #25, r1
.L25:
	movw	r4, #13806
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	lr, [r4, #272]
	mov	r4, lr
	lsl	r6, lr, #22
	asrs	r5, r4, #31
	cmp	r6, #0
	lsl	r4, r5, #22
	orr	r7, r4, lr, lsr #10
	sbcs	r4, r7, #0
	blt	.L212
.L49:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L161
	cmp	r4, #-16777216
	mov	r5, r4
	it	lt
	movlt	r5, #-16777216
.L50:
	add	r5, r5, r1
	ssat	r5, #25, r5
.L26:
	movw	r1, #13468
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L213
.L51:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L162
	cmp	r8, #-16777216
	mov	r4, r8
	it	lt
	movlt	r4, #-16777216
.L52:
	add	r4, r4, r5
	ssat	r4, #25, r4
.L27:
	movw	r1, #13006
	ldr	r5, .L279
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r6, [r1, #272]
	smull	r6, r7, r6, r5
	cmp	r6, #0
	sbcs	r1, r7, #0
	blt	.L214
.L53:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L163
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L54:
	add	r1, r1, r4
	ssat	r1, #25, r1
.L28:
	movw	r4, #12676
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	lr, [r4, #272]
	mov	r4, lr
	lsl	r6, lr, #22
	asrs	r5, r4, #31
	cmp	r6, #0
	lsl	r4, r5, #22
	orr	r7, r4, lr, lsr #10
	sbcs	r4, r7, #0
	blt	.L215
.L55:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L164
	cmp	r4, #-16777216
	mov	r5, r4
	it	lt
	movlt	r5, #-16777216
.L56:
	add	r5, r5, r1
	ssat	r5, #25, r5
.L29:
	movw	r1, #12316
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L216
.L57:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L165
	cmp	r8, #-16777216
	mov	r4, r8
	it	lt
	movlt	r4, #-16777216
.L58:
	add	r4, r4, r5
	ssat	r4, #25, r4
.L30:
	movw	r1, #11513
	ldr	r5, .L279
	add	r1, r1, r3
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r6, [r1, #272]
	smull	r6, r7, r6, r5
	cmp	r6, #0
	sbcs	r1, r7, #0
	blt	.L217
.L59:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L166
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L60:
	add	r1, r1, r4
	ssat	r1, #25, r1
.L61:
	movw	r4, #11245
	add	r4, r4, r3
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	lr, [r4, #272]
	mov	r6, lr
	lsl	r4, lr, #22
	asrs	r7, r6, #31
	cmp	r4, #0
	lsl	r6, r7, #22
	orr	r5, r6, lr, lsr #10
	sbcs	r6, r5, #0
	blt	.L218
.L62:
	lsrs	r4, r4, #23
	asrs	r7, r5, #23
	orr	r6, r4, r5, lsl #9
	cmp	r6, #16777216
	sbcs	r4, r7, #0
	bge	.L167
	cmp	r6, #-16777216
	mov	r5, r6
	it	lt
	movlt	r5, #-16777216
.L63:
	movw	r4, #10614
	adds	r6, r5, r1
	add	r4, r4, r3
	ssat	r6, #25, r6
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asr	r9, r1, #31
	lsls	r4, r1, #22
	lsl	r7, r9, #22
	cmp	r4, #0
	orr	r5, r7, r1, lsr #10
	sbcs	r1, r5, #0
	blt	.L219
.L64:
	lsrs	r1, r4, #23
	asr	r9, r5, #23
	orr	r8, r1, r5, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L168
	cmp	r8, #-16777216
	mov	r5, r8
	it	lt
	movlt	r5, #-16777216
.L65:
	movw	r4, #9923
	add	r6, r6, r5
	add	r4, r4, r3
	ssat	r6, #25, r6
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asr	r9, r1, #31
	lsls	r4, r1, #22
	lsl	r7, r9, #22
	cmp	r4, #0
	orr	r5, r7, r1, lsr #10
	sbcs	r1, r5, #0
	blt	.L220
.L66:
	lsrs	r1, r4, #23
	asr	r9, r5, #23
	orr	r8, r1, r5, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L169
	cmp	r8, #-16777216
	mov	r5, r8
	it	lt
	movlt	r5, #-16777216
.L67:
	movw	r4, #9421
	add	r6, r6, r5
	add	r4, r4, r3
	ssat	r6, #25, r6
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asr	r9, r1, #31
	lsls	r4, r1, #22
	lsl	r7, r9, #22
	cmp	r4, #0
	orr	r5, r7, r1, lsr #10
	sbcs	r1, r5, #0
	blt	.L221
.L68:
	lsrs	r1, r4, #23
	asr	r9, r5, #23
	orr	r8, r1, r5, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L170
	cmp	r8, #-16777216
	mov	r5, r8
	it	lt
	movlt	r5, #-16777216
.L69:
	movw	r4, #9239
	add	r6, r6, r5
	add	r4, r4, r3
	ssat	r6, #25, r6
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asrs	r5, r1, #31
	lsl	r8, r1, #22
	lsls	r4, r5, #22
	cmp	r8, #0
	orr	r9, r4, r1, lsr #10
	sbcs	r1, r9, #0
	blt	.L222
.L70:
	lsr	r1, r8, #23
	asr	r5, r9, #23
	orr	r4, r1, r9, lsl #9
	cmp	r4, #16777216
	sbcs	r1, r5, #0
	bge	.L171
	cmp	r4, #-16777216
	mov	r5, r4
	it	lt
	movlt	r5, #-16777216
.L71:
	movw	r4, #8677
	add	r6, r6, r5
	add	r4, r4, r3
	ssat	r6, #25, r6
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asrs	r5, r1, #31
	lsl	r8, r1, #22
	lsls	r4, r5, #22
	cmp	r8, #0
	orr	r9, r4, r1, lsr #10
	sbcs	r1, r9, #0
	blt	.L223
.L72:
	lsr	r1, r8, #23
	asr	r5, r9, #23
	orr	r4, r1, r9, lsl #9
	cmp	r4, #16777216
	sbcs	r1, r5, #0
	bge	.L172
	cmp	r4, #-16777216
	mov	r5, r4
	it	lt
	movlt	r5, #-16777216
.L73:
	movw	r1, #8008
	ldr	r4, .L279
	add	r6, r6, r5
	add	r1, r1, r3
	ssat	r6, #25, r6
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	smull	r4, r5, r1, r4
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L224
.L74:
	lsrs	r1, r4, #23
	asr	r9, r5, #23
	orr	r8, r1, r5, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L173
	cmp	r8, #-16777216
	mov	r4, r8
	it	lt
	movlt	r4, #-16777216
.L75:
	movw	r1, #7521
	adds	r5, r4, r6
	add	r1, r1, r3
	ssat	r5, #25, r5
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L225
.L76:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L174
	cmp	r8, #-16777216
	mov	r4, r8
	it	lt
	movlt	r4, #-16777216
.L77:
	movw	r1, #7075
	add	r5, r5, r4
	add	r1, r1, r3
	ssat	r5, #25, r5
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L226
	b	.L280
.L281:
	.align	2
.L279:
	.word	3350528
.L280:
.L78:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L175
	cmp	r8, #-16777216
	mov	r4, r8
	it	lt
	movlt	r4, #-16777216
.L79:
	movw	r1, #6631
	add	r5, r5, r4
	add	r1, r1, r3
	ssat	r5, #25, r5
	ubfx	r1, r1, #0, #15
	add	r1, r0, r1, lsl #2
	ldr	r1, [r1, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L227
.L80:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L176
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L81:
	movw	r4, #6211
	add	r5, r5, r1
	add	r4, r4, r3
	ssat	r5, #25, r5
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L228
.L82:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L177
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L83:
	movw	r4, #5441
	add	r5, r5, r1
	add	r4, r4, r3
	ssat	r5, #25, r5
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L229
.L84:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L178
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L85:
	add	r4, r3, #5024
	add	r5, r5, r1
	ubfx	r4, r4, #0, #15
	ssat	r5, #25, r5
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asr	r9, r1, #31
	lsls	r6, r1, #22
	lsl	r4, r9, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L230
.L86:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L179
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L87:
	movw	r4, #4601
	add	r5, r5, r1
	add	r4, r4, r3
	ssat	r5, #25, r5
	ubfx	r4, r4, #0, #15
	add	r4, r0, r4, lsl #2
	ldr	r1, [r4, #272]
	asrs	r7, r1, #31
	lsl	r8, r1, #22
	lsls	r4, r7, #22
	cmp	r8, #0
	orr	r9, r4, r1, lsr #10
	sbcs	r1, r9, #0
	blt	.L231
.L88:
	lsr	r1, r8, #23
	asr	r7, r9, #23
	orr	r6, r1, r9, lsl #9
	cmp	r6, #16777216
	sbcs	r1, r7, #0
	bge	.L180
	cmp	r6, #-16777216
	mov	r1, r6
	it	lt
	movlt	r1, #-16777216
.L89:
	ldr	r4, .L282
	add	r1, r1, r5
	smull	r6, r7, r2, r4
	ssat	r1, #25, r1
	cmp	r6, #0
	sbcs	r2, r7, #0
	blt	.L232
.L90:
	lsrs	r5, r6, #23
	movw	r4, #19336
	mvn	lr, #-16777216
	ldr	r6, .L282
	orr	r5, r5, r7, lsl #9
	add	r4, r4, r3
	cmp	r5, #-16777216
	ite	ge
	addge	r2, r1, r5
	addlt	r2, r1, #-16777216
	ubfx	r4, r4, #0, #15
	cmp	r2, #-16777216
	add	r4, r0, r4, lsl #2
	it	lt
	movlt	r2, #-16777216
	ldr	r1, [r4, #272]
	cmp	r2, lr
	mov	r4, r1
	smull	r6, r7, r1, r6
	it	ge
	movge	r2, lr
	cmp	r1, #-16777216
	it	lt
	movlt	r4, #-16777216
	str	r2, [r0, #148]
	cmp	r4, lr
	it	ge
	movge	r4, lr
	cmp	r6, #0
	sbcs	r1, r7, #0
	blt	.L233
.L91:
	lsrs	r1, r6, #23
	asr	r9, r7, #23
	orr	r8, r1, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r1, r9, #0
	bge	.L181
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L92:
	movw	r5, #19102
	add	r1, r1, r2
	ldr	r6, .L282+4
	ssat	r1, #25, r1
	add	r5, r5, r3
	smull	r6, r7, r1, r6
	ubfx	r5, r5, #0, #15
	cmp	r6, #0
	add	r5, r0, r5, lsl #2
	str	r1, [r5, #272]
	sbcs	r1, r7, #0
	blt	.L234
.L93:
	lsrs	r5, r6, #23
	movw	r1, #19783
	mvn	r6, #-16777216
	ldr	r8, .L282
	orr	r5, r5, r7, lsl #9
	add	r1, r1, r3
	adds	r7, r4, r5
	ubfx	r1, r1, #0, #15
	cmp	r7, #-16777216
	add	r1, r0, r1, lsl #2
	it	lt
	movlt	r7, #-16777216
	ldr	r4, [r1, #272]
	cmp	r7, r6
	mov	r1, r4
	smull	r8, r9, r4, r8
	it	ge
	movge	r7, r6
	cmp	r4, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r6
	it	ge
	movge	r1, r6
	cmp	r8, #0
	sbcs	r4, r9, #0
	blt	.L235
.L94:
	lsr	r6, r8, #23
	asr	r5, r9, #23
	orr	r4, r6, r9, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L182
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L95:
	movw	r5, #19337
	add	r4, r4, r7
	ldr	r6, .L282+4
	ssat	r4, #25, r4
	add	r5, r5, r3
	smull	r10, fp, r4, r6
	ubfx	r5, r5, #0, #15
	cmp	r10, #0
	add	r5, r0, r5, lsl #2
	str	r4, [r5, #272]
	sbcs	r4, fp, #0
	blt	.L236
.L96:
	lsr	r6, r10, #23
	movw	r4, #20336
	mvn	r5, #-16777216
	ldr	r8, .L282
	orr	r6, r6, fp, lsl #9
	add	r4, r4, r3
	add	r6, r6, r1
	ubfx	r4, r4, #0, #15
	cmp	r6, #-16777216
	add	r4, r0, r4, lsl #2
	it	lt
	movlt	r6, #-16777216
	ldr	r1, [r4, #272]
	cmp	r6, r5
	mov	r7, r1
	smull	r8, r9, r1, r8
	it	ge
	movge	r6, r5
	cmp	r1, #-16777216
	it	lt
	movlt	r7, #-16777216
	cmp	r7, r5
	it	ge
	movge	r7, r5
	cmp	r8, #0
	sbcs	r1, r9, #0
	blt	.L237
.L97:
	lsr	r1, r8, #23
	asr	r5, r9, #23
	orr	r4, r1, r9, lsl #9
	cmp	r4, #16777216
	sbcs	r1, r5, #0
	bge	.L183
	cmp	r4, #-16777216
	mov	r1, r4
	it	lt
	movlt	r1, #-16777216
.L98:
	add	r1, r1, r6
	movw	r5, #19784
	ldr	r4, .L282+4
	ssat	r1, #25, r1
	add	r5, r5, r3
	ubfx	r6, r5, #0, #15
	smull	r4, r5, r1, r4
	add	r6, r0, r6, lsl #2
	cmp	r4, #0
	str	r1, [r6, #272]
	sbcs	r1, r5, #0
	blt	.L238
.L99:
	lsrs	r1, r4, #23
	movw	lr, #20538
	mvn	r4, #-16777216
	ldr	r6, .L282
	orr	r1, r1, r5, lsl #9
	add	lr, lr, r3
	add	r1, r1, r7
	ubfx	lr, lr, #0, #15
	cmp	r1, #-16777216
	add	lr, r0, lr, lsl #2
	it	lt
	movlt	r1, #-16777216
	ldr	r5, [lr, #272]
	cmp	r1, r4
	mov	lr, r5
	smull	r6, r7, r5, r6
	it	ge
	movge	r1, r4
	cmp	r5, #-16777216
	it	lt
	movlt	lr, #-16777216
	str	r1, [r0, #104]
	cmp	lr, r4
	it	ge
	movge	lr, r4
	cmp	r6, #0
	sbcs	r1, r7, #0
	blt	.L239
.L100:
	lsrs	r1, r6, #23
	asrs	r5, r7, #23
	orr	r4, r1, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r1, r5, #0
	bge	.L184
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L101:
	add	r2, r2, r4
	movw	r5, #20337
	ldr	r4, .L282+4
	ssat	r2, #25, r2
	add	r5, r5, r3
	ubfx	r1, r5, #0, #15
	smull	r4, r5, r2, r4
	add	r1, r0, r1, lsl #2
	cmp	r4, #0
	str	r2, [r1, #272]
	sbcs	r2, r5, #0
	blt	.L240
.L102:
	lsrs	r1, r4, #23
	movw	r2, #20928
	mvn	r4, #-16777216
	ldr	r6, .L282
	orr	r1, r1, r5, lsl #9
	add	r2, r2, r3
	add	lr, lr, r1
	ubfx	r2, r2, #0, #15
	cmp	lr, #-16777216
	add	r2, r0, r2, lsl #2
	it	lt
	movlt	lr, #-16777216
	ldr	r1, [r2, #272]
	cmp	lr, r4
	mov	r2, r1
	smull	r6, r7, r1, r6
	it	ge
	movge	lr, r4
	cmp	r1, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r4
	it	ge
	movge	r2, r4
	cmp	r6, #0
	sbcs	r1, r7, #0
	blt	.L241
.L103:
	lsrs	r1, r6, #23
	asrs	r5, r7, #23
	orr	r4, r1, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r1, r5, #0
	bge	.L185
	cmp	r4, #-16777216
	mov	r1, r4
	it	lt
	movlt	r1, #-16777216
.L104:
	movw	r4, #20539
	add	r1, r1, lr
	ldr	r6, .L282+4
	ssat	r1, #25, r1
	add	r4, r4, r3
	smull	r6, r7, r1, r6
	ubfx	r4, r4, #0, #15
	cmp	r6, #0
	add	r4, r0, r4, lsl #2
	str	r1, [r4, #272]
	sbcs	r1, r7, #0
	blt	.L242
.L105:
	lsr	lr, r6, #23
	movw	r1, #21556
	mvn	r4, #-16777216
	ldr	r6, .L282
	orr	lr, lr, r7, lsl #9
	add	r1, r1, r3
	add	lr, lr, r2
	ubfx	r1, r1, #0, #15
	cmp	lr, #-16777216
	add	r1, r0, r1, lsl #2
	it	lt
	movlt	lr, #-16777216
	ldr	r2, [r1, #272]
	cmp	lr, r4
	mov	r5, r2
	smull	r8, r9, r2, r6
	it	ge
	movge	lr, r4
	cmp	r2, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r4
	it	ge
	movge	r5, r4
	cmp	r8, #0
	sbcs	r2, r9, #0
	blt	.L243
.L106:
	lsr	r2, r8, #23
	asr	r7, r9, #23
	orr	r6, r2, r9, lsl #9
	cmp	r6, #16777216
	sbcs	r2, r7, #0
	bge	.L186
	cmp	r6, #-16777216
	mov	r2, r6
	it	lt
	movlt	r2, #-16777216
.L107:
	movw	r4, #20929
	add	r2, r2, lr
	ldr	r6, .L282+4
	ssat	r2, #25, r2
	add	r4, r4, r3
	smull	r6, r7, r2, r6
	ubfx	r4, r4, #0, #15
	cmp	r6, #0
	add	r4, r0, r4, lsl #2
	str	r2, [r4, #272]
	sbcs	r2, r7, #0
	blt	.L244
.L108:
	movw	r9, #6601
	lsrs	r1, r6, #23
	ldr	r4, .L282+8
	add	r9, r9, r3
	b	.L283
.L284:
	.align	2
.L282:
	.word	5029888
	.word	-5038080
	.word	-8388608
.L283:
	orr	r1, r1, r7, lsl #9
	ldr	r2, [r0, #152]
	ubfx	r9, r9, #0, #15
	add	r1, r1, r5
	add	r9, r0, r9, lsl #2
	ssat	r1, #25, r1
	ldr	ip, [r9, #272]
	str	r1, [r0, #108]
	smull	r4, r5, ip, r4
	cmp	ip, #-16777216
	mov	r8, ip
	it	lt
	movlt	r8, #-16777216
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L245
.L109:
	lsrs	r1, r4, #23
	mvn	r4, #-16777216
	orr	r1, r1, r5, lsl #9
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r4
	ite	le
	rsble	r1, r2, r1
	rsbgt	r1, r2, r4
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r4
	it	ge
	movge	r1, r4
	asrs	r5, r1, #31
	lsls	r6, r1, #22
	lsls	r4, r5, #22
	cmp	r6, #0
	orr	r7, r4, r1, lsr #10
	sbcs	r1, r7, #0
	blt	.L246
.L110:
	lsrs	r1, r6, #23
	ldr	r4, .L285
	orr	r1, r1, r7, lsl #9
	add	r1, r1, r2
	ssat	r1, #25, r1
	smull	r4, r5, r1, r4
	cmp	r4, #0
	sbcs	r2, r5, #0
	blt	.L247
.L111:
	lsrs	r4, r4, #23
	ldr	r2, [r0, #8]
	asrs	r7, r5, #23
	ldr	lr, [r0, #88]
	orr	r6, r4, r5, lsl #9
	cmp	r7, #0
	itt	eq
	cmpeq	r6, #16777216
	mvneq	r6, #-16777216
	add	r6, r6, r2
	ssat	r6, #25, r6
	smull	r6, r7, r6, lr
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L248
.L113:
	lsrs	r5, r6, #23
	mvn	r4, #-16777216
	ldr	r6, [r0, #156]
	orr	r7, r5, r7, lsl #9
	movw	r5, #8601
	ldr	r10, .L285
	cmp	r7, #-16777216
	add	r5, r5, r3
	str	r6, [sp]
	it	lt
	movlt	r7, #-16777216
	cmp	ip, #16777216
	ubfx	r5, r5, #0, #15
	it	ge
	movge	r8, r4
	add	fp, r0, r5, lsl #2
	cmp	r7, r4
	ite	le
	addle	r8, r8, r7
	addgt	r8, r8, r4
	cmp	r8, #-16777216
	it	lt
	movlt	r8, #-16777216
	cmp	r4, r8
	it	ge
	movge	r4, r8
	str	r4, [r9, #272]
	ldr	ip, [fp, #272]
	smull	r6, r7, ip, r10
	cmp	ip, #-16777216
	mov	r10, ip
	it	lt
	movlt	r10, #-16777216
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L249
.L115:
	lsrs	r4, r6, #23
	mvn	r5, #-16777216
	ldr	r6, .L285+4
	orr	r4, r4, r7, lsl #9
	ldr	r7, [sp]
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	ite	le
	rsble	r7, r7, r4
	rsbgt	r7, r7, r5
	cmp	r7, #-16777216
	mov	r4, r7
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	smull	r8, r9, r4, r6
	cmp	r8, #0
	sbcs	r4, r9, #0
	blt	.L250
.L116:
	lsr	r4, r8, #23
	ldr	r5, [sp]
	ldr	r6, .L285
	orr	r4, r4, r9, lsl #9
	add	r5, r5, r4
	ssat	r4, #25, r5
	smull	r6, r7, r4, r6
	str	r4, [r0, #156]
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L251
.L117:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r5, #0
	itet	eq
	cmpeq	r4, #16777216
	movne	r6, r4
	mvneq	r6, #-16777216
	add	r6, r6, r2
	ssat	r6, #25, r6
	smull	r6, r7, r6, lr
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L252
.L119:
	lsrs	r5, r6, #23
	mvn	r4, #-16777216
	movw	r9, #11101
	ldr	r6, .L285
	orr	r5, r5, r7, lsl #9
	add	r9, r9, r3
	cmp	r5, #-16777216
	ubfx	r9, r9, #0, #15
	it	lt
	movlt	r5, #-16777216
	cmp	ip, #16777216
	add	r9, r0, r9, lsl #2
	it	ge
	movge	r10, r4
	cmp	r5, r4
	ite	le
	addle	r10, r10, r5
	addgt	r10, r10, r4
	cmp	r10, #-16777216
	it	lt
	movlt	r10, #-16777216
	cmp	r4, r10
	it	ge
	movge	r4, r10
	str	r4, [fp, #272]
	ldr	ip, [r9, #272]
	smull	r6, r7, ip, r6
	cmp	ip, #-16777216
	mov	r8, ip
	it	lt
	movlt	r8, #-16777216
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L253
.L121:
	lsrs	r4, r6, #23
	mvn	r5, #-16777216
	ldr	r6, .L285+8
	orr	r4, r4, r7, lsl #9
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	ite	le
	rsble	r4, r1, r4
	rsbgt	r4, r1, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	smull	r6, r7, r4, r6
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L254
.L122:
	lsrs	r4, r6, #23
	ldr	r6, .L285
	orr	r4, r4, r7, lsl #9
	add	r1, r1, r4
	ssat	r1, #25, r1
	smull	r6, r7, r1, r6
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L255
.L123:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r5, #0
	itet	eq
	cmpeq	r4, #16777216
	movne	r6, r4
	mvneq	r6, #-16777216
	add	r6, r6, r2
	ssat	r6, #25, r6
	smull	r6, r7, r6, lr
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L256
.L125:
	lsrs	r5, r6, #23
	mvn	r4, #-16777216
	movw	r10, #14101
	ldr	r6, .L285
	orr	r5, r5, r7, lsl #9
	add	r10, r10, r3
	cmp	r5, #-16777216
	ubfx	r10, r10, #0, #15
	it	lt
	movlt	r5, #-16777216
	cmp	ip, #16777216
	add	r10, r0, r10, lsl #2
	it	ge
	movge	r8, r4
	cmp	r5, r4
	ite	le
	addle	r3, r8, r5
	addgt	r3, r8, r4
	cmp	r3, #-16777216
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r4
	it	ge
	movge	r3, r4
	str	r3, [r9, #272]
	ldr	r3, [r10, #272]
	cmp	r3, #-16777216
	mov	ip, r3
	smull	r6, r7, r3, r6
	it	lt
	movlt	ip, #-16777216
	cmp	r4, ip
	it	ge
	movge	r4, ip
	cmp	r6, #0
	str	r4, [r0, #12]
	sbcs	r4, r7, #0
	blt	.L257
.L127:
	lsrs	r4, r6, #23
	mvn	r5, #-16777216
	ldr	r6, .L285+12
	orr	r4, r4, r7, lsl #9
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	ite	le
	rsble	r4, r1, r4
	rsbgt	r4, r1, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	smull	r4, r5, r4, r6
	cmp	r4, #0
	sbcs	r6, r5, #0
	blt	.L258
.L128:
	lsrs	r6, r4, #23
	ldr	r4, .L285
	orr	r6, r6, r5, lsl #9
	add	r1, r1, r6
	ssat	r1, #25, r1
	smull	r4, r5, r1, r4
	str	r1, [r0, #152]
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L259
.L129:
	asrs	r7, r5, #23
	lsrs	r1, r4, #23
	cmp	r7, #0
	orr	r6, r1, r5, lsl #9
	itt	eq
	cmpeq	r6, #16777216
	mvneq	r6, #-16777216
	add	r6, r6, r2
	ssat	r6, #25, r6
	smull	r6, r7, r6, lr
	cmp	r6, #0
	sbcs	r2, r7, #0
	blt	.L260
.L131:
	lsrs	r2, r6, #23
	mvn	r1, #-16777216
	orr	r2, r2, r7, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r1
	it	ge
	movge	r2, r1
	cmp	r3, #16777216
	str	r2, [r0, #4]
	bge	.L132
	str	ip, [r0, #12]
.L148:
	add	r2, r2, ip
	movs	r3, #0
	ssat	r2, #25, r2
	str	r2, [r10, #272]
	str	r3, [r0, #4]
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L2:
	movs	r3, #0
	ldr	r2, .L285+16
	ldr	r6, [r0]
	movs	r4, #0
	str	r3, [r0, #36]
	mov	r7, r3
	str	r3, [r0, r2]
	mov	r1, r3
	ldr	r3, .L285+20
	mov	r10, #0
	mov	fp, #0
	mov	lr, #1
	strd	r3, [sp]
	b	.L3
.L151:
	mvn	r4, #-16777216
	b	.L11
.L150:
	mvn	r2, #-16777216
	b	.L7
.L286:
	.align	2
.L285:
	.word	-8388608
	.word	3355392
	.word	2516480
	.word	1677568
	.word	131380
	.word	4194303
.L149:
	mvn	ip, #-16777216
	b	.L5
.L14:
	cmp	r6, #-16777216
	mov	r1, r6
	it	lt
	movlt	r1, #-16777216
	b	.L15
.L132:
	str	r1, [r0, #12]
	mov	ip, r1
	b	.L148
.L186:
	mvn	r2, #-16777216
	b	.L107
.L185:
	mvn	r1, #-16777216
	b	.L104
.L184:
	mvn	r4, #-16777216
	b	.L101
.L183:
	mvn	r1, #-16777216
	b	.L98
.L182:
	mvn	r4, #-16777216
	b	.L95
.L181:
	mvn	r1, #-16777216
	b	.L92
.L180:
	mvn	r1, #-16777216
	b	.L89
.L179:
	mvn	r1, #-16777216
	b	.L87
.L178:
	mvn	r1, #-16777216
	b	.L85
.L177:
	mvn	r1, #-16777216
	b	.L83
.L176:
	mvn	r1, #-16777216
	b	.L81
.L175:
	mvn	r4, #-16777216
	b	.L79
.L174:
	mvn	r4, #-16777216
	b	.L77
.L173:
	mvn	r4, #-16777216
	b	.L75
.L172:
	mvn	r5, #-16777216
	b	.L73
.L171:
	mvn	r5, #-16777216
	b	.L71
.L170:
	mvn	r5, #-16777216
	b	.L69
.L169:
	mvn	r5, #-16777216
	b	.L67
.L168:
	mvn	r5, #-16777216
	b	.L65
.L167:
	mvn	r5, #-16777216
	b	.L63
.L166:
	mvn	r1, #-16777216
	b	.L60
.L165:
	mvn	r4, #-16777216
	b	.L58
.L164:
	mvn	r5, #-16777216
	b	.L56
.L163:
	mvn	r1, #-16777216
	b	.L54
.L162:
	mvn	r4, #-16777216
	b	.L52
.L161:
	mvn	r5, #-16777216
	b	.L50
.L160:
	mvn	r1, #-16777216
	b	.L48
.L159:
	mvn	r4, #-16777216
	b	.L46
.L158:
	mvn	r5, #-16777216
	b	.L44
.L157:
	mvn	r5, #-16777216
	b	.L42
.L156:
	mvn	r1, #-16777216
	b	.L40
.L155:
	mvn	r1, #-16777216
	b	.L38
.L154:
	mvn	r1, #-16777216
	b	.L36
.L153:
	mvn	r1, #-16777216
	b	.L34
.L12:
	sub	r1, r4, #7340032
	cmp	r1, #0
	str	r1, [r0, #4]
	ble	.L261
	adds	r6, r6, #37
	mov	r4, r1
	str	r6, [r0]
	b	.L16
.L152:
	mvn	r1, #-16777216
	b	.L32
.L209:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L43
.L208:
	ldr	r6, .L287
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L41
.L211:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L47
.L210:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L45
.L213:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L51
.L212:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L49
.L215:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L55
.L214:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L53
.L217:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L59
.L216:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L57
.L219:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L64
.L218:
	ldr	r6, .L287
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L62
.L200:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L9
.L199:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L8
.L198:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L6
.L197:
	ldr	r2, .L287
	movs	r3, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	b	.L4
.L260:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L131
.L259:
	ldr	r6, .L287
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L129
.L258:
	ldr	r6, .L287
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L128
.L257:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L127
.L256:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L125
.L255:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L123
.L254:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L122
.L253:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L121
.L252:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L119
.L251:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L117
.L250:
	ldr	r4, .L287
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L116
.L249:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L115
.L201:
	ldr	r4, .L287
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L10
.L241:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L103
.L240:
	ldr	r6, .L287
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L102
.L229:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L84
.L228:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L82
.L231:
	ldr	r6, .L287
	movs	r7, #0
	adds	r8, r8, r6
	adc	r9, r9, r7
	b	.L88
.L230:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L86
.L233:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L91
.L232:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L90
.L239:
	ldr	r4, .L287
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L100
.L238:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L99
.L237:
	ldr	r4, .L287
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L97
.L236:
	ldr	r4, .L287
	movs	r5, #0
	adds	r10, r10, r4
	adc	fp, fp, r5
	b	.L96
.L235:
	ldr	r4, .L287
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L94
.L234:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L93
.L221:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L68
.L220:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L66
.L223:
	ldr	r4, .L287
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L72
.L222:
	ldr	r4, .L287
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L70
.L225:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L76
.L224:
	ldr	r8, .L287
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L74
.L227:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L80
.L226:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L78
.L244:
	ldr	r8, .L287
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L108
.L243:
	ldr	r6, .L287
	movs	r7, #0
	adds	r8, r8, r6
	adc	r9, r9, r7
	b	.L106
.L288:
	.align	2
.L287:
	.word	8388607
.L242:
	ldr	r4, .L289
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L105
.L248:
	ldr	r4, .L289
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L113
.L247:
	ldr	r6, .L289
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L111
.L246:
	ldr	r4, .L289
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L110
.L245:
	ldr	r6, .L289
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L109
.L207:
	ldr	r8, .L289
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L39
.L206:
	ldr	r8, .L289
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L37
.L205:
	ldr	r8, .L289
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L35
.L204:
	ldr	r8, .L289
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L33
.L203:
	ldr	r8, .L289
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L31
.L202:
	ldr	r6, .L289
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L13
.L261:
	sub	r1, r4, #6815744
	cmp	r1, #0
	str	r1, [r0, #4]
	ble	.L262
	adds	r6, r6, #36
	mov	r7, r1
	str	r6, [r0]
	b	.L17
.L262:
	sub	r5, r4, #6291456
	cmp	r5, #0
	str	r5, [r0, #4]
	ble	.L263
	adds	r6, r6, #35
	mov	r4, r5
	str	r6, [r0]
	b	.L18
.L263:
	sub	r5, r4, #5767168
	cmp	r5, #0
	str	r5, [r0, #4]
	ble	.L264
	adds	r6, r6, #34
	str	r6, [r0]
	b	.L19
.L264:
	sub	r1, r4, #5242880
	cmp	r1, #0
	str	r1, [r0, #4]
	ble	.L265
	adds	r6, r6, #33
	mov	r7, r1
	str	r6, [r0]
	b	.L20
.L265:
	sub	r1, r4, #4718592
	cmp	r1, #0
	str	r1, [r0, #4]
	ble	.L266
	adds	r6, r6, #32
	str	r6, [r0]
	b	.L21
.L266:
	sub	r1, r4, #4194304
	cmp	r1, #0
	str	r1, [r0, #4]
	ble	.L267
	adds	r6, r6, #31
	str	r6, [r0]
	b	.L22
.L267:
	sub	r5, r4, #3670016
	cmp	r5, #0
	str	r5, [r0, #4]
	ble	.L268
	adds	r6, r6, #30
	str	r6, [r0]
	b	.L23
.L268:
	sub	r1, r4, #3145728
	cmp	r1, #0
	str	r1, [r0, #4]
	ble	.L269
	adds	r6, r6, #29
	mov	r4, r1
	str	r6, [r0]
	b	.L24
.L269:
	sub	r1, r4, #2621440
	cmp	r1, #0
	str	r1, [r0, #4]
	ble	.L270
	adds	r6, r6, #28
	str	r6, [r0]
	b	.L25
.L270:
	sub	r5, r4, #2097152
	cmp	r5, #0
	str	r5, [r0, #4]
	ble	.L271
	adds	r6, r6, #27
	str	r6, [r0]
	b	.L26
.L271:
	sub	r5, r4, #1572864
	cmp	r5, #0
	str	r5, [r0, #4]
	ble	.L272
	adds	r6, r6, #26
	mov	r4, r5
	str	r6, [r0]
	b	.L27
.L272:
	sub	r5, r4, #1048576
	cmp	r5, #0
	str	r5, [r0, #4]
	ble	.L273
	adds	r6, r6, #25
	mov	r1, r5
	str	r6, [r0]
	b	.L28
.L273:
	sub	r5, r4, #524288
	cmp	r5, #0
	str	r5, [r0, #4]
	ble	.L274
	adds	r6, r6, #24
	str	r6, [r0]
	b	.L29
.L274:
	str	r4, [r0, #4]
	cbz	r4, .L275
	adds	r6, r6, #23
	str	r6, [r0]
	b	.L30
.L275:
	adds	r6, r6, #22
	mov	r1, #524288
	str	r6, [r0]
	b	.L61
.L290:
	.align	2
.L289:
	.word	8388607
	.size	OEM1_4_spn, .-OEM1_4_spn
	.align	2
	.global	dance_ir_h_l_spn
	.thumb
	.thumb_func
	.type	dance_ir_h_l_spn, %function
dance_ir_h_l_spn:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	ldr	r3, [r0, #80]
	sub	sp, sp, #36
	ldr	r4, .L491
	smull	r4, r5, r3, r4
	cmp	r4, #0
	sbcs	r2, r5, #0
	blt	.L429
.L292:
	lsrs	r1, r4, #23
	mvn	r6, #-16777216
	ldr	r2, [r0, #4]
	orr	r1, r1, r5, lsl #9
	ldr	r4, .L491+4
	cmp	r1, #-16777216
	smull	r4, r5, r3, r4
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r6
	ite	le
	addle	r2, r2, r1
	addgt	r2, r2, r6
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r6
	it	ge
	movge	r2, r6
	cmp	r4, #0
	str	r2, [r0, #152]
	sbcs	r2, r5, #0
	blt	.L430
.L293:
	lsrs	r2, r4, #23
	mvn	r4, #-16777216
	ldr	r1, .L491+8
	orr	r2, r2, r5, lsl #9
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r4
	it	ge
	movge	r2, r4
	add	r1, r1, r2
	cmp	r1, #16777216
	bge	.L390
	ldr	r4, .L491
	smull	r4, r5, r1, r4
	cmp	r4, #0
	sbcs	r2, r5, #0
	blt	.L431
.L295:
	lsrs	r2, r4, #23
	asrs	r7, r5, #23
	orr	r6, r2, r5, lsl #9
	cmp	r6, #16777216
	sbcs	r2, r7, #0
	bge	.L391
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L294:
	ssat	r3, #25, r3
	str	r6, [r0, #148]
	sub	r2, r3, #4194304
	str	r3, [r0, #4]
	str	r3, [r0, #144]
	cmp	r2, #-16777216
	blt	.L296
	cmp	r2, #0
	str	r2, [r0, #4]
	ble	.L296
	ldr	r3, [r0]
	adds	r3, r3, #2
	str	r3, [r0]
.L297:
	ldr	r3, [r0, #84]
	ssat	r3, #25, r3
	asrs	r7, r3, #31
	lsls	r4, r3, #22
	lsls	r2, r7, #22
	cmp	r4, #0
	orr	r5, r2, r3, lsr #10
	sbcs	r3, r5, #0
	blt	.L432
.L298:
	lsrs	r3, r4, #23
	orr	r3, r3, r5, lsl #9
	sub	r3, r3, #4194304
	vmov	s15, r3	@ int
	str	r3, [r0, #4]
	vcvt.f32.s32	s15, s15
	vcmpe.f32	s15, #0
	vmrs	APSR_nzcv, FPSCR
	blt	.L433
	ldr	r6, .L491+12
.L299:
	ldr	r1, [r0, #88]
	str	r6, [r0, #188]
	ssat	r7, #25, r1
	asrs	r3, r7, #31
	mov	r2, r7
	lsls	r4, r7, #22
	lsl	lr, r3, #22
	cmp	r4, #0
	orr	r5, lr, r7, lsr #10
	sbcs	r7, r5, #0
	blt	.L434
.L300:
	lsrs	r4, r4, #23
	orr	r4, r4, r5, lsl #9
	sub	r4, r4, #4194304
	vmov	s15, r4	@ int
	str	r4, [r0, #4]
	vcvt.f32.s32	s15, s15
	vcmpe.f32	s15, #0
	vmrs	APSR_nzcv, FPSCR
	blt	.L435
	str	r6, [sp]
	movs	r7, #0
	ldr	r6, .L491+12
	ldr	r5, .L491+12
	strd	r6, [sp, #8]
.L301:
	ldr	r4, [r0, #84]
	ldr	r6, .L491+4
	str	r5, [r0, #192]
	smull	r6, r7, r4, r6
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L436
.L302:
	lsrs	r5, r6, #23
	mvn	r6, #-16777216
	ldr	r4, .L491+8
	orr	r5, r5, r7, lsl #9
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r6
	it	ge
	movge	r5, r6
	add	r4, r4, r5
	cmp	r4, r6
	it	ge
	movge	r4, r6
	smull	r8, r9, r4, r4
	asrs	r5, r4, #31
	cmp	r8, #0
	sbcs	r6, r9, #0
	blt	.L437
.L303:
	lsr	lr, r8, #23
	mvn	r10, #-16777216
	mov	fp, #0
	asr	r7, r9, #23
	orr	r6, lr, r9, lsl #9
	cmp	r10, r6
	sbcs	lr, fp, r7
	blt	.L304
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	mov	r10, r6
	asr	fp, r6, #31
.L304:
	mul	r6, r4, fp
	umull	r8, r9, r4, r10
	mla	r10, r10, r5, r6
	mov	r4, r8
	add	r9, r9, r10
	cmp	r4, #0
	mov	r5, r9
	sbcs	r6, r9, #0
	blt	.L438
.L305:
	lsrs	r4, r4, #23
	mvn	r8, #-16777216
	mov	r9, #0
	asrs	r7, r5, #23
	orr	r6, r4, r5, lsl #9
	cmp	r8, r6
	sbcs	r4, r9, r7
	blt	.L395
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	mov	r4, r6
	asrs	r5, r6, #31
	strd	r4, [sp, #16]
.L306:
	mul	r3, r1, r3
	asrs	r5, r1, #31
	str	r6, [r0, #216]
	mla	r4, r2, r5, r3
	umull	r2, r3, r1, r2
	add	r3, r3, r4
	cmp	r2, #0
	sbcs	r4, r3, #0
	blt	.L439
.L307:
	lsrs	r2, r2, #23
	orr	r2, r2, r3, lsl #9
	ssat	r2, #25, r2
	smull	r2, r3, r2, r1
	cmp	r2, #0
	sbcs	r4, r3, #0
	blt	.L440
.L308:
	lsr	r8, r2, #23
	orr	r8, r8, r3, lsl #9
	ssat	r8, #25, r8
	smull	r8, r9, r8, r1
	cmp	r8, #0
	sbcs	r3, r9, #0
	blt	.L441
.L309:
	ldr	r10, [r0, #96]
	lsr	r4, r8, #23
	mov	r6, r10
	orr	r1, r4, r9, lsl #9
	lsl	r2, r10, #21
	asrs	r7, r6, #31
	mov	r4, r1
	cmp	r2, #0
	lsl	r1, r7, #21
	ssat	r4, #25, r4
	orr	r3, r1, r10, lsr #11
	str	r4, [sp, #24]
	str	r4, [r0, #220]
	sbcs	r1, r3, #0
	blt	.L442
.L310:
	lsrs	r2, r2, #23
	asr	r9, r3, #23
	orr	r8, r2, r3, lsl #9
	cmp	r8, #16777216
	sbcs	r3, r9, #0
	bge	.L396
	cmp	r8, #-16777216
	mov	r1, r8
	it	lt
	movlt	r1, #-16777216
.L311:
	ldr	fp, [r0, #100]
	mov	r4, fp
	lsl	r2, fp, #21
	asrs	r5, r4, #31
	cmp	r2, #0
	lsl	r4, r5, #21
	orr	r3, r4, fp, lsr #11
	sbcs	r4, r3, #0
	blt	.L443
.L312:
	lsrs	r2, r2, #23
	asr	r9, r3, #23
	orr	r8, r2, r3, lsl #9
	cmp	r8, #16777216
	sbcs	r3, r9, #0
	bge	.L397
	cmp	r8, #-16777216
	it	lt
	movlt	r8, #-16777216
.L313:
	add	r8, r8, r1
	ldr	r3, [r0, #148]
	ssat	r8, #25, r8
	smull	r8, r9, r8, r3
	cmp	r8, #0
	sbcs	r3, r9, #0
	blt	.L444
.L314:
	ldr	r3, .L491+16
	lsr	r1, r8, #23
	mvn	r5, #-16777216
	ldr	r6, .L491+20
	ldr	r3, [r0, r3]
	orr	r1, r1, r9, lsl #9
	add	r2, r3, #202
	cmp	r1, #-16777216
	ubfx	r2, r2, #0, #15
	it	lt
	movlt	r1, #-16777216
	add	r2, r0, r2, lsl #2
	cmp	r1, r5
	ldr	r4, [r2, #272]
	it	ge
	movge	r1, r5
	cmp	r4, #-16777216
	mov	r2, r4
	smull	r6, r7, r4, r6
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r5
	it	ge
	movge	r2, r5
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L445
.L315:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L398
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L316:
	add	r1, r1, r4
	ubfx	r5, r3, #0, #15
	ldr	r4, .L491+24
	ssat	r1, #25, r1
	add	r5, r0, r5, lsl #2
	str	r1, [r5, #272]
	smull	r4, r5, r1, r4
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L446
	b	.L492
.L493:
	.align	2
.L491:
	.word	16768768
	.word	-8388608
	.word	8380160
	.word	8388607
	.word	131344
	.word	5029888
	.word	-5038080
.L492:
.L317:
	lsrs	r1, r4, #23
	add	lr, r3, #744
	mvn	r6, #-16777216
	ldr	r7, .L494
	ubfx	lr, lr, #0, #15
	orr	r1, r1, r5, lsl #9
	add	lr, r0, lr, lsl #2
	add	r1, r1, r2
	ldr	r4, [lr, #272]
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r4, #-16777216
	mov	r2, r4
	smull	r8, r9, r4, r7
	it	lt
	movlt	r2, #-16777216
	cmp	r1, r6
	it	ge
	movge	r1, r6
	cmp	r2, r6
	it	ge
	movge	r2, r6
	cmp	r8, #0
	sbcs	r4, r9, #0
	blt	.L447
.L318:
	lsr	r6, r8, #23
	asr	r5, r9, #23
	orr	r4, r6, r9, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L399
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L319:
	add	r1, r1, r4
	ldr	r6, .L494+4
	add	r4, r3, #203
	ssat	r1, #25, r1
	ubfx	r4, r4, #0, #15
	smull	r6, r7, r1, r6
	add	r4, r0, r4, lsl #2
	cmp	r6, #0
	str	r1, [r4, #272]
	sbcs	r1, r7, #0
	blt	.L448
.L320:
	lsrs	r1, r6, #23
	addw	lr, r3, #1902
	mvn	r4, #-16777216
	ldr	r5, .L494
	ubfx	lr, lr, #0, #15
	orr	r1, r1, r7, lsl #9
	add	lr, r0, lr, lsl #2
	add	r1, r1, r2
	ldr	r6, [lr, #272]
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r6, #-16777216
	mov	r2, r6
	smull	r8, r9, r6, r5
	it	lt
	movlt	r2, #-16777216
	cmp	r1, r4
	it	ge
	movge	r1, r4
	cmp	r2, r4
	it	ge
	movge	r2, r4
	cmp	r8, #0
	sbcs	r4, r9, #0
	blt	.L449
.L321:
	lsr	r4, r8, #23
	asr	r7, r9, #23
	orr	r6, r4, r9, lsl #9
	cmp	r6, #16777216
	sbcs	r4, r7, #0
	bge	.L400
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L322:
	add	r1, r1, r6
	addw	r4, r3, #745
	ldr	r5, .L494+4
	ssat	r1, #25, r1
	ubfx	r4, r4, #0, #15
	smull	r8, r9, r1, r5
	add	r4, r0, r4, lsl #2
	cmp	r8, #0
	str	r1, [r4, #272]
	sbcs	r1, r9, #0
	blt	.L450
.L323:
	addw	r7, r3, #3806
	lsr	r1, r8, #23
	mvn	r5, #-16777216
	ldr	r6, .L494
	ubfx	r7, r7, #0, #15
	orr	r1, r1, r9, lsl #9
	add	r7, r0, r7, lsl #2
	add	r1, r1, r2
	ldr	r4, [r7, #272]
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r4, #-16777216
	mov	r2, r4
	smull	r6, r7, r4, r6
	it	lt
	movlt	r2, #-16777216
	cmp	r1, r5
	it	ge
	movge	r1, r5
	cmp	r2, r5
	it	ge
	movge	r2, r5
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L451
.L324:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L401
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L325:
	add	r1, r1, r4
	ldr	r6, .L494+4
	addw	r4, r3, #1903
	ssat	r1, #25, r1
	ubfx	r4, r4, #0, #15
	smull	r6, r7, r1, r6
	add	r4, r0, r4, lsl #2
	cmp	r6, #0
	str	r1, [r4, #272]
	sbcs	r1, r7, #0
	blt	.L452
.L326:
	lsrs	r4, r6, #23
	movw	r1, #26831
	mvn	r5, #-16777216
	ldr	lr, [r0, #144]
	orr	r4, r4, r7, lsl #9
	add	r1, r1, r3
	movs	r6, #0
	add	r2, r2, r4
	ubfx	r1, r1, #0, #15
	str	r6, [r0, #4]
	cmp	r2, #-16777216
	add	r1, r0, r1, lsl #2
	it	lt
	movlt	r2, #-16777216
	ldr	r4, [r1, #272]
	cmp	r2, r5
	it	ge
	movge	r2, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	str	r2, [r0, #196]
	cmp	r4, r5
	it	ge
	movge	r4, r5
	smull	r4, r5, r4, lr
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L453
.L327:
	lsr	ip, r4, #23
	movw	r1, #6011
	mvn	r4, #-16777216
	ldr	r7, .L494
	orr	ip, ip, r5, lsl #9
	add	r1, r1, r3
	cmp	ip, #-16777216
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	ip, #-16777216
	add	r1, r0, r1, lsl #2
	cmp	ip, r4
	ite	le
	addle	ip, r2, ip
	addgt	ip, r2, r4
	ldr	r5, [r1, #272]
	cmp	ip, #-16777216
	mov	r1, r5
	smull	r8, r9, r5, r7
	it	lt
	movlt	ip, #-16777216
	cmp	r5, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	ip, r4
	it	ge
	movge	ip, r4
	cmp	r1, r4
	it	ge
	movge	r1, r4
	cmp	r8, #0
	sbcs	r4, r9, #0
	blt	.L454
.L328:
	lsr	r4, r8, #23
	asr	r7, r9, #23
	orr	r6, r4, r9, lsl #9
	cmp	r6, #16777216
	sbcs	r4, r7, #0
	bge	.L402
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L329:
	add	r6, r6, ip
	addw	r4, r3, #3807
	ldr	r5, .L494+4
	ssat	r6, #25, r6
	ubfx	r4, r4, #0, #15
	smull	r8, r9, r6, r5
	add	r4, r0, r4, lsl #2
	cmp	r8, #0
	str	r6, [r4, #272]
	sbcs	r4, r9, #0
	blt	.L455
.L330:
	lsr	ip, r8, #23
	movw	r7, #9313
	mvn	r5, #-16777216
	ldr	r6, .L494
	orr	ip, ip, r9, lsl #9
	add	r7, r7, r3
	add	ip, ip, r1
	ubfx	r7, r7, #0, #15
	cmp	ip, #-16777216
	add	r7, r0, r7, lsl #2
	it	lt
	movlt	ip, #-16777216
	ldr	r4, [r7, #272]
	cmp	ip, r5
	mov	r1, r4
	smull	r6, r7, r4, r6
	it	ge
	movge	ip, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r5
	it	ge
	movge	r1, r5
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L456
.L331:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L403
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L332:
	add	r4, r4, ip
	movw	r5, #6012
	ldr	r6, .L494+4
	movw	ip, #9314
	ssat	r4, #25, r4
	add	r5, r5, r3
	add	ip, ip, r3
	smull	r6, r7, r4, r6
	ubfx	r5, r5, #0, #15
	ubfx	ip, ip, #0, #15
	add	r5, r0, r5, lsl #2
	cmp	r6, #0
	add	ip, r0, ip, lsl #2
	str	r4, [r5, #272]
	sbcs	r4, r7, #0
	blt	.L457
.L333:
	lsrs	r4, r6, #23
	movw	r5, #13770
	mvn	r6, #-16777216
	orr	r4, r4, r7, lsl #9
	add	r5, r5, r3
	add	r1, r1, r4
	ubfx	r5, r5, #0, #15
	cmp	r1, #-16777216
	add	r5, r0, r5, lsl #2
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r6
	it	ge
	movge	r1, r6
	str	r1, [ip, #272]
	ldr	r4, [r5, #272]
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r6
	it	ge
	movge	r4, r6
	smull	r4, r5, r4, lr
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L458
.L334:
	lsrs	r6, r4, #23
	movw	r1, #17303
	mvn	r4, #-16777216
	ldr	r7, .L494
	orr	r6, r6, r5, lsl #9
	add	r1, r1, r3
	cmp	r6, #-16777216
	ubfx	r1, r1, #0, #15
	it	lt
	movlt	r6, #-16777216
	b	.L495
.L496:
	.align	2
.L494:
	.word	5029888
	.word	-5038080
.L495:
	add	r1, r0, r1, lsl #2
	cmp	r6, r4
	ite	le
	addle	r2, r2, r6
	addgt	r2, r2, r4
	ldr	r5, [r1, #272]
	cmp	r2, #-16777216
	mov	r1, r5
	smull	r8, r9, r5, r7
	it	lt
	movlt	r2, #-16777216
	cmp	r5, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r2, r4
	it	ge
	movge	r2, r4
	cmp	r1, r4
	it	ge
	movge	r1, r4
	cmp	r8, #0
	sbcs	r4, r9, #0
	blt	.L459
.L335:
	lsr	r4, r8, #23
	asr	r7, r9, #23
	orr	r6, r4, r9, lsl #9
	cmp	r6, #16777216
	sbcs	r4, r7, #0
	bge	.L404
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L336:
	movw	r4, #13771
	add	r2, r2, r6
	ldr	r5, .L497
	ssat	r2, #25, r2
	add	r4, r4, r3
	smull	r8, r9, r2, r5
	ubfx	r4, r4, #0, #15
	cmp	r8, #0
	add	r4, r0, r4, lsl #2
	str	r2, [r4, #272]
	sbcs	r2, r9, #0
	blt	.L460
.L337:
	lsr	r4, r8, #23
	movw	r2, #20505
	mvn	r5, #-16777216
	ldr	r6, .L497+4
	orr	r4, r4, r9, lsl #9
	add	r2, r2, r3
	add	r1, r1, r4
	ubfx	r2, r2, #0, #15
	cmp	r1, #-16777216
	add	r2, r0, r2, lsl #2
	it	lt
	movlt	r1, #-16777216
	ldr	r4, [r2, #272]
	cmp	r1, r5
	mov	r2, r4
	smull	r6, r7, r4, r6
	it	ge
	movge	r1, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r5
	it	ge
	movge	r2, r5
	cmp	r6, #0
	sbcs	r4, r7, #0
	blt	.L461
.L338:
	lsrs	r6, r6, #23
	asrs	r5, r7, #23
	orr	r4, r6, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r6, r5, #0
	bge	.L405
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L339:
	movw	r5, #17304
	add	r1, r1, r4
	ldr	r6, .L497
	ssat	r1, #25, r1
	add	r5, r5, r3
	smull	r6, r7, r1, r6
	ubfx	r5, r5, #0, #15
	cmp	r6, #0
	add	r5, r0, r5, lsl #2
	str	r1, [r5, #272]
	sbcs	r1, r7, #0
	blt	.L462
.L340:
	lsrs	r4, r6, #23
	movw	r1, #20506
	orr	r4, r4, r7, lsl #9
	add	r1, r1, r3
	ldr	r3, .L497+8
	add	r2, r2, r4
	ubfx	r1, r1, #0, #15
	smull	r8, r9, r10, r3
	add	r1, r0, r1, lsl #2
	ssat	r2, #25, r2
	cmp	r8, #0
	str	r2, [r1, #272]
	sbcs	r1, r9, #0
	ldr	r3, [ip, #272]
	blt	.L463
.L341:
	mov	r6, #12582912
	lsr	r1, r8, #23
	smull	r6, r7, r3, r6
	orr	r1, r1, r9, lsl #9
	cmp	r6, #0
	ssat	r1, #25, r1
	sbcs	r3, r7, #0
	blt	.L464
.L342:
	lsrs	r4, r6, #23
	mvn	r5, #-16777216
	ldr	r3, [r0, #80]
	orr	r4, r4, r7, lsl #9
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	ite	le
	addle	r4, r1, r4
	addgt	r4, r1, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	smull	r4, r5, r4, r3
	cmp	r4, #0
	sbcs	r1, r5, #0
	blt	.L465
.L343:
	cmp	r10, #-16777216
	mvn	r1, #-16777216
	lsr	r6, r4, #23
	ldr	r4, .L497+8
	it	lt
	movlt	r10, #-16777216
	orr	r6, r6, r5, lsl #9
	smull	r4, r5, fp, r4
	cmp	r10, r1
	it	ge
	movge	r10, r1
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r1
	ite	le
	addle	ip, r10, r6
	addgt	ip, r10, r1
	cmp	ip, #-16777216
	it	lt
	movlt	ip, #-16777216
	cmp	ip, r1
	it	ge
	movge	ip, r1
	cmp	r4, #0
	sbcs	r1, r5, #0
	str	ip, [r0, #200]
	blt	.L466
.L344:
	mov	r6, #12582912
	lsrs	r1, r4, #23
	str	r2, [r0, #12]
	smull	r6, r7, r2, r6
	orr	r1, r1, r5, lsl #9
	cmp	r6, #0
	ssat	r1, #25, r1
	sbcs	r2, r7, #0
	blt	.L467
.L345:
	lsrs	r2, r6, #23
	asrs	r5, r7, #23
	orr	r4, r2, r7, lsl #9
	cmp	r4, #16777216
	sbcs	r2, r5, #0
	bge	.L406
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
.L346:
	add	r4, r4, r1
	ssat	r4, #25, r4
	smull	r4, r5, r4, r3
	cmp	r4, #0
	sbcs	r3, r5, #0
	blt	.L468
.L347:
	cmp	fp, #-16777216
	mvn	r1, #-16777216
	lsr	r6, r4, #23
	ldr	r2, [r0, #156]
	it	lt
	movlt	fp, #-16777216
	orr	r6, r6, r5, lsl #9
	mov	r4, r2
	cmp	fp, r1
	ldr	r5, [sp]
	it	ge
	movge	fp, r1
	cmp	r2, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r4, r1
	it	ge
	movge	r4, r1
	cmp	r6, r1
	ite	le
	addle	r3, fp, r6
	addgt	r3, fp, r1
	cmp	r3, #-16777216
	smull	r6, r7, r4, r5
	it	lt
	movlt	r3, #-16777216
	cmp	r3, r1
	it	ge
	movge	r3, r1
	cmp	r6, #0
	sbcs	r1, r7, #0
	str	r3, [r0, #204]
	blt	.L469
.L348:
	ldr	r1, [r0, #160]
	lsrs	r5, r6, #23
	mvn	r6, #-16777216
	orr	r5, r5, r7, lsl #9
	cmp	r1, #-16777216
	ldr	r7, .L497+8
	it	lt
	movlt	r1, #-16777216
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r1, r6
	it	ge
	movge	r1, r6
	cmp	r5, r6
	ite	le
	addle	r1, r1, r5
	addgt	r1, r1, r6
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r6
	it	ge
	movge	r1, r6
	smull	r10, fp, r1, r7
	str	r1, [r0, #160]
	cmp	r10, #0
	sbcs	r1, fp, #0
	blt	.L470
.L349:
	lsr	r5, r10, #23
	ldr	r1, .L497+12
	asr	r7, fp, #23
	orr	r6, r5, fp, lsl #9
	smull	r8, r9, r2, r1
	cmp	r7, #0
	itt	eq
	cmpeq	r6, #16777216
	mvneq	r6, #-16777216
	cmp	r8, #0
	sbcs	r2, r9, #0
	blt	.L471
.L351:
	lsr	r2, r8, #23
	asr	fp, r9, #23
	orr	r10, r2, r9, lsl #9
	cmp	r10, #16777216
	sbcs	r2, fp, #0
	bge	.L408
	cmp	r10, #-16777216
	it	lt
	movlt	r10, #-16777216
.L352:
	add	r6, r6, r10
	mvn	r1, #-16777216
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r1
	ite	le
	addle	r2, ip, r6
	addgt	r2, ip, r1
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r1
	it	ge
	movge	r2, r1
	ldr	r1, [sp]
	smull	r6, r7, r2, r1
	cmp	r6, #0
	sbcs	r1, r7, #0
	blt	.L472
.L353:
	lsrs	r1, r6, #23
	ldr	lr, [r0, #172]
	mvn	r6, #-16777216
	orr	r1, r1, r7, lsl #9
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	lr, #-16777216
	mov	r7, r1
	mov	r1, lr
	it	lt
	movlt	r1, #-16777216
	cmp	r7, r6
	ite	le
	addle	r7, r4, r7
	addgt	r7, r4, r6
	ldr	r4, [sp]
	cmp	r1, r6
	it	ge
	movge	r1, r6
	cmp	r7, #-16777216
	it	lt
	movlt	r7, #-16777216
	smull	r4, r5, r1, r4
	cmp	r6, r7
	it	ge
	movge	r6, r7
	cmp	r4, #0
	str	r6, [r0, #156]
	sbcs	r6, r5, #0
	blt	.L473
.L354:
	lsrs	r6, r4, #23
	ldr	r4, [r0, #176]
	ldr	r7, .L497+8
	orr	r6, r6, r5, lsl #9
	cmp	r4, #-16777216
	mvn	r5, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	b	.L498
.L499:
	.align	2
.L497:
	.word	-5038080
	.word	5029888
	.word	-8388608
	.word	-1677824
.L498:
	cmp	r6, r5
	ite	le
	addle	r4, r4, r6
	addgt	r4, r4, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	smull	r8, r9, r4, r7
	str	r4, [r0, #176]
	cmp	r8, #0
	sbcs	r4, r9, #0
	blt	.L474
.L355:
	lsr	r7, r8, #23
	ldr	r6, .L500
	asr	r5, r9, #23
	orr	r4, r7, r9, lsl #9
	smull	r6, r7, lr, r6
	cmp	r5, #0
	itt	eq
	cmpeq	r4, #16777216
	mvneq	r4, #-16777216
	cmp	r6, #0
	sbcs	r5, r7, #0
	blt	.L475
.L357:
	lsrs	r5, r6, #23
	asr	r9, r7, #23
	orr	r8, r5, r7, lsl #9
	cmp	r8, #16777216
	sbcs	r5, r9, #0
	bge	.L410
	cmp	r8, #-16777216
	it	lt
	movlt	r8, #-16777216
.L358:
	add	r4, r4, r8
	mvn	r5, #-16777216
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	ite	le
	addle	r4, r3, r4
	addgt	r4, r3, r5
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r5
	it	ge
	movge	r4, r5
	ldr	r5, [sp]
	smull	r6, r7, r4, r5
	cmp	r6, #0
	sbcs	r5, r7, #0
	blt	.L476
.L359:
	lsrs	r5, r6, #23
	mvn	r6, #-16777216
	orr	r5, r5, r7, lsl #9
	ldr	r7, .L500+4
	cmp	r5, #-16777216
	smull	r8, r9, r2, r7
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r6
	ite	le
	addle	r5, r1, r5
	addgt	r5, r1, r6
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r6
	it	ge
	movge	r5, r6
	cmp	r8, #0
	sbcs	r1, r9, #0
	str	r5, [r0, #172]
	blt	.L477
.L360:
	asr	r7, r9, #23
	lsr	r1, r8, #23
	cmp	r7, #0
	orr	r6, r1, r9, lsl #9
	itt	eq
	cmpeq	r6, #16777216
	mvneq	r6, #-16777216
	ldrd	r10, [sp, #16]
	add	r6, r6, ip
	mov	r1, fp
	ssat	r6, #25, r6
	asr	r9, r6, #31
	mul	r1, r6, r1
	umull	r6, r7, r6, r10
	mla	r1, r10, r9, r1
	cmp	r6, #0
	add	r7, r7, r1
	sbcs	r1, r7, #0
	blt	.L478
.L362:
	lsrs	r1, r6, #23
	asr	fp, r7, #23
	orr	r10, r1, r7, lsl #9
	cmp	r10, #16777216
	sbcs	r1, fp, #0
	bge	.L412
	cmp	r10, #-16777216
	it	lt
	movlt	r10, #-16777216
.L363:
	ldr	r6, .L500+4
	add	r10, r10, r2
	smull	r6, r7, r4, r6
	ssat	r10, #25, r10
	cmp	r6, #0
	str	r10, [r0, #208]
	sbcs	r2, r7, #0
	blt	.L479
.L364:
	lsrs	r2, r6, #23
	asr	r9, r7, #23
	orr	r8, r2, r7, lsl #9
	cmp	r9, #0
	itt	eq
	cmpeq	r8, #16777216
	mvneq	r8, #-16777216
	add	r8, r8, r3
	ssat	r8, #25, r8
	ldrd	r5, [sp, #16]
	mov	r2, r8
	asrs	r3, r2, #31
	mul	r2, r8, r6
	umull	r8, r9, r8, r5
	mla	r3, r5, r3, r2
	cmp	r8, #0
	add	r9, r9, r3
	sbcs	r3, r9, #0
	blt	.L480
.L366:
	lsr	r1, r8, #23
	asr	r3, r9, #23
	orr	r2, r1, r9, lsl #9
	cmp	r2, #16777216
	sbcs	r1, r3, #0
	bge	.L414
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
.L367:
	ldr	r6, [r0, #164]
	add	r4, r4, r2
	mvn	r3, #-16777216
	cmp	r6, #-16777216
	mov	r5, r6
	it	lt
	movlt	r5, #-16777216
	cmp	r4, #-16777216
	ldrd	r8, [sp, #8]
	it	lt
	movlt	r4, #-16777216
	cmp	r5, r3
	mov	r1, r9
	it	ge
	movge	r5, r3
	cmp	r4, r3
	mul	r1, r5, r1
	it	ge
	movge	r4, r3
	asrs	r3, r5, #31
	str	r4, [sp, #16]
	mla	r1, r8, r3, r1
	umull	r2, r3, r5, r8
	str	r4, [r0, #212]
	add	r3, r3, r1
	cmp	r2, #0
	sbcs	r1, r3, #0
	blt	.L481
.L368:
	lsrs	r2, r2, #23
	asr	r9, r3, #23
	orr	r8, r2, r3, lsl #9
	cmp	r8, #16777216
	sbcs	r3, r9, #0
	bge	.L415
	cmp	r8, #-16777216
	it	lt
	movlt	r8, #-16777216
.L369:
	ldr	r1, [r0, #168]
	mvn	r3, #-16777216
	ldr	r2, .L500+4
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r3
	ite	le
	addle	r1, r8, r1
	addgt	r1, r8, r3
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r3
	it	ge
	movge	r1, r3
	smull	r2, r3, r1, r2
	str	r1, [r0, #168]
	cmp	r2, #0
	sbcs	r4, r3, #0
	blt	.L482
.L370:
	lsrs	r7, r2, #23
	ldr	r2, .L500
	orr	r7, r7, r3, lsl #9
	asrs	r3, r3, #23
	str	r7, [sp]
	str	r3, [sp, #4]
	smull	r6, r7, r6, r2
	ldrd	r3, [sp]
	cmp	r4, #0
	mov	fp, r3
	itet	eq
	cmpeq	r3, #16777216
	movne	lr, r3
	mvneq	lr, #-16777216
	cmp	r6, #0
	sbcs	r3, r7, #0
	blt	.L483
.L372:
	lsrs	r6, r6, #23
	asrs	r3, r7, #23
	orr	r2, r6, r7, lsl #9
	cmp	r2, #16777216
	sbcs	r4, r3, #0
	bge	.L417
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
.L373:
	add	r2, r2, lr
	mvn	r3, #-16777216
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ite	le
	addle	r2, r10, r2
	addgt	r2, r10, r3
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ldrd	r8, [sp, #8]
	it	ge
	movge	r2, r3
	asrs	r7, r2, #31
	mul	r6, r2, r9
	umull	r2, r3, r2, r8
	mla	r6, r8, r7, r6
	cmp	r2, #0
	add	r3, r3, r6
	sbcs	r4, r3, #0
	blt	.L484
.L374:
	lsrs	r2, r2, #23
	asrs	r7, r3, #23
	orr	r6, r2, r3, lsl #9
	cmp	r6, #16777216
	sbcs	r3, r7, #0
	bge	.L418
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L375:
	ldr	lr, [r0, #180]
	add	r5, r5, r6
	mvn	r3, #-16777216
	cmp	lr, #-16777216
	mov	ip, lr
	it	lt
	movlt	ip, #-16777216
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	ip, r3
	it	ge
	movge	ip, r3
	cmp	r5, r3
	ldrd	r6, [sp, #8]
	mov	r2, ip
	it	ge
	movge	r5, r3
	asrs	r3, r2, #31
	mul	r2, ip, r7
	str	r5, [r0, #164]
	mla	r5, r6, r3, r2
	umull	r2, r3, ip, r6
	add	r3, r3, r5
	cmp	r2, #0
	sbcs	r4, r3, #0
	blt	.L485
.L376:
	lsrs	r2, r2, #23
	asrs	r7, r3, #23
	orr	r6, r2, r3, lsl #9
	cmp	r6, #16777216
	sbcs	r3, r7, #0
	bge	.L419
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L377:
	ldr	r5, [r0, #184]
	mvn	r3, #-16777216
	mov	r4, r6
	ldr	r2, .L500+4
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r5, r3
	ite	le
	addle	r4, r4, r5
	addgt	r4, r4, r3
	cmp	r4, #-16777216
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r3
	it	ge
	movge	r4, r3
	smull	r2, r3, r4, r2
	str	r4, [sp, #28]
	cmp	r2, #0
	b	.L501
.L502:
	.align	2
.L500:
	.word	-1677824
	.word	-8388608
.L501:
	str	r4, [r0, #184]
	sbcs	r4, r3, #0
	blt	.L486
.L378:
	asr	r9, r3, #23
	lsrs	r6, r2, #23
	ldr	r2, .L503
	cmp	r9, #0
	orr	r8, r6, r3, lsl #9
	smull	r6, r7, lr, r2
	it	eq
	cmpeq	r8, #16777216
	mov	lr, r8
	ite	ne
	movne	r3, r8
	mvneq	r3, #-16777216
	cmp	r6, #0
	mov	r5, r3
	sbcs	r3, r7, #0
	blt	.L487
.L380:
	lsrs	r6, r6, #23
	asrs	r3, r7, #23
	orr	r2, r6, r7, lsl #9
	cmp	r2, #16777216
	sbcs	r4, r3, #0
	bge	.L421
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
.L381:
	add	r2, r2, r5
	mvn	r3, #-16777216
	ldr	r4, [sp, #16]
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ite	le
	addle	r4, r4, r2
	addgt	r4, r4, r3
	cmp	r4, #-16777216
	mov	r2, r4
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	ldrd	r4, [sp, #8]
	it	ge
	movge	r2, r3
	asrs	r7, r2, #31
	mul	r6, r2, r5
	umull	r2, r3, r2, r4
	mla	r6, r4, r7, r6
	cmp	r2, #0
	add	r3, r3, r6
	sbcs	r4, r3, #0
	blt	.L488
.L382:
	lsrs	r2, r2, #23
	asrs	r7, r3, #23
	orr	r6, r2, r3, lsl #9
	cmp	r6, #16777216
	sbcs	r3, r7, #0
	bge	.L422
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L383:
	add	r6, r6, ip
	mvn	r3, #-16777216
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	ldrd	r4, [sp]
	cmp	r6, r3
	it	ge
	movge	r6, r3
	cmp	r5, #0
	it	eq
	cmpeq	r4, #16777216
	str	r6, [r0, #180]
	it	eq
	moveq	fp, r3
	add	r2, fp, r10
	cmp	r2, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r2, r3
	it	ge
	movge	r2, r3
	ldr	r3, [sp, #24]
	smull	r2, r3, r2, r3
	cmp	r2, #0
	sbcs	r4, r3, #0
	blt	.L489
.L385:
	lsrs	r2, r2, #23
	asrs	r7, r3, #23
	orr	r6, r2, r3, lsl #9
	cmp	r6, #16777216
	sbcs	r3, r7, #0
	bge	.L424
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L386:
	add	r1, r1, r6
	mvn	r3, #-16777216
	ldr	r2, [sp, #16]
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r3
	it	ge
	movge	r1, r3
	cmp	r9, #0
	it	eq
	cmpeq	r8, #16777216
	str	r1, [r0, #104]
	it	eq
	moveq	lr, r3
	add	r2, r2, lr
	cmp	r2, #-16777216
	mov	r4, r2
	it	lt
	movlt	r4, #-16777216
	cmp	r4, r3
	it	ge
	movge	r4, r3
	ldr	r3, [sp, #24]
	smull	r2, r3, r4, r3
	cmp	r2, #0
	sbcs	r1, r3, #0
	blt	.L490
.L388:
	lsrs	r2, r2, #23
	asrs	r7, r3, #23
	orr	r6, r2, r3, lsl #9
	cmp	r6, #16777216
	sbcs	r3, r7, #0
	bge	.L426
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
.L389:
	ldr	r3, [sp, #28]
	add	r3, r3, r6
	ssat	r5, #25, r3
	str	r5, [r0, #108]
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L296:
	mov	r3, #4194304
	str	r3, [r0, #144]
	b	.L297
.L426:
	mvn	r6, #-16777216
	b	.L389
.L424:
	mvn	r6, #-16777216
	b	.L386
.L422:
	mvn	r6, #-16777216
	b	.L383
.L421:
	mvn	r2, #-16777216
	b	.L381
.L419:
	mvn	r6, #-16777216
	b	.L377
.L418:
	mvn	r6, #-16777216
	b	.L375
.L417:
	mvn	r2, #-16777216
	b	.L373
.L415:
	mvn	r8, #-16777216
	b	.L369
.L414:
	mvn	r2, #-16777216
	b	.L367
.L412:
	mvn	r10, #-16777216
	b	.L363
.L410:
	mvn	r8, #-16777216
	b	.L358
.L408:
	mvn	r10, #-16777216
	b	.L352
.L406:
	mvn	r4, #-16777216
	b	.L346
.L405:
	mvn	r4, #-16777216
	b	.L339
.L404:
	mvn	r6, #-16777216
	b	.L336
.L403:
	mvn	r4, #-16777216
	b	.L332
.L402:
	mvn	r6, #-16777216
	b	.L329
.L401:
	mvn	r4, #-16777216
	b	.L325
.L400:
	mvn	r6, #-16777216
	b	.L322
.L399:
	mvn	r4, #-16777216
	b	.L319
.L398:
	mvn	r4, #-16777216
	b	.L316
.L397:
	mvn	r8, #-16777216
	b	.L313
.L396:
	mvn	r1, #-16777216
	b	.L311
.L395:
	strd	r8, [sp, #16]
	mvn	r6, #-16777216
	b	.L306
.L449:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L321
.L448:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L320
.L447:
	ldr	r6, .L503+4
	movs	r7, #0
	adds	r8, r8, r6
	adc	r9, r9, r7
	b	.L318
.L446:
	ldr	r6, .L503+4
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L317
.L443:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r2, r2, r4
	adcs	r3, r3, r5
	b	.L312
.L438:
	ldr	r6, .L503+4
	movs	r7, #0
	adds	r4, r8, r6
	adc	r5, r9, r7
	b	.L305
.L440:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r2, r2, r4
	adcs	r3, r3, r5
	b	.L308
.L439:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r2, r2, r4
	adcs	r3, r3, r5
	b	.L307
.L445:
	ldr	r8, .L503+4
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L315
.L444:
	ldr	r2, .L503+4
	movs	r3, #0
	adds	r8, r8, r2
	adc	r9, r9, r3
	b	.L314
.L442:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r2, r2, r4
	adcs	r3, r3, r5
	b	.L310
.L441:
	ldr	r2, .L503+4
	movs	r3, #0
	adds	r8, r8, r2
	adc	r9, r9, r3
	b	.L309
.L459:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L335
.L456:
	ldr	r8, .L503+4
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L331
.L451:
	ldr	r8, .L503+4
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L324
.L450:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L323
.L455:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L330
.L452:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L326
.L454:
	ldr	r4, .L503+4
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L328
.L504:
	.align	2
.L503:
	.word	-1677824
	.word	8388607
.L453:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L327
.L475:
	ldr	r10, .L505
	mov	fp, #0
	adds	r6, r6, r10
	adc	r7, r7, fp
	b	.L357
.L472:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L353
.L471:
	ldr	r10, .L505
	mov	fp, #0
	adds	r8, r8, r10
	adc	r9, r9, fp
	b	.L351
.L470:
	ldr	r6, .L505
	movs	r7, #0
	adds	r10, r10, r6
	adc	fp, fp, r7
	b	.L349
.L469:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L348
.L468:
	ldr	r2, .L505
	movs	r3, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	b	.L347
.L458:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L334
.L457:
	ldr	r4, .L505
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L333
.L467:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L345
.L462:
	ldr	r4, .L505
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L340
.L461:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L338
.L460:
	ldr	r4, .L505
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L337
.L466:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L344
.L465:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L343
.L464:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L342
.L463:
	ldr	r4, .L505
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L341
.L430:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L293
.L429:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L292
.L490:
	ldr	r6, .L505
	movs	r7, #0
	adds	r2, r2, r6
	adcs	r3, r3, r7
	b	.L388
.L437:
	ldr	r6, .L505
	movs	r7, #0
	adds	r8, r8, r6
	adc	r9, r9, r7
	b	.L303
.L432:
	ldr	r2, .L505
	movs	r3, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	b	.L298
.L436:
	ldr	r4, .L505
	movs	r5, #0
	adds	r6, r6, r4
	adcs	r7, r7, r5
	b	.L302
.L434:
	ldr	r8, .L505
	mov	r9, #0
	adds	r4, r4, r8
	adc	r5, r5, r9
	b	.L300
.L480:
	ldr	r2, .L505
	movs	r3, #0
	adds	r8, r8, r2
	adc	r9, r9, r3
	b	.L366
.L479:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L364
.L474:
	ldr	r4, .L505
	movs	r5, #0
	adds	r8, r8, r4
	adc	r9, r9, r5
	b	.L355
.L473:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L354
.L481:
	ldr	r8, .L505
	mov	r9, #0
	adds	r2, r2, r8
	adc	r3, r3, r9
	b	.L368
.L478:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L362
.L477:
	ldr	r6, .L505
	movs	r7, #0
	adds	r8, r8, r6
	adc	r9, r9, r7
	b	.L360
.L476:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L359
.L489:
	ldr	r6, .L505
	movs	r7, #0
	adds	r2, r2, r6
	adcs	r3, r3, r7
	b	.L385
.L488:
	ldr	r6, .L505
	movs	r7, #0
	adds	r2, r2, r6
	adcs	r3, r3, r7
	b	.L382
.L487:
	ldr	r2, .L505
	movs	r3, #0
	adds	r6, r6, r2
	adcs	r7, r7, r3
	b	.L380
.L486:
	ldr	r6, .L505
	movs	r7, #0
	adds	r2, r2, r6
	adcs	r3, r3, r7
	b	.L378
.L483:
	ldr	r8, .L505
	mov	r9, #0
	adds	r6, r6, r8
	adc	r7, r7, r9
	b	.L372
.L482:
	ldr	r8, .L505
	mov	r9, #0
	adds	r2, r2, r8
	adc	r3, r3, r9
	b	.L370
.L485:
	ldr	r6, .L505
	movs	r7, #0
	adds	r2, r2, r6
	adcs	r3, r3, r7
	b	.L376
.L484:
	ldr	r6, .L505
	movs	r7, #0
	adds	r2, r2, r6
	adcs	r3, r3, r7
	b	.L374
.L431:
	ldr	r6, .L505
	movs	r7, #0
	adds	r4, r4, r6
	adcs	r5, r5, r7
	b	.L295
.L435:
	vldr.32	s14, .L505+4
	vmov.f32	s0, #1.6e+1
	str	r0, [sp]
	mvn	r4, #-16777216
	vmul.f32	s15, s15, s14
	vmul.f32	s0, s15, s0
	bl	exp2f
	vmov.f32	s15, s0
	ldr	r0, [sp]
	vcvt.s32.f32	s15, s15, #23
	ldr	r1, [r0, #88]
	ldr	r3, [r0, #188]
	mov	r2, r1
	vmov	r5, s15	@ int
	str	r3, [sp]
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	cmp	r1, #-16777216
	it	lt
	movlt	r2, #-16777216
	cmp	r5, r4
	it	ge
	movge	r5, r4
	cmp	r2, r4
	it	ge
	movge	r2, r4
	cmp	r5, #-16777216
	it	lt
	movlt	r5, #-16777216
	asrs	r3, r2, #31
	cmp	r5, r4
	it	ge
	movge	r5, r4
	mov	r6, r5
	asrs	r7, r5, #31
	strd	r6, [sp, #8]
	b	.L301
.L433:
	vldr.32	s14, .L505+4
	vmov.f32	s0, #1.6e+1
	str	r0, [sp]
	vmul.f32	s15, s15, s14
	vmul.f32	s0, s15, s0
	bl	exp2f
	vmov.f32	s15, s0
	mvn	r3, #-16777216
	ldr	r0, [sp]
	vcvt.s32.f32	s15, s15, #23
	vmov	r6, s15	@ int
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r3
	it	ge
	movge	r6, r3
	cmp	r6, #-16777216
	it	lt
	movlt	r6, #-16777216
	cmp	r6, r3
	it	ge
	movge	r6, r3
	b	.L299
.L391:
	mvn	r6, #-16777216
	b	.L294
.L390:
	mov	r6, r4
	b	.L294
.L506:
	.align	2
.L505:
	.word	8388607
	.word	872415232
	.size	dance_ir_h_l_spn, .-dance_ir_h_l_spn
	.section	.text.fv1_init,"ax",%progbits
	.align	2
	.global	fv1_init
	.thumb
	.thumb_func
	.type	fv1_init, %function
fv1_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	ldr	r0, .L509
	movs	r5, #0
	bl	_Znwj
	mov	r4, r0
	mov	r1, r5
	mov	r2, #131072
	str	r5, [r4]
	add	r0, r0, #272
	str	r5, [r4, #4]
	add	r8, r4, #20
	str	r5, [r4, #8]
	add	r7, r4, #32
	str	r5, [r4, #12]
	str	r5, [r4, #16]
	str	r5, [r4, #20]
	str	r5, [r4, #24]
	str	r5, [r4, #28]
	str	r5, [r4, #32]
	str	r5, [r4, #36]
	str	r5, [r4, #40]
	str	r5, [r4, #44]
	str	r5, [r4, #48]
	str	r5, [r4, #52]
	str	r5, [r4, #56]
	str	r5, [r4, #60]
	str	r5, [r4, #64]
	str	r5, [r4, #68]
	str	r5, [r4, #72]
	str	r5, [r4, #76]
	str	r5, [r4, #80]
	str	r5, [r4, #84]
	str	r5, [r4, #88]
	str	r5, [r4, #92]
	str	r5, [r4, #96]
	str	r5, [r4, #100]
	str	r5, [r4, #104]
	str	r5, [r4, #108]
	str	r5, [r4, #112]
	str	r5, [r4, #116]
	str	r5, [r4, #120]
	str	r5, [r4, #124]
	str	r5, [r4, #128]
	str	r5, [r4, #132]
	str	r5, [r4, #136]
	str	r5, [r4, #140]
	str	r5, [r4, #144]
	str	r5, [r4, #148]
	str	r5, [r4, #152]
	str	r5, [r4, #156]
	str	r5, [r4, #160]
	str	r5, [r4, #164]
	str	r5, [r4, #168]
	str	r5, [r4, #172]
	str	r5, [r4, #176]
	str	r5, [r4, #180]
	str	r5, [r4, #184]
	str	r5, [r4, #188]
	str	r5, [r4, #192]
	str	r5, [r4, #196]
	str	r5, [r4, #200]
	str	r5, [r4, #204]
	str	r5, [r4, #208]
	str	r5, [r4, #212]
	str	r5, [r4, #216]
	str	r5, [r4, #220]
	str	r5, [r4, #224]
	str	r5, [r4, #228]
	str	r5, [r4, #232]
	str	r5, [r4, #236]
	str	r5, [r4, #240]
	str	r5, [r4, #244]
	str	r5, [r4, #248]
	str	r5, [r4, #252]
	str	r5, [r4, #256]
	str	r5, [r4, #260]
	str	r5, [r4, #264]
	str	r5, [r4, #268]
	bl	memset
	ldr	r1, .L509+4
	ldr	r3, .L509+8
	movs	r2, #1
	str	r5, [r4, r1]
	mvn	r1, #274
	str	r5, [r4, r3]
	sub	r3, r3, #8519680
	ldr	r6, .L509+12
	add	r0, r4, #16
	add	r3, r3, r1
	add	ip, r4, #28
	add	r1, r4, #36
	add	lr, r4, #24
	str	r3, [r4, r6]
	adds	r6, r6, #12
	str	r5, [r4, r6]
	adds	r6, r6, #4
	str	r3, [r4, r6]
	add	r6, r4, #44
	ldr	r3, .L509+16
	str	r5, [r4, r3]
	adds	r3, r3, #12
	str	r5, [r4, r3]
	adds	r3, r3, #12
	strb	r2, [r4, r3]
	adds	r3, r3, #4
	add	r2, r4, #40
	str	r5, [r4, r3]
	subs	r3, r3, #52
	ldr	r5, .L509+20
	str	r8, [r4, r3]
	adds	r3, r3, #4
	str	r0, [r4, r3]
	adds	r3, r3, #12
	mov	r0, r4
	str	ip, [r4, r3]
	adds	r3, r3, #12
	str	lr, [r4, r5]
	adds	r5, r5, #12
	str	r1, [r4, r3]
	adds	r3, r3, #16
	ldr	r1, .L509+24
	str	r7, [r4, r5]
	str	r6, [r4, r1]
	str	r2, [r4, r3]
	pop	{r4, r5, r6, r7, r8, pc}
.L510:
	.align	2
.L509:
	.word	131412
	.word	131344
	.word	131348
	.word	131352
	.word	131380
	.word	131376
	.word	131396
	.size	fv1_init, .-fv1_init
	.section	.text.fv1_free,"ax",%progbits
	.align	2
	.global	fv1_free
	.thumb
	.thumb_func
	.type	fv1_free, %function
fv1_free:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cbz	r0, .L511
	ldr	r1, .L513
	b	_ZdlPvj
.L511:
	bx	lr
.L514:
	.align	2
.L513:
	.word	131412
	.size	fv1_free, .-fv1_free
	.section	.text.fv1_set_fx,"ax",%progbits
	.align	2
	.global	fv1_set_fx
	.thumb
	.thumb_func
	.type	fv1_set_fx, %function
fv1_set_fx:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	cbz	r0, .L521
	movs	r3, #0
	ldr	r2, .L522
	push	{r4, r5, r6, lr}
	ldr	r5, .L522+4
	movs	r6, #1
	mov	r4, r0
	ldr	r5, [r5, r1, lsl #2]
	mov	r1, r3
	strb	r6, [r0, r2]
	movs	r2, #124
	str	r3, [r0, #4]
	str	r3, [r0, #8]
	str	r3, [r0], #144
	bl	memset
	ldr	r3, .L522+8
	str	r5, [r4, r3]
	pop	{r4, r5, r6, pc}
.L521:
	bx	lr
.L523:
	.align	2
.L522:
	.word	131404
	.word	.LANCHOR0
	.word	131408
	.size	fv1_set_fx, .-fv1_set_fx
	.section	.text.fv1_process,"ax",%progbits
	.align	2
	.global	fv1_process
	.thumb
	.thumb_func
	.type	fv1_process, %function
fv1_process:
	@ args = 8, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	cmp	r0, #0
	beq	.L536
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	vpush.64	{d8}
	sub	sp, sp, #68
	ldr	r4, [sp, #116]
	cmp	r4, #0
	beq	.L524
	vldr.32	s16, .L540
	mov	lr, r1
	mvn	r5, #-16777216
	str	r2, [sp, #24]
	vmul.f32	s0, s0, s16
	mov	r4, r0
	vmul.f32	s2, s2, s16
	movs	r0, #0
	vmul.f32	s1, s1, s16
	str	r3, [sp, #20]
	vcvt.s32.f32	s15, s0
	str	r0, [sp, #4]
	ldr	r0, .L540+4
	ldr	r3, .L540+8
	vstr.32	s15, [sp, #32]	@ int
	vcvt.s32.f32	s15, s2
	ldr	r6, [sp, #32]
	ldr	r7, .L540+12
	cmp	r6, #-16777216
	vstr.32	s15, [sp, #36]	@ int
	vcvt.s32.f32	s15, s1
	ldr	fp, .L540+36
	it	lt
	movlt	r6, #-16777216
	ldr	r8, .L540+40
	ldr	r10, .L540+44
	add	r7, r7, r4
	str	r6, [sp, #32]
	add	fp, fp, r4
	ldr	r6, [sp, #36]
	add	r8, r8, r4
	ldr	r2, [sp, #32]
	add	r10, r10, r4
	mov	r1, r6
	cmp	r6, #-16777216
	vmov	r6, s15	@ int
	ldr	r9, .L540+48
	it	lt
	movlt	r1, #-16777216
	str	lr, [sp, #12]
	cmp	r6, #-16777216
	ldr	r6, .L540+16
	str	r1, [sp, #36]
	vmov	r1, s15	@ int
	it	lt
	movlt	r1, #-16777216
	cmp	r2, r5
	add	r6, r6, r4
	add	r9, r9, r4
	it	ge
	movge	r2, r5
	str	r1, [sp, #28]
	ldr	r1, .L540+20
	str	r2, [sp, #32]
	ldr	r2, [sp, #36]
	vldr.32	s17, .L540+24
	cmp	r2, r5
	str	r6, [sp, #60]
	str	r7, [sp, #8]
	it	ge
	movge	r2, r5
	str	r2, [sp, #36]
	ldr	r2, [sp, #28]
	cmp	r2, r5
	it	ge
	movge	r2, r5
	str	r2, [sp, #28]
	adds	r2, r4, r0
	str	r2, [sp, #52]
	adds	r2, r4, r3
	subs	r3, r3, #60
	str	r2, [sp, #48]
	adds	r2, r4, r3
	adds	r3, r3, #40
	adds	r3, r4, r3
	str	r2, [sp, #56]
	str	r3, [sp, #40]
	adds	r3, r4, r1
	str	r3, [sp, #44]
	ldr	r3, [sp, #112]
	str	r3, [sp, #16]
	b	.L531
.L527:
	lsrs	r2, r6, #23
	ldr	r1, [r8]
	orr	r2, r2, r7, lsl #9
	add	r1, r1, r2
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r5
	it	ge
	movge	r1, r5
	smull	r2, r3, lr, r1
	str	r1, [r8]
	cmp	r2, #0
	mov	r7, r3
	mov	r6, r2
	sbcs	r3, r3, #0
	blt	.L537
.L528:
	lsrs	r2, r6, #23
	ldr	r3, [sp, #8]
	orr	r2, r2, r7, lsl #9
	ldr	r3, [r3, #12]
	subs	r0, r0, r2
	ldr	r2, [sp, #8]
	cmp	r0, #-16777216
	it	lt
	movlt	r0, #-16777216
	cmp	r0, r5
	it	ge
	movge	r0, r5
	str	r0, [fp]
	ldr	r0, [r2]
	ldr	r2, [r3]
	asr	lr, r2, #8
	smull	r2, r3, r0, lr
	cmp	r2, #0
	mov	r7, r3
	mov	r6, r2
	sbcs	r3, r3, #0
	blt	.L538
.L529:
	lsrs	r2, r6, #23
	ldr	r1, [r10]
	orr	r2, r2, r7, lsl #9
	add	r1, r1, r2
	cmp	r1, #-16777216
	it	lt
	movlt	r1, #-16777216
	cmp	r1, r5
	it	ge
	movge	r1, r5
	smull	r2, r3, lr, r1
	str	r1, [r10]
	cmp	r2, #0
	mov	r7, r3
	mov	r6, r2
	sbcs	r3, r3, #0
	blt	.L539
.L530:
	lsr	lr, r6, #23
	ldr	r3, [sp, #40]
	ldr	r6, [sp, #44]
	orr	lr, lr, r7, lsl #9
	ldr	r1, [r3]
	ldr	ip, [r6]
	rsb	r0, lr, r0
	ldr	r7, [sp, #60]
	ldr	r6, [sp, #8]
	cmp	r0, #-16777216
	ldr	r3, [r9]
	ldr	r2, [r9, #8]
	it	lt
	movlt	r0, #-16777216
	ldr	lr, [r7, #8]
	cmp	r0, r5
	it	ge
	movge	r0, r5
	str	r0, [r6]
	ldr	r1, [r1]
	ldr	r2, [r2]
	ldr	r6, .L540+28
	asrs	r1, r1, #21
	sub	r2, r3, r2, asr #12
	ldr	r0, [ip]
	asr	r1, r6, r1
	vldr.32	s14, [r4, #104]	@ int
	ldr	r3, [lr]
	asrs	r0, r0, #21
	ands	r1, r1, r2
	vldr.32	s15, [r4, #108]	@ int
	vcvt.f32.s32	s14, s14
	asr	r0, r6, r0
	str	r1, [r9]
	vcvt.f32.s32	s15, s15
	ldr	r2, [r7]
	vmul.f32	s14, s14, s17
	sub	r3, r2, r3, asr #12
	vmul.f32	s15, s15, s17
	ldr	r2, [sp, #4]
	ands	r0, r0, r3
	ldr	r3, [sp, #20]
	str	r0, [r7]
	vstmia.32	r3!, {s14}
	str	r3, [sp, #20]
	ldr	r3, [sp, #16]
	vstmia.32	r3!, {s15}
	str	r3, [sp, #16]
	ldr	r3, [sp, #116]
	cmp	r3, r2
	beq	.L524
.L531:
	ldr	r3, [sp, #12]
	mov	r0, r4
	vldmia.32	r3!, {s14}
	str	r3, [sp, #12]
	vmul.f32	s14, s14, s16
	ldr	r3, [sp, #24]
	vldmia.32	r3!, {s15}
	vcvt.s32.f32	s14, s14
	vmul.f32	s15, s15, s16
	str	r3, [sp, #24]
	ldr	r3, [sp, #4]
	vmov	r1, s14	@ int
	vcvt.s32.f32	s15, s15
	adds	r3, r3, #1
	cmp	r1, #-16777216
	str	r3, [sp, #4]
	mov	r3, #0
	vmov	r2, s15	@ int
	it	lt
	movlt	r1, #-16777216
	str	r3, [r4]
	cmp	r2, #-16777216
	ldr	r3, [sp, #32]
	it	lt
	movlt	r2, #-16777216
	str	r3, [r4, #80]
	cmp	r1, r5
	ldr	r3, [sp, #28]
	it	ge
	movge	r1, r5
	str	r3, [r4, #84]
	cmp	r2, r5
	ldr	r3, [sp, #36]
	it	ge
	movge	r2, r5
	str	r3, [r4, #88]
	ldr	r3, [sp, #52]
	ldr	r3, [r3]
	str	r1, [r4, #96]
	str	r2, [r4, #100]
	blx	r3
	ldr	r3, [r4, #4]
	ldr	r0, [sp, #56]
	mov	lr, #0
	cmp	r3, #-16777216
	ldr	r6, [sp, #48]
	ldr	r2, [r0]
	it	lt
	movlt	r3, #-16777216
	ldr	r1, [fp, #12]
	subs	r2, r2, #1
	strb	lr, [r6]
	cmp	r3, r5
	str	r2, [r0]
	it	ge
	movge	r3, r5
	str	r3, [r4, #8]
	movs	r3, #0
	str	r3, [r4, #4]
	ldr	r2, [r1]
	ldr	r0, [fp]
	asr	lr, r2, #8
	smull	r2, r3, r0, lr
	cmp	r2, #0
	mov	r7, r3
	mov	r6, r2
	sbcs	r3, r3, #0
	bge	.L527
	ldr	r2, .L540+32
	movs	r3, #0
	adds	r6, r6, r2
	adcs	r7, r7, r3
	b	.L527
.L539:
	ldr	r2, .L540+32
	movs	r3, #0
	adds	r6, r6, r2
	adcs	r7, r7, r3
	b	.L530
.L538:
	ldr	r2, .L540+32
	movs	r3, #0
	adds	r6, r6, r2
	adcs	r7, r7, r3
	b	.L529
.L537:
	ldr	r2, .L540+32
	movs	r3, #0
	adds	r6, r6, r2
	adcs	r7, r7, r3
	b	.L528
.L524:
	add	sp, sp, #68
	@ sp needed
	vldm	sp!, {d8}
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L536:
	bx	lr
.L541:
	.align	2
.L540:
	.word	1258291200
	.word	131408
	.word	131404
	.word	131364
	.word	131392
	.word	131396
	.word	872415232
	.word	4194303
	.word	8388607
	.word	131348
	.word	131352
	.word	131368
	.word	131380
	.size	fv1_process, .-fv1_process
	.global	programs
	.section	.data.programs,"aw",%progbits
	.align	2
	.set	.LANCHOR0,. + 0
	.type	programs, %object
	.size	programs, 8
programs:
	.word	dance_ir_h_l_spn
	.word	OEM1_4_spn
	.ident	"GCC: (GNU Tools for ARM Embedded Processors) 5.4.1 20160919 (release) [ARM/embedded-5-branch revision 240496]"
