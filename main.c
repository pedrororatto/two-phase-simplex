#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define MAX_SIZE 100

void printMatrix(float matrix[MAX_SIZE][MAX_SIZE], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f\t", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void initializePhaseOne(float A[MAX_SIZE][MAX_SIZE], float b[MAX_SIZE], float c[MAX_SIZE],
                        float A_phase1[MAX_SIZE][MAX_SIZE], float c_phase1[MAX_SIZE],
                        int var, int constraint_count) {
    // Inicialize a matriz A da Fase 1 com as variáveis artificiais
    for (int i = 0; i < constraint_count; i++) {
        for (int j = var; j < var + constraint_count; j++) {
            if (i == j - var) {
                A_phase1[i][j] = 1.0;  // Adiciona variáveis artificiais como identidade
            } else {
                A_phase1[i][j] = 0.0;
            }
        }
    }

    // Copie os coeficientes da matriz A original para A_phase1
    for (int i = 0; i < constraint_count; i++) {
        for (int j = 0; j < var; j++) {
            A_phase1[i][j] = A[i][j];
        }
    }

    // Copie os coeficientes da matriz A original para A_phase1
    for (int i = 0; i < constraint_count; i++) {
        b[i] = A[i][var];  // Atualiza o vetor b com os termos independentes da matriz A original
    }

    // Inicialize o vetor c da Fase 1 com zeros
    for (int i = 0; i < var + constraint_count; i++) {
        c_phase1[i] = 0.0;
    }

    // Adiciona penalidades (M) às variáveis artificiais no vetor c_phase1
    for (int i = var; i < var + constraint_count; i++) {
        c_phase1[i] = 1.0;
    }
}

void simplexPhaseOne(float A_phase1[MAX_SIZE][MAX_SIZE], float b[MAX_SIZE], float c_phase1[MAX_SIZE],
                     int var, int constraint_count, int *opt_point) {
    while (1) {
        // Encontrar a coluna pivot
        int pivot_col = -1;
        for (int j = 0; j < var + constraint_count; j++) {
            if (c_phase1[j] > 0) {
                if (pivot_col == -1 || c_phase1[j] > c_phase1[pivot_col]) {
                    pivot_col = j;
                }
            }
        }

        // Se não houver coluna pivot, alcançamos a solução ótima da Fase 1
        if (pivot_col == -1) {
            break;
        }

        // Encontrar a linha pivot usando a regra da razão mínima
        int pivot_row = -1;
        float min_ratio = FLT_MAX;
        for (int i = 0; i < constraint_count; i++) {
            if (A_phase1[i][pivot_col] > 0) {
                float ratio = b[i] / A_phase1[i][pivot_col];
                if (ratio < min_ratio) {
                    min_ratio = ratio;
                    pivot_row = i;
                }
            }
        }

        // Se não houver linha pivot, o problema é ilimitado
        if (pivot_row == -1) {
            printf("Problema inviável (Fase 1)\n");
            exit(1);
        }

        // Realizar operações de pivoteamento para tornar o elemento pivot 1 e zerar outros elementos
        float pivot_element = A_phase1[pivot_row][pivot_col];
        for (int j = 0; j < var + constraint_count; j++) {
            A_phase1[pivot_row][j] /= pivot_element;
        }
        b[pivot_row] /= pivot_element;

        // Realizar operações de pivoteamento nas outras linhas
        for (int i = 0; i < constraint_count; i++) {
            if (i != pivot_row) {
                float ratio = A_phase1[i][pivot_col];
                for (int j = 0; j < var + constraint_count; j++) {
                    A_phase1[i][j] -= ratio * A_phase1[pivot_row][j];
                }
                b[i] -= ratio * b[pivot_row];
            }
        }

        // Atualizar o vetor c_phase1
        for (int j = 0; j < var + constraint_count; j++) {
            c_phase1[j] -= c_phase1[pivot_col] * A_phase1[pivot_row][j];
        }

        // Atualizar a posição da variável básica no vetor de pontos ótimos
        opt_point[pivot_row] = pivot_col;
    }
}

