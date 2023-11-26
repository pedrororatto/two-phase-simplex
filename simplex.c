#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_VARIABLES 100
#define MAX_CONSTRAINTS 100
#define MAX_ITERATIONS 100 
#define EPSILON 1E-8 //Tolerância do erro

int AIM, NV, NVNN, NVNP, NVUR, NC, NV0, NN[MAX_VARIABLES], NP[MAX_VARIABLES], UR[MAX_VARIABLES], OPTIMAL, COUNT;
/*
    AIM: função objetivo (min/max)
    NV: número de variáveis
    NVNN: número de variáveis não negativas
    NVNP: número de variáveis não positivas
    NVUR: número de variáveis irrestritas
    NC: número de restrições
    NV0: número de variáveis originais
    NN: variáveis não negativas
    NP: variáveis não positivas
    UR: variáveis irrestritas
    OPTIMAL: indicador de solução ótima
    COUNT: contador de iterações
*/
int BASIC[MAX_VARIABLES], LB[MAX_ITERATIONS], EB[MAX_ITERATIONS];
/*
    BASIC: variáveis básicas
    LB: variáveis que deixam a base
    EB: variáveis que entram na base
*/
double C[MAX_VARIABLES], V[MAX_VARIABLES], CTemp[MAX_VARIABLES], A[MAX_CONSTRAINTS][MAX_VARIABLES], b[MAX_CONSTRAINTS][1], ATemp[MAX_CONSTRAINTS][MAX_VARIABLES];
/*
    C: coeficientes da função objetivo
    V: sinal das variáveis
    CTemp: coeficientes da função objetivo temporários
    A: coeficientes das restrições
    b: lado direito das restrições
    ATemp: coeficientes das restrições temporários
*/
int CONSYMBOL[MAX_CONSTRAINTS][1];
int PI, PJ;
// PI: A linha index do pivot
// PJ: A coluna index do pivot
int STATE; 
// STATE: Indicador de primeira ou segunda fase
double Z = 0; 
// Z: valor objetivo
double CSIGN; 
// CSIGN: controla o sinal da função objetivo baseado em AIM

void defineObjetivo() {
    printf("\nA função objetivo será MAX(1) ou MIN(2) ? ");
    printf("\nPor favor, insira 1 para MAX ou 2 para MIN:  ");
    scanf("%d", &AIM);
    while (AIM != 1 && AIM != 2) {
        printf("\n MENSAGEM DE ERRO: Entrada inválida! Não foi possível ler...");
        printf("\nPor favor, insira apenas 1 ou 2:  ");
        scanf("%d", &AIM);
    }

    double CSIGN = (AIM == 1) ? 1.0 : -1.0;

    printf("Número de variáveis ? (por favor, insira um NÚMERO INTEIRO)  ");
    scanf("%d", &NV);

    printf("Por favor, insira o coeficiente de cada variável:\n");
    for (int J = 1; J <= NV; J++) {
        printf(" >>>>   X%d? ", J);
        scanf("%lf", &C[J]);
        C[J] *= CSIGN;
    }

    printf("Sua função objetivo inserida é equivalente a: \n");
    printf("        MAX Z = ");
    for (int J = 1; J <= NV - 1; J++) {
        printf("(%lf)X%d + ", C[J], J);
    }
    printf("(%lf)X%d\n", C[NV], NV);

    printf("Confirmar? (S/N)   ");
    char OFCON;
    scanf(" %c", &OFCON);
    if (OFCON == 'N' || OFCON == 'n') {
        defineObjetivo();
    } else {
        printf("Sua função objetivo foi confirmada.\n");
    }
}

