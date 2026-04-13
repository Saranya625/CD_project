	.data
	.align	2

a:	.space	16
b:	.space	16
c:	.space	16
mat_r65:	.word	0
mat_c66:	.word	0
mat_k67:	.word	0
mat_acc68:	.word	0

t_73:	.word	0
t_74:	.word	0
t_75:	.word	0
t_76:	.word	0
t_77:	.word	0
t_78:	.word	0
t_79:	.word	0
t_80:	.word	0
t_85:	.word	0
t_87:	.word	0
t_89:	.word	0
t_151:	.word	0

.Ls0:	.asciz	"c:"

	.text
	.globl	main
main:
	addi	t0, x0, 0
	addi	t1, x0, 0
	la	t2, a
	slli	t3, t0, 1
	addi	t4, t3, 0
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 1
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 0
	addi	t3, x0, 1
	la	t4, a
	slli	t5, t2, 1
	addi	t0, t5, 1
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 2
	addi	a5, t2, 0
	sw	t3, 0(a5)
	addi	t4, x0, 1
	addi	t5, x0, 0
	la	t0, a
	slli	t1, t4, 1
	addi	t2, t1, 0
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	t5, x0, 3
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 1
	addi	t1, x0, 1
	la	t2, a
	slli	t3, t0, 1
	addi	t4, t3, 1
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 4
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 0
	addi	t3, x0, 0
	la	t4, b
	slli	t5, t2, 1
	addi	t0, t5, 0
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 5
	addi	a5, t2, 0
	sw	t3, 0(a5)
	addi	t4, x0, 0
	addi	t5, x0, 1
	la	t0, b
	slli	t1, t4, 1
	addi	t2, t1, 1
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	t5, x0, 6
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 1
	addi	t1, x0, 0
	la	t2, b
	slli	t3, t0, 1
	addi	t4, t3, 0
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 7
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 1
	addi	t3, x0, 1
	la	t4, b
	slli	t5, t2, 1
	addi	t0, t5, 1
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 8
	addi	a5, t2, 0
	sw	t3, 0(a5)
	la	t4, mat_r65
	la	t5, mat_c66
	la	t0, mat_k67
	la	t1, mat_acc68
	addi	t2, x0, 0
	addi	t3, x0, 1
	addi	t4, x0, 2
	addi	t5, x0, 2
	addi	t0, x0, 2
	addi	t1, x0, 4
	la	t6, t_73
	sw	t2, 0(t6)
	addi	t2, x0, 8
	la	t6, t_74
	sw	t3, 0(t6)
	addi	t3, x0, 8
	la	t6, t_75
	sw	t4, 0(t6)
	la	t4, a
	la	t6, t_76
	sw	t5, 0(t6)
	la	t5, b
	la	t6, t_77
	sw	t0, 0(t6)
	la	t0, c
	la	t6, t_78
	sw	t1, 0(t6)
	la	t1, mat_r65
	addi	a5, t1, 0
	la	t6, t_79
	sw	t2, 0(t6)
	addi	t2, x0, 0
	sw	t2, 0(a5)
	la	t6, t_73
	sw	t2, 0(t6)
	la	t6, t_80
	sw	t3, 0(t6)
L1:
	la	t6, mat_r65
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_85
	sw	t2, 0(t6)
	beqz	t2, L2
	la	t0, mat_c66
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, t_73
	sw	t1, 0(t6)
L3:
	la	t6, mat_c66
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_87
	sw	t2, 0(t6)
	beqz	t2, L4
	la	t0, mat_acc68
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t2, mat_k67
	addi	a5, t2, 0
	sw	t1, 0(a5)
L5:
	la	t6, mat_k67
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_89
	sw	t2, 0(t6)
	beqz	t2, L6
	la	t6, mat_r65
	lw	t0, 0(t6)
	slli	t1, t0, 3
	la	t2, a
	add	t3, t2, t1
	la	t6, mat_c66
	lw	t4, 0(t6)
	slli	t5, t4, 2
	la	t6, mat_k67
	lw	t0, 0(t6)
	slli	t1, t0, 2
	add	t2, t3, t1
	slli	t3, t0, 3
	add	t4, t3, t5
	la	t5, b
	add	t0, t5, t4
	lw	t1, 0(t2)
	lw	t2, 0(t0)
	mul	t3, t1, t2
	la	t6, mat_acc68
	lw	t4, 0(t6)
	add	t5, t4, t3
	la	t0, mat_acc68
	addi	a5, t0, 0
	sw	t5, 0(a5)
	la	t6, mat_k67
	lw	t1, 0(t6)
	addi	t2, t1, 1
	la	t3, mat_k67
	addi	a5, t3, 0
	sw	t2, 0(a5)
	j	L5
L6:
	la	t6, mat_r65
	lw	t0, 0(t6)
	slli	t1, t0, 3
	la	t2, c
	add	t3, t2, t1
	la	t6, mat_c66
	lw	t4, 0(t6)
	slli	t5, t4, 2
	la	t6, mat_acc68
	lw	t0, 0(t6)
	add	t1, t3, t5
	addi	a5, t1, 0
	sw	t0, 0(a5)
	la	t6, mat_c66
	lw	t2, 0(t6)
	addi	t3, t2, 1
	la	t4, mat_c66
	addi	a5, t4, 0
	sw	t3, 0(a5)
	j	L3
L4:
	la	t6, mat_r65
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_r65
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L1
L2:
	la	a0, .Ls0
	addi	a7, x0, 4
	ecall
	addi	t0, x0, 0
	addi	t1, x0, 0
	la	t2, c
	slli	t3, t0, 1
	addi	t4, t3, 0
	slli	t5, t4, 2
	add	t0, t2, t5
	lw	t1, 0(t0)
	addi	a0, t1, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t2, x0, 0
	addi	t3, x0, 1
	la	t4, c
	slli	t5, t2, 1
	addi	t0, t5, 1
	slli	t1, t0, 2
	add	t2, t4, t1
	lw	t3, 0(t2)
	addi	a0, t3, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t4, x0, 1
	addi	t5, x0, 0
	la	t0, c
	slli	t1, t4, 1
	addi	t2, t1, 0
	slli	t3, t2, 2
	add	t4, t0, t3
	lw	t5, 0(t4)
	addi	a0, t5, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t0, x0, 1
	addi	t1, x0, 1
	la	t2, c
	slli	t3, t0, 1
	addi	t4, t3, 1
	slli	t5, t4, 2
	add	t0, t2, t5
	lw	t1, 0(t0)
	addi	a0, t1, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t2, x0, 0
	la	t6, t_151
	sw	t2, 0(t6)

.Lexit:
	addi	a7, x0, 10
	ecall
