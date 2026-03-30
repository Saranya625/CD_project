# RISC-V (RV32IM) — compiler project backend
# Source IR : output.ir

	.data
	.align	2

U:	.space	36	# 9 words
Inv:	.space	36	# 9 words
detv:	.word	0
mat_tmpA95:	.space	36	# 9 words
mat_r96:	.word	0
mat_c97:	.word	0
mat_i125:	.word	0
mat_j126:	.word	0
mat_r127:	.word	0

	# temporaries
t_8:	.word	0
t_16:	.word	0
t_32:	.word	0
t_76:	.word	0
t_85:	.word	0
t_100:	.word	0
t_101:	.word	0
t_102:	.word	0
t_103:	.word	0
t_104:	.word	0
t_105:	.word	0
t_131:	.word	0
t_132:	.word	0
t_133:	.word	0
t_134:	.word	0
t_139:	.word	0
t_140:	.word	0
t_158:	.word	0
t_164:	.word	0
t_165:	.word	0
t_166:	.word	0
t_167:	.word	0
t_168:	.word	0
t_169:	.word	0
t_170:	.word	0
t_174:	.word	0
t_179:	.word	0
t_185:	.word	0
t_186:	.word	0
t_187:	.word	0
t_188:	.word	0
t_189:	.word	0
t_190:	.word	0
t_196:	.word	0
t_199:	.word	0
t_200:	.word	0
t_203:	.word	0
t_208:	.word	0
t_213:	.word	0
t_218:	.word	0
t_223:	.word	0
t_230:	.word	0
t_233:	.word	0
t_234:	.word	0
t_235:	.word	0
t_237:	.word	0
t_238:	.word	0
t_239:	.word	0
t_240:	.word	0
t_241:	.word	0
t_242:	.word	0
t_255:	.word	0
t_261:	.word	0
t_267:	.word	0

	# strings
.Ls0:	.asciz	"det:"
.Ls1:	.asciz	"\ninv:"

	.text
	.globl	main
