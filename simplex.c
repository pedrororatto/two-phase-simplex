#include <stdio.h>
#include <stdlib.h>

#define MAX_VAR 100
#define MAX_COEFICIENTES 100

void funcaoObjetivo() {
    //variaveis e coeficientes da função objetivo
    int numVar, coeficientes[MAX_COEFICIENTES]; 
    //Variaveis e coeficientes das restrições
    int numRestricoes, restricoes[MAX_VAR][MAX_COEFICIENTES];
    //Tipo de inequação das restrições
    int tipoInequacao;
    int varArtificiais[MAX_VAR];
    int min = -1;

    //Função objetivo inicial
    printf("Digite o número de variáveis da função objetivo: ");
    scanf("%d", &numVar);
    for (int i = 0; i < numVar; i++)
    {
        printf("Digite o coeficiente da variável x%d: ", i+1);
        scanf("%d", &coeficientes[i]);
        coeficientes[i] *= min;
    }

    //Restrições
    printf("\n\nDigite o número de restrições: ");
    scanf("%d", &numRestricoes);
    for (int i = 0; i < numRestricoes; i++)
    {
        printf("\nRestrição %d: \n", i+1);
        for (int j = 0; j < numVar; j++)
        {
            printf("Digite o coeficiente da variável x%d: ", j+1);
            scanf("%d", &restricoes[i][j]);
        }
        printf("Digite o valor do termo independente: ");
        scanf("%d", &restricoes[i][numVar]);

        printf("Digite o tipo de inequação: \n\t1 - <=\n\t2 - >=\n\t3 - =\n");
        scanf("%d", &tipoInequacao);
        switch (tipoInequacao)
        {
        case 1:
            printf("<=\n");
            break;
        case 2:
            printf(">=\n");
            break;
        case 3:
            printf("=\n");
            break;
        default:
            printf("Opção inválida\n");
            break;
        }

    }

    /*
    //Imprimir restrições
    printf("\n\tSujeito a: \n");
    for (int i = 0; i < numRestricoes; i++)
    {
        printf("\t");
        for (int j = 0; j < numVar; j++)
        {
            printf(" %dx%d", restricoes[i][j], j+1);
            if (j < numVar-1)
            {
                printf(" +");
            }
        }
        if (tipoInequacao[i] == 1)
        {
            printf(" <= %d\n", restricoes[i][numVar]);
        }
        else if (tipoInequacao[i] == 2)
        {
            printf(" >= %d\n", restricoes[i][numVar]);
        }
        else if (tipoInequacao[i] == 3)
        {
            printf(" = %d\n", restricoes[i][numVar]);
        }
    }
    
    //Função objetivo final
    printf("\nFunção objetivo: \n\n\t MAX Z = ");
    for (int i = 0; i < numVar; i++)
    {
        printf(" %dx%d", coeficientes[i], i+1);
        if (i < numVar-1)
        {
            printf(" +");
        }
    }
    printf("\n\t Sujeito a: \n");
    for (int i = 0; i < numRestricoes; i++)
    {
        printf("\t");
        for (int j = 0; j < numVar; j++)
        {
            printf(" %dx%d", restricoes[i][j], j+1);
            if (j < numVar-1)
            {
                printf(" +");
            }
        }
        if (tipoInequacao[i] == 1)
        {
            printf(" <= %d\n", restricoes[i][numVar]);
        }
        else if (tipoInequacao[i] == 2)
        {
            printf(" >= %d\n", restricoes[i][numVar]);
        }
        else if (tipoInequacao[i] == 3)
        {
            printf(" = %d\n", restricoes[i][numVar]);
        }
    }
    */
    

    
    
    printf("\n");   
}

void formaPadrao(){

}

int main() {
    
    funcaoObjetivo();

    return 0;
}