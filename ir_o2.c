#include "ir_internal.h"

#include <stdio.h>

int ir_should_use_cache_local_matmul(IROptLevel opt_level, int rows, int inner, int cols)
{
    if (opt_level < IR_OPT_O2) {
        return 0;
    }
    if (rows <= 1 || inner <= 1 || cols <= 1) {
        return 0;
    }
    if (rows <= 4 && inner <= 4 && cols <= 4) {
        return 0;
    }
    return rows * inner * cols >= 64;
}

int emit_matrix_mul_cache_locality(IRContext *ctx, const char *dst, const char *a, const char *b,
                                   int rows, int inner, int cols, FILE *out)
{
    int row_var_id;
    int bt_row_var_id;
    int k_var_id;
    int acc_var_id;
    int bt_name_id;
    char row_var[32];
    char bt_row_var[32];
    char k_var[32];
    char acc_var[32];
    char bt_name[32];
    int row_addr;
    int bt_row_addr;
    int k_addr;
    int acc_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int inner_const;
    int four;
    int inner_bytes;
    int dst_cols_bytes;
    int bt_cols_bytes;
    int base_a;
    int base_bt;
    int base_dst;
    int row_label;
    int row_end_label;
    int bt_row_label;
    int bt_row_end_label;
    int k_label;
    int k_end_label;

    if (!ctx || !dst || !a || !b || rows <= 0 || inner <= 0 || cols <= 0) {
        return 0;
    }

    bt_name_id = new_temp(ctx);
    snprintf(bt_name, sizeof(bt_name), "mat_bt%d", bt_name_id);
    ir_printf(ctx, out, "decl_matrix %s, %d, %d", bt_name, cols, inner);

    {
        int src_row_var_id = new_temp(ctx);
        int src_col_var_id = new_temp(ctx);
        char src_row_var[32];
        char src_col_var[32];
        int src_row_addr;
        int src_col_addr;
        int src_rows_const;
        int src_cols_const;
        int src_cols_bytes;
        int bt_inner_bytes;
        int base_src;
        int base_bt_local;
        int src_row_label;
        int src_row_end_label;
        int src_col_label;
        int src_col_end_label;

        snprintf(src_row_var, sizeof(src_row_var), "mat_tr%d", src_row_var_id);
        snprintf(src_col_var, sizeof(src_col_var), "mat_tc%d", src_col_var_id);
        ir_printf(ctx, out, "decl int %s", src_row_var);
        ir_printf(ctx, out, "decl int %s", src_col_var);

        src_row_addr = new_temp(ctx);
        src_col_addr = new_temp(ctx);
        ir_printf(ctx, out, "t%d = addr %s", src_row_addr, src_row_var);
        ir_printf(ctx, out, "t%d = addr %s", src_col_addr, src_col_var);

        zero = new_temp(ctx);
        one = new_temp(ctx);
        src_rows_const = new_temp(ctx);
        src_cols_const = new_temp(ctx);
        ir_printf(ctx, out, "t%d = const int 0", zero);
        ir_printf(ctx, out, "t%d = const int 1", one);
        ir_printf(ctx, out, "t%d = const int %d", src_rows_const, inner);
        ir_printf(ctx, out, "t%d = const int %d", src_cols_const, cols);
        four = new_temp(ctx);
        src_cols_bytes = new_temp(ctx);
        bt_inner_bytes = new_temp(ctx);
        ir_printf(ctx, out, "t%d = const int 4", four);
        ir_printf(ctx, out, "t%d = * t%d, t%d", src_cols_bytes, src_cols_const, four);
        ir_printf(ctx, out, "t%d = const int %d", bt_inner_bytes, inner * 4);

        base_src = new_temp(ctx);
        base_bt_local = new_temp(ctx);
        ir_printf(ctx, out, "t%d = addr %s", base_src, safe_str(b));
        ir_printf(ctx, out, "t%d = addr %s", base_bt_local, bt_name);

        ir_printf(ctx, out, "store [t%d], t%d", src_row_addr, zero);
        src_row_label = new_label(ctx);
        src_row_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", src_row_label);
        {
            int src_row_val = new_temp(ctx);
            int src_row_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", src_row_val, src_row_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", src_row_cmp, src_row_val, src_rows_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", src_row_cmp, src_row_end_label);
        }

        ir_printf(ctx, out, "store [t%d], t%d", src_col_addr, zero);
        src_col_label = new_label(ctx);
        src_col_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", src_col_label);
        {
            int src_col_val = new_temp(ctx);
            int src_col_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", src_col_val, src_col_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", src_col_cmp, src_col_val, src_cols_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", src_col_cmp, src_col_end_label);
        }

        {
            int src_row_val = new_temp(ctx);
            int src_col_val = new_temp(ctx);
            int src_row_off = new_temp(ctx);
            int src_row_base = new_temp(ctx);
            int src_col_off = new_temp(ctx);
            int bt_row_off = new_temp(ctx);
            int bt_row_base = new_temp(ctx);
            int bt_col_off = new_temp(ctx);
            int addr_src = new_temp(ctx);
            int addr_bt = new_temp(ctx);
            int value = new_temp(ctx);

            ir_printf(ctx, out, "t%d = load %s", src_row_val, src_row_var);
            ir_printf(ctx, out, "t%d = load %s", src_col_val, src_col_var);
            ir_printf(ctx, out, "t%d = * t%d, t%d", src_row_off, src_row_val, src_cols_bytes);
            ir_printf(ctx, out, "t%d = + t%d, t%d", src_row_base, base_src, src_row_off);
            ir_printf(ctx, out, "t%d = * t%d, t%d", src_col_off, src_col_val, four);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_src, src_row_base, src_col_off);
            ir_printf(ctx, out, "t%d = * t%d, t%d", bt_row_off, src_col_val, bt_inner_bytes);
            ir_printf(ctx, out, "t%d = + t%d, t%d", bt_row_base, base_bt_local, bt_row_off);
            ir_printf(ctx, out, "t%d = * t%d, t%d", bt_col_off, src_row_val, four);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_bt, bt_row_base, bt_col_off);
            ir_printf(ctx, out, "t%d = load [t%d]", value, addr_src);
            ir_printf(ctx, out, "store [t%d], t%d", addr_bt, value);
        }

        {
            int src_col_val = new_temp(ctx);
            int src_col_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", src_col_val, src_col_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", src_col_next, src_col_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", src_col_addr, src_col_next);
        }
        ir_printf(ctx, out, "goto L%d", src_col_label);
        ir_printf(ctx, out, "label L%d", src_col_end_label);

        {
            int src_row_val = new_temp(ctx);
            int src_row_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", src_row_val, src_row_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", src_row_next, src_row_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", src_row_addr, src_row_next);
        }
        ir_printf(ctx, out, "goto L%d", src_row_label);
        ir_printf(ctx, out, "label L%d", src_row_end_label);
    }

    row_var_id = new_temp(ctx);
    bt_row_var_id = new_temp(ctx);
    k_var_id = new_temp(ctx);
    acc_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(bt_row_var, sizeof(bt_row_var), "mat_bt_r%d", bt_row_var_id);
    snprintf(k_var, sizeof(k_var), "mat_k%d", k_var_id);
    snprintf(acc_var, sizeof(acc_var), "mat_acc%d", acc_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", bt_row_var);
    ir_printf(ctx, out, "decl int %s", k_var);
    ir_printf(ctx, out, "decl int %s", acc_var);

    row_addr = new_temp(ctx);
    bt_row_addr = new_temp(ctx);
    k_addr = new_temp(ctx);
    acc_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", bt_row_addr, bt_row_var);
    ir_printf(ctx, out, "t%d = addr %s", k_addr, k_var);
    ir_printf(ctx, out, "t%d = addr %s", acc_addr, acc_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    inner_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    ir_printf(ctx, out, "t%d = const int %d", inner_const, inner);
    four = new_temp(ctx);
    inner_bytes = new_temp(ctx);
    dst_cols_bytes = new_temp(ctx);
    bt_cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", inner_bytes, inner_const, four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", dst_cols_bytes, cols_const, four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", bt_cols_bytes, inner_const, four);

    base_a = new_temp(ctx);
    base_bt = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_bt, bt_name);
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", bt_row_addr, zero);
    bt_row_label = new_label(ctx);
    bt_row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", bt_row_label);
    {
        int bt_row_val = new_temp(ctx);
        int bt_row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", bt_row_val, bt_row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", bt_row_cmp, bt_row_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", bt_row_cmp, bt_row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", acc_addr, zero);
    ir_printf(ctx, out, "store [t%d], t%d", k_addr, zero);
    k_label = new_label(ctx);
    k_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", k_label);
    {
        int k_val = new_temp(ctx);
        int k_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", k_cmp, k_val, inner_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", k_cmp, k_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int bt_row_val = new_temp(ctx);
        int k_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int bt_row_off = new_temp(ctx);
        int bt_row_base = new_temp(ctx);
        int k_off = new_temp(ctx);
        int addr_a = new_temp(ctx);
        int addr_bt = new_temp(ctx);
        int val_a = new_temp(ctx);
        int val_bt = new_temp(ctx);
        int mul_val = new_temp(ctx);
        int acc_val = new_temp(ctx);
        int acc_next = new_temp(ctx);

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", bt_row_val, bt_row_var);
        ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, inner_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);
        ir_printf(ctx, out, "t%d = * t%d, t%d", bt_row_off, bt_row_val, bt_cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", bt_row_base, base_bt, bt_row_off);
        ir_printf(ctx, out, "t%d = * t%d, t%d", k_off, k_val, four);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, k_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_bt, bt_row_base, k_off);
        ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
        ir_printf(ctx, out, "t%d = load [t%d]", val_bt, addr_bt);
        ir_printf(ctx, out, "t%d = * t%d, t%d", mul_val, val_a, val_bt);
        ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", acc_next, acc_val, mul_val);
        ir_printf(ctx, out, "store [t%d], t%d", acc_addr, acc_next);
    }

    {
        int k_val = new_temp(ctx);
        int k_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", k_next, k_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", k_addr, k_next);
    }
    ir_printf(ctx, out, "goto L%d", k_label);
    ir_printf(ctx, out, "label L%d", k_end_label);

    {
        int row_val = new_temp(ctx);
        int bt_row_val = new_temp(ctx);
        int acc_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_dst = new_temp(ctx);

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, dst_cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);
        ir_printf(ctx, out, "t%d = load %s", bt_row_val, bt_row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, bt_row_val, four);
        ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, acc_val);
    }

    {
        int bt_row_val = new_temp(ctx);
        int bt_row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", bt_row_val, bt_row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", bt_row_next, bt_row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", bt_row_addr, bt_row_next);
    }
    ir_printf(ctx, out, "goto L%d", bt_row_label);
    ir_printf(ctx, out, "label L%d", bt_row_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}