main:
			# IR_BEGIN
	# IR_BEGIN
			# decl_matrix U, 3, 3
	# decl_matrix U, 3, 3
			# t1 = const int 0
	li	t0, 0
			# t3 = addr U
	la	t1, U
			# t4 = mul t1, cols(U)
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
			# t11 = addr U
	la	t2, U
			# t12 = mul t9, cols(U)
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
			# t19 = addr U
	la	t4, U
			# t20 = mul t17, cols(U)
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
			# t27 = addr U
	la	t5, U
			# t28 = mul t25, cols(U)
	li	t6, 3
	mul	t0, t4, t6
			# t30 = mul t28, 4
	slli	t1, t0, 2
			# t31 = add t27, t30
	add	t2, t5, t1
			# t32 = const int 0
	li	t3, 0
			# store [t31], t32
	sw	t3, 0(t2)
			# t33 = const int 1
	li	t4, 1
			# t34 = const int 1
	li	t5, 1
			# t35 = addr U
	la	t0, U
			# t36 = mul t33, cols(U)
	li	t6, 3
	mul	t1, t4, t6
			# t37 = add t36, t34
	addi	t2, t1, 1
			# t38 = mul t37, 4
	la	t6, t_32
	sw	t3, 0(t6)
	slli	t3, t2, 2
			# t39 = add t35, t38
	add	t4, t0, t3
			# store [t39], t8
	li	t5, 1
	sw	t5, 0(t4)
			# t40 = const int 1
	li	t0, 1
			# t41 = const int 2
	li	t1, 2
			# t42 = addr U
	la	t2, U
			# t43 = mul t40, cols(U)
	li	t6, 3
	mul	t3, t0, t6
			# t44 = add t43, t41
	addi	t4, t3, 2
			# t45 = mul t44, 4
	la	t6, t_8
	sw	t5, 0(t6)
	slli	t5, t4, 2
			# t46 = add t42, t45
	add	t0, t2, t5
			# t47 = const int 4
	li	t1, 4
			# store [t46], t47
	sw	t1, 0(t0)
			# t48 = const int 2
	li	t2, 2
			# t50 = addr U
	la	t3, U
			# t51 = mul t48, cols(U)
	li	t6, 3
	mul	t4, t2, t6
			# t53 = mul t51, 4
	slli	t5, t4, 2
			# t54 = add t50, t53
	add	t0, t3, t5
			# store [t54], t32
	li	t1, 0
	sw	t1, 0(t0)
			# t55 = const int 2
	li	t2, 2
			# t56 = const int 1
	li	t3, 1
			# t57 = addr U
	la	t4, U
			# t58 = mul t55, cols(U)
	li	t6, 3
	mul	t5, t2, t6
			# t59 = add t58, t56
	addi	t0, t5, 1
			# t60 = mul t59, 4
	la	t6, t_32
	sw	t1, 0(t6)
	slli	t1, t0, 2
			# t61 = add t57, t60
	add	t2, t4, t1
			# store [t61], t32
	li	t3, 0
	sw	t3, 0(t2)
			# t62 = const int 2
	li	t4, 2
			# t63 = const int 2
	li	t5, 2
			# t64 = addr U
	la	t0, U
			# t65 = mul t62, cols(U)
	li	t6, 3
	mul	t1, t4, t6
			# t66 = add t65, t63
	addi	t2, t1, 2
			# t67 = mul t66, 4
	la	t6, t_32
	sw	t3, 0(t6)
	slli	t3, t2, 2
			# t68 = add t64, t67
	add	t4, t0, t3
			# store [t68], t8
	li	t5, 1
	sw	t5, 0(t4)
			# decl_matrix Inv, 3, 3
	# decl_matrix Inv, 3, 3
			# decl decimal detv
	# decl decimal detv
			# t70 = const int 0
	li	t0, 0
			# t71 = addr U
	la	t1, U
			# t72 = mul t70, cols(U)
	li	t6, 3
	mul	t2, t0, t6
			# t74 = mul t72, 4
	slli	t3, t2, 2
			# t75 = add t71, t74
	add	t4, t1, t3
			# t76 = load [t75]
	la	t6, t_8
	sw	t5, 0(t6)
	lw	t5, 0(t4)
			# t78 = const int 1
	li	t0, 1
			# t79 = addr U
	la	t1, U
			# t80 = mul t78, cols(U)
	li	t6, 3
	mul	t2, t0, t6
			# t81 = add t80, t78
	addi	t3, t2, 1
			# t82 = mul t81, 4
	slli	t4, t3, 2
			# t83 = add t79, t82
	la	t6, t_76
	sw	t5, 0(t6)
	add	t5, t1, t4
			# t84 = load [t83]
	lw	t0, 0(t5)
			# t85 = * t76, t84
	la	t6, t_76
	lw	t1, 0(t6)
	mul	t2, t1, t0
			# t86 = const int 2
	li	t3, 2
			# t87 = addr U
	la	t4, U
			# t88 = mul t86, cols(U)
	li	t6, 3
	mul	t5, t3, t6
			# t89 = add t88, t86
	addi	t0, t5, 2
			# t90 = mul t89, 4
	slli	t1, t0, 2
			# t91 = add t87, t90
	la	t6, t_85
	sw	t2, 0(t6)
	add	t2, t4, t1
			# t92 = load [t91]
	lw	t3, 0(t2)
			# t93 = * t85, t92
	la	t6, t_85
	lw	t4, 0(t6)
	mul	t5, t4, t3
			# t94 = addr detv
	la	t0, detv
			# store [t94], t93
	sw	t5, 0(t0)
			# decl_matrix mat_tmpA95, 3, 3
	# decl_matrix mat_tmpA95, 3, 3
			# decl int mat_r96
	# decl int mat_r96
			# decl int mat_c97
	# decl int mat_c97
			# t98 = addr mat_r96
	la	t1, mat_r96
			# t99 = addr mat_c97
	la	t2, mat_c97
			# t100 = const int 0
	li	t3, 0
			# t101 = const int 1
	li	t4, 1
			# t102 = const int 3
	li	t5, 3
			# t103 = const int 3
	li	t0, 3
			# t104 = const int 4
	li	t1, 4
			# t105 = const int 12
	li	t2, 12
			# t106 = addr U
	la	t6, t_100
	sw	t3, 0(t6)
	la	t3, U
			# t107 = addr mat_tmpA95
	la	t6, t_101
	sw	t4, 0(t6)
	la	t4, mat_tmpA95
			# store [t98], t100
	la	t6, t_102
	sw	t5, 0(t6)
	la	t5, mat_r96
	la	t6, t_103
	sw	t0, 0(t6)
	li	t0, 0
	sw	t0, 0(t5)
			# label L1
	la	t6, t_100
	sw	t0, 0(t6)
	la	t6, t_104
	sw	t1, 0(t6)
	la	t6, t_105
	sw	t2, 0(t6)
