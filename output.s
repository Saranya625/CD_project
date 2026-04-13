	.data
	.align	2

a:	.space	64
b:	.space	64
c:	.space	64
mat_r241:	.word	0
mat_c242:	.word	0
mat_acc243:	.word	0

t_8:	.word	0
t_16:	.word	0
t_24:	.word	0
t_32:	.word	0
t_40:	.word	0
t_48:	.word	0
t_56:	.word	0
t_64:	.word	0
t_72:	.word	0
t_80:	.word	0
t_88:	.word	0
t_96:	.word	0
t_104:	.word	0
t_112:	.word	0
t_120:	.word	0
t_128:	.word	0
t_247:	.word	0
t_248:	.word	0
t_249:	.word	0
t_250:	.word	0
t_251:	.word	0
t_256:	.word	0
t_258:	.word	0
t_262:	.word	0
t_263:	.word	0
t_288:	.word	0
t_299:	.word	0
t_320:	.word	0

.Ls0:	.asciz	"c:"

	.text
	.globl	main
main:
	addi	t0, x0, 0
	la	t1, a
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	addi	t5, x0, 1
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 0
	addi	t1, x0, 1
	la	t2, a
	slli	t3, t0, 2
	addi	t4, t3, 1
	la	t6, t_8
	sw	t5, 0(t6)
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 2
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 0
	addi	t3, x0, 2
	la	t4, a
	slli	t5, t2, 2
	addi	t0, t5, 2
	la	t6, t_16
	sw	t1, 0(t6)
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 3
	addi	a5, t2, 0
	sw	t3, 0(a5)
	addi	t4, x0, 0
	addi	t5, x0, 3
	la	t0, a
	slli	t1, t4, 2
	addi	t2, t1, 3
	la	t6, t_24
	sw	t3, 0(t6)
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	t5, x0, 10
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 1
	la	t1, a
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	la	t6, t_32
	sw	t5, 0(t6)
	addi	t5, x0, 4
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 1
	addi	t1, x0, 1
	la	t2, a
	slli	t3, t0, 2
	addi	t4, t3, 1
	la	t6, t_40
	sw	t5, 0(t6)
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 5
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 1
	addi	t3, x0, 2
	la	t4, a
	slli	t5, t2, 2
	addi	t0, t5, 2
	la	t6, t_48
	sw	t1, 0(t6)
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 6
	addi	a5, t2, 0
	sw	t3, 0(a5)
	addi	t4, x0, 1
	addi	t5, x0, 3
	la	t0, a
	slli	t1, t4, 2
	addi	t2, t1, 3
	la	t6, t_56
	sw	t3, 0(t6)
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	t5, x0, 11
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 2
	la	t1, a
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	la	t6, t_64
	sw	t5, 0(t6)
	addi	t5, x0, 7
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 2
	addi	t1, x0, 1
	la	t2, a
	slli	t3, t0, 2
	addi	t4, t3, 1
	la	t6, t_72
	sw	t5, 0(t6)
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 8
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 2
	addi	t3, x0, 2
	la	t4, a
	slli	t5, t2, 2
	addi	t0, t5, 2
	la	t6, t_80
	sw	t1, 0(t6)
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 9
	addi	a5, t2, 0
	sw	t3, 0(a5)
	addi	t4, x0, 2
	addi	t5, x0, 3
	la	t0, a
	slli	t1, t4, 2
	addi	t2, t1, 3
	la	t6, t_88
	sw	t3, 0(t6)
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	t5, x0, 12
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 3
	la	t1, a
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	la	t6, t_96
	sw	t5, 0(t6)
	addi	t5, x0, 13
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 3
	addi	t1, x0, 1
	la	t2, a
	slli	t3, t0, 2
	addi	t4, t3, 1
	la	t6, t_104
	sw	t5, 0(t6)
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 14
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 3
	addi	t3, x0, 2
	la	t4, a
	slli	t5, t2, 2
	addi	t0, t5, 2
	la	t6, t_112
	sw	t1, 0(t6)
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 15
	addi	a5, t2, 0
	sw	t3, 0(a5)
	addi	t4, x0, 3
	addi	t5, x0, 3
	la	t0, a
	slli	t1, t4, 2
	addi	t2, t1, 3
	la	t6, t_120
	sw	t3, 0(t6)
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	t5, x0, 16
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 0
	la	t1, b
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	addi	a5, t4, 0
	la	t6, t_128
	sw	t5, 0(t6)
	addi	t5, x0, 3
	sw	t5, 0(a5)
	addi	t0, x0, 0
	addi	t1, x0, 1
	la	t2, b
	slli	t3, t0, 2
	addi	t4, t3, 1
	la	t6, t_24
	sw	t5, 0(t6)
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	a5, t0, 0
	addi	t1, x0, 2
	sw	t1, 0(a5)
	addi	t2, x0, 0
	addi	t3, x0, 2
	la	t4, b
	slli	t5, t2, 2
	addi	t0, t5, 2
	la	t6, t_16
	sw	t1, 0(t6)
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	a5, t2, 0
	addi	t3, x0, 1
	sw	t3, 0(a5)
	addi	t4, x0, 0
	addi	t5, x0, 3
	la	t0, b
	slli	t1, t4, 2
	addi	t2, t1, 3
	la	t6, t_8
	sw	t3, 0(t6)
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	a5, t4, 0
	addi	t5, x0, 10
	sw	t5, 0(a5)
	addi	t0, x0, 1
	la	t1, b
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	addi	a5, t4, 0
	addi	t5, x0, 6
	sw	t5, 0(a5)
	addi	t0, x0, 1
	addi	t1, x0, 1
	la	t2, b
	slli	t3, t0, 2
	addi	t4, t3, 1
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	a5, t0, 0
	addi	t1, x0, 5
	sw	t1, 0(a5)
	addi	t2, x0, 1
	addi	t3, x0, 2
	la	t4, b
	slli	t5, t2, 2
	addi	t0, t5, 2
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	a5, t2, 0
	addi	t3, x0, 4
	sw	t3, 0(a5)
	addi	t4, x0, 1
	addi	t5, x0, 3
	la	t0, b
	slli	t1, t4, 2
	addi	t2, t1, 3
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	a5, t4, 0
	addi	t5, x0, 11
	sw	t5, 0(a5)
	addi	t0, x0, 2
	la	t1, b
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	addi	a5, t4, 0
	addi	t5, x0, 9
	sw	t5, 0(a5)
	addi	t0, x0, 2
	addi	t1, x0, 1
	la	t2, b
	slli	t3, t0, 2
	addi	t4, t3, 1
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	a5, t0, 0
	addi	t1, x0, 8
	sw	t1, 0(a5)
	addi	t2, x0, 2
	addi	t3, x0, 2
	la	t4, b
	slli	t5, t2, 2
	addi	t0, t5, 2
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	a5, t2, 0
	addi	t3, x0, 7
	sw	t3, 0(a5)
	addi	t4, x0, 2
	addi	t5, x0, 3
	la	t0, b
	slli	t1, t4, 2
	addi	t2, t1, 3
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	a5, t4, 0
	addi	t5, x0, 12
	sw	t5, 0(a5)
	addi	t0, x0, 3
	la	t1, b
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	addi	a5, t4, 0
	addi	t5, x0, 13
	sw	t5, 0(a5)
	addi	t0, x0, 3
	addi	t1, x0, 1
	la	t2, b
	slli	t3, t0, 2
	addi	t4, t3, 1
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	a5, t0, 0
	addi	t1, x0, 14
	sw	t1, 0(a5)
	addi	t2, x0, 3
	addi	t3, x0, 2
	la	t4, b
	slli	t5, t2, 2
	addi	t0, t5, 2
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	a5, t2, 0
	addi	t3, x0, 15
	sw	t3, 0(a5)
	addi	t4, x0, 3
	addi	t5, x0, 3
	la	t0, b
	slli	t1, t4, 2
	addi	t2, t1, 3
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	a5, t4, 0
	addi	t5, x0, 16
	sw	t5, 0(a5)
	la	t0, mat_r241
	la	t1, mat_c242
	la	t2, mat_acc243
	addi	t3, x0, 0
	addi	t4, x0, 1
	addi	t5, x0, 4
	addi	t0, x0, 4
	addi	t1, x0, 16
	la	t2, a
	la	t6, t_247
	sw	t3, 0(t6)
	la	t3, b
	la	t6, t_248
	sw	t4, 0(t6)
	la	t4, c
	la	t6, t_249
	sw	t5, 0(t6)
	la	t5, mat_r241
	addi	a5, t5, 0
	la	t6, t_250
	sw	t0, 0(t6)
	addi	t0, x0, 0
	sw	t0, 0(a5)
	la	t6, t_247
	sw	t0, 0(t6)
	la	t6, t_251
	sw	t1, 0(t6)
