# Matrix Inverse
```
int main() {
    float a[3][3];
    float b[3][3];
    float mat_tmpA1[3][3];

    int i, j, r;

    // ---- Copy matrix a into temporary matrix ----
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            mat_tmpA1[r][c] = a[r][c];
        }
    }

    // ---- Initialize b as Identity Matrix ----
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (i == j)
                b[i][j] = 1;
            else
                b[i][j] = 0;
        }
    }

    // ---- Gauss-Jordan Elimination ----
    for (i = 0; i < 3; i++) {

        float pivot = mat_tmpA1[i][i];

        // Normalize pivot row
        for (j = 0; j < 3; j++) {
            mat_tmpA1[i][j] = mat_tmpA1[i][j] / pivot;
            b[i][j] = b[i][j] / pivot;
        }

        // Eliminate other rows
        for (r = 0; r < 3; r++) {

            if (r == i)
                continue;

            float factor = mat_tmpA1[r][i];

            for (j = 0; j < 3; j++) {
                mat_tmpA1[r][j] = mat_tmpA1[r][j] - factor * mat_tmpA1[i][j];
                b[r][j] = b[r][j] - factor * b[i][j];
            }
        }
    }
    return 0;
}
```

# Matrix Determinant
```
int main() {
    float a[4][4];
    float e;

    int mat_c1 = 0;
    float det_acc2 = 0;

    while (mat_c1 < 4) {

        int mat_r11, mat_sc12, mat_mc13;
        float mat_tmp17[3][3];

        mat_r11 = 1;  // skip first row

        // Build 3x3 minor matrix
        while (mat_r11 < 4) {

            mat_mc13 = 0;
            mat_sc12 = 0;

            while (mat_sc12 < 4) {

                if (mat_sc12 == mat_c1) {
                    mat_sc12++;
                    continue;
                }

                mat_tmp17[mat_r11 - 1][mat_mc13] =
                    a[mat_r11][mat_sc12];

                mat_mc13++;
                mat_sc12++;
            }

            mat_r11++;
        }

        // ---- determinant of 3x3 minor ----
        int mat_c47 = 0;
        float det_acc48 = 0;

        while (mat_c47 < 3) {

            int mat_r57, mat_sc58, mat_mc59;
            float mat_tmp63[2][2];

            mat_r57 = 1;

            // build 2x2 minor
            while (mat_r57 < 3) {

                mat_mc59 = 0;
                mat_sc58 = 0;

                while (mat_sc58 < 3) {

                    if (mat_sc58 == mat_c47) {
                        mat_sc58++;
                        continue;
                    }

                    mat_tmp63[mat_r57 - 1][mat_mc59] =
                        mat_tmp17[mat_r57][mat_sc58];

                    mat_mc59++;
                    mat_sc58++;
                }

                mat_r57++;
            }

            // determinant of 2x2
            float minor2 =
                mat_tmp63[0][0] * mat_tmp63[1][1] -
                mat_tmp63[0][1] * mat_tmp63[1][0];

            float term = mat_tmp17[0][mat_c47] * minor2;

            if (mat_c47 % 2 == 1)
                term = -term;

            det_acc48 += term;

            mat_c47++;
        }

        // combine with element from first row of original matrix
        float term = a[0][mat_c1] * det_acc48;

        if (mat_c1 % 2 == 1)
            term = -term;

        det_acc2 += term;

        mat_c1++;
    }

    e = det_acc2;
}
```
4×4 determinant
   ↓
3×3 minor determinant
   ↓
2×2 determinant

# Matrix Transpose
```
int main() {
    float a[3][3];
    float b[3][3];

    int mat_r1 = 0;
    int mat_c2;

    while (mat_r1 < 3) {

        mat_c2 = 0;

        while (mat_c2 < 3) {

            b[mat_r1][mat_c2] = a[mat_c2][mat_r1];

            mat_c2 = mat_c2 + 1;
        }

        mat_r1 = mat_r1 + 1;
    }

    return 0;
}
```

# Matrix Multiplication
```
int main() {
    float a[3][3];
    float b[3][3];
    float c[3][3];

    int mat_r1 = 0;
    int mat_c2, mat_k3;
    int mat_acc4;

    while (mat_r1 < 3) {

        mat_c2 = 0;

        while (mat_c2 < 3) {

            mat_acc4 = 0;
            mat_k3 = 0;

            while (mat_k3 < 3) {

                mat_acc4 += a[mat_r1][mat_k3] * b[mat_k3][mat_c2];

                mat_k3 = mat_k3 + 1;
            }

            c[mat_r1][mat_c2] = mat_acc4;

            mat_c2 = mat_c2 + 1;
        }

        mat_r1 = mat_r1 + 1;
    }

    return 0;
}
```

# Matrix Addition
```
int main() {
    float a[3][3];
    float b[3][3];
    float c[3][3];

    int mat_r1 = 0;
    int mat_c2;

    while (mat_r1 < 3) {

        mat_c2 = 0;

        while (mat_c2 < 3) {

            c[mat_r1][mat_c2] =
                a[mat_r1][mat_c2] + b[mat_r1][mat_c2];

            mat_c2 = mat_c2 + 1;
        }

        mat_r1 = mat_r1 + 1;
    }

    return 0;
}
```

# Matrix Subtraction
```
int main() {
    float a[3][3];
    float b[3][3];
    float c[3][3];

    int mat_r1 = 0;
    int mat_c2;

    while (mat_r1 < 3) {

        mat_c2 = 0;

        while (mat_c2 < 3) {

            c[mat_r1][mat_c2] =
                a[mat_r1][mat_c2] - b[mat_r1][mat_c2];

            mat_c2 = mat_c2 + 1;
        }

        mat_r1 = mat_r1 + 1;
    }

    return 0;
}
```

# Array Sorting
```
int main() {
    int d[5];
    int e[5];

    int arr_i1 = 0;

    // Copy d → e
    while (arr_i1 < 5) {
        e[arr_i1] = d[arr_i1];
        arr_i1 = arr_i1 + 1;
    }

    int arr_i18 = 0;
    int arr_j19;

    // Bubble sort
    while (arr_i18 < 5) {

        arr_j19 = 0;

        while (arr_j19 < 5 - 1 - arr_i18) {

            if (e[arr_j19] > e[arr_j19 + 1]) {
                int temp = e[arr_j19];
                e[arr_j19] = e[arr_j19 + 1];
                e[arr_j19 + 1] = temp;
            }

            arr_j19 = arr_j19 + 1;
        }

        arr_i18 = arr_i18 + 1;
    }

    return 0;
}
```