L1:
			# t108 = load mat_r96
	la	t6, mat_r96
	lw	t0, 0(t6)
			# t109 = lt t108, t102
	li	t1, 3
	slt	t2, t0, t1
			# ifz t109 goto L2
	beqz	t2, L2
			# store [t99], t100
	la	t0, mat_c97
	li	t1, 0
	sw	t1, 0(t0)
			# label L3
L3:
			# t110 = load mat_c97
	la	t6, mat_c97
	lw	t0, 0(t6)
			# t111 = lt t110, t103
	li	t1, 3
	slt	t2, t0, t1
			# ifz t111 goto L4
	beqz	t2, L4
			# t112 = load mat_r96
	la	t6, mat_r96
	lw	t0, 0(t6)
			# t113 = * t112, t105
	li	t1, 12
	mul	t2, t0, t1
			# t114 = + t106, t113
	la	t3, U
	add	t4, t3, t2
			# t115 = + t107, t113
	la	t5, mat_tmpA95
	add	t0, t5, t2
			# t116 = load mat_c97
	la	t6, mat_c97
	lw	t1, 0(t6)
			# t117 = * t116, t104
	slli	t2, t1, 2
			# t118 = + t114, t117
	add	t3, t4, t2
			# t119 = + t115, t117
	add	t4, t0, t2
			# t120 = load [t118]
	lw	t5, 0(t3)
			# store [t119], t120
	sw	t5, 0(t4)
			# t121 = load mat_c97
	la	t6, mat_c97
	lw	t0, 0(t6)
			# t122 = + t121, t101
	addi	t1, t0, 1
			# store [t99], t122
	la	t2, mat_c97
	sw	t1, 0(t2)
			# goto L3
	j	L3
			# label L4
L4:
			# t123 = load mat_r96
	la	t6, mat_r96
	lw	t0, 0(t6)
			# t124 = + t123, t101
	addi	t1, t0, 1
			# store [t98], t124
	la	t2, mat_r96
	sw	t1, 0(t2)
			# goto L1
	j	L1
			# label L2
L2:
			# decl int mat_i125
	# decl int mat_i125
			# decl int mat_j126
	# decl int mat_j126
			# decl int mat_r127
	# decl int mat_r127
			# t128 = addr mat_i125
	la	t0, mat_i125
			# t129 = addr mat_j126
	la	t1, mat_j126
			# t130 = addr mat_r127
	la	t2, mat_r127
			# t131 = const int 0
	li	t3, 0
			# t132 = const int 1
	li	t4, 1
			# t133 = const decimal 1.0
	li	t6, 1065353216
	fmv.w.x	ft1, t6
			# t134 = const int 3
	li	t5, 3
			# store [t128], t131
	sw	t3, 0(t0)
			# label L5
	la	t6, t_131
	sw	t3, 0(t6)
	la	t6, t_132
	sw	t4, 0(t6)
	la	t6, t_134
	sw	t5, 0(t6)
	la	t6, t_133
	fsw	ft1, 0(t6)
