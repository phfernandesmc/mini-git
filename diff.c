#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diff.h"

/* A tabela (m+1) x (n+1) fica num vetor so; DP(i, j) acessa a posicao. */
#define DP(i, j) dp[(size_t)(i) * (size_t)(n + 1) + (size_t)(j)]

void diff_arquivos(const Arquivo *antigo, const Arquivo *novo) {
    int m = antigo ? antigo->n : 0;
    int n = novo   ? novo->n   : 0;
    char **A = antigo ? antigo->linhas : NULL;
    char **B = novo   ? novo->linhas   : NULL;

    int *dp = calloc((size_t)(m + 1) * (size_t)(n + 1), sizeof(int));

    /* DP(i, j) = tamanho da LCS entre os sufixos A[i..m-1] e B[j..n-1].
     * Preenchemos de tras para frente; a linha m e a coluna n valem 0
     * (sufixo vazio). Usar sufixos permite andar a tabela do inicio
     * para o fim na reconstrucao, imprimindo o diff ja na ordem certa. */
    for (int i = m - 1; i >= 0; i--) {
        for (int j = n - 1; j >= 0; j--) {
            if (strcmp(A[i], B[j]) == 0)
                DP(i, j) = DP(i + 1, j + 1) + 1;
            else
                DP(i, j) = DP(i + 1, j) > DP(i, j + 1) ? DP(i + 1, j) : DP(i, j + 1);
        }
    }

    /* Reconstrucao: segue o caminho que gerou o maximo. */
    int i = 0, j = 0;
    while (i < m && j < n) {
        if (strcmp(A[i], B[j]) == 0) {           /* linha comum: mantida */
            printf("  %s\n", A[i]);
            i++; j++;
        } else if (DP(i + 1, j) >= DP(i, j + 1)) { /* pular A[i]: removida */
            printf("- %s\n", A[i]);
            i++;
        } else {                                  /* pular B[j]: adicionada */
            printf("+ %s\n", B[j]);
            j++;
        }
    }
    while (i < m) printf("- %s\n", A[i++]);
    while (j < n) printf("+ %s\n", B[j++]);

    free(dp);
}

static int arquivos_iguais(const Arquivo *a, const Arquivo *b) {
    if (a->n != b->n) return 0;
    for (int i = 0; i < a->n; i++)
        if (strcmp(a->linhas[i], b->linhas[i]) != 0) return 0;
    return 1;
}

void diff_commits(const Commit *a, const Commit *b) {
    int houve_mudanca = 0;

    /* Arquivos presentes no commit b: modificados ou novos. */
    for (int k = 0; k < b->n_arquivos; k++) {
        const Arquivo *novo = b->arquivos[k];
        const Arquivo *velho = commit_buscar_arquivo(a, novo->nome);
        if (velho && arquivos_iguais(velho, novo)) continue;
        printf("=== %s%s ===\n", novo->nome, velho ? "" : " (novo)");
        diff_arquivos(velho, novo);
        houve_mudanca = 1;
    }

    /* Arquivos que existiam em a e sumiram em b. */
    for (int k = 0; k < a->n_arquivos; k++) {
        const Arquivo *velho = a->arquivos[k];
        if (commit_buscar_arquivo(b, velho->nome)) continue;
        printf("=== %s (removido) ===\n", velho->nome);
        diff_arquivos(velho, NULL);
        houve_mudanca = 1;
    }

    if (!houve_mudanca) puts("(nenhuma diferenca)");
}
