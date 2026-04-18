	.data
	.align	2

a:	.space	16
inva:	.space	16
e:	.word	0
mat_tmpA65:	.space	16
mat_r66:	.word	0
mat_c67:	.word	0
mat_i95:	.word	0
mat_j96:	.word	0
mat_r97:	.word	0
mat_pivot130:	.word	0

t_33:	.word	0
t_34:	.word	0
t_35:	.word	0
t_36:	.word	0
t_41:	.word	0
t_46:	.word	0
t_51:	.word	0
t_56:	.word	0
t_57:	.word	0
t_58:	.word	0
t_59:	.word	0
t_70:	.word	0
t_71:	.word	0
t_72:	.word	0
t_73:	.word	0
t_74:	.word	0
t_75:	.word	0
t_79:	.word	0
t_81:	.word	0
t_101:	.word	0
t_102:	.word	0
tf_103:	.word	0
tf_104:	.word	0
t_105:	.word	0
t_107:	.word	0
t_109:	.word	0
t_110:	.word	0
t_111:	.word	0
t_112:	.word	0
t_128:	.word	0
t_129:	.word	0
t_132:	.word	0
t_133:	.word	0
tf_140:	.word	0
t_141:	.word	0
t_143:	.word	0
t_144:	.word	0
t_146:	.word	0
t_147:	.word	0
t_152:	.word	0
t_157:	.word	0
t_162:	.word	0
t_167:	.word	0
tf_170:	.word	0
tf_171:	.word	0
tf_181:	.word	0
tf_182:	.word	0
tf_183:	.word	0
tf_184:	.word	0
tf_185:	.word	0
tf_186:	.word	0
t_187:	.word	0
t_189:	.word	0
t_190:	.word	0
t_195:	.word	0
tf_202:	.word	0
tf_203:	.word	0
tf_204:	.word	0
tf_205:	.word	0
tf_206:	.word	0
t_210:	.word	0
t_212:	.word	0
t_213:	.word	0
t_215:	.word	0
t_216:	.word	0
tf_222:	.word	0
tf_223:	.word	0
t_225:	.word	0
t_226:	.word	0
t_231:	.word	0
t_236:	.word	0
t_241:	.word	0
tf_251:	.word	0
tf_252:	.word	0
tf_253:	.word	0
tf_254:	.word	0
tf_255:	.word	0
tf_256:	.word	0
tf_257:	.word	0
tf_258:	.word	0
tf_265:	.word	0
tf_273:	.word	0
tf_281:	.word	0
tf_289:	.word	0
tf_297:	.word	0
t_298:	.word	0