L5:
			# t135 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t136 = lt t135, t134
	li	t1, 3
	slt	t2, t0, t1
			# ifz t136 goto L6
	la	t6, t_134
	sw	t1, 0(t6)
	beqz	t2, L6
			# store [t129], t131
	la	t0, mat_j126
	li	t1, 0
	sw	t1, 0(t0)
			# label L7
	la	t6, t_131
	sw	t1, 0(t6)
L7:
			# t137 = load mat_j126
	la	t6, mat_j126
	lw	t0, 0(t6)
			# t138 = lt t137, t134
	li	t1, 3
	slt	t2, t0, t1
			# ifz t138 goto L8
	la	t6, t_134
	sw	t1, 0(t6)
	beqz	t2, L8
			# t139 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t140 = load mat_j126
	la	t6, mat_j126
	lw	t1, 0(t6)
			# t141 = eq t139, t140
	sub	t2, t0, t1
	seqz	t2, t2
			# ifz t141 goto L9
	la	t6, t_139
	sw	t0, 0(t6)
	la	t6, t_140
	sw	t1, 0(t6)
	beqz	t2, L9
			# t142 = addr Inv
	la	t0, Inv
			# t143 = mul t139, cols(Inv)
	la	t6, t_139
	lw	t1, 0(t6)
	li	t6, 3
	mul	t2, t1, t6
			# t144 = add t143, t140
	la	t6, t_140
	lw	t3, 0(t6)
	add	t4, t2, t3
			# t145 = mul t144, 4
	slli	t5, t4, 2
			# t146 = add t142, t145
	la	t6, t_139
	sw	t1, 0(t6)
	la	t1, Inv
	add	t0, t1, t5
			# store [t146], t132
	li	t2, 1
	sw	t2, 0(t0)
			# goto L10
	la	t6, t_132
	sw	t2, 0(t6)
	la	t6, t_140
	sw	t3, 0(t6)
	j	L10
			# label L9
L9:
			# t147 = addr Inv
	la	t0, Inv
			# t148 = mul t139, cols(Inv)
	la	t6, t_139
	lw	t1, 0(t6)
	li	t6, 3
	mul	t2, t1, t6
			# t149 = add t148, t140
	la	t6, t_140
	lw	t3, 0(t6)
	add	t4, t2, t3
			# t150 = mul t149, 4
	slli	t5, t4, 2
			# t151 = add t147, t150
	la	t1, Inv
	add	t0, t1, t5
			# store [t151], t131
	li	t2, 0
	sw	t2, 0(t0)
			# label L10
	la	t6, t_131
	sw	t2, 0(t6)
L10:
			# t152 = load mat_j126
	la	t6, mat_j126
	lw	t0, 0(t6)
			# t153 = + t152, t132
	addi	t1, t0, 1
			# store [t129], t153
	la	t2, mat_j126
	sw	t1, 0(t2)
			# goto L7
	j	L7
			# label L8
L8:
			# t154 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t155 = + t154, t132
	addi	t1, t0, 1
			# store [t128], t155
	la	t2, mat_i125
	sw	t1, 0(t2)
			# goto L5
	j	L5
			# label L6
L6:
			# store [t128], t131
	la	t0, mat_i125
	li	t1, 0
	sw	t1, 0(t0)
			# label L11
	la	t6, t_131
	sw	t1, 0(t6)
