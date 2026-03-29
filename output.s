# RISC-V (RV32IM) — compiler project backend
# Source IR : output.ir

	.data
	.align	2

S:	.space	36	# 9 words
T:	.space	36	# 9 words
P:	.space	36	# 9 words
mat_r70:	.word	0
mat_c71:	.word	0
mat_i99:	.word	0
mat_j100:	.word	0
mat_k101:	.word	0
mat_acc117:	.word	0

	# temporaries
t_8:	.word	0
t_16:	.word	0
t_24:	.word	0
t_47:	.word	0
t_72:	.word	0
t_73:	.word	0
t_74:	.word	0
t_75:	.word	0
t_76:	.word	0
t_77:	.word	0
t_78:	.word	0
t_79:	.word	0
t_80:	.word	0
t_81:	.word	0
t_102:	.word	0
t_103:	.word	0
t_104:	.word	0
t_105:	.word	0
t_106:	.word	0
t_107:	.word	0
t_108:	.word	0
t_109:	.word	0
t_110:	.word	0
t_111:	.word	0
t_118:	.word	0
t_123:	.word	0
t_138:	.word	0
t_139:	.word	0
t_140:	.word	0
t_144:	.word	0
t_145:	.word	0
t_154:	.word	0

	# strings
.Ls0:	.asciz	"t:"
.Ls1:	.asciz	"p:"

	.text
	.globl	main