void defineVariaveis() {
    NVNN = NVNP = NVUR = 0;

    printf("\nIndique se a restrição de cada variável é: ");
    printf("\nNÃO NEGATIVA(1), NÃO POSITIVA(2) ou IRRESTRITA(3).\n");
    for (int J = 1; J <= NV; J++) {
        printf(" >>>>   X%d? ", J);
        scanf("%lf", &V[J]);
    }

    printf("Suas variáveis não negativas incluem: \n");
    printf("        ");
    for (int J = 1; J <= NV; J++) {
        if (V[J] == 1) {
            printf("X%d ", J);
            NVNN = NVNN + 1;
            NN[NVNN] = J;
        }
    }
    if (NVNN == 0) printf("NENHUMA");

    printf("\nSuas variáveis não positivas incluem: \n");
    printf("        ");
    for (int J = 1; J <= NV; J++) {
        if (V[J] == 2) {
            printf("X%d ", J);
            NVNP = NVNP + 1;
            NP[NVNP] = J;
        }
    }
    if (NVNP == 0) printf("NENHUMA");

    printf("\nSuas variáveis irrestritas incluem: \n");
    printf("        ");
    for (int J = 1; J <= NV; J++) {
        if (V[J] == 3) {
            printf("X%d ", J);
            NVUR = NVUR + 1;
            UR[NVUR] = J;
        }
    }
    if (NVUR == 0) printf("NENHUMA");

    printf("\nConfirmar? (S/N)   ");
    char VCON;
    scanf(" %c", &VCON);
    if (VCON == 'N' || VCON == 'n') {
        defineVariaveis();
    } else {
        printf("Os sinais de suas variáveis foram confirmados.\n");
    }
}

void defineRestricoes() {
    printf("\nNúmero de restrições EXCLUINDO restrições de variáveis definidas? (por favor, insira um NÚMERO INTEIRO:) ");
    scanf("%d", &NC);

    while (NC < 0 || NC >= MAX_CONSTRAINTS) {
        printf("\n MENSAGEM DE ERRO : Entrada inválida! Não foi possível ler...\n");
        printf("Por favor, insira um NÚMERO INTEIRO válido (entre 0 e %d):  ", MAX_CONSTRAINTS - 1);
        scanf("%d", &NC);
    }

    for (int I = 1; I <= NC; I++) {
        printf("Coeficiente de cada variável na restrição Nº %d:\n", I);
        for (int J = 1; J <= NV; J++) {
            printf(" >>>>   X%d? ", J);
            scanf("%lf", &A[I][J]);
        }

        printf("Desigualdade ou igualdade (1: <= | 2: >= | 3: =) na restrição Nº %d (por favor, insira 1/2/3): \n >>>>   ", I);
        scanf("%d", &CONSYMBOL[I][1]);

        while (CONSYMBOL[I][1] != 1 && CONSYMBOL[I][1] != 2 && CONSYMBOL[I][1] != 3) {
            printf(" MENSAGEM DE ERRO : Entrada inválida! Não foi possível ler...\n");
            printf("Por favor, insira apenas 1 ou 2 ou 3:  ");
            scanf("%d", &CONSYMBOL[I][1]);
        }

        printf("Lado direito na restrição Nº %d:\n >>>>   ", I);
        scanf("%lf", &b[I][1]);
    }

    printf("Suas restrições (excluindo restrições de variáveis) são: \n");
    for (int I = 1; I <= NC; I++) {
        printf("       ");
        for (int J = 1; J <= NV - 1; J++) {
            printf("(%lf)X%d + ", A[I][J], J);
        }
        printf("(%lf)X%d", A[I][NV], NV);
        switch (CONSYMBOL[I][1]) {
            case 1:
                printf(" <= ");
                break;
            case 2:
                printf(" >= ");
                break;
            case 3:
                printf(" = ");
                break;
        }
        printf("%lf\n", b[I][1]);
    }

    printf("Confirmar? (S/N)   ");
    char CCON;
    scanf(" %c", &CCON);
    if (CCON == 'N' || CCON == 'n') {
        defineRestricoes();
    } else {
        printf("Suas restrições foram confirmadas.\n");
    }
}


