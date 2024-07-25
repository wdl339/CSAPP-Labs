// 韦东良 522031910516
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    // 共11个变量
    int i, j, k;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    if (M == 32 && N == 32){
        // 8×8分块访问
        for (i = 0; i < N; i += 8){
            for (j = 0; j < M; j += 8){
                for (k = i; k < i + 8; k++){
                    tmp0 = A[k][j];
                    tmp1 = A[k][j + 1];
                    tmp2 = A[k][j + 2];
                    tmp3 = A[k][j + 3];
                    tmp4 = A[k][j + 4];
                    tmp5 = A[k][j + 5];
                    tmp6 = A[k][j + 6];
                    tmp7 = A[k][j + 7];
                    B[j][k] = tmp0;
                    B[j + 1][k] = tmp1;
                    B[j + 2][k] = tmp2;
                    B[j + 3][k] = tmp3;
                    B[j + 4][k] = tmp4;
                    B[j + 5][k] = tmp5;
                    B[j + 6][k] = tmp6;
                    B[j + 7][k] = tmp7;
                }
            }
        }
    } 
    else if (M == 64 && N == 64) {
        for (i = 0; i < 64; i += 8){
            for (j = 0; j < 64; j += 8){
                // 对8×8块再细分成4×4块
                // 拿A的左上、右上放到B的左上、右上（不是左下）
                for (k = i; k < i + 4; k++){
                    tmp0 = A[k][j];
                    tmp1 = A[k][j + 1];
                    tmp2 = A[k][j + 2];
                    tmp3 = A[k][j + 3];
                    tmp4 = A[k][j + 4];
                    tmp5 = A[k][j + 5];
                    tmp6 = A[k][j + 6];
                    tmp7 = A[k][j + 7];
                    B[j][k] = tmp0;
                    B[j + 1][k] = tmp1;
                    B[j + 2][k] = tmp2;
                    B[j + 3][k] = tmp3;
                    B[j][k + 4] = tmp4;
                    B[j + 1][k + 4] = tmp5;
                    B[j + 2][k + 4] = tmp6;
                    B[j + 3][k + 4] = tmp7;
                }
                // B的右上放到左下，A的左下放到B的右上
                for (k = j; k < j + 4; k++){
                    tmp0 = A[i + 4][k];
                    tmp1 = A[i + 5][k];
                    tmp2 = A[i + 6][k];
                    tmp3 = A[i + 7][k];
                    tmp4 = B[k][i + 4];
                    tmp5 = B[k][i + 5];
                    tmp6 = B[k][i + 6];
                    tmp7 = B[k][i + 7];
                    B[k][i + 4] = tmp0;
                    B[k][i + 5] = tmp1;
                    B[k][i + 6] = tmp2;
                    B[k][i + 7] = tmp3;
                    B[k + 4][i] = tmp4;
                    B[k + 4][i + 1] = tmp5;
                    B[k + 4][i + 2] = tmp6;
                    B[k + 4][i + 3] = tmp7;
                }
                // A的右下放到B的右下
                for (k = i + 4; k < i + 8; k++){
                    tmp0 = A[k][j + 4];
                    tmp1 = A[k][j + 5];
                    tmp2 = A[k][j + 6];
                    tmp3 = A[k][j + 7];
                    B[j + 4][k] = tmp0;
                    B[j + 5][k] = tmp1;
                    B[j + 6][k] = tmp2;
                    B[j + 7][k] = tmp3;
                }
            }
        }
    }  
    else if (M == 61 && N == 67){
        // 23×23分块
        for (i = 0; i < 67; i += 23){
            for (j = 0; j < 61; j += 23){
                for (k = i; k < i + 23 && k < 67; k++){
                    for (tmp1 = j; tmp1 < j + 23 && tmp1 < 61; tmp1++){
                        tmp0 = A[k][tmp1];
                        B[tmp1][k] = tmp0;
                    }
                }
            }
        }
    }
    else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp0 = A[i][j];
                B[j][i] = tmp0;
            }
        }  
    }  
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char trans_desc24[] = "test 24";
void trans24(int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 24){
            for (j = 0; j < 61; j += 24){
                for (k = i; k < i + 24 && k < 67; k++){
                    for (int l = j; l < j + 24 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }  
}

char trans_desc23[] = "test 23";
void trans23(int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 23){
            for (j = 0; j < 61; j += 23){
                for (k = i; k < i + 23 && k < 67; k++){
                    for (int l = j; l < j + 23 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }  
}

char trans_desc22[] = "test 22";
void trans22(int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 22){
            for (j = 0; j < 61; j += 22){
                for (k = i; k < i + 22 && k < 67; k++){
                    for (int l = j; l < j + 22 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }  
}

char trans_desc21[] = "test 21";
void trans21(int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 21){
            for (j = 0; j < 61; j += 21){
                for (k = i; k < i + 21 && k < 67; k++){
                    for (int l = j; l < j + 21 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }  
}

char trans_desc20[] = "test 20";
void trans20(int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 20){
            for (j = 0; j < 61; j += 20){
                for (k = i; k < i + 20 && k < 67; k++){
                    for (int l = j; l < j + 20 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }
}

char trans_desc19[] = "test 19";
void trans19 (int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 19){
            for (j = 0; j < 61; j += 19){
                for (k = i; k < i + 19 && k < 67; k++){
                    for (int l = j; l < j + 19 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }
}

char trans_desc18[] = "test 18";
void trans18 (int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 18){
            for (j = 0; j < 61; j += 18){
                for (k = i; k < i + 18 && k < 67; k++){
                    for (int l = j; l < j + 18 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }
}

char trans_desc17[] = "test 17";
void trans17 (int M, int N, int A[N][M], int B[M][N])
{

    int i, j, k, tmp;

    if (M == 61 && N == 67){
        for (i = 0; i < 67; i += 17){
            for (j = 0; j < 61; j += 17){
                for (k = i; k < i + 17 && k < 67; k++){
                    for (int l = j; l < j + 17 && l < 61; l++){
                        tmp = A[k][l];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }  
    }
}



/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
    //registerTransFunction(trans24, trans_desc24); 
    //registerTransFunction(trans23, trans_desc23);
    //registerTransFunction(trans22, trans_desc22);
    //registerTransFunction(trans21, trans_desc21);
    //registerTransFunction(trans20, trans_desc20);
    //registerTransFunction(trans19, trans_desc19);
    //registerTransFunction(trans18, trans_desc18);
    //registerTransFunction(trans17, trans_desc17);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