main:
			# IR_BEGIN
	# IR_BEGIN
			# decl_matrix S, 3, 3
	# decl_matrix S, 3, 3
			# t1 = const int 0
	li	t0, 0
			# t3 = addr S
	la	t1, S
			# t4 = mul t1, cols(S)
	li	t6, 3
	mul	t2, t0, t6
			# t6 = mul t4, 4
	slli	t3, t2, 2
			# t7 = add t3, t6
	add	t4, t1, t3
			# t8 = const int 1
	li	t5, 1
			# store [t7], t8
	sw	t5, 0(t4)
			# t9 = const int 0
	li	t0, 0
			# t10 = const int 1
	li	t1, 1
			# t11 = addr S
	la	t2, S
			# t12 = mul t9, cols(S)
	li	t6, 3
	mul	t3, t0, t6
			# t13 = add t12, t10
	addi	t4, t3, 1
			# t14 = mul t13, 4
	la	t6, t_8
	sw	t5, 0(t6)
	slli	t5, t4, 2
			# t15 = add t11, t14
	add	t0, t2, t5
			# t16 = const int 2
	li	t1, 2
			# store [t15], t16
	sw	t1, 0(t0)
			# t17 = const int 0
	li	t2, 0
			# t18 = const int 2
	li	t3, 2
			# t19 = addr S
	la	t4, S
			# t20 = mul t17, cols(S)
	li	t6, 3
	mul	t5, t2, t6
			# t21 = add t20, t18
	addi	t0, t5, 2
			# t22 = mul t21, 4
	la	t6, t_16
	sw	t1, 0(t6)
	slli	t1, t0, 2
			# t23 = add t19, t22
	add	t2, t4, t1
			# t24 = const int 3
	li	t3, 3
			# store [t23], t24
	sw	t3, 0(t2)
			# t25 = const int 1
	li	t4, 1
			# t27 = addr S
	la	t5, S
			# t28 = mul t25, cols(S)
	li	t6, 3
	mul	t0, t4, t6
			# t30 = mul t28, 4
	slli	t1, t0, 2
			# t31 = add t27, t30
	add	t2, t5, t1
			# store [t31], t16
	la	t6, t_24
	sw	t3, 0(t6)
	li	t3, 2
	sw	t3, 0(t2)
			# t32 = const int 1
	li	t4, 1
			# t33 = const int 1
	li	t5, 1
			# t34 = addr S
	la	t0, S
			# t35 = mul t32, cols(S)
	li	t6, 3
	mul	t1, t4, t6
			# t36 = add t35, t33
	addi	t2, t1, 1
			# t37 = mul t36, 4
	la	t6, t_16
	sw	t3, 0(t6)
	slli	t3, t2, 2
			# t38 = add t34, t37
	add	t4, t0, t3
			# t39 = const int 4
	li	t5, 4
			# store [t38], t39
	sw	t5, 0(t4)
			# t40 = const int 1
	li	t0, 1
			# t41 = const int 2
	li	t1, 2
			# t42 = addr S
	la	t2, S
			# t43 = mul t40, cols(S)
	li	t6, 3
	mul	t3, t0, t6
			# t44 = add t43, t41
	addi	t4, t3, 2
			# t45 = mul t44, 4
	slli	t5, t4, 2
			# t46 = add t42, t45
	add	t0, t2, t5
			# t47 = const int 5
	li	t1, 5
			# store [t46], t47
	sw	t1, 0(t0)
			# t48 = const int 2
	li	t2, 2
			# t50 = addr S
	la	t3, S
			# t51 = mul t48, cols(S)
	li	t6, 3
	mul	t4, t2, t6
			# t53 = mul t51, 4
	slli	t5, t4, 2
			# t54 = add t50, t53
	add	t0, t3, t5
			# store [t54], t24
	la	t6, t_47
	sw	t1, 0(t6)
	li	t1, 3
	sw	t1, 0(t0)
			# t55 = const int 2
	li	t2, 2
			# t56 = const int 1
	li	t3, 1
			# t57 = addr S
	la	t4, S
			# t58 = mul t55, cols(S)
	li	t6, 3
	mul	t5, t2, t6
			# t59 = add t58, t56
	addi	t0, t5, 1
			# t60 = mul t59, 4
	slli	t1, t0, 2
			# t61 = add t57, t60
	add	t2, t4, t1
			# store [t61], t47
	li	t3, 5
	sw	t3, 0(t2)
			# t62 = const int 2
	li	t4, 2
			# t63 = const int 2
	li	t5, 2
			# t64 = addr S
	la	t0, S
			# t65 = mul t62, cols(S)
	li	t6, 3
	mul	t1, t4, t6
			# t66 = add t65, t63
	addi	t2, t1, 2
			# t67 = mul t66, 4
	slli	t3, t2, 2
			# t68 = add t64, t67
	add	t4, t0, t3
			# t69 = const int 6
	li	t5, 6
			# store [t68], t69
	sw	t5, 0(t4)
			# decl_matrix T, 3, 3
	# decl_matrix T, 3, 3
			# decl_matrix P, 3, 3
	# decl_matrix P, 3, 3
			# decl int mat_r70
	# decl int mat_r70
			# decl int mat_c71
	# decl int mat_c71
			# t72 = addr mat_r70
	la	t0, mat_r70
			# t73 = addr mat_c71
	la	t1, mat_c71
			# t74 = const int 0
	li	t2, 0
			# t75 = const int 1
	li	t3, 1
			# t76 = const int 3
	li	t4, 3
			# t77 = const int 3
	li	t5, 3
			# t78 = const int 4
	la	t6, t_72
	sw	t0, 0(t6)
	li	t0, 4
			# t79 = const int 12
	la	t6, t_73
	sw	t1, 0(t6)
	li	t1, 12
			# t80 = addr S
	la	t6, t_74
	sw	t2, 0(t6)
	la	t2, S
			# t81 = addr T
	la	t6, t_75
	sw	t3, 0(t6)
	la	t3, T
			# store [t72], t74
	la	t6, t_76
	sw	t4, 0(t6)
	la	t6, t_72
	lw	t4, 0(t6)
	la	t6, t_77
	sw	t5, 0(t6)
	li	t5, 0
	sw	t5, 0(t4)
			# label L1
	la	t6, t_78
	sw	t0, 0(t6)
	la	t6, t_79
	sw	t1, 0(t6)
	la	t6, t_80
	sw	t2, 0(t6)
	la	t6, t_81
	sw	t3, 0(t6)
	la	t6, t_72
	sw	t4, 0(t6)
	la	t6, t_74
	sw	t5, 0(t6)
