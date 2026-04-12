# RISC-V (RV32IM) — compiler project backend
# Source IR : output.ir

	.data
	.align	2

a:	.space	16	# 4 words
b:	.space	16	# 4 words
c:	.space	16	# 4 words

	# temporaries
t_8:	.word	0
t_80:	.word	0
t_83:	.word	0
t_106:	.word	0
t_120:	.word	0
t_122:	.word	0
t_123:	.word	0
t_128:	.word	0
t_137:	.word	0
t_160:	.word	0
t_163:	.word	0
t_186:	.word	0
t_200:	.word	0
t_202:	.word	0
t_203:	.word	0
t_208:	.word	0
t_217:	.word	0
t_226:	.word	0

	# strings
.Ls0:	.asciz	"c:"

	.text
	.globl	main
main:
			# IR_BEGIN
	# IR_BEGIN
			# decl_matrix a, 2, 2
	# decl_matrix a, 2, 2
			# t1 = const int 0
	addi	t0, x0, 0
			# t3 = addr a
	la	t1, a
			# t4 = mul t1, cols(a)
	slli	t2, t0, 1
			# t6 = mul t4, 4
	slli	t3, t2, 2
			# t7 = add t3, t6
	add	t4, t1, t3
			# t8 = const int 1
	addi	t5, x0, 1
			# store [t7], t8
	sw	t5, 0(t4)
			# t9 = const int 0
	addi	t0, x0, 0
			# t10 = const int 1
	addi	t1, x0, 1
			# t11 = addr a
	la	t2, a
			# t12 = mul t9, cols(a)
	slli	t3, t0, 1
			# t13 = add t12, t10
	addi	t4, t3, 1
			# t14 = mul t13, 4
	la	t6, t_8
	sw	t5, 0(t6)
	slli	t5, t4, 2
			# t15 = add t11, t14
	add	t0, t2, t5
			# t16 = const int 2
	addi	t1, x0, 2
			# store [t15], t16
	sw	t1, 0(t0)
			# t17 = const int 1
	addi	t2, x0, 1
			# t19 = addr a
	la	t3, a
			# t20 = mul t17, cols(a)
	slli	t4, t2, 1
			# t22 = mul t20, 4
	slli	t5, t4, 2
			# t23 = add t19, t22
	add	t0, t3, t5
			# t24 = const int 3
	addi	t1, x0, 3
			# store [t23], t24
	sw	t1, 0(t0)
			# t25 = const int 1
	addi	t2, x0, 1
			# t26 = const int 1
	addi	t3, x0, 1
			# t27 = addr a
	la	t4, a
			# t28 = mul t25, cols(a)
	slli	t5, t2, 1
			# t29 = add t28, t26
	addi	t0, t5, 1
			# t30 = mul t29, 4
	slli	t1, t0, 2
			# t31 = add t27, t30
	add	t2, t4, t1
			# t32 = const int 4
	addi	t3, x0, 4
			# store [t31], t32
	sw	t3, 0(t2)
			# decl_matrix b, 2, 2
	# decl_matrix b, 2, 2
			# t33 = const int 0
	addi	t4, x0, 0
			# t35 = addr b
	la	t5, b
			# t36 = mul t33, cols(b)
	slli	t0, t4, 1
			# t38 = mul t36, 4
	slli	t1, t0, 2
			# t39 = add t35, t38
	add	t2, t5, t1
			# t40 = const int 5
	addi	t3, x0, 5
			# store [t39], t40
	sw	t3, 0(t2)
			# t41 = const int 0
	addi	t4, x0, 0
			# t42 = const int 1
	addi	t5, x0, 1
			# t43 = addr b
	la	t0, b
			# t44 = mul t41, cols(b)
	slli	t1, t4, 1
			# t45 = add t44, t42
	addi	t2, t1, 1
			# t46 = mul t45, 4
	slli	t3, t2, 2
			# t47 = add t43, t46
	add	t4, t0, t3
			# t48 = const int 6
	addi	t5, x0, 6
			# store [t47], t48
	sw	t5, 0(t4)
			# t49 = const int 1
	addi	t0, x0, 1
			# t51 = addr b
	la	t1, b
			# t52 = mul t49, cols(b)
	slli	t2, t0, 1
			# t54 = mul t52, 4
	slli	t3, t2, 2
			# t55 = add t51, t54
	add	t4, t1, t3
			# t56 = const int 7
	addi	t5, x0, 7
			# store [t55], t56
	sw	t5, 0(t4)
			# t57 = const int 1
	addi	t0, x0, 1
			# t58 = const int 1
	addi	t1, x0, 1
			# t59 = addr b
	la	t2, b
			# t60 = mul t57, cols(b)
	slli	t3, t0, 1
			# t61 = add t60, t58
	addi	t4, t3, 1
			# t62 = mul t61, 4
	slli	t5, t4, 2
			# t63 = add t59, t62
	add	t0, t2, t5
			# t64 = const int 8
	addi	t1, x0, 8
			# store [t63], t64
	sw	t1, 0(t0)
			# decl_matrix c, 2, 2
	# decl_matrix c, 2, 2
			# t65 = const int 0
	addi	t2, x0, 0
			# t67 = const int 0
	addi	t3, x0, 0
			# t68 = addr a
	la	t4, a
			# t69 = mul t65, cols(a)
	slli	t5, t2, 1
			# t71 = mul t69, 4
	slli	t0, t5, 2
			# t72 = add t68, t71
	add	t1, t4, t0
			# t73 = addr b
	la	t2, b
			# t74 = mul t67, cols(b)
	slli	t3, t3, 1
			# t76 = mul t74, 4
	slli	t4, t3, 2
			# t77 = add t73, t76
	add	t5, t2, t4
			# t78 = load [t72]
	lw	t0, 0(t1)
			# t79 = load [t77]
	lw	t1, 0(t5)
			# t80 = * t78, t79
	mul	t2, t0, t1
			# t81 = const int 0
	addi	t3, x0, 0
			# t83 = const int 1
	addi	t4, x0, 1
			# t84 = addr a
	la	t5, a
			# t85 = mul t81, cols(a)
	slli	t0, t3, 1
			# t86 = add t85, t83
	addi	t1, t0, 1
			# t87 = mul t86, 4
	la	t6, t_80
	sw	t2, 0(t6)
	slli	t2, t1, 2
			# t88 = add t84, t87
	add	t3, t5, t2
			# t89 = addr b
	la	t6, t_83
	sw	t4, 0(t6)
	la	t4, b
			# t90 = mul t83, cols(b)
	addi	t5, x0, 1
	slli	t0, t5, 1
			# t92 = mul t90, 4
	slli	t1, t0, 2
			# t93 = add t89, t92
	add	t2, t4, t1
			# t94 = load [t88]
	addi	t6, t3, 0
	lw	t3, 0(t6)
			# t95 = load [t93]
	lw	t4, 0(t2)
			# t96 = * t94, t95
	mul	t5, t3, t4
			# t97 = + t80, t96
	la	t6, t_80
	lw	t0, 0(t6)
	add	t1, t0, t5
			# t98 = const int 0
	addi	t2, x0, 0
			# t100 = addr c
	la	t3, c
			# t101 = mul t98, cols(c)
	slli	t4, t2, 1
			# t103 = mul t101, 4
	slli	t5, t4, 2
			# t104 = add t100, t103
	add	t0, t3, t5
			# store [t104], t97
	sw	t1, 0(t0)
			# t105 = const int 0
	addi	t1, x0, 0
			# t106 = const int 1
	addi	t2, x0, 1
			# t107 = const int 0
	addi	t3, x0, 0
			# t108 = addr a
	la	t4, a
			# t109 = mul t105, cols(a)
	slli	t5, t1, 1
			# t111 = mul t109, 4
	slli	t0, t5, 2
			# t112 = add t108, t111
	add	t1, t4, t0
			# t113 = addr b
	la	t6, t_106
	sw	t2, 0(t6)
	la	t2, b
			# t114 = mul t107, cols(b)
	slli	t3, t3, 1
			# t115 = add t114, t106
	addi	t4, t3, 1
			# t116 = mul t115, 4
	slli	t5, t4, 2
			# t117 = add t113, t116
	add	t0, t2, t5
			# t118 = load [t112]
	addi	t6, t1, 0
	lw	t1, 0(t6)
			# t119 = load [t117]
	lw	t2, 0(t0)
			# t120 = * t118, t119
	mul	t3, t1, t2
			# t121 = const int 0
	addi	t4, x0, 0
			# t122 = const int 1
	addi	t5, x0, 1
			# t123 = const int 1
	addi	t0, x0, 1
			# t124 = addr a
	la	t1, a
			# t125 = mul t121, cols(a)
	slli	t2, t4, 1
			# t126 = add t125, t123
	la	t6, t_120
	sw	t3, 0(t6)
	addi	t3, t2, 1
			# t127 = mul t126, 4
	slli	t4, t3, 2
			# t128 = add t124, t127
	la	t6, t_122
	sw	t5, 0(t6)
	add	t5, t1, t4
			# t129 = addr b
	la	t6, t_123
	sw	t0, 0(t6)
	la	t0, b
			# t130 = mul t123, cols(b)
	addi	t1, x0, 1
	slli	t2, t1, 1
			# t131 = add t130, t122
	addi	t3, t2, 1
			# t132 = mul t131, 4
	slli	t4, t3, 2
			# t133 = add t129, t132
	la	t6, t_128
	sw	t5, 0(t6)
	add	t5, t0, t4
			# t134 = load [t128]
	la	t6, t_128
	lw	t0, 0(t6)
	lw	t1, 0(t0)
			# t135 = load [t133]
	lw	t2, 0(t5)
			# t136 = * t134, t135
	mul	t3, t1, t2
			# t137 = + t120, t136
	la	t6, t_120
	lw	t4, 0(t6)
	add	t5, t4, t3
			# t138 = const int 0
	addi	t0, x0, 0
			# t139 = const int 1
	addi	t1, x0, 1
			# t140 = addr c
	la	t2, c
			# t141 = mul t138, cols(c)
	slli	t3, t0, 1
			# t142 = add t141, t139
	addi	t4, t3, 1
			# t143 = mul t142, 4
	la	t6, t_137
	sw	t5, 0(t6)
	slli	t5, t4, 2
			# t144 = add t140, t143
	add	t0, t2, t5
			# store [t144], t137
	la	t6, t_137
	lw	t1, 0(t6)
	sw	t1, 0(t0)
			# t145 = const int 1
	addi	t2, x0, 1
			# t147 = const int 0
	addi	t3, x0, 0
			# t148 = addr a
	la	t4, a
			# t149 = mul t145, cols(a)
	slli	t5, t2, 1
			# t151 = mul t149, 4
	slli	t0, t5, 2
			# t152 = add t148, t151
	add	t1, t4, t0
			# t153 = addr b
	la	t2, b
			# t154 = mul t147, cols(b)
	slli	t3, t3, 1
			# t156 = mul t154, 4
	slli	t4, t3, 2
			# t157 = add t153, t156
	add	t5, t2, t4
			# t158 = load [t152]
	lw	t0, 0(t1)
			# t159 = load [t157]
	lw	t1, 0(t5)
			# t160 = * t158, t159
	mul	t2, t0, t1
			# t161 = const int 1
	addi	t3, x0, 1
			# t163 = const int 1
	addi	t4, x0, 1
			# t164 = addr a
	la	t5, a
			# t165 = mul t161, cols(a)
	slli	t0, t3, 1
			# t166 = add t165, t163
	addi	t1, t0, 1
			# t167 = mul t166, 4
	la	t6, t_160
	sw	t2, 0(t6)
	slli	t2, t1, 2
			# t168 = add t164, t167
	add	t3, t5, t2
			# t169 = addr b
	la	t6, t_163
	sw	t4, 0(t6)
	la	t4, b
			# t170 = mul t163, cols(b)
	addi	t5, x0, 1
	slli	t0, t5, 1
			# t172 = mul t170, 4
	slli	t1, t0, 2
			# t173 = add t169, t172
	add	t2, t4, t1
			# t174 = load [t168]
	addi	t6, t3, 0
	lw	t3, 0(t6)
			# t175 = load [t173]
	lw	t4, 0(t2)
			# t176 = * t174, t175
	mul	t5, t3, t4
			# t177 = + t160, t176
	la	t6, t_160
	lw	t0, 0(t6)
	add	t1, t0, t5
			# t178 = const int 1
	addi	t2, x0, 1
			# t180 = addr c
	la	t3, c
			# t181 = mul t178, cols(c)
	slli	t4, t2, 1
			# t183 = mul t181, 4
	slli	t5, t4, 2
			# t184 = add t180, t183
	add	t0, t3, t5
			# store [t184], t177
	sw	t1, 0(t0)
			# t185 = const int 1
	addi	t1, x0, 1
			# t186 = const int 1
	addi	t2, x0, 1
			# t187 = const int 0
	addi	t3, x0, 0
			# t188 = addr a
	la	t4, a
			# t189 = mul t185, cols(a)
	slli	t5, t1, 1
			# t191 = mul t189, 4
	slli	t0, t5, 2
			# t192 = add t188, t191
	add	t1, t4, t0
			# t193 = addr b
	la	t6, t_186
	sw	t2, 0(t6)
	la	t2, b
			# t194 = mul t187, cols(b)
	slli	t3, t3, 1
			# t195 = add t194, t186
	addi	t4, t3, 1
			# t196 = mul t195, 4
	slli	t5, t4, 2
			# t197 = add t193, t196
	add	t0, t2, t5
			# t198 = load [t192]
	addi	t6, t1, 0
	lw	t1, 0(t6)
			# t199 = load [t197]
	lw	t2, 0(t0)
			# t200 = * t198, t199
	mul	t3, t1, t2
			# t201 = const int 1
	addi	t4, x0, 1
			# t202 = const int 1
	addi	t5, x0, 1
			# t203 = const int 1
	addi	t0, x0, 1
			# t204 = addr a
	la	t1, a
			# t205 = mul t201, cols(a)
	slli	t2, t4, 1
			# t206 = add t205, t203
	la	t6, t_200
	sw	t3, 0(t6)
	addi	t3, t2, 1
			# t207 = mul t206, 4
	slli	t4, t3, 2
			# t208 = add t204, t207
	la	t6, t_202
	sw	t5, 0(t6)
	add	t5, t1, t4
			# t209 = addr b
	la	t6, t_203
	sw	t0, 0(t6)
	la	t0, b
			# t210 = mul t203, cols(b)
	addi	t1, x0, 1
	slli	t2, t1, 1
			# t211 = add t210, t202
	addi	t3, t2, 1
			# t212 = mul t211, 4
	slli	t4, t3, 2
			# t213 = add t209, t212
	la	t6, t_208
	sw	t5, 0(t6)
	add	t5, t0, t4
			# t214 = load [t208]
	la	t6, t_208
	lw	t0, 0(t6)
	lw	t1, 0(t0)
			# t215 = load [t213]
	lw	t2, 0(t5)
			# t216 = * t214, t215
	mul	t3, t1, t2
			# t217 = + t200, t216
	la	t6, t_200
	lw	t4, 0(t6)
	add	t5, t4, t3
			# t218 = const int 1
	addi	t0, x0, 1
			# t219 = const int 1
	addi	t1, x0, 1
			# t220 = addr c
	la	t2, c
			# t221 = mul t218, cols(c)
	slli	t3, t0, 1
			# t222 = add t221, t219
	addi	t4, t3, 1
			# t223 = mul t222, 4
	la	t6, t_217
	sw	t5, 0(t6)
	slli	t5, t4, 2
			# t224 = add t220, t223
	add	t0, t2, t5
			# store [t224], t217
	la	t6, t_217
	lw	t1, 0(t6)
	sw	t1, 0(t0)
			# print_str "c:"
	la	a0, .Ls0
	addi	a7, x0, 4
	ecall
			# t226 = const int 0
	addi	t2, x0, 0
			# t225 = addr c
	la	t3, c
			# t227 = mul t226, cols(c)
	slli	t4, t2, 1
			# t229 = mul t227, 4
	slli	t5, t4, 2
			# t230 = add t225, t229
	add	t0, t3, t5
			# t231 = load [t230]
	lw	t1, 0(t0)
			# print t231
	addi	a0, t1, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
			# t232 = addr c
	la	t6, t_226
	sw	t2, 0(t6)
	la	t2, c
			# t233 = mul t226, cols(c)
	addi	t3, x0, 0
	slli	t4, t3, 1
			# t234 = add t233, t8
	addi	t5, t4, 1
			# t235 = mul t234, 4
	slli	t0, t5, 2
			# t236 = add t232, t235
	add	t1, t2, t0
			# t237 = load [t236]
	lw	t2, 0(t1)
			# print t237
	addi	a0, t2, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
			# t238 = addr c
	la	t6, t_226
	sw	t3, 0(t6)
	la	t3, c
			# t239 = mul t8, cols(c)
	addi	t4, x0, 1
	slli	t5, t4, 1
			# t241 = mul t239, 4
	slli	t0, t5, 2
			# t242 = add t238, t241
	add	t1, t3, t0
			# t243 = load [t242]
	lw	t2, 0(t1)
			# print t243
	addi	a0, t2, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
			# t244 = addr c
	la	t3, c
			# t245 = mul t8, cols(c)
	la	t6, t_8
	sw	t4, 0(t6)
	slli	t4, t4, 1
			# t246 = add t245, t8
	addi	t5, t4, 1
			# t247 = mul t246, 4
	slli	t0, t5, 2
			# t248 = add t244, t247
	add	t1, t3, t0
			# t249 = load [t248]
	lw	t2, 0(t1)
			# print t249
	addi	a0, t2, 0
	addi	a7, x0, 1
	ecall
	addi	a0, x0, 10
	addi	a7, x0, 11
	ecall
			# ret t226
			# IR_END
	# IR_END

.Lexit:
	addi	a7, x0, 10
	ecall