void formaPadrao() {
    int I, J, K;

    if (NVNP > 0) { // Tornar as variáveis não positivas não negativas
        for (K = 1; K <= NVNP; K++) {
            C[NP[K]] = -1 * C[NP[K]];
        }
        for (I = 1; I <= NC; I++) {
            for (K = 1; K <= NVNP; K++) {
                A[I][NP[K]] = -1 * A[I][NP[K]];
            }
        }
    }

    if (NVUR > 0) { // Tornar as variáveis irrestritas não negativas dividindo essas variáveis
        NV = NV + NVUR;
        for (K = 1; K <= NVUR; K++) {
            C[NV - NVUR + K] = -1 * C[UR[K]];
        }
        for (I = 1; I <= NC; I++) {
            K = 1;
            for (J = NV - NVUR + 1; J <= NV; J++) {
                A[I][J] = -1 * A[I][UR[K]];
                K = K + 1;
            }
        }
    }

    for (I = 1; I <= NC; I++) {
        switch (CONSYMBOL[I][1]) {
            case 1: { // <= adicionar uma variável de folga
                if (b[I][1] < 0) {
                    for (J = 1; J <= NV; J++) {
                        A[I][J] = -1 * A[I][J];
                        if (A[I][J] == -0) A[I][J] = 0;
                    }
                    b[I][1] = -1 * b[I][1];
                }
                NV = NV + 1;
                C[NV] = 0;
                A[I][NV] = 1;
                break;
            }

            case 2: { // >= subtrair uma variável de excesso
                if (b[I][1] < 0) {
                    for (J = 1; J <= NV; J++) {
                        A[I][J] = -1 * A[I][J];
                        if (A[I][J] == -0) A[I][J] = 0;
                    }
                    b[I][1] = -1 * b[I][1];
                }
                NV = NV + 1;
                C[NV] = 0;
                A[I][NV] = -1;
                break;
            }

            case 3: { // = apenas transformar o lado direito negativo em não negativo
                if (b[I][1] < 0) {
                    for (J = 1; J <= NV; J++) {
                        A[I][J] = -1 * A[I][J];
                        if (A[I][J] == -0) A[I][J] = 0;
                    }
                    b[I][1] = -1 * b[I][1];
                }
                break;
            }
        }
    }

    NV = NV + NC; // Adicionar uma variável artificial para cada restrição diretamente para construir uma forma canônica
    for (I = 1; I <= NC; I++) {
        for (J = NV - NC + 1; J <= NV; J++) {
            A[I][J] = 0;
        }
        A[I][NV - NC + I] = 1;
    }

    // Imprimir a forma padrão
    printf("MIN Z = ");
    for (J = 1; J <= NV - 1; J++) {
        printf("(%lf)X%d + ", C[J], J);
    }
    printf("(%lf)X%d\n", C[NV], NV);

    printf("s.a.\n");
    for (I = 1; I <= NC; I++) {
        for (J = 1; J <= NV - 1; J++) {
            printf("(%lf)X%d + ", A[I][J], J);
        }
        printf("(%lf)X%d = %lf\n", A[I][J], J, b[I][1]);
    }
    for (J = 1; J <= NV - 1; J++) {
        printf("X%d, ", J);
    }
    printf("X%d >= 0\n", NV);
} 

void simplex() {
    int I, J;

    // Imprimir as informações do cabeçalho
    printf(" ITERAÇÃO #%d:\n", COUNT);
    COUNT++;
    printf(" BV");
    for (J = 1; J <= NV; J++) {
        printf("%8s", "X");
        printf("%d ", J);
    }
    printf("%9s", " CV");
    printf("\n");
    for (J = 1; J <= NV + 1; J++) {
        printf("-----------");
    }
    printf("\n");

    // Imprimir as informações da linha x
    for (I = 1; I <= NC; I++) {
        printf(" X%d", BASIC[I]);
        for (J = 1; J <= NV; J++) {
            printf("%9.2lf ", A[I][J]);
        }
        printf("%9.2lf\n", b[I][1]);
    }

    // Imprimir as informações da linha z
    printf(" -Z");
    for (J = 1; J <= NV; J++) {
        printf("%9.2lf ", C[J]);
    }
    printf("%9.2lf\n", Z);

    e10:
        achaPivot();
        operacaoLinha(); // Realizar a operação de linha de Gauss-Jordan
        solucaoOtima();

        if (OPTIMAL == 0)
            goto e10; // Se não encontrar a solução ótima, vá para e10 para continuar
        if (OPTIMAL == -1) { // Se a função objetivo é ilimitada, então saia
            printf("\n**********************************************************************\n");
            printf("****** OPPS! Valor Objetivo Ilimitado!\n");
            exit(EXIT_FAILURE);
        }
}

