# IR Output Samples

The following IR outputs were generated with `./compiler` from the provided sample programs.

## test_switch.cd
```text
IR_BEGIN
decl int choice
print_str "Enter a number (1-3): "
t1 = addr choice
scan [t1]
t2 = load choice
t3 = const int 1
t4 = eq t2, t3
ifz t4 goto L3
label L2
print_str "You selected ONE\n"
goto L1
goto L1
label L3
t5 = const int 2
t6 = eq t2, t5
ifz t6 goto L5
label L4
print_str "You selected TWO\n"
goto L1
goto L1
label L5
t7 = const int 3
t8 = eq t2, t7
ifz t8 goto L7
label L6
print_str "You selected THREE\n"
goto L1
goto L1
label L7
label L8
print_str "Invalid choice\n"
label L1
ret
IR_END
```

## test_bubble_sort.cd
```text
IR_BEGIN
decl int i
decl int j
decl int temp
decl int n
decl_array a, 10
print_str "Enter number of elements:"
t1 = addr n
scan [t1]
t2 = load n
t3 = const int 0
t4 = lt t2, t3
t5 = load n
t6 = const int 10
t7 = gt t5, t6
t8 = or t4, t7
ifz t8 goto L1
print_str "Invalid size. Enter a value between 0 and 10."
t9 = const int 0
ret t9
label L1
print_str "Enter array elements:"
t10 = addr i
t11 = const int 0
store [t10], t11
label L2
t12 = load i
t13 = load n
t14 = lt t12, t13
ifz t14 goto L3
t16 = load i
t15 = addr a
t17 = mul t16, 1
t18 = add t15, t17
scan [t18]
label L4
t19 = addr i
t20 = load i
t21 = const int 1
t22 = + t20, t21
store [t19], t22
goto L2
label L3
t23 = addr i
t24 = const int 0
store [t23], t24
label L5
t25 = load i
t26 = load n
t27 = const int 1
t28 = - t26, t27
t29 = lt t25, t28
ifz t29 goto L6
t30 = addr j
t31 = const int 0
store [t30], t31
label L8
t32 = load j
t33 = load n
t34 = load i
t35 = - t33, t34
t36 = const int 1
t37 = - t35, t36
t38 = lt t32, t37
ifz t38 goto L9
t40 = load j
t39 = addr a
t41 = mul t40, 1
t42 = add t39, t41
t43 = load [t42]
t45 = load j
t46 = const int 1
t47 = + t45, t46
t44 = addr a
t48 = mul t47, 1
t49 = add t44, t48
t50 = load [t49]
t51 = gt t43, t50
ifz t51 goto L11
t52 = addr temp
t54 = load j
t53 = addr a
t55 = mul t54, 1
t56 = add t53, t55
t57 = load [t56]
store [t52], t57
t59 = load j
t58 = addr a
t60 = mul t59, 1
t61 = add t58, t60
t63 = load j
t64 = const int 1
t65 = + t63, t64
t62 = addr a
t66 = mul t65, 1
t67 = add t62, t66
t68 = load [t67]
store [t61], t68
t70 = load j
t71 = const int 1
t72 = + t70, t71
t69 = addr a
t73 = mul t72, 1
t74 = add t69, t73
t75 = load temp
store [t74], t75
label L11
label L10
t76 = addr j
t77 = load j
t78 = const int 1
t79 = + t77, t78
store [t76], t79
goto L8
label L9
label L7
t80 = addr i
t81 = load i
t82 = const int 1
t83 = + t81, t82
store [t80], t83
goto L5
label L6
print_str "Sorted Array:"
t84 = addr i
t85 = const int 0
store [t84], t85
label L12
t86 = load i
t87 = load n
t88 = lt t86, t87
ifz t88 goto L13
t90 = load i
t89 = addr a
t91 = mul t90, 1
t92 = add t89, t91
t93 = load [t92]
print t93
label L14
t94 = addr i
t95 = load i
t96 = const int 1
t97 = + t95, t96
store [t94], t97
goto L12
label L13
t98 = const int 0
ret t98
IR_END
```