L1:
			# t82 = load mat_r70
	la	t6, mat_r70
	lw	t0, 0(t6)
			# t83 = lt t82, t76
	li	t1, 3
	slt	t2, t0, t1
			# ifz t83 goto L2
	beqz	t2, L2
			# store [t73], t74
	la	t6, t_73
	lw	t0, 0(t6)
	li	t1, 0
	sw	t1, 0(t0)
			# label L3
	la	t6, t_73
	sw	t0, 0(t6)
L3:
			# t84 = load mat_c71
	la	t6, mat_c71
	lw	t0, 0(t6)
			# t85 = lt t84, t77
	li	t1, 3
	slt	t2, t0, t1
			# ifz t85 goto L4
	beqz	t2, L4
			# t86 = load mat_r70
	la	t6, mat_r70
	lw	t0, 0(t6)
			# t87 = * t86, t79
	li	t1, 12
	mul	t2, t0, t1
			# t88 = + t80, t87
	la	t6, t_80
	lw	t3, 0(t6)
	add	t4, t3, t2
			# t89 = + t81, t87
	la	t6, t_81
	lw	t5, 0(t6)
	add	t0, t5, t2
			# t90 = load mat_c71
	la	t6, mat_c71
	lw	t1, 0(t6)
			# t91 = * t90, t78
	slli	t2, t1, 2
			# t92 = + t88, t91
	add	t3, t4, t2
			# t93 = + t89, t91
	add	t4, t0, t2
			# t94 = load [t92]
	lw	t5, 0(t3)
			# store [t93], t94
	sw	t5, 0(t4)
			# t95 = load mat_c71
	la	t6, mat_c71
	lw	t0, 0(t6)
			# t96 = + t95, t75
	addi	t1, t0, 1
			# store [t73], t96
	la	t6, t_73
	lw	t2, 0(t6)
	sw	t1, 0(t2)
			# goto L3
	j	L3
			# label L4
L4:
			# t97 = load mat_r70
	la	t6, mat_r70
	lw	t0, 0(t6)
			# t98 = + t97, t75
	addi	t1, t0, 1
			# store [t72], t98
	la	t6, t_72
	lw	t2, 0(t6)
	sw	t1, 0(t2)
			# goto L1
	j	L1
			# label L2
L2:
			# decl int mat_i99
	# decl int mat_i99
			# decl int mat_j100
	# decl int mat_j100
			# decl int mat_k101
	# decl int mat_k101
			# t102 = addr mat_i99
	la	t0, mat_i99
			# t103 = addr mat_j100
	la	t1, mat_j100
			# t104 = addr mat_k101
	la	t2, mat_k101
			# t105 = const int 0
	li	t3, 0
			# t106 = const int 1
	li	t4, 1
			# t107 = const int 3
	li	t5, 3
			# t108 = const int 4
	la	t6, t_102
	sw	t0, 0(t6)
	li	t0, 4
			# t109 = const int 12
	la	t6, t_103
	sw	t1, 0(t6)
	li	t1, 12
			# t110 = addr S
	la	t6, t_104
	sw	t2, 0(t6)
	la	t2, S
			# t111 = addr P
	la	t6, t_105
	sw	t3, 0(t6)
	la	t3, P
			# store [t102], t105
	la	t6, t_106
	sw	t4, 0(t6)
	la	t6, t_102
	lw	t4, 0(t6)
	la	t6, t_107
	sw	t5, 0(t6)
	li	t5, 0
	sw	t5, 0(t4)
			# label L5
	la	t6, t_108
	sw	t0, 0(t6)
	la	t6, t_109
	sw	t1, 0(t6)
	la	t6, t_110
	sw	t2, 0(t6)
	la	t6, t_111
	sw	t3, 0(t6)
	la	t6, t_102
	sw	t4, 0(t6)
	la	t6, t_105
	sw	t5, 0(t6)
L5:
			# t112 = load mat_i99
	la	t6, mat_i99
	lw	t0, 0(t6)
			# t113 = lt t112, t107
	li	t1, 3
	slt	t2, t0, t1
			# ifz t113 goto L6
	beqz	t2, L6
	la	t6, t_107
	sw	t1, 0(t6)
			# t114 = load mat_i99
	la	t6, mat_i99
	lw	t0, 0(t6)
			# store [t103], t114
	la	t6, t_103
	lw	t1, 0(t6)
	sw	t0, 0(t1)
			# label L7
	la	t6, t_103
	sw	t1, 0(t6)