L11:
			# t156 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t157 = lt t156, t134
	li	t1, 3
	slt	t2, t0, t1
			# ifz t157 goto L12
	la	t6, t_134
	sw	t1, 0(t6)
	beqz	t2, L12
			# t158 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t159 = addr mat_tmpA95
	la	t1, mat_tmpA95
			# t160 = mul t158, cols(mat_tmpA95)
	li	t6, 3
	mul	t2, t0, t6
			# t161 = add t160, t158
	add	t3, t2, t0
			# t162 = mul t161, 4
	slli	t4, t3, 2
			# t163 = add t159, t162
	add	t5, t1, t4
			# t164 = load [t163]
	la	t6, t_158
	sw	t0, 0(t6)
	lw	t0, 0(t5)
			# t165 = const decimal 1.0
	li	t6, 1065353216
	fmv.w.x	ft1, t6
			# t167 = const decimal 0.0
	li	t6, 0
	fmv.w.x	ft2, t6
			# t168 = const decimal 1.0
	li	t6, 1065353216
	fmv.w.x	ft3, t6
			# t169 = * t165, t168
	fmul.s	ft4, ft1, ft3
			# t170 = * t164, t168
	fcvt.s.w	ft5, t0
	fmul.s	ft6, ft5, ft3
			# t171 = eq t170, t167
	feq.s	t1, ft6, ft2
			# ifz t171 goto L13
	la	t6, t_165
	fsw	ft1, 0(t6)
	la	t6, t_167
	fsw	ft2, 0(t6)
	la	t6, t_168
	fsw	ft3, 0(t6)
	la	t6, t_169
	fsw	ft4, 0(t6)
	la	t6, t_164
	fsw	ft5, 0(t6)
	la	t6, t_170
	fsw	ft6, 0(t6)
	beqz	t1, L13
			# t166 = const decimal 0.0
	li	t6, 0
	fmv.w.x	ft1, t6
			# goto L14
	la	t6, t_166
	fsw	ft1, 0(t6)
	j	L14
			# label L13
L13:
			# t166 = / t169, t170
	la	t6, t_169
	flw	ft1, 0(t6)
	la	t6, t_170
	flw	ft2, 0(t6)
	fdiv.s	ft3, ft1, ft2
			# label L14
	la	t6, t_169
	fsw	ft1, 0(t6)
	la	t6, t_170
	fsw	ft2, 0(t6)
	la	t6, t_166
	fsw	ft3, 0(t6)
L14:
			# store [t129], t131
	la	t0, mat_j126
	li	t1, 0
	sw	t1, 0(t0)
			# label L15
	la	t6, t_131
	sw	t1, 0(t6)
L15:
			# t172 = load mat_j126
	la	t6, mat_j126
	lw	t0, 0(t6)
			# t173 = lt t172, t134
	li	t1, 3
	slt	t2, t0, t1
			# ifz t173 goto L16
	la	t6, t_134
	sw	t1, 0(t6)
	beqz	t2, L16
			# t174 = load mat_j126
	la	t6, mat_j126
	lw	t0, 0(t6)
			# t175 = addr mat_tmpA95
	la	t1, mat_tmpA95
			# t176 = mul t158, cols(mat_tmpA95)
	la	t6, t_158
	lw	t2, 0(t6)
	li	t6, 3
	mul	t3, t2, t6
			# t177 = add t176, t174
	add	t4, t3, t0
			# t178 = mul t177, 4
	slli	t5, t4, 2
			# t179 = add t175, t178
	la	t6, t_174
	sw	t0, 0(t6)
	add	t0, t1, t5
			# t180 = addr Inv
	la	t1, Inv
			# t181 = mul t158, cols(Inv)
	li	t6, 3
	mul	t2, t2, t6
			# t182 = add t181, t174
	la	t6, t_174
	lw	t3, 0(t6)
	add	t4, t2, t3
			# t183 = mul t182, 4
	slli	t5, t4, 2
			# t184 = add t180, t183
	la	t6, t_179
	sw	t0, 0(t6)
	add	t0, t1, t5
			# t185 = load [t179]
	la	t6, t_179
	lw	t1, 0(t6)
	lw	t2, 0(t1)
			# t186 = load [t184]
	lw	t3, 0(t0)
			# t187 = * t185, t133
	fcvt.s.w	ft1, t2
	li	t6, 1065353216
	fmv.w.x	ft2, t6
	fmul.s	ft3, ft1, ft2
			# t188 = * t186, t133
	fcvt.s.w	ft4, t3
	fmul.s	ft5, ft4, ft2
			# t189 = * t187, t166
	la	t6, t_166
	flw	ft6, 0(t6)
	fmul.s	ft7, ft3, ft6
			# t190 = * t188, t166
	fmul.s	fa1, ft5, ft6
			# store [t179], t189
	fsw	ft7, 0(t1)
			# store [t184], t190
	fsw	fa1, 0(t0)
			# t191 = load mat_j126
	la	t6, mat_j126
	lw	t4, 0(t6)
			# t192 = + t191, t132
	addi	t5, t4, 1
			# store [t129], t192
	la	t0, mat_j126
	sw	t5, 0(t0)
			# goto L15
	la	t6, t_185
	fsw	ft1, 0(t6)
	la	t6, t_133
	fsw	ft2, 0(t6)
	la	t6, t_187
	fsw	ft3, 0(t6)
	la	t6, t_186
	fsw	ft4, 0(t6)
	la	t6, t_188
	fsw	ft5, 0(t6)
	la	t6, t_166
	fsw	ft6, 0(t6)
	la	t6, t_189
	fsw	ft7, 0(t6)
	la	t6, t_190
	fsw	fa1, 0(t6)
	j	L15
			# label L16