void achaPivot() {
    double CVMAX, RMAX, R; // R: a razão do lado direito para o coeficiente de restrição positivo correspondente
    int I, J;
    CVMAX = 0.0; // Usado para encontrar o C[] mais negativo
    RMAX = 99999999.0; // Usado para encontrar o mínimo R
    OPTIMAL = 0; // Define o estado de encontrar a solução ótima

    for (J = 1; J <= NV; J++) { // Para encontrar o índice da coluna do pivô
        if (C[J] < 0.0 && C[J] < CVMAX) {
            CVMAX = C[J];
            PJ = J;
        }
    }

    for (I = 1; I <= NC; I++) { // Para encontrar o índice da linha do pivô
        if (A[I][PJ] <= 0.0)
            goto skip1; // O coeficiente de restrição deve ser estritamente positivo ao calcular a razão
        R = fabs(b[I][1] / A[I][PJ]);
        if (R < RMAX) {
            RMAX = R;
            PI = I;
        }
    skip1:;
    }

    // Imprima os índices do pivô, se necessário
    printf("\nÍndice linha: %d, Índice coluna: %d\n", PI, PJ);

    LB[COUNT] = BASIC[PI]; // Registra o índice da variável que deve deixar a base
    EB[COUNT] = PJ;       // Registra o índice da variável que deve entrar na base
    BASIC[PI] = PJ;       // Altera a base
}

void operacaoLinha() {
    int I, J;

    for (J = 1; J <= NV; J++) { // Processar A[linha do pivô][] exceto o pivô
        if (J == PJ)
            goto e30;
        A[PI][J] = A[PI][J] / A[PI][PJ];
    e30:;
    }
    b[PI][1] = b[PI][1] / A[PI][PJ]; // Processar b[linha do pivô][1]

    // O seguinte é devido aos diferentes objetivos da fase I e da fase II
    if (STATE == 1)
        Z = Z - C[PJ] * b[PI][1];
    if (STATE == 2)
        Z = Z + C[PJ] * b[PI][1];

    A[PI][PJ] = A[PI][PJ] / A[PI][PJ]; // Faça o pivô ser igual a um.

    for (J = 1; J <= NV; J++) { // Processar C[] exceto a coluna do pivô
        if (J == PJ)
            goto e90;
        C[J] = C[J] - C[PJ] * A[PI][J];
    e90:;
    }
    C[PJ] = C[PJ] - C[PJ] * A[PI][PJ]; // Processar C[coluna do pivô]

    for (I = 1; I <= NC; I++) { // Processar b[][1] exceto a linha do pivô
        if (I == PI)
            goto e50;
        b[I][1] = b[I][1] - b[PI][1] * A[I][PJ];

        for (J = 1; J <= NV; J++) { // Processar A[][] exceto a linha do pivô e a coluna do pivô
            if (J == PJ)
                goto e70;
            A[I][J] = A[I][J] - A[I][PJ] * A[PI][J];
        e70:;
        }
    e50:;
    }

    for (I = 1; I <= NC; I++) { // Processar A[][coluna do pivô] exceto a linha do pivô
        if (I == PI)
            goto e60;
        A[I][PJ] = A[I][PJ] - A[I][PJ] * A[PI][PJ];
    e60:;
    }

    printf(" ITERAÇÃO #%d : ", COUNT);
    printf(" (X%d Sai e X%d Entra)\n", LB[COUNT], EB[COUNT]);
    COUNT++;
    printf(" BV");
    for (J = 1; J <= NV; J++) {
        printf("%8s", "X");
        printf("%d ", J);
    }
    printf("%9s", " CV");
    printf("\n");
    for (J = 1; J <= NV + 1; J++) {
        printf("-----------");
    }
    printf("\n");

    for (I = 1; I <= NC; I++) {
        printf(" X%d", BASIC[I]);
        for (J = 1; J <= NV; J++) {
            printf("%9.2lf ", A[I][J]);
        }
        printf("%9.2lf\n", b[I][1]);
    }

    printf(" -Z");
    for (J = 1; J <= NV; J++) {
        printf("%9.2lf ", C[J]);
    }
    printf("%9.2lf\n", Z);
}