L7:
			# t115 = load mat_j100
	la	t6, mat_j100
	lw	t0, 0(t6)
			# t116 = lt t115, t107
	li	t1, 3
	slt	t2, t0, t1
			# ifz t116 goto L8
	beqz	t2, L8
	la	t6, t_107
	sw	t1, 0(t6)
			# decl int mat_acc117
	# decl int mat_acc117
			# t118 = addr mat_acc117
	la	t0, mat_acc117
			# store [t118], t105
	li	t1, 0
	sw	t1, 0(t0)
			# store [t104], t105
	la	t6, t_104
	lw	t2, 0(t6)
	sw	t1, 0(t2)
			# label L9
	la	t6, t_118
	sw	t0, 0(t6)
	la	t6, t_104
	sw	t2, 0(t6)
L9:
			# t119 = load mat_k101
	la	t6, mat_k101
	lw	t0, 0(t6)
			# t120 = lt t119, t107
	li	t1, 3
	slt	t2, t0, t1
			# ifz t120 goto L10
	beqz	t2, L10
			# t121 = load mat_i99
	la	t6, mat_i99
	lw	t0, 0(t6)
			# t122 = load mat_j100
	la	t6, mat_j100
	lw	t1, 0(t6)
			# t123 = load mat_k101
	la	t6, mat_k101
	lw	t2, 0(t6)
			# t124 = * t121, t109
	li	t3, 12
	mul	t4, t0, t3
			# t125 = * t122, t109
	mul	t5, t1, t3
			# t126 = + t110, t124
	la	t6, t_110
	lw	t0, 0(t6)
	add	t1, t0, t4
			# t127 = + t110, t125
	la	t6, t_123
	sw	t2, 0(t6)
	add	t2, t0, t5
			# t128 = * t123, t108
	la	t6, t_109
	sw	t3, 0(t6)
	la	t6, t_123
	lw	t3, 0(t6)
	slli	t4, t3, 2
			# t129 = + t126, t128
	add	t5, t1, t4
			# t130 = + t127, t128
	add	t0, t2, t4
			# t131 = load [t129]
	lw	t1, 0(t5)
			# t132 = load [t130]
	lw	t2, 0(t0)
			# t133 = * t131, t132
	mul	t3, t1, t2
			# t134 = load mat_acc117
	la	t6, mat_acc117
	lw	t4, 0(t6)
			# t135 = + t134, t133
	add	t5, t4, t3
			# store [t118], t135
	la	t6, t_118
	lw	t0, 0(t6)
	sw	t5, 0(t0)
			# t136 = load mat_k101
	la	t6, mat_k101
	lw	t1, 0(t6)
			# t137 = + t136, t106
	addi	t2, t1, 1
			# store [t104], t137
	la	t6, t_104
	lw	t3, 0(t6)
	sw	t2, 0(t3)
			# goto L9
	j	L9
			# label L10
L10:
			# t138 = load mat_i99
	la	t6, mat_i99
	lw	t0, 0(t6)
			# t139 = load mat_j100
	la	t6, mat_j100
	lw	t1, 0(t6)
			# t140 = load mat_acc117
	la	t6, mat_acc117
	lw	t2, 0(t6)
			# t141 = * t138, t109
	li	t3, 12
	mul	t4, t0, t3
			# t142 = * t139, t109
	mul	t5, t1, t3
			# t143 = + t111, t141
	la	t6, t_138
	sw	t0, 0(t6)
	la	t6, t_111
	lw	t0, 0(t6)
	la	t6, t_139
	sw	t1, 0(t6)
	add	t1, t0, t4
			# t144 = + t111, t142
	la	t6, t_140
	sw	t2, 0(t6)
	add	t2, t0, t5
			# t145 = * t139, t108
	la	t6, t_139
	lw	t3, 0(t6)
	slli	t4, t3, 2
			# t146 = + t143, t145
	add	t5, t1, t4
			# store [t146], t140
	la	t6, t_140
	lw	t0, 0(t6)
	sw	t0, 0(t5)
			# t147 = eq t138, t139
	la	t6, t_138
	lw	t1, 0(t6)
	la	t6, t_144
	sw	t2, 0(t6)
	sub	t2, t1, t3
	seqz	t2, t2
			# ifz t147 goto L11
	beqz	t2, L11
	la	t6, t_140
	sw	t0, 0(t6)
	la	t6, t_138
	sw	t1, 0(t6)
	la	t6, t_145
	sw	t4, 0(t6)
			# goto L12
	j	L12
			# label L11