L16:
			# store [t130], t131
	la	t0, mat_r127
	li	t1, 0
	sw	t1, 0(t0)
			# label L17
	la	t6, t_131
	sw	t1, 0(t6)
L17:
			# t193 = load mat_r127
	la	t6, mat_r127
	lw	t0, 0(t6)
			# t194 = lt t193, t134
	li	t1, 3
	slt	t2, t0, t1
			# ifz t194 goto L18
	la	t6, t_134
	sw	t1, 0(t6)
	beqz	t2, L18
			# t195 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t196 = load mat_r127
	la	t6, mat_r127
	lw	t1, 0(t6)
			# t197 = eq t196, t195
	sub	t2, t1, t0
	seqz	t2, t2
			# ifz t197 goto L19
	la	t6, t_196
	sw	t1, 0(t6)
	beqz	t2, L19
			# t198 = + t196, t132
	la	t6, t_196
	lw	t0, 0(t6)
	addi	t1, t0, 1
			# store [t130], t198
	la	t2, mat_r127
	sw	t1, 0(t2)
			# goto L17
	j	L17
			# label L19
L19:
			# t199 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t200 = load mat_r127
	la	t6, mat_r127
	lw	t1, 0(t6)
			# store [t129], t131
	la	t2, mat_j126
	li	t3, 0
	sw	t3, 0(t2)
			# label L20
	la	t6, t_199
	sw	t0, 0(t6)
	la	t6, t_200
	sw	t1, 0(t6)