.Ls0:	.asciz	"Det:"
.Ls1:	.asciz	"inv:"

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
	addi	t1, x0, 2
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 0
	addi	t3, x0, 1
	la	t4, a
	slli	t5, t2, 1
	addi	t0, t5, 1
	slli	t1, t0, 2
	add	t2, t4, t1
	addi	t3, x0, 1
	addi	a5, t2, 0
	sw	t3, 0(a5)
	addi	t4, x0, 1
	addi	t5, x0, 0
	la	t0, a
	slli	t1, t4, 1
	addi	t2, t1, 0
	slli	t3, t2, 2
	add	t4, t0, t3
	addi	t5, x0, 5
	addi	a5, t4, 0
	sw	t5, 0(a5)
	addi	t0, x0, 1
	addi	t1, x0, 1
	la	t2, a
	slli	t3, t0, 1
	addi	t4, t3, 1
	slli	t5, t4, 2
	add	t0, t2, t5
	addi	t1, x0, 3
	addi	a5, t0, 0
	sw	t1, 0(a5)
	addi	t2, x0, 0
	addi	t3, x0, 1
	addi	t4, x0, 0
	addi	t5, x0, 1
	la	t0, a
	slli	t1, t2, 1
	la	t6, t_33
	sw	t2, 0(t6)
	addi	t2, t1, 0
	la	t6, t_34
	sw	t3, 0(t6)
	slli	t3, t2, 2
	la	t6, t_35
	sw	t4, 0(t6)
	add	t4, t0, t3
	la	t6, t_36
	sw	t5, 0(t6)
	la	t5, a
	addi	t0, x0, 0
	slli	t1, t0, 1
	addi	t2, t1, 1
	slli	t3, t2, 2
	la	t6, t_41
	sw	t4, 0(t6)
	add	t4, t5, t3
	la	t5, a
	addi	t0, x0, 1
	slli	t1, t0, 1
	addi	t2, t1, 0
	slli	t3, t2, 2
	la	t6, t_46
	sw	t4, 0(t6)
	add	t4, t5, t3
	la	t5, a
	la	t6, t_34
	sw	t0, 0(t6)
	slli	t0, t0, 1
	addi	t1, t0, 1
	slli	t2, t1, 2
	add	t3, t5, t2
	la	t6, t_51
	sw	t4, 0(t6)
	la	t6, t_41
	lw	t4, 0(t6)
	lw	t5, 0(t4)
	la	t6, t_46
	lw	t0, 0(t6)
	lw	t1, 0(t0)
	la	t6, t_51
	lw	t2, 0(t6)
	la	t6, t_56
	sw	t3, 0(t6)
	lw	t3, 0(t2)
	la	t6, t_56
	lw	t4, 0(t6)
	la	t6, t_57
	sw	t5, 0(t6)
	lw	t5, 0(t4)
	la	t6, t_57
	lw	t0, 0(t6)
	la	t6, t_58
	sw	t1, 0(t6)
	mul	t1, t0, t5
	la	t6, t_58
	lw	t2, 0(t6)
	la	t6, t_59
	sw	t3, 0(t6)
	mul	t3, t2, t3
	sub	t4, t1, t3
	la	t5, e
	addi	a5, t5, 0
	fcvt.s.w	ft8, t4
	fsw	ft8, 0(a5)
	la	t0, mat_r66
	la	t1, mat_c67
	addi	t2, x0, 0
	addi	t3, x0, 1
	addi	t4, x0, 2
	addi	t5, x0, 2
	addi	t0, x0, 4
	addi	t1, x0, 8
	la	t6, t_70
	sw	t2, 0(t6)
	la	t2, a
	la	t6, t_71
	sw	t3, 0(t6)
	la	t3, mat_tmpA65
	la	t6, t_72
	sw	t4, 0(t6)
	la	t4, mat_r66
	addi	a5, t4, 0
	la	t6, t_73
	sw	t5, 0(t6)
	addi	t5, x0, 0
	sw	t5, 0(a5)
	la	t6, t_74
	sw	t0, 0(t6)
	la	t6, t_75
	sw	t1, 0(t6)
	la	t6, t_70
	sw	t5, 0(t6)
L1:
	la	t6, mat_r66
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_79
	sw	t2, 0(t6)
	beqz	t2, L2
	la	t0, mat_c67
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
L3:
	la	t6, mat_c67
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_81
	sw	t2, 0(t6)
	beqz	t2, L4
	la	t6, mat_r66
	lw	t0, 0(t6)
	slli	t1, t0, 3
	la	t2, a
	add	t3, t2, t1
	la	t4, mat_tmpA65
	add	t5, t4, t1
	la	t6, mat_c67
	lw	t0, 0(t6)
	slli	t1, t0, 2
	add	t2, t3, t1
	add	t3, t5, t1
	lw	t4, 0(t2)
	addi	a5, t3, 0
	fcvt.s.w	ft8, t4
	fsw	ft8, 0(a5)
	la	t6, mat_c67
	lw	t5, 0(t6)
	addi	t0, t5, 1
	la	t1, mat_c67
	addi	a5, t1, 0
	sw	t0, 0(a5)
	j	L3
L4:
	la	t6, mat_r66
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_r66
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L1
L2:
	la	t0, mat_i95
	la	t1, mat_j96
	la	t2, mat_r97
	addi	t3, x0, 0
	addi	t4, x0, 1
	addi	t6, x0, 0
	fmv.w.x	ft1, t6
	li	t6, 1065353216
	fmv.w.x	ft2, t6
	addi	t5, x0, 2
	addi	a5, t0, 0
	sw	t3, 0(a5)
	la	t6, t_101
	sw	t3, 0(t6)
	la	t6, t_102
	sw	t4, 0(t6)
	la	t6, t_105
	sw	t5, 0(t6)
	la	t6, tf_103
	fsw	ft1, 0(t6)
	la	t6, tf_104
	fsw	ft2, 0(t6)