void solucaoOtima() {
    int I, J;
    OPTIMAL = 0;
    bool SUBEXIT[MAX_VARIABLES], EXIT = 1; // Controle para verificar se C[] é positivo ou zero
    for (J = 1; J <= NV; J++) { // Se todos os C[] são positivos ou zero, encontramos a solução ótima
        SUBEXIT[J] = 0;
        if (C[J] >= 0.0)
            SUBEXIT[J] = 1;
        else
            SUBEXIT[J] = 0;
        EXIT = EXIT && SUBEXIT[J];
    }
    if (EXIT)
        OPTIMAL = 1;
    if (LB[COUNT - 2] == EB[COUNT - 1] && LB[COUNT - 1] == EB[COUNT - 2])
        OPTIMAL = -1;
    // Se a variável de entrada e a variável de saída aparecerem alternadamente e indefinidamente, então o valor objetivo é ilimitado.
}

void bfsInicial() {
    int J, K;
    K = 1;
    // A BFS inicial são as últimas NC variáveis (ou seja, variáveis artificiais), pois adicionamos uma variável artificial para cada restrição
    for (J = NV - NC + 1; J <= NV; J++) {
        BASIC[K] = J;
        K++;
    }
}

int find(int ARR[], int LENGTH, int SEEK) {
    for (int I = 1; I <= LENGTH; I++) {
        if (ARR[I] == SEEK) return I;
    }
    return -1;
}

void primeiraFase() {
    int I, J, K;
    double PivotChange; // PivotChange: o coeficiente do elemento pivot
    printf("\n###################################################################");
    printf("\n######             SIMPLEX DE DUAS FASES: FASE I             ######");
    printf("\n###################################################################\n");
    COUNT = 1; // Inicializa a contagem de iterações no método simplex, começando por 1
    OPTIMAL = 0; // O estado de encontrar ÓTIMO é falso.
    STATE = 1; // Esta é a PRIMEIRA fase

    for (J = 1; J <= NV; J++) { // Usa um array temporário para registrar o C[] processado
        CTemp[J] = C[J];
    }

    // Os dois loops a seguir fazem os coeficientes das variáveis artificiais serem UM, enquanto outros são ZERO
    // Minimiza a soma das variáveis artificiais
    for (J = 1; J <= NV - NC; J++) {
        C[J] = 0;
    }
    for (J = NV - NC + 1; J <= NV; J++) {
        C[J] = 1;
    }

    // As variáveis artificiais devem ser substituídas na linha r. VEJA a Seção 3.4.2 em (Taha, 2014)
    for (J = 1; J <= NV; J++) {
        for (I = 1; I <= NC; I++) {
            C[J] = C[J] - A[I][J];
        }
    }
    for (I = 1; I <= NC; I++) {
        Z = Z - b[I][1];
    }

    bfsInicial(); 
    simplex(); 

    if (fabs(Z) > EPSILON) { // Se o ótimo for maior que 0 na Fase I, não há solução viável
        printf("OPA! Sem Solução Viável!\n");
        exit(EXIT_FAILURE);
    } else {
        for (I = 1; I <= NC; I++) { // Verifica as variáveis artificiais no nível zero no final da Fase I
            if (BASIC[I] > NV - NC) {
                LB[COUNT] = BASIC[I];
                for (J = 1; J <= NV - NC + 1; J++) {
                    if (A[I][J] != 0 && find(BASIC, NC, J)) {
                        EB[COUNT] = J;
                        BASIC[I] = J;
                        PivotChange = A[I][J];
                        PI = I;
                        PJ = J;
                    }
                }
                for (J = 1; J <= NV - NC; J++) {
                    A[I][J] = A[I][J] / PivotChange;
                }
                printf("\nComo as variáveis artificiais X%d estão no nível ZERO,\n", LB[COUNT]);
                printf("elas devem ser substituídas por uma variável não básica com um coeficiente não nulo.\n");
                operacaoLinha();
                printf("\n");
            }
        }
        printf("\nAs variáveis artificiais concluíram sua missão, agora passamos para a Fase II.\n");
    }
    NV = NV - NC; // Descarta as variáveis artificiais
}

