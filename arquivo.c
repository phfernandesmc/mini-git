#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

/* Linhas maiores que isso sao quebradas em mais de uma (limitacao aceitavel). */
#define MAX_LINHA 1024

char *duplicar_texto(const char *s) {
    size_t tam = strlen(s) + 1;
    char *copia = malloc(tam);
    if (copia) memcpy(copia, s, tam);
    return copia;
}

/* Vetor dinamico: dobra a capacidade quando enche. */
static void adicionar_linha(Arquivo *a, int *capacidade, const char *texto) {
    if (a->n == *capacidade) {
        *capacidade = (*capacidade == 0) ? 8 : *capacidade * 2;
        a->linhas = realloc(a->linhas, (size_t)*capacidade * sizeof(char *));
    }
    a->linhas[a->n++] = duplicar_texto(texto);
}

Arquivo *arquivo_ler(const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) return NULL;

    Arquivo *a = malloc(sizeof *a);
    a->nome = duplicar_texto(caminho);
    a->linhas = NULL;
    a->n = 0;
    int capacidade = 0;

    char buffer[MAX_LINHA];
    while (fgets(buffer, sizeof buffer, f)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';   /* tira a quebra de linha */
        adicionar_linha(a, &capacidade, buffer);
    }
    fclose(f);
    return a;
}

Arquivo *arquivo_copiar(const Arquivo *origem) {
    Arquivo *a = malloc(sizeof *a);
    a->nome = duplicar_texto(origem->nome);
    a->n = origem->n;
    a->linhas = malloc((size_t)(a->n > 0 ? a->n : 1) * sizeof(char *));
    for (int i = 0; i < a->n; i++)
        a->linhas[i] = duplicar_texto(origem->linhas[i]);
    return a;
}

void arquivo_liberar(Arquivo *a) {
    if (!a) return;
    for (int i = 0; i < a->n; i++) free(a->linhas[i]);
    free(a->linhas);
    free(a->nome);
    free(a);
}