L5:
	la	t6, mat_i95
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_105
	sw	t1, 0(t6)
	la	t6, t_107
	sw	t2, 0(t6)
	beqz	t2, L6
	la	t0, mat_j96
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, t_101
	sw	t1, 0(t6)
L7:
	la	t6, mat_j96
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_105
	sw	t1, 0(t6)
	la	t6, t_109
	sw	t2, 0(t6)
	beqz	t2, L8
	la	t6, mat_i95
	lw	t0, 0(t6)
	la	t6, mat_j96
	lw	t1, 0(t6)
	sub	t2, t0, t1
	seqz	t2, t2
	la	t6, t_110
	sw	t0, 0(t6)
	la	t6, t_111
	sw	t1, 0(t6)
	la	t6, t_112
	sw	t2, 0(t6)
	beqz	t2, L9
	la	t0, inva
	la	t6, t_110
	lw	t1, 0(t6)
	slli	t2, t1, 1
	la	t6, t_111
	lw	t3, 0(t6)
	add	t4, t2, t3
	slli	t5, t4, 2
	la	t6, t_110
	sw	t1, 0(t6)
	la	t1, inva
	add	t0, t1, t5
	addi	a5, t0, 0
	li	t6, 1065353216
	fmv.w.x	ft1, t6
	fsw	ft1, 0(a5)
	la	t6, t_111
	sw	t3, 0(t6)
	la	t6, tf_104
	fsw	ft1, 0(t6)
	j	L10
L9:
	la	t0, inva
	la	t6, t_110
	lw	t1, 0(t6)
	slli	t2, t1, 1
	la	t6, t_111
	lw	t3, 0(t6)
	add	t4, t2, t3
	slli	t5, t4, 2
	la	t1, inva
	add	t0, t1, t5
	addi	a5, t0, 0
	addi	t6, x0, 0
	fmv.w.x	ft1, t6
	fsw	ft1, 0(a5)
	la	t6, tf_103
	fsw	ft1, 0(t6)
L10:
	la	t6, mat_j96
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_j96
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L7
L8:
	la	t6, mat_i95
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_i95
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L5
L6:
	la	t0, mat_i95
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, t_101
	sw	t1, 0(t6)
L11:
	la	t6, mat_i95
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_105
	sw	t1, 0(t6)
	la	t6, t_128
	sw	t2, 0(t6)
	beqz	t2, L12
	la	t0, mat_pivot130
	la	t6, mat_i95
	lw	t1, 0(t6)
	addi	a5, t0, 0
	sw	t1, 0(a5)
	la	t6, t_129
	sw	t1, 0(t6)
L13:
	la	t6, mat_pivot130
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_132
	sw	t0, 0(t6)
	la	t6, t_105
	sw	t1, 0(t6)
	la	t6, t_133
	sw	t2, 0(t6)
	beqz	t2, L15
	la	t0, mat_tmpA65
	la	t6, t_132
	lw	t1, 0(t6)
	slli	t2, t1, 1
	la	t6, t_129
	lw	t3, 0(t6)
	add	t4, t2, t3
	slli	t5, t4, 2
	la	t6, t_132
	sw	t1, 0(t6)
	la	t1, mat_tmpA65
	add	t0, t1, t5
	lw	t2, 0(t0)
	fcvt.s.w	ft8, t2
	li	t6, 1065353216
	fmv.w.x	ft1, t6
	fmul.s	ft0, ft8, ft1
	la	t6, tf_140
	fsw	ft0, 0(t6)
	la	t6, tf_140
	flw	ft2, 0(t6)
	addi	t6, x0, 0
	fmv.w.x	ft3, t6
	la	t6, t_129
	sw	t3, 0(t6)
	feq.s	t3, ft2, ft3
	la	t6, t_141
	sw	t3, 0(t6)
	la	t6, tf_104
	fsw	ft1, 0(t6)
	la	t6, tf_140
	fsw	ft2, 0(t6)
	la	t6, tf_103
	fsw	ft3, 0(t6)
	beqz	t3, L14
	la	t6, t_132
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_pivot130
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L13
L14:
	la	t6, mat_pivot130
	lw	t0, 0(t6)
	la	t6, t_129
	lw	t1, 0(t6)
	sub	t2, t0, t1
	seqz	t2, t2
	la	t6, t_143
	sw	t0, 0(t6)
	la	t6, t_129
	sw	t1, 0(t6)
	la	t6, t_144
	sw	t2, 0(t6)
	beqz	t2, L17
	j	L16