L11:
			# t145 = * t138, t108
	la	t6, t_138
	lw	t0, 0(t6)
	slli	t1, t0, 2
			# t148 = + t144, t145
	la	t6, t_144
	lw	t2, 0(t6)
	add	t3, t2, t1
			# store [t148], t140
	la	t6, t_140
	lw	t4, 0(t6)
	sw	t4, 0(t3)
			# label L12
L12:
			# t149 = load mat_j100
	la	t6, mat_j100
	lw	t0, 0(t6)
			# t150 = + t149, t106
	addi	t1, t0, 1
			# store [t103], t150
	la	t6, t_103
	lw	t2, 0(t6)
	sw	t1, 0(t2)
			# goto L7
	j	L7
			# label L8
L8:
			# t151 = load mat_i99
	la	t6, mat_i99
	lw	t0, 0(t6)
			# t152 = + t151, t106
	addi	t1, t0, 1
			# store [t102], t152
	la	t6, t_102
	lw	t2, 0(t6)
	sw	t1, 0(t2)
			# goto L5
	j	L5
			# label L6
L6:
			# print_str "t:"
	la	a0, .Ls0
	li	a7, 4
	ecall
			# t154 = const int 0
	li	t0, 0
			# t153 = addr T
	la	t1, T
			# t155 = mul t154, cols(T)
	li	t6, 3
	mul	t2, t0, t6
			# t156 = add t155, t8
	addi	t3, t2, 1
			# t157 = mul t156, 4
	slli	t4, t3, 2
			# t158 = add t153, t157
	add	t5, t1, t4
			# t159 = load [t158]
	la	t6, t_154
	sw	t0, 0(t6)
	lw	t0, 0(t5)
			# print t159
	mv	a0, t0
	li	a7, 1
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# t160 = addr T
	la	t1, T
			# t161 = mul t8, cols(T)
	li	t2, 1
	li	t6, 3
	mul	t3, t2, t6
			# t163 = mul t161, 4
	slli	t4, t3, 2
			# t164 = add t160, t163
	add	t5, t1, t4
			# t165 = load [t164]
	lw	t0, 0(t5)
			# print t165
	mv	a0, t0
	li	a7, 1
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# print_str "p:"
	la	a0, .Ls1
	li	a7, 4
	ecall
			# t166 = addr P
	la	t1, P
			# t167 = mul t154, cols(P)
	li	t2, 0
	li	t6, 3
	mul	t3, t2, t6
			# t168 = add t167, t16
	addi	t4, t3, 2
			# t169 = mul t168, 4
	slli	t5, t4, 2
			# t170 = add t166, t169
	add	t0, t1, t5
			# t171 = load [t170]
	lw	t1, 0(t0)
			# print t171
	mv	a0, t1
	li	a7, 1
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# t172 = addr P
	la	t6, t_154
	sw	t2, 0(t6)
	la	t2, P
			# t173 = mul t16, cols(P)
	li	t3, 2
	li	t6, 3
	mul	t4, t3, t6
			# t175 = mul t173, 4
	slli	t5, t4, 2
			# t176 = add t172, t175
	add	t0, t2, t5
			# t177 = load [t176]
	lw	t1, 0(t0)
			# print t177
	mv	a0, t1
	li	a7, 1
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# ret t154
			# IR_END
	# IR_END

.Lexit:
	li	a7, 10
	ecall