void segundaFase(){
    int I, J, K;
    printf("\n###################################################################");
    printf("\n######             SIMPLEX DE DUAS FASES: FASE II            ######");
    printf("\n###################################################################\n");
    COUNT = 1; // Inicializa a contagem de iterações no método simplex a partir de 1
    OPTIMAL = 0; // O estado de encontrar OPTIMAL é falso.
    STATE = 2; // Esta é a SEGUNDA fase

    for (J = 1; J <= NV; J++) { // Restaura a função objetivo original
        C[J] = -1 * CTemp[J];
    }

    // Os loops seguintes são usados para substituir as variáveis básicas por coeficientes não nulos na linha z
    for (I = 1; I <= NC; I++) {
        for (J = 1; J <= NV; J++) {
            ATemp[I][J] = A[I][J];
        }
    }
    for (I = 1; I <= NC; I++) {
        for (J = 1; J <= NV; J++) {
            if (CTemp[BASIC[I]] == 0) goto e00;
            ATemp[I][J] = CTemp[BASIC[I]] * A[I][J];
            e00: ;
        }
    }
    for (I = 1; I <= NC; I++) {
        Z = Z - CTemp[BASIC[I]] * b[I][1];
    }
    for (J = 1; J <= NV; J++) {
        for (I = 1; I <= NC; I++) {
            if (CTemp[BASIC[I]] == 0) goto e10;
            C[J] = C[J] + ATemp[I][J];
            e10: ;
        }
    }

    simplex();
}

void resultado() {
    int I, J, K;
    double OX[NV0]; // OX: os valores finais restaurados para as variáveis originais X

    for (J = 1; J <= NV0; J++) {
        OX[J] = 0;
        for (I = 1; I <= NC; I++) {
            if (BASIC[I] == J) {
                OX[J] = b[I][1];
            }
        }
    }
    if (NVNP > 0) {
        for (J = 1; J <= NV0; J++) {
            for (K = 1; K <= NVNP; K++) {
                if (NP[K] == J)
                    OX[J] = -b[NP[K - 1]][1];
            }
        }
    }
    if (NVUR > 0) {
        for (J = 1; J <= NV0; J++) {
            for (K = 1; K <= NVUR; K++) {
                if (UR[K] == J)
                    OX[J] = OX[J] - b[find(BASIC, NC, UR[K] + NVNP + NV0 - 1)][1];
            }
        }
    }
    for (J = 1; J <= NV0; J++) {
        if (OX[J] == -0) OX[J] = 0;
    }

    printf("\n****** As variáveis ótimas do seu problema original são:");
    for (J = 1; J <= NV0; J++) {
        printf("\n       | X%d %f", J, OX[J]);
    }
    printf("\n****** O ótimo do seu problema original é: %f\n", (-1) * CSIGN * Z);
    printf("\n");
}

int main() {
    defineObjetivo();
    defineVariaveis();
    defineRestricoes();
    formaPadrao();
    NV0 = NV;
    primeiraFase();
    segundaFase();
    resultado();
    return 0;
}