L17:
	la	t0, mat_j96
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, t_101
	sw	t1, 0(t6)
L18:
	la	t6, mat_j96
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_105
	sw	t1, 0(t6)
	la	t6, t_146
	sw	t2, 0(t6)
	beqz	t2, L19
	la	t6, mat_j96
	lw	t0, 0(t6)
	la	t1, mat_tmpA65
	la	t6, t_129
	lw	t2, 0(t6)
	slli	t3, t2, 1
	add	t4, t3, t0
	slli	t5, t4, 2
	la	t6, t_147
	sw	t0, 0(t6)
	add	t0, t1, t5
	la	t1, mat_tmpA65
	la	t6, t_129
	sw	t2, 0(t6)
	la	t6, t_143
	lw	t2, 0(t6)
	slli	t3, t2, 1
	la	t6, t_147
	lw	t4, 0(t6)
	add	t5, t3, t4
	la	t6, t_152
	sw	t0, 0(t6)
	slli	t0, t5, 2
	la	t6, t_143
	sw	t2, 0(t6)
	la	t2, mat_tmpA65
	add	t1, t2, t0
	la	t3, inva
	la	t6, t_147
	sw	t4, 0(t6)
	la	t6, t_129
	lw	t4, 0(t6)
	slli	t5, t4, 1
	la	t6, t_147
	lw	t0, 0(t6)
	la	t6, t_157
	sw	t1, 0(t6)
	add	t1, t5, t0
	slli	t2, t1, 2
	la	t6, t_129
	sw	t4, 0(t6)
	la	t4, inva
	add	t3, t4, t2
	la	t5, inva
	la	t6, t_147
	sw	t0, 0(t6)
	la	t6, t_143
	lw	t0, 0(t6)
	slli	t1, t0, 1
	la	t6, t_147
	lw	t2, 0(t6)
	la	t6, t_162
	sw	t3, 0(t6)
	add	t3, t1, t2
	slli	t4, t3, 2
	la	t0, inva
	add	t5, t0, t4
	la	t6, t_152
	lw	t1, 0(t6)
	lw	t2, 0(t1)
	la	t6, t_157
	lw	t3, 0(t6)
	lw	t4, 0(t3)
	la	t6, t_167
	sw	t5, 0(t6)
	la	t6, t_162
	lw	t5, 0(t6)
	flw	ft1, 0(t5)
	la	t6, t_167
	lw	t0, 0(t6)
	flw	ft2, 0(t0)
	addi	a5, t1, 0
	fcvt.s.w	ft8, t4
	fsw	ft8, 0(a5)
	addi	a5, t3, 0
	fcvt.s.w	ft8, t2
	fsw	ft8, 0(a5)
	addi	a5, t5, 0
	fsw	ft2, 0(a5)
	addi	a5, t0, 0
	fsw	ft1, 0(a5)
	la	t6, mat_j96
	lw	t1, 0(t6)
	addi	t2, t1, 1
	la	t3, mat_j96
	addi	a5, t3, 0
	sw	t2, 0(a5)
	la	t6, tf_170
	fsw	ft1, 0(t6)
	la	t6, tf_171
	fsw	ft2, 0(t6)
	j	L18
L19:
	j	L16
L15:
	addi	t0, x0, 2
	la	t1, mat_i95
	addi	a5, t1, 0
	sw	t0, 0(a5)
	j	L12