void simplexPhaseTwo(float A[MAX_SIZE][MAX_SIZE], float b[MAX_SIZE], float c[MAX_SIZE],
                     int var, int constraint_count, int *opt_point) {
    while (1) {
        // Encontrar a coluna pivot
        int pivot_col = -1;
        for (int j = 0; j < var; j++) {
            if (c[j] > 0) {
                if (pivot_col == -1 || c[j] > c[pivot_col]) {
                    pivot_col = j;
                }
            }
        }

        // Se não houver coluna pivot, alcançamos a solução ótima
        if (pivot_col == -1) {
            break;
        }

        // Encontrar a linha pivot usando a regra da razão mínima
        int pivot_row = -1;
        float min_ratio = FLT_MAX;
        for (int i = 0; i < constraint_count; i++) {
            if (A[i][pivot_col] > 0) {
                float ratio = b[i] / A[i][pivot_col];
                if (ratio < min_ratio) {
                    min_ratio = ratio;
                    pivot_row = i;
                }
            }
        }

        // Se não houver linha pivot, o problema é ilimitado
        if (pivot_row == -1) {
            printf("Problema inviável (Fase 2)\n");
            exit(1);
        }

        // Realizar operações de pivoteamento para tornar o elemento pivot 1 e zerar outros elementos
        float pivot_element = A[pivot_row][pivot_col];
        for (int j = 0; j < var; j++) {
            A[pivot_row][j] /= pivot_element;
        }
        b[pivot_row] /= pivot_element;

        // Realizar operações de pivoteamento nas outras linhas
        for (int i = 0; i < constraint_count; i++) {
            if (i != pivot_row) {
                float ratio = A[i][pivot_col];
                for (int j = 0; j < var; j++) {
                    A[i][j] -= ratio * A[pivot_row][j];
                }
                b[i] -= ratio * b[pivot_row];
            }
        }

        // Atualizar o vetor c
        for (int j = 0; j < var; j++) {
            c[j] -= c[pivot_col] * A[pivot_row][j];
        }

        // Atualizar a posição da variável básica no vetor de pontos ótimos
        opt_point[pivot_row] = pivot_col;
    }
}

void simplexTwoPhases(float A[MAX_SIZE][MAX_SIZE], float b[MAX_SIZE], float c[MAX_SIZE],
                      int var, int constraint_count) {
    float A_phase1[MAX_SIZE][MAX_SIZE];
    float c_phase1[MAX_SIZE];
    int opt_point[MAX_SIZE];

    initializePhaseOne(A, b, c, A_phase1, c_phase1, var, constraint_count);
    simplexPhaseOne(A_phase1, b, c_phase1, var, constraint_count, opt_point);
    simplexPhaseTwo(A, b, c, var, constraint_count, opt_point);
}

int main() {
    int var, constraint_count;

    // Obter o número de variáveis e restrições
    printf("Digite o número de variáveis: ");
    scanf("%d", &var);
    printf("Digite o número de restrições: ");
    scanf("%d", &constraint_count);

    float A[MAX_SIZE][MAX_SIZE];
    float b[MAX_SIZE];
    float c[MAX_SIZE];

    // Obter os coeficientes da matriz A
    printf("Digite os coeficientes da matriz A (%dx%d):\n", constraint_count, var);
    for (int i = 0; i < constraint_count; i++) {
        for (int j = 0; j < var; j++) {
            printf("A[%d][%d] = ", i + 1, j + 1);
            scanf("%f", &A[i][j]);
        }
    }

    // Obter os termos independentes da matriz b
    printf("Digite os termos independentes da matriz b (%d):\n", constraint_count);
    for (int i = 0; i < constraint_count; i++) {
        scanf("%f", &b[i]);
    }

    // Obter os coeficientes do vetor c
    printf("Digite os coeficientes do vetor c (%d):\n", var);
    for (int i = 0; i < var; i++) {
        scanf("%f", &c[i]);
    }

    simplexTwoPhases(A, b, c, var, constraint_count);

    return 0;
}