L20:
			# t201 = load mat_j126
	la	t6, mat_j126
	lw	t0, 0(t6)
			# t202 = lt t201, t134
	li	t1, 3
	slt	t2, t0, t1
			# ifz t202 goto L21
	beqz	t2, L21
			# t203 = load mat_j126
	la	t6, mat_j126
	lw	t0, 0(t6)
			# t204 = addr mat_tmpA95
	la	t1, mat_tmpA95
			# t205 = mul t200, cols(mat_tmpA95)
	la	t6, t_200
	lw	t2, 0(t6)
	li	t6, 3
	mul	t3, t2, t6
			# t206 = add t205, t199
	la	t6, t_199
	lw	t4, 0(t6)
	add	t5, t3, t4
			# t207 = mul t206, 4
	la	t6, t_203
	sw	t0, 0(t6)
	slli	t0, t5, 2
			# t208 = add t204, t207
	la	t6, t_200
	sw	t2, 0(t6)
	la	t2, mat_tmpA95
	add	t1, t2, t0
			# t209 = addr mat_tmpA95
	la	t3, mat_tmpA95
			# t210 = mul t200, cols(mat_tmpA95)
	la	t6, t_199
	sw	t4, 0(t6)
	la	t6, t_200
	lw	t4, 0(t6)
	li	t6, 3
	mul	t5, t4, t6
			# t211 = add t210, t203
	la	t6, t_203
	lw	t0, 0(t6)
	la	t6, t_208
	sw	t1, 0(t6)
	add	t1, t5, t0
			# t212 = mul t211, 4
	slli	t2, t1, 2
			# t213 = add t209, t212
	la	t6, t_200
	sw	t4, 0(t6)
	la	t4, mat_tmpA95
	add	t3, t4, t2
			# t214 = addr mat_tmpA95
	la	t5, mat_tmpA95
			# t215 = mul t199, cols(mat_tmpA95)
	la	t6, t_203
	sw	t0, 0(t6)
	la	t6, t_199
	lw	t0, 0(t6)
	li	t6, 3
	mul	t1, t0, t6
			# t216 = add t215, t203
	la	t6, t_203
	lw	t2, 0(t6)
	la	t6, t_213
	sw	t3, 0(t6)
	add	t3, t1, t2
			# t217 = mul t216, 4
	slli	t4, t3, 2
			# t218 = add t214, t217
	la	t6, t_199
	sw	t0, 0(t6)
	la	t0, mat_tmpA95
	add	t5, t0, t4
			# t219 = addr Inv
	la	t1, Inv
			# t220 = mul t200, cols(Inv)
	la	t6, t_203
	sw	t2, 0(t6)
	la	t6, t_200
	lw	t2, 0(t6)
	li	t6, 3
	mul	t3, t2, t6
			# t221 = add t220, t203
	la	t6, t_203
	lw	t4, 0(t6)
	la	t6, t_218
	sw	t5, 0(t6)
	add	t5, t3, t4
			# t222 = mul t221, 4
	slli	t0, t5, 2
			# t223 = add t219, t222
	la	t2, Inv
	add	t1, t2, t0
			# t224 = addr Inv
	la	t3, Inv
			# t225 = mul t199, cols(Inv)
	la	t6, t_203
	sw	t4, 0(t6)
	la	t6, t_199
	lw	t4, 0(t6)
	li	t6, 3
	mul	t5, t4, t6
			# t226 = add t225, t203
	la	t6, t_203
	lw	t0, 0(t6)
	la	t6, t_223
	sw	t1, 0(t6)
	add	t1, t5, t0
			# t227 = mul t226, 4
	slli	t2, t1, 2
			# t228 = add t224, t227
	la	t4, Inv
	add	t3, t4, t2
			# t229 = load [t208]
	la	t6, t_208
	lw	t5, 0(t6)
	flw	ft1, 0(t5)
			# t231 = load [t213]
	la	t6, t_213
	lw	t0, 0(t6)
	flw	ft2, 0(t0)
			# t232 = load [t218]
	la	t6, t_218
	lw	t1, 0(t6)
	flw	ft3, 0(t1)
			# t233 = load [t223]
	la	t6, t_223
	lw	t2, 0(t6)
	flw	ft4, 0(t2)
			# t234 = load [t228]
	flw	ft5, 0(t3)
			# t230 = * t229, t133
	li	t6, 1065353216
	fmv.w.x	ft6, t6
	fmul.s	ft7, ft1, ft6
			# t235 = * t231, t133
	fmul.s	fa1, ft2, ft6
			# t236 = * t232, t133
	la	t6, t_233
	fsw	ft4, 0(t6)
	fmul.s	ft4, ft3, ft6
			# t237 = * t233, t133
	la	t6, t_234
	fsw	ft5, 0(t6)
	la	t6, t_233
	flw	ft5, 0(t6)
	fmul.s	ft1, ft5, ft6
			# t238 = * t234, t133
	la	t6, t_230
	fsw	ft7, 0(t6)
	la	t6, t_234
	flw	ft7, 0(t6)
	fmul.s	ft2, ft7, ft6
			# t239 = * t230, t236
	la	t6, t_235
	fsw	fa1, 0(t6)
	la	t6, t_230
	flw	fa1, 0(t6)
	fmul.s	ft3, fa1, ft4
			# t240 = * t230, t238
	fmul.s	ft5, fa1, ft2
			# t241 = - t235, t239
	la	t6, t_237
	fsw	ft1, 0(t6)
	la	t6, t_235
	flw	ft1, 0(t6)
	fsub.s	ft7, ft1, ft3
			# t242 = - t237, t240
	la	t6, t_237
	flw	ft6, 0(t6)
	fsub.s	ft4, ft6, ft5
			# store [t213], t241
	fsw	ft7, 0(t0)
			# store [t223], t242
	fsw	ft4, 0(t2)
			# t243 = load mat_j126
	la	t6, mat_j126
	lw	t3, 0(t6)
			# t244 = + t243, t132
	addi	t4, t3, 1
			# store [t129], t244
	la	t5, mat_j126
	sw	t4, 0(t5)
			# goto L20
	la	t6, t_235
	fsw	ft1, 0(t6)
	la	t6, t_238
	fsw	ft2, 0(t6)
	la	t6, t_239
	fsw	ft3, 0(t6)
	la	t6, t_242
	fsw	ft4, 0(t6)
	la	t6, t_240
	fsw	ft5, 0(t6)
	la	t6, t_237
	fsw	ft6, 0(t6)
	la	t6, t_241
	fsw	ft7, 0(t6)
	la	t6, t_230
	fsw	fa1, 0(t6)
	j	L20
			# label L21