L16:
	la	t0, mat_tmpA65
	la	t6, t_129
	lw	t1, 0(t6)
	slli	t2, t1, 1
	add	t3, t2, t1
	slli	t4, t3, 2
	add	t5, t0, t4
	lw	t0, 0(t5)
	li	t6, 1065353216
	fmv.w.x	ft1, t6
	addi	t6, x0, 0
	fmv.w.x	ft2, t6
	li	t6, 1065353216
	fmv.w.x	ft3, t6
	fmul.s	ft0, ft1, ft3
	la	t6, tf_185
	fsw	ft0, 0(t6)
	fcvt.s.w	ft8, t0
	fmul.s	ft0, ft8, ft3
	la	t6, tf_186
	fsw	ft0, 0(t6)
	la	t6, tf_186
	flw	ft4, 0(t6)
	la	t6, t_129
	sw	t1, 0(t6)
	feq.s	t1, ft4, ft2
	la	t6, t_187
	sw	t1, 0(t6)
	la	t6, tf_181
	fsw	ft1, 0(t6)
	la	t6, tf_183
	fsw	ft2, 0(t6)
	la	t6, tf_184
	fsw	ft3, 0(t6)
	la	t6, tf_186
	fsw	ft4, 0(t6)
	beqz	t1, L20
	addi	t6, x0, 0
	fmv.w.x	ft1, t6
	la	t6, tf_182
	fsw	ft1, 0(t6)
	j	L21
L20:
	la	t6, tf_185
	flw	ft1, 0(t6)
	la	t6, tf_186
	flw	ft2, 0(t6)
	fdiv.s	ft0, ft1, ft2
	la	t6, tf_182
	fsw	ft0, 0(t6)
	la	t6, tf_185
	fsw	ft1, 0(t6)
	la	t6, tf_186
	fsw	ft2, 0(t6)
L21:
	la	t0, mat_j96
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, t_101
	sw	t1, 0(t6)
L22:
	la	t6, mat_j96
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_105
	sw	t1, 0(t6)
	la	t6, t_189
	sw	t2, 0(t6)
	beqz	t2, L23
	la	t6, mat_j96
	lw	t0, 0(t6)
	la	t1, mat_tmpA65
	la	t6, t_129
	lw	t2, 0(t6)
	slli	t3, t2, 1
	add	t4, t3, t0
	slli	t5, t4, 2
	la	t6, t_190
	sw	t0, 0(t6)
	add	t0, t1, t5
	la	t1, inva
	la	t6, t_129
	sw	t2, 0(t6)
	slli	t2, t2, 1
	la	t6, t_190
	lw	t3, 0(t6)
	add	t4, t2, t3
	slli	t5, t4, 2
	la	t6, t_195
	sw	t0, 0(t6)
	add	t0, t1, t5
	la	t6, t_195
	lw	t1, 0(t6)
	lw	t2, 0(t1)
	flw	ft1, 0(t0)
	fcvt.s.w	ft8, t2
	li	t6, 1065353216
	fmv.w.x	ft2, t6
	fmul.s	ft0, ft8, ft2
	la	t6, tf_203
	fsw	ft0, 0(t6)
	fmul.s	ft0, ft1, ft2
	la	t6, tf_204
	fsw	ft0, 0(t6)
	la	t6, tf_203
	flw	ft3, 0(t6)
	la	t6, tf_182
	flw	ft4, 0(t6)
	fmul.s	ft0, ft3, ft4
	la	t6, tf_205
	fsw	ft0, 0(t6)
	la	t6, tf_204
	flw	ft5, 0(t6)
	fmul.s	ft0, ft5, ft4
	la	t6, tf_206
	fsw	ft0, 0(t6)
	addi	a5, t1, 0
	la	t6, tf_205
	flw	ft6, 0(t6)
	fsw	ft6, 0(a5)
	addi	a5, t0, 0
	la	t6, tf_206
	flw	ft7, 0(t6)
	fsw	ft7, 0(a5)
	la	t6, mat_j96
	lw	t3, 0(t6)
	addi	t4, t3, 1
	la	t5, mat_j96
	addi	a5, t5, 0
	sw	t4, 0(a5)
	la	t6, tf_202
	fsw	ft1, 0(t6)
	la	t6, tf_104
	fsw	ft2, 0(t6)
	la	t6, tf_203
	fsw	ft3, 0(t6)
	la	t6, tf_182
	fsw	ft4, 0(t6)
	la	t6, tf_204
	fsw	ft5, 0(t6)
	la	t6, tf_205
	fsw	ft6, 0(t6)
	la	t6, tf_206
	fsw	ft7, 0(t6)
	j	L22
L23:
	la	t0, mat_r97
	addi	a5, t0, 0
	addi	t1, x0, 0
	sw	t1, 0(a5)
	la	t6, t_101
	sw	t1, 0(t6)