L1:
	la	t6, mat_r241
	lw	t0, 0(t6)
	addi	t1, x0, 4
	slt	t2, t0, t1
	la	t6, t_249
	sw	t1, 0(t6)
	la	t6, t_256
	sw	t2, 0(t6)
	beqz	t2, L2
	la	t0, mat_c242
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, t_247
	sw	t1, 0(t6)
L3:
	la	t6, mat_c242
	lw	t0, 0(t6)
	addi	t1, x0, 4
	slt	t2, t0, t1
	la	t6, t_258
	sw	t2, 0(t6)
	beqz	t2, L4
	la	t0, mat_acc243
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, mat_r241
	lw	t2, 0(t6)
	la	t6, mat_c242
	lw	t3, 0(t6)
	slli	t4, t2, 4
	la	t5, a
	add	t0, t5, t4
	slli	t1, t3, 2
	la	t2, b
	add	t3, t2, t1
	lw	t4, 0(t0)
	lw	t5, 0(t3)
	la	t6, t_262
	sw	t0, 0(t6)
	mul	t0, t4, t5
	la	t6, t_263
	sw	t1, 0(t6)
	la	t6, mat_acc243
	lw	t1, 0(t6)
	add	t2, t1, t0
	la	t3, mat_acc243
	addi	a5, t3, 0
	sw	t2, 0(a5)
	la	t6, t_262
	lw	t4, 0(t6)
	addi	t5, t4, 4
	la	t6, t_263
	lw	t0, 0(t6)
	addi	t1, t0, 16
	la	t2, b
	add	t3, t2, t1
	la	t6, t_262
	sw	t4, 0(t6)
	lw	t4, 0(t5)
	lw	t5, 0(t3)
	la	t6, t_263
	sw	t0, 0(t6)
	mul	t0, t4, t5
	la	t6, mat_acc243
	lw	t1, 0(t6)
	add	t2, t1, t0
	la	t3, mat_acc243
	addi	a5, t3, 0
	sw	t2, 0(a5)
	addi	t4, x0, 2
	slli	t5, t4, 2
	la	t6, t_262
	lw	t0, 0(t6)
	add	t1, t0, t5
	slli	t2, t4, 4
	la	t6, t_263
	lw	t3, 0(t6)
	add	t4, t2, t3
	la	t5, b
	la	t6, t_262
	sw	t0, 0(t6)
	add	t0, t5, t4
	la	t6, t_288
	sw	t1, 0(t6)
	addi	t6, t1, 0
	lw	t1, 0(t6)
	lw	t2, 0(t0)
	la	t6, t_263
	sw	t3, 0(t6)
	mul	t3, t1, t2
	la	t6, mat_acc243
	lw	t4, 0(t6)
	add	t5, t4, t3
	la	t0, mat_acc243
	addi	a5, t0, 0
	sw	t5, 0(a5)
	addi	t1, x0, 3
	slli	t2, t1, 2
	la	t6, t_262
	lw	t3, 0(t6)
	add	t4, t3, t2
	slli	t5, t1, 4
	la	t6, t_263
	lw	t0, 0(t6)
	add	t1, t5, t0
	la	t2, b
	add	t3, t2, t1
	la	t6, t_299
	sw	t4, 0(t6)
	addi	t6, t4, 0
	lw	t4, 0(t6)
	lw	t5, 0(t3)
	mul	t0, t4, t5
	la	t6, mat_acc243
	lw	t1, 0(t6)
	add	t2, t1, t0
	la	t3, mat_acc243
	addi	a5, t3, 0
	sw	t2, 0(a5)
	la	t6, mat_r241
	lw	t4, 0(t6)
	la	t6, mat_c242
	lw	t5, 0(t6)
	slli	t0, t4, 4
	la	t1, c
	add	t2, t1, t0
	slli	t3, t5, 2
	add	t4, t2, t3
	la	t6, mat_acc243
	lw	t5, 0(t6)
	addi	a5, t4, 0
	sw	t5, 0(a5)
	la	t6, mat_c242
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_c242
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L3
L4:
	la	t6, mat_r241
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_r241
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L1
L2:
	la	a0, .Ls0
	addi	a7, x0, 4
	ecall
	addi	t0, x0, 0
	la	t1, c
	slli	t2, t0, 2
	slli	t3, t2, 2
	add	t4, t1, t3
	lw	t5, 0(t4)
	addi	a0, t5, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	la	t6, t_320
	sw	t0, 0(t6)
	la	t0, c
	addi	t1, x0, 1
	slli	t2, t1, 2
	addi	t3, t2, 1
	slli	t4, t3, 2
	add	t5, t0, t4
	lw	t0, 0(t5)
	addi	a0, t0, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	la	t1, c
	addi	t2, x0, 2
	slli	t3, t2, 2
	addi	t4, t3, 2
	slli	t5, t4, 2
	add	t0, t1, t5
	lw	t1, 0(t0)
	addi	a0, t1, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	la	t2, c
	addi	t3, x0, 3
	slli	t4, t3, 2
	addi	t5, t4, 3
	slli	t0, t5, 2
	add	t1, t2, t0
	lw	t2, 0(t1)
	addi	a0, t2, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall

.Lexit:
	addi	a7, x0, 10
	ecall