L21:
			# t245 = load mat_r127
	la	t6, mat_r127
	lw	t0, 0(t6)
			# t246 = + t245, t132
	addi	t1, t0, 1
			# store [t130], t246
	la	t2, mat_r127
	sw	t1, 0(t2)
			# goto L17
	j	L17
			# label L18
L18:
			# t247 = load mat_i125
	la	t6, mat_i125
	lw	t0, 0(t6)
			# t248 = + t247, t132
	addi	t1, t0, 1
			# store [t128], t248
	la	t2, mat_i125
	sw	t1, 0(t2)
			# goto L11
	j	L11
			# label L12
L12:
			# print_str "det:"
	la	a0, .Ls0
	li	a7, 4
	ecall
			# t249 = load detv
	la	t6, detv
	lw	t0, 0(t6)
			# print t249
	mv	a0, t0
	li	a7, 1
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# print_str "\ninv:"
	la	a0, .Ls1
	li	a7, 4
	ecall
			# t250 = addr Inv
	la	t1, Inv
			# t251 = mul t32, cols(Inv)
	li	t2, 0
	li	t6, 3
	mul	t3, t2, t6
			# t252 = add t251, t32
	addi	t4, t3, 0
			# t253 = mul t252, 4
	slli	t5, t4, 2
			# t254 = add t250, t253
	add	t0, t1, t5
			# t255 = load [t254]
	flw	ft1, 0(t0)
			# print t255
	fsgnj.s	fa0, ft1, ft1
	li	a7, 2
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# t256 = addr Inv
	la	t1, Inv
			# t257 = mul t32, cols(Inv)
	la	t6, t_32
	sw	t2, 0(t6)
	li	t6, 3
	mul	t2, t2, t6
			# t258 = add t257, t8
	addi	t3, t2, 1
			# t259 = mul t258, 4
	slli	t4, t3, 2
			# t260 = add t256, t259
	add	t5, t1, t4
			# t261 = load [t260]
	flw	ft2, 0(t5)
			# print t261
	fsgnj.s	fa0, ft2, ft2
	li	a7, 2
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# t262 = addr Inv
	la	t0, Inv
			# t263 = mul t32, cols(Inv)
	li	t1, 0
	li	t6, 3
	mul	t2, t1, t6
			# t264 = add t263, t16
	addi	t3, t2, 2
			# t265 = mul t264, 4
	slli	t4, t3, 2
			# t266 = add t262, t265
	add	t5, t0, t4
			# t267 = load [t266]
	flw	ft3, 0(t5)
			# print t267
	fsgnj.s	fa0, ft3, ft3
	li	a7, 2
	ecall
	li	a0, 10
	li	a7, 11
	ecall
			# ret t32
	la	t6, t_255
	fsw	ft1, 0(t6)
	la	t6, t_261
	fsw	ft2, 0(t6)
	la	t6, t_267
	fsw	ft3, 0(t6)
			# IR_END
	# IR_END

.Lexit:
	li	a7, 10
	ecall