L24:
	la	t6, mat_r97
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_105
	sw	t1, 0(t6)
	la	t6, t_210
	sw	t2, 0(t6)
	beqz	t2, L25
	la	t6, mat_i95
	lw	t0, 0(t6)
	la	t6, mat_r97
	lw	t1, 0(t6)
	sub	t2, t1, t0
	seqz	t2, t2
	la	t6, t_212
	sw	t1, 0(t6)
	la	t6, t_213
	sw	t2, 0(t6)
	beqz	t2, L26
	la	t6, t_212
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_r97
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L24
L26:
	la	t6, mat_i95
	lw	t0, 0(t6)
	la	t6, mat_r97
	lw	t1, 0(t6)
	la	t2, mat_tmpA65
	slli	t3, t1, 1
	add	t4, t3, t0
	slli	t5, t4, 2
	la	t6, t_215
	sw	t0, 0(t6)
	add	t0, t2, t5
	flw	ft1, 0(t0)
	li	t6, 1065353216
	fmv.w.x	ft2, t6
	fmul.s	ft0, ft1, ft2
	la	t6, tf_223
	fsw	ft0, 0(t6)
	la	t6, t_216
	sw	t1, 0(t6)
	la	t1, mat_j96
	addi	a5, t1, 0
	addi	t2, x0, 0
	sw	t2, 0(a5)
	la	t6, tf_222
	fsw	ft1, 0(t6)
	la	t6, tf_104
	fsw	ft2, 0(t6)
L27:
	la	t6, mat_j96
	lw	t0, 0(t6)
	addi	t1, x0, 2
	slt	t2, t0, t1
	la	t6, t_225
	sw	t2, 0(t6)
	beqz	t2, L28
	la	t6, mat_j96
	lw	t0, 0(t6)
	la	t1, mat_tmpA65
	la	t6, t_216
	lw	t2, 0(t6)
	slli	t3, t2, 1
	add	t4, t3, t0
	slli	t5, t4, 2
	la	t6, t_226
	sw	t0, 0(t6)
	add	t0, t1, t5
	la	t1, mat_tmpA65
	la	t6, t_216
	sw	t2, 0(t6)
	la	t6, t_215
	lw	t2, 0(t6)
	slli	t3, t2, 1
	la	t6, t_226
	lw	t4, 0(t6)
	add	t5, t3, t4
	la	t6, t_231
	sw	t0, 0(t6)
	slli	t0, t5, 2
	la	t6, t_215
	sw	t2, 0(t6)
	la	t2, mat_tmpA65
	add	t1, t2, t0
	la	t3, inva
	la	t6, t_226
	sw	t4, 0(t6)
	la	t6, t_216
	lw	t4, 0(t6)
	slli	t5, t4, 1
	la	t6, t_226
	lw	t0, 0(t6)
	la	t6, t_236
	sw	t1, 0(t6)
	add	t1, t5, t0
	slli	t2, t1, 2
	la	t4, inva
	add	t3, t4, t2
	la	t5, inva
	la	t6, t_226
	sw	t0, 0(t6)
	la	t6, t_215
	lw	t0, 0(t6)
	slli	t1, t0, 1
	la	t6, t_226
	lw	t2, 0(t6)
	la	t6, t_241
	sw	t3, 0(t6)
	add	t3, t1, t2
	slli	t4, t3, 2
	la	t0, inva
	add	t5, t0, t4
	la	t6, t_231
	lw	t1, 0(t6)
	flw	ft1, 0(t1)
	la	t6, t_236
	lw	t2, 0(t6)
	flw	ft2, 0(t2)
	la	t6, t_241
	lw	t3, 0(t6)
	flw	ft3, 0(t3)
	flw	ft4, 0(t5)
	li	t6, 1065353216
	fmv.w.x	ft5, t6
	fmul.s	ft0, ft1, ft5
	la	t6, tf_251
	fsw	ft0, 0(t6)
	fmul.s	ft0, ft2, ft5
	la	t6, tf_252
	fsw	ft0, 0(t6)
	fmul.s	ft0, ft3, ft5
	la	t6, tf_253
	fsw	ft0, 0(t6)
	fmul.s	ft0, ft4, ft5
	la	t6, tf_254
	fsw	ft0, 0(t6)
	la	t6, tf_223
	flw	ft6, 0(t6)
	la	t6, tf_252
	flw	ft7, 0(t6)
	fmul.s	ft0, ft6, ft7
	la	t6, tf_255
	fsw	ft0, 0(t6)
	la	t6, tf_254
	flw	fa1, 0(t6)
	fmul.s	ft0, ft6, fa1
	la	t6, tf_256
	fsw	ft0, 0(t6)
	la	t6, tf_251
	flw	ft1, 0(t6)
	la	t6, tf_255
	flw	ft2, 0(t6)
	fsub.s	ft0, ft1, ft2
	la	t6, tf_257
	fsw	ft0, 0(t6)
	la	t6, tf_253
	flw	ft3, 0(t6)
	la	t6, tf_256
	flw	ft4, 0(t6)
	fsub.s	ft0, ft3, ft4
	la	t6, tf_258
	fsw	ft0, 0(t6)
	addi	a5, t1, 0
	la	t6, tf_257
	flw	ft5, 0(t6)
	fsw	ft5, 0(a5)
	addi	a5, t3, 0
	la	t6, tf_258
	flw	ft7, 0(t6)
	fsw	ft7, 0(a5)
	la	t6, mat_j96
	lw	t4, 0(t6)
	addi	t5, t4, 1
	la	t0, mat_j96
	addi	a5, t0, 0
	sw	t5, 0(a5)
	la	t6, tf_251
	fsw	ft1, 0(t6)
	la	t6, tf_255
	fsw	ft2, 0(t6)
	la	t6, tf_253
	fsw	ft3, 0(t6)
	la	t6, tf_256
	fsw	ft4, 0(t6)
	la	t6, tf_257
	fsw	ft5, 0(t6)
	la	t6, tf_223
	fsw	ft6, 0(t6)
	la	t6, tf_258
	fsw	ft7, 0(t6)
	la	t6, tf_254
	fsw	fa1, 0(t6)
	j	L27
L28:
	la	t6, mat_r97
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_r97
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L24
L25:
	la	t6, mat_i95
	lw	t0, 0(t6)
	addi	t1, t0, 1
	la	t2, mat_i95
	addi	a5, t2, 0
	sw	t1, 0(a5)
	j	L11
L12:
	la	a0, .Ls0
	addi	a7, x0, 4
	ecall
	la	t6, e
	flw	ft1, 0(t6)
	fsgnj.s	fa0, ft1, ft1
	addi	a7, x0, 2
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	la	a0, .Ls1
	addi	a7, x0, 4
	ecall
	addi	t0, x0, 0
	addi	t1, x0, 0
	la	t2, inva
	slli	t3, t0, 1
	addi	t4, t3, 0
	slli	t5, t4, 2
	add	t0, t2, t5
	flw	ft2, 0(t0)
	fsgnj.s	fa0, ft2, ft2
	addi	a7, x0, 2
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t1, x0, 0
	addi	t2, x0, 1
	la	t3, inva
	slli	t4, t1, 1
	addi	t5, t4, 1
	slli	t0, t5, 2
	add	t1, t3, t0
	flw	ft3, 0(t1)
	fsgnj.s	fa0, ft3, ft3
	addi	a7, x0, 2
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t2, x0, 1
	addi	t3, x0, 0
	la	t4, inva
	slli	t5, t2, 1
	addi	t0, t5, 0
	slli	t1, t0, 2
	add	t2, t4, t1
	flw	ft4, 0(t2)
	fsgnj.s	fa0, ft4, ft4
	addi	a7, x0, 2
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t3, x0, 1
	addi	t4, x0, 1
	la	t5, inva
	slli	t0, t3, 1
	addi	t1, t0, 1
	slli	t2, t1, 2
	add	t3, t5, t2
	flw	ft5, 0(t3)
	fsgnj.s	fa0, ft5, ft5
	addi	a7, x0, 2
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
	addi	t4, x0, 0
	la	t6, t_298
	sw	t4, 0(t6)
	la	t6, tf_265
	fsw	ft1, 0(t6)
	la	t6, tf_273
	fsw	ft2, 0(t6)
	la	t6, tf_281
	fsw	ft3, 0(t6)
	la	t6, tf_289
	fsw	ft4, 0(t6)
	la	t6, tf_297
	fsw	ft5, 0(t6)

.Lexit:
	addi	a7, x0, 10
	ecall